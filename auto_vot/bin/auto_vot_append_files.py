#! /usr/bin/env python

"""
concate_feature_and_labels_files.py
Author: Joseph Keshet, 18/11/2013
"""

import argparse
import sys
from os.path import basename 
from autovot.utilities import *

if __name__ == "__main__":
    # parse arguments
    parser = argparse.ArgumentParser(description='Append set of features and labels')
    parser.add_argument('features_filename', help="front end features file name")
    parser.add_argument('labels_filename', help="front end labels file name")
    parser.add_argument('appended_features_filename', help="front end features file name to be appended")
    parser.add_argument('appended_labels_filename', help="front end labels file name to be appended")
    parser.add_argument("--logging_level", help="print out level (DEBUG, INFO, WARNING or ERROR)", default="INFO")
    args = parser.parse_args()

    logging_defaults(args.logging_level)

    # open files
    in_features = open(args.features_filename)
    in_labels = open(args.labels_filename)

    # read infra text header
    header = in_labels.readline()
    dims = header.split()

    # read file lines
    lines = list()
    for x, y in zip(in_features, in_labels):
        lines.append((x, y))

    # close files
    in_features.close()
    in_labels.close()

    if len(lines) != int(dims[0]):
        logging.error("%s and %s are not of the same length or %s is missing a header" %
            (args.features_filename, args.labels_filename, args.labels_filename))
        exit(-1)

    try:
        # try to open the files
        app_features = open(args.appended_features_filename, 'r')
        app_labels = open(args.appended_labels_filename, 'r')

        # now read the appended files
        app_features = open(args.appended_features_filename, 'r')
        app_labels = open(args.appended_labels_filename, 'r')

        # read infra text header
        app_header = app_labels.readline()
        app_dims = app_header.split()

        # read file to lines
        for x, y in zip(app_features, app_labels):
            lines.append((x, y))

        # close files
        in_features.close()
        in_labels.close()

        # assert header
        if len(lines) != int(dims[0])+int(app_dims[0]):
            logging.error("Something wrong with the header of %s" % args.appended_labels_filename)
            exit(-1)
    
    except Exception as exception:
        if exception.errno != 2:
            logging.error("Something wrong with opening %s and %s for reading." % \
                (args.appended_features_filename, args.appended_labels_filename))

    # open appended files for writing
    out_features = open(args.appended_features_filename, 'w')
    out_labels = open(args.appended_labels_filename, 'w')

    # write labels header
    header = "%d 2\n" % len(lines)
    out_labels.write(header)

    # write data
    for x, y in lines:
        out_features.write(x)
        out_labels.write(y)

    # close files
    out_features.close()
    out_labels.close()
