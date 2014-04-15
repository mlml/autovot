//=============================================================================
// File Name: infra_matrix.imp
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
#ifndef _OFERD_MATRIX_IMP_
#define _OFERD_MATRIX_IMP_

//*****************************************************************************
// Included Files
//*****************************************************************************
#include "infra_matrix.h"
#include "infra_exception.h"
#include "infra_refcount_darray.imp"
#include "infra_vector.imp"
#include <stdlib.h> // for exit()
#include <math.h> // for pow()

/*---------------------------------------------------------------------------*/
infra::matrix_base::matrix_base(const infra::matrix_base& other) :
  _height(other._height), _width(other._width),
  _data(other._data), _memory(other._memory), 
  _allocated_height(other._allocated_height), 
  _allocated_width(other._allocated_width) {}

/*---------------------------------------------------------------------------*/
infra::matrix_base::~matrix_base() {}

/*---------------------------------------------------------------------------*/
void infra::matrix_base::swap(infra::matrix_base& other) {

  unsigned long i = _height;
  _height = other._height;
  other._height = i;

  i = _width;
  _width = other._width;
  other._width = i;

  i = _allocated_height;
  _allocated_height = other._allocated_height;
  other._allocated_height = i;

  i = _allocated_width;
  _allocated_width = other._allocated_width;
  other._allocated_width = i;

  double* r = _data;
  _data = other._data;
  other._data = r;

  _memory.swap(other._memory);
}

/*---------------------------------------------------------------------------*/
#ifdef _ENDIAN_SWAP_

infra::matrix_base& infra::matrix_base::load_binary(FILE* stream) {

  // read header from file
  char header;
   std::size_t read_size = 0;

  read_size = fread(&header, sizeof(char), 1, stream);
  infra_assert(header == 'M',
               "Wrong file format when reading vector from file. The header " 
               << "in the file is \"" << header << "\" and should be \"M\"");

  // read total block size
  size_t block_size;
  read_size = fread(&block_size, sizeof(size_t), 1, stream);
  block_size = SWAB32(block_size);
  
  // read vector dimensions
  unsigned long file_height;
  unsigned long file_width;
  read_size = fread(&file_height, sizeof(unsigned long), 1, stream);
  file_height = SWAB32(file_height);
  read_size = fread(&file_width, sizeof(unsigned long), 1, stream);
  file_width = SWAB32(file_width);
  
  infra_assert(file_height == height() && file_width == width(), 
               "When calling load_binary for matrix_views, size of matrix in "
               << "file is " << file_height << "x" << file_width << " whereas "
               << "size of this matrix_view is " << height() << "x" 
               << width());

  double *a = new double[file_height * file_width];
  read_size = fread(a, sizeof(double), file_height * file_width, stream);
  iterator this_iter = begin();
  iterator this_end = end();
  const double* tmp_pointer = a;
  while(this_iter < this_end) {
    unsigned long long ulla;
    ulla = SWAB64( *((unsigned long long *)tmp_pointer) );
    *this_iter = *((double*)&ulla);
    ++this_iter;
    ++tmp_pointer;
  }
  delete [] a;
  return (*this);
}

#else

infra::matrix_base& infra::matrix_base::load_binary(FILE* stream) {

  // read header from file
  char header;
   std::size_t read_size = 0;

  read_size = fread(&header, sizeof(char), 1, stream);
  infra_assert(header == 'M',
               "Wrong file format when reading vector from file. The header " 
               << "in the file is \"" << header << "\" and should be \"M\"");

  // read total block size
  size_t block_size;
  read_size = fread(&block_size, sizeof(size_t), 1, stream);
  
  // read vector dimensions
  unsigned long file_height;
  unsigned long file_width;
  read_size = fread(&file_height, sizeof(unsigned long), 1, stream);
  read_size = fread(&file_width, sizeof(unsigned long), 1, stream);
  
  infra_assert(file_height == height() && file_width == width(), 
               "When calling load_binary for matrix_views, size of matrix in "
               << "file is " << file_height << "x" << file_width << " whereas "
               << "size of this matrix_view is " << height() << "x" 
               << width());

  double *a = new double[file_height * file_width];
  read_size = fread(a, sizeof(double), file_height * file_width, stream);
  iterator this_iter = begin();
  iterator this_end = end();
  const double* tmp_pointer = a;
  while(this_iter < this_end) {
    *this_iter = *tmp_pointer;
    ++this_iter;
    ++tmp_pointer;
  }
  delete [] a;
  return (*this);
}

