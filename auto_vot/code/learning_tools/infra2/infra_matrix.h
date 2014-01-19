//=============================================================================
// File Name: infra_matrix.h
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
#ifndef _OFERD_MATRIX_H_
#define _OFERD_MATRIX_H_

//*****************************************************************************
// Included Files
//*****************************************************************************
#include <stdio.h>
#include <iostream>
#include "infra_refcount_darray.h"
#include "infra_vector.h"

namespace infra {
//*****************************************************************************
/** Implements a basic algebric matrix of floating point (double) numbers. 
    @author Ofer Dekel (oferd@cs.huji.ac.il)
*/
class matrix_base 
{
 public:
//=============================================================================
// friend declaration and forward declarations
//=============================================================================
  class const_iterator;  
  class iterator;  

//=============================================================================
// matrix_base Function Declarations
//=============================================================================
//-----------------------------------------------------------------------------
/** Copy constructor 
    @param other A reference to the other matrix_base
*/
  inline matrix_base(const matrix_base& other);

//-----------------------------------------------------------------------------
/** Standard Destructor. 
*/
  inline ~matrix_base();

//-----------------------------------------------------------------------------
/** Swaps the memory and dimensions of this matrix_base with the memory and
    dimensions of another matrix_base.
    @param other The other matrix_base
*/
  inline void swap(matrix_base& other);

//=============================================================================
// Binary file interface
//=============================================================================
//-----------------------------------------------------------------------------
/** Load the matrix_base from a binary file without resizing it. If the matrix
    saved in the file is of a different size than this matrix, an error occurs.
    @param stream The stream from which the matrix_base is read.
    @return A reference to this matrix.
*/
  inline matrix_base& load_binary(FILE* stream);

//-----------------------------------------------------------------------------
/** Save the matrix_base to a binary file
    @param stream The stream to which the matrix_base will be written.
*/
  inline void save_binary(FILE* stream) const;

//=============================================================================
// Other initialization methods
//=============================================================================
//-----------------------------------------------------------------------------
/** Assignment operator.
    @param other The matrix_base being assigned
    @return A reference to this matrix
*/
  inline matrix_base& operator = (const matrix_base& other);

//=============================================================================
// access to matrix_base parameters
//=============================================================================
//----------------------------------------------------------------------------
/** Accesses the matrix_base width
    @return the matrix_base width. 
*/
  inline unsigned long width() const; 
  
//----------------------------------------------------------------------------
/** Accesses the matrix_base height
    @return the matrix_base height. 
*/
  inline unsigned long height() const; 

//----------------------------------------------------------------------------
/** Accesses the matrix_base allocated height. This is useful when directly 
    accessing the matrix memory. 
    @return the matrix_base allocated height. 
*/
  inline unsigned long allocated_height() const; 

//=============================================================================
// reference operators
//=============================================================================
//----------------------------------------------------------------------------
/** Accesses the matrix_base entries
    @return The matrix_base entry at coordinate (i,j)
    @param i The row coordinate
    @param j The column coordinate
*/
  inline double& operator() (unsigned long i, unsigned long j);

//----------------------------------------------------------------------------
/** Accesses the matrix_base entries
    @return The matrix_base entry at coordinate (i,j)
    @param i The row coordinate
    @param j The column coordinate
*/
  inline const double& operator() (unsigned long i, unsigned long j) const;

//=============================================================================
// iterators
//=============================================================================
//-----------------------------------------------------------------------------
/** Returns an iterator that points to entry (0,0) in the matrix_base.
    @return An iterator that points to the first entry in the matrix_base.
*/
  inline iterator begin();

//-----------------------------------------------------------------------------
/** Returns a const_iterator that points to the first entry in
    matrix_base. Similar to the non-const begin().
    @see begin().
*/
  inline const_iterator begin() const;

//-----------------------------------------------------------------------------
/** Returns an iterator that points to one past the location of the last entry
    in the matrix_base. That is, if another iterator were to point to the last
    entry in a matrix_base and the ++ operator would be called, then this
    iterator would equal end().
    @return An iterator that points to one position past the last reachable
    entry in the matrix_base.
*/
  inline iterator end();

//-----------------------------------------------------------------------------
/** STL compatibility - Returns a const_iterator that points to one position
    past the last entry in the matrix_base. Similar to the non-const 
    end().
    @see end().
*/
  inline const_iterator end() const;

//=============================================================================
// views
//=============================================================================

//-----------------------------------------------------------------------------
/** Returns a matrix_base which is a submatrix of this matrix
    @param top The top row of the sub matrix
    @param left The left-most column of the sub matrix
    @param height The height of the submatrix
    @param width The width of the submatrix
    @return The submatrix
*/
  inline matrix_base submatrix(unsigned long top, unsigned long left, 
                               unsigned long height, unsigned long width); 

//-----------------------------------------------------------------------------
/** Returns a const matrix_base which is a submatrix of this matrix
    @param top The top row of the sub matrix
    @param left The left-most column of the sub matrix
    @param height The height of the submatrix
    @param width The width of the submatrix
    @return The submatrix
*/
  inline const matrix_base submatrix(unsigned long top, unsigned long left, 
                            unsigned long height, unsigned long width) const; 

//-----------------------------------------------------------------------------
/** Get a reference to a specific row
*/
  inline vector_base row(unsigned long i);

//-----------------------------------------------------------------------------
/** Get a reference to a specific row
*/
  inline const vector_base row(unsigned long i) const;

//-----------------------------------------------------------------------------
/** Get a reference to a specific column
*/
  inline vector_base column(unsigned long j);

//-----------------------------------------------------------------------------
/** Get a reference to a specific column
*/
  inline const vector_base column(unsigned long j) const;

//-----------------------------------------------------------------------------
/** Get a reference to a specific diagonal in the matrix
    @param k The index of the diagonal, a positive number specifies a 
    diagonal which is above the main diagonal and a negatvie number 
    specifies a diagonal which is below the main diagonal.
*/
  inline vector_base diagonal(long k = 0);

//-----------------------------------------------------------------------------
/** Get a reference to a specific diagonal in the matrix
    @param k The index of the diagonal, a positive number specifies a 
    diagonal which is above the main diagonal and a negatvie number 
    specifies a diagonal which is below the main diagonal.
*/
  inline const vector_base diagonal(long k = 0) const;

//=============================================================================
// comparisons
//=============================================================================
//-----------------------------------------------------------------------------
/** Equality operator
    @param other The matrix_base being compared
    @return 'true' if the two vectrors are identical
*/
  inline bool operator == (const matrix_base& other) const;

//-----------------------------------------------------------------------------
/** Inequality operator
    @param other The matrix_base being compared
    @return 'true' if the two vectrors are not identical
*/
  inline bool operator != (const matrix_base& other) const;

//=============================================================================
// arithmetic operations
//=============================================================================
//-----------------------------------------------------------------------------
/** Set the matrix_base to be all zero
    @return A reference to this matrix.
*/
  inline matrix_base& zeros();
  
//-----------------------------------------------------------------------------
/** Set the matrix_base to be all ones
    @return A reference to this matrix.
*/
  inline matrix_base& ones();
  
//-----------------------------------------------------------------------------
/** Set the matrix_base to be the identity matrix_base
    @return A reference to this matrix.
*/
  inline matrix_base& eye();
  
//-----------------------------------------------------------------------------
/** Sets all elements of a matrix to equal a scalar.
    @param scalar The scalar 
    @return A reference to this matrix.
*/
  inline matrix_base& operator = (const double& scalar);
  
//-----------------------------------------------------------------------------
/** Adds a scalar to all entries in the matrix_base.
    @param scalar The scalar to add to all entries
    @return A reference to this matrix.
*/
  inline matrix_base& operator += (const double& scalar);
  
//-----------------------------------------------------------------------------
/** Subtracts a scalar from all entries in the matrix_base.
    @param scalar The scalar to subtract from the matrix_base entries
    @return A reference to this matrix.
*/
  inline matrix_base& operator -= (const double& scalar);
  
//-----------------------------------------------------------------------------
/** Self-multipilcation by a scalar. Multiplies all of the matrix_base entries
    by some scalar.
    @param scalar The scalar to multiply all of the matrix_base by
    @return A reference to this matrix.
*/
  inline matrix_base& operator *= (const double& scalar);
  
//-----------------------------------------------------------------------------
/** Self-division by a scalar. Divides all of the matrix_base entries
    by some scalar.
    @param scalar The scalar to divide all of the matrix_base by
    @return A reference to this matrix.
*/
  inline matrix_base& operator /= (const double& scalar);
  
//-----------------------------------------------------------------------------
/** Adds another matrix_base to this matrix_base.
    @param other The other matrix_base to add to this one.
    @return A reference to this matrix.
*/
  inline matrix_base& operator += (const matrix_base& other);
  
//-----------------------------------------------------------------------------
/** Subtracts another matrix_base from this matrix_base.
    @param other The other matrix_base to subtract from this one.
    @return A reference to this matrix.
*/
  inline matrix_base& operator -= (const matrix_base& other);
  
//-----------------------------------------------------------------------------
/** Coordinate-wise multiplication with another matrix_base
    @param other The other matrix_base to multiply (coord-wise) with this one.
    @return A reference to this matrix.
*/
  inline matrix_base& operator *= (const matrix_base& other);
  
//-----------------------------------------------------------------------------
/** Coordinate-wise division with another matrix_base
    @param other The other matrix_base to divide (coord-wise) this one by.
    @return A reference to this matrix.
*/
  inline matrix_base& operator /= (const matrix_base& other);
  
//-----------------------------------------------------------------------------
/** Coordiante wise exponent. Changes every element a_ij of the matrix to be
    exp(a_ij + scalar). The outcome is clipped at 1.0e+128.
    @param scalar An additive shift of the exponent
    @return A reference to this matrix.
*/
  inline matrix_base& exp(const double& scalar = 0.0);
  
//-----------------------------------------------------------------------------
/** Changes every element v_i of the matrix_base to be log(v_i) / log(scalar)
    @param scalar The logarithm base
    @return A reference to this matrix.
*/
  inline matrix_base& log(const double scalar = 2.718281828459);
  
//-----------------------------------------------------------------------------
/** Coordiante wise power. Changes every element a_ij of the matrix_base to be
    a_ij^scalar.
    @param scalar The power by which to raise every matrix_base element.
    @return A reference to this matrix.
*/
  inline matrix_base& pow(const double& scalar);
  
//-----------------------------------------------------------------------------
/** Returns the maximal value in the matrix_base
    @return The maximal value in the matrix_base
*/
  inline double max() const;
  
//-----------------------------------------------------------------------------
/** Returns the minimum value in the matrix_base
    @return The minimum value in the matrix_base
*/
  inline double min() const;
  
//=============================================================================
// structural operations
//=============================================================================
//-----------------------------------------------------------------------------
/** Swaps rows i and j
    @param i The index of the first row
    @param j The index of the second row
    @return A reference to this matrix.
*/
  inline matrix_base& swap_rows(unsigned long i, unsigned long j);

//-----------------------------------------------------------------------------
/** Swaps columns i and j
    @param i The index of the first column
    @param j The index of the second column
    @return A reference to this matrix.
*/
  inline matrix_base& swap_columns(unsigned long i, unsigned long j);

protected:
//-----------------------------------------------------------------------------
/** A protected function used by infra::matrix to create the underlying
    matrix_base.
*/
  inline matrix_base(unsigned long height, unsigned long width, 
                     double* data, infra::refcount_darray memory, 
                     unsigned long allocated_height, 
                     unsigned long allocated_width) :
    _height(height), _width(width), _data(data), _memory(memory), 
    _allocated_height(allocated_height), _allocated_width(allocated_width) {}

//=============================================================================
// matrix_base Data Members
//=============================================================================
  unsigned long _height;
  unsigned long _width;
  double* _data;
  infra::refcount_darray _memory;
  unsigned long _allocated_height;
  unsigned long _allocated_width;
};

//*****************************************************************************
// const_iterator internal class
//*****************************************************************************
//-----------------------------------------------------------------------------
/** Implements a constant iterator to a matrix_base
*/
class matrix_base::const_iterator {
  friend class matrix_base;
 public:
//-----------------------------------------------------------------------------
/** Copy constructor
    @param other A reference to the const_iterator being copied
*/
  inline const_iterator(const const_iterator& other);

//-----------------------------------------------------------------------------
/** operator < - determines which of two const_iterators is lower in memory
    @param other A reference to the const_iterator to compare with
*/
  inline bool operator < (const matrix_base::const_iterator& other) const;

//-----------------------------------------------------------------------------
/** operator != - determines if two const_iterators are the different
    @param other A reference to the const_iterator to compare with
*/
  inline bool operator != (const matrix_base::const_iterator& other) const;

//-----------------------------------------------------------------------------
/** operator != - determines if two const_iterators are the same
    @param other A reference to the const_iterator to compare with
*/
  inline bool operator == (const matrix_base::const_iterator& other) const;

//-----------------------------------------------------------------------------
/** Reference operator, constant version
    @return A constant reference to the entry pointed to.
*/
  inline const double& operator*();

//-----------------------------------------------------------------------------
/** Reference operator, constant version
    @return A constant pointer to the entry pointed to.
*/
  inline const double* operator->();

//-----------------------------------------------------------------------------
/** Reference operator, constant version
    @return A constant pointer to the entry pointed to.
*/
  inline const double* ptr();

//-----------------------------------------------------------------------------
/** operator++, prefix version. Iterates in column-wise order.
*/
  inline const_iterator operator++();

//-----------------------------------------------------------------------------
/** operator++, postfix version. Iterates in column-wise order.
*/
  inline const_iterator operator++(int);

//-----------------------------------------------------------------------------
/** operator+=, increments the iterator by a given offset
*/
  inline const_iterator operator+=(unsigned long offset);

//-----------------------------------------------------------------------------
/** operator--, prefix version. Iterates in column-wise order.
*/
  inline const_iterator operator--();

//-----------------------------------------------------------------------------
/** operator--, postfix version. Iterates in column-wise order.
*/
  inline const_iterator operator--(int);

//-----------------------------------------------------------------------------
/** move right
*/
  inline const_iterator right();

//-----------------------------------------------------------------------------
/** move right
*/
  inline const_iterator left();

//-----------------------------------------------------------------------------
/** move right
*/
  inline const_iterator up();

//-----------------------------------------------------------------------------
/** move right
*/
  inline const_iterator down();

//-----------------------------------------------------------------------------
/** get the row of the current element
    @return The row of the current element
*/
  inline unsigned long row();

//-----------------------------------------------------------------------------
/** get the column of the current element
    @return The column of the current element
*/
  inline unsigned long column();

