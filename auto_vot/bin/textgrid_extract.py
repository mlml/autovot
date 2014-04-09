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
