//=============================================================================
// File Name: infra_mm_funcs.cpp
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
#include "infra_mm_funcs.h"
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
  void cblas_dgemm(const enum CBLAS_ORDER Order,
		   const enum CBLAS_TRANSPOSE TransA,
		   const enum CBLAS_TRANSPOSE TransB, 
                   const int M, 
                   const int N,
		   const int K, 
                   const double alpha, 
                   const double *A,
		   const int lda, 
                   const double *B, 
                   const int ldb,
		   const double beta, 
                   double *C, 
                   const int ldc);
}

#endif //_USE_ATLAS_  
//.............................................................................

//-----------------------------------------------------------------------------
void infra::prod(const infra::matrix_base& A, const infra::matrix_base& B,
		 infra::matrix_base& outcome) {

  infra_assert(A.width() == B.height() && A.height() == outcome.height() &&
	        B.width() == outcome.width() ,
	       "The dimensions of the matrices A,B and outcome in the call to "
	       << "prod(A,B,outcome) are inadequate for matrix "
	       << "multiplication. The dimensions of A,B and outcome are "
	       << A.height() << "x" << A.width() << ", "
	       << B.height() << "x" << B.width() << " and "
	       << outcome.height() << "x" << outcome.width()
	       << " respectively");

//.............................................................................
#ifdef _USE_ATLAS_
  cblas_dgemm(CblasColMajor,
	      CblasNoTrans,
	      CblasNoTrans,
	      A.height(),
	      B.width(),
	      A.width(),
	      1.0,               // a scalar to multiply by
	      A.begin().ptr(),
	      A.allocated_height(),
	      B.begin().ptr(),
	      B.allocated_height(),
	      0.0,               // a scalar to multiply by
	      outcome.begin().ptr(),
	      outcome.allocated_height()); 

#else //_USE_ATLAS_  
//.............................................................................

  for(unsigned long i = 0; i < outcome.height(); ++i) {
    for(unsigned long j = 0; j < outcome.width(); ++j) {
      outcome(i,j) = 0.0;
      for(unsigned long k = 0; k < A.width(); ++k) {
	outcome(i,j) += A(i,k) * B(k,j);
      }
    }
  }

#endif //_USE_ATLAS_
}

//-----------------------------------------------------------------------------
infra::matrix_base infra::operator* (const infra::matrix_base& A, 
                                     const infra::matrix_base& B) {

  infra::matrix outcome(A.height(), B.width());
  prod(A, B, outcome);
  return outcome;
}

//-----------------------------------------------------------------------------
void infra::t_prod(const infra::matrix_base& A, const infra::matrix_base& B,
		   infra::matrix_base& outcome) {

  infra_assert(A.height() == B.height() && A.width() == outcome.height() &&
	        B.width() == outcome.width() ,
	       "The dimensions of the matrices A,B and outcome in the call to "
	       << "t_prod(A,B,outcome) are inadequate for matrix "
	       << "multiplication with A transposed. The dimensions of A,B "
	       << "and outcome are "
	       << A.height() << "x" << A.width() << ", "
	       << B.height() << "x" << B.width() << " and "
	       << outcome.height() << "x" << outcome.width()
	       << " respectively");
  
//.............................................................................
#ifdef _USE_ATLAS_
  cblas_dgemm(CblasColMajor,
	      CblasTrans,
	      CblasNoTrans,
	      A.width(),
	      B.width(),
	      A.height(),
	      1.0,               // a scalar to multiply by
	      A.begin().ptr(),
	      A.allocated_height(),
	      B.begin().ptr(),
	      B.allocated_height(),
	      0.0,               // a scalar to multiply by
	      outcome.begin().ptr(),
	      outcome.allocated_height()); 

#else //_USE_ATLAS_  
//.............................................................................

  for(unsigned long i = 0; i < outcome.height(); ++i) {
    for(unsigned long j = 0; j < outcome.width(); ++j) {
      outcome(i,j) = 0.0;
      for(unsigned long k = 0; k < A.height(); ++k) {
	outcome(i,j) += A(k,i) * B(k,j);
      }
    }
  }

#endif //_USE_ATLAS_
}

