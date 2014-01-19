//=============================================================================
// File Name: kernels.h
// Written by: Shai Shalev-Shwartz (shais@cs.huji.ac.il)
//
// Distributed as part of the kernels library
// Copyright (C) 2006 Shai Shalev-Shwartz
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

//=============================================================================
// File Name: kernels.h
// Written by: Shai Shalev-Shwartz
//
//=============================================================================
#ifndef _SHAI_KERNELS_H_
#define _SHAI_KERNELS_H_

// Included Files
#include <infra.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cstdlib>

//*****************************************************************************
/** Namespace for various kernel operators.
 */
namespace kernels {
  
  
  /** A basic class for kernels. 
   */
  class BaseKernel {
  public:
    
    //-----------------------------------------------------------------------------
    /** operator () for vectors
     @param v1 the first vector
     @param v2 the second vector
     @returns the kernel operator applied to the two vectors
     */
    virtual double operator() (const infra::vector_view v1, const infra::vector_view v2) const = 0;
    
    //-----------------------------------------------------------------------------
    /** operator () for a matrices
     @param M1 a row-wise matrix of vectors
     @param M2 a row-wise matrix of vectors
     @returns The Gram matrix M1*M2' calculated with respect to the kernel
     operator. Coordiante [i,j] of the outcome is the i'th row of M1
     multplied by the j'tj row of M2
     */
    virtual infra::matrix_view operator() (const infra::matrix_view M1, const infra::matrix_view M2) const = 0;
    
    //-----------------------------------------------------------------------------
    /** operator () for a matrix and a vector
     @param M a row-wise matrix of vectors
     @param v a vector
     @returns The vector of inner products of each row in M and v, using the 
     kernel operator. 
     */
    virtual infra::vector_view operator() (const infra::matrix_view M,
                                           const infra::vector_view v) const = 0;
    
    
    /** save_binary
     @param fid Pointer to FILE
     */
    void save_binary(FILE * fid) {
      param_.save_binary(fid);
    }
    
    virtual ~BaseKernel() { }
    
  protected:
    /** Constructor.
     @param v A vector_view of the paraemeters
     */
    BaseKernel(infra::vector_view v) : param_(v) { }
    
    
    
    infra::vector param_;
    
  };
  
  
  
  
  //*****************************************************************************
  /** Implements a linear kernel
   Inherited from BaseKernel
   */
  class linear : public BaseKernel {
  public:
    //-----------------------------------------------------------------------------
    /** Constructor, K(x,y) = v(0) * (x*y + v(1))
     */
    linear(infra::vector_view v) : BaseKernel(v) { 
      bool wrong_parameters = false;
      if (v.size() != 2) {
        wrong_parameters = true;
      } else if (v(0) <= 0 || v(1) < 0) {
        wrong_parameters = true;
      }
      if (wrong_parameters) {
        std::cerr << "The parameter vector:\n" << v << "is wrong for linear kernel.\n" 
        << "Aborting." << std::endl;
        exit(EXIT_FAILURE);
      }
      
      _b = param_(1); _s = param_(0); 
    }
    
    //-----------------------------------------------------------------------------
    /** operator () for vectors
     @param v1 the first vector
     @param v2 the second vector
     @returns the kernel operator applied to the two vectors
     */
    inline double operator() (const infra::vector_view v1, const infra::vector_view v2) const;
    
    //-----------------------------------------------------------------------------
    /** operator () for a matrices
     @param M1 a row-wise matrix of vectors
     @param M2 a row-wise matrix of vectors
     @returns The Gram matrix M1*M2' calculated with respect to the kernel
     operator. Coordiante [i,j] of the outcome is the i'th row of M1
     multplied by the j'tj row of M2
     */
    inline infra::matrix_view operator() (const infra::matrix_view M1,
                                          const infra::matrix_view M2) const;
    
    //-----------------------------------------------------------------------------
    /** operator () for a matrix and a vector
     @param M a row-wise matrix of vectors
     @param v a vector
     @returns The vector of inner products of each row in M and v, using the 
     kernel operator. 
     */
    inline infra::vector_view operator() (const infra::matrix_view M,
                                          const infra::vector_view v) const;
    
