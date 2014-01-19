#! /usr/bin/env python

"""
auto_vot_extract_features.py
Author: Joseph Keshet, 18/11/2013
"""

import argparse
import wave
import os
from textgrid import *
from utilities import *


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
        self.max_num_instances = 0

    def extract_definition(self, args):
        self.vot_tier = args.vot_tier
        self.vot_mark = args.vot_mark
        self.window_tier = args.window_tier
        self.window_mark = args.window_mark
        self.window_min = args.window_min
        self.window_max = args.window_max
        try:
            self.max_num_instances = args.max_num_instances
        except Exception as exception:
            self.max_num_instances = -1


def textgrid2front_end(textgrid_list, wav_list, input_filename, features_filename, features_dir, definitions,
                       decoding=False):

    # check the number of TextGrid files is the same as WAV files.
    if not num_lines(wav_list) == num_lines(textgrid_list):
        print "Error: number of TextGrid files should match the number of WAVs"
        exit()

    input_file = open(input_filename, 'w')
    feature_file = open(features_filename, 'w')
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

        if decoding:

            if definitions.window_tier == "":
                print "Error: when extracting features for decoding the window tier name has to be given"
                exit(-1)

            # check if the window tier is one of the tiers in the TextGrid
            if definitions.window_tier in tier_names:
                tier_index = tier_names.index(definitions.vot_tier)
                instances = list()
                # run over all intervals in the tier
                for interval in textgrid[tier_index]:
                    if (definitions.window_mark == "*" and re.search(r'\S', interval.mark())) \
                        or (interval.mark() == definitions.window_mark):
                        window_min = interval.xmin()
                        window_max = interval.xmax()
                        new_instance = Instance()
                        new_instance.set(wav_filename, window_min, window_max, window_min, window_max)
                        instances.append(new_instance)
                # check if the given mark was ever found
                if not instances:
                    print "Warning: the mark '%s' has not found in tier '%s' of %s" % (definitions.window_mark,
                                                                                       definitions.window_tier,
                                                                                       textgrid_filename)
                    continue
        else:

            # check if the VOT tier is one of the tiers in the TextGrid
            if definitions.vot_tier in tier_names:
                tier_index = tier_names.index(definitions.vot_tier)
                # run over all intervals in the tier
                instances = list()
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
                    continue

            # if the window tier is empty and not decoding, fix window information
            if definitions.window_tier == "":
                for i in xrange(1, len(instances)-1):
                    # check if window_min is less than the previous vot_max
                    if instances[i].window_min < instances[i-1].vot_max:
                        instances[i].window_min = max(instances[i].vot_min - 0.02, instances[i-1].vot_max + 0.02)
                    # check if window_max is greater than the next vot_min
                    if instances[i].window_max > instances[i+1].vot_min:
                        instances[i].window_max = min(instances[i].vot_max + 0.02, instances[i+1].vot_min - 0.02)
                    # check for consistency
                    if instances[i].window_min > instances[i].vot_min \
                        or instances[i].vot_min > instances[i].window_max \
                        or instances[i].window_min > instances[i].vot_max \
                        or instances[i].window_max < instances[i].vot_max:
                        print "Error: something wrong in the TextGrid VOT tier:", instances[i]
            else:
                print "Info: --window_tier and --window_mark for training mode need to be implemented. Using defaults."

        # write out the information
        max_num_instances = definitions.max_num_instances
        num_instances = 0
        for instance in instances:
            if max_num_instances > 0 and num_instances >= max_num_instances:
                break
            basename = os.path.splitext(os.path.basename(textgrid_filename))[0]
            feature_line = '%s/%s_%.3f.txt\n' % (features_dir, basename, instance.window_min)
            input_file.write(str(instance))
            feature_file.write(feature_line)
            num_instances += 1

    input_file.close()
    feature_file.close()
    textgrid_list.close()
    wav_list.close()


if __name__ == "__main__":
    # parse arguments
    parser = argparse.ArgumentParser(description='Extract acoustic features of AutoVOT')
    parser.add_argument('textgrid_list', help="list of manually labeled TextGrid files (input)")
    parser.add_argument('wav_list', help="list of WAV files (input)")
    parser.add_argument('input_filename', help="AutoVOT front end input file name (output)")
    parser.add_argument('features_filename', help="AutoVOT front end features file name (output)")
    parser.add_argument('labels_filename', help="AutoVOT front end labels file name (output)")
    parser.add_argument('features_dir', help="directory to put the feature files (output)")
    parser.add_argument('--decoding', help='Extract features for decoding based on window_tier (vot_tier is '
                                           'ignored), otherwise extract features for training based on manual '
                                           'labeling given in the vot_tier', action='store_const', const=True,
                        default=False)
    parser.add_argument('--vot_tier', help='name of the tier to extract VOTs from', default='vot')
    parser.add_argument('--vot_mark', help='VOT mark value (e.g., "pos", "neg") or "*" for any string', default='*')
    parser.add_argument('--window_tier', help='used this window as a search window for training. If not given, '
                                              'a constant window with parameters [window_min, window_max] around the '
                                              'manually labeled VOT will be used', default='')
    parser.add_argument('--window_mark', help='window mark value or "*" for any string', default='')
    parser.add_argument('--window_min', help='window left boundary (in msec) relative to the VOT right boundary '
                                             '(usually should be negative, that is, before the VOT right boundary.)',
                        default=-0.05, type=float)
    parser.add_argument('--window_max', help='window right boundary (in msec) relative to the VOT right boundary ('
                                             'usually should be positive, that is, after the VOT left boundary.)',
                        default=0.8, type=float)
    parser.add_argument('--max_num_instances', help='max number of instances per file to use (default is to use '
                                                    'everything)', default=0, type=int)
    parser.add_argument("--debug", help="verbose printing", action='store_const', const=True, default=False)
    args = parser.parse_args()

    # check that the features_dir exists

    # prepare configuration files for the front end (means acoustic features extraction)
    tier_definitions = TierDefinitions()
    tier_definitions.extract_definition(args)

    # prepare files for front end
    textgrid2front_end(args.textgrid_list, args.wav_list, args.input_filename, args.features_filename,
                       args.features_dir, tier_definitions, args.decoding)

    # call front end
    cmd_vot_front_end = 'VotFrontEnd2 %s %s %s' % (args.input_filename, args.features_filename, 
                                                          args.labels_filename)
    easy_call(cmd_vot_front_end, verbose=args.debug)