#endif // _ENDIAN_SWAP_

/*---------------------------------------------------------------------------*/

#ifdef _ENDIAN_SWAP_

void infra::matrix_base::save_binary(FILE* stream) const {

  // write header
  char header = 'M';
  fwrite(&header, sizeof(char), 1, stream);

  // write the total block size
  size_t block_size = sizeof(unsigned long) * 2 +
                      sizeof(double) * _height * _width;
  size_t _block_size = SWAB32(block_size);
  fwrite(&_block_size, sizeof(size_t), 1, stream);

  // write the matrix_base dimensions
  size_t __height = SWAB32(_height);
  size_t __width = SWAB32(_width);
  fwrite(&__height, sizeof(unsigned long), 1, stream);
  fwrite(&__width, sizeof(unsigned long), 1, stream);

  double *a = new double[height() * width()];
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
  fwrite(a, sizeof(double), height() * width(), stream);
  delete [] a;
}

#else

void infra::matrix_base::save_binary(FILE* stream) const {

  // write header
  char header = 'M';
  fwrite(&header, sizeof(char), 1, stream);

  // write the total block size
  size_t block_size = sizeof(unsigned long) * 2 +
                      sizeof(double) * _height * _width;
  fwrite(&block_size, sizeof(size_t), 1, stream);

  // write the matrix_base dimensions
  fwrite(&_height, sizeof(unsigned long), 1, stream);
  fwrite(&_width, sizeof(unsigned long), 1, stream);

  // write the data
  if(height() * width() == _allocated_height * _allocated_width) {
    fwrite(_data, sizeof(double), height() * width(), stream);
  } else {
    double *a = new double[height() * width()];
    const_iterator this_iter = begin();
    const_iterator this_end = end();
    double* tmp_pointer = a;
    while(this_iter < this_end) {
      *tmp_pointer = *this_iter;
      ++this_iter;
      ++tmp_pointer;
    }
    fwrite(a, sizeof(double), height() * width(), stream);
    delete [] a;
  }
}

#endif // _ENDIAN_SWAP_

/*---------------------------------------------------------------------------*/
infra::matrix_base& infra::matrix_base::operator = (const infra::matrix_base& 
                                                    other) {

  infra_assert( height() == other.height() && width() == other.width(),
		"When calling operator =, both left and right hand matrices "
		<< "must have equal dimensions. In this case, "
		<< "the left-hand matrix is " << height() << "x" << width()
		<< " and the right-hand matrix is " << other.height() << "x"
		<< other.width());	      
  
  iterator this_iter = begin();
  iterator this_end = end();
  const_iterator other_iter = other.begin();
  while(this_iter < this_end) {
    *this_iter = *other_iter;
    ++this_iter;
    ++other_iter;
  }

  return (*this);
}

/*---------------------------------------------------------------------------*/
unsigned long infra::matrix_base::height() const {
  return _height;
}

/*---------------------------------------------------------------------------*/
unsigned long infra::matrix_base::allocated_height() const {
  return _allocated_height;
}

/*---------------------------------------------------------------------------*/
unsigned long infra::matrix_base::width() const {
  return _width;
}

/*---------------------------------------------------------------------------*/
double& infra::matrix_base::operator() (unsigned long i, unsigned long j) {

  infra_assert(i<height() && j<width(), "The coordiantes given are ("
	       << i << "," << j << ") and they exceed the matrix dimensions "
	       << "which are " << height() << "x" << width());

  return _data[j * _allocated_height + i];
}

