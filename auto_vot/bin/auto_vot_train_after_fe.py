#! /usr/bin/env python

"""
auto_vot_train_after_fe.py
Author: Joseph Keshet, 18/11/2013
"""

import argparse
from autovot.utilities import *


if __name__ == "__main__":
    # parse arguments
    parser = argparse.ArgumentParser(description='Train AutoVOT after features extraction')
    parser.add_argument('features_filename', help="AutoVOT front end features file name (training)")
    parser.add_argument('labels_filename', help="AutoVOT front end labels file name (training)")
    parser.add_argument('model_filename', help="output model file name")
    parser.add_argument("--logging_level", help="print out level (DEBUG, INFO, WARNING or ERROR)", default="INFO")
    args = parser.parse_args()

    logging_defaults(args.logging_level)

    # random file lines
    features_filename_rs = args.features_filename + ".rs"
    labels_filename_rs = args.labels_filename + ".rs"
    random_shuffle_data(args.features_filename, args.labels_filename, features_filename_rs, labels_filename_rs)

    # Training
    cmd_vot_training = 'InitialVotTrain -verbose %s -pos_only -vot_loss -epochs 1 -loss_eps 4 -min_vot_length 5 -C ' \
                       '50 %s %s %s' % (args.logging_level, features_filename_rs, labels_filename_rs,
                                        args.model_filename)
    easy_call(cmd_vot_training)

