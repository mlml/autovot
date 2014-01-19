//=============================================================================
// File Name: htk2db.cpp
// Written by: Joseph Keshet (jkeshet@cs.huji.ac.il)
//
// Distributed as part of the HTK utilities
// Copyright (C) 2004 Joseph Keshet
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//=============================================================================
#include <infra.h>
#include <cmdline/cmd_line.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "HtkFile.h"

#define LAST_S 4

using namespace std;
using namespace learning;

int main(int argc, char *argv[])
{
	string htk_filename;
	string htk_stats;
	string out_filename;
	
	cmd_line cmdline;
	cmdline.info("Computes the cepstral distance between all frames. Each line\n"	\
							 "in the output file corresponds to a landmark and the values\n" \
							 "along the columns are the s-distance between the frames\n" \
							 "around the landmark. s-distance is the distance of two\n" \
							 "frames that are 2*s-1 apart around the landmark. s in "	\
							 "{1,2,3,4}");
	cmdline.add_master_option("<htk file>", &htk_filename);
	cmdline.add_master_option("[<htk stats>]", &htk_stats);
	cmdline.add_master_option("<output file>", &out_filename);
	int rc = cmdline.parse(argc, argv);
	if (rc  == 2) {
		out_filename = htk_stats;
		htk_stats = "";
	}
	else if (rc != 3) {
		cmdline.print_help();
		return EXIT_FAILURE;
	}
	
	// load MFCC statistics
	infra::vector mfcc_mean, mfcc_std;
	if (htk_stats != "") {
		std::ifstream mfcc_stats_ifs(htk_stats.c_str());
		if (!mfcc_stats_ifs.good()) {
			cerr << "Warning: Unable to mfcc stats from " << htk_stats << endl;
		}
		else {
			infra::matrix tmp1(mfcc_stats_ifs);
			mfcc_mean.resize(tmp1.width());
			mfcc_mean = tmp1.row(0);
			mfcc_std.resize(tmp1.width());
			mfcc_std = tmp1.row(1);
		}
		mfcc_stats_ifs.close();
	}
	
	// open input htk file
	FILE *instream;
	if ( (instream=fopen(htk_filename.c_str(), "rb")) == NULL) {
		cerr << "Error: Cannot open input file " << htk_filename  << "." << endl;
		return EXIT_FAILURE;
	}
	// read all features from htk file
	HtkFile my_htk(instream);
	if (!my_htk.read_header()) {
		std::cerr << "Error: Unable to read HTK header of " << htk_filename << std::endl;
    return EXIT_FAILURE;
	}
	std::vector<infra::vector> features;
	while ( !feof(instream) ) {
		infra::vector data(my_htk.num_coefs());
		size_t rc = my_htk.read_next_vector(data);
		if ( rc != my_htk.num_coefs() )
			break;
		if (htk_stats != "") {
			for (int j=0; j < int(rc); j++)
				data[j] = (data[j] - mfcc_mean[j])/mfcc_std[j];
		}
		features.push_back(data);
	}
	fclose(instream);
	
	// Open output text file
	ofstream outstream(out_filename.c_str());
	if ( !outstream.good() ) {
		cerr << "Cannot open output file " << out_filename  << "." << endl;
		return EXIT_FAILURE;
	}
	outstream << features.size() << " " << LAST_S << endl;
	
	// Compute all distances. Run over all possible alignment landmarks y_i
	infra::vector w(LAST_S);
	infra::vector x(features[0].size());
	for (int i=0; i < (int)features.size(); i++) {
		for (int s = 1; s <= LAST_S; s++) {
			// define -1 to be the distance of undeined regions
			if (i-s < 0 || i+s-1 >= (int)features.size()) {
				w(s-1) = -1;
			}
			else {
				w(s-1) = 0;
				for (int j=1; j <= s; j++)
					w(s-1) += (features[i-j] - features[i+j-1]).norm2();
				w(s-1) /= s;
			}
			outstream << w(s-1) << " ";
		}
		outstream << endl;
	}
	outstream.close();
	
	return EXIT_SUCCESS;
}



