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


#ifndef _MY_DATASET_H_
#define _MY_DATASET_H_

/************************************************************************
 Project:  Initial VOT Detection
 Module:   Dataset Definitions
 Purpose:  Defines the data structs of the instances and the labels
 Date:     26 Feb., 2010
 
 *************************** INCLUDE FILES ******************************/
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <vector>
#include <map>
#include "infra.h"

#define MAX_LINE_SIZE 4096

/***********************************************************************/

class StringVector : public std::vector<std::string> {
public:
  
  unsigned int read(std::string &filename) {
    std::ifstream ifs;
    char line[MAX_LINE_SIZE];
    ifs.open(filename.c_str());
    if (!ifs.is_open()) {
      std::cerr << "Unable to open file list:" << filename << std::endl;
      exit(-1);
    }    
    while (!ifs.eof()) {
      ifs.getline(line,MAX_LINE_SIZE);
      if (strcmp(line,""))
        push_back(std::string(line));
    }
    ifs.close();
    return size();
  }
};

/***********************************************************************/

typedef std::vector<int> IntVector;
typedef std::vector<double> DoubleVector;

/***********************************************************************/

class SpeechUtterance
{
public:
  void read(std::string &filename);
  unsigned long size() { return scores.height(); }  
  unsigned long dim() { return scores.width(); } 
  
public:
  infra::matrix scores;
};

/***********************************************************************/

class VotLocation 
{
public:
	int burst;
	int voice;
	bool is_neg() { return (burst > voice); }
	bool is_pos() { return (burst < voice); }

};

std::ostream& operator<< (std::ostream& os, const VotLocation& y);

/***********************************************************************/

class Instances
{
public:
  void read(std::string &filename, bool labels_given=true);
  unsigned long size() { return file_list.size(); }

public:
  StringVector file_list;
  IntVector word_start;
  IntVector word_end;
  IntVector vot_burst;
  IntVector vot_voice;
};

/***********************************************************************/

class NewInstances
{
public:
  void read(std::string &filename, bool labels_given=true);
  unsigned long size() { return file_list.size(); }
	
public:
  StringVector file_list;
  DoubleVector word_start;
  DoubleVector word_end;
  DoubleVector vot_burst;
  DoubleVector vot_voice;
};

/***********************************************************************/

class Dataset 
{
public:
  Dataset(std::string& instances_filelist, std::string& labels_filename);
  unsigned int read(SpeechUtterance &x, VotLocation &y);
  unsigned long size() { return instances_file_list.size(); } 
  bool labels_given() { return read_labels; }
  
private:
  StringVector instances_file_list;
  infra::matrix labels;
  int current_file;
  bool read_labels;
};

#endif // _MY_DATASET_H_
