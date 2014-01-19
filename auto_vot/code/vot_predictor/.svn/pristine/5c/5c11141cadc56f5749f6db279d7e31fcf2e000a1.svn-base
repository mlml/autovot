//=============================================================================
// File Name: infra_vv_funcs.cpp
// Written by: Ofer Dekel (oferd@cs.huji.ac.il)
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

//*****************************************************************************
// Included Files
//*****************************************************************************
#include "infra_vv_funcs.h"
#include "infra_vector.imp"
#include "infra_exception.h"

//.............................................................................
#ifdef _USE_ATLAS_
/*-----------------------------------------------------------------------------
  if _USE_ATLAS_ is defined at compile time then algebraic operations are
  performed using the 'atlas' BLAS hardware optimized algebra procedures.
  This requires linking to the 'cblas' and 'atlas' libraries. Usually,
  these libraries are shared objects found in '/usr/lib/libcblas.so' and
  '/usr/lib/libatlas.so'.
-----------------------------------------------------------------------------*/
extern "C" {
   double cblas_ddot(const int N, 
                     const double *X, 
                     const int incX,
                     const double *Y, 
                     const int incY);
}

#endif //_USE_ATLAS_  
//.............................................................................

//-----------------------------------------------------------------------------
void infra::prod(const infra::vector_base& u, const infra::vector_base& v,
		 double& outcome) {

  infra_assert(u.size() == v.size(), "When calling prod(), both vectors "
	       << "must be of equal size. In this case, the sizes are "
	       << u.size() << " and " << v.size() << " respectively");
    
//.............................................................................
#ifdef _USE_ATLAS_
  outcome = cblas_ddot(u.size(), u.begin().ptr(), u.step(), v.begin().ptr(), 
                       v.step());

#else //_USE_ATLAS_
//.............................................................................

  infra::vector_base::const_iterator u_iter = u.begin();
  infra::vector_base::const_iterator u_end = u.end();
  infra::vector_base::const_iterator v_iter = v.begin();
  
  outcome = 0.0;
  while(u_iter < u_end) {
    outcome += (*u_iter) * (*v_iter);
    ++u_iter;
    ++v_iter;
  }

#endif //_USE_ATLAS_
}

//-----------------------------------------------------------------------------
double infra::operator*(const infra::vector_base& u, 
                        const infra::vector_base& v) {

  // call the prod function 
  double outcome;
  prod(u, v, outcome);
  return outcome;
}

//-----------------------------------------------------------------------------
void infra::add_prod(const infra::vector_base& u, const infra::vector_base& v,
		     double& outcome) {
  
  infra_assert(u.size() == v.size(), "When calling add_prod(), both vectors "
	       << "must be of equal size. In this case, the sizes are "
	       << u.size() << " and " << v.size() << " respectively");
    
//.............................................................................
#ifdef _USE_ATLAS_
  outcome += cblas_ddot(u.size(), u.begin().ptr(), u.step(), v.begin().ptr(), 
                        v.step());

#else //_USE_ATLAS_
//.............................................................................

  infra::vector_base::const_iterator u_iter = u.begin();
  infra::vector_base::const_iterator u_end = u.end();
  infra::vector_base::const_iterator v_iter = v.begin();
  
  while(u_iter < u_end) {
    outcome += (*u_iter) * (*v_iter);
    ++u_iter;
    ++v_iter;
  }

#endif //_USE_ATLAS_
}

//-----------------------------------------------------------------------------
void infra::sum(const infra::vector_base& u, const infra::vector_base& v,
		infra::vector_base& outcome) {

  infra_assert(u.size() == v.size() && u.size() == outcome.size(),
	       "When calling sum(u,v,outcome), all three vectors must be of "
	       "equal size. In this case, the sizes are " << u.size() << ", "
	       << v.size() << " and " << outcome.size() << " respectively");
    
  infra::vector_base::const_iterator u_iter = u.begin();
  infra::vector_base::const_iterator v_iter = v.begin();
  infra::vector_base::iterator outcome_iter = outcome.begin();
  infra::vector_base::const_iterator u_end = u.end();
  
  while(u_iter < u_end) {
    *outcome_iter = *u_iter + *v_iter;

    ++u_iter;
    ++v_iter;
    ++outcome_iter;
  }
}

//-----------------------------------------------------------------------------
infra::vector_base infra::operator+(const infra::vector_base& u, 
                                    const infra::vector_base& v) {

  // call the prod function 
  infra::vector outcome(u.size());
  sum(u, v, outcome);
  return outcome;
}

//-----------------------------------------------------------------------------
void infra::diff(const infra::vector_base& u, const infra::vector_base& v,
		 infra::vector_base& outcome) {

  infra_assert(u.size() == v.size() && u.size() == outcome.size(),
	       "When calling diff(u,v,outcome), all three vectors must be of "
	       "equal size. In this case, the sizes are " << u.size() << ", "
	       << v.size() << " and " << outcome.size() << " respectively");
    
  infra::vector_base::const_iterator u_iter = u.begin();
  infra::vector_base::const_iterator v_iter = v.begin();
  infra::vector_base::iterator outcome_iter = outcome.begin();
  infra::vector_base::const_iterator u_end = u.end();
  
  while(u_iter < u_end) {
    *outcome_iter = *u_iter - *v_iter;

    ++u_iter;
    ++v_iter;
    ++outcome_iter;
  }
}

