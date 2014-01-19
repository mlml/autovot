//=============================================================================
// File Name: infra_vm_funcs.cpp
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
#include "infra_vm_funcs.h"
#include "infra_vector.imp"
#include "infra_matrix.imp"
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
  enum CBLAS_ORDER {CblasRowMajor=101, CblasColMajor=102};
  enum CBLAS_TRANSPOSE {CblasNoTrans=111, CblasTrans=112, CblasConjTrans=113};
  void cblas_dgemv(const enum CBLAS_ORDER order,
		   const enum CBLAS_TRANSPOSE TransA, 
                   const int M, 
                   const int N,
		   const double alpha, 
                   const double *A, 
                   const int lda,
		   const double *X, 
                   const int incX, 
                   const double beta,
		   double *Y, 
                   const int incY);
}

#endif //_USE_ATLAS_  
//.............................................................................

//-----------------------------------------------------------------------------
void infra::prod(const infra::matrix_base& M, const infra::vector_base& v,
		 infra::vector_base& outcome) {

  infra_assert(M.width() == v.size() &&  M.height() == outcome.size(),
	       "When calling prod(M,v,outcome), the dimensions of M must "
	       << "equal the sizes of outcome and v respectively. In this "
	       << "case, the dimensions of M are " << M.height() << "x"
	       << M.width() << " and the sizes of outcome and v are "
	       << outcome.size() << " and " << v.size());
    
//.............................................................................
#ifdef _USE_ATLAS_
  cblas_dgemv(CblasColMajor,
	      CblasNoTrans,
	      M.height(),
	      M.width(),
	      1.0,               // a scalar to multiply by
	      M.begin().ptr(),
	      M.allocated_height(),
	      v.begin().ptr(),
	      v.step(),          // v stride
	      0.0,               // a scalar to multiply by
	      outcome.begin().ptr(),
	      outcome.step());   // outcome stride

#else //_USE_ATLAS_  
//.............................................................................

  infra::matrix_base::const_iterator M_iter = M.begin();
  infra::matrix_base::const_iterator M_end = M.end();
  infra::vector_base::const_iterator v_iter = v.begin();
  infra::vector_base::iterator outcome_end = outcome.end();
  
  // initialize outcome vector_base
  outcome.zeros();

  // calculate the multiplication
  while(M_iter < M_end) {
    infra::vector_base::iterator outcome_iter = outcome.begin();
    while(outcome_iter < outcome_end) {
      *outcome_iter += (*M_iter) * (*v_iter);
      ++outcome_iter;
      ++M_iter;
    }
    ++v_iter;
  }
  
#endif //_USE_ATLAS_
}

//-----------------------------------------------------------------------------
infra::vector_base infra::operator* (const infra::matrix_base& M, 
                                     const infra::vector_base& v) {

  infra::vector outcome(M.height());
  prod(M, v, outcome);
  return outcome;
}

//-----------------------------------------------------------------------------
void infra::prod(const infra::vector_base& v, const infra::matrix_base& M,
		 infra::vector_base& outcome) {

  infra_assert(M.height() == v.size() &&  M.width() == outcome.size(),
	       "When calling prod(v,M,outcome), the dimensions of M must "
	       << "equal the sizes of v and outcome respectively. In this "
	       << "case, the dimensions of M are " << M.height() << "x"
	       << M.width() << " and the sizes of v and outcome are "
	       << v.size() << " and " << outcome.size());
    
//.............................................................................
#ifdef _USE_ATLAS_
  cblas_dgemv(CblasColMajor,
	      CblasTrans,
	      M.height(),
	      M.width(),
	      1.0,               // a scalar to multiply by
	      M.begin().ptr(),
	      M.allocated_height(),
	      v.begin().ptr(),
	      v.step(),          // v stride
	      0.0,               // a scalar to multiply by
	      outcome.begin().ptr(),
	      outcome.step());   // outcome stride
  
#else //_USE_ATLAS_  
//.............................................................................

  infra::matrix_base::const_iterator M_iter = M.begin();
  infra::matrix_base::const_iterator M_end = M.end();
  infra::vector_base::const_iterator v_end = v.end();
  infra::vector_base::iterator outcome_iter = outcome.begin();

  // initialize outcome vector_base
  outcome.zeros();

  // calculate the multiplication
  while(M_iter < M_end) {
    infra::vector_base::const_iterator v_iter = v.begin();
    while(v_iter < v_end) {
      *outcome_iter += (*M_iter) * (*v_iter);
      ++v_iter;
      ++M_iter;
    }
    ++outcome_iter;
  }
  
#endif //_USE_ATLAS_
}

