#! /usr/bin/env python3
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
# auto_vot_decode.py: Use an existing classifier to measure VOT for
# stops in a set of textgrids and corresponding wav files. Run with -h
# argument for information on arguments, and see documentation for
# usage examples.


from __future__ import division
from __future__ import unicode_literals
from __future__ import print_function
from __future__ import absolute_import
from builtins import zip
from builtins import range
from builtins import open
from builtins import int
from future import standard_library
standard_library.install_aliases()
import argparse
import os
import tempfile
import shutil
import csv

import numpy as np
import textgrid as tg

from auto_vot_extract_features import *
from helpers.utilities import *


if __name__ == "__main__":
    # parse arguments
    parser = argparse.ArgumentParser(description='Use an existing classifier to measure VOT for stops in a set of '
                                                 'textgrids and corresponding wav files.')
    parser.add_argument('wav_filenames', default='', help="Text file listing  WAV files")
    parser.add_argument('textgrid_filenames', default='', help="Text file listing corresponding manually labeled "
                                                               "TextGrid files containing stops VOT is to be measured for")
    parser.add_argument('model_filename', help="Name of classifier to be used to measure VOT")
    parser.add_argument('--vot_tier', help='Name of the tier containing manually labeled VOTs to compare automatic '
                                           'measurements to (optional. default is none)', default='')
    parser.add_argument('--vot_mark', help='On vot_tier, only intervals with this mark value (e.g. "vot", "pos", '
                                           '"neg") are used, or "*" for any string (this is the default)', default='*')
    parser.add_argument('--window_tier', help='Name of the tier containing windows to be searched as possible starts'
                                              'of the predicted VOT (default: none). If not given *and* vot_tier given'
                                              ', a window with boundaries window_min and window_max to the left and '
                                              'right of the manually labeled VOT will be used . NOTE: either window_tier'
                                              ' or vot_tier must be specified. If both are specified, window_tier is '
                                              'used, and window_min and window_max are ignored.', default='')
    parser.add_argument('--window_mark', help='VOT is only predicted for intervals on the window tier with this mark '
                                              'value (e.g. "vot", "pos", "neg"), or "*" for any string (this is the '
                                              'default)', default='*')
    parser.add_argument('--window_min', help='Left boundary of the window (in msec) relative to the VOT '
                                             'interval\'s left boundary.', default=-50, type=float)
    parser.add_argument('--window_max', help='Right boundary of the window (in msec) relative to the VOT interval\'s'
                        ' right boundary. Usually should be positive, that is, after the VOT interval\'s right '
                        'boundary. (default: %(default)s)', default=800, type=float)
    parser.add_argument('--min_vot_length', help='Minimum allowed length of predicted VOT (in msec) (default: '
                                                 '%(default)s)', default=15, type=int)
    parser.add_argument('--max_vot_length', help='Maximum allowed length of predicted VOT (in msec) (default: '
                        '%(default)s)', default=250, type=int)
    parser.add_argument('--max_num_instances', default=0, type=int, help='Maximum number of instances per file to use '
                                                                         '(default: use everything)')
    parser.add_argument('--ignore_existing_tiers', help='add a new AutoVOT tier to output textgrids, even if one '
                                                        'already exists (default: don\'t do so)',
                        action='store_const', const=True, default=False)
    parser.add_argument('--csv_file', help='Write a CSV file with this name with one row per predicited VOT, '
                                           'with columns for the prediction and the confidence of the prediction'
                                           ' (default: don\'t do this)', default='')

    parser.add_argument("--logging_level", help="Level of verbosity of information printed out by this program ("
                                                "DEBUG, INFO, WARNING or ERROR), in order of increasing verbosity. "
                                                "See http://docs.python.org/2/howto/logging for definitions. ("
                                                "default: %(default)s)", default="INFO")
    args = parser.parse_args()

    logging_defaults(args.logging_level)

    # extract tier definitions
    tier_definitions = TierDefinitions()
    args.window_min /= 1000.0   # convert msec to seconds
    args.window_max /= 1000.0   # convert msec to seconds
    tier_definitions.extract_definition(args)

    wav_files = list()
    textgrid_files = list()
    if is_valid_wav(args.wav_filenames) and is_textgrid(args.textgrid_filenames):
        logging.info("Input arguments consist of a single WAV and a single TextGrid.")
        wav_files = [args.wav_filenames]
        textgrid_files = [args.textgrid_filenames]
    elif is_valid_wav(args.wav_filenames) and not is_textgrid(args.textgrid_filenames):
        logging.error("%s valid WAV file while %s is not a valid TextGrid." % (args.wav_filenames,
                                                                               args.textgrid_filenames))
        logging.error("(note that the TextGrid file cannot have been saved as a 'short text file'")
        logging.error("or 'binary file' in Praat)")
        exit()
    elif not is_valid_wav(args.wav_filenames) and is_textgrid(args.textgrid_filenames):
        logging.error("%s not valid WAV file while %s is a valid TextGrid." % (args.wav_filenames,
                                                                               args.textgrid_filenames))
        exit()
    else:
        if num_lines(args.wav_filenames) != num_lines(args.textgrid_filenames):
            logging.error("The files %s and %s should have the same number of lines" % (args.wav_filenames,
                                                                                        args.textgrid_filenames))
            exit()

        with open(args.wav_filenames, 'r') as f:
            wav_files = f.readlines()

        with open(args.textgrid_filenames, 'r') as f:
            textgrid_files = f.readlines()

    problematic_files = list()

    out_file = None
    if args.csv_file:
        try:
            csv_file = open(args.csv_file, 'w')
            out_file = csv.writer(csv_file)
        except:
            logging.warning("Couldn't open %s for writing. CSV file not being written." % args.csv_name)
            out_file = None

    if out_file:
        out_file.writerow(['wav_file', 'time', 'vot', 'confidence'])

    # run over files
    for wav_file, textgrid_file in zip(wav_files, textgrid_files):

        wav_file = wav_file.strip()
        textgrid_file = textgrid_file.strip()

        if wav_file == "" or textgrid_file == "":
            continue

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
        with open(textgrid_list, 'w') as f:
            f.write(textgrid_file + '\n')

        wav_list = my_basename + ".wav_list"
        with open(wav_list, 'w') as f:
            f.write(wav_file + '\n')

        logging.debug("working_dir=%s" % working_dir)

        logging.debug("%s, %s" % (textgrid_list, wav_list))
        logging.debug("%s" % input_filename)

        # call front end (extract features)
        problematic_file = textgrid2front_end(textgrid_list, wav_list, input_filename, features_filename,
                                         features_dir, tier_definitions, decoding=True)
        if len(problematic_file):
            problematic_files += problematic_file
            continue
        
        cmd_vot_front_end = 'VotFrontEnd2 -verbose %s %s %s %s' % (args.logging_level, input_filename, features_filename,
                                                                   labels_filename)
        easy_call(cmd_vot_front_end)

        # decoding (i.e., generate VOT predictions)
        cmd_vot_decode = 'VotDecode -verbose %s -max_onset 200 -min_vot_length %d -max_vot_length %d ' \
                         '-output_predictions %s %s %s %s' % (args.logging_level, args.min_vot_length,
                                                              args.max_vot_length, preds_filename, features_filename,
                                                              labels_filename, args.model_filename)
        easy_call(cmd_vot_decode)

        # convert decoding back to TextGrid
        xmin_proc_win = list()
        xmax_proc_win = list()
        with open(input_filename, 'r') as f:
            for line in f:
                items = line.strip().split()
                xmin_proc_win.append(float(items[1]))
                xmax_proc_win.append(float(items[2]))

        # convert decoding back to TextGrid. first generate list of xmin, xmax and mark
        xmin_preds = list()
        xmax_preds = list()
        mark_preds = list()
        k = 0
        with open(features_filename, 'r') as f:
            feature_filelist = [line.strip() for line in f]
        with open(preds_filename, 'r') as f:
            vot_predictions = [line.strip() for line in f]
        for feature_filename, vot_prediction in zip(feature_filelist, vot_predictions):
            (confidence, xmin, xmax) = vot_prediction.split()
            xmin = float(xmin)
            xmax = float(xmax)
            if confidence == '0':
                logging.warning("Zero confidence stop detected.")
                prevoicing_decision = True
            else:
                # check pre-voicing
                features = np.loadtxt(feature_filename.rstrip(), skiprows=1)
                rapt_voicing_feature = features[:, 7]
                converted_rapt_voicing = np.where(rapt_voicing_feature < 0.01, -1, 1)
                prevoicing_decision = np.mean(converted_rapt_voicing[int(xmin):int(xmax)]) > 0
            if xmin < xmax:  # positive VOT
                xmin_preds.append(xmin_proc_win[k] + xmin/1000.0)
                xmax_preds.append(xmin_proc_win[k] + xmax/1000.0)
                if prevoicing_decision:
                    mark_preds.append("-{:.12g}".format(float(confidence)))
                else:
                    mark_preds.append("{:.12g}".format(float(confidence)))
            else:  # negative VOT
                xmin_preds.append(xmin_proc_win[k] + xmax/1000.0)
                xmax_preds.append(xmin_proc_win[k] + xmin/1000.0)
                mark_preds.append("neg {:.12g}".format(float(confidence)))
            k += 1

        # add "AutoVOT" tier to textgrid_filename
        textgrid = tg.TextGrid()
        textgrid.read(textgrid_file)
        auto_vot_tier = tg.IntervalTier(name='AutoVOT', minTime=textgrid.minTime, maxTime=textgrid.maxTime)
        auto_vot_tier.add(textgrid.minTime, xmin_preds[0], '')
        try:
            for i in range(len(xmin_preds) - 1):
                ## instead of mark_preds[i] (confidence number), just put 'pred' in the interval
                auto_vot_tier.add(xmin_preds[i], xmax_preds[i], 'pred')

                auto_vot_tier.add(xmax_preds[i], xmin_preds[i + 1], '')
            ## instead of mark_preds[i] (confidence number), just put 'pred' in the interval

            auto_vot_tier.add(xmin_preds[-1], xmax_preds[-1], 'pred')
            auto_vot_tier.add(xmax_preds[-1], textgrid.maxTime, '')
        except ValueError as e:
            logging.error("Overlapping stops detected, textgrid output stopped at {}".format(xmin_preds[i]))
            
        ## check if target textgrid already has a tier named
        ## "AutoVOT", modulo preceding or trailing spaces or case. If
        ## so, action taken depends on if --ignore_existing_tiers flag
        ## invoked
        existing_tiers = [n.strip() for n in [x.lower() for x in textgrid.getNames()] if n.strip() == 'autovot']
        if len(existing_tiers) > 0:
            logging.warning("File %s already contains a tier with the name \"AutoVOT\"" % textgrid_file)
            if args.ignore_existing_tiers:
                logging.warning("Writing a new AutoVOT tier (in addition to existing one(s))")
                textgrid.append(auto_vot_tier)
                textgrid.write(textgrid_file)
            else:
                logging.warning("New \"AutoVOT\" tier is NOT being written to the file.")
                logging.warning("(use the --ignore_existing_tiers flag if you'd like to do so)")
        else:
            textgrid.append(auto_vot_tier)
            textgrid.write(textgrid_file)

        # delete the working directory at the end
        if args.logging_level != "DEBUG":
            shutil.rmtree(path=working_dir, ignore_errors=True)

        if out_file:
            for i in range(len(xmin_preds) - 1):
                vot, conf = xmax_preds[i] - xmin_preds[i], mark_preds[i]
                out_file.writerow([wav_file, "{:.12g}".format(xmin_preds[i]), "{:.12g}".format(vot), conf])
            vot, conf = xmax_preds[-1] - xmin_preds[-1], mark_preds[-1]
            out_file.writerow([wav_file, "{:.12g}".format(xmin_preds[-1]), "{:.12g}".format(vot), conf])
            
    if out_file:
        csv_file.close()

    if len(problematic_files):
        logging.warning("**********************************")
        logging.warning("Prediction made for all files except these ones, where something was wrong:")
        logging.warning(problematic_files)
        logging.warning("Look for lines beginning with WARNING or ERROR in the program's output to see what went "
                        "wrong.")
        logging.warning("**********************************")

    logging.info("All done.")

