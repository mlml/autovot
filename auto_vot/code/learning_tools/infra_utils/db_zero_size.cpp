//=============================================================================
// File Name: db_zero_size.cpp
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
  string filename;
  cmd_line cmdline;
  cmdline.info("Creates zero size infra db file.");
  cmdline.add_master_option("source_file", &filename);
  if ( cmdline.parse(argc, argv) < 1) {
    cmdline.print_help();
    return EXIT_FAILURE;
  }
  
  // open target file
  FILE *stream;
  if ( (stream=fopen(filename.c_str(), "w")) == NULL) {
    cerr << "Cannot open target file " << filename  << "." << endl;
    return EXIT_FAILURE;
  }

  unsigned long total_num_vectors = 0;
  infra::save_binary(stream, total_num_vectors);
  
  // close file
  fclose(stream);

  return EXIT_SUCCESS;
}
