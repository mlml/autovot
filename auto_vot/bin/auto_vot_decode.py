#! /usr/bin/env python

"""
auto_vot_decode.py
Author: Joseph Keshet, 18/11/2013
"""

import argparse
import os
import tempfile
import shutil

from auto_vot_extract_features import *
from autovot.utilities import *
from autovot.textgrid import *


if __name__ == "__main__":
    # parse arguments
    parser = argparse.ArgumentParser(description='Decode AutoVOT')
    parser.add_argument('wav_filenames', default='', help="a list of WAV files.")
    parser.add_argument('textgrid_filenames', default='', help="a list of TextGrid files.")
    parser.add_argument('model_filename', help="output model file name")
    parser.add_argument('--single_file', help="wav_filenames and textgrid_filenames are single file names",
                        action='store_const', const=True, default=False)
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

    parser.add_argument('--min_vot_length', help='minimum allowed length of predicted VOT (in msec) in decoding',
                        default=15, type=int)
    parser.add_argument('--max_vot_length', help='maximum allowed length of predicted VOT (in msec) in decoding',
                        default=250, type=int)
    parser.add_argument('--max_num_instances', default=0, type=int, help='max number of instances per file to use ('
                                                                         'default is to use everything)')
    parser.add_argument('--ignore_existing_tiers', help='add a new AutoVOT tier to output textgrids, even if one already exists',
                        action='store_const', const=True, default=False)
    parser.add_argument("--logging_level", help="print out level (DEBUG, INFO, WARNING or ERROR)", default="INFO")
    args = parser.parse_args()

    logging_defaults(args.logging_level)

    # extract tier definitions
    tier_definitions = TierDefinitions()
    tier_definitions.extract_definition(args)

    if args.single_file:
        wav_files = [args.wav_filenames]
        textgrid_files = [args.textgrid_filenames]
    else:
        if num_lines(args.wav_filenames) != num_lines(args.textgrid_filenames):
            print "Error: the files %s and %s should have the same number of lines" % (args.labeled_textgrid_list,
                                                                                       args.predicted_vot_tier)
            exit()

        f = open(args.wav_filenames)
        wav_files = f.readlines()
        f.close()

        f = open(args.textgrid_filenames)
        textgrid_files = f.readlines()
        f.close()

    # run over files
    for wav_file, textgrid_file in zip(wav_files, textgrid_files):

        wav_file = wav_file.strip()
        textgrid_file = textgrid_file.strip()

        # intermediate files that will be used to represent the locations of the VOTs, their windows and the features
        working_dir = tempfile.mkdtemp()
        features_dir = working_dir + "/features"
        my_basename = os.path.splitext(os.path.basename(wav_file))[0]
        my_basename = working_dir + "/" + my_basename
        os.makedirs(features_dir)
        input_filename = my_basename + ".input"
        features_filename = my_basename + ".feature_filelist"
        labels_filename = my_basename + ".labels"
        preds_filename = my_basename + ".preds"
        final_vot_filename = my_basename + ".vot"

        textgrid_list = my_basename + ".tg_list"
        f = open(textgrid_list, 'w')
        f.write(textgrid_file + '\n')
        f.close()

        wav_list = my_basename + ".wav_list"
        f = open(wav_list, 'w')
        f.write(wav_file + '\n')
        f.close()

        logging.debug("working_dir=%s" % working_dir)

        print textgrid_list, wav_list
        print input_filename

        # call front end
        textgrid2front_end(textgrid_list, wav_list, input_filename, features_filename, features_dir, tier_definitions,
                           decoding=False)
        cmd_vot_front_end = 'VotFrontEnd2 -verbose %s %s %s %s' % (args.logging_level, input_filename, features_filename,
                                                                   labels_filename)
        easy_call(cmd_vot_front_end)

        # testing
        cmd_vot_decode = 'InitialVotDecode -verbose %s -max_onset 200 -min_vot_length %d -max_vot_length %d ' \
                         '-output_predictions %s %s %s %s' % (args.logging_level, args.min_vot_length,
                                                              args.max_vot_length, preds_filename, features_filename,
                                                              labels_filename, args.model_filename)
        easy_call(cmd_vot_decode)

        # convert decoding back to TextGrid
        xmin_proc_win = list()
        xmax_proc_win = list()
        for line in open(input_filename):
            items = line.strip().split()
            xmin_proc_win.append(float(items[1]))
            xmax_proc_win.append(float(items[2]))

        # convert decoding back to TextGrid. first generate list of xmin, xmax and mark
        xmin_preds = list()
        xmax_preds = list()
        mark_preds = list()
        k = 0
        for line in open(preds_filename):
            (confidence, xmin, xmax) = line.strip().split()
            xmin = float(xmin)
            xmax = float(xmax)
            if xmin < xmax:  # positive VOT
                xmin_preds.append(xmin_proc_win[k] + xmin/1000)
                xmax_preds.append(xmin_proc_win[k] + xmax/1000)
                mark_preds.append(confidence)
            else:  # negative VOT
                xmin_preds.append(xmin_proc_win[k] + xmax/1000)
                xmax_preds.append(xmin_proc_win[k] + xmin/1000)
                mark_preds.append("neg " + confidence)
                # print confidence, (xmin/1000), (xmax/1000), xmin_proc_win[k], " --> ", xmin_proc_win[k] + (xmin/1000), \
                #     xmin_proc_win[k] + (xmax/1000), " [", confidence, "]"
            k += 1

        # add "AutoVOT" tier to textgrid_filename
        textgrid = TextGrid()
        textgrid.read(textgrid_file)
        auto_vot_tier = IntervalTier(name='AutoVOT', xmin=textgrid.xmin(), xmax=textgrid.xmax())
        auto_vot_tier.append(Interval(textgrid.xmin(), xmin_preds[0], ''))
        # print textgrid.xmin(), xmin_preds[0], ''
        for i in xrange(len(xmin_preds) - 1):
            auto_vot_tier.append(Interval(xmin_preds[i], xmax_preds[i], mark_preds[i]))
            # print xmin_preds[i], xmax_preds[i], mark_preds[i]
            auto_vot_tier.append(Interval(xmax_preds[i], xmin_preds[i + 1], ''))
            # print xmax_preds[i], xmin_preds[i+1], ''
        auto_vot_tier.append(Interval(xmin_preds[-1], xmax_preds[-1], mark_preds[-1]))
        # print xmin_preds[-1], xmax_preds[-1], mark_preds[-1]
        auto_vot_tier.append(Interval(xmax_preds[-1], textgrid.xmax(), ''))
        # print xmax_preds[-1], textgrid.xmax(), ''


        ## check if target textgrid already has a tier named "AutoVOT", modulo preceding or trailing spaces or case
        existing_tiers = [n.strip() for n in textgrid.tierNames(case="lower") if n.strip()=='autovot']
        if len(existing_tiers)>0:
            print "WARNING: file %s already contains >=1 tier with a name similar to AutoVOT" % textgrid_file
            if args.ignore_existing_tiers:
                print "--ignore_existing_tiers flag used: writing a new AutoVOT tier (in addition to existing one(s))"
                textgrid.append(auto_vot_tier)
                textgrid.write(textgrid_file)
            else:
                logging.warning("new AutoVOT tier is *not* being written to the file")
                logging.warning("use the --ignore_existing_tiers flag if you'd like to do so")


        # delete the working directory at the end
        if args.logging_level != "DEBUG":
            shutil.rmtree(path=working_dir, ignore_errors=True)
