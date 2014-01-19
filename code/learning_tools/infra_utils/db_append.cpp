//=============================================================================
// File Name: db_append.cpp
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
  string src_filename;
  string trg_filename;

  cmd_line cmdline;
  cmdline.info("Append db file to another db file.");
  cmdline.add_master_option("source_file", &src_filename);
  cmdline.add_master_option("target_file", &trg_filename);
  if ( cmdline.parse(argc, argv) < 1) {
    cmdline.print_help();
    return EXIT_FAILURE;
  }
  
  // open target file
  FILE *target_stream;
  if ( (target_stream=fopen(trg_filename.c_str(), "r+")) == NULL) {
    cerr << "Cannot open target file " << trg_filename  << "." << endl;
    return EXIT_FAILURE;
  }
  
  // read number of vector from target
  unsigned long target_num_vectors;
  rewind(target_stream);
  infra::load_binary(target_stream, target_num_vectors);
  
  // skip to the end of target file
  fseek(target_stream, 0L, SEEK_END);

  // open source file
  FILE *source_stream;
  if ( (source_stream=fopen(src_filename.c_str(), "r")) == NULL) {
    cerr << "Cannot open source file " << src_filename  << "." << endl;
    return EXIT_FAILURE;
  }

  // read number of vector from source
  unsigned long source_num_vectors;
  infra::load_binary(source_stream, source_num_vectors);

  // read all source vectors and write them to target file
  for (unsigned int j=0; j < source_num_vectors; j++) {
    infra::vector tmp(source_stream);
    infra::save_binary(target_stream, tmp);
  }
  
  // append number of vectors to target header
  rewind(target_stream);
  infra::save_binary(target_stream, (target_num_vectors + source_num_vectors) );
  
  // close files
  fclose(source_stream);
  fclose(target_stream);

  return EXIT_SUCCESS;
}
