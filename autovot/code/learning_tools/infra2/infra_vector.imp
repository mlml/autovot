//=============================================================================
// File Name: infra_vector.imp
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
#ifndef _OFERD_VECTOR_IMP_
#define _OFERD_VECTOR_IMP_

//*****************************************************************************
// Included Files
//*****************************************************************************
#include "infra_vector.h"
#include "infra_exception.h"
#include "infra_refcount_darray.imp"
#include <math.h>

#define SWAB32(A)  ((((unsigned long)(A) & 0xff000000) >> 24) |	\
		    (((unsigned long)(A) & 0x00ff0000) >> 8)  |	\
		    (((unsigned long)(A) & 0x0000ff00) << 8)  |	\
		    (((unsigned long)(A) & 0x000000ff) << 24))
#define SWAB64(A)  (((unsigned long long)(SWAB32((int)(((A) << 32) >> 32))) << 32) | \
		    (unsigned int)SWAB32(((int)((A) >> 32))))

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
  void cblas_dcopy(const int N, const double *X, const int incX,
                   double *Y, const int incY);
}

#endif //_USE_ATLAS_  
//.............................................................................
/*---------------------------------------------------------------------------*/
infra::vector_base::vector_base(const infra::vector_base& other) :
  _size(other.size()),
  _data(other._data),
  _step(other._step),
  _memory(other._memory),
  _allocated_size(other._allocated_size) {
}

/*---------------------------------------------------------------------------*/
infra::vector_base::~vector_base() {}

/*---------------------------------------------------------------------------*/

#ifdef _ENDIAN_SWAP_

infra::vector_base& infra::vector_base::load_binary(FILE* stream) {

  // read header from file
  char header;
  std::size_t read_size = 0;

  read_size = fread(&header, sizeof(char), 1, stream);
  infra_assert(header == 'v',
               "Wrong file format when reading vector from file. The header " 
               << "in the file is \"" << header << "\" and should be \"v\"");

  // read total block size
  size_t block_size;
  read_size = fread(&block_size, sizeof(size_t), 1, stream);
  block_size = SWAB32(block_size);
  
  // read vector dimensions
  unsigned long file_allocated_size;
  read_size = fread(&file_allocated_size, sizeof(unsigned long), 1, stream);
  file_allocated_size = SWAB32(file_allocated_size);
  
  infra_assert(file_allocated_size == size(), 
               "When calling load_binary for vector_views, size of vector in "
               << "file is " << file_allocated_size << " whereas size of "
               << "this vector_view is " << size());
  double *a = new double[file_allocated_size];
  unsigned long long ulla;
  read_size = fread(a, sizeof(double), file_allocated_size, stream);
  iterator this_iter = begin();
  iterator this_end = end();
  const double* tmp_pointer = a;
  while(this_iter < this_end) {
    ulla = SWAB64( *((unsigned long long *)tmp_pointer) );
    *this_iter = *((double*)&ulla);
    ++this_iter;
    ++tmp_pointer;
  }
  delete [] a;
  return (*this);
}

#else

infra::vector_base& infra::vector_base::load_binary(FILE* stream) {

  // read header from file
  char header;
  std::size_t read_size = 0;

  read_size = fread(&header, sizeof(char), 1, stream);
  infra_assert(header == 'v',
               "Wrong file format when reading vector from file. The header " 
               << "in the file is \"" << header << "\" and should be \"v\"");

  // read total block size
  size_t block_size;
  read_size = fread(&block_size, sizeof(size_t), 1, stream);
  
  // read vector dimensions
  unsigned long file_allocated_size;
  read_size = fread(&file_allocated_size, sizeof(unsigned long), 1, stream);
  
  infra_assert(file_allocated_size == size(), 
               "When calling load_binary for vector_views, size of vector in "
               << "file is " << file_allocated_size << " whereas size of "
               << "this vector_view is " << size());

  // if the step size is 1 then simply read the array from file into memory
  // otherwise, read it into a temporary location and copy the data into the
  // relevant elements of this vector 
  if(_step == 1) {
    // read the entire data from the file all at once.
    read_size = fread(_data, sizeof(double), _size, stream);
  } else {
    double *a = new double[file_allocated_size];
    read_size = fread(a, sizeof(double), file_allocated_size, stream);
    iterator this_iter = begin();
    iterator this_end = end();
    const double* tmp_pointer = a;
    while(this_iter < this_end) {
      *this_iter = *tmp_pointer;
      ++this_iter;
      ++tmp_pointer;
    }
    delete [] a;
  }
  return (*this);
}

