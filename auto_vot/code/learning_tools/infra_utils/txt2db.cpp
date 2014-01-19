//=============================================================================
// File Name: txt2db.cpp
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
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace learning;
using std::cerr;
using std::endl;
using std::string;

void get_tokens(std::vector<std::string>& tokens, const std::string& str)
{
  class tokenizer {
  public:
    tokenizer(const std::string& str) : in_(str) {}
    bool next() { return in_ >> token_; }
    std::string token() const { return token_; }
  private:
    std::istringstream in_;
    std::string token_;
  };
  
  tokens.clear();
  tokenizer t(str);
  while (t.next())
    tokens.push_back(t.token());
}

int main(int argc, char *argv[])
{
  std::string infra_filename;
  std::string txt_filename;

  cmd_line cmdline;
  cmdline.info("Convert a text file into a db and labels files. \n"
	       "Output two files: {db_file}.db and {db_file}.labels.");
  cmdline.add_master_option("text_file", &txt_filename);
  cmdline.add_master_option("db_file (with no extension)", &infra_filename);
  if ( cmdline.parse(argc, argv) < 2) {
    cmdline.print_help();
    return EXIT_FAILURE;
  }
  
  // open input text file
  std::ifstream instream;
  instream.open(txt_filename.c_str());
  if (!instream.good()) {
    cerr << "Cannot open input file " << txt_filename  << "." << endl;
    return EXIT_FAILURE;
  }
  
  // open input infra file
  std::string instances_filename = infra_filename + ".db";
  FILE *outstream;
  if ( (outstream=fopen(instances_filename.c_str(), "wb")) == NULL) {
    cerr << "Cannot open output file " << instances_filename  << "." << endl;
    return EXIT_FAILURE;
  }
  std::string labels_filename = infra_filename + ".labels";
  std::ofstream labels_ofs;
  labels_ofs.open(labels_filename.c_str());
  if (!labels_ofs.good()) {
    cerr << "Cannot open output file " << labels_filename  << "." << endl;
    return EXIT_FAILURE;
  }  
  unsigned long num_vectors = 0;

  // write temporary header
  infra::save_binary(outstream, num_vectors);

  while (instream.good()) {
    std::vector<std::string> tokens;
    string line;
    getline(instream, line, '\n');
    get_tokens(tokens, line);
    if (line == "" || tokens.size() == 0)
      break;
    infra::vector v(tokens.size()-1);
    
    labels_ofs << tokens[0] << endl;
    for (uint i = 1; i < tokens.size(); i++) 
      v[i-1] = strtod(tokens[i].c_str(), NULL);
    infra::save_binary(outstream, v);
    num_vectors++;
  }

  // update header
  rewind(outstream);
  infra::save_binary(outstream, num_vectors);

  // close files
  instream.close();
  fclose(outstream);

  return EXIT_SUCCESS;
}
