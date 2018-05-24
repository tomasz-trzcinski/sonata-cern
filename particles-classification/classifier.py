#!/usr/bin/python
import sys, getopt
import math
from math import *

from sklearn import preprocessing
from sklearn.ensemble import RandomForestClassifier
from sklearn.externals import joblib
import pandas as pd
from root_pandas import read_root


VERBOSE = False

#simple logger function
def logger(text):
	if VERBOSE == True:
		print text
	else :
		print "none"

class Classifier:
	def __init__(self):
		self.clf = RandomForestClassifier(n_estimators=75, n_jobs=-1, random_state=11, oob_score=True)
		#list containing all features that CAN be used to classify
		self.features = [u'TPCNcls', u'TPCsignal',
				u'ped', u'Pt', u'Px', u'Py', u'Pz',
	   			u'nSigmaTOFPi', u'nSigmaTOFK', u'nSigmaTOFP', u'nSigmaTOFe',
	   			u'nSigmaTPCPi', u'nSigmaTPCK', u'nSigmaTPCP', u'nSigmaTPCe',
	   			u'tbeta', u'cov0', u'cov1', u'cov2', u'cov3', u'cov4', u'cov5',
	   			u'cov6', u'cov7', u'cov8', u'cov9', u'cov10', u'cov11',
	   			u'cov12', u'cov13', u'cov14', u'cov15', u'cov16', u'cov17',
	   			u'cov18', u'cov19', u'cov20']
		#list of features that are obsolete to classification
		self.ignore_list = [u'cov0', u'cov1', u'cov3', u'cov4',
				u'cov6', u'cov7', u'cov8', u'cov10',
	   			u'cov11', u'cov12', u'cov15', u'cov16']
		#name of branch containing classifier's decision
		self.signal_label = 'PDGCode'
		#data scaler (optional)
		self.min_max_scaler = preprocessing.MinMaxScaler()
		#minimal raise in classifier performance
		self.min_improve = 0.0005

	#change number of tree estimators in Random Forest
	def set_estimators(self, n):
		self.clf = RandomForestClassifier(n_estimators=n,
				n_jobs=-1, random_state=11, oob_score=True)

	#change minimal raise in classifier performance
	def set_min_improv(self, min):
		self.min_improve = min

	#clear ignore list so that all attributes are used for classification
	def clear_ignore_list(self):
		self.ignore_list = []

	#returns normalized values by provided scaler
	def scale_values(self, X):
		x = X.values
		x_scaled = self.min_max_scaler.fit_transform(x)
		return  pd.DataFrame(x_scaled, columns=X.columns)

	#tune classifier hyper-parameters
	def tune_classifier(self, X, y, valid_features):
		logger("Tuning classifier parameters")
		#list of parameters to be tuned and their possible values
		tuned_features = [ 'n_estimators', 'max_depth', 'max_features' ]
		param_grid = {
			'n_estimators' : [50, 75, 100, 150, 200, 400],
			'max_depth' : [ None, 5, 10, 20, 30, 40 ],
			'max_features' : [ len(valid_features), len(valid_features)/2, 
				  int(math.sqrt(len(valid_features))), int(math.log(len(valid_features), 2))]
		}
		if VERBOSE:
			print "Tuning from:"
			print param_grid
		#current attributes of starting classifier
		current_grid = {
			'n_estimators' : 1,
			'max_depth' : None,
			'max_features' : int(math.sqrt(len(valid_features))) #auto
		}
		#current best classifier score
		max_cv = 0.0
		#map holding parameters of best classifier
		best_grid = {}
		#for every tunned parameter find best classifier and save it to best_grid
		#by comparing its oob cross-validatoin score
		for feature in tuned_features:
			for i in param_grid[feature]:
				current_grid[feature] = i
				test_clf = RandomForestClassifier(n_estimators=current_grid['n_estimators'],
					max_features=current_grid['max_features'],
	 				max_depth=current_grid['max_depth'],
					n_jobs=-1, random_state=11, oob_score=True)
				test_clf.fit(X, y)
				if ( test_clf.oob_score_ > (max_cv + min_improve)) :
					self.clf = test_clf
					max_cv = test_clf.oob_score_
					best_grid[feature] = current_grid[feature]
			current_grid[feature] = best_grid[feature]

		if VERBOSE:
			print "Best features found: "
			print best_grid

	#prepare classifier for discrimination task
	def fit(self, file='ttree.root', tune=False, scale=False):
		logger("Read data from %s" % file)
		train = read_root(file)
		#list containing features used in current analysis
		valid_features = [ x for x in self.features if x not in self.ignore_list]

		#prepare training dataset with X holding
		#observations and y holding decisions
		y = train[self.signal_label]
		X = train[valid_features]
		if scale :
			X = self.scale_values(X)

		if tune :
			self.tune_classifier(X, y, valid_features)
		else :
			logger("Training classifier on data")
			self.clf.fit(X, y)

		if VERBOSE:
			print "Classifier OOB score : %f" % self.clf.oob_score_

	#predict particle type
	def predict(self, input_file, root_file, tree_key='predicted', scale=False):
		logger("Predicting particles PID from file: %s" % input_file)
		pred = read_root(input_file).dropna(axis=0)
		#list containing features used in current analysis
		valid_features = [ x for x in self.features if x not in self.ignore_list]

		#prepare observations
		X = pred[valid_features]
		if scale:
			X = self.scale_values(X)

		#save all classifier decisions
		#adding chosen class to "predicted" branch
		#and probabilities to branches "predicted0..4"
		probs = self.clf.predict_proba(X)
		for i in xrange(len(probs[0])):
			pred_name = 'predicted%d' % i
			pred[pred_name] = probs[:,i]
		pred['predicted'] = self.clf.predict(X)

		#save resulting DataFrame with added branches
		logger("Saving results to: %s" % root_file)
		pred.to_root(root_file, tree_key)

	#save classifier to given file
	def save_clf(self, file='clf.pkl'):
		logger("Saving classifier to %s" % file)
		joblib.dump(self.clf, file)

	#load classifier from given file
	def load_clf(self, file='clf.pkl'):
		logger("Loading classifier from %s" % file)
		self.clf = joblib.load(file)

	#save scaler to given file
	def save_scaler(self, file='scaler.pkl'):
		logger("Saving scaler to %s" % file)
		joblib.dump(self.min_max_scaler, file)

	#load scaler from given file
	def load_scaler(self, file='scaler.pkl'):
		logger("Loading scaler from %s" % file)
		self.min_max_scaler = joblib.load(file)

