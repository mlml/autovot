#! /usr/bin/env python

"""
performance_of_textgrids.py
Author: Joseph Keshet, 18/11/2013
"""

import argparse
from autovot.textgrid import *
import math


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


if __name__ == "__main__":
    # parse arguments
    parser = argparse.ArgumentParser(description='Compute the performance between a set of predicted textgrid files '
                                                 'and a set of labeled textgrid files.')
    parser.add_argument('labeled_textgrid_list', help="list of manually labeled TextGrid files")
    parser.add_argument('predicted_textgrid_list', help="list of predicted TextGrid files")
    parser.add_argument('labeled_vot_tier', help='name of the VOT tier of the manual labeled TextGrids',
                        default='vot')
    parser.add_argument('predicted_vot_tier', help='name of the VOT tier to predicted TetGrids', default='vot')
    parser.add_argument("--debug", help="verbose printing", action='store_const', const=True, default=False)
    args = parser.parse_args()

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

    labeled_vots = list()
    predicted_vots = list()
    for labeled_textgrid, predicted_textgrid in zip(labeled_textgrids, predicted_textgrids):
        labeled_vots = read_textgrid_tier(labeled_textgrid.strip(), args.labeled_vot_tier)
        predicted_vots = read_textgrid_tier(predicted_textgrid.strip(), args.predicted_vot_tier)
        for (x,y) in zip(labeled_vots, predicted_vots):
            print x.mark(), x.xmax()-x.xmin(), y.mark(), y.xmax()-y.xmin(),
            if abs(x.xmax()-x.xmin() - y.xmax()+y.xmin()) > 0.01:
                print "*"
            else:
                print
        # compute means
        mean_labeled = 0
        mean_predicted = 0
        for x, y in zip(labeled_vots, predicted_vots):
            mean_labeled += x.xmax()-x.xmin()
            mean_predicted += y.xmax()-y.xmin()
        mean_predicted /= float(len(labeled_vots))
        mean_labeled /= float(len(predicted_vots))

        #print mean_labeled, mean_predicted
        print "----------------------------------------------------"

        # compute correlation
        sum_xy = 0
        sum_x2 = 0
        sum_y2 = 0
        for x, y in zip(labeled_vots, predicted_vots):
            x_vot = x.xmax()-x.xmin()
            y_vot = y.xmax()-y.xmin()
            if abs(x_vot - y_vot) > 0.01:
                print "differences are burst=%f voicing=%f vot=%f conf. %s" % (abs(x.xmin()-y.xmin()), abs(x.xmax()-y
                .xmax()), abs(x_vot - y_vot), y.mark())
                continue
            sum_xy += (x_vot-mean_labeled)*(y_vot-mean_predicted)
            sum_x2 += (x_vot-mean_labeled)*(x_vot-mean_labeled)
            sum_y2 += (y_vot-mean_labeled)*(y_vot-mean_labeled)

        corr = sum_xy/math.sqrt(sum_x2*sum_y2)

        print "correlation=", corr
        print "----------------------------------------------------"
