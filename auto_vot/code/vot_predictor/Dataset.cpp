/************************************************************************
 Copyright (c) 2014 Joseph Keshet, Morgan Sonderegger, Thea Knowles

This file is part of Autovot, a package for automatic extraction of
voice onset time (VOT) from audio files.

Autovot is free software: you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

Autovot is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with Autovot.  If not, see
<http://www.gnu.org/licenses/>.
************************************************************************/

/************************************************************************
 Project:  Initial VOT Detection
 Module:   Dataset Definition
 Purpose:  Defines the data structs of the instances and the labels
 Date:     26 Feb., 2010
  
 **************************** INCLUDE FILES *****************************/
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include "Dataset.h"
#include "Logger.h"

/************************************************************************
 Function:     Instances::read
 
 Description:  Read intance to front-end processing from file stream
 Inputs:       string &filename
 Output:       void
 Comments:     none.
 ***********************************************************************/
void Instances::read(std::string &filename, bool labels_given)
{
  // open stream
  std::ifstream ifs(filename.c_str());
  if (!ifs.good()) {
    LOG(ERROR) << "Unable to read instances from " << filename;
    exit(-1);
  } 
  
  // read each line
  char line[MAX_LINE_SIZE];
  
  unsigned long line_num = 1;
  while (!ifs.eof()) {
    ifs.getline(line,MAX_LINE_SIZE);
    if (strcmp(line,"")) {
      char _filename[FILENAME_MAX];
      int _word_start;
      int _word_end;
      int _vot_burst;
      int _vot_voice;
      sscanf(line, "%s %d %d %d %d", _filename, &_word_start, &_word_end, &_vot_burst, &_vot_voice);
      // validate line format

      if (labels_given && ( _word_start > _vot_burst || _vot_burst > _word_end) ) {
        LOG(ERROR) << "Problem in line number " << line_num <<  " of the input filelist " << filename << std::endl
				<< "       should be   word_start <= vot_burst < word_end" << std::endl
        << "       but read word_start=" << _word_start << " vot_burst=" << _vot_burst
				<< " vot_voice=" << _vot_voice << " word_end=" << _word_end;
        exit(-1);
      }
      file_list.push_back(std::string(_filename));
      word_start.push_back(_word_start);
      word_end.push_back(_word_end);
      vot_burst.push_back(_vot_burst);
      vot_voice.push_back(_vot_voice);
      // name
      
    }
    line_num++;
  }
  ifs.close();
}
  
  
/************************************************************************
 Function:     NewInstances::read
 
 Description:  Read intance to front-end processing from file stream (new format)
 Inputs:       string &filename
 Output:       void
 Comments:     none.
 ***********************************************************************/
void NewInstances::read(std::string &filename, bool labels_given)
{
  // open stream
  std::ifstream ifs(filename.c_str());
  if (!ifs.good()) {
    LOG(ERROR) << "Unable to read instances from " << filename ;
    exit(-1);
  }
  
  // read each line
  char line[MAX_LINE_SIZE];
  
  unsigned long line_num = 1;
  while (!ifs.eof()) {
    ifs.getline(line,MAX_LINE_SIZE);
    if (strcmp(line,"")) {
      char _filename[FILENAME_MAX];
      double _word_start;
      double _word_end;
      double _vot_burst;
      double _vot_voice;

			// parse the name of the WAV file given between "
			char *token = strtok(line, "\"");
			if (token== NULL) {
				LOG(ERROR) << "Unable to read instances from " << filename << " at line " << line_num << " (1)";
				exit(-1);
			}
			strcpy(_filename, token);

			// parse the rest of the line
      sscanf(token+strlen(token)+1, "%lf %lf %lf %lf", &_word_start, &_word_end, &_vot_burst, &_vot_voice);
      //printf("%s %lf %lf %lf %lf\n", _filename, _word_start, _word_end, _vot_burst, _vot_voice);

      // validate line format
      if (labels_given && ( _word_start > _vot_burst || _vot_burst > _word_end) ) {
        LOG(ERROR) << "Problem in line number " << line_num <<  " of the input filelist " << filename << std::endl
				<< "       should be   word_start <= vot_burst < word_end" << std::endl
        << "       but read word_start=" << _word_start << " vot_burst=" << _vot_burst
				<< " vot_voice=" << _vot_voice << " word_end=" << _word_end;
        exit(-1);
      }
      file_list.push_back(std::string(_filename));
      word_start.push_back(_word_start);
      word_end.push_back(_word_end);
      vot_burst.push_back(_vot_burst);
      vot_voice.push_back(_vot_voice);
      // name
      
    }
    line_num++;
  }
  ifs.close();
}