//-----------------------------------------------------------------------------
infra::vector_base infra::operator* (const infra::vector_base& v,
                                     const infra::matrix_base& M) {

  infra::vector outcome(M.width());
  prod(v, M, outcome);
  return outcome;
}

//-----------------------------------------------------------------------------
void infra::add_prod(const infra::matrix_base& M, const infra::vector_base& v,
		     infra::vector_base& outcome) {

  infra_assert(M.width() == v.size() &&  M.height() == outcome.size(),
	       "When calling add_prod(M,v,outcome), the dimensions of M must "
	       << "equal the sizes of outcome and v respectively. In this "
	       << "case, the dimensions of M are " << M.height() << "x"
	       << M.width() << " and the sizes of outcome and v are "
	       << outcome.size() << " and " << v.size());

//.............................................................................
#ifdef _USE_ATLAS_
  cblas_dgemv(CblasColMajor,
	      CblasNoTrans,
	      M.height(),
	      M.width(),
	      1.0,               // a scalar to multiply by
	      M.begin().ptr(),
	      M.allocated_height(),
	      v.begin().ptr(),
	      v.step(),          // v stride
	      1.0,               // a scalar to multiply outcome by
	      outcome.begin().ptr(),
	      outcome.step());   // outcome stride

#else //_USE_ATLAS_  
//.............................................................................

  infra::matrix_base::const_iterator M_iter = M.begin();
  infra::matrix_base::const_iterator M_end = M.end();
  infra::vector_base::const_iterator v_iter = v.begin();
  infra::vector_base::iterator outcome_end = outcome.end();

  // calculate the multiplication
  while(M_iter < M_end) {
    infra::vector_base::iterator outcome_iter = outcome.begin();
    while(outcome_iter < outcome_end) {
      *outcome_iter += (*M_iter) * (*v_iter);
      ++outcome_iter;
      ++M_iter;
    }
    ++v_iter;
  }
  
#endif //_USE_ATLAS_
}

//-----------------------------------------------------------------------------
void infra::add_prod(const infra::vector_base& v, const infra::matrix_base& M, 
		     infra::vector_base& outcome) {

  infra_assert(M.height() == v.size() &&  M.width() == outcome.size(),
	       "When calling add_prod(v,M,outcome), the dimensions of M must "
	       << "equal the sizes of v and outcome respectively. In this "
	       << "case, the dimensions of M are " << M.height() << "x"
	       << M.width() << " and the sizes of v and outcome are "
	       << v.size() << " and " << outcome.size());
    
//.............................................................................
#ifdef _USE_ATLAS_
  cblas_dgemv(CblasColMajor,
	      CblasTrans,
	      M.height(),
	      M.width(),
	      1.0,               // a scalar to multiply by
	      M.begin().ptr(),
	      M.allocated_height(),
	      v.begin().ptr(),
	      v.step(),          // v stride
	      1.0,               // a scalar to multiply by
	      outcome.begin().ptr(),
	      outcome.step());   // outcome stride

#else //_USE_ATLAS_  
//.............................................................................

  infra::matrix_base::const_iterator M_iter = M.begin();
  infra::matrix_base::const_iterator M_end = M.end();
  infra::vector_base::const_iterator v_end = v.end();
  infra::vector_base::iterator outcome_iter = outcome.begin();

  // calculate the multiplication
  while(M_iter < M_end) {
    infra::vector_base::const_iterator v_iter = v.begin();
    while(v_iter < v_end) {
      *outcome_iter += (*M_iter) * (*v_iter);
      ++v_iter;
      ++M_iter;
    }
    ++outcome_iter;
  }

#endif //_USE_ATLAS_
}

