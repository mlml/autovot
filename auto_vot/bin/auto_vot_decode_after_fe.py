#! /usr/bin/env python

"""
auto_vot_decode_after_fe.py
Author: Joseph Keshet, 18/11/2013
"""

import argparse
from utilities import *


if __name__ == "__main__":

    # parse arguments
    parser = argparse.ArgumentParser(description='Train AutoVOT after features extraction')
    parser.add_argument('features_filename', help="AutoVOT front end features file name (training)")
    parser.add_argument('labels_filename', help="AutoVOT front end labels file name (training)")
    parser.add_argument('model_filename', help="output model file name")
    parser.add_argument('--log_filename', default='', help="log file name")
    parser.add_argument("--debug", help="verbose printing", action='store_const', const=True, default=False)
    args = parser.parse_args()

    # decoding
    cmd_vot_decode = 'InitialVotDecode -pos_only %s %s %s' % (args.features_filename, args.labels_filename,
                                                              args.model_filename)
    easy_call(cmd_vot_decode, verbose=args.debug, log_filename=args.log_filename)