/*---------------------------------------------------------------------------*/
const double& infra::matrix_base::operator() (unsigned long i,
					      unsigned long j) const {

  infra_assert(i<height() && j<width(), "The coordiantes given are ("
	       << i << "," << j << ") and they exceed the matrix dimensions "
	       << "which are " << height() << "x" << width());

  return _data[j * _allocated_height + i];
}

/*---------------------------------------------------------------------------*/
infra::matrix_base::iterator infra::matrix_base::begin() {
  return iterator(_data, 0, 0, _height, _allocated_height);
}

/*---------------------------------------------------------------------------*/
infra::matrix_base::iterator infra::matrix_base::end() {
  if(_height == 0) return iterator(_data, 0, 0, _height, _allocated_height);
  return iterator(_data+(_allocated_height*(_width-1)+_height), _height,
		  _width-1, _height, _allocated_height);
}

/*---------------------------------------------------------------------------*/
infra::matrix_base::const_iterator infra::matrix_base::begin() const{
  return const_iterator(_data, 0, 0, _height, _allocated_height);
}

/*---------------------------------------------------------------------------*/
infra::matrix_base::const_iterator infra::matrix_base::end() const {
  if(_height == 0) return iterator(_data, 0, 0, _height, _allocated_height);
  return iterator(_data+(_allocated_height*(_width-1)+_height), _height,
		  _width-1, _height, _allocated_height);
}

/*---------------------------------------------------------------------------*/
infra::matrix_base infra::matrix_base::submatrix(unsigned long t, 
                                          unsigned long l, 
                                          unsigned long h, 
                                          unsigned long w) {

  infra_assert( t + h <= height() && l + w <= width(),
		"When calling submatrix(), the sub matrix must be completely "
                << "contained in the original matrix. In this case, "
		<< "the dimension of the original matrix are " << height() 
                << "x" << width() << " whereas the bottom-right corner of the "
                << "requested submatrix is (" << t + h << ","
		<< l + w << ").");
  
  return matrix_base(h, 
                     w, 
                     _data + t + l * _allocated_height,
                     _memory,
                     _allocated_height,
                     _allocated_width);
} 

/*---------------------------------------------------------------------------*/
const infra::matrix_base infra::matrix_base::submatrix(unsigned long t, 
                                                       unsigned long l, 
                                                       unsigned long h, 
                                                       unsigned long w) const {

  infra_assert( t + h <= height() && l + w <= width(),
		"When calling submatrix(), the sub matrix must be completely "
                << "contained in the original matrix. In this case, "
		<< "the dimension of the original matrix are " << height() 
                << "x" << width() << " whereas the bottom-right corner of the "
                << "requested submatrix is (" << t + h << ","
		<< l + w << ").");
  
  return matrix_base(h, 
                     w, 
                     _data + t + l * _allocated_height,
                     _memory,
                     _allocated_height,
                     _allocated_width);
} 

/*---------------------------------------------------------------------------*/
inline infra::vector_base infra::matrix_base::row(unsigned long i) {

  infra_assert( i < height() ,
		"When calling row(), requested row " << i << " where the " 
                << " matrix height is " << height() );

  return infra::vector_base(_width,
                            _data + i,
                            _allocated_height,
                            _memory,
                            _allocated_height * _allocated_width);
}

/*---------------------------------------------------------------------------*/
inline const infra::vector_base infra::matrix_base::row(unsigned long i) 
  const {

  infra_assert( i < height() ,
		"When calling row(), requested row " << i << " where the " 
                << " matrix height is " << height() );

  return infra::vector_base(_width,
                            _data + i,
                            _allocated_height,
                            _memory,
                            _allocated_height * _allocated_width);
}

/*---------------------------------------------------------------------------*/
inline infra::vector_base infra::matrix_base::column(unsigned long j) {

  infra_assert( j < width(), 
		"When calling column(), requested column " << j << " where "
                << "the matrix width is " << width() );

  return infra::vector_base(_height,
                            _data + j * _allocated_height,
                            1,
                            _memory,
                            _allocated_height * _allocated_width);
}