//-----------------------------------------------------------------------------
void infra::prod_t(const infra::matrix_base& A, const infra::matrix_base& B,
		   infra::matrix_base& outcome) {

  infra_assert(A.width() == B.width() && A.height() == outcome.height() &&
	        B.height() == outcome.width() ,
	       "The dimensions of the matrices A,B and outcome in the call to "
	       << "t_prod(A,B,outcome) are inadequate for matrix "
	       << "multiplication with B transposed. The dimensions of A,B "
	       << "and outcome are "
	       << A.height() << "x" << A.width() << ", "
	       << B.height() << "x" << B.width() << " and "
	       << outcome.height() << "x" << outcome.width()
	       << " respectively");
  
//.............................................................................
#ifdef _USE_ATLAS_
  cblas_dgemm(CblasColMajor,
	      CblasNoTrans,
	      CblasTrans,
	      A.height(),
	      B.height(),
	      A.width(),
	      1.0,               // a scalar to multiply by
	      A.begin().ptr(),
	      A.allocated_height(),
	      B.begin().ptr(),
	      B.allocated_height(),
	      0.0,               // a scalar to multiply by
	      outcome.begin().ptr(),
	      outcome.allocated_height()); 

#else //_USE_ATLAS_  
//.............................................................................

  for(unsigned long i = 0; i < outcome.height(); ++i) {
    for(unsigned long j = 0; j < outcome.width(); ++j) {
      outcome(i,j) = 0.0;
      for(unsigned long k = 0; k < A.width(); ++k) {
	outcome(i,j) += A(i,k) * B(j,k);
      }
    }
  }

#endif //_USE_ATLAS_
}

//-----------------------------------------------------------------------------
void infra::sum(const infra::matrix_base& A, const infra::matrix_base& B,
		infra::matrix_base& outcome) {

  infra_assert(A.height() == B.height() && A.width() == B.width() &&
	       A.height() == outcome.height() && A.width() == outcome.width(),
	       "When calling sum(A,B,outcome), the dimensions of all three "
	       << "matrices must be equal. In this case, the dimensions of A, "
	       << "B and outcome are "
	       << A.height() << "x" << A.width() << ", "
	       << B.height() << "x" << B.width() << " and "
	       << outcome.height() << "x" << outcome.width()
	       << " respectively");
  
  infra::matrix_base::const_iterator A_iter = A.begin();
  infra::matrix_base::const_iterator B_iter = B.begin();
  infra::matrix_base::iterator outcome_iter = outcome.begin();
  infra::matrix_base::const_iterator A_end = A.end();
  
  while(A_iter < A_end) {
    *outcome_iter = *A_iter + *B_iter;

    ++A_iter;
    ++B_iter;
    ++outcome_iter;
  }
}

//-----------------------------------------------------------------------------
infra::matrix_base infra::operator+ (const infra::matrix_base& A, 
                                     const infra::matrix_base& B) {

  infra::matrix outcome(A.height(), B.width());
  sum(A, B, outcome);
  return outcome;
}

//-----------------------------------------------------------------------------
void infra::diff(const infra::matrix_base& A, const infra::matrix_base& B,
		 infra::matrix_base& outcome) {

  infra_assert(A.height() == B.height() && A.width() == B.width() &&
	       A.height() == outcome.height() && A.width() == outcome.width(),
	       "When calling diff(A,B,outcome), the dimensions of all three "
	       << "matrices must be equal. In this case, the dimensions of A, "
	       << "B and outcome are "
	       << A.height() << "x" << A.width() << ", "
	       << B.height() << "x" << B.width() << " and "
	       << outcome.height() << "x" << outcome.width()
	       << " respectively");
  
  infra::matrix_base::const_iterator A_iter = A.begin();
  infra::matrix_base::const_iterator B_iter = B.begin();
  infra::matrix_base::iterator outcome_iter = outcome.begin();
  infra::matrix_base::const_iterator A_end = A.end();
  
  while(A_iter < A_end) {
    *outcome_iter = *A_iter - *B_iter;

    ++A_iter;
    ++B_iter;
    ++outcome_iter;
  }
}

//-----------------------------------------------------------------------------
infra::matrix_base infra::operator- (const infra::matrix_base& A, 
                                     const infra::matrix_base& B) {

  infra::matrix outcome(A.height(), B.width());
  diff(A, B, outcome);
  return outcome;
}

