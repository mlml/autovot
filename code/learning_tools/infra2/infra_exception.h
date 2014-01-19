//=============================================================================
// File Name: infra_exception.h
// Written by: Joseph Keshet (19.02.04)
//
// Distributed as part of the infra C++ library for linear algebra
// Copyright (C) 2004 Ofer Dekel
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
#ifndef _JKESHET_EXCEPTION_H_
#define _JKESHET_EXCEPTION_H_

//*****************************************************************************
// Included Files
//*****************************************************************************
#include <string>
#include <sstream>

namespace infra {

//-----------------------------------------------------------------------------
/** A callback function which is called whenever an infra_exception is
    not caught. 
*/
void terminate();

//-----------------------------------------------------------------------------
/** Implements an exception which indicates that the infra library was somehow
    misused.
*/
class exception  
{
 public:
//-----------------------------------------------------------------------------
/** Default constructor 
*/
  exception( std::string expression, std::string message, 
	     std::string file, int line);

//-----------------------------------------------------------------------------
/** Resets the exception parameters. This function should never be called
    explicitly, except using the infra_verify() macro
*/
  void set_external (std::string expression, std::string file, int line);

//-----------------------------------------------------------------------------
/** Prints the error message to cerr. Should not be called explicitly, except
    using the infra_assert() macro.
*/
  void print();
  
//=============================================================================
// Data Members
//=============================================================================
  protected:
  std::string _expression;
  std::string _message;
  std::string _file;
  int _line;
  bool _internal;
};
};

//-----------------------------------------------------------------------------
#ifdef NDEBUG
#define infra_verify( statement ) statement;

#else
#define infra_verify( statement )                                     \
try { statement; }                                                    \
catch (infra::exception& exc) {                                       \
exc.set_external(#statement, __FILE__, __LINE__);                     \
throw exc; } 
#endif // NDEBUG


//-----------------------------------------------------------------------------
#ifdef NDEBUG
#define infra_assert( expression, message )                           

#else
#define infra_assert( expression, message )                           \
if ( !( expression ) ) {                                              \
  std::ostringstream __infra_str_;                                    \
  __infra_str_ << message;                                            \
  throw infra::exception(#expression, __infra_str_.str(),             \
                               __FILE__, __LINE__);                   \
}                                                                     
#endif // NDEBUG
#endif // _JKESHET_EXCEPTION_H_

//*****************************************************************************
//                                     E O F
//*****************************************************************************
