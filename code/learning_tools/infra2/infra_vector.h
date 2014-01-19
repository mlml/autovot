//=============================================================================
// File Name: infra_vector.h
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
#ifndef _OFERD_VECTOR_BASE_H_
#define _OFERD_VECTOR_BASE_H_

//*****************************************************************************
// Included Files
//*****************************************************************************
#include <iostream>
#include <stdio.h>
#include "infra_refcount_darray.h"

typedef unsigned int uint;

namespace infra {
//=============================================================================
// forward declarations
//=============================================================================
class matrix_base;

//*****************************************************************************
/** Implements a basic algebric vector of doubles. This classname
    should not be used directly, instead use infra::vector or
    infra::vector_view.
    @author Ofer Dekel (oferd@cs.huji.ac.il)
*/
class vector_base {
 public:
//=============================================================================
// friend declaration and forward declarations
//=============================================================================
  friend class matrix_base;
  class const_iterator;
  class iterator;

//=============================================================================
// vector_base Function Declarations
//=============================================================================

//-----------------------------------------------------------------------------
/** Copy constructor. References the memory of the other vector, DOES NOT
    allocate any new memory. Both this vector and the vector being copied 
    point to the same location in memory.
    @param other A reference to the vector_base being referenced
*/
  inline vector_base(const vector_base& other);

//-----------------------------------------------------------------------------
/** Standard Destructor. 
*/
  inline ~vector_base();

//=============================================================================
// Binary file interface
//=============================================================================
//-----------------------------------------------------------------------------
/** Load the vector_base from a binary file without resizing it. If the vector
    saved in the file is of a different size than this vector, an error occurs.
    @param stream The stream from which the vector_base is read.
    @return A reference to this vector.
*/
  inline vector_base& load_binary(FILE* stream);

//-----------------------------------------------------------------------------
/** Save the vector_base to a binary file
    @param stream The stream to which the vector_base will be written.
*/
  inline void save_binary(FILE* stream) const;

//=============================================================================
// Other initialization methods
//=============================================================================
//-----------------------------------------------------------------------------
/** Assignment operator. This operator assigns the values of this vector to
    equal those of the other vector. This function will not change the size of
    this vector. Both vectors must be of equal size, otherwise the operation
    will fail.  
    @param other The vector_base being assigned 
    @return A reference to this vector
*/
  inline vector_base& operator = (const vector_base& other);

//-----------------------------------------------------------------------------
/** copy_neg is like the operator =, but it assigns this vector_base with a 
    copy of the negative of the other vector_base. this = -other.
    @param other The vector_base being assigned
    @return A reference to this vector
*/
  inline vector_base& copy_neg (const vector_base& other);

//=============================================================================
// access to vector_base parameters
//=============================================================================
//----------------------------------------------------------------------------
/** Accesses the vector_base size.
    @return the vector_base size
*/
  inline unsigned long size() const; 
  
//----------------------------------------------------------------------------
/** Accesses the vector_base step size.
    @return the vector_base step size
*/
  inline unsigned long step() const; 
  
//=============================================================================
// reference operators
//=============================================================================
//-----------------------------------------------------------------------------
/** Reference operator. Returns a reference to an entry in the vector_base
    @param i The index to the requested entry
    @returns A reference to the requested vector_base entry
*/
  inline double& operator[] (unsigned long i);

//-----------------------------------------------------------------------------
/** Reference operator. Returns a refetence to an entry in the vector_base
    @param i The index to the requested entry
    @returns A reference to the requested vector_base entry
*/
  inline const double& operator[] (unsigned long i) const;

//-----------------------------------------------------------------------------
/** Reference operator. Returns a refetence to an entry in the vector_base
    @see operator[]
*/
  inline double& operator() (unsigned long i);

//-----------------------------------------------------------------------------
/** Reference operator. Returns a refetence to an entry in the vector_base
    @see operator[]
*/
  inline const double& operator() (unsigned long i) const;

//-----------------------------------------------------------------------------
/** Returns a reference to the first entry in the vector_base
    @returns A reference to the first vector_base entry
*/
  inline double& front();

//-----------------------------------------------------------------------------
/** Returns a reference to the first entry in the vector_base
    @returns A reference to the first vector_base entry
*/
  inline const double& front() const;

//-----------------------------------------------------------------------------
/** Returns a reference to the last entry in the vector_base
    @returns A reference to the last vector_base entry
*/
  inline double& back();

//-----------------------------------------------------------------------------
/** Returns a reference to the last entry in the vector_base
    @returns A reference to the last vector_base entry
*/
  inline const double& back() const;

//*****************************************************************************
// iterators
//*****************************************************************************

//-----------------------------------------------------------------------------
/** Returns an iterator that points to the first entry in the vector_base.
    @return An iterator that points to the first entry in the vector_base.
*/
  inline iterator begin();

//-----------------------------------------------------------------------------
/** Returns a const_iterator that points to the first entry in
    vector_base. Similar to the non-const begin().  
    @see begin().
*/
  inline const_iterator begin() const;

//-----------------------------------------------------------------------------
/** Returns an iterator that points to one past the location of the last entry
    in the vector_base. That is, if another iterator were to point to the last
    entry in a vector_base and the ++ operator would be called, then this
    iterator would equal end().
    @return An iterator that points to one position past the last reachable
    entry in the vector_base.
*/
  inline iterator end();

//-----------------------------------------------------------------------------
/** Returns a const_iterator that points to one position past the last entry in
    the vector_base. Similar to the non-const end().
    @see end().
*/
  inline const_iterator end() const;

//=============================================================================
// comparisons
//=============================================================================
//-----------------------------------------------------------------------------
/** Equality operator
    @param other The vector_base being compared
    @return 'true' if the two vectrors are identical
*/
  inline bool operator == (const vector_base& other) const;

//-----------------------------------------------------------------------------
/** Inequality operator
    @param other The vector_base being compared
    @return 'true' if the two vectrors are not identical
*/
  inline bool operator != (const vector_base& other) const;

//=============================================================================
// arithmetic operations
//=============================================================================
//-----------------------------------------------------------------------------
/** Set the vector_base to be all zeros
    @return A reference to this vector.
*/
  inline vector_base& zeros();

//-----------------------------------------------------------------------------
/** Set the vector_base to be all ones
    @return A reference to this vector.
*/
  inline vector_base& ones();

//-----------------------------------------------------------------------------
/** Set the vector_base to be v[i]=i
    @return A reference to this vector.
*/
  inline vector_base& ascending();
  
//-----------------------------------------------------------------------------
/** Set the vector_base to be v[i]=width-i-1
    @return A reference to this vector.
*/
  inline vector_base& descending();
  
//-----------------------------------------------------------------------------
/** Sets all elements of a vector to equal a scalar.
    @param scalar The scalar 
    @return A reference to this vector.
*/
  inline vector_base& operator = (const double& scalar);
  
//-----------------------------------------------------------------------------
/** Adds a scalar to all of the vector_base entries.
    @param scalar The scalar to add to all of the vector_base
    @return A reference to this vector.
*/
  inline vector_base& operator += (const double& scalar);
  
//-----------------------------------------------------------------------------
/** Subtracts a scalar from all of the vector_base entries.
    @param scalar The scalar to subtract from all of the vector_base
    @return A reference to this vector.
*/
  inline vector_base& operator -= (const double& scalar);
  
//-----------------------------------------------------------------------------
/** Self-multipilcation by a scalar. Multiplies all of the vector_base entries
    by some scalar.
    @param scalar The scalar to multiply all of the vector_base by
    @return A reference to this vector.
*/
  inline vector_base& operator *= (const double& scalar);
  
//-----------------------------------------------------------------------------
/** Self-division by a scalar. Divides all of the vector_base entries
    by some scalar.
    @param scalar The scalar to divide all of the vector_base by
    @return A reference to this vector.
*/
  inline vector_base& operator /= (const double& scalar);
  
//-----------------------------------------------------------------------------
/** Adds another vector_base to this one.
    @param other The other vector_base to be added this vector_base
    @return A reference to this vector.
*/
  inline vector_base& operator += (const vector_base& other);
  
//-----------------------------------------------------------------------------
/** Subtracts another vector_base from this one.
    @param other The vector_base being subtracted from this vector_base
    @return A reference to this vector.
*/
  inline vector_base& operator -= (const vector_base& other);

//-----------------------------------------------------------------------------
/** Point-wise multiplication of another vector_base with this one.  
    @param other The other vector_base to be multiplied (point-wise) with this
    vector_base
    @return A reference to this vector.
*/
  inline vector_base& operator *= (const vector_base& other);
  
//-----------------------------------------------------------------------------
/** Point-wise division of another vector_base with this one.  
    @param other The other vector_base which divides (point-wise) this
    vector_base
    @return A reference to this vector.
*/
  inline vector_base& operator /= (const vector_base& other);

//-----------------------------------------------------------------------------
/** Changes every element v_i of the vector_base to be exp(v_i + scalar)
    The outcome is clipped at 1.0e+128
    @param scalar The additive shift to the exponent
    @return A reference to this vector.
*/
  inline vector_base& exp(const double scalar = 0.0);
  
//-----------------------------------------------------------------------------
/** Changes every element v_i of the vector_base to be log(v_i) / log(scalar)
    @param scalar The logarithm base
    @return A reference to this vector.
*/
  inline vector_base& log(const double scalar = 2.718281828459);
  
//-----------------------------------------------------------------------------
/** Coordiante wise power. Changes every element v_i of the vector_base to be
    v_i^scalar.
    @param scalar The power by which to raise every vector_base element.
    @return A reference to this vector.
*/
  inline vector_base& pow(const double& scalar);
  
//-----------------------------------------------------------------------------
/** Returns the maximum value in the vector_base
    @return The maximum value in the vector_base
*/
  inline double max() const;
  
//-----------------------------------------------------------------------------
/** Returns the coordinate of maximal value in the vector_base
    @return The coordinate of maximal value in the vector_base
*/
  inline unsigned long argmax() const;
  
//-----------------------------------------------------------------------------
/** Returns the minimum value in the vector_base
    @return The minimum value in the vector_base
*/
  inline double min() const;
  
//-----------------------------------------------------------------------------
/** Returns the coordinate of minimal value in the vector_base
    @return The coordinate of minimal value in the vector_base
*/
  inline unsigned long argmin() const;
  
//-----------------------------------------------------------------------------
/** Returns the sum of all vector_base elements
    @return The sum of all vector_base elements
*/
  inline double sum() const;
  
//-----------------------------------------------------------------------------
/** Returns the l1 norm of the vector_base.
    @return The l1 norm of the vector_base.
*/
  inline double norm1() const;
  
//-----------------------------------------------------------------------------
/** Returns the squared-l2 norm of the vector_base.
    @return The squared-l2 norm of the vector_base.
*/
  inline double norm2() const;

//=============================================================================
// views
//=============================================================================
//-----------------------------------------------------------------------------
/** Returns a vector_base which is a subvector of this vector
    @param from The first element of this vector which appears in the new
                vector
    @param s The number of elements to include in the new vector
    @param inc The number of elements to increment between elements 
               of the original vector (default = 1)
    @return A new vector_base which is a subset of the original vector.
*/
  inline vector_base subvector(unsigned long from, unsigned long s,
			       unsigned long inc=1);

//-----------------------------------------------------------------------------
/** Returns a vector_base which is a subvector of this vector
    @param from The first element of this vector which appears in the new
                vector
    @param s The number of elements to include in the new vector
    @param inc The number of elements to increment between elements 
               of the original vector (default = 1)
    @return A new vector_base which is a subset of the original vector.
*/
  inline const vector_base subvector(unsigned long from, unsigned long s,
				     unsigned long inc=1) 
    const;

protected:
//-----------------------------------------------------------------------------
/** A protected function used by infra::vector to create the underlying
    vector_base.
*/
  inline vector_base(unsigned long size, double* data, unsigned long step, 
               infra::refcount_darray memory, unsigned long allocated_size) : 
    _size(size), _data(data), _step(step), _memory(memory), 
    _allocated_size(allocated_size) {}

//=============================================================================
// vector_base Data Members
//=============================================================================
  unsigned long _size;
  double* _data;
  unsigned long _step;
  infra::refcount_darray _memory;
  unsigned long _allocated_size;
};

//*****************************************************************************
// const_iterator internal class
//*****************************************************************************
//-----------------------------------------------------------------------------
/** Implements a constant iterator to a vector_base
*/
class vector_base::const_iterator {
  friend class vector_base;
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
  inline bool operator < (const vector_base::const_iterator& other) const;

//-----------------------------------------------------------------------------
/** operator != - determines which of two const_iterators is lower in memory
    @param other A reference to the const_iterator to compare with
*/
  inline bool operator != (const vector_base::const_iterator& other) const;

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
/** operator++, prefix version
*/
  inline const_iterator operator++();

//-----------------------------------------------------------------------------
/** operator++, postfix version
*/
  inline const_iterator operator++(int);

//-----------------------------------------------------------------------------
/** operator++, prefix version
*/
  inline const_iterator operator+=(unsigned long offset);

//-----------------------------------------------------------------------------
/** operator--, prefix version
*/
  inline const_iterator operator--();

//-----------------------------------------------------------------------------
/** operator--, postfix version
*/
  inline const_iterator operator--(int);

