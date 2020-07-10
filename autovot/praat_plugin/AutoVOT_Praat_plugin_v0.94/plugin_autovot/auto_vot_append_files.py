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
# auto_vot_append_files.py : Append set of features and labels


import argparse

from helpers.utilities import *

if __name__ == "__main__":
    # parse arguments
    parser = argparse.ArgumentParser(description='Append set of features and labels')
    parser.add_argument('features_filename', help="front end features filename")
    parser.add_argument('labels_filename', help="front end labels filename")
    parser.add_argument('appended_features_filename', help="front end features filename to be appended")
    parser.add_argument('appended_labels_filename', help="front end labels filename to be appended")
    parser.add_argument("--logging_level", help="Level of verbosity of information printed out by this program ("
                                                "DEBUG, INFO, WARNING or ERROR), in order of increasing verbosity. "
                                                "See http://docs.python.org/2/howto/logging for definitions. ("
                                                "default: %(default)s)", default="INFO")

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
        logging.error("%s and %s are not of the same length or %s is missing a header" % (args.features_filename,
                                                                                          args.labels_filename,
                                                                                          args.labels_filename))
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
            logging.error("Something wrong with opening %s and %s for reading." % (args.appended_features_filename,
                                                                                   args.appended_labels_filename))

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
