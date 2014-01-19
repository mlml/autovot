//=============================================================================
// File Name: cmd_option.h 
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
#ifndef _CMD_OPTION_H_
#define _CMD_OPTION_H_

//*****************************************************************************
// Included Files
//*****************************************************************************
#include <string>
#include <cstdlib>

namespace learning {

//*****************************************************************************
/** Implements a abstract class of command line option. The command option is 
    a class that keep data of each specific option in the command line.
    @author Joseph Keshet (jkeshet@cs.huji.ac.il)
*/
class cmd_option
{
 public:
//-----------------------------------------------------------------------------
/** Default constructor.
 */
  cmd_option() { }
//-----------------------------------------------------------------------------
/** Standard Destructor.
 */
  virtual ~cmd_option() { }
//-----------------------------------------------------------------------------
/** Returns referance to the helper string
*/
  std::string& helper() { return helper_;} 
//-----------------------------------------------------------------------------
/** Returns referance to the tag string
*/
  std::string& tag() { return tag_;}
//-----------------------------------------------------------------------------
/** Sets a value from the command argument
    @param arg Command argument
*/
  virtual void set_value(char *arg) = 0;

  virtual bool needs_value() = 0;

 protected:
  std::string tag_;
  std::string helper_;
  
};

//-----------------------------------------------------------------------------

template <typename T>
struct option_type_traits { };

template <> struct option_type_traits<int>
{
  typedef int option_type;
  static inline bool is_bool() { return false; }
  static option_type from_string(char *str) { return strtol(str, NULL, 10); }
};

template <> struct option_type_traits<unsigned int>
{
  typedef unsigned int option_type;
  static inline bool is_bool() { return false; }
  static option_type from_string(char *str) { return strtoul(str, NULL, 10); }
};

template <> struct option_type_traits<bool>
{
  typedef bool option_type;
  static inline bool is_bool() { return true; }
  static option_type from_string(char *str) { return true; }
};

template <> struct option_type_traits<double>
{
  typedef double option_type;
  static inline bool is_bool() { return false; }
  static option_type from_string(char *str) { return strtod(str, NULL); }
};

template <> struct option_type_traits<std::string>
{
  typedef std::string option_type;
  static inline bool is_bool() { return false; }
  static option_type from_string(char *str) { return std::string(str); }
};

//-----------------------------------------------------------------------------



//*****************************************************************************
/** Implements of command option for the templated basic type.
    @author Joseph Keshet (jkeshet@cs.huji.ac.il)
*/
template <typename T >
class type_cmd_option : public cmd_option {
  public:
  typedef option_type_traits<T> traits_type;
  typedef typename traits_type::option_type option_type;
  bool is_bool() { return traits_type::is_bool(); }
  option_type from_string(char *str) { return traits_type::from_string(str); }

//-----------------------------------------------------------------------------
/** Constructs a command option
    @param tag Command option tag.
    @param helper Command help string that appears when using the '-h' option
    @param var Pointer to the value
    @param deault_value The default value that will be set in case this command
    option was not set.
*/
  type_cmd_option(const std::string &tag, const std::string &helper, T *value,
		  T default_value) {
		    tag_ = tag;
		    helper_ = helper;
		    value_ = value;
		    default_value_ = default_value;
		    *value_ = default_value;
		  }
//-----------------------------------------------------------------------------
/** Sets a value from the command argument
    @param arg Command argument
*/
  void set_value(char *str) { *value_ = from_string(str); }
//-----------------------------------------------------------------------------
/** Returns true if this option needs value after the tag
*/
  bool needs_value() { return (!is_bool()); }

//*****************************************************************************
// Data Members
//*****************************************************************************
 protected:
  T *value_;
  T default_value_;
};

}; // namespace learning

#endif // _CMD_OPTION_H_

//=============================================================================
//                                 E  O  F
//=============================================================================