/*---------------------------------------------------------------------------*/
inline const infra::vector_base infra::matrix_base::column(unsigned long j) 
  const {

  infra_assert( j < width(), 
		"When calling column(), requested column " << j << " where "
                << "the matrix width is " << width() );

  return infra::vector_base(_height,
                            _data + j * _allocated_height,
                            1,
                            _memory,
                            _allocated_height * _allocated_width);
}

/*---------------------------------------------------------------------------*/
inline infra::vector_base infra::matrix_base::diagonal(long k) {
  
  infra_assert( -k < (long)height() && k < (long)width(),
		"When calling diagonal(), requested diagonal " << k 
                << " where the matrix dimensions are " << height() << "x" 
                << width());

  unsigned long min_dim = _height;
  if ((long)_width - k < (long)_height) min_dim = _width - k;
  
  if (k > 0) {
    return infra::vector_base( min_dim,
                              _data + _allocated_height * k,
                              _allocated_height + 1,
                              _memory,
                              _allocated_height * _allocated_width);
  } else {
    return infra::vector_base(min_dim + k,
                              _data - k,
                              _allocated_height + 1,
                              _memory,
                              _allocated_height * _allocated_width);
  }
}

/*---------------------------------------------------------------------------*/
inline const infra::vector_base infra::matrix_base::diagonal(long k) const {
  
  infra_assert( -k < (long)height() && k < (long)width(),
		"When calling diagonal(), requested diagonal " << k 
                << " where the matrix dimensions are " << height() << "x" 
                << width());

  unsigned long min_dim = _height;
  if ((long)_width - k < (long)_height) min_dim = _width - k;
  
  if (k > 0) {
    return infra::vector_base( min_dim,
                              _data + _allocated_height * k,
                              _allocated_height + 1,
                              _memory,
                              _allocated_height * _allocated_width);
  } else {
    return infra::vector_base(min_dim + k,
                              _data - k,
                              _allocated_height + 1,
                              _memory,
                              _allocated_height * _allocated_width);
  }
}