 protected:
//-----------------------------------------------------------------------------
/** Constructs a const_iterator
    @param ptr A pointer to double 
    @param step The step size of the matrix_base
*/
  inline const_iterator(double* ptr, unsigned long row, unsigned long column, 
                        unsigned long matrix_height, 
                        unsigned long allocated_height);

//=============================================================================
// const_iterator Data Members
//=============================================================================
  double* _ptr; 
  unsigned long _row; 
  unsigned long _column;
  unsigned long _matrix_height;
  unsigned long _allocated_height;
};

//*****************************************************************************
// iterator internal class
//*****************************************************************************
//-----------------------------------------------------------------------------
/** Implements an iterator to a matrix_base. 
*/
class matrix_base::iterator : public matrix_base::const_iterator {
  friend class matrix_base;
public:
//-----------------------------------------------------------------------------
/** Copy constructor
    @param other A reference to the iterator being copied
*/
  inline iterator(const iterator& other);

//-----------------------------------------------------------------------------
/** Reference operator
    @return A reference to the entry pointed to.
*/
  inline double& operator*();

//-----------------------------------------------------------------------------
/** Reference operator
    @return A pointer to the entry pointed to.
*/
  inline double* operator->();

//-----------------------------------------------------------------------------
/** Reference operator
    @return A pointer to the entry pointed to.
*/
  inline double* ptr();

