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

#ifndef _CMD_LINE_H_
#define _CMD_LINE_H_

//*****************************************************************************
// Included Files
//*****************************************************************************
#include <string>
#include <vector>


namespace learning {

class cmd_option;

//*****************************************************************************
/** Implements a class that parse the command line.
    @author Joseph Keshet (jkeshet@cs.huji.ac.il)
*/
class cmd_line
{
 public:
//-----------------------------------------------------------------------------
/** Adds general information that will be printed when -h is called
 */
  void info(const std::string &info) { info_ = info; }
//-----------------------------------------------------------------------------
/** Adds command line option of int type
 */
  void add(const std::string &tag, const std::string &helper, int *value,
	   const int default_value);
//-----------------------------------------------------------------------------
/** Adds command line option of unsigned int type
 */
  void add(const std::string &tag, const std::string &helper, 
	   unsigned int *value, const int default_value);
//-----------------------------------------------------------------------------
/** Adds command line option of double type
 */
  void add(const std::string &tag, const std::string &helper, double *value,
	   const double default_value);
//-----------------------------------------------------------------------------
/** Adds command line option of bool type
 */
  void add(const std::string &tag, const std::string &helper, bool *value,
	   const bool default_value);
//-----------------------------------------------------------------------------
/** Adds command line option of std::string type
 */
  void add(const std::string &tag, const std::string &helper, 
	   std::string *value, const std::string &default_value);
//-----------------------------------------------------------------------------
/** Adds command line option
 */
  void add(cmd_option *cmd_option);
//-----------------------------------------------------------------------------
/** Adds command line option
 */
  void add_master_option(const std::string &helper, 
			 std::string *master_option);
//-----------------------------------------------------------------------------
/** Parses command line. Returns the number of arguments proceeded.
 */  
  int parse(int argc, char **argv) ;
//-----------------------------------------------------------------------------
/** Prints help information for program and usage of tags
 */  
  void print_help();
  //-----------------------------------------------------------------------------
  /** Destructor
    */  
  ~cmd_line();
 private:
  std::string info_;
  std::vector<cmd_option*> options_vector;
  std::vector<cmd_option*> master_options_vector;
  std::string program_name;
};

}; // namespace learning

#endif // _CMD_LINE_H_