    ~linear() { }
    
    //=============================================================================
    // kernels Function Declarations
    //=============================================================================
  protected:
    double _b;
    double _s;
  };
  
  //*****************************************************************************
  /** Implements a polynomial kernel
   
   */
  class polynomial : public BaseKernel {
  public:
    //-----------------------------------------------------------------------------
    /** Constructor K(x,y) = s * (x*y + b)^d = v(0) * (x*y + v(1))^(v(2))
     */
    polynomial(infra::vector_view v) : BaseKernel(v) {
      bool wrong_parameters = false;
      if (v.size() != 3) {
        wrong_parameters = true;
      } else if (v(0) <= 0 || v(1) < 0 || v(2) <= 0) {
        wrong_parameters = true;
      }
      if (wrong_parameters) {
        std::cerr << "The parameter vector:\n" << v << "is wrong for polynomial kernel.\n" 
        << "Aborting." << std::endl;
        exit(EXIT_FAILURE);
      }
      _d = v(2); _b = v(1); _s = v(0);
    }
    
    //-----------------------------------------------------------------------------
    /** operator () for vectors
     @param v1 the first vector
     @param v2 the second vector
     @returns the kernel operator applied to the two vectors
     */
    inline double operator() (const infra::vector_view v1, const infra::vector_view v2) const;
    
    //-----------------------------------------------------------------------------
    /** operator () for a matrices
     @param M1 a row-wise matrix of vectors
     @param M2 a row-wise matrix of vectors
     @returns The Gram matrix M1*M2' calculated with respect to the kernel
     operator. Coordiante [i,j] of the outcome is the i'th row of M1
     multplied by the j'tj row of M2
     */
    inline infra::matrix_view operator() (const infra::matrix_view M1,
                                          const infra::matrix_view M2) const;
    
    //-----------------------------------------------------------------------------
    /** operator () for a matrix and a vector
     @param M a row-wise matrix of vectors
     @param v a vector
     @returns The vector of inner products of each row in M and v, using the 
     kernel operator. 
     */
    inline infra::vector_view operator() (const infra::matrix_view M,
                                          const infra::vector_view v) const;
    
    
    ~polynomial() { }
    
    //=============================================================================
    // kernels Function Declarations
    //=============================================================================
  protected:
    double _d;
    double _b;
    double _s;
  };
  
  //*****************************************************************************
  /** Implements a gaussian kernel
   */
  class gaussian : public BaseKernel {
  public:
    //-----------------------------------------------------------------------------
    /** Constructor K(x,y) = v(0) * exp( - ||x-y||^2 / 2 v(1))
     */
    gaussian(infra::vector_view v) : BaseKernel(v) {
      bool wrong_parameters = false;
      if (v.size() != 2) {
        wrong_parameters = true;
      } else if (v(0) <= 0 || v(1) <= 0) {
        wrong_parameters = true;
      }
      if (wrong_parameters) {
        std::cerr << "The parameter vector:\n" << v << "is wrong for gaussian kernel.\n" 
        << "Aborting." << std::endl;
        exit(EXIT_FAILURE);
      }
      
      _s = log(param_(0)); _sigma2  = param_(1);
    }
    
    //-----------------------------------------------------------------------------
    /** operator () for vectors
     @param v1 the first vector
     @param v2 the second vector
     @returns the kernel operator applied to the two vectors
     */
    inline double operator() (const infra::vector_view v1, const infra::vector_view v2) const;
    
    //-----------------------------------------------------------------------------
    /** operator () for a matrices
     @param M1 a row-wise matrix of vectors
     @param M2 a row-wise matrix of vectors
     @returns The Gram matrix M1*M2' calculated with respect to the kernel
     operator. Coordiante [i,j] of the outcome is the i'th row of M1
     multplied by the j'tj row of M2
     */
    inline infra::matrix_view operator() (const infra::matrix_view M1,
                                          const infra::matrix_view M2) const;
    
