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
# auto_vot_performance.py: Compute various measures of performance
# given a set of labeled VOTsand predicted VOTs for the same stops,
# optionally writing information for each stop to a CSV file.
#

import argparse, os, csv
import numpy as np
import scipy.stats

corr2 = scipy.stats.spearmanr
corr1 = scipy.stats.pearsonr


from helpers.textgrid import *
from helpers.utilities import *

def num_lines(filename):
    lines = 0
    for _ in open(filename):
        lines += 1
    return lines


def read_textgrid_tier(textgrid_filename, vot_tier):
     # read TextGrid
    textgrid = TextGrid()
    textgrid.read(textgrid_filename)

    # extract tier names
    tier_names = textgrid.tierNames()

    # check if the VOT tier is one of the tiers in the TextGrid
    vots = list()
    if vot_tier in tier_names:
        tier_index = tier_names.index(vot_tier)
        # run over all intervals in the tier
        for interval in textgrid[tier_index]:
            if re.search(r'\S', interval.mark()):
                vots.append(interval)
    return vots

# def remove_outliers(x_list, y_list):
#     x_array = np.array(x_list)
#     x_mean = np.mean(x_array)
#     x_std = np.std(x_array)
#     y_array = np.array(y_list)
#     y_mean = np.mean(y_array)
#     y_std = np.std(y_array)

#     x_list_ro = list()
#     y_list_ro = list()
#     if len(x_list) != len(y_list):
#         logging.error("Something's wrong. The length of the input lists is not the same.")
#     for x, y in zip(x_list, y_list):
#         if abs(x-x_mean) < 3.0*x_std and abs(y-y_mean) < 3.0*y_std:
#             x_list_ro.append(x)
#             y_list_ro.append(y)

#     return x_list_ro, y_list_ro


if __name__ == "__main__":
    # parse arguments
    parser = argparse.ArgumentParser(description='Compute various measures of performance given a set of labeled VOTs'
                                                 'and predicted VOTs for the same stops, optionally writing information'
                                                 'for each stop to a CSV file.')
    parser.add_argument('labeled_textgrid_list', help="textfile listing TextGrid files containing manually labeled "
                                                      "VOTs (one file per line)")
    parser.add_argument('predicted_textgrid_list', help="textfile listing TextGrid files containing predicted VOTs ("
                                                        "one file per line). This can be the same as "
                                                        "labeled_textgrid_list, provided two different tiers are "
                                                        "given for labeled_vot_tier and predicted_vot_tier.")
    parser.add_argument('labeled_vot_tier', help='name of the tier containing manually labeled VOTs in the TextGrids '
                                                 'in labeled_textgrid_list (default: %(default)s)', default='vot')
    parser.add_argument('predicted_vot_tier', help='name of the tier containing automatically labeled VOTs in the '
                                                   'TextGrids in predicted_textgrid_list (default: %(default)s)',
                        default='AutoVOT')
    parser.add_argument('--csv_file', help='csv file to dump labeled and predicted VOT info to (default: none)')
    args = parser.parse_args()

    if is_textgrid(args.labeled_textgrid_list) and is_textgrid(args.predicted_textgrid_list):
        labeled_textgrids = [args.labeled_textgrid_list]
        predicted_textgrids = [args.predicted_textgrid_list]
    else:
        if num_lines(args.labeled_textgrid_list) != num_lines(args.predicted_textgrid_list):
            print "Error: the files %s and %s should have the same number of lines" % (args.labeled_textgrid_list,
                                                                                       args.predicted_vot_tier)
            exit()

        f = open(args.labeled_textgrid_list)
        labeled_textgrids = f.readlines()
        f.close()

        f = open(args.predicted_textgrid_list)
        predicted_textgrids = f.readlines()
        f.close()

    if len(labeled_textgrids) != len(predicted_textgrids):
        logging.error("Something's wrong. Didn't read the same number of files from textgrid lists.")

    x_xmin = []
    x_xmax = []
    x_vot = []
    y_xmin = []
    y_xmax = []
    y_vot = []

    x_files = []
    y_files = []
    
    for labeled_textgrid, predicted_textgrid in zip(labeled_textgrids, predicted_textgrids):
        labeled_vots = read_textgrid_tier(labeled_textgrid.strip(), args.labeled_vot_tier)
        
        x_xmin = x_xmin + [x.xmin() for x in labeled_vots]
        x_xmax = x_xmax + [x.xmax() for x in labeled_vots]
        x_vot = x_vot + [(x.xmax()-x.xmin()) for x in labeled_vots]
        x_files = x_files + [labeled_textgrid.strip() for x in labeled_vots]

        predicted_vots = read_textgrid_tier(predicted_textgrid.strip(), args.predicted_vot_tier)
        y_xmin = y_xmin + [y.xmin() for y in predicted_vots]
        y_xmax = y_xmax + [y.xmax() for y in predicted_vots]
        y_vot = y_vot + [(y.xmax()-y.xmin()) for y in predicted_vots]
        y_files = y_files + [predicted_textgrid.strip() for x in predicted_vots]


    labF = os.path.abspath(args.labeled_textgrid_list)
    predF = os.path.abspath(args.predicted_textgrid_list)


    print "\n\nEvaluating labeled vs. predicted VOTs, using:"
    print "- labeled VOTs: '%s' tier in %s" % (args.labeled_vot_tier, labF)
    print "- predicted VOTs: '%s' tier in %s" % (args.predicted_vot_tier, predF)

    ## performance measure 1
    print
    print "Correlations (Pearson/Spearman) between predicted/labeled:"
    print "-------------"

    print "Left edge (burst onset): ", corr1(x_xmin, y_xmin)[0], "/", corr2(x_xmin, y_xmin)[0]
    print "Right edge (voicing onset): ", corr1(x_xmax, y_xmax)[0], "/", corr2(x_xmax, y_xmax)[0]
    print "VOTs: ", corr1(x_vot, y_vot)[0], "/", corr2(x_xmax, y_xmax)[0]

    print "\n(Note: if the Pearson and Spearman correlations differ much,\nyou probably have outliers which are " \
          "strongly influencing Pearson's R)\n"

    ## numpy arrays of labeled VOTs and predicted VOTs (in sec)
    X, Y = np.array(x_vot), np.array(y_vot)

    ## performance measure 2
    print "Mean, standard deviation of labeled/predicted difference:"
    print "------------------------------"
    print "VOT:", 1000*np.mean(abs(X-Y)), "msec,", 1000*np.std(abs(X-Y)), "msec\n"

    ## performance measure 3
    thresholds = [2, 5, 10, 15, 20, 25, 50]
    print "Percentage of examples with labeled/predicted VOT difference of at most:"
    print "------------------------------"
    for thresh in thresholds:
        print "%d msec: " % thresh, 100*(len(X[abs(X-Y)< thresh/1000.0])/float(len(X)))

    ## dump predicted/labeled VOT info to a CSV, for later examination in R, excel, etc.
    if args.csv_file:
        print "\nWriting labeled / predicted VOT info to %s" % args.csv_file
        out_file = open(args.csv_file, 'wb')
        csv_file = csv.writer(out_file)
        csv_file.writerow(['filename_labeled', 'filename_predicted', 'time_in_labeledF', 'time_in_predictedF',
                           'tier_in_labeledF', 'tier_in_predictedF', 'vot_labeled', 'vot_predicted'])
        for i, xmin in enumerate(x_xmin):
            csv_file.writerow([x_files[i], y_files[i], str(xmin), str(y_xmin[i]), args.labeled_vot_tier,
                               args.predicted_vot_tier, str(x_vot[i]), str(y_vot[i])])
        out_file.close()
