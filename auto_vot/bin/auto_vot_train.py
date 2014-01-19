#! /usr/bin/env python

"""
auto_vot_train.py
Author: Joseph Keshet, 18/11/2013
"""

import shutil
import tempfile
from auto_vot_extract_features import *
from utilities import *


if __name__ == "__main__":
    # parse arguments
    parser = argparse.ArgumentParser(description='Training of AutoVOT')
    parser.add_argument('textgrid_list', help="list of manually labeled TextGrid files")
    parser.add_argument('wav_list', help="list of WAV files")
    parser.add_argument('model_filename', help="output model file name")
    parser.add_argument('--vot_tier', default='vot', help='name of the tier to extract VOTs from')
    parser.add_argument('--vot_mark', default='*', help='VOT mark value (e.g., "pos", "neg") or "*" for any string')
    parser.add_argument('--window_tier', default='', help='used this window as a search window for training. If not '
                                                          'given, a constant window with parameters [window_min, '
                                                          'window_max] around the manually labeled VOT will be used')
    parser.add_argument('--window_mark', default='', help='window mark value or "*" for any string')
    parser.add_argument('--window_min', default=-0.05, type=float, help='window left boundary (in msec) relative to '
                                                                        'the VOT right boundary (usually should be '
                                                                        'negative, that is, before the VOT right '
                                                                        'boundary.)')
    parser.add_argument('--window_max', default=0.8, type=float, help='window right boundary (in msec) relative to '
                                                                      'the VOT right boundary (usually should be '
                                                                      'positive, that is, after the VOT left boundary)')
    parser.add_argument('--cv_textgrid_list', default='', help='list of manually labeled TextGrid files for '
                                                               'cross-validation')
    parser.add_argument('--cv_wav_list', default='', help='list of WAV files for cross-validation')
    parser.add_argument('--max_num_instances', default=0, type=int, help='max number of instances per file to use ('
                                                                         'default is to use everything)')
    parser.add_argument("--debug", help="verbose printing", action='store_const', const=True, default=False)
    args = parser.parse_args()

    # intermediate files that will be used to represent the locations of the VOTs, their windows and the features
    working_dir = tempfile.mkdtemp()
    features_dir = working_dir + "/features"
    basename = working_dir + "/training"
    os.makedirs(features_dir)
    input_filename = working_dir + "/training.input"
    features_filename = working_dir + "/training.feature_filelist"
    labels_filename = working_dir + "/training.labels"

    tier_definitions = TierDefinitions()
    tier_definitions.extract_definition(args)

    # prepare configuration files for the front end (means acoustic features extraction)
    textgrid2front_end(args.textgrid_list, args.wav_list, input_filename, features_filename, features_dir,
                       tier_definitions)

    # call front end
    cmd_vot_front_end = 'VotFrontEnd2 %s %s %s' % (input_filename, features_filename, labels_filename)
    easy_call(cmd_vot_front_end, verbose=args.debug)

    # random file lines
    features_filename_rs = features_filename + ".rs"
    labels_filename_rs = labels_filename + ".rs"
    num_instances = random_shuffle_data(features_filename, labels_filename, features_filename_rs, labels_filename_rs)

    # Split files to cross-validation
    if args.cv_textgrid_list == '':
        print "Info: using 20% of the data for cross-validation"
        num_instances_for_training = int(num_instances * 0.80)
        instance_range_for_training = (0, num_instances_for_training - 1)
        instance_range_for_test = (num_instances_for_training, num_instances - 1)
        if args.debug:
            print "num_instances=", num_instances
            print "num_instances_for_training=", num_instances_for_training
            print "instance_range_for_training=", instance_range_for_training
            print "instance_range_for_test=", instance_range_for_test
        features_filename_training = features_filename_rs + ".train"
        extract_lines(features_filename_rs, features_filename_training, instance_range_for_training)
        labels_filename_training = labels_filename_rs + ".train"
        extract_lines(labels_filename_rs, labels_filename_training, instance_range_for_training, has_header=True)
        features_filename_test = features_filename_rs + ".test"
        extract_lines(features_filename_rs, features_filename_test, instance_range_for_test)
        labels_filename_test = labels_filename_rs + ".test"
        extract_lines(labels_filename_rs, labels_filename_test, instance_range_for_test, has_header=True)
    else:
        features_filename_training = features_filename_rs
        labels_filename_training = labels_filename_rs
        input_filename_test = working_dir + "/training.input.test"
        features_filename_test = working_dir + "/training.feature_filelist.test"
        labels_filename_test = working_dir + "/training.labels.test"
        # prepare configuration files for the front end (means acoustic features extraction)
        textgrid2front_end(args.cv_textgrid_list, args.cv_wav_list, input_filename_test, features_filename_test,
                           features_dir, tier_definitions)
        # call front end
        cmd_vot_front_end = 'VotFrontEnd2 %s %s %s' % (input_filename_test, features_filename_test,
                                                              labels_filename_test)
        easy_call(cmd_vot_front_end, verbose=args.debug)

    # Training
    cmd_vot_training = 'InitialVotTrain -pos_only -vot_loss -epochs 2 -loss_eps 4 -min_vot_length 5 %s %s %s' % \
                       (features_filename_training, labels_filename_training, args.model_filename)
    easy_call(cmd_vot_training, verbose=args.debug)

    # Test
    cmd_vot_decode = 'InitialVotDecode %s %s %s' % (features_filename_test, labels_filename_test,
                                                           args.model_filename)
    easy_call(cmd_vot_decode, verbose=args.debug)

    # remove working directory and its content
    if not args.debug:
        shutil.rmtree(path=working_dir, ignore_errors=True)
