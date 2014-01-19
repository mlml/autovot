//=============================================================================
// File Name: db_average_norm_l2.cpp
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

#define MAX_NUM_VECTORS 30000

#define min(a,b) ( (a<b) ? (a) : (b) )

int main(int argc, char *argv[])
{
  string in_db_file;

  cmd_line cmdline;
  cmdline.info("Print out the average norm l2 of vectors in db file.");
  cmdline.add_master_option("db_file>", &in_db_file);
  if ( cmdline.parse(argc, argv) < 1) {
    cmdline.print_help();
    return EXIT_FAILURE;
  }
  
  // open input db file
  FILE *instream;
  if ( (instream=fopen(in_db_file.c_str(), "rb")) == NULL) {
    cerr << "Cannot open input db file " << in_db_file  << "." << endl;
    return EXIT_FAILURE;
  }
  
  double mean_norm2 = 0.0;
  unsigned long num_vectors;
  infra::load_binary(instream, num_vectors);

  for (uint j=0; j < num_vectors; j++) {
    infra::vector instance(instream);
    mean_norm2 += instance.norm2();
  }
  fclose(instream);
  
  std::cout << mean_norm2/double(num_vectors) << std::endl;

  return EXIT_SUCCESS;
}
