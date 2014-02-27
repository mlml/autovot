#! /usr/bin/env python

"""
auto_vot_decode_after_fe.py
Author: Joseph Keshet, 18/11/2013
"""

import argparse
from autovot.utilities import *


if __name__ == "__main__":

    # parse arguments
    parser = argparse.ArgumentParser(description='Train AutoVOT after features extraction')
    parser.add_argument('features_filename', help="AutoVOT front end features file name (training)")
    parser.add_argument('labels_filename', help="AutoVOT front end labels file name (training)")
    parser.add_argument('model_filename', help="output model file name")
    parser.add_argument("--logging_level", help="print out level (DEBUG, INFO, WARNING or ERROR)", default="INFO")
    args = parser.parse_args()

    logging_defaults(args.logging_level)

    # decoding
    cmd_vot_decode = 'InitialVotDecode -verbose %s -pos_only %s %s %s' % (args.logging_level, args.features_filename,
                                                                          args.labels_filename, args.model_filename)
    easy_call(cmd_vot_decode)
