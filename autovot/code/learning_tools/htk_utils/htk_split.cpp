//=============================================================================
// File Name: htk_split.cpp
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
#include <sstream>


using namespace std;
using namespace learning;

int main(int argc, char *argv[])
{
  string htk_filename;
  unsigned int frame_length;
  unsigned int frame_shift;
  
  cmd_line cmdline;
  cmdline.info("Split an HTK file to several HTK files with a given frame length\n"
               "The generated HTK files will have the extension .1, .2, ...");
  cmdline.add("-l", "number of frames per HTK file", &frame_length, 200);
  cmdline.add("-s", "number of frames to shift between one file and onther", &frame_shift, 100);
  cmdline.add_master_option("HTK_file", &htk_filename);
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
  // read the whole input HTK file
  HtkFile input_htk(instream); 
  if (!input_htk.read_header()) {
		std::cerr << "Error: Unable to read HTK header of " << htk_filename << std::endl;
		return EXIT_FAILURE;
	}
  input_htk.print_header();
  std::vector<float *> htk_data;
  float *data = new float[input_htk.num_coefs()];
  while (!feof(instream)) {
    data = new float[input_htk.num_coefs()];
    input_htk.read_next_vector(data) ;
    htk_data.push_back(data);
  }
  fclose(instream);
  
  // remove the htk extension
  std::string htk_filename2 = htk_filename;
  size_t ext_pos = htk_filename2.find(".htk");
  std::string htk_filename3 = htk_filename2.substr(0,ext_pos);

  // split into output file
  long num_output_file = 0;
  while (num_output_file*frame_shift + frame_length < (unsigned int)input_htk.nSamples()) {
    FILE *outstream;
    std::ostringstream oss;
    oss << htk_filename3 << "." << num_output_file << ".htk";
    if ( (outstream=fopen(oss.str().c_str(), "wb")) == NULL) {
      cerr << "Cannot open output file " << oss.str()  << "." << endl;
      return EXIT_FAILURE;
    }
    HtkFile output_htk(outstream);
    output_htk.header = input_htk.header;
    output_htk.header.nSamples = frame_length;
    output_htk.write_header();
    for (unsigned long i = num_output_file*frame_shift; i < num_output_file*frame_shift + frame_length; i++) {
      data = htk_data[i];
      output_htk.write_next_vector(data);
    }
    fclose(outstream);
    num_output_file++;
  }
  
  // delete all elements from the vectors
  for (unsigned int i = 0; i < htk_data.size(); i++) {
    data = htk_data[i];
    delete [] data;
  }
  
  return EXIT_SUCCESS;
}
