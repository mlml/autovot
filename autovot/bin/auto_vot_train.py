#! /usr/bin/env python
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
# auto_vot_train.py: Train a classifier to automatically measure VOT,
# using manually annotated VOTs in a set of textgrids and
# corresponding wav files. Run with -h argument for information on
# arguments, and see documentation for usage examples.
#

import os
import shutil
import tempfile

from auto_vot_extract_features import *
from helpers.utilities import *


if __name__ == "__main__":
    # parse arguments
    parser = argparse.ArgumentParser(description='Train a classifier to automatically measure VOT, using manually '
                                                 'annotated VOTs in a set of textgrids and corresponding wav files.  '
                                                 'See documentation for usage examples.')
    parser.add_argument('wav_list', help="Text file listing  WAV files")
    parser.add_argument('textgrid_list', help="Text file listing corresponding manually labeled TextGrid files")
    parser.add_argument('model_filename', help="Name of classifiers (output)")
    parser.add_argument('--vot_tier', default='vot', help='Name of the tier to extract VOTs from (default: %(default)s)')
    parser.add_argument('--vot_mark', default='*', help='Only intervals on the vot_tier with this mark value (e.g. '
                                                        '"vot", "pos", "neg") are used for training, or "*" for any '
                                                        'string (this is the default)')
    parser.add_argument('--window_min', help='Left boundary of the window (in msec) relative to the VOT interval\'s '
                                             'right boundary. Usually should be negative, that is, before the VOT '
                                             'interval\'s left boundary. (default: %(default)s)', default=-50, type=float)
    parser.add_argument('--window_max', help='Right boundary of the window (in msec) relative to the VOT interval\'s '
                                             'right boundary. Usually should be positive, that is, after the VOT '
                                             'interval\'s right boundary. (default: %(default)s)', default=800, type=float)
    parser.add_argument('--cv_auto', help='Use 20%% of the training set for cross-validation (default: don\'t do this)',
                        action='store_true', default=False)
    parser.add_argument('--cv_wav_list', default='', help='Text file listing WAV files for cross-validation (default: none)')
    parser.add_argument('--cv_textgrid_list', default='', help='Text file listing corresponding manually labeled '
                                                               'TextGrid files for cross-validation (default: none)')
    parser.add_argument('--max_num_instances', default=0, type=int, help='Maximum number of instances per file to use '
                                                                          '(default: use everything)')
    parser.add_argument("--logging_level", help="Level of verbosity of information printed out by this program ("
                                                "DEBUG, INFO, WARNING or ERROR), in order of increasing verbosity. "
                                                "See http://docs.python.org/2/howto/logging for definitions. ("
                                                "default: %(default)s)", default="INFO")
    args = parser.parse_args()

    logging_defaults(args.logging_level)

    # intermediate files that will be used to represent the locations
    # of the VOTs, their windows and the location of the corresponding
    # feature files.  This is all kept in a working directory, which
    # is deleted at the end of the script.
    
    working_dir = tempfile.mkdtemp()
    features_dir = working_dir + "/features"
    basename = working_dir + "/training"
    os.makedirs(features_dir)
    input_filename = working_dir + "/training.input"
    features_filename = working_dir + "/training.feature_filelist"
    labels_filename = working_dir + "/training.labels"

    tier_definitions = TierDefinitions()
    args.window_min /= 1000.0   # convert msec to seconds
    args.window_max /= 1000.0   # convert msec to seconds
    tier_definitions.extract_definition(args)

    # Prepare configuration files for the front end (i.e., the acoustic feature extraction, which results in the feature files)
    problematic_files = textgrid2front_end(args.textgrid_list, args.wav_list, input_filename, features_filename,
                                           features_dir, tier_definitions)

    # call front end
    cmd_vot_front_end = 'VotFrontEnd2 -verbose %s %s %s %s' % (args.logging_level, input_filename, features_filename,
                                                               labels_filename)
    easy_call(cmd_vot_front_end)

    # Randomize training order of the examples.  We assume the user wants to do this, as it tends to result in better classifiers.
    features_filename_rs = features_filename + ".rs"
    labels_filename_rs = labels_filename + ".rs"
    num_instances = random_shuffle_data(features_filename, labels_filename, features_filename_rs, labels_filename_rs)

    ## Now carry out training and testing.  Three options:

    # Option 1: if --cv_auto flag given, hold out 20% of the data for cross-validation, use the remaining 80% for training
    if args.cv_textgrid_list == '' and args.cv_auto:
        logging.debug("Using 20% of the data for cross-validation")
        num_instances_for_training = int(num_instances * 0.80)
        instance_range_for_training = (0, num_instances_for_training - 1)
        instance_range_for_test = (num_instances_for_training, num_instances - 1)
        logging.debug("num_instances=%d" % num_instances)
        logging.debug("num_instances_for_training=%d" % num_instances_for_training)
        logging.debug("instance_range_for_training=%d %d" % instance_range_for_training)
        logging.debug("instance_range_for_test=%d %d" % instance_range_for_test)
        features_filename_training = features_filename_rs + ".train"
        extract_lines(features_filename_rs, features_filename_training, instance_range_for_training)
        labels_filename_training = labels_filename_rs + ".train"
        extract_lines(labels_filename_rs, labels_filename_training, instance_range_for_training, has_header=True)
        features_filename_test = features_filename_rs + ".test"
        extract_lines(features_filename_rs, features_filename_test, instance_range_for_test)
        labels_filename_test = labels_filename_rs + ".test"
        extract_lines(labels_filename_rs, labels_filename_test, instance_range_for_test, has_header=True)
        # Training
        cmd_vot_training = 'VotTrain -verbose %s -pos_only -vot_loss -epochs 2 -loss_eps 4 -min_vot_length 5 %s %s %s' % \
            (args.logging_level, features_filename_training, labels_filename_training, args.model_filename)
        easy_call(cmd_vot_training)
        # Testing
        cmd_vot_decode = 'VotDecode -verbose %s %s %s %s' % (args.logging_level, features_filename_test,
            labels_filename_test, args.model_filename)
        easy_call(cmd_vot_decode)

    ## Option 2: Otherwise, if the user specified data to be used for cross-validation, use that.
    elif args.cv_textgrid_list != '':
        logging.debug("Using user data for cross-validation")
        features_filename_training = features_filename_rs
        labels_filename_training = labels_filename_rs
        input_filename_test = working_dir + "/training.input.test"
        features_filename_test = working_dir + "/training.feature_filelist.test"
        labels_filename_test = working_dir + "/training.labels.test"
        # prepare configuration files for the front end (i.e., the acoustic feature extraction, which results in the feature files)
        problematic_files += textgrid2front_end(args.cv_textgrid_list, args.cv_wav_list, input_filename_test,
                                                features_filename_test, features_dir, tier_definitions)
        # call front end
        cmd_vot_front_end = 'VotFrontEnd2 -verbose %s %s %s %s' % (args.logging_level, input_filename_test,
            features_filename_test, labels_filename_test)
        easy_call(cmd_vot_front_end)
        # Training
        cmd_vot_training = 'VotTrain -verbose %s -pos_only -vot_loss -epochs 2 -loss_eps 4 -min_vot_length 5 ' \
                           '- C 50 %s %s %s' % (args.logging_level, features_filename_training,
                                                labels_filename_training, args.model_filename)
        easy_call(cmd_vot_training)
        # Test
        cmd_vot_decode = 'VotDecode -verbose %s %s %s %s' % (args.logging_level, features_filename_test,
            labels_filename_test, args.model_filename)
        easy_call(cmd_vot_decode)

    ## Option 3: Otherwise, use all data for training, and don't do any cross-validation.
    else:
        features_filename_training = features_filename_rs
        labels_filename_training = labels_filename_rs
        # Training
        cmd_vot_training = 'VotTrain -verbose %s -pos_only -vot_loss -epochs 2 -loss_eps 4 -min_vot_length 5 ' \
                           '-C 50 %s %s %s' % (args.logging_level, features_filename_training,
                                               labels_filename_training, args.model_filename)
        easy_call(cmd_vot_training)

    # remove working directory and its content
    if args.logging_level != "DEBUG":
        shutil.rmtree(path=working_dir, ignore_errors=True)

    if len(problematic_files):
        logging.warning("Classifiers trained using all files except these ones, where something was wrong:")
        logging.warning(problematic_files)
        logging.warning("Look for lines beginning with WARNING or ERROR in the program's output to see what went "
                        "wrong.")

    logging.info("All done.")
