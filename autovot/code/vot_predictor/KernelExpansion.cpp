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


#include <iostream>
#include "KernelExpansion.h"
#include "Logger.h"

KernelExpansion::KernelExpansion(std::string _kernel_name, int _d, double _sigma)
{
  kernel_name = _kernel_name;
  d = _d;
  sigma = _sigma;
}

int KernelExpansion::features_dim() 
{
  int D;
  if (kernel_name == "poly2")
    D = d + d*(d-1)/2.0;
  else if  (kernel_name == "rbf2")
    D = 1 + 2.0*d + d*(d-1)/2.0;
  else if  (kernel_name == "rbf3")
    D = 1 + 3.0*d + 3.0*d*(d-1)/2.0 + d*(d-1)*(d-2)/6.0;
  else if  (kernel_name == "" || kernel_name == "none")
    D = d;
  else {
		LOG(ERROR) << "This type of kernel expansion is not supported" ;
    exit (-1);
  }
  return D;
}

infra::vector_view KernelExpansion::expand(infra::vector_base x)
{
  infra::vector x_expanded(features_dim());
  
  if (kernel_name == "poly2") {
    int loc = 0;
    for (int j=0; j < int(x.size()); j++)  {
      x_expanded(loc) = x[j]*x[j];
      loc++;
    }
    for (int j=0; j < int(x.size())-1; j++) {
      for (int k=j+1; k < int(x.size()); k++) { 
        x_expanded(loc) = sqrt(2.0)*x[j]*x[k];
        loc++;
      }
    }
  } // poly2
  else if (kernel_name == "rbf2") {
    // exp(-||x||^2/(2*sigma2))
    double x_norm2 = 0.0;
    for (int j=0; j < int(x.size()); j++)  {
      x_norm2 += x[j]*x[j];
    }
    double exp2_coef = exp(-x_norm2/(2.0*sigma*sigma));

    // exp(-<x,z>) expansion
    int loc = 0;
		double tmp_coef;
    x_expanded(loc) = 1.0;
    x_expanded(loc) *= exp2_coef;
    loc++;
		tmp_coef = 1/sigma;
    for (int j=0; j < int(x.size()); j++)  {
      x_expanded(loc) = x[j];
      x_expanded(loc) *= tmp_coef*exp2_coef;
      loc++;
    }
		tmp_coef = 1.0/(sqrt(2.0)*sigma*sigma);
    for (int j=0; j < int(x.size()); j++)  {
      x_expanded(loc) = x[j]*x[j];
      x_expanded(loc) *= tmp_coef*exp2_coef;
      loc++;
    }
    tmp_coef = 1.0/(sigma*sigma);
		for (int j=0; j < int(x.size())-1; j++) {
      for (int k=j+1; k < int(x.size()); k++) { 
        x_expanded(loc) = x[j]*x[k];
        x_expanded(loc) *= tmp_coef*exp2_coef;
        loc++;
      }
    }
  }
  else if (kernel_name == "rbf3") {
    // exp(-||x||^2/sigma2)
    double x_norm2 = 0.0;
    for (int j=0; j < int(x.size()); j++) {
      x_norm2 += x[j]*x[j];
    }
    double exp2_coef = exp(-x_norm2/(2.0*sigma*sigma));
    // exp(-x) expansion
    int loc = 0;
		double tmp_coef;
    x_expanded(loc) = 1.0;
    x_expanded(loc) *= exp2_coef;
    loc++;
		tmp_coef = 1/sigma;
    for (int j=0; j < int(x.size()); j++)  {
      x_expanded(loc) = x[j];
      x_expanded(loc) *= tmp_coef*exp2_coef;
      loc++;
    }
		tmp_coef = 1.0/(sqrt(2.0)*sigma*sigma);
    for (int j=0; j < int(x.size()); j++)  {
      x_expanded(loc) = x[j]*x[j];
      x_expanded(loc) *= tmp_coef*exp2_coef;
      loc++;
		}
		tmp_coef = 1.0/(sigma*sigma);
    for (int j=0; j < int(x.size())-1; j++) {
      for (int k=j+1; k < int(x.size()); k++) {
        x_expanded(loc) = x[j]*x[k];
        x_expanded(loc) *= tmp_coef*exp2_coef;
        loc++;
			}
		}
		tmp_coef = 1.0/(sqrt(6.0)*sigma*sigma*sigma);
		for (int j=0; j < int(x.size()); j++)  {
      x_expanded(loc) = x[j]*x[j]*x[j];
      x_expanded(loc) *= tmp_coef*exp2_coef;
      loc++;
    }
		tmp_coef = sqrt(3.0)/(sqrt(6.0)*sigma*sigma*sigma);
		for (int j=0; j < int(x.size())-1; j++) {
			for (int k=j+1; k < int(x.size()); k++) {
        x_expanded(loc) = x[j]*x[j]*x[k];
        x_expanded(loc) *= tmp_coef*exp2_coef;
        loc++;
        x_expanded(loc) = x[j]*x[k]*x[k];
        x_expanded(loc) *= tmp_coef*exp2_coef;
        loc++;          
      }
    }
		tmp_coef = 1.0/(sigma*sigma*sigma);
    for (int j=0; j < int(x.size())-2; j++) {
      for (int k=j+1; k < int(x.size())-1; k++) { 
        for (int l=k+1; l < int(x.size()); l++) { 
          x_expanded(loc) = x[j]*x[k]*x[l];
          x_expanded(loc) *= tmp_coef*exp2_coef;
          loc++;
        }
      }
    }
  }
	else {
		x_expanded = x;
	}
  
  return x_expanded;
}


