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
#include "HtkFile.h"
#include <cmdline/cmd_line.h>
#include <infra.h>
#include <iostream>
#include <fstream>
#include <deque>
#include <string.h>

using namespace std;
using namespace learning;

int main(int argc, char *argv[])
{
  bool print_header;
  string htk_filename;
  string out_filename;
  unsigned int num_multiframes;
  size_t rc = 0;
    
  cmd_line cmdline;
  cmdline.info("Convert HTK file to DB file format.");
  cmdline.add("-h", "prints HTK header [optional]", &print_header, false);
  cmdline.add("-m", "number multiframes [1]", &num_multiframes, 1);
  cmdline.add_master_option("HTK_file", &htk_filename);
  cmdline.add_master_option("db_file", &out_filename);
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
  FILE *outstream;
  if ( (outstream=fopen(out_filename.c_str(), "wb")) == NULL) {
    cerr << "Cannot open output file " << out_filename  << "." << endl;
    return EXIT_FAILURE;
  }

  HtkFile my_htk(instream); 
  if (!my_htk.read_header()) {
		std::cerr << "Error: Unable to read HTK header of " << htk_filename << std::endl;
		return EXIT_FAILURE;
	}
  if (print_header) {
    my_htk.print_header();
    cout << "num_coefs = " << my_htk.num_coefs() << endl;
  }
  // initiate cyclic buffer
  deque<double *> cyclic_buffer;
  deque<double *>::iterator I;
  for (unsigned int i = 0; i < num_multiframes-1; i++) {
    double *data = new double[my_htk.num_coefs()];    
    rc = my_htk.read_next_vector(data);
    if ( rc != my_htk.num_coefs() ) {
      return EXIT_FAILURE;
    }
    cyclic_buffer.push_back(data);
  }
  unsigned long num_multiframes_saved = 0;

  // write temporary header
  infra::save_binary(outstream, num_multiframes_saved);

  // write features vectors
  double *multidata = new double[my_htk.num_coefs()*num_multiframes];
  while ( !feof(instream) ) {
    double *data = new double[my_htk.num_coefs()];    
    rc = my_htk.read_next_vector(data);
    if ( rc != my_htk.num_coefs() ) {
      delete [] data;
      break;
    }
    cyclic_buffer.push_back(data);
    for (uint i = 0; i < num_multiframes; i++) {
      memcpy(multidata + i*my_htk.num_coefs(), (double *)cyclic_buffer[i], 
	     my_htk.num_coefs()*sizeof(double));
    }
    infra::save_binary(outstream,multidata,my_htk.num_coefs()*num_multiframes);
    num_multiframes_saved++;
    // remove last element
    double *tmp_data = cyclic_buffer.front();
    delete [] tmp_data;
    cyclic_buffer.pop_front();
  }

  // update header
  rewind(outstream);
  infra::save_binary(outstream, num_multiframes_saved);

  // close file
  fclose(outstream);

  // clean memory
  for (I = cyclic_buffer.begin(); I != cyclic_buffer.end(); I++)
    delete [] (*I);

  delete [] multidata;

  return EXIT_SUCCESS;
}