    //-----------------------------------------------------------------------------
    /** operator () for a matrix and a vector
     @param M a row-wise matrix of vectors
     @param v a vector
     @returns The vector of inner products of each row in M and v, using the 
     kernel operator. 
     */
    inline infra::vector_view operator() (const infra::matrix_view M,
                                          const infra::vector_view v) const;
    
    ~gaussian() { }
    
    //=============================================================================
    // kernels Function Declarations
    //=============================================================================
  protected:
    double _sigma2;
    double _s;
  };
  
  
  
  //*****************************************************************************
  /** Implements a chi2 kernel
   */
  class chi2 : public BaseKernel {
  public:
    //-----------------------------------------------------------------------------
    /** Constructor K(x,y) = v(0) * exp( - v(1) chi2(x,y) )
        where chi2(x,y) = sum_i (x_i - y_i)^2 / (x_i + y_i)
     */
    chi2(infra::vector_view v) : BaseKernel(v) {
      bool wrong_parameters = false;
      if (v.size() != 2) {
        wrong_parameters = true;
      } else if (v(0) <= 0 || v(1) <= 0) {
        wrong_parameters = true;
      }
      if (wrong_parameters) {
        std::cerr << "The parameter vector:\n" << v << "is wrong for chi2 kernel.\n" 
        << "Aborting." << std::endl;
        exit(EXIT_FAILURE);
      }
      
      _s = param_(0); _gamma  = param_(1);
    }
    
    //-----------------------------------------------------------------------------
    /** operator () for vectors
     @param v1 the first vector
     @param v2 the second vector
     @returns the kernel operator applied to the two vectors
     */
    inline double operator() (const infra::vector_view v1, const infra::vector_view v2) const;
    
    //-----------------------------------------------------------------------------
    /** operator () for a matrices
     @param M1 a row-wise matrix of vectors
     @param M2 a row-wise matrix of vectors
     @returns The Gram matrix M1*M2' calculated with respect to the kernel
     operator. Coordiante [i,j] of the outcome is the i'th row of M1
     multplied by the j'tj row of M2
     */
    inline infra::matrix_view operator() (const infra::matrix_view M1,
                                          const infra::matrix_view M2) const;
    
    //-----------------------------------------------------------------------------
    /** operator () for a matrix and a vector
     @param M a row-wise matrix of vectors
     @param v a vector
     @returns The vector of inner products of each row in M and v, using the 
     kernel operator. 
     */
    inline infra::vector_view operator() (const infra::matrix_view M,
                                          const infra::vector_view v) const;
    
    ~chi2() { }
    
    //=============================================================================
    // kernels Function Declarations
    //=============================================================================
  protected:
    double _gamma;
    double _s;
  };
  
  
  /** A builder for kernels. This gives a simple interface for code that needs
   to use a kernel but gets the name of the kernel in runtime. 
   For example, you can create a file named linear.kernel that contains:
   
   linear
   
   2 1 0
   
   Then, in your code, suppose that kernel_filename is of type std::string and contains
   the string "linear". You write:
   
   kernels::KernelsBuilder K(kernel_filename);
   
   Now, you can do things like 
   
   double x_norm = K(x,x); 
   
   infra::vector_view v = K(M,x);
   
   */
  class KernelsBuilder {
  public:
    
    enum MyKernelsType { LINEAR = 0 , POLYNOMIAL = 1, GAUSSIAN = 2, CHI2 = 3 };
    