 protected:
//-----------------------------------------------------------------------------
/** Constructs a const_iterator
    @param ptr A pointer to double 
    @param step The step size of the matrix_base
*/
  inline iterator(double* ptr, unsigned long row, unsigned long column,
                  unsigned long matrix_height, unsigned long allocated_height);
};

//*****************************************************************************
// the matrix_view class
//*****************************************************************************
/** Implements a matrix_view, i.e. a matrix which references another matrix's 
    memory.
    @author Ofer Dekel (oferd@cs.huji.ac.il)
*/
typedef matrix_base matrix_view;

//*****************************************************************************
// the matrix class
//*****************************************************************************
/** Implements a basic algebric matrix of floating point (real) numbers. 
    @author Ofer Dekel (oferd@cs.huji.ac.il)
*/
class matrix : public matrix_base
{
//=============================================================================
// matrix Function Declarations
//=============================================================================
 public:
//-----------------------------------------------------------------------------
/** Constructs a matrix of given height and width
    @param height The matrix height
    @param width The matrix width
*/
  inline explicit matrix(unsigned long height = 0, unsigned long width = 0);

//-----------------------------------------------------------------------------
/** Copy constructor 
    @param other A reference to the other matrix
*/
  inline matrix(const matrix& other);

//-----------------------------------------------------------------------------
/** Copy constructor 
    @param other A reference to the other matrix
*/
  inline matrix(const matrix_base& other);

//-----------------------------------------------------------------------------
/** Standard Destructor. 
*/
  inline ~matrix();

//-----------------------------------------------------------------------------
/** Resizes the matrix by reallocating its memory. The original coordiantes
    that still fit in the new matrix dimensions remain intact. New
    coordinates added to the matrix are not initialized. Note that if any 
    matrix_view pointing to this matrix is unaffected by this: the view keeps 
    the old memory while this matrix gets new memory.
    @param height The new height of the matrix
    @param width The new width of the matrix
    @return A reference to this matrix.
*/
  inline matrix_base& resize(unsigned long height, unsigned long width);

//-----------------------------------------------------------------------------
/** Reshape does not reallocate memory. It mearly changes the dimensions of
    the matrix and redistributes the matrix elements in column-wise order.
    For example, the matrix [a c ; b d] can be reshaped to be [a b c d].
    The product of the new height and new weight must equal the product
    of the original height and width.
    @param height The new height of the matrix
    @param width The new width of the matrix
    @return A reference to this matrix.
*/
  inline matrix_base& reshape(unsigned long height, unsigned long width);
  
//-----------------------------------------------------------------------------
/** Sets all elements of a matrix to equal a scalar.
    @param scalar The scalar 
    @return A reference to this matrix.
*/
  inline matrix_base& operator = (const double& scalar);

//=============================================================================
// File interface
//=============================================================================
//-----------------------------------------------------------------------------
/** Load a double matrix from a binary file
    @param stream The stream that the matrix is to be read from. 
*/
  inline matrix(FILE* stream);

//-----------------------------------------------------------------------------
/** Load a matrix from an input stream
    @param stream The stream that the matrix is to be read from. 
*/
  inline matrix(std::istream& stream);

//=============================================================================
// A protected function
//=============================================================================
protected:
//-----------------------------------------------------------------------------
/** A protected function which enables children of matrix to play with its
    construction - use at your own risk.
*/
  inline matrix(unsigned long height, unsigned long width, 
		double* data, infra::refcount_darray memory, 
		unsigned long allocated_height, 
		unsigned long allocated_width) :
    matrix_base(height,width,data,memory,allocated_height,allocated_width) {}
};

//*****************************************************************************
// declaration of stream operators
//*****************************************************************************
//----------------------------------------------------------------------------
/** Output streaming operator for matrix_base.
    @return The output stream, to allow concatinated output.
    @param os The output stream to print to.
    @param mat A reference to the matrix_base being printed.
*/
inline std::ostream& operator<< (std::ostream& os, 
                                 const infra::matrix_base& m);

//----------------------------------------------------------------------------
/** Input streaming operator for matrix_base without resizing it. If the matrix
    saved in the file is of a different size than this matrix, an error occurs.
    @return The input stream, to allow concatinated input.
    @param is The input stream to read from.
    @param mat A reference to the matrix_base being read.
*/
inline std::istream& operator>> (std::istream& is, infra::matrix_base& m);
};
#endif
//*****************************************************************************
//                                     E O F
//*****************************************************************************
