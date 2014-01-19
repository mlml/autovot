//=============================================================================
// File Name: db2txt.cpp
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
  string db_filename;
  string txt_filename;

  cmd_line cmdline;
  cmdline.info("Convert db and labels files into a single text file. This utility\n"
	       "assumes that the db file and the label file have the save filename\n"
	       "and their extentions are .db and .labels, respectively.");
  cmdline.add_master_option("db_file(with no extension)", &db_filename);
  cmdline.add_master_option("txt_file", &txt_filename);
  if ( cmdline.parse(argc, argv) < 2) {
    cmdline.print_help();
    return EXIT_FAILURE;
  }
  
  // open input infra file
  std::string instances_filename = db_filename + ".db";
  FILE *instream;
  if ( (instream=fopen(instances_filename.c_str(), "rb")) == NULL) {
    cerr << "Cannot open input file " << instances_filename  << "." << endl;
    return EXIT_FAILURE;
  }
  
  string labels_filename = db_filename + ".labels";
  ifstream labels_ifs(labels_filename.c_str());
  if (!labels_ifs.good()) {
    cerr << "Cannot open output file " << labels_filename  << "." << endl;
    return EXIT_FAILURE;
  }  

  // open input text file
  ofstream text_ofs;
  text_ofs.open(txt_filename.c_str());
  if (!text_ofs.good()) {
    cerr << "Cannot open input file " << txt_filename  << "." << endl;
    return EXIT_FAILURE;
  }
  

  unsigned long num_vectors;
  infra::load_binary(instream, num_vectors);

  uint label;
  for (uint j=0; j < num_vectors; j++) {
    infra::vector tmp(instream);
    labels_ifs >> label;
    text_ofs << label;
    for (uint i=0; i < tmp.size(); i++) {
      text_ofs << " " << tmp(i);
    }
    text_ofs << endl;
  }
  
  return EXIT_SUCCESS;
}