    /** A constructor. 
     @param file_name A name of a file with the name of the kernel and a text infra vector with the parameters.
     Use "linear" for linear kernel, "polynomial" for polynomail kernel and "gaussian" for Gaussian kernel.
     */
    KernelsBuilder(std::string file_name) : K(NULL), smart_pointer_counter(NULL) {
      if (file_name == "default") {
        infra::vector v(2); v(0) = 1.0; v(1) = 0.0;
        kernel_code = LINEAR;
        AllocateBaseKernel(kernel_code,v);
      } else { 
        // open the file
        std::ifstream kernel_file(file_name.c_str());
        
        // read the name of the kernel
        std::string s; 
        kernel_file >> s;
        
        // read the infra::vector of paramters
        infra::vector v(kernel_file);
        
        // construct the kernel
        if (s == "linear") {
          kernel_code = LINEAR;
        } else if (s == "polynomial") {
          kernel_code = POLYNOMIAL;
        } else if (s == "gaussian") {
          kernel_code = GAUSSIAN;
        } else if (s == "chi2") {
          kernel_code = CHI2;
        }
        
        AllocateBaseKernel(kernel_code,v);
        
      }
    }
    
    
    /** A constructor. 
     @param fid A pointer to FILE. 
     */
    KernelsBuilder(FILE * fid) : K(NULL), smart_pointer_counter(NULL) {
      
      // read the code for the kernel
      infra::load_binary(fid, kernel_code);
      
      // read the infra::vector of parameters
      infra::vector v(fid);
      
      AllocateBaseKernel(kernel_code,v);
      
    }
    
    
    /** Copy constructor. 
     @param other Other KerenlsBuilder to copy. 
     */
    KernelsBuilder(const KernelsBuilder& other) : K(other.K) , 
    smart_pointer_counter(other.smart_pointer_counter), kernel_code(other.kernel_code) {
      *smart_pointer_counter += 1;
    }
    
    
    /** save to binary file.
     @param fid Pointer to FILE.
     */
    void save_binary(FILE * fid) {
      infra::save_binary(fid,kernel_code); // write the code of the kernel
      K->save_binary(fid); // write the parameter vectors to file
    }
    
    
    //-----------------------------------------------------------------------------
    /** operator () for vectors
     @param v1 the first vector
     @param v2 the second vector
     @returns the kernel operator applied to the two vectors
     */
    double operator() (const infra::vector_view v1, const infra::vector_view v2) const {return((*K)(v1,v2)); }
    
    //-----------------------------------------------------------------------------
    /** operator () for a matrices
     @param M1 a row-wise matrix of vectors
     @param M2 a row-wise matrix of vectors
     @returns The Gram matrix M1*M2' calculated with respect to the kernel
     operator. Coordiante [i,j] of the outcome is the i'th row of M1
     multplied by the j'tj row of M2
     */
    infra::matrix_view operator() (const infra::matrix_view M1,
                                   const infra::matrix_view M2) const {return((*K)(M1,M2)); }
    
    //-----------------------------------------------------------------------------
    /** operator () for a matrix and a vector
     @param M a row-wise matrix of vectors
     @param v a vector
     @returns The vector of inner products of each row in M and v, using the 
     kernel operator. 
     */
    infra::vector_view operator() (const infra::matrix_view M,
                                   const infra::vector_view v) const {return((*K)(M,v)); }
    
    
    ~KernelsBuilder() { 
      *smart_pointer_counter -= 1;
      if (*smart_pointer_counter == 0) { 
        delete K; 
        delete smart_pointer_counter;
      }
    }
    
  protected:
    
    void AllocateBaseKernel(short kernel_code, infra::vector_view v) {
      
      // allocate the kernel
      if (kernel_code == LINEAR) {
        K = new linear(v);
      } else if (kernel_code == POLYNOMIAL) {
        K = new polynomial(v); 
      } else if (kernel_code == GAUSSIAN) {
        K = new gaussian(v);
      } else if (kernel_code == CHI2) {
        K = new chi2(v);        
      } else {
        std::cerr << kernel_code << " is an unknown kernel code. Abort" << std::endl;
        exit(EXIT_FAILURE);
      }
      
      // allocate the smart_pointer_counter 
      smart_pointer_counter = new int(1);
      (*smart_pointer_counter) = 1;
    }
    
    
    BaseKernel * K; // pointer to the actual kernel we're going to use
    int * smart_pointer_counter; // counter for knowing if to delete K
    short kernel_code;
    
  };
  
  
  
  
  
};
#endif
//*****************************************************************************
//                                     E O F
//*****************************************************************************
