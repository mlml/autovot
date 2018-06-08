//=============================================================================
// File Name: txt2htk.cpp
// Written by: Joseph Keshet (jkeshet@cs.huji.ac.il)
//
// Distributed as part of the HTK utilities
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

#include <cmdline/cmd_line.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include "HtkFile.h"

using namespace learning;
using std::cerr;
using std::endl;
using std::string;

void get_tokens(std::vector<std::string>& tokens, const std::string& str)
{
	class tokenizer {
public:
		tokenizer(const std::string& str) : in_(str) {}
		bool next() { return static_cast<bool>(in_ >> token_); }
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
	std::string htk_filename;
	std::string txt_filename;
	unsigned int sampPeriod;
	std::string parmKind = "USER";
	
	cmd_line cmdline;
	cmdline.info("Convert a text file into an HTK file.\n");
  cmdline.add("-sampPeriod", "sample period in 100ns units [100000]", &sampPeriod, 100000);
	cmdline.add_master_option("txt_file", &txt_filename);
	cmdline.add_master_option("htk_file", &htk_filename);
	if ( cmdline.parse(argc, argv) < 2) {
		cmdline.print_help();
		return EXIT_FAILURE;
	}
	
	// open input text file
	std::ifstream instream;
	instream.open(txt_filename.c_str());
	if (!instream.good()) {
		cerr << "Error: Cannot open input file " << txt_filename  << "." << endl;
		return EXIT_FAILURE;
	}
	
	// open input infra file
	FILE *outstream;
	if ( (outstream=fopen(htk_filename.c_str(), "wb")) == NULL) {
		cerr << "Error: Cannot open output file " << htk_filename  << "." << endl;
		return EXIT_FAILURE;
	}
	unsigned long num_vectors = 0;
	HtkFile htk_file(outstream);

	float *data = 0;
	int num_coefs;
	while (instream.good()) {
		std::vector<std::string> tokens;
		string line;
		getline(instream, line, '\n');
		get_tokens(tokens, line);
		if (line == "" || tokens.size() == 0)
			break;
		if (data == 0) {
			data = new float[tokens.size()];
			num_coefs = tokens.size();
			htk_file.header.nSamples = 0;
			htk_file.header.sampPeriod = sampPeriod;
			htk_file.header.sampSize = num_coefs*sizeof(float);
			htk_file.header.parmKind = 9; // USER
			htk_file.write_header(); // write temporary header
		}
		for (uint i = 0; i < tokens.size(); i++) 
			data[i] = float(strtod(tokens[i].c_str(), NULL)); 
		htk_file.write_next_vector(data);
		num_vectors++;
	}
	delete [] data;
	
	// update header
	rewind(outstream);
	htk_file.header.nSamples = num_vectors;
	htk_file.write_header();	

	// close files
	instream.close();
	fclose(outstream);
	
	return EXIT_SUCCESS;
}
