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


#include <string>
#include <infra.h>

class KernelExpansion 
{
public:
  KernelExpansion(std::string _kernel_name, int _d, double _sigma = 1.0);
  int features_dim();
  infra::vector_view expand(infra::vector_base x);
  bool is_linear_kernel() { return (kernel_name == ""); }

private: 
  std::string kernel_name;
  int d;
  double sigma;
};