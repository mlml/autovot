//=============================================================================
// File Name: htk_split_from_phn.cpp
// Written by: Joseph Keshet (jkeshet@ttic.edu)
//
// Distributed as part of the HTK utilities
// Copyright (C) 2012 Joseph Keshet
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
#include "HtkFile.h"
#include <cmdline/cmd_line.h>
#include <iostream>
#include <fstream>
#include <sstream>


using namespace std;
using namespace learning;

int main(int argc, char *argv[])
{
  string silence_symbol;
  string in_htk_filename;
  string in_phn_filename;
  string out_htk_filename;
  string out_phn_filename;
	bool verbose;
  
  cmd_line cmdline;
  cmdline.info("Split an HTK file to several HTK files based on the events in PHN file\n"
               "The generated HTK files will have the extension .1, .2, ...");
  cmdline.add("-v", "verbose", &verbose, false);
  cmdline.add_master_option("input_HTK_file", &in_htk_filename);
  cmdline.add_master_option("input_PHN_file", &in_phn_filename);
  cmdline.add_master_option("out_htk_file", &out_htk_filename);
  if ( cmdline.parse(argc, argv) < 3) {
    cmdline.print_help();
    return EXIT_FAILURE;
  }
  
  // open input HTK file
  FILE *instream;
  if ( (instream=fopen(in_htk_filename.c_str(), "rb")) == NULL) {
    cerr << "Error: Cannot open input file " << in_htk_filename  << "." << endl;
    return EXIT_FAILURE;
  }
  // read the whole input HTK file
  HtkFile in_htk(instream);
  if (!in_htk.read_header()) {
		std::cerr << "Error: Unable to read HTK header of " << in_htk_filename << std::endl;
		return EXIT_FAILURE;
	}
	if (verbose)
		in_htk.print_header();
  std::vector<float *> data;
  float *frame_data = new float[in_htk.num_coefs()];
  while (!feof(instream)) {
    frame_data = new float[in_htk.num_coefs()];
    in_htk.read_next_vector(frame_data) ;
    data.push_back(frame_data);
  }
  fclose(instream);
  
  // read labels file
  std::vector<int> start_times;
  std::vector<int> end_times;
  std::vector<std::string> phonemes;
  
  std::ifstream ifs_phn(in_phn_filename.c_str());
  if (!ifs_phn.good()) {
    std::cerr << "Error: Unable to open labels file " << in_phn_filename << std::endl;
    return EXIT_FAILURE;
  }
  while (ifs_phn.good()) {
    float start_time;
    float end_time;
    std::string phoneme;
    ifs_phn >> start_time; // time units [seconds]
    ifs_phn >> end_time; // time units [seconds]
    ifs_phn >> phoneme;
    if (phoneme == "") break;
    start_times.push_back(int(floor(start_time*100+0.5))); // convert seconds into 10msec frame rate
    end_times.push_back(int(floor(end_time*100+0.5))); // convert seconds into 10msec frame rate
    phonemes.push_back( phoneme );
//		cout << start_time << " " << end_time << " " << phoneme << endl;
//		cout << (start_time*100) << " " << (end_time*100) << " " << phoneme << endl;
//		cout << floor(start_time*100+0.5) << " " << floor(end_time*100+0.5) << " " << phoneme << endl;
//		cout << start_times[start_times.size()-1] << " " << end_times[start_times.size()-1] << " " << phonemes[start_times.size()-1] << endl;
  }
  ifs_phn.close();
  
	// run over all events in the inut PHN file
	for (int i=0; i < int(start_times.size()); i++) {
		std::ostringstream oss_htk_filename;
		oss_htk_filename <<  out_htk_filename << "." << i+1;
		// open output HTK file
		FILE *outstream;
		if ( (outstream=fopen(oss_htk_filename.str().c_str(), "wb")) == NULL) {
			cerr << "Cannot open output file " << oss_htk_filename.str()  << "." << endl;
			return EXIT_FAILURE;
		}
		HtkFile out_htk(outstream);
		out_htk.header = in_htk.header;
		out_htk.header.nSamples = end_times[i]-start_times[i];
		out_htk.write_header();
		
		int actual_num_frames = 0;
		for (int t=start_times[i]; t < end_times[i]; t++) {
			frame_data = data[t];
			out_htk.write_next_vector(frame_data);
			actual_num_frames++;
		}
		fclose(outstream);
		if (verbose) {
			std::cout << oss_htk_filename.str() << " includes " << actual_num_frames << " frames ("
			<< start_times[i] << "-" << end_times[i]-1 << ")." << std::endl;
		}
	}


// delete all elements from the vectors
for (unsigned int i = 0; i < data.size(); i++) {
	frame_data = data[i];
	delete [] frame_data;
}

return EXIT_SUCCESS;
}
