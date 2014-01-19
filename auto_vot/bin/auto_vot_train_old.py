#! /usr/bin/env python

"""
auto_vot_train.py
Author: Joseph Keshet, 18/11/2013
"""

import argparse
import os
import shutil
import subprocess
import tempfile
import wave
import random

from textgrid import *


class Instance:
    def __init__(self):
        self.wav_filename = ""
        self.window_min = 0
        self.window_max = 0
        self.vot_min = 0
        self.vot_max = 0

    def set(self, wav_filename, window_min, window_max, vot_min, vot_max):
        self.wav_filename = wav_filename
        self.window_min = window_min
        self.window_max = window_max
        self.vot_min = vot_min
        self.vot_max = vot_max

    def __str__(self):
        return '"%s" %.3f %.3f %.3f %.3f\n' % (self.wav_filename, self.window_min, self.window_max, self.vot_min,
                                               self.vot_max)
        ## the following lines refer to the old VotFrontEnd (that cannot deal with WAV file names that have spaces in
        ## their names:
        ##return '%s %d %d %d %d\n' % (self.wav_filename, int(16000*self.window_min), int(16000*self.window_max),
        ##                             int(16000*self.vot_min), int(16000*self.vot_max))


class TierDefinitions:
    def __init__(self):
        self.vot_tier = ''
        self.vot_mark = ''
        self.window_tier = ''
        self.window_mark = ''
        self.window_min = 0
        self.window_max = 0
        self.condition_tier = ''
        self.condition_mark = ''
        self.max_num_instances = 0

    def extract_definition(self, args):
        self.vot_tier = args.vot_tier
        self.vot_mark = args.vot_mark
        self.window_tier = args.window_tier
        self.window_mark = args.window_mark
        self.window_min = args.window_min
        self.window_max = args.window_max
        self.condition_tier = args.condition_tier
        self.condition_mark = args.condition_mark
        self.max_num_instances = args.max_num_instances


def easy_call(command, log_file='', verbose=False):
    os.environ['DYLD_LIBRARY_PATH'] = \
        '/Users/jkeshet/Projects/goldrick_projects/code/audiofile-0.3.4/libaudiofile/.libs'
    try:
        if verbose:
            print command
        return_code = subprocess.call(command, shell=True)
        if verbose:
            print "return_code:", return_code
    except Exception as exception:
        print "Error: could not execute the following"
        print ">>>>", command
        print type(exception)     # the exception instance
        print exception.args      # arguments stored in .args
        exit(-1)


def num_lines(filename):
    lines = 0
    for _ in open(filename):
        lines += 1
    return lines