def print_help():
	print "./pid-classifier.py -hfgtva --input <file> --root <file> --classifier <file> --normalize <file> --estimators <n> --min_improv <value>"
	print "-h, --help                 print help message"
	print "-i, --input <file>         input root file"
	print "-r, --root <file>          output root file"
	print "-c, --classifier <file>    input classifier file"
	print "-n, --normalize <file>     intput/output scaler file, disabled if none"
	print "-f, --fit                  train classifier on given data"
	print "-g, --guess                predict TTree tracks particle ID"
	print "-t, --tune 				  tune in hyperparameters"
	print "-v, --verbose              print debugging messages"
	print "-e, --estimators <n>		  set numbers of estimators in forest"
	print "-m, --min_improv <value>	  minimal improvement for CV score in tuning"
	print "-a, --all 				  use all features to train/predict"

def main(argv):
	try:
		opts, args = getopt.getopt(argv, "hi:r:c:fp:gtvn:e:m:a",
							 ["--help", "--input=", "--root=",
		 					  "--classifier=", "--fit", "--pid=",
		 					  "--guess", "--tune", "--verbose", "--normalize=",
		 					  "--estimators=", "--min_improv=", "--all"])
	except getopt.GetoptError:
		print_help()
		sys.exit(2)

	input_file = 'ttree.root'
	root_file = 'predicted.root'
	clf_file = 'clf.pkl'
	tree_key = 'predicted'
	scaler_file = None
	scale = False
	pid = 0
	tune = False
	train = False
	predict = False
	classifier = Classifier()
	for opt, arg in opts:
		if opt == '-h':
			print_help()
			sys.exit(0)
		if opt in ("-e", "--estimators"):
			n_estimators = int(arg)
			classifier.set_estimators(n_estimators)
		if opt in ("-m", "--min_improv"):
			min_improv = float(arg)
			classifier.set_min_improv(min_improv)
		if opt in ("-i", "--input"):
			input_file = str(arg)
			print input_file
		if opt in ("-v", "--verbose"):
			global VERBOSE
			VERBOSE = True
		if opt in ("-r", "--root"):
			root_file = arg
		if opt in ("-c", "--classifier"):
			clf_file = arg
		if opt in ("-f", "--fit"):
			train = True
		if opt in ("-t", "--tune"):
			tune = True
		if opt in ("-g", "--guess"):
			predict = True
		if opt in ("-n", "--normalize"):
			scale = True
			scaler_file = arg
		if opt in ("-a", "--all"):
			classifier.clear_ignore_list()


	if predict == train :
		print "You can only either train or predict data. Exiting..."
		sys.exit(1)
	#prepare classifier and save it
	if train :
		classifier.fit(input_file, tune, scale)
		classifier.save_clf(clf_file)
		if scale:
			classifier.save_scaler(scaler_file)
	#predict particle types based on existing classifier
	elif predict:
		classifier.load_clf(clf_file)
		if scale:
			classifier.load_scaler(scaler_file)
		classifier.predict(input_file, root_file, tree_key, scale)



if __name__ == "__main__" :
	main(sys.argv[1:])