/************************************************************************
 Function:     SpeechUtterance::read
 
 Description:  Read SpeechUtterance from file stream
 Inputs:       string &filename
 Output:       bool - true if success, otherwise false
 Comments:     none.
 ***********************************************************************/
void SpeechUtterance::read(std::string &filename)
{
  // load score matrix
  std::ifstream ifs(filename.c_str());
  if (ifs.good()) {
    infra::matrix tmp(ifs);
    scores.resize(tmp.height(), tmp.width());
    ///std::cout << "tmp_before=" << tmp << std::endl;
    //tmp.submatrix(0,43,tmp.height(),tmp.width()-43).zeros(); // works
    //tmp.submatrix(0,47,tmp.height(),tmp.width()-47).zeros(); // does not work
    //tmp.submatrix(0,43,tmp.height(),4).zeros(); // ??
    ///std::cout << "tmp_after=" << tmp << std::endl;
    //std::cout << "Debug: using only the first 43 features" << std::endl;
    scores = tmp;
  }
  else {
    LOG(ERROR) << "Unable to read instance from " << filename;
  }
}


/************************************************************************
 Function:     operator << for VotLocation
 
 Description:  Write VotLocation& vector to output stream
 Inputs:       std::ostream&, const StringVector&
 Output:       std::ostream&
 Comments:     none.
 ***********************************************************************/
std::ostream& operator<< (std::ostream& os, const VotLocation& y)
{
  os << y.burst << " " << y.voice  ;
  
  return os;
}

/************************************************************************
 Function:     Dataset::Dataset
 
 Description:  Constructor
 Inputs:       std::string dataset_filename
 Output:       void.
 Comments:     none.
 ***********************************************************************/
Dataset::Dataset(std::string& instances_filelist, std::string& labels_filename)
{
  
  // Read list of files into StringVector
  instances_file_list.read(instances_filelist);
  if (labels_filename == "null") {
    LOG(INFO) << "No labels were given.";
    read_labels = false;
  }
  else {
		// read the WHOLE label file
		std::ifstream ifs_labels(labels_filename.c_str());
		if (!ifs_labels.good()) {
			LOG(ERROR) << "Unable to open " << labels_filename << " for reading";
			exit(-1);
		}
		infra::matrix tmp(ifs_labels);
		ifs_labels.close();
		labels.resize(tmp.height(), tmp.width());
		labels = tmp;
		if ( labels.height() != instances_file_list.size() || labels.width() != 2 ) {
			LOG(ERROR) << " The width of the matrix in labels file should be " << instances_file_list.size() << "x 2 .";
			exit(-1);
		}
    read_labels = true;
  }
  current_file = 0;
}


/************************************************************************
 Function:     Dataset::read
 
 Description:  Read next instance and label
 Inputs:       SpeechUtterance&
 VotLocation&
 Output:       number of frames of x successfully read.
 Comments:     none.
 ***********************************************************************/
unsigned int Dataset::read(SpeechUtterance &x, VotLocation &y)
{
	LOG(DEBUG) << "current file=" << current_file << " " << instances_file_list[current_file];
	x.read(instances_file_list[current_file]);  
  if (read_labels) {
    y.burst = labels.row(current_file)[0]-1;
    y.voice = labels.row(current_file)[1]-1;
  }
	//std::cout << "x=" << x.scores << std::endl;
	//std::cout << "y=" << y.onset << " " << y.offset << std::endl;
	
  current_file++;
	return x.size();
}


// --------------------------  EOF ------------------------------------//
