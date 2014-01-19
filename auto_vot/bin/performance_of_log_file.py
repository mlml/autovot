#! /usr/bin/env python


import argparse
import math


if __name__ == "__main__":
    # parse arguments
    parser = argparse.ArgumentParser(description='Compute the Correlation between the predicated VOT and the manually '
                                     'labeled VOT by grepping the log file')
    parser.add_argument('log_filename', help="log of decoding")
    args = parser.parse_args()

    # open files
    log_file = open(args.log_filename)

    predicted = list()
    labeled = list()
    for line in log_file:
        if "Predicted VOT" in line:
            items = line.split()
            predicted.append(int(items[3])-int(items[2]))
            #print "predicted", int(items[3]), int(items[2]), int(items[3])-int(items[2])
        if "Labeled VOT" in line:
            items = line.split()
            labeled.append(int(items[3])-int(items[2]))
            #print "labeled", int(items[3]), int(items[2]), int(items[3])-int(items[2])

    # close files
    log_file.close()

    # compute means
    mean_labeled = 0
    mean_predicted = 0
    for x, y in zip(labeled, predicted):
        mean_labeled += x
        mean_predicted += y
    mean_predicted /= float(len(labeled))
    mean_labeled /= float(len(predicted))

    #print mean_labeled, mean_predicted

    # compute correlation
    sum_xy = 0
    sum_x2 = 0
    sum_y2 = 0
    for x, y in zip(labeled, predicted):
        sum_xy += (x-mean_labeled)*(y-mean_predicted)
        sum_x2 += (x-mean_labeled)*(x-mean_labeled)
        sum_y2 += (y-mean_predicted)*(y-mean_predicted)

    corr = sum_xy/math.sqrt(sum_x2*sum_y2)

    print "correlation=", corr