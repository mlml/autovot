//=============================================================================
// File Name: db_remove_labels.cpp
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

//=============================================================================
// File Name: remove_labels - removes negative labels from db and label files
// Written by: Joseph Keshet
//=============================================================================

//*****************************************************************************
// Included Files
//*****************************************************************************
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <infra.h>
#include <string>
#include <cmd_line.h>

//*****************************************************************************
// usings
//*****************************************************************************
using std::vector;
using std::cerr;
using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::ifstream;
using std::ofstream;
using std::pair;
using namespace learning;

//*****************************************************************************
// MACROS
//*****************************************************************************
#define _file_safety( cmd , good_return_val ) \
if(cmd != good_return_val){perror("error accessing data file: "); exit(1);}

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
// build the index of instances and labels
void build_index(ifstream& ifs, FILE* f,
		 vector<pair<string,unsigned long> >& v) {

  // determine dataset size
  unsigned long size;
  infra::load_binary(f,size);
  v.resize(size);

  // variables to be read from the file
  char header;
  size_t block_size;

  // read entries
  for(unsigned long i=0; i<size; ++i) {

    ifs >> v[i].first;
    if(!ifs.good()) { cerr << "labels file too short\n"; exit(1); }
    v[i].second = ftell(f);

    // read first header
    _file_safety(fread(&header, sizeof(char), 1, f),1);
    _file_safety(fread(&block_size, sizeof(size_t), 1, f),1);
#ifdef _ENDIAN_SWAP_
    block_size = SWAB32(block_size);
#endif
    _file_safety(fseek(f, block_size, SEEK_CUR),0);
  }
}

//----------------------------------------------------------------------------
// the main
int main(int argc,char* argv[]) 
{
  string input_filename, output_filename;

  cmd_line cmdline;
  cmdline.info("Remove instances which their labels are less than zero.");
  cmdline.add_master_option("in_file{.db,.labels}", &input_filename);
  cmdline.add_master_option("out_file{.db,.labels}", &output_filename);
  if ( cmdline.parse(argc, argv) < 2) {
    cmdline.print_help();
    return EXIT_FAILURE;
  }

  // open the data file
  std::string instances_filename = input_filename + ".db";
  FILE* f = fopen(instances_filename.c_str(),"r");
  if (f == NULL) {
    std::cerr << "Error: unable to open file: " << instances_filename << std::endl;
    return EXIT_FAILURE;
  }
  std::string labels_filename = input_filename + ".labels";
  std::ifstream ifs(labels_filename.c_str());
  if (!ifs.good()) {
    std::cerr << "Error: unable to open file: " << labels_filename << std::endl;
    return EXIT_FAILURE;
  }

  vector<pair<string,unsigned long> > v;

  build_index(ifs,f,v);

  ifs.close();
  
  // create new files
  instances_filename = output_filename + ".db";
  FILE* of = fopen(instances_filename.c_str(),"w+");
  if (of == NULL) {
    std::cerr << "Error: unable to open file: " << instances_filename << std::endl;
    return EXIT_FAILURE;
  }
  labels_filename = output_filename + ".labels";
  std::ofstream ofs(labels_filename.c_str());
  if (!ofs.good()) {
    std::cerr << "Error: unable to open file: " << labels_filename << std::endl;
    return EXIT_FAILURE;
  }

  infra::save_binary(of,(unsigned long)(v.size()));
  
  unsigned long new_size = 0;
  for (unsigned long i=0; i<v.size(); ++i) {
    if (atoi((v[i].first).c_str()) >= 0) {
      fseek(f,v[i].second,SEEK_SET);
      infra::vector vec(f);
      vec.save_binary(of);
      ofs << v[i].first << endl;
      new_size++;
    }
  }

  fseek(of,0L,SEEK_SET);
  infra::save_binary(of,new_size);

  fclose(f);
  fclose(of);
  ofs.close();
}

//*****************************************************************************
//                                     E O F
//*****************************************************************************