#endif // _ENDIAN_SWAP_


/*---------------------------------------------------------------------------*/
#ifdef _ENDIAN_SWAP_

void infra::vector_base::save_binary(FILE* stream) const {

  // write header
  char header = 'v';
  fwrite(&header, sizeof(char), 1, stream);

  // write the total block size
  size_t block_size = sizeof(unsigned long) + sizeof(double) * size();
  size_t _block_size = SWAB32(block_size);
  fwrite(&_block_size, sizeof(size_t), 1, stream);

  // write the vector_base dimension
  size_t __size = SWAB32(_size);
  fwrite(&__size, sizeof(unsigned long), 1, stream);

  double *a = new double[_size];
  unsigned long long ulla;
  const_iterator this_iter = begin();
  const_iterator this_end = end();
  double* tmp_pointer = a;
  while(this_iter < this_end) {
    ulla = SWAB64(*((unsigned long long *)&(*this_iter)));
    *tmp_pointer = *((double*)&ulla);
    ++this_iter;
    ++tmp_pointer;
  }
  // write the data
  fwrite(a, sizeof(double), _size, stream);
  delete [] a;

}

#else

void infra::vector_base::save_binary(FILE* stream) const {

  // write header
  char header = 'v';
  fwrite(&header, sizeof(char), 1, stream);

  // write the total block size
  size_t block_size = sizeof(unsigned long) + sizeof(double) * size();
  fwrite(&block_size, sizeof(size_t), 1, stream);

  // write the vector_base dimension
  fwrite(&_size, sizeof(unsigned long), 1, stream);

  // if the step size is 1 then simply write the array from memory to file.
  // otherwise, copy it into a temporary location and write that data to file.
  if(_step == 1) {
    // read the entire data from the file all at once.
    fwrite(_data, sizeof(double), _size, stream);
  } else {
    double *a = new double[_size];
    const_iterator this_iter = begin();
    const_iterator this_end = end();
    double* tmp_pointer = a;
    while(this_iter < this_end) {
      *tmp_pointer = *this_iter;
      ++this_iter;
      ++tmp_pointer;
    }
    // write the data
    fwrite(a, sizeof(double), _size, stream);
    delete [] a;
  }
}

#endif // _ENDIAN_SWAP_