def textgrid2front_end(textgrid_list, wav_list, definitions):

    # check the number of TextGrid files is the same as WAV files.
    if not num_lines(wav_list) == num_lines(textgrid_list):
        print "Error: number of TextGrid files should match the number of WAVs"
        exit()

    # intermediate files that will be used to represent the locations of the VOTs, their windows and the features
    tmp_input_fd, tmp_input_filename = tempfile.mkstemp()
    tmp_features_fd, tmp_features_filename = tempfile.mkstemp()
    tmp_labels_fd, tmp_labels_filename = tempfile.mkstemp()
    tmp_features_dir = tempfile.mkdtemp()

    textgrid_list = open(textgrid_list)
    wav_list = open(wav_list)
    for textgrid_filename, wav_filename in zip(textgrid_list, wav_list):
        textgrid_filename = textgrid_filename.strip()
        wav_filename = wav_filename.strip()
        print textgrid_filename, wav_filename
        # check the sampling rate and number bits of the WAV
        wav_file = wave.Wave_read(wav_filename)
        if wav_file.getframerate() != 16000 or wav_file.getsampwidth() != 2 or wav_file.getnchannels() != 1 \
            or wav_file.getcomptype() != 'NONE':
            print "Error: WAV file format should be sampling_rate=16000, sample_width=2 and num_channels=1"
            print "       Consider changing the file parameters with a utility such as 'sox' as follows:"
            print "       sox input.wav  -c 1 -r 16000 output.wav"
            print "       (sox can be downloaded from http://sox.sourceforge.net)"
            exit(-1)
        textgrid = TextGrid()
        # read TextGrid
        textgrid.read(textgrid_filename)
        # extract tier names
        tier_names = textgrid.tierNames()
        # check if the VOT tier is one of the tiers in the TextGrid
        if definitions.vot_tier in tier_names:
            instances = list()
            tier_index = tier_names.index(definitions.vot_tier)

            # run over all intervals in the tier
            for interval in textgrid[tier_index]:
                if (definitions.vot_mark == "*" and re.search(r'\S', interval.mark())) \
                    or (interval.mark() == definitions.vot_mark):
                    window_min = max(interval.xmin() + definitions.window_min, 0)
                    window_max = min(interval.xmax() + definitions.window_max, textgrid.xmax())
                    new_instance = Instance()
                    new_instance.set(wav_filename, window_min, window_max, interval.xmin(), interval.xmax())
                    instances.append(new_instance)
            # check if the given mark was ever found
            if not instances:
                print "Warning: the mark '%s' has not found in tier '%s' of %s" % (definitions.vot_mark,
                                                                                   definitions.vot_tier,
                                                                                   textgrid_filename)
            # now correct window information
            if definitions.window_tier == "":
                ##print "first ", instances[0],
                for i in xrange(1, len(instances)-1):
                    ##print "curr   ", instances[i],
                    # check if window_min is less than the previous vot_max
                    if instances[i].window_min < instances[i-1].vot_max:
                        ##print "prev   ", instances[i-1],
                        instances[i].window_min = max(instances[i].vot_min - 0.02, instances[i-1].vot_max + 0.02)
                    # check if window_max is greater than the next vot_min
                    if instances[i].window_max > instances[i+1].vot_min:
                        ##print "next   ", instances[i+1],
                        instances[i].window_max = min(instances[i].vot_max + 0.02, instances[i+1].vot_min - 0.02)
                    ##print "after  ", instances[i],
                    if instances[i].window_min > instances[i].vot_min \
                        or instances[i].vot_min > instances[i].window_max \
                        or instances[i].window_min > instances[i].vot_max \
                        or instances[i].window_max < instances[i].vot_max:
                        print "Error: something wrong in the TextGrid VOT tier:", instances[i]
            else:
                # read window tier
                pass
                # align window tier with vot tier
                pass
                # update new window information
                pass

            # check for condition tier
            if not definitions.condition_tier == "":
                # read condition tier
                pass
                # align condition tier with vot tier
                pass
                # filter out all the non-relevant element in the instances list
                pass

            # write out the information
            max_num_instances = definitions.max_num_instances
            num_instances = 0
            for instance in instances:
                if max_num_instances > 0 and num_instances >= max_num_instances:
                    break
                basename = os.path.splitext(os.path.basename(textgrid_filename))[0]
                feature_line = '%s/%s_%.3f.txt\n' % (tmp_features_dir, basename, instance.window_min)
                os.write(tmp_input_fd, str(instance))
                os.write(tmp_features_fd, feature_line)
                num_instances += 1
        else:
            print "Warning: the tier '%s' is not found in %s" % (definitions.vot_tier, textgrid_filename)
    os.close(tmp_input_fd)
    os.close(tmp_features_fd)
    textgrid_list.close()
    wav_list.close()
    return tmp_input_filename, tmp_features_filename, tmp_labels_filename, tmp_features_dir


def random_shuffle_data(tmp_input_filename, tmp_features_filename):

    # read files
    input_file = open(tmp_input_filename)
    features_file = open(tmp_features_filename)
    lines = list()
    for x, y in zip(input_file, features_file):
        lines.append((x, y))
    input_file.close()
    features_file.close()

    # random shuffle the instances
    random.shuffle(lines)

    # write back the result
    input_file = open(tmp_input_filename, 'w')
    features_file = open(tmp_features_filename, 'w')
    for x, y in lines:
        input_file.write(x)
        features_file.write(y)
    input_file.close()
    features_file.close()

    return len(lines)


def extract_lines(input_filename, output_filename, lines_range):
    if lines_range[0] >= lines_range[1]:
        print "Error: range should be causal."
        exit(-1)
    input_file = open(input_filename)
    output_file = open(output_filename, 'w')
    for line_num, line in enumerate(input_file):
        if lines_range[0] <= line_num <= lines_range[1]:
            output_file.write(line)
    input_file.close()
    output_file.close()


