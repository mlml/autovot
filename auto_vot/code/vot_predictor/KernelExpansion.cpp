/*
 *  KernelExpansion.cpp
 *  pa_primal
 *
 *  Created by Joseph Keshet on 28/8/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include "KernelExpansion.h"

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
    std::cerr << "Error: this type of kernel expansion is not supported" << std::endl;
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
    // exp(-||x||^2/sigma2)
    double exp2_coef = 0.0;
		for (int j=0; j < int(x.size()); j++)  {
      exp2_coef += x[j]*x[j]/(sigma*sigma);
    }
    exp2_coef = exp(-exp2_coef/2.0);
		double exp2_coef_div_sigma = exp2_coef/sigma;
		double exp2_coef_div_sigma2 = exp2_coef/(sigma*sigma);
		double exp2_coef_sqrt2_div_sigma2 = sqrt(2.0)*exp2_coef/(sigma*sigma);
		
    // exp(-x) expansion
    int loc = 0;
    x_expanded(loc) = 1.0;
    x_expanded(loc) *= exp2_coef;
    loc++;
    for (int j=0; j < int(x.size()); j++)  {
      x_expanded(loc) = x[j];
      x_expanded(loc) *= exp2_coef_div_sigma;
      loc++;
    }
    for (int j=0; j < int(x.size()); j++)  {
      x_expanded(loc) = x[j]*x[j];
      x_expanded(loc) *= exp2_coef_div_sigma2;
      loc++;
    }
    for (int j=0; j < int(x.size())-1; j++) {
      for (int k=j+1; k < int(x.size()); k++) { 
        x_expanded(loc) = x[j]*x[k];
        x_expanded(loc) *= exp2_coef_sqrt2_div_sigma2;
        loc++;
      }
    }
  }
  else if (kernel_name == "rbf3") {
    // exp(-||x||^2/sigma2)
    double exp2_coef = 0.0;
    for (int j=0; j < int(x.size()); j++) {
      exp2_coef += x[j]*x[j]/(sigma*sigma);
    }
    exp2_coef = exp(-exp2_coef/2.0);
    // exp(-x) expansion
    int loc = 0;
    x_expanded(loc) = 1.0;
    x_expanded(loc) *= exp2_coef;
    loc++;
    for (int j=0; j < int(x.size()); j++)  {
      x_expanded(loc) = x[j]/sigma;
      x_expanded(loc) *= exp2_coef;
      loc++;
    }
    for (int j=0; j < int(x.size()); j++)  {
      x_expanded(loc) = 1.0/(sqrt(2.0))*x[j]*x[j]/(sigma*sigma);
      x_expanded(loc) *= exp2_coef;
      loc++;
      x_expanded(loc) = 1.0/sqrt(6.0)*x[j]*x[j]*x[j]/(sigma*sigma*sigma);
      x_expanded(loc) *= exp2_coef;
      loc++;
    }
    for (int j=0; j < int(x.size())-1; j++) {
      for (int k=j+1; k < int(x.size()); k++) { 
        x_expanded(loc) = x[j]*x[k]/(sigma*sigma);
        x_expanded(loc) *= exp2_coef;
        loc++;
        x_expanded(loc) = sqrt(3.0)/sqrt(6.0)*x[j]*x[j]*x[k]/(sigma*sigma*sigma);
        x_expanded(loc) *= exp2_coef;
        loc++;
        x_expanded(loc) = sqrt(3.0)/sqrt(6.0)*x[j]*x[k]*x[k]/(sigma*sigma*sigma);
        x_expanded(loc) *= exp2_coef;
        loc++;          
      }
    }
    for (int j=0; j < int(x.size())-2; j++) {
      for (int k=j+1; k < int(x.size())-1; k++) { 
        for (int l=k+1; l < int(x.size()); l++) { 
          x_expanded(loc) = x[j]*x[k]*x[l]/(sigma*sigma*sigma);
          x_expanded(loc) *= exp2_coef;
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