/*---------------------------------------------------------------------------*/
infra::vector_base& infra::vector_base::operator = (const infra::vector_base&
						    other) {
  infra_assert(size() == other.size(), "When calling operator =, both left "
	       << "and right hand vectors must have equal size. In this case, "
	       << "the left-hand size was " << size() << " and the right-hand "
	       << "size was " << other.size());

//.............................................................................
#ifdef _USE_ATLAS_
  cblas_dcopy(size(), other.begin().ptr(), other.step(), begin().ptr(), 
              step());

#else //_USE_ATLAS_  
//.............................................................................

  // set all of the elements of the vector_base to equal 'value'
  iterator this_iter = begin();
  iterator this_end = end();
  const_iterator other_iter = other.begin();
  while(this_iter < this_end) {
    *this_iter = *other_iter;
    ++this_iter;
    ++other_iter;
  }

#endif //_USE_ATLAS_

  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::vector_base& infra::vector_base::copy_neg(const infra::vector_base&
						 other) {

  infra_assert(size() == other.size(), "When calling copy_neg(), both left "
	       << "and right hand vectors must have equal size. In this case, "
	       << "the left-hand size was " << size() << " and the right-hand "
	       << "size was " << other.size());

  // set all of the elements of the vector_base to equal 'value'
  iterator this_iter = begin();
  iterator this_end = end();
  const_iterator other_iter = other.begin();
  while(this_iter < this_end) {
    *this_iter = - *other_iter;
    ++this_iter;
    ++other_iter;
  }
  return (*this);
}

/*---------------------------------------------------------------------------*/
unsigned long infra::vector_base::size() const {
  return _size;
}

/*---------------------------------------------------------------------------*/
unsigned long infra::vector_base::step() const {
  return _step;
}

/*---------------------------------------------------------------------------*/
const double& infra::vector_base::operator[] (unsigned long i) const {
  infra_assert( i < size(), "The vector coordinate being referenced exceeds "
		<< "the vector size. The index given is " << i
		<< " while the vector size is " << size() );
  
  return *(_data + (i * _step));
}

/*---------------------------------------------------------------------------*/
double& infra::vector_base::operator[] (unsigned long i) {
  infra_assert( i < size(), "The vector coordinate being referenced exceeds "
		<< "the vector size. The index given is " << i
		<< " while the vector size is " << size() );
  
  return *(_data + (i * _step));
}

/*---------------------------------------------------------------------------*/
const double& infra::vector_base::operator() (unsigned long i) const {
  infra_assert( i < size(), "The vector coordinate being referenced exceeds "
		<< "the vector size. The index given is " << i
		<< " while the vector size is " << size() );
  
  return *(_data + (i * _step));
}

/*---------------------------------------------------------------------------*/
double& infra::vector_base::operator() (unsigned long i) {
  infra_assert( i < size(), "The vector coordinate being referenced exceeds "
		<< "the vector size. The index given is " << i
		<< " while the vector size is " << size() );
  
  return *(_data + (i * _step));
}

/*---------------------------------------------------------------------------*/
const double& infra::vector_base::front() const {
  return *_data;
}

/*---------------------------------------------------------------------------*/
double& infra::vector_base::front() {
  return *_data;
}

/*---------------------------------------------------------------------------*/
const double& infra::vector_base::back() const {
  return *(_data + (_size - 1) * _step);
}

/*---------------------------------------------------------------------------*/
double& infra::vector_base::back() {
  return *(_data + (_size - 1) * _step);
}

/*---------------------------------------------------------------------------*/
infra::vector_base::iterator infra::vector_base::begin() {
  return iterator(_data,_step);
}

/*---------------------------------------------------------------------------*/
infra::vector_base::const_iterator infra::vector_base::begin() const {
  return const_iterator(_data, _step);
}

/*---------------------------------------------------------------------------*/
infra::vector_base::iterator infra::vector_base::end() {
  return iterator(_data + (_size * _step), _step);
}

/*---------------------------------------------------------------------------*/
infra::vector_base::const_iterator infra::vector_base::end() const {
  return const_iterator(_data + (_size * _step), _step);
}

/*---------------------------------------------------------------------------*/
bool infra::vector_base::operator == (const infra::vector_base& other) const {

  infra_assert(size() == other.size(), "When calling operator ==, both left "
	       << "and right hand vectors must have equal size. In this case, "
	       << "the left-hand size was " << size() << " and the right-hand "
	       << "size was " << other.size());

  const_iterator this_iter = begin();
  const_iterator other_iter = other.begin();
  const_iterator this_end = end();
  
  while(this_iter < this_end) {
    if(*this_iter != *other_iter) return false;
    ++this_iter;
    ++other_iter;
  }
  return true;
}

/*---------------------------------------------------------------------------*/
bool infra::vector_base::operator != (const infra::vector_base& other) const {

  infra_assert(size() == other.size(), "When calling operator !=, both left "
	       << "and right hand vectors must have equal size. In this case, "
	       << "the left-hand size was " << size() << " and the right-hand "
	       << "size was " << other.size());

  const_iterator this_iter = begin();
  const_iterator other_iter = other.begin();
  const_iterator this_end = end();
  
  while(this_iter < this_end) {
    if(*this_iter != *other_iter) return true;
    ++this_iter;
    ++other_iter;
  }
  return false;
}

/*---------------------------------------------------------------------------*/
infra::vector_base& infra::vector_base::zeros() {

  iterator this_iter = begin();
  iterator this_end = end();
  
  while(this_iter < this_end) {
    *this_iter = 0.0;
    ++this_iter;
  }
  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::vector_base& infra::vector_base::ones() {

  iterator this_iter = begin();
  iterator this_end = end();
  
  while(this_iter < this_end) {
    *this_iter = 1.0;
    ++this_iter;
  }
  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::vector_base& infra::vector_base::ascending() {

  iterator this_iter = begin();
  iterator this_end = end();
  double val = 0.0;
  
  while(this_iter < this_end) {
    *this_iter = val;
    ++this_iter;
    val += 1.0;
  }
  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::vector_base& infra::vector_base::descending() {

  iterator this_iter = begin();
  iterator this_end = end();
  double val = double(size() - 1);
  
  while(this_iter < this_end) {
    *this_iter = val;
    ++this_iter;
    val -= 1.0;
  }
  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::vector_base& infra::vector_base::operator += (const double& scalar) {

  iterator this_iter = begin();
  iterator this_end = end();
  
  while(this_iter < this_end) {
    *this_iter += scalar;
    ++this_iter;
  }
  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::vector_base& infra::vector_base::operator = (const double& scalar) {

  iterator this_iter = begin();
  iterator this_end = end();
  
  while(this_iter < this_end) {
    *this_iter = scalar;
    ++this_iter;
  }
  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::vector_base& infra::vector_base::operator -= (const double& scalar) {

  iterator this_iter = begin();
  iterator this_end = end();
  
  while(this_iter < this_end) {
    *this_iter -= scalar;
    ++this_iter;
  }
  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::vector_base& infra::vector_base::operator *= (const double& scalar) {

  iterator this_iter = begin();
  iterator this_end = end();
  
  while(this_iter < this_end) {
    *this_iter *= scalar;
    ++this_iter;
  }
  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::vector_base& infra::vector_base::operator /= (const double& scalar) {

  infra_assert( scalar != 0.0, "Divide by zero error");

  iterator this_iter = begin();
  iterator this_end = end();
  
  while(this_iter < this_end) {
    *this_iter /= scalar;
    ++this_iter;
  }
  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::vector_base& infra::vector_base::operator += (const infra::vector_base& 
                                                     other) {

  infra_assert(size() == other.size(), "When calling operator +=, both left "
	       << "and right hand vectors must have equal size. In this case, "
	       << "the left-hand size was " << size() << " and the right-hand "
	       << "size was " << other.size());
	      
  iterator this_iter = begin();
  const_iterator other_iter = other.begin();
  iterator this_end = end();
  
  while(this_iter < this_end) {
    *this_iter += *other_iter;

    ++this_iter;
    ++other_iter;
  }
  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::vector_base& infra::vector_base::operator -= (const infra::vector_base& 
                                                     other) {

  infra_assert(size() == other.size(), "When calling operator -=, both left "
	       << "and right hand vectors must have equal size. In this case, "
	       << "the left-hand size was " << size() << " and the right-hand "
	       << "size was " << other.size());
	      
  iterator this_iter = begin();
  const_iterator other_iter = other.begin();
  iterator this_end = end();
  
  while(this_iter < this_end) {
    *this_iter -= *other_iter;

    ++this_iter;
    ++other_iter;
  }
  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::vector_base& infra::vector_base::operator *= (const infra::vector_base& 
                                                     other) {

  infra_assert(size() == other.size(), "When calling operator *=, both left "
	       << "and right hand vectors must have equal size. In this case, "
	       << "the left-hand size was " << size() << " and the right-hand "
	       << "size was " << other.size());
	      
  iterator this_iter = begin();
  const_iterator other_iter = other.begin();
  iterator this_end = end();
  
  while(this_iter < this_end) {
    *this_iter *= *other_iter;

    ++this_iter;
    ++other_iter;
  }
  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::vector_base& infra::vector_base::operator /= (const infra::vector_base& 
                                                     other) {

  infra_assert(size() == other.size(), "When calling operator /=, both left "
	       << "and right hand vectors must have equal size. In this case, "
	       << "the left-hand size was " << size() << " and the right-hand "
	       << "size was " << other.size());
	      
  iterator this_iter = begin();
  const_iterator other_iter = other.begin();
  iterator this_end = end();
  
  while(this_iter < this_end) {
    
    infra_assert( *other_iter != 0.0, "Divide by zero error");
    *this_iter /= *other_iter;

    ++this_iter;
    ++other_iter;
  }
  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::vector_base& infra::vector_base::exp(const double scalar) {

  const iterator this_end = end();
  iterator this_iter = begin();
  
  while(this_iter < this_end) {

    if(*this_iter + scalar > 128) {
      *this_iter = 1.0e+128;
    }

    else {
      *this_iter = ::exp(*this_iter + scalar);
    }
    ++this_iter;
  }
  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::vector_base& infra::vector_base::log(const double scalar) {

  const iterator this_end = end();
  iterator this_iter = begin();
  double log_base  = ::log(scalar);
  
  while(this_iter < this_end) {
    *this_iter = ::log(*this_iter) / log_base;
    ++this_iter;
  }
  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::vector_base& infra::vector_base::pow(const double& scalar) {

  const iterator this_end = end();
  iterator this_iter = begin();
  
  while(this_iter < this_end) {
    *this_iter = ::pow(*this_iter, scalar);
    ++this_iter;
  }

  return (*this);
}

/*---------------------------------------------------------------------------*/
double infra::vector_base::max() const {

  const_iterator this_iter = begin();
  const_iterator this_end = end();

  // initialize the minimal value
  double max = *this_iter;
  ++this_iter;

  // find minimal value
  while(this_iter < this_end) {
    if (*this_iter > max) {
      max = *this_iter;
    }
    ++this_iter;
  }

  return max;
}

/*---------------------------------------------------------------------------*/
unsigned long infra::vector_base::argmax() const {

  const_iterator this_iter = begin();
  const_iterator this_end = end();

  // initialize the minimal value
  double max = *this_iter;
  unsigned long argmax = 0;
  ++this_iter;
  unsigned long current_index = 1;
  
  // find maximal value
  while(this_iter < this_end) {
    if (*this_iter > max) {
      max = *this_iter;
      argmax = current_index;
    }
    ++this_iter;
    ++current_index;
  }

  return argmax;
}

/*---------------------------------------------------------------------------*/
double infra::vector_base::min() const {

  const_iterator this_iter = begin();
  const_iterator this_end = end();

  // initialize the minimal value
  double min = *this_iter;
  ++this_iter;

  // find minimal value
  while(this_iter < this_end) {
    if (*this_iter < min) {
      min = *this_iter;
    }
    ++this_iter;
  }

  return min;
}

/*---------------------------------------------------------------------------*/
unsigned long infra::vector_base::argmin() const {

  const_iterator this_iter = begin();
  const_iterator this_end = end();

  // initialize the minimal value
  double min = *this_iter;
  unsigned long argmin = 0;
  ++this_iter;
  unsigned long current_index = 1;
  
  // find minimal value
  while(this_iter < this_end) {
    if (*this_iter < min) {
      min = *this_iter;
      argmin = current_index;
    }
    ++this_iter;
    ++current_index;
  }

  return argmin;
}

/*---------------------------------------------------------------------------*/
double infra::vector_base::sum() const {

  const_iterator this_iter = begin();
  const_iterator this_end = end();

  // initialize the minimal value
  double sum = 0.0;
  
  // find minimal value
  while(this_iter < this_end) {
    sum += *this_iter;
    ++this_iter;
  }
  
  return sum;
}

/*---------------------------------------------------------------------------*/
double infra::vector_base::norm1() const {

  const_iterator this_iter = begin();
  const_iterator this_end = end();

  // initialize the minimal value
  double norm = 0.0;
  
  // find minimal value
  while(this_iter < this_end) {
    if (*this_iter > 0.0) {
      norm += *this_iter;
    } else {
      norm -= *this_iter;
    }
    ++this_iter;
  }
  
  return norm;
}

/*---------------------------------------------------------------------------*/
double infra::vector_base::norm2() const {

  const_iterator this_iter = begin();
  const_iterator this_end = end();

  // initialize the minimal value
  double norm = 0.0;
  
  // find minimal value
  while(this_iter < this_end) {
    norm += (*this_iter) * (*this_iter);
    ++this_iter;
  }
  
  return norm;
}

/*---------------------------------------------------------------------------*/
infra::vector_base infra::vector_base::subvector(unsigned long from, 
                                                 unsigned long s,
						 unsigned long inc) {
  infra_assert(from < size(), 
	       "When calling subvector(), the subvector should "
	       << "begin before the end of the original vector. You requested "
               << "a subvector which begins at " << from << " while the size "
               << "of the original vector is only " << size());

  infra_assert(from + (s-1)*inc < size() , 
	       "When calling subvector(), the end of "
               << "the subvector should not exceed the end of the original "
	       << "vector. You requested a subvector which begins at " 
               << from << " has increments of " << inc 
	       << " and therefore ends at " << from + (s-1)*inc 
	       << " while the size "
               << "of the original vector is only " << size());


  return vector_base(s, 
                     _data+(from*_step), 
                     _step*inc, 
                     _memory, 
                     _allocated_size);
}

/*---------------------------------------------------------------------------*/
const infra::vector_base infra::vector_base::subvector(unsigned long from, 
						       unsigned long s,
						       unsigned long inc) const {

  infra_assert(from < size(), 
	       "When calling subvector(), the subvector should "
	       << "begin before the end of the original vector. You requested "
               << "a subvector which begins at " << from << " while the size "
               << "of the original vector is only " << size());

  infra_assert(from + (s-1)*inc <= size(), 
	       "When calling subvector(), the end of "
               << "the subvector should not exceed the end of the original "
	       << "vector. You requested a subvector which begins at " 
               << from << " has increments of " << inc 
	       << " and therefore ends at " << from + (s-1)*inc 
	       << " while the size "
               << "of the original vector is only " << size());


  return vector_base(s, 
                     _data+(from*_step), 
                     _step*inc, 
                     _memory, 
                     _allocated_size);
}

/*---------------------------------------------------------------------------*/
infra::vector_base::const_iterator::const_iterator(double* ptr, 
                                                   unsigned long step=1) : 
  _ptr(ptr), _step(step) {}

/*---------------------------------------------------------------------------*/
infra::vector_base::const_iterator::const_iterator 
  (const const_iterator& other) : _ptr(other._ptr), _step(other._step) {}

/*---------------------------------------------------------------------------*/
bool infra::vector_base::const_iterator::operator < 
  (const vector_base::const_iterator& other) const {
  return _ptr < other._ptr;
} 

//-----------------------------------------------------------------------------
inline bool infra::vector_base::const_iterator::operator != 
  (const vector_base::const_iterator& other) const {
  return _ptr != other._ptr;
}

/*---------------------------------------------------------------------------*/
const double& infra::vector_base::const_iterator::operator* () {
  return *_ptr;
} 

/*---------------------------------------------------------------------------*/
double& infra::vector_base::iterator::operator* () {
  return *_ptr;
} 

/*---------------------------------------------------------------------------*/
const double* infra::vector_base::const_iterator::operator-> () {
  return _ptr;
} 

/*---------------------------------------------------------------------------*/
const double* infra::vector_base::const_iterator::ptr() {
  return _ptr;
} 

/*---------------------------------------------------------------------------*/
double* infra::vector_base::iterator::operator-> () {
  return _ptr;
} 

/*---------------------------------------------------------------------------*/
double* infra::vector_base::iterator::ptr() {
  return _ptr;
} 

/*---------------------------------------------------------------------------*/
infra::vector_base::const_iterator 
infra::vector_base::const_iterator::operator++() {
  _ptr += _step; 
  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::vector_base::const_iterator 
infra::vector_base::const_iterator::operator++(int) {
  const_iterator new_iter(*this);
  _ptr += _step; 
  return new_iter;
}

/*---------------------------------------------------------------------------*/
infra::vector_base::const_iterator 
infra::vector_base::const_iterator::operator+=(unsigned long offset) {
  _ptr += _step * offset; 
  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::vector_base::const_iterator 
infra::vector_base::const_iterator::operator--() {
  _ptr -= _step; 
  return *this;
}

/*---------------------------------------------------------------------------*/
infra::vector_base::const_iterator 
infra::vector_base::const_iterator::operator--(int) {
  const_iterator new_iter(*this);
  _ptr -= _step; 
  return new_iter;
}

/*---------------------------------------------------------------------------*/
infra::vector_base::iterator::iterator(double* ptr, unsigned long step=1) : 
  const_iterator(ptr, step) {}

/*---------------------------------------------------------------------------*/
infra::vector_base::iterator::iterator 
  (const iterator& other) : const_iterator(other._ptr, other._step) {}

/*---------------------------------------------------------------------------*/
infra::vector::vector(unsigned long size) :
  vector_base(size, 0, 1, refcount_darray(size), size) {
  _data = _memory.ptr();
}

/*---------------------------------------------------------------------------*/
infra::vector::vector(const infra::vector& other) :
  vector_base(other.size(), 0, 1, refcount_darray(other.size()),other.size()) {
  _data = _memory.ptr();
  (vector_base)(*this) = other;
}

/*---------------------------------------------------------------------------*/
infra::vector::vector(const infra::vector_view& other) :
  vector_base(other.size(), 0, 1, refcount_darray(other.size()),other.size()) {
  _data = _memory.ptr();
  (vector_base)(*this) = other;
}

/*---------------------------------------------------------------------------*/

#ifdef _ENDIAN_SWAP_

infra::vector::vector(FILE* stream) : 
  vector_base(0,0,0,infra::refcount_darray(0),0) {

  // read header from file
  char header;
  std::size_t read_size = 0;

  read_size = fread(&header, sizeof(char), 1, stream);
  infra_assert(header == 'v',
               "Wrong file format when reading vector from file. The header " 
               << "in the file is \"" << header << "\" and should be \"v\"");
  
  // read total block size
  size_t block_size;
  read_size = fread(&block_size, sizeof(size_t), 1, stream);
  block_size = SWAB32(block_size);
  
  // read vector dimensions
  unsigned long file_allocated_size;
  read_size = fread(&file_allocated_size, sizeof(unsigned long), 1, stream);
  file_allocated_size = SWAB32(file_allocated_size);
  
  infra::refcount_darray new_array(file_allocated_size);
  read_size = fread(new_array.ptr(), sizeof(double), file_allocated_size, stream);
  double *a = new_array.ptr();
  unsigned long long ulla;
  for (uint i=0; i < file_allocated_size; i++) {      
      ulla = SWAB64( *((unsigned long long *) a));
      *a = *((double*)&ulla);
      a++;
  }

  _memory.swap(new_array);
  _data = _memory.ptr();
  _allocated_size = file_allocated_size;
  _size = file_allocated_size;
  _step = 1;
}

#else

infra::vector::vector(FILE* stream) : 
  vector_base(0,0,0,infra::refcount_darray(0),0) {

  // read header from file
  char header;
  std::size_t read_size = 0;
  
  read_size = fread(&header, sizeof(char), 1, stream);
  infra_assert(header == 'v',
               "Wrong file format when reading vector from file. The header " 
               << "in the file is \"" << header << "\" and should be \"v\"");
  
  // read total block size
  size_t block_size;
  read_size = fread(&block_size, sizeof(size_t), 1, stream);
  
  // read vector dimensions
  unsigned long file_allocated_size;
  read_size = fread(&file_allocated_size, sizeof(unsigned long), 1, stream);
  
  infra::refcount_darray new_array(file_allocated_size);
  read_size = fread(new_array.ptr(), sizeof(double), file_allocated_size, stream);
  
  _memory.swap(new_array);
  _data = _memory.ptr();
  _allocated_size = file_allocated_size;
  _size = file_allocated_size;
  _step = 1;
}

#endif // _ENDIAN_SWAP_

/*---------------------------------------------------------------------------*/
infra::vector::vector(std::istream& stream) : 
  vector_base(0,0,0,infra::refcount_darray(0),0) {

  // read vector dimensions
  unsigned long size;
  stream >> size;
  
  infra::refcount_darray new_array(size);
  _memory.swap(new_array);
  _data = _memory.ptr();
  _allocated_size = size;
  _size = size;
  _step = 1;

  iterator this_iter = begin();
  iterator this_end = end();
  while(this_iter < this_end) {
    stream >> (*this_iter);
    ++this_iter;
  }
}

/*---------------------------------------------------------------------------*/
void infra::vector::swap(infra::vector& other) {

  unsigned long size = _size;
  _size = other._size;
  other._size = size;

  unsigned long allocated_size = _allocated_size;
  _allocated_size = other._allocated_size;
  other._allocated_size = allocated_size;

  unsigned long step = _step;
  _step = other._step;
  other._step = step;

  double* data = _data;
  _data = other._data;
  other._data = data;

  _memory.swap(other._memory);
}

/*---------------------------------------------------------------------------*/
infra::vector_base& infra::vector::resize(unsigned long size) {

  // get the number of elements that must be copied to the new vector
  unsigned long min_size = _allocated_size;
  if (size < min_size) min_size = size;

  // allocate new memory
  infra::refcount_darray new_array(size);

  // copy min_size elements
//.............................................................................
#ifdef _USE_ATLAS_
  cblas_dcopy(min_size, _memory.ptr(), 1, new_array.ptr(), 1);
  
#else //_USE_ATLAS_  
//.............................................................................

  const double* this_ptr = _data;
  const double* this_end = _data + _step * min_size;
  double* new_ptr = new_array.ptr();
  while(this_ptr < this_end) {
    *new_ptr = *this_ptr;
    this_ptr += _step;
    ++new_ptr;
  }
#endif //_USE_ATLAS_
  
  _memory.swap(new_array);
  _data = _memory.ptr();
  _allocated_size = size;
  _size = size;
  _step = 1;
  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::vector::~vector() {}

/*---------------------------------------------------------------------------*/
infra::vector_base& infra::vector::operator = (const double& scalar) {

  iterator this_iter = begin();
  iterator this_end = end();
  
  while(this_iter < this_end) {
    *this_iter = scalar;
    ++this_iter;
  }
  return (*this);
}

/*---------------------------------------------------------------------------*/
std::ostream& infra::operator<< (std::ostream& os,
				 const infra::vector_base& v) {

  os << v.size() << " ";
  
  infra::vector_base::const_iterator iter = v.begin();
  infra::vector_base::const_iterator end = v.end();

  while(iter < end) {
    os << *iter << " ";
    ++iter;
  }
  return os;
}

/*---------------------------------------------------------------------------*/
std::istream& infra::operator>> (std::istream& is, infra::vector_base& v) {

  // read size from the stream
  unsigned long size;
  is >> size;
  infra_assert(v.size() == size, 
               "When calling operator>> for vector_views, size of vector in "
               << "file is " << size << " whereas size of "
               << "this vector_view is " << v.size());
  
  infra::vector_base::iterator iter = v.begin();
  infra::vector_base::iterator end = v.end();
  
  while(iter < end) {
    is >> *iter;
    ++iter;
  }
  return is;
}

#endif
//*****************************************************************************
//                                   E O F
//*****************************************************************************