/*---------------------------------------------------------------------------*/
infra::matrix_base& infra::matrix_base::zeros() {
  operator=(0.0);
  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::matrix_base& infra::matrix_base::ones() {
  operator=(1.0);
  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::matrix_base& infra::matrix_base::eye() {
  zeros();
  diagonal() = 1;
  return (*this);
}

/*---------------------------------------------------------------------------*/
bool infra::matrix_base::operator == (const infra::matrix_base& other) const {

  infra_assert( height() == other.height() && width() == other.width(),
		"When calling operator ==, both left and right hand matrices "
		<< "must have equal dimensions. In this case, "
		<< "the left-hand matrix is " << height() << "x" << width()
		<< " and the right-hand matrix is " << other.height() << "x"
		<< other.width());	      
  
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
bool infra::matrix_base::operator != (const infra::matrix_base& other) const {

  infra_assert( height() == other.height() && width() == other.width(),
		"When calling operator ==, both left and right hand matrices "
		<< "must have equal dimensions. In this case, "
		<< "the left-hand matrix is " << height() << "x" << width()
		<< " and the right-hand matrix is " << other.height() << "x"
		<< other.width());	      
  
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
infra::matrix_base& infra::matrix_base::operator *= (const double& scalar) {

  iterator this_iter = begin();
  iterator this_end = end();
  
  while(this_iter < this_end) {
    *this_iter *= scalar;
    ++this_iter;
  }
  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::matrix_base& infra::matrix_base::operator /= (const double& scalar) {

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
infra::matrix_base& infra::matrix_base::operator = (const double& scalar) {

  iterator this_iter = begin();
  iterator this_end = end();
  
  while(this_iter < this_end) {
    *this_iter = scalar;
    ++this_iter;
  }
  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::matrix_base& infra::matrix_base::operator += (const double& scalar) {

  iterator this_iter = begin();
  iterator this_end = end();
  
  while(this_iter < this_end) {
    *this_iter += scalar;
    ++this_iter;
  }
  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::matrix_base& infra::matrix_base::operator -= (const double& scalar) {

  iterator this_iter = begin();
  iterator this_end = end();
  
  while(this_iter < this_end) {
    *this_iter -= scalar;
    ++this_iter;
  }
  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::matrix_base& infra::matrix_base::operator += (const infra::matrix_base& 
                                                     other) {

  infra_assert( height() == other.height() && width() == other.width(),
		"When calling operator +=, both left and right hand matrices "
		<< "must have equal dimensions. In this case, "
		<< "the left-hand matrix is " << height() << "x" << width()
		<< " and the right-hand matrix is " << other.height() << "x"
		<< other.width());	      
  
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
infra::matrix_base& infra::matrix_base::operator -= (const infra::matrix_base& 
                                                     other) {

  infra_assert( height() == other.height() && width() == other.width(),
		"When calling operator -=, both left and right hand matrices "
		<< "must have equal dimensions. In this case, "
		<< "the left-hand matrix is " << height() << "x" << width()
		<< " and the right-hand matrix is " << other.height() << "x"
		<< other.width());	      
  
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
infra::matrix_base& infra::matrix_base::operator *= (const infra::matrix_base& 
                                                     other) {

  infra_assert( height() == other.height() && width() == other.width(),
		"When calling operator *=, both left and right hand matrices "
		<< "must have equal dimensions. In this case, "
		<< "the left-hand matrix is " << height() << "x" << width()
		<< " and the right-hand matrix is " << other.height() << "x"
		<< other.width());	      
  
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
infra::matrix_base& infra::matrix_base::operator /= (const infra::matrix_base& 
                                                     other) {

  infra_assert( height() == other.height() && width() == other.width(),
		"When calling operator /=, both left and right hand matrices "
		<< "must have equal dimensions. In this case, "
		<< "the left-hand matrix is " << height() << "x" << width()
		<< " and the right-hand matrix is " << other.height() << "x"
		<< other.width());	      
  
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
infra::matrix_base& infra::matrix_base::exp(const double& scalar) {

  const iterator this_end = end();
  iterator this_iter = begin();
  
  while(this_iter < this_end) {

    if(*this_iter + scalar > 128) {
      *this_iter = 1.0e+128;
    }

    *this_iter = ::exp(*this_iter + scalar);
    ++this_iter;
  }
  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::matrix_base& infra::matrix_base::log(const double scalar) {

  const iterator this_end = end();
  iterator this_iter = begin();
  
  double log_base = ::log(scalar); 
  
  while(this_iter < this_end) {
    *this_iter = ::log(*this_iter) / log_base; 
    ++this_iter;
  }
  return (*this);
}


/*---------------------------------------------------------------------------*/
infra::matrix_base& infra::matrix_base::pow(const double& scalar) {

  const iterator this_end = end();
  iterator this_iter = begin();
  
  while(this_iter < this_end) {
    *this_iter = ::pow(*this_iter, scalar);
    ++this_iter;
  }
  return (*this);
}

/*---------------------------------------------------------------------------*/
double infra::matrix_base::max() const {

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
double infra::matrix_base::min() const {

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
infra::matrix_base& infra::matrix_base::swap_rows(unsigned long i, 
                                                  unsigned long j) {

  infra_assert(i<_height && j<_height, "The row indices given are "
	       << i << " and " << j << " and they must be less than the "
	       << "matrix height which is " << height());

  for (unsigned long k=0; k<_width; ++k) {
    double tmp = operator()(i,k);
    operator()(i,k) = operator()(j,k);
    operator()(j,k) = tmp;
  }
  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::matrix_base& infra::matrix_base::swap_columns(unsigned long i, 
                                                     unsigned long j) {

  infra_assert(i<_width && j<_width, "The column indices given are "
	       << i << " and " << j << " and they must be less than the "
	       << "matrix width which is " << width());

  for (unsigned long k=0; k<_height; ++k) {
    double tmp = operator()(k,i);
    operator()(k,i) = operator()(k,j);
    operator()(k,j) = tmp;
  }
  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::matrix_base::const_iterator::const_iterator(double* ptr, 
                                                   unsigned long row,
                                                   unsigned long column,
                                                   unsigned long matrix_height,
                                              unsigned long allocated_height) :
  _ptr(ptr), _row(row), _column(column), _matrix_height(matrix_height),
  _allocated_height(allocated_height) {}

/*---------------------------------------------------------------------------*/
infra::matrix_base::const_iterator::const_iterator 
  (const const_iterator& other) : _ptr(other._ptr), 
                                  _row(other._row), _column(other._column), 
                                  _matrix_height(other._matrix_height),
                                  _allocated_height(other._allocated_height) {}

/*---------------------------------------------------------------------------*/
bool infra::matrix_base::const_iterator::operator < 
  (const matrix_base::const_iterator& other) const {
  return _ptr < other._ptr;
} 

/*---------------------------------------------------------------------------*/
bool infra::matrix_base::const_iterator::operator !=
  (const matrix_base::const_iterator& other) const {
  return _ptr != other._ptr;
} 

/*---------------------------------------------------------------------------*/
bool infra::matrix_base::const_iterator::operator ==
  (const matrix_base::const_iterator& other) const {
  return _ptr == other._ptr;
} 

/*---------------------------------------------------------------------------*/
const double& infra::matrix_base::const_iterator::operator* () {
  return *_ptr;
} 

/*---------------------------------------------------------------------------*/
double& infra::matrix_base::iterator::operator* () {
  return *_ptr;
} 

/*---------------------------------------------------------------------------*/
const double* infra::matrix_base::const_iterator::operator-> () {
  return _ptr;
} 

/*---------------------------------------------------------------------------*/
const double* infra::matrix_base::const_iterator::ptr() {
  return _ptr;
} 

/*---------------------------------------------------------------------------*/
double* infra::matrix_base::iterator::operator-> () {
  return _ptr;
} 

/*---------------------------------------------------------------------------*/
double* infra::matrix_base::iterator::ptr() {
  return _ptr;
} 

/*---------------------------------------------------------------------------*/
unsigned long infra::matrix_base::const_iterator::row() {
  return _row;
}

/*---------------------------------------------------------------------------*/
unsigned long infra::matrix_base::const_iterator::column() {
  return _column;
}

/*---------------------------------------------------------------------------*/
infra::matrix_base::const_iterator 
infra::matrix_base::const_iterator::operator++() {

  ++_ptr;
  ++_row;
  
  // deal with column change 
  if(_row >= _matrix_height) {
    _ptr += _allocated_height - _row;
    ++_column;
    _row = 0;
  }

  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::matrix_base::const_iterator 
infra::matrix_base::const_iterator::operator++(int) {

  const_iterator new_iter(*this);

  ++_ptr;
  ++_row;

  // deal with column change 
  if(_row >= _matrix_height) {
    
    _ptr += _allocated_height - _row;
    ++_column;
    _row = 0;
  }

  return new_iter;
}

/*---------------------------------------------------------------------------*/
infra::matrix_base::const_iterator 
infra::matrix_base::const_iterator::operator+=(unsigned long offset) {
  
  _row += offset;
  _ptr += (_row/_matrix_height) * _allocated_height + (_row%_matrix_height) 
          - _row;
  _row = _row%_matrix_height;
  _column += (_row/_matrix_height);
  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::matrix_base::const_iterator 
infra::matrix_base::const_iterator::operator--() {

  --_ptr;
  --_row;

  // deal with column change 
#if 0  
  if(_row < 0) {
    
    _ptr += _row - _allocated_height + _matrix_height - 1;
    --_column;
    _row = _matrix_height - 1;
  }
#endif

  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::matrix_base::const_iterator 
infra::matrix_base::const_iterator::operator--(int) {

  const_iterator new_iter(*this);

  --_ptr;
  --_row;

  // deal with column change 
#if 0
  if(_row < 0) {
   
    _ptr += _row - _allocated_height + _matrix_height - 1;
    --_column;
    _row = _matrix_height - 1;
  }
#endif

  return new_iter;
}

/*---------------------------------------------------------------------------*/
infra::matrix_base::const_iterator 
infra::matrix_base::const_iterator::right() {
  _ptr += _allocated_height;
  ++_column;
  return *this;
}

/*---------------------------------------------------------------------------*/
infra::matrix_base::const_iterator 
infra::matrix_base::const_iterator::left() {
  _ptr -= _allocated_height;
  --_column;
  return *this;
}

/*---------------------------------------------------------------------------*/
infra::matrix_base::const_iterator 
infra::matrix_base::const_iterator::up() {
  --_ptr;
  --_row;
  return *this;
}

/*---------------------------------------------------------------------------*/
infra::matrix_base::const_iterator 
infra::matrix_base::const_iterator::down() {
  ++_ptr;
  ++_row;
  return *this;
}

/*---------------------------------------------------------------------------*/
infra::matrix_base::iterator::iterator(double* ptr, 
                                       unsigned long row,
                                       unsigned long column,
                                       unsigned long matrix_height,
                                       unsigned long allocated_height) :
  const_iterator(ptr, row, column, matrix_height, allocated_height) {}

/*---------------------------------------------------------------------------*/
infra::matrix_base::iterator::iterator 
  (const iterator& other) : const_iterator(other._ptr, other._row, 
                                           other._column, 
                                           other._matrix_height, 
                                           other._allocated_height) {}


/*---------------------------------------------------------------------------*/
infra::matrix::matrix(unsigned long height, unsigned long width) :
  matrix_base(height, width, 0, refcount_darray(height*width), height, width) {
  _data = _memory.ptr();
}

/*---------------------------------------------------------------------------*/
infra::matrix::matrix(const infra::matrix& other) :
  matrix_base(other.height(), other.width(), 0, 
              refcount_darray(other.height() * other.width()),
              other.height(), other.width()) {
  _data = _memory.ptr();
  (matrix_base)(*this) = other;
}

/*---------------------------------------------------------------------------*/
infra::matrix::matrix(const infra::matrix_base& other) :
  matrix_base(other.height(), other.width(), 0, 
              refcount_darray(other.height() * other.width()),
              other.height(), other.width()) {
  _data = _memory.ptr();
  (matrix_base)(*this) = other;
}

/*---------------------------------------------------------------------------*/
infra::matrix_base& infra::matrix::resize(unsigned long height, 
                                          unsigned long width) {
  
  // get minimal dimensions (that must be copied from old to new)
  unsigned long min_height = _height;
  if(height < _height) min_height = height;
  unsigned long min_width = _width;
  if(width < _width) min_width = width;

  // allocate new memory 
  refcount_darray new_array(height * width);
  double* new_ptr = new_array.ptr(); 
  
  for(uint j=0; j<min_width; ++j) { 
    for(uint i=0; i<min_height; ++i) { 
      *(new_ptr + i + j * height) = operator()(i,j);
    }
  }
  // reassign members
  _height = height;
  _width = width;
  _allocated_height = height;
  _allocated_width = width;
  _memory.swap(new_array);
  _data = _memory.ptr();
  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::matrix_base& infra::matrix::reshape(unsigned long height, 
                                           unsigned long width) {

  infra_assert(_height*_width == height*width, "The product of the new matrix "
	       << "dimensions (" << height << "," << width << ") does not "
	       << "equal the ammount of allocated memory (" <<
	       _height*_width << ")");

  _height = height;
  _width = width;
  _allocated_height = height;
  _allocated_width = width;
  return (*this);
}

/*---------------------------------------------------------------------------*/
infra::matrix::~matrix() {}

/*---------------------------------------------------------------------------*/
infra::matrix_base& infra::matrix::operator = (const double& scalar) {

  iterator this_iter = begin();
  iterator this_end = end();
  
  while(this_iter < this_end) {
    *this_iter = scalar;
    ++this_iter;
  }
  return (*this);
}

/*---------------------------------------------------------------------------*/

#ifdef _ENDIAN_SWAP_

infra::matrix::matrix(FILE* stream) : 
  matrix_base(0,0,0,infra::refcount_darray(0),0,0) {

  // read header from file
  char header;
  std::size_t read_size = 0;

  read_size = fread(&header, sizeof(char), 1, stream);
  infra_assert(header == 'M',
               "Wrong file format when reading matrix from file. The header " 
               << "in the file is \"" << header << "\" and should be \"M\"");
  
  // read total block size
  size_t block_size;
  read_size = fread(&block_size, sizeof(size_t), 1, stream);
  block_size = SWAB32(block_size);
  
  // read matrix dimensions
  unsigned long file_height, file_width;
  read_size = fread(&file_height, sizeof(unsigned long), 1, stream);
  file_height = SWAB32(file_height);
  read_size = fread(&file_width, sizeof(unsigned long), 1, stream);
  file_width = SWAB32(file_width);
  
  infra::refcount_darray new_array(file_height * file_width);
  read_size = fread(new_array.ptr(), sizeof(double), file_height * file_width, stream);
  unsigned long long ulla;
  double *a = new_array.ptr();   
  for (uint i=0; i < file_height*file_width; i++) {
    ulla = SWAB64( *((unsigned long long *)(a+i)) );
    a[i] = *((double*)&ulla);
  }
  
  _memory.swap(new_array);
  _data = _memory.ptr();
  _height = file_height;
  _width = file_width;
  _allocated_height = file_height;
  _allocated_width = file_width;
}

#else

infra::matrix::matrix(FILE* stream) : 
  matrix_base(0,0,0,infra::refcount_darray(0),0,0) {

  // read header from file
  char header;
  std::size_t read_size = 0;

  read_size = fread(&header, sizeof(char), 1, stream);
  infra_assert(header == 'M',
               "Wrong file format when reading matrix from file. The header " 
               << "in the file is \"" << header << "\" and should be \"M\"");
  
  // read total block size
  size_t block_size;
  read_size = fread(&block_size, sizeof(size_t), 1, stream);
  
  // read matrix dimensions
  unsigned long file_height, file_width;
  read_size = fread(&file_height, sizeof(unsigned long), 1, stream);
  read_size = fread(&file_width, sizeof(unsigned long), 1, stream);
  
  infra::refcount_darray new_array(file_height * file_width);
  read_size = fread(new_array.ptr(), sizeof(double), file_height * file_width, stream);
  
  _memory.swap(new_array);
  _data = _memory.ptr();
  _height = file_height;
  _width = file_width;
  _allocated_height = file_height;
  _allocated_width = file_width;
}

#endif // _ENDIAN_SWAP_

/*---------------------------------------------------------------------------*/
infra::matrix::matrix(std::istream& stream) : 
  matrix_base(0,0,0,infra::refcount_darray(0),0,0) {

  // read matrix dimensions
  unsigned long h,w;
  stream >> h;
  stream >> w;
  
  infra::refcount_darray new_array(h*w);
  _memory.swap(new_array);
  _data = _memory.ptr();
  _allocated_height = h;
  _allocated_width = w;
  _height = h;
  _width = w;

  for(unsigned int i=0; i<h; ++i) {
    for(unsigned int j=0; j<w; ++j) {
      stream >> (*this)(i,j);
    }
  }
}

/*---------------------------------------------------------------------------*/
std::ostream& infra::operator<<(std::ostream& os, const infra::matrix_base& m){

  os << m.height() << " " << m.width() << "\n";

  for(unsigned long i=0; i<m.height(); ++i) {
    for(unsigned long j=0; j<m.width(); ++j) {
      os << m(i,j) << " ";
    }
    os << std::endl;
  }
  
  return os;
}

/*---------------------------------------------------------------------------*/
std::istream& infra::operator>> (std::istream& is, infra::matrix_base& m) {

  // read height and width parameters from the stream
  unsigned long height, width;
  is >> height;
  is >> width;

  infra_assert(height == m.height() && width == m.width(), 
               "When calling operator>> for matrices, size of matrix in stream "
               << "is " << height << "x" << width << " whereas "
               << "size of this matrix is " << m.height() << "x" 
               << m.width());

  for(unsigned long i=0; i<m.height(); ++i) {
    for(unsigned long j=0; j<m.width(); ++j) {
      is >> m(i,j);
    }
  }

  return is;
}

#endif
//*****************************************************************************
//                                     E O F
//*****************************************************************************