 protected:
//-----------------------------------------------------------------------------
/** Constructs a const_iterator
    @param ptr A pointer to double 
    @param step The step size of the vector_base
*/
  inline const_iterator(double* ptr, unsigned long step);

//=============================================================================
// const_iterator Data Members
//=============================================================================
  double* _ptr; 
  unsigned long _step;
};

//*****************************************************************************
// iterator internal class
//*****************************************************************************
//-----------------------------------------------------------------------------
/** Implements an iterator to a vector_base. 
*/
class vector_base::iterator : public vector_base::const_iterator {
  friend class vector_base;
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
    @param step The step size of the vector_base
*/
  inline iterator(double* ptr, unsigned long step);
};

//*****************************************************************************
// class vector_view
//*****************************************************************************
//*****************************************************************************
/** Implements a vector_view, i.e. a vector which references another vector's 
    memory.
    @author Ofer Dekel (oferd@cs.huji.ac.il)
*/
typedef vector_base vector_view;

//*****************************************************************************
// class vector
//*****************************************************************************
//*****************************************************************************
/** Implements a basic algebric vector of floating point (double) numbers
    which has its own memory.
    @author Ofer Dekel (oferd@cs.huji.ac.il)
*/
class vector : public vector_base {
//=============================================================================
// vector Function Declarations
//=============================================================================
 public:
//-----------------------------------------------------------------------------
/** Constructs a vector of a given size. The default size is 0
    @param size The size of the vector
*/
  inline explicit vector(unsigned long size=0);

//-----------------------------------------------------------------------------
/** Copy constructor.
    @param other A reference to the vector being copied
*/
  inline vector(const vector& other);

//-----------------------------------------------------------------------------
/** Copy constructor.
    @param other A reference to the vector being copied
*/
  inline vector(const vector_base& other);

//-----------------------------------------------------------------------------
/** Standard Destructor. 
*/
  inline ~vector();

//-----------------------------------------------------------------------------
/** Swaps the memory and dimensions of this vector with the memory and
    dimensions of another vector.
    @param other The other vector
*/
  inline void swap(vector& other);

//-----------------------------------------------------------------------------
/** Resizes the vector by reallocating its memory. The original coordiantes
    that still fit in the new vector size remain intact. New coordinates added
    to the vector are not initialized. Note that if any vector_view pointing 
    to this vector is unaffected by this: the view keeps the old memory
    while this vector gets new memory.
    @param size The new size of the vector
    @return A reference to this vector.
*/
  inline vector_base& resize(unsigned long size);

//-----------------------------------------------------------------------------
/** Sets all elements of a vector to equal a scalar.
    @param scalar The scalar 
    @return A reference to this vector.
*/
  inline vector_base& operator = (const double& scalar);
  
//=============================================================================
// File interface
//=============================================================================
//-----------------------------------------------------------------------------
/** Load a vector from a binary file
    @param stream The stream that the vector is to be read from. 
*/
  inline vector(FILE* stream);

//-----------------------------------------------------------------------------
/** Load a vector from an input stream
    @param stream The stream that the vector is to be read from. 
*/
  inline vector(std::istream& stream);
};

//*****************************************************************************
// Declaration of stream operators
//*****************************************************************************
//----------------------------------------------------------------------------
/** Output streaming operator for vector_base.
    @return The output stream, to allow concatinated output.
    @param os The output stream to print to.
    @param v A reference to the vector_base being printed.
*/
inline std::ostream& operator<< (std::ostream& os,
				 const infra::vector_base& v);

//----------------------------------------------------------------------------
/** Input streaming operator for vector_base without resizing it. If the vector
    saved in the file is of a different size than this vector, an error occurs.
    @return The input stream, to allow concatinated input.
    @param is The input stream to read from.
    @param v A reference to the vector_base being read.
*/
inline std::istream& operator>> (std::istream& is, infra::vector_base& v);
};

#endif
//*****************************************************************************
//                                     E O F
//*****************************************************************************
