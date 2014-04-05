#! /usr/bin/env python

"""
auto_vot_decode_after_fe.py

If features have already been extracted (using
auto_vot_extract_features.py) to files in in features_filename, with
corresponding labels in labels_filename, for a set of stops, use an
existing classifier (model_filename) to measure their VOTs, outputting
simply the average error (abs(predicted - labeled VOT)) when done.

Author: Joseph Keshet, 18/11/2013
"""

import argparse

from autovot.utilities import *


if __name__ == "__main__":

    # parse arguments
    parser = argparse.ArgumentParser(description='Decoding when features have already been extracted')
    parser.add_argument('features_filename', help="AutoVOT front end features filename (training)")
    parser.add_argument('labels_filename', help="AutoVOT front end labels filename (training)")
    parser.add_argument('model_filename', help="Name of classifier to be used to measure VOT")
    parser.add_argument("--logging_level", help="Level of verbosity of information printed out by this program ("
                                                "DEBUG, INFO, WARNING or ERROR), in order of increasing verbosity. "
                                                "See http://docs.python.org/2/howto/logging for definitions. ("
                                                "default: %(default)s)", default="INFO")

    args = parser.parse_args()

    logging_defaults(args.logging_level)

    # decoding
    cmd_vot_decode = 'InitialVotDecode -verbose %s -pos_only %s %s %s' % (args.logging_level, args.features_filename,
                                                                          args.labels_filename, args.model_filename)
    easy_call(cmd_vot_decode)
