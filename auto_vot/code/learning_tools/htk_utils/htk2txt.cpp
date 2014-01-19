//=============================================================================
// File Name: htk2txt.cpp
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
#include "HtkFile.h"
#include <cmdline/cmd_line.h>
#include <iostream>
#include <fstream>


using namespace std;
using namespace learning;

int main(int argc, char *argv[])
{
  bool print_header;
  bool print_num_frames;
  string htk_filename;
  string txt_filename;
  ofstream txt_file;
  
  cmd_line cmdline;
  cmdline.info("Convert HTK file to text file. Print result to stdout if no\n"
               "txt_file is given.");
  cmdline.add("-h", "prints HTK header to stdout [optional]", &print_header, false);
  cmdline.add("-n", "prints number of frames in the file and exits.", &print_num_frames, false);
  cmdline.add_master_option("HTK_file", &htk_filename);
  cmdline.add_master_option("[txt_file]", &txt_filename);
  if ( cmdline.parse(argc, argv) < 1) {
    cmdline.print_help();
    return EXIT_FAILURE;
  }
  
  // open input HTK file
  FILE *instream;
  if ( (instream=fopen(htk_filename.c_str(), "rb")) == NULL) {
    cerr << "Error: Cannot open input file " << htk_filename  << "." << endl;
    return EXIT_FAILURE;
  }
  
  // open output text file
  if (txt_filename != "") {
    txt_file.open (txt_filename.c_str(), ios::out | ios::trunc);
    if (!txt_file.good()) {
      cerr << "Cannot open output file " << txt_filename  << "." << endl;
      return EXIT_FAILURE;
    }
  }
  
  HtkFile my_htk(instream); 
	if (!my_htk.read_header()) {
		std::cerr << "Error: Unable to read HTK header of " << htk_filename << std::endl;
		return EXIT_FAILURE;
	}
  if (print_num_frames) {
    cout << my_htk.header.nSamples << endl;
    exit(EXIT_SUCCESS);
  }
  if (print_header) my_htk.print_header();
  float *data;
  data = new float[my_htk.num_coefs()];
  
  while ( my_htk.read_next_vector(data) == my_htk.num_coefs() ) {
    for (unsigned int j=0; j < my_htk.num_coefs(); j++) {
      if (txt_filename != "") 
        txt_file << data[j] << " ";
      else 
        cout << data[j] << " ";
    }
    if (txt_filename != "") 
      txt_file << endl;
    else
      cout << endl;
  }
  
  delete [] data;
  
  return EXIT_SUCCESS;
}
