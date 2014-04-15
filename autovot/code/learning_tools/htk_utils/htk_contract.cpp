//=============================================================================
// File Name: htk_contract.cpp
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
  cmdline.info("Remove silence frames from HTK file");
  cmdline.add("-v", "verbose", &verbose, false);
  cmdline.add_master_option("silence_symbol", &silence_symbol);
  cmdline.add_master_option("input_HTK_file", &in_htk_filename);
  cmdline.add_master_option("input_PHN_file", &in_phn_filename);
  cmdline.add_master_option("out_htk_file", &out_htk_filename);
  cmdline.add_master_option("output_PHN_file", &out_phn_filename);
  if ( cmdline.parse(argc, argv) < 5) {
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
  for (int i = 0; i < in_htk.header.nSamples; i++) {
		if (feof(instream)) break;
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
    start_times.push_back(int(start_time*100)); // convert seconds into 10msec frame rate
    end_times.push_back(int(end_time*100)); // convert seconds into 10msec frame rate
    phonemes.push_back( phoneme );
  }
  ifs_phn.close();
  
  // calculate how many frames should we remove
	int actual_num_frames = 0;
  for (int i=0; i < int(phonemes.size()); i++) {
    if (phonemes[i] != silence_symbol) {
      for (int t=start_times[i]; t < end_times[i]; t++) {
				actual_num_frames++;
      }
    }
  }

	if (actual_num_frames == 0) {
		if (1)
			std::cout << "Warning: All phonemes are in the input PHN are siences. HTK was not generated" << std::endl;
		return EXIT_SUCCESS;
	}
	
  // open output HTK file
  FILE *outstream;
  if ( (outstream=fopen(out_htk_filename.c_str(), "wb")) == NULL) {
    cerr << "Error: Cannot open output file " << out_htk_filename  << "." << endl;
    return EXIT_FAILURE;
  }
  HtkFile out_htk(outstream);
  out_htk.header = in_htk.header;
  //out_htk.header.nSamples = in_htk.header.nSamples - num_frames_to_delete;
  out_htk.header.nSamples = actual_num_frames;
  out_htk.write_header();
  
  // open output labels file
  std::ofstream ofs_phn(out_phn_filename.c_str());
  if (!ofs_phn.good()) {
    std::cerr << "Error: Unable to open labels file " << out_phn_filename << std::endl;
    return EXIT_FAILURE;
  }
  
  // run over all input phonemes
  int last_end_time = 0;
  for (int i=0; i < int(phonemes.size()); i++) {
    if (phonemes[i] != silence_symbol) {
      // write data
      for (int t=start_times[i]; t < end_times[i]; t++) {
        frame_data = data[t];
        out_htk.write_next_vector(frame_data);
      }
      // write labels
      ofs_phn << float(last_end_time)/100.0 << " " << float(last_end_time + end_times[i]-start_times[i])/100.0 << " " << phonemes[i] << std::endl;
      last_end_time = last_end_time + end_times[i]-start_times[i];
    }
  }
  fclose(outstream);

	if (verbose)
		std::cout << "Read " << data.size() << " frames. Wrote " << actual_num_frames << " frames to HTK file." << std::endl;
	
  // delete all elements from the vectors
  for (unsigned int i = 0; i < data.size(); i++) {
    frame_data = data[i];
    delete [] frame_data;
  }
  
  return EXIT_SUCCESS;
}