//-----------------------------------------------------------------------------
void infra::subtract_prod(const infra::matrix_base& M, 
                          const infra::vector_base& v,
			  infra::vector_base& outcome) {
  
  infra_assert(M.width() == v.size() &&  M.height() == outcome.size(),
	       "When calling subtract_prod(M,v,outcome), the dimensions of M "
	       << "must equal the sizes of outcome and v respectively. In "
	       << "this case, the dimensions of M are " << M.height() << "x"
	       << M.width() << " and the sizes of outcome and v are "
	       << outcome.size() << " and " << v.size());
    
//.............................................................................
#ifdef _USE_ATLAS_
  cblas_dgemv(CblasColMajor,
	      CblasNoTrans,
	      M.height(),
	      M.width(),
	      -1.0,               // a scalar to multiply by
	      M.begin().ptr(),
	      M.allocated_height(),
	      v.begin().ptr(),
	      v.step(),          // v stride
	      1.0 ,              // a scalar to multiply outcome by
	      outcome.begin().ptr(),
	      outcome.step());   // outcome stride

#else //_USE_ATLAS_  
//.............................................................................

  infra::matrix_base::const_iterator M_iter = M.begin();
  infra::matrix_base::const_iterator M_end = M.end();
  infra::vector_base::const_iterator v_iter = v.begin();
  infra::vector_base::iterator outcome_end = outcome.end();

  // calculate the multiplication
  while(M_iter < M_end) {
    infra::vector_base::iterator outcome_iter = outcome.begin();
    while(outcome_iter < outcome_end) {
      *outcome_iter -= (*M_iter) * (*v_iter);
      ++outcome_iter;
      ++M_iter;
    }
    ++v_iter;
  }
  
#endif //_USE_ATLAS_
}

//-----------------------------------------------------------------------------
void infra::subtract_prod(const infra::vector_base& v, 
                          const infra::matrix_base& M, 
			  infra::vector_base& outcome) {

  infra_assert(M.height() == v.size() &&  M.width() == outcome.size(),
	       "When calling subtract_prod(v,M,outcome), the dimensions of M "
	       << "must equal the sizes of v and outcome respectively. In "
	       << "this case, the dimensions of M are " << M.height() << "x"
	       << M.width() << " and the sizes of v and outcome are "
	       << v.size() << " and " << outcome.size());
    
//.............................................................................
#ifdef _USE_ATLAS_
  cblas_dgemv(CblasColMajor,
	      CblasTrans,
	      M.height(),
	      M.width(),
	      -1.0,               // a scalar to multiply by
	      M.begin().ptr(),
	      M.allocated_height(),
	      v.begin().ptr(),
	      v.step(),          // v stride
	      1.0,               // a scalar to multiply by
	      outcome.begin().ptr(),
	      outcome.step());   // outcome stride

#else //_USE_ATLAS_  
//.............................................................................

  infra::matrix_base::const_iterator M_iter = M.begin();
  infra::matrix_base::const_iterator M_end = M.end();
  infra::vector_base::const_iterator v_end = v.end();
  infra::vector_base::iterator outcome_iter = outcome.begin();

  // calculate the multiplication
  while(M_iter < M_end) {
    infra::vector_base::const_iterator v_iter = v.begin();
    while(v_iter < v_end) {
      *outcome_iter -= (*M_iter) * (*v_iter);
      ++v_iter;
      ++M_iter;
    }
    ++outcome_iter;
  }

#endif //_USE_ATLAS_
}

//-----------------------------------------------------------------------------
void infra::row_min(const infra::matrix_base& M, infra::vector_base& outcome) {

  infra_assert(M.height() == outcome.size() , "When calling "
	       << "row_min(M,outcome), the height of M must equal the size "
	       << "of outcome. In this case, the dimensions of M are "
	       << M.height() << "x" << M.width() << " and the size of outcome "
	       << "is " << outcome.size());
    
  for(uint i=0; i<M.height(); ++i) {
    outcome[i] = M.row(i).min();
  } 
}

//-----------------------------------------------------------------------------
void infra::row_max(const infra::matrix_base& M, infra::vector_base& outcome) {

  infra_assert(M.height() == outcome.size() , "When calling "
	       << "row_max(M,outcome), the height of M must equal the size "
	       << "of outcome. In this case, the dimensions of M are "
	       << M.height() << "x" << M.width() << " and the size of outcome "
	       << "is " << outcome.size());
    
  for(uint i=0; i<M.height(); ++i) {
    outcome[i] = M.row(i).max();
  } 
}

