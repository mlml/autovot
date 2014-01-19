//=============================================================================
// File Name: db_normalize.cpp
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
  string out_db_file;
  string values_file;

  cmd_line cmdline;
  cmdline.info("Normalize the features in input db file to have zero mean and \n"
	       "unit variance. By default this program estimates the means and the\n"
	       "standard deviations from the input file, unless the -n option is \n"
	       "given.");
  cmdline.add_master_option("<input DB file>", &in_db_file);
  cmdline.add_master_option("<output DB file>", &out_db_file);
  cmdline.add("-n", "pre-calculated estimation of the means and stddevs [optional]", 
	      &values_file, "");
  if ( cmdline.parse(argc, argv) < 2) {
    cmdline.print_help();
    return EXIT_FAILURE;
  }
  
  // open input db file
  FILE *instream;
  if ( (instream=fopen(in_db_file.c_str(), "rb")) == NULL) {
    cerr << "Cannot open input file " << in_db_file  << "." << endl;
    return EXIT_FAILURE;
  }
  
  // Open output db file
  FILE *outstream;
  if ( (outstream=fopen(out_db_file.c_str(), "wb")) == NULL) {
    cerr << "Cannot open output file " << in_db_file  << "." << endl;
    return EXIT_FAILURE;
  }

  unsigned long num_vectors;
  infra::load_binary(instream, num_vectors);

  infra::vector v(instream);
  uint n = v.size();
  double means[n];
  double stddev[n];

  if (values_file == "") {
    // Estimate means and stddev from the input file
    for (uint i=0; i < n; i++) {
      means[i] = 0.0;
      stddev[i] = 0.0;
    }
    
    rewind(instream);
    infra::load_binary(instream, num_vectors);
    
    uint m = min(num_vectors, MAX_NUM_VECTORS);
    for (uint j=0; j < m; j++) {
      infra::vector u(instream);
      for (uint i=0; i < n; i++) {
	means[i] += u[i];
	stddev[i] += u[i]*u[i];
      }
    }

    for (uint i=0; i < n; i++) {
      means[i] /= m;
      stddev[i] = sqrt( stddev[i]/m - means[i]*means[i]);
      cout << i << " " << means[i] << " " << stddev[i] << endl;
    }
    
  }

  else {
    // Read means and stddevs values from file

    ifstream ifs_values(values_file.c_str());
    if (!ifs_values.good()) {
      cerr << "Unable to open values file " << values_file << ". Exiting."
	   << endl;
      return EXIT_FAILURE;
    }

    uint j;
    while (ifs_values.good()) {
      ifs_values >> j;
      ifs_values >> means[j];
      ifs_values >> stddev[j];
    }
    
  }
  
  rewind(instream);
  infra::load_binary(instream, num_vectors);
  infra::save_binary(outstream, num_vectors);

  for (uint j=0; j < num_vectors; j++) {
    infra::vector u(instream);
    for (uint i=0; i < n; i++) {
      u[i] -= means[i];
      u[i] /= stddev[i];
    }
    u.save_binary(outstream);
  }
  
  fclose(instream);
  fclose(outstream);

  return EXIT_SUCCESS;
}