//-----------------------------------------------------------------------------
void infra::coordwise_mult(const infra::matrix_base& A, 
                           const infra::matrix_base& B,
			   infra::matrix_base& outcome) {

  infra_assert(A.height() == B.height() && A.width() == B.width() &&
	       A.height() == outcome.height() && A.width() == outcome.width(),
	       "When calling coordwise_mult(A,B,outcome), the dimensions of "
	       << " all three matrices must be equal. In this case, the "
	       << "dimensions of A, B and outcome are "
	       << A.height() << "x" << A.width() << ", "
	       << B.height() << "x" << B.width() << " and "
	       << outcome.height() << "x" << outcome.width()
	       << " respectively");
  
  infra::matrix_base::const_iterator A_iter = A.begin();
  infra::matrix_base::const_iterator B_iter = B.begin();
  infra::matrix_base::iterator outcome_iter = outcome.begin();
  infra::matrix_base::const_iterator A_end = A.end();
  
  while(A_iter < A_end) {
    *outcome_iter = *A_iter * *B_iter;

    ++A_iter;
    ++B_iter;
    ++outcome_iter;
  }
}

//-----------------------------------------------------------------------------
void infra::coordwise_div(const infra::matrix_base& A, 
                          const infra::matrix_base& B,
			  infra::matrix_base& outcome) {

  infra_assert(A.height() == B.height() && A.width() == B.width() &&
	       A.height() == outcome.height() && A.width() == outcome.width(),
	       "When calling coordwise_div(A,B,outcome), the dimensions of "
	       << " all three matrices must be equal. In this case, the "
	       << "dimensions of A, B and outcome are "
	       << A.height() << "x" << A.width() << ", "
	       << B.height() << "x" << B.width() << " and "
	       << outcome.height() << "x" << outcome.width()
	       << " respectively");
  
  infra::matrix_base::const_iterator A_iter = A.begin();
  infra::matrix_base::const_iterator B_iter = B.begin();
  infra::matrix_base::iterator outcome_iter = outcome.begin();
  infra::matrix_base::const_iterator A_end = A.end();
  
  while(A_iter < A_end) {

    infra_assert( *B_iter != 0.0 , "Right hand matrix has zero elements. "
		  << "Divide by zero error");
    
    *outcome_iter = *A_iter / *B_iter;

    ++A_iter;
    ++B_iter;
    ++outcome_iter;
  }
}

//-----------------------------------------------------------------------------
infra::matrix_base infra::operator+(const infra::matrix_base& u, 
                                    const double& s) {
  infra::matrix new_matrix(u);
  new_matrix += s;
  return new_matrix;
}

//-----------------------------------------------------------------------------
infra::matrix_base infra::operator+(const double& s,
                                    const infra::matrix_base& u) {
  infra::matrix new_matrix(u);
  new_matrix += s;
  return new_matrix;
}

//-----------------------------------------------------------------------------
infra::matrix_base infra::operator*(const infra::matrix_base& u, 
                                    const double& s) {
  infra::matrix new_matrix(u);
  new_matrix *= s;
  return new_matrix;
}

//-----------------------------------------------------------------------------
infra::matrix_base infra::operator*(const double& s,
                                    const infra::matrix_base& u) {
  infra::matrix new_matrix(u);
  new_matrix *= s;
  return new_matrix;
}

//-----------------------------------------------------------------------------
infra::matrix_base infra::operator-(const infra::matrix_base& u, 
                                    const double& s) {
  infra::matrix new_matrix(u);
  new_matrix -= s;
  return new_matrix;
}

//-----------------------------------------------------------------------------
infra::matrix_base infra::operator-(const double& s,
                                    const infra::matrix_base& u) {
  infra::matrix new_matrix(u.height(), u.width());

  infra::matrix_base::iterator iter = new_matrix.begin();
  infra::matrix_base::iterator end = new_matrix.end();
  infra::matrix_base::const_iterator u_iter = u.begin();

  while(iter < end) {
    *iter = s - *(u_iter);
    ++iter;
    ++u_iter;
  }

  return new_matrix;
}

//-----------------------------------------------------------------------------
infra::matrix_base infra::operator/(const infra::matrix_base& u, 
                                    const double& s) {
  infra::matrix new_matrix(u);
  new_matrix /= s;
  return new_matrix;
}

//-----------------------------------------------------------------------------
infra::matrix_base infra::operator/(const double& s,
                                    const infra::matrix_base& u) {
  infra::matrix new_matrix(u.height(), u.width());

  infra::matrix_base::iterator iter = new_matrix.begin();
  infra::matrix_base::iterator end = new_matrix.end();
  infra::matrix_base::const_iterator u_iter = u.begin();

  while(iter < end) {
    infra_assert( *u_iter != 0.0, "Divide by zero error");
    *iter = s / (*u_iter);
    ++iter;
    ++u_iter;
  }

  return new_matrix;
}


//*****************************************************************************
//                                     E O F
//*****************************************************************************
