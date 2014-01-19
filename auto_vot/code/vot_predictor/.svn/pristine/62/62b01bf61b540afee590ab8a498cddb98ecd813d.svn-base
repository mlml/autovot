//=============================================================================
// File Name: infra_exception.cpp
// Written by: Joseph Keshet (19.02.04)
//=============================================================================

//*****************************************************************************
// Included Files
//*****************************************************************************
#include "infra_exception.h"
#include <stdexcept>
#include <sstream>
#include <iostream>

//-----------------------------------------------------------------------------
infra::exception::exception( std::string expression, std::string message, 
			     std::string file, int line) :
  _expression(expression), _message(message), _file(file), _line(line),
  _internal(true) {
  std::set_terminate (infra::terminate);
}

//-----------------------------------------------------------------------------
void infra::exception::set_external (std::string expression, std::string file,
				     int line) {
  _expression = expression;
  _file = file;
  _line = line;
  _internal = false;
}

//-----------------------------------------------------------------------------
void infra::exception::print() {

  if(_internal) {
    std::cerr << "In " << _file << ", on line " << _line << ", the following "
	      << "condition was not met: \"" << _expression
	      << "\". " << _message << "." << std::endl;
  }
  
  else {
    std::cerr << "In " << _file << ", on line " << _line<< ", the statement \""
	      << _expression << "\" incorrectly uses the infra library and "
	      << "caused an exception. " << _message << "." << std::endl;
  }
}

//-----------------------------------------------------------------------------
void infra::terminate()
{
  // try to catch anything thrown
  try { throw; }
  catch (infra::exception& exc) {exc.print();}
  catch (std::exception& exc) {std::cerr << exc.what() << std::endl;}
  catch (...) {}
}

//*****************************************************************************
//                                     E O F
//*****************************************************************************
