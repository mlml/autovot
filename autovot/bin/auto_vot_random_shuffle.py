##! /usr/bin/env python
#
# Copyright (c) 2014 Joseph Keshet, Morgan Sonderegger, Thea Knowles
#
# This file is part of Autovot, a package for automatic extraction of
# voice onset time (VOT) from audio files.
#
# Autovot is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# Autovot is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with Autovot.  If not, see
# <http://www.gnu.org/licenses/>.
#
# auto_vot_random_shuffle.py: Random shuffle a set of features and labels.
#

import argparse
import random

if __name__ == "__main__":

    # command line arguments
    parser = argparse.ArgumentParser("Random shuffle a set of features and labels")
    parser.add_argument("vot_fe_features_in")
    parser.add_argument("vot_fe_labels_in")
    parser.add_argument("vot_fe_features_out")
    parser.add_argument("vot_fe_labels_out")
    args = parser.parse_args()

    with open(args.vot_fe_features_in, 'r') as f:
        features_in = [features.strip() for features in f]
        
    with open(args.vot_fe_labels_in, 'r') as f:
        labels_in = [labels.strip() for labels in f]
        # remove header
        header = labels_in.pop(0)

    if len(features_in) != len(labels_in):
        print "Error: the input files %s and %s are not of the same length." % (args.vot_fe_featueres_in, 
            args.vot_fe_labels_in)
        exit()
    
    instances = list()

    for features, labels in zip(features_in, labels_in):
        instances.append((features, labels))

    random.shuffle(instances)

    features_out = open(args.vot_fe_features_out, 'w')
    labels_out = open(args.vot_fe_labels_out, 'w')
    print >>labels_out, header
    for features, labels in instances:
        print >>features_out, features
        print >>labels_out, labels
    features_out.close()
    labels_out.close()