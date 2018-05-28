#!/bin/bash

aliroot -q -l testBatch.C

aliroot -q -l 'runBatch.C(0, 150, "train.root")'
aliroot -q -l 'runBatch.C(150, 950, "test.root")'
classifier.py -i "train.root" -f
classifier.py -i "test.root" -g

roc_curve.py "predicted.root"
efficiency.py "predicted.root" 11 13 211 321 2212
purity.py "predicted.root" 11 13 211 321 2212