//-----------------------------------------------------------------------------
infra::vector_base infra::operator-(const infra::vector_base& u, 
                                    const infra::vector_base& v) {

  // call the prod function 
  infra::vector outcome(u.size());
  diff(u, v, outcome);
  return outcome;
}

//-----------------------------------------------------------------------------
void infra::coordwise_mult(const infra::vector_base& u, 
                           const infra::vector_base& v,
			   infra::vector_base& outcome) {

  infra_assert(u.size() == v.size() && u.size() == outcome.size(),
	       "When calling coordwise_mult(u,v,outcome), all three vectors "
	       << "must be of equal size. In this case, the sizes are "
	       << u.size() << ", " << v.size() << " and " << outcome.size()
	       << " respectively");
    
  infra::vector_base::const_iterator u_iter = u.begin();
  infra::vector_base::const_iterator v_iter = v.begin();
  infra::vector_base::iterator outcome_iter = outcome.begin();
  infra::vector_base::const_iterator u_end = u.end();
  
  while(u_iter < u_end) {
    *outcome_iter = *u_iter * *v_iter;

    ++u_iter;
    ++v_iter;
    ++outcome_iter;
  }
}

//-----------------------------------------------------------------------------
void infra::coordwise_div(const infra::vector_base& u, 
                          const infra::vector_base& v,
			  infra::vector_base& outcome) {

  infra_assert(u.size() == v.size() && u.size() == outcome.size(),
	       "When calling coordwise_div(u,v,outcome), all three vectors "
	       << "must be of equal size. In this case, the sizes are "
	       << u.size() << ", " << v.size() << " and " << outcome.size()
	       << " respectively");
    
  infra::vector_base::const_iterator u_iter = u.begin();
  infra::vector_base::const_iterator v_iter = v.begin();
  infra::vector_base::iterator outcome_iter = outcome.begin();
  infra::vector_base::const_iterator u_end = u.end();
  
  while(u_iter < u_end) {

    infra_assert( *v_iter != 0.0 , "Right hand vector_base has zero elements. "
		  << "Divide by zero error");
    
    *outcome_iter = *u_iter / *v_iter;

    ++u_iter;
    ++v_iter;
    ++outcome_iter;
  }
}

//-----------------------------------------------------------------------------
void infra::dist2(const infra::vector_base& u, const infra::vector_base& v,
		  double& outcome) {

  infra_assert(u.size() == v.size(), "When calling dist(), both vectors "
	       << "must be of equal size. In this case, the sizes are "
	       << u.size() << " and " << v.size() << " respectively");
    
  infra::vector_base::const_iterator u_iter = u.begin();
  infra::vector_base::const_iterator u_end = u.end();
  infra::vector_base::const_iterator v_iter = v.begin();
  
  outcome = 0.0;
  while(u_iter < u_end) {
    outcome += (*u_iter - *v_iter) * (*u_iter - *v_iter);
    ++u_iter;
    ++v_iter;
  }
}

//-----------------------------------------------------------------------------
double infra::dist2(const infra::vector_base& u, const infra::vector_base& v) {

  // call the prod function 
  double outcome;
  dist2(u,v,outcome);
  return outcome;
}

//-----------------------------------------------------------------------------
infra::vector_base infra::operator+(const infra::vector_base& u, 
                                    const double& s) {
  infra::vector new_vector(u);
  new_vector += s;
  return new_vector;
}

//-----------------------------------------------------------------------------
infra::vector_base infra::operator+(const double& s,
                                    const infra::vector_base& u) {
  infra::vector new_vector(u);
  new_vector += s;
  return new_vector;
}

//-----------------------------------------------------------------------------
infra::vector_base infra::operator*(const infra::vector_base& u, 
                                    const double& s) {
  infra::vector new_vector(u);
  new_vector *= s;
  return new_vector;
}

//-----------------------------------------------------------------------------
infra::vector_base infra::operator*(const double& s,
                                    const infra::vector_base& u) {
  infra::vector new_vector(u);
  new_vector *= s;
  return new_vector;
}

//-----------------------------------------------------------------------------
infra::vector_base infra::operator-(const infra::vector_base& u, 
                                    const double& s) {
  infra::vector new_vector(u);
  new_vector -= s;
  return new_vector;
}

//-----------------------------------------------------------------------------
infra::vector_base infra::operator-(const double& s,
                                    const infra::vector_base& u) {
  infra::vector new_vector(u.size());

  infra::vector_base::iterator iter = new_vector.begin();
  infra::vector_base::iterator end = new_vector.end();
  infra::vector_base::const_iterator u_iter = u.begin();

  while(iter < end) {
    *iter = s - *(u_iter);
    ++iter;
    ++u_iter;
  }

  return new_vector;
}

//-----------------------------------------------------------------------------
infra::vector_base infra::operator/(const infra::vector_base& u, 
                                    const double& s) {
  infra::vector new_vector(u);
  new_vector /= s;
  return new_vector;
}

//-----------------------------------------------------------------------------
infra::vector_base infra::operator/(const double& s,
                                    const infra::vector_base& u) {
  infra::vector new_vector(u.size());

  infra::vector_base::iterator iter = new_vector.begin();
  infra::vector_base::iterator end = new_vector.end();
  infra::vector_base::const_iterator u_iter = u.begin();

  while(iter < end) {
    infra_assert( *u_iter != 0.0, "Divide by zero error");
    *iter = s / (*u_iter);
    ++iter;
    ++u_iter;
  }

  return new_vector;
}


//*****************************************************************************
//                                     E O F
//*****************************************************************************
