//=============================================================================
// File Name: infra2txt.cpp
// Written by: Joseph Keshet (jkeshet@cs.huji.ac.il)
//
// Distributed as part of the infra utilities
// Copyright (C) 2006 Joseph Keshet
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
#include <cmd_line.h>
#include <iostream>
#include <fstream>

using namespace std;
using namespace learning;

int main(int argc, char *argv[])
{
  string infra_filename;
  string txt_filename = "";
  bool print_header;

  cmd_line cmdline;
  cmdline.info("Converts db file to text files. Print result to stdout if no\n"
	       "txt_file is given.");
  cmdline.add_master_option("infra_file", &infra_filename);
  cmdline.add_master_option("[text_file]", &txt_filename);
  cmdline.add("-h", "prints header [optional]", &print_header, false);
  if ( cmdline.parse(argc, argv) < 1) {
    cmdline.print_help();
    return EXIT_FAILURE;
  }
  
  // open input infra file
  FILE *instream;
  if ( (instream=fopen(infra_filename.c_str(), "rb")) == NULL) {
    cerr << "Cannot open input file " << infra_filename  << "." << endl;
    return EXIT_FAILURE;
  }
 
  if (txt_filename == "") {
    unsigned long num_vectors;
    infra::load_binary(instream, num_vectors);

    if (print_header) 
      cout << "num_vectors = " << num_vectors << endl;

    for (unsigned int j=0; j < num_vectors; j++) {
      infra::vector tmp(instream);
      cout << tmp << endl;
    }
    
  }
  else {
    
    // open input text file
    ofstream text_ofs;
    text_ofs.open(txt_filename.c_str());
    if (!text_ofs.good()) {
      cerr << "Cannot open input file " << txt_filename  << "." << endl;
      return EXIT_FAILURE;
    }
    
    unsigned long num_vectors;
    infra::load_binary(instream, num_vectors);
    
    if (print_header) 
      cout << "num_vectors = " << num_vectors << endl;

    for (unsigned int j=0; j < num_vectors; j++) {
      infra::vector tmp(instream);
      text_ofs << tmp << endl;
    }
  } 
  return EXIT_SUCCESS;

}
