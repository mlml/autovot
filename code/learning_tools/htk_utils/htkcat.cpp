//=============================================================================
// File Name: htkcat.cpp
// Written by: Joseph Keshet (jkeshet@cs.huji.ac.il)
//
// Distributed as part of the HTK utilities
// Copyright (C) 2008 Joseph Keshet
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


using namespace std;
using namespace learning;

int main(int argc, char *argv[])
{
  bool print_header;
  string in_htk_filename;
  string in_labels_filename;
  string out_htk_filename;
  string out_labels_filename;
  uint cat_frames;
  
  cmd_line cmdline;
  cmdline.info("Concatenate several features vectors and arrange their corresponding\n"
               "frame labels. This utility assumes that the labels are given for\n"
               "each speech frame.\n");
  cmdline.add("-h", "prints HTK header to stdout", &print_header, false);
  cmdline.add("-n", "num. frames to concate from each side", &cat_frames, 0);
  cmdline.add_master_option("<input htk file>", &in_htk_filename);
  cmdline.add_master_option("<input lab list>", &in_labels_filename);
  cmdline.add_master_option("<output htk file>", &out_htk_filename);
  cmdline.add_master_option("<output lab file>", &out_labels_filename);
  if ( cmdline.parse(argc, argv) < 4) {
    cmdline.print_help();
    return EXIT_FAILURE;
  }
  
  // open input HTK file
  FILE *instream;
  if ( (instream=fopen(in_htk_filename.c_str(), "rb")) == NULL) {
    cerr << "Error: Cannot open input file " << in_htk_filename  << "." << endl;
    return EXIT_FAILURE;
  }
  
  // open the label file 
  ifstream ifs_labels(in_labels_filename.c_str());
  if (!ifs_labels.good()) {
    cerr << "Error: unable to open file: " << in_labels_filename << endl;
    return EXIT_FAILURE;
  }
  
  
  // open output HTK file
  FILE *outstream;
  if ( (outstream=fopen(out_htk_filename.c_str(), "wb")) == NULL) {
    cerr << "Cannot open input file " << out_htk_filename  << "." << endl;
    return EXIT_FAILURE;
  }
  
  // open the label file
  ofstream ofs_labels(out_labels_filename.c_str());
  if (!ofs_labels.good()) {
    cerr << "Error: unable to open file: " << out_labels_filename << endl;
    return EXIT_FAILURE;
  }

  // read the whole input HTK file
  HtkFile in_htk(instream); 
  if (!in_htk.read_header()) {
		std::cerr << "Error: Unable to read HTK header of " << in_htk_filename << std::endl;
		return EXIT_FAILURE;
	}
  if (print_header) in_htk.print_header();
  vector<float *> htk_data;
  vector<string> lab_data;
  float *data;
  string label;
  while (!feof(instream)) {
    data = new float[in_htk.num_coefs()];
    in_htk.read_next_vector(data) ;
    htk_data.push_back(data);
    if (!ifs_labels.good()) {
      cerr << "Error: labels and HTK file do not have the same number of frames."
      << endl;
      return EXIT_FAILURE;
    }
    ifs_labels >> label;
    lab_data.push_back(label);
  }
  fclose(instream);
  ifs_labels.close();
  
  // write data to output HTK file
  HtkFile out_htk(outstream);
  out_htk.header.nSamples = in_htk.header.nSamples-(2*cat_frames);
  out_htk.header.sampPeriod = in_htk.header.sampPeriod;
  out_htk.header.sampSize = in_htk.header.sampSize*(2*cat_frames+1);
  out_htk.header.parmKind = 9; // USER
  out_htk.write_header(); // write temporary header
  for (uint i = cat_frames; i < in_htk.header.nSamples-cat_frames; i++) {
    for (uint j = i - cat_frames; j <= i + cat_frames; j++) {
      out_htk.header.sampSize = in_htk.header.sampSize;
      out_htk.write_next_vector(htk_data[j]);
    }
    ofs_labels << lab_data[i] << endl;
  }
  
  // delete all elements from the vectors
  for (unsigned int i = 0; i < htk_data.size(); i++) {
    data = htk_data[i];
    delete [] data;
  }
  
  fclose(outstream);
  ofs_labels.close();

  return EXIT_SUCCESS;
}
