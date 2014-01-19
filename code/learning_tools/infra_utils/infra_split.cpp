//=============================================================================
// File Name: infra_split.cpp
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
  unsigned int frame_length;
  unsigned int frame_shift;
  string ext;

  cmd_line cmdline;
  cmdline.info("Split an infra text file to several infra text files with a given frame length\n"
               "The generated infra text files will have the extension .1, .2, ...");
  cmdline.add("-l", "number of frames per infra text file", &frame_length, 200);
  cmdline.add("-s", "number of frames to shift between one file and onther", &frame_shift, 100);
  cmdline.add("-ext", "default extension of the infra files [\"scores\"]", &ext, "scores");
  cmdline.add_master_option("infra_file", &infra_filename);
  if ( cmdline.parse(argc, argv) < 1) {
    cmdline.print_help();
    return EXIT_FAILURE;
  }
  
  // open input infra file
  ifstream ifs(infra_filename.c_str());
  if (!ifs.good()) {
    cerr << "Error: unable to read from " << infra_filename << endl;
    return EXIT_FAILURE;
  }
  infra::matrix mat(ifs);

  // remove the extension
  std::string infra_filename2 = infra_filename;
  size_t ext_pos = infra_filename2.find(ext);
  std::string infra_filename3 = infra_filename2.substr(0,ext_pos);

  long num_output_file = 0;
  while (num_output_file*frame_shift + frame_length < mat.height()) {
    ofstream ofs;
    std::ostringstream oss;
    oss << infra_filename3  << num_output_file << "." << ext;
    ofs.open(oss.str().c_str());
    if (!ofs.good()) {
      cerr << "Error: unable to write " << oss.str() << endl;
      return EXIT_FAILURE;
    }
    ofs << mat.submatrix(num_output_file*frame_shift, 0, frame_length, mat.width());
    ofs.close();
    num_output_file++;
  }
  
  
  return EXIT_SUCCESS;

}
