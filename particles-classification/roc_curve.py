#!/usr/bin/python
# -*- coding: utf-8 -*-

import pandas as pd
import sys
import matplotlib.pyplot as plt
from root_pandas import read_root
from sklearn import datasets
from sklearn.metrics import roc_curve, auc

#set up using better text handlers
plt.figure()
plt.rc('text', usetex=True)
plt.rc('text.latex', unicode=True)

def choose_signal(value, signal):
	if abs(value) == signal:
		return 1
	else :
		return 0

#check if arguments are valid
if len(sys.argv) != 2 :
	print "Wrong usage! ./plot-curve.py <file_name>"
	sys.exit(0)

#prepare DataFrame file
predicted = read_root(sys.argv[1])

pids = [ 11, 13, 211, 321, 2212]
lw = 2

#count ROC curve values for each decision class inside DataFrame file
j = 0
for pid in pids:
	predicted['CurrCode'] = predicted.apply(lambda row: choose_signal(row['PDGCode'],pid), axis=1)
	fpr, tpr, _ = roc_curve(predicted['CurrCode'], predicted['predicted%d'%j])
	roc_auc = auc(fpr, tpr)
	plt.plot(fpr, tpr, lw=lw,
			 label='Krzywa ROC klasy %d (pole = %0.6f)' % (pid,roc_auc))
	j += 1

#set up rest of the plot
plt.plot([0, 1], [0, 1], color='navy', lw=lw, linestyle='--')
plt.xlim([0.0, 1.0])
plt.xlim([0.0, 1.05])
plt.xlabel(u'1 - specyficzność', fontsize=12)
plt.ylabel(u'Czułość', fontsize=12)
plt.title(u'Krzywa ROC')
plt.legend(loc="lower right")
plt.show()

