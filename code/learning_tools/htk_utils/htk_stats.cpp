//=============================================================================
// File Name: htk_stats.cpp
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

#include <cmdline/cmd_line.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include "HtkFile.h"

using namespace std;
using namespace learning;


int main(int argc, char *argv[])
{
  string file_list;
  cmd_line cmdline;
  cmdline.info("Computes the mean and the variance of each of the feature in the given\n"
               "list of HTK files.");
  cmdline.add_master_option("htk-file-list", &file_list);
  if ( cmdline.parse(argc, argv) < 1) {
    cmdline.print_help();
    return EXIT_FAILURE;
  }
  
  // open HTK file list
  ifstream file_list_ifs(file_list.c_str());
  if (!file_list_ifs.good()) {
    cerr << "Error: Unable to open file list " << file_list << endl;
    return EXIT_FAILURE;
  }
  
  // read first file name from stream
  string htk_filename;    
  file_list_ifs >> htk_filename;
  
  // open file
  FILE *htk_stream;
  if ( (htk_stream = fopen(htk_filename.c_str(),"r")) == NULL) {
    cerr << "Error: Cannot open HTK file " << htk_filename  << "." << endl;
    return EXIT_FAILURE;
  }
  
  // read header and data from first HTK
  HtkFile my_htk(htk_stream); 
  if (!my_htk.read_header()) {
		std::cerr << "Error: Unable to read HTK header of " << htk_filename << std::endl;
		return EXIT_FAILURE;
	}
  int num_coefs = my_htk.num_coefs();
  int num_vectors = 0;
  float data[num_coefs];
  
  // construct means and variance accomulators
  float means[num_coefs];
  float stddev[num_coefs];
  for (int i=0; i < num_coefs; i++) {
    means[i] = 0; 
    stddev[i] = 0;
  }
  
  // update data of the first file
  for (int j=0; j < my_htk.nSamples(); j++) {
    my_htk.read_next_vector(data);
    num_vectors++;
    for (int i=0; i < num_coefs; i++) {
      means[i] = (data[i] + (num_vectors-1)*means[i])/float(num_vectors); 
      stddev[i] = (data[i]*data[i] + (num_vectors-1)*stddev[i])/float(num_vectors) ;
    }
  }
  
  // close file
  fclose(htk_stream);
  
  // loop over the rest of files in the list
  while (file_list_ifs.good()) {
    // read file name from stream
    string htk_filename;    
    file_list_ifs >> htk_filename;
    if (htk_filename == "") continue;
    // open file
    FILE *htk_stream;
    if ( (htk_stream = fopen(htk_filename.c_str(),"r")) == NULL) {
      cerr << "Error: Cannot open HTK file " << htk_filename  << "." << endl;
      return EXIT_FAILURE;
    }
    
    // read header and data from HTK file
    HtkFile my_htk(htk_stream); 
    my_htk.read_header();
    my_htk.read_next_vector(data);
    
    // update data of the first file
    for (int j=0; j < my_htk.nSamples(); j++) {
      my_htk.read_next_vector(data);
      num_vectors++;
      for (int i=0; i < num_coefs; i++) {
        means[i] = (data[i] + (num_vectors-1)*means[i])/float(num_vectors); 
        stddev[i] = (data[i]*data[i] + (num_vectors-1)*stddev[i])/float(num_vectors) ;
      }
    }
    
    // close file
    fclose(htk_stream);
  }
  
  // close file
  file_list_ifs.close();
  
  cout << "2 " << num_coefs << endl;
  for (int i=0; i < num_coefs; i++) 
    cout << means[i] << " ";
  cout << endl;
  for (int i=0; i < num_coefs; i++) {
    stddev[i] = sqrt( stddev[i] - means[i]*means[i]);
    cout << stddev[i] << " ";
  }
  cout << endl;
  
  return EXIT_SUCCESS;
}
