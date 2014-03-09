#! /usr/bin/env python

"""
performance_of_textgrids.py
Author: Joseph Keshet, 18/11/2013
"""

import argparse
import math

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


from itertools import imap
def pearsonr(x, y):
  # Assume len(x) == len(y)
  n = len(x)
  sum_x = float(sum(x))
  sum_y = float(sum(y))
  sum_x_sq = sum(map(lambda x: pow(x, 2), x))
  sum_y_sq = sum(map(lambda x: pow(x, 2), y))
  psum = sum(imap(lambda x, y: x * y, x, y))
  num = psum - (sum_x * sum_y/n)
  den = pow((sum_x_sq - pow(sum_x, 2) / n) * (sum_y_sq - pow(sum_y, 2) / n), 0.5)
  if den == 0: return 0
  return num / den


def mean(x_list):
    x_mean = 0
    for x in x_list:
        x_mean += x
    x_mean /= float(len(x_list))
    return x_mean


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
        print "Error: something's wrong. Didn't read the same number of files from textgrid lists."

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

        # for x1, x2, x3, y1, y2, y3 in zip(x_xmin, x_xmax, x_vot, y_xmin, y_xmax, y_vot):
        #     print x1, y1,
        #     if abs(x1-y1) > 0.1:
        #         print "*",
        #     else:
        #         print ".",
        #     print x2, y2,
        #     if abs(x2-y2) > 0.05:
        #         print "*",
        #     else:
        #         print ".",
        #     print x3, y3,
        #     if abs(x3-y3) > 0.05:
        #         print "*"
        #     else:
        #         print
    print "xmin correlation=", correlation(x_xmin, y_xmin)
    print "xmax correlation=", correlation(x_xmax, y_xmax)
    print "vot correlation=", correlation(x_vot, y_vot)
