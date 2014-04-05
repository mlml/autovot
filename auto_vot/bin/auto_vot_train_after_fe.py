#! /usr/bin/env python

"""
auto_vot_train_after_fe.py

Train a classifier to automatically measure VOT, using manually
annotated VOTs for which features have already been extracted using
auto_vot_extract_features.py, resulting in a set of feature files and
labels.

Author: Joseph Keshet, 18/11/2013
"""

import argparse
from autovot.utilities import *


if __name__ == "__main__":
    # parse arguments
    parser = argparse.ArgumentParser(description='Train a classifier to automatically measure VOT, using manually '
                                                 'annotated VOTs for which features have already been extracted using '
                                                 'auto_vot_extract_features.py, resulting in a set of feature files and labels.')
    parser.add_argument('features_filename', help="AutoVOT front end features filename (training)")
    parser.add_argument('labels_filename', help="AutoVOT front end labels filename (training)")
    parser.add_argument('model_filename', help="Name of classifiers which are outputted")
    parser.add_argument("--logging_level", help="Level of verbosity of information printed out by this program ("
                                                "DEBUG, INFO, WARNING or ERROR), in order of increasing verbosity. "
                                                "See http://docs.python.org/2/howto/logging for definitions. ("
                                                "default: %(default)s)", default="INFO")
    args = parser.parse_args()

    logging_defaults(args.logging_level)

    # random file lines
    features_filename_rs = args.features_filename + ".rs"
    labels_filename_rs = args.labels_filename + ".rs"
    random_shuffle_data(args.features_filename, args.labels_filename, features_filename_rs, labels_filename_rs)

    # Training
    cmd_vot_training = 'InitialVotTrain -verbose %s -pos_only -vot_loss -epochs 2 -loss_eps 4 -min_vot_length 5 -C ' \
                       '50 %s %s %s' % (args.logging_level, features_filename_rs, labels_filename_rs,
                                        args.model_filename)
    easy_call(cmd_vot_training)

