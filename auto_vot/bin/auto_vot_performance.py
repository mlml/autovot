#! /usr/bin/env python

"""
performance_of_textgrids.py
Author: Joseph Keshet, 18/11/2013
"""

import argparse
import math
import numpy as np

from autovot.textgrid import *
from autovot.utilities import *


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


def remove_outliers(x_list, y_list):
    x_array = np.array(x_list)
    x_mean = np.mean(x_array)
    x_std = np.std(x_array)
    y_array = np.array(y_list)
    y_mean = np.mean(y_array)
    y_std = np.std(y_array)

    x_list_ro = list()
    y_list_ro = list()
    if len(x_list) != len(y_list):
        logging.error("Something's wrong. The length of the input lists is not the same.")
    for x, y in zip(x_list, y_list):
        if abs(x-x_mean) < 3.0*x_std and abs(y-y_mean) < 3.0*y_std:
            x_list_ro.append(x)
            y_list_ro.append(y)

    return x_list_ro, y_list_ro

def correlation(x_list, y_list):
    # compute first moments
    x_mean = sum(x_list)/float(len(x_list))
    y_mean = sum(y_list)/float(len(y_list))
    # compute second moments
    sum_xy = 0
    sum_x2 = 0
    sum_y2 = 0
    for x, y in zip(x_list, y_list):
        sum_xy += (x-x_mean)*(y-y_mean)
        sum_x2 += (x-x_mean)*(x-x_mean)
        sum_y2 += (y-y_mean)*(y-y_mean)
    #correlation
    corr = sum_xy/math.sqrt(sum_x2*sum_y2)
    return corr


if __name__ == "__main__":
    # parse arguments
    parser = argparse.ArgumentParser(description='Compute the performance between a set of predicted textgrid files '
                                                 'and a set of labeled textgrid files.')
    parser.add_argument('labeled_textgrid_list', help="list of manually labeled TextGrid files")
    parser.add_argument('predicted_textgrid_list', help="list of predicted TextGrid files  (can be the same as "
                                                        "labeled_textgrid_list when the same TextGrid is used with "
                                                        "two different tiers to compare)")
    parser.add_argument('labeled_vot_tier', help='name of the VOT tier of the manual labeled TextGrids',
                        default='vot')
    parser.add_argument('predicted_vot_tier', help='name of the VOT tier to predicted TetGrids', default='vot')
    parser.add_argument("--debug", help="verbose printing", action='store_const', const=True, default=False)
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
    for labeled_textgrid, predicted_textgrid in zip(labeled_textgrids, predicted_textgrids):
        labeled_vots = read_textgrid_tier(labeled_textgrid.strip(), args.labeled_vot_tier)
        x_xmin = x_xmin + [x.xmin() for x in labeled_vots]
        x_xmax = x_xmax + [x.xmax() for x in labeled_vots]
        x_vot = x_vot + [(x.xmax()-x.xmin()) for x in labeled_vots]

        predicted_vots = read_textgrid_tier(predicted_textgrid.strip(), args.predicted_vot_tier)
        y_xmin = y_xmin + [y.xmin() for y in predicted_vots]
        y_xmax = y_xmax + [y.xmax() for y in predicted_vots]
        y_vot = y_vot + [(y.xmax()-y.xmin()) for y in predicted_vots]

    print "xmin correlation=", correlation(x_xmin, y_xmin)
    print "xmax correlation=", correlation(x_xmax, y_xmax)

    x_vot_ro, y_vot_ro = remove_outliers(x_vot, y_vot)
    print "vot correlation=", correlation(x_vot_ro, y_vot_ro)
