//=============================================================================
// File Name: htk_shuffle.cpp
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

//*****************************************************************************
// Included Files
//*****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmdline/cmd_line.h>
#include "HtkFile.h"

using namespace std;
using namespace learning;


//----------------------------------------------------------------------------
// fill the vector with a random permutation of 0,...,size-1
void rand_perm(vector<unsigned long>& v) {

  for(unsigned long i=0; i< v.size(); ++i) {
    v[i] = i;
  }

  for(unsigned long i=0; i< v.size(); ++i) {
    unsigned long selected = i + rand() % (v.size() - i);
    unsigned long tmp = v[selected];
    v[selected] = v[i];
    v[i] = tmp;
  }
}


//----------------------------------------------------------------------------
// the main
int main(int argc,char* argv[]) 
{
  string input_htk_filename, input_target_filename;
  string output_htk_filename, output_target_filename;
  bool remove_silences;
  string silence_symbol;
	bool dump_mode;
  
  cmd_line cmdline;
  cmdline.info("Create new HTK file and a new label file from a given list of HTK files and labels.\n"
							 "This utility assumes that the labels are given for each speech frame.\n"
               "The resulted files contains the same speech frames, but random shuffled.\n");
  cmdline.add("-remove_silences", "removes the silence at the beginning and end of each file", 
              &remove_silences, false);
  cmdline.add("-silence_symbol", "silence symbol [sil]", &silence_symbol, "sil");
  cmdline.add("-dump", "the shuffle is not performed", &dump_mode, false);
  cmdline.add_master_option("input_htk_list", &input_htk_filename);
  cmdline.add_master_option("input_target_list", &input_target_filename);
  cmdline.add_master_option("output_htk_file", &output_htk_filename);
  cmdline.add_master_option("output_target_file", &output_target_filename);
  if ( cmdline.parse(argc, argv) < 4) {
    cmdline.print_help();
    return EXIT_FAILURE;
  }

  // init srand
  srand(time(NULL));
  
  // open the HTK file list
  std::ifstream ifs_htk_list(input_htk_filename.c_str());
  if (!ifs_htk_list.good()) {
    std::cerr << "Error: unable to open file: " << input_htk_filename << std::endl;
    return EXIT_FAILURE;
  }
  // open the target file list
  std::ifstream ifs_target_list(input_target_filename.c_str());
  if (!ifs_target_list.good()) {
    std::cerr << "Error: unable to open file: " << input_target_filename << std::endl;
    return EXIT_FAILURE;
  }

  // open htk ouput file
  FILE *htk_output;
  if ( (htk_output = fopen(output_htk_filename.c_str(),"w")) == NULL) {
    cerr << "Error: Cannot open HTK file " << output_htk_filename  << "." << endl;
    return EXIT_FAILURE;
  }
  HtkFile my_htk_output(htk_output);
  
  // open the target file list
  std::ofstream ofs_target_list(output_target_filename.c_str());
  if (!ofs_target_list.good()) {
    std::cerr << "Error: unable to open file: " << output_target_filename << std::endl;
    return EXIT_FAILURE;
  }
  
  // data structure to store all data
  vector<float *> features;
  vector<string> targets;
  vector<float *> file_features;
  vector<string> file_targets;
  float *data;
  // read all features and targets
  while (ifs_htk_list.good() && ifs_target_list.good()) {
    // read file name from stream
    string htk_filename;    
    ifs_htk_list >> htk_filename;
    if (htk_filename == "") continue;
    cout << htk_filename << endl;
    // open file
    FILE *htk_input;
    if ( (htk_input = fopen(htk_filename.c_str(),"r")) == NULL) {
      cerr << "Error: Cannot open HTK file " << htk_filename  << "." << endl;
      return EXIT_FAILURE;
    }
    // read header and data
    HtkFile my_htk(htk_input);
		if (!my_htk.read_header()) {
			std::cerr << "Error: Unable to read HTK header of " << htk_filename << std::endl;
			return EXIT_FAILURE;
		}
    //my_htk.print_header();
    uint num_coefs = my_htk.num_coefs();
    my_htk_output.header.sampPeriod = my_htk.header.sampPeriod;
    my_htk_output.header.sampSize = my_htk.header.sampSize;
    my_htk_output.header.parmKind = my_htk.header.parmKind;
    // read file name from stream
    string target_filename;    
    ifs_target_list >> target_filename;
    ifstream target_ifs(target_filename.c_str());
    if (!target_ifs.good()) {
      cerr << "Error: unable to open " << target_filename << "." << endl;
      return EXIT_FAILURE;
    }
    file_features.clear();
    file_targets.clear();
    for (int j=0; j < my_htk.nSamples(); j++) {
      data = new float[num_coefs];
      my_htk.read_next_vector(data);
      if (!target_ifs.good()) {
        cerr << "Error: unable to read from " << target_filename << endl;
        return EXIT_FAILURE;
      }
      string target;
      target_ifs >> target;
      file_features.push_back(data);
      file_targets.push_back(target);
    }
    fclose(htk_input);
    target_ifs.close();
    unsigned int first = 0 ;
    unsigned int last = file_targets.size()-1;
    if (remove_silences) {
      while (file_targets[first++] == silence_symbol) continue;
      while (file_targets[last--] == silence_symbol) continue;
      first--; last++;
      //cout << "first=" << first << " last=" << last << endl;
    }
    for (unsigned int k=first; k <= last; k++) {
      features.push_back(file_features[k]);
      targets.push_back(file_targets[k]);
    }      
  }
  // close list files
  ifs_htk_list.close();
  ifs_target_list.close();

  // get a random permutation
  vector<unsigned long> perm(features.size());
	if (dump_mode) {
		for (unsigned int k = 0; k < perm.size(); k++) 
			perm[k] = k;
	}
	else {
		rand_perm(perm);
	}
  
  my_htk_output.header.nSamples = features.size();
  my_htk_output.write_header();
  cout << "Processed " << features.size() << " frames." << endl; 
  for (unsigned long i=0; i < features.size(); ++i) {
    data = features[perm[i]];
    my_htk_output.write_next_vector(features[perm[i]]);
    ofs_target_list << targets[perm[i]] << endl;
  }
  fclose(htk_output);
  ofs_target_list.close();
}


//*****************************************************************************
//                                     E O F
//*****************************************************************************