//-----------------------------------------------------------------------------
void infra::row_sum(const infra::matrix_base& M, infra::vector_base& outcome) {

  infra_assert(M.height() == outcome.size() , "When calling "
	       << "row_sum(M,outcome), the height of M must equal the size "
	       << "of outcome. In this case, the dimensions of M are "
	       << M.height() << "x" << M.width() << " and the size of outcome "
	       << "is " << outcome.size());
    
  for(uint i=0; i<M.height(); ++i) {
    outcome[i] = M.row(i).sum();
  } 
}

//-----------------------------------------------------------------------------
void infra::column_min(const infra::matrix_base& M, 
                       infra::vector_base& outcome) {

  infra_assert(M.width() == outcome.size() , "When calling "
	       << "column_min(M,outcome), the width of M must equal the "
	       << "size of outcome. In this case, the dimensions of M are "
	       << M.height() << "x" << M.width() << " and the size of outcome "
	       << "is " << outcome.size());
    
  for(uint i=0; i<M.width(); ++i) {
    outcome[i] = M.column(i).min();
  } 
}

//-----------------------------------------------------------------------------
void infra::column_max(const infra::matrix_base& M, 
                       infra::vector_base& outcome) {

  infra_assert(M.width() == outcome.size() , "When calling "
	       << "column_max(M,outcome), the width of M must equal the "
	       << "size of outcome. In this case, the dimensions of M are "
	       << M.height() << "x" << M.width() << " and the size of outcome "
	       << "is " << outcome.size());
    
  for(uint i=0; i<M.width(); ++i) {
    outcome[i] = M.column(i).max();
  } 
}

//-----------------------------------------------------------------------------
void infra::column_sum(const infra::matrix_base& M, 
                       infra::vector_base& outcome) {

  infra_assert(M.width() == outcome.size() , "When calling "
	       << "column_sum(M,outcome), the width of M must equal the "
	       << "size of outcome. In this case, the dimensions of M are "
	       << M.height() << "x" << M.width() << " and the size of outcome "
	       << "is " << outcome.size());
    
  for(uint i=0; i<M.width(); ++i) {
    outcome[i] = M.column(i).sum();
  } 
}

//-----------------------------------------------------------------------------
void infra::add_row_vector(infra::matrix_base& M, 
                           const infra::vector_base& v) {

  infra_assert(M.width() == v.size() , "When calling "
	       << "add_row_vector(M,v), the width of M must equal the "
	       << "size of v. In this case, the dimensions of M are "
	       << M.height() << "x" << M.width() << " and the size of v is "
	       << v.size());
    
  for(uint i=0; i<M.height(); ++i) {
    M.row(i) += v;
  } 
}

//-----------------------------------------------------------------------------
void infra::subtract_row_vector(infra::matrix_base& M, 
                                     const infra::vector_base& v) {

  infra_assert(M.width() == v.size() , "When calling "
	       << "subtract_row_vector(M,v), the width of M must equal the "
	       << "size of v. In this case, the dimensions of M are "
	       << M.height() << "x" << M.width() << " and the size of v is "
	       << v.size());
    
  for(uint i=0; i<M.height(); ++i) {
    M.row(i) -= v;
  } 
}

//-----------------------------------------------------------------------------
void infra::add_column_vector(infra::matrix_base& M, 
                              const infra::vector_base& v) {

  infra_assert(M.height() == v.size() , "When calling "
	       << "add_column_vector(M,v), the height of M must equal the "
	       << "size of v. In this case, the dimensions of M are "
	       << M.height() << "x" << M.width() << " and the size of v is "
	       << v.size());
    
  for(uint i=0; i<M.width(); ++i) {
    M.column(i) += v;
  } 
}

//-----------------------------------------------------------------------------
void infra::subtract_column_vector(infra::matrix_base& M, 
                                   const infra::vector_base& v) {

  infra_assert(M.height() == v.size() , "When calling "
	       << "subtract_column_vector(M,v), the height of M must equal "
	       << "the size of v. In this case, the dimensions of M are "
	       << M.height() << "x" << M.width() << " and the size of v is "
	       << v.size());
    
  for(uint i=0; i<M.width(); ++i) {
    M.column(i) -= v;
  } 
}

//*****************************************************************************
//                                     E O F
//*****************************************************************************
