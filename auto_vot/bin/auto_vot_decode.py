#! /usr/bin/env python

"""
auto_vot_decode.py
Author: Joseph Keshet, 18/11/2013
"""

import argparse
import os
import tempfile
from auto_vot_extract_features import *
from utilities import *
from textgrid import *
import shutil


if __name__ == "__main__":
    # parse arguments
    parser = argparse.ArgumentParser(description='Decode AutoVOT after features extraction')
    parser.add_argument('wav_filename', default='', help="a Wav filename")
    parser.add_argument('textgrid_filename', default='', help="TextGrid with a window tier")
    parser.add_argument('model_filename', help="output model file name")
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
    ## MS: set to 15 because that's the default for InitialVotDecode

    parser.add_argument('--max_vot_length', help='maximum allowed length of predicted VOT (in msec) in decoding',
                        default=250, type=int)

    ## MS: set to 250 because that's what was in the call to
    ## InitialVotDecode below, before I changed it

    parser.add_argument("--debug", help="verbose printing", action='store_const', const=True, default=False)
    args = parser.parse_args()

    if args.wav_filename == '' and args.textgrid_filename == '':
        print 'Error: either the parameters --wav_filename and --textgrid_filename should be given or ' \
              'the parameters --features_filename and --labels_filename should be given.'
        exit(-1)

    # extract tier definitions
    tier_definitions = TierDefinitions()
    tier_definitions.extract_definition(args)

    # intermediate files that will be used to represent the locations of the VOTs, their windows and the features
    working_dir = tempfile.mkdtemp()
    features_dir = working_dir + "/features"
    basename = os.path.splitext(os.path.basename(args.wav_filename))[0]
    basename = working_dir + "/" + basename
    os.makedirs(features_dir)
    input_filename = basename + ".input"
    features_filename = basename + ".feature_filelist"
    labels_filename = basename + ".labels"
    preds_filename = basename + ".preds"
    final_vot_filename = basename + ".vot"

    textgrid_list = basename + ".tg_list"
    f = open(textgrid_list, 'w')
    f.write(args.textgrid_filename + '\n')
    f.close()

    wav_list = basename + ".wav_list"
    f = open(wav_list, 'w')
    f.write(args.wav_filename + '\n')
    f.close()

    if args.debug:
        print "working_dir=", working_dir

    # call front end
    textgrid2front_end(textgrid_list, wav_list, input_filename, features_filename, features_dir, tier_definitions,
                       decoding=False)
    cmd_vot_front_end = 'VotFrontEnd2 %s %s %s' % (input_filename, features_filename, labels_filename)
    easy_call(cmd_vot_front_end, verbose=args.debug)

    # testing
    cmd_vot_decode = 'InitialVotDecode -max_onset 200 -min_vot_length %d -max_vot_length %d -output_predictions %s ' \
                     '%s %s %s' % (args.min_vot_length, args.max_vot_length, preds_filename, features_filename,
                                   labels_filename, args.model_filename)
    easy_call(cmd_vot_decode, verbose=args.debug)


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
        if args.debug:
            print confidence, (xmin/1000), (xmax/1000), xmin_proc_win[k], " --> ", xmin_proc_win[k] + (xmin/1000), \
                xmin_proc_win[k] + (xmax/1000), " [", confidence, "]"
        k += 1

    # add "AutoVOT" tier to textgrid_filename
    textgrid = TextGrid()
    textgrid.read(args.textgrid_filename)
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
    textgrid.append(auto_vot_tier)
    textgrid.write(args.textgrid_filename)

    # delete the working directory at the end
    if not args.debug:
        shutil.rmtree(path=working_dir, ignore_errors=True)