if __name__ == "__main__":
    # parse arguments
    parser = argparse.ArgumentParser(description='Training of AutoVOT')
    parser.add_argument('textgrid_list', help="list of manually labeled TextGrid files")
    parser.add_argument('wav_list', help="list of WAV files")
    parser.add_argument('model_filename', help="output model file name")
    parser.add_argument('--vot_tier', default='vot', help='name of the tier to extract VOTs from')
    parser.add_argument('--vot_mark', default='*', help='VOT mark value (e.g., "pos", "neg") or "*" for any string')
    parser.add_argument('--window_tier', default='', help='used this window as a search window for training. If not '
        'given, a constant window with parameters [window_min,window_max] around the manually labeled VOT will be used')
    parser.add_argument('--window_mark', default='', help='window mark value or "*" for any string')
    parser.add_argument('--window_min', default=-0.05, type=float, help='window left boundary (in msec) relative to '
        'the VOT right boundary (usually should be negative, that is, before the VOT right boundary.)')
    parser.add_argument('--window_max', default=0.8, type=float, help='window right boundary (in msec) relative to '
        'the VOT right boundary (usually should be positive, that is, after the VOT left boundary.)')
    parser.add_argument('--condition_tier', default='', help='extract VOTs that meets a condition of this tier')
    parser.add_argument('--condition_mark', default='', help='extract VOTs whenever the mark equals condition_mark')
    parser.add_argument('--cv_textgrid_list', default='', help='list of manually labeled TextGrid files for '
        'cross-validation')
    parser.add_argument('--cv_wav_list', default='', help='list of WAV files for cross-validation')
    parser.add_argument('--max_num_instances', default=0, type=int, help='max number of instances per file to use ('
        'default is to use everything)')
    parser.add_argument('--C', default=10.0, type=float, help='C value for training PA')
    parser.add_argument('--log_file', default='', help='redirect output to a log file')
    parser.add_argument("--debug", help="verbose printing", action='store_const', const=True, default=False)
    args = parser.parse_args()

    if args.condition_tier != "" or args.condition_mark != "":
        print "Info: --condition_tier  and --condition_mark are not yet implemented. Using defaults"

    tier_definitions = TierDefinitions()
    tier_definitions.extract_definition(args)

    # prepare configuration files for the front end (means acoustic features extraction)
    tmp_input_filename, tmp_features_filename, tmp_labels_filename, tmp_features_dir = \
        textgrid2front_end(args.textgrid_list, args.wav_list, tier_definitions)

    # random file lines
    num_instances = random_shuffle_data(tmp_input_filename, tmp_features_filename)

    # Split files to cross-validation
    if args.cv_textgrid_list == '':
        print "Info: using 20% of the data for cross-validation"
        num_instances_for_training = int(num_instances * 0.80)
        instance_range_for_training = (0, num_instances_for_training-1)
        instance_range_for_test = (num_instances_for_training, num_instances-1)
        if args.debug:
            print "num_instances=", num_instances
            print "num_instances_for_training=", num_instances_for_training
            print "instance_range_for_training=", instance_range_for_training
            print "instance_range_for_test=", instance_range_for_test
        tmp_input_filename_training = tmp_input_filename + ".train"
        extract_lines(tmp_input_filename, tmp_input_filename_training, instance_range_for_training)
        tmp_features_filename_training = tmp_features_filename + ".train"
        extract_lines(tmp_features_filename, tmp_features_filename_training, instance_range_for_training)
        tmp_labels_filename_training = tmp_labels_filename + ".train"
        extract_lines(tmp_labels_filename, tmp_labels_filename_training, instance_range_for_training)
        tmp_input_filename_test = tmp_input_filename + ".test"
        extract_lines(tmp_input_filename, tmp_input_filename_test, instance_range_for_test)
        tmp_features_filename_test = tmp_features_filename + ".test"
        extract_lines(tmp_features_filename, tmp_features_filename_test, instance_range_for_test)
        tmp_labels_filename_test = tmp_labels_filename + ".test"
        extract_lines(tmp_labels_filename, tmp_labels_filename_test, instance_range_for_test)
    else:
        tmp_input_filename_training = tmp_input_filename
        tmp_features_filename_training = tmp_features_filename
        tmp_labels_filename_training = tmp_labels_filename
        tmp_input_filename_test, tmp_features_filename_test, tmp_labels_filename_test, tmp_features_dir_test = \
            textgrid2front_end(args.cv_textgrid_list, args.cv_wav_list, tier_definitions)

    # Training
    cmd_vot_front_end = 'VotFrontEnd2 %s %s %s' % (tmp_input_filename_training,
                                                             tmp_features_filename_training,
                                                             tmp_labels_filename_training)
    easy_call(cmd_vot_front_end, verbose=args.debug, log_file=args.log_file)
    if args.sigma2 == 0.0:
        kernel_string = ''
    else:
        kernel_string = '-kernel_expansion rbf2 -sigma %f' % args.sigma2
    cmd_vot_training = 'InitialVotTrain -pos_only -vot_loss -epochs 2 ' \
                       '-loss_eps 4 -min_vot_length 5 %s -C %f %s %s %s' % \
                       (kernel_string, args.C, tmp_features_filename_training, tmp_labels_filename_training,
                        args.model_filename)
    easy_call(cmd_vot_training, verbose=args.debug, log_file=args.log_file)

    # Testing
    cmd_vot_front_end = 'VotFrontEnd2 %s %s %s' % (tmp_input_filename_test, tmp_features_filename_test,
                                                             tmp_labels_filename_test)
    easy_call(cmd_vot_front_end, verbose=args.debug, log_file=args.log_file)
    cmd_vot_decode = 'InitialVotDecode %s %s %s %s' % \
                     (kernel_string, tmp_features_filename_test, tmp_labels_filename_test,  args.model_filename)
    easy_call(cmd_vot_decode, verbose=args.debug, log_file=args.log_file)

    if not args.debug:
        os.remove(tmp_input_filename)
        os.remove(tmp_features_filename)
        os.remove(tmp_labels_filename)
        os.remove(tmp_input_filename_training)
        os.remove(tmp_features_filename_training)
        os.remove(tmp_labels_filename_training)
        os.remove(tmp_input_filename_test)
        os.remove(tmp_features_filename_test)
        os.remove(tmp_labels_filename_test)
        shutil.rmtree(path=tmp_features_dir, ignore_errors=True)

