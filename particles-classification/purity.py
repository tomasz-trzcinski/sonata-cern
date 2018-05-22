#!/usr/bin/python
# -*- coding: utf-8 -*-
# script for computing purity of a predicted sample
# run with no arguments to see usage


from root_pandas import read_root
import pandas as pd
import numpy as np
import math

import matplotlib.pyplot as plt
import matplotlib.ticker
import sys


pd.options.mode.chained_assignment = None
allowed_tags = [ '11', '13', '211', '321', '2212', 'isPion', 'isProton', 'isKaon' ]

def choose_signal(value, signal):
	if abs(value) == signal:
		return 1
	else :
		return 0

def isClassifierTag(arg):
	classifier_args = [ '11', '13', '211', '321', '2212' ]
	if arg in classifier_args:
		return True
	else:
		return False

def printUsage():
	print "Usage: ./purity.py <file_name> <pid_labels>..."
	print "Allowed labels:"
	print allowed_tags

if len(sys.argv) < 3 :
	printUsage()
	sys.exit(0)

args_list = sys.argv[2:]
for arg in args_list :
	if arg not in allowed_tags :
		printUsage()
		sys.exit(0)

input_file = sys.argv[1]
predicted = read_root(input_file)

scores_list = []
errors_list = []
particles_list = []
threshold_list = []

for i in xrange(0, len(args_list)):
	scores_list.append([])
	errors_list.append([])
	particles_list.append([])

labels_pid = {
	'11' : 11,
	'13' : 13,
	'211': 211,
	'321': 321,
	'2212' : 2212,
	'isPion' : 211,
	'isKaon' : 321,
	'isProton' : 2212
}

#for every label prepare plot of purity values for a given range of momentum
prev = 0
for i in np.logspace(-1, 1, dtype='float64'):
	counter = -1
	curr_df = predicted[(predicted["Pt"]>=prev) & (predicted["Pt"]<i)]
	#no particles in momentum range - continue
	if len(curr_df) == 0 :
		prev = i
		continue

	#for every label count purity value for single momentum range
	threshold_list.append(i)
	for arg in args_list:
		counter += 1
		#prepare the slice of DataFrame
		pid = labels_pid[arg]
		curr_df['PIDCode'] = curr_df.apply(lambda row: (1 if row['PDGCode']==pid else 0), axis=1)
		if isClassifierTag( arg ):
			curr_df['decision'] = curr_df.apply(lambda row: 1 if row['predicted']==pid else 0, axis=1)
		else :
			curr_df['decision'] = curr_df[arg]

		all_particles = float( len(curr_df[ (curr_df['PIDCode']==1) ]) )
		all_data = float( len(curr_df) )
		all_chosen = float ( len(curr_df[ (curr_df['decision']==1) ]) )
		all_true = float( len(curr_df[ (curr_df['PIDCode'] == curr_df['decision']) & (curr_df['PIDCode']==1) ]) )
		err_all = math.sqrt( all_chosen )
		err_true = math.sqrt( all_true )

		#check if any particle was chosen at all
		if all_chosen == 0:
			#check if there was any particle to chose from
			if all_particles == 0 :
				scores_list[counter].append(1.0)
				errors_list[counter].append(0.0)
			else:
				scores_list[counter].append(0)
				errors_list[counter].append(0)
		else:
			scores_list[counter].append( all_true / all_chosen )
			errors_list[counter].append( math.sqrt( math.pow(err_true, 2)/math.pow(all_chosen, 2)
						+ math.pow(all_true, 2)*math.pow(err_all, 2)/math.pow(all_chosen, 4) ) )
		particles_list[counter].append( all_chosen )
	prev = i

#prepare legend for every plot
legends_list = []
args_legend = {
	'11' : u'Las Losowy elektrony ',
	'13' : u'Las Losowy muony ',
	'211' : u'Las Losowy piony ',
	'321' : u'Las Losowy kaony ',
	'2212' : u'Las Losowy protony ',
	'isProton' : u'Tradycyjny PID protony ',
	'isPion' : u'Tradycyjny PID piony ',
	'isKaon' : u'Tradycyjny PID kaony '
}

for i in xrange(0, len(args_list)) :
	legend = args_legend[args_list[i]]
	legend += str( sum(scores_list[i][j] * particles_list[i][j] for j in range(len(scores_list[i])) ) / sum(particles_list[i]) )
	legends_list.append(legend)
for i in xrange(0, len(legends_list)):
	plt.errorbar(threshold_list, scores_list[i], errors_list[i],
			  label=legends_list[i], fillstyle='none')

#set up rest of the plot
plt.rc('text', usetex=True)
plt.rc('text.latex', unicode=True)
plt.rc('font', **{'family':'serif', 'serif':['Times']})
plt.xscale("log")
ax = plt.gca()
ax.tick_params(axis = 'both', which = 'major', labelsize = 14)
ax.tick_params(axis = 'both', which = 'minor', labelsize = 10)
ax.get_xaxis().set_major_formatter(matplotlib.ticker.ScalarFormatter())
plt.legend(frameon=False, loc='lower left', fontsize=14)
plt.xlabel(r'${\mathit{p}}_{\rm T}$ (GeV/$\mathit{c}$)', fontsize=14)
plt.ylabel(u'Specyficzność', fontsize=14)
plt.ylim(0.0, 1.2)
plt.text(0.3, 0.4, r'ALICE Simulation '"\n"' pp $\sqrt{s}$ = 7 TeV '"\n"' PYTHIA 6.4 (Perugia-0 tune)', fontsize=14)
plt.title(u'Specyficzność', fontsize=14)
plt.show()
