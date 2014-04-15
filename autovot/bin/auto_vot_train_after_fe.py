#! /usr/bin/env python
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
# auto_vot_train_after_fe.py : Train a classifier to automatically
# measure VOT, using manually annotated VOTs for which features have
# already been extracted using auto_vot_extract_features.py, resulting
# in a set of feature files and labels.
#


import argparse
from helpers.utilities import *


if __name__ == "__main__":
    # parse arguments
    parser = argparse.ArgumentParser(description='Train a classifier to automatically measure VOT, using manually '
                                                 'annotated VOTs for which features have already been extracted using '
                                                 'auto_vot_extract_features.py, resulting in a set of feature files and labels.')
    parser.add_argument('features_filename', help="AutoVOT front end features filename (training)")
    parser.add_argument('labels_filename', help="AutoVOT front end labels filename (training)")
    parser.add_argument('model_filename', help="Name of classifiers (output)")
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
    cmd_vot_training = 'VotTrain -verbose %s -pos_only -vot_loss -epochs 2 -loss_eps 4 -min_vot_length 5 -C ' \
                       '50 %s %s %s' % (args.logging_level, features_filename_rs, labels_filename_rs,
                                        args.model_filename)
    easy_call(cmd_vot_training)

