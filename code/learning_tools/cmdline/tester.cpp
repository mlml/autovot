//=============================================================================
// File Name: tester.cpp 
// Written by: Joseph Keshet (jkeshet@cs.huji.ac.il)
//
// Distributed as part of the command-line library
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

#include <iostream>
#include <cstdlib>
#include "cmd_line.h"

int main(int argc, char *argv[])
{
  double r;
  std::string s;
  int i;
  unsigned int ui;
  bool b;
  std::string master1, master2;
  
  learning::cmd_line cmdline;
  cmdline.info("This is a tester program for cmd_line class");
  cmdline.add("-r", "your value for real", &r, 3.1415);
  cmdline.add("-s", "your value for string", &s, "my string");
  cmdline.add("-i", "your integer", &i, 73);
  cmdline.add_master_option("input_filename", &master1);
  cmdline.add("-ui", "your unsigned integer", &ui, 1973);
  cmdline.add("-b", "your boolean", &b, false);
  cmdline.add_master_option("output_filename", &master2);
  int rc = cmdline.parse(argc, argv);

  if (rc < 1) {
    cmdline.print_help();
    return EXIT_FAILURE;
  }

  std::cout << "After parsing " << rc << " arguments:" << std::endl;
  std::cout << "r=" << r << std::endl;
  std::cout << "s=" << s << std::endl;
  std::cout << "i=" << i << std::endl;
  std::cout << "b=" << b << std::endl;
  std::cout << "input=" << master1 << std::endl;
  std::cout << "output=" << master2 << std::endl;  

  return EXIT_SUCCESS;
}
