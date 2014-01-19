#! /usr/bin/env python

import argparse
from textgrid import *


if __name__ == "__main__":

    # command line arguments
    parser = argparse.ArgumentParser()
    parser.add_argument("textgrid_filename", help="TextGrid to be examined")
    parser.add_argument("--tier_name", help="optional valid tier name", default='')
    args = parser.parse_args()

    textgrid = TextGrid()

    # read TextGrid
    textgrid.read(args.textgrid_filename)

    # extract tier names
    tier_names = textgrid.tierNames()

    # if there is no argument for tier name to extract print all tier names
    if args.tier_name == '':
        for tier_name in tier_names:
            print tier_name
    else:
        # check if the tier name is in the TextGrid
        if args.tier_name in tier_names:
            tier_index = tier_names.index(args.tier_name)
            # print all its interval, which has some value in their description (mark)
            for interval in textgrid[tier_index]:
                if interval.mark() != '':
                    print interval.xmin(), interval.xmax(), interval.mark()
        else:
            print "The tier '%s' is not found in %s" % (args.tier_name, args.textgrid_filename)
