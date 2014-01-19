//=============================================================================
// File Name: infra_binary.h
// Written by: Ofer Dekel (29.02.04)
//=============================================================================
#ifndef _OFERD_BINARY_H_
#define _OFERD_BINARY_H_

//*****************************************************************************
// Included Files
//*****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include "infra_vector.h"
#include "infra_matrix.h"

#define SWAB16(A)  ((((unsigned short)(A) & 0xff00) >> 8) |	\
		    (((unsigned short)(A) & 0x00ff) << 8))
#define SWAB32(A)  ((((unsigned long)(A) & 0xff000000) >> 24) |	\
		    (((unsigned long)(A) & 0x00ff0000) >> 8)  |	\
		    (((unsigned long)(A) & 0x0000ff00) << 8)  |	\
		    (((unsigned long)(A) & 0x000000ff) << 24))
#define SWAB64(A)  (((unsigned long long)(SWAB32((int)(((A) << 32) >> 32))) << 32) | \
		    (unsigned int)SWAB32(((int)((A) >> 32))))

namespace infra {

// The binaryTraits structures are used by the binary file interface. They
// are used to tell the compiler what file header defines each type that
// is saved to file. The supported types and their header are:
//
//   infra::vector   v
//   infra::matrix   M
//   short           i
//   unsigned short  I
//   long            l
//   unsigned long   L
//   double          r
//

 template<typename T> struct binaryTraits {};
 
 template<> struct binaryTraits<infra::vector> {
   static const char header = 'v';
 };

 template<> struct binaryTraits<infra::matrix> {
   static const char header = 'M';
 };


 template<> struct binaryTraits<long> {
   static const char header = 'l';
   static const size_t block_size = sizeof(long);
 };
 
 template<> struct binaryTraits<unsigned short> {
   static const char header = 'I';
   static const size_t block_size = sizeof(unsigned short);
 };
 
 template<> struct binaryTraits<short> {
   static const char header = 'i';
   static const size_t block_size = sizeof(short);
 };
 
 template<> struct binaryTraits<unsigned long> {
   static const char header = 'L';
   static const size_t block_size = sizeof(unsigned long);
 };
 
 template<> struct binaryTraits<unsigned int> {
   static const char header = 'L';
   static const size_t block_size = sizeof(unsigned long);
 };
 
 template<> struct binaryTraits<double> {
   static const char header = 'r';
   static const size_t block_size = sizeof(double);
 };

 //----------------------------------------------------------------------------
 /** Saves an infra::vector_base to a file using the binary file format.
     @param stream The file stream to save to
     @param v A constant reference to the vector being saved
 */
 void save_binary(FILE* stream, const infra::vector_base& v);

 //----------------------------------------------------------------------------
 /** Saves an infra::vector to a file using the binary file format.
     @param stream The file stream to save to
     @param v A constant reference to the vector being saved
 */
 void save_binary(FILE* stream, const infra::vector& v);

//----------------------------------------------------------------------------
/** Saves an infra::matrix_base to a file using the binary file format.
    @param stream The file stream to save to
    @param M A constant reference to the matrix being saved
*/
 void save_binary(FILE* stream, const infra::matrix_base& M);

//----------------------------------------------------------------------------
/** Saves an infra::matrix to a file using the binary file format.
    @param stream The file stream to save to
    @param M A constant reference to the matrix being saved
*/
 void save_binary(FILE* stream, const infra::matrix& M);
 
//----------------------------------------------------------------------------
/** Saves a c-array of doubles in the infra::vector format
    @param stream The file stream to save to
    @param t A pointer to the beginning of the array
    @param size The size of the array
*/
 void save_binary(FILE* stream, const double* v, size_t size);
 
//----------------------------------------------------------------------------
/** Loads an infra::vector from file using the binary file format
    @param stream The file stream to load from
    @param v A reference to the matrix being loaded
*/
 void load_binary(FILE* stream, infra::vector_base& v);

//----------------------------------------------------------------------------
/** Loads an infra::matrix from file using the binary file format
    @param stream The file stream to load from
    @param M A reference to the matrix being loaded
*/
 void load_binary(FILE* stream, infra::matrix_base& M);

//*****************************************************************************
//  Implementation of template functions
//*****************************************************************************
//----------------------------------------------------------------------------
/** Saves basic types to file using the binary file format.
    @param stream The file stream to save to
    @param t A constant reference to the type being saved
*/
template<typename T>
void save_binary(FILE* stream, const T& t) {

#ifdef _ENDIAN_SWAP_

  char header = binaryTraits<T>::header;
  size_t block_size = binaryTraits<T>::block_size;
  
  // write the header
  fwrite(&header, sizeof(char), 1, stream);

  // write the block_size
  size_t _block_size = SWAB32(block_size);
  fwrite(&_block_size, sizeof(size_t), 1, stream);

  // write the data
  T t2;
  if (block_size == 2) {
    unsigned short a = SWAB16( *((unsigned short *)&t) );
    t2 = *((T*)&a);
  }
  else if (block_size == 4) {
    unsigned long a = SWAB32( *((unsigned long *)&t) );
    t2 = *((T*)&a);
  }
  else if (block_size == 8) {
    unsigned long long a = SWAB64( *((unsigned long long *)&t) );
    t2 = *((T*)&a);
  }
  fwrite(&t2, block_size, 1, stream);

#else

  char header = binaryTraits<T>::header;
  size_t block_size = binaryTraits<T>::block_size;
  
  // write the header
  fwrite(&header, sizeof(char), 1, stream);

  // write the block_size
  fwrite(&block_size, sizeof(size_t), 1, stream);

  // write the data
  fwrite(&t, block_size, 1, stream);

#endif // _ENDIAN_SWAP_

 }

//----------------------------------------------------------------------------
/** Loads a basic type from file using the binary file format
    @param stream The file stream to load from
    @param t A reference to the type being loaded
*/
template<typename T>
void load_binary(FILE* stream, T& t) {
 
  std::size_t read_size = 0;
  
#ifdef _ENDIAN_SWAP_

  char header;

  // read the header
  read_size = fread(&header, sizeof(char), 1, stream);
  if (header != binaryTraits<T>::header) {
    std::cerr << "Wrong file format when reading vector from file.\n";
    exit(1);
  }

  // read block size;
  size_t block_size;
  read_size = fread(&block_size, sizeof(size_t), 1, stream);
  block_size = SWAB32(block_size);

  // read the data
  read_size = fread(&t, block_size, 1, stream);
  if (block_size == 2) {
    unsigned short a = SWAB16( *((unsigned short *)&t) );
    t = *((T*)&a);
  }
  else if (block_size == 4) {
    unsigned long a = SWAB32( *((unsigned long *)&t) );
    t = *((T*)&a);
  }
  else if (block_size == 8) {
    unsigned long long a = SWAB64( *((unsigned long long *) &t) );
    t = *((T*)&a);
  }


#else

  char header;

  // read the header
  read_size = fread(&header, sizeof(char), 1, stream);
  if (header != binaryTraits<T>::header) {
    std::cerr << "Wrong file format when reading vector from file.\n";
    exit(1);
  }

  // read block size;
  size_t block_size;
  read_size = fread(&block_size, sizeof(size_t), 1, stream);

  // read the data
  read_size = fread(&t, block_size, 1, stream);

#endif // _ENDIAN_SWAP_

}



//----------------------------------------------------------------------------
/** Checks of the stream is currently pointing to an object of a specified
    type, without changing the position of the stream. Example:
    long l = 0;
    if(is_binary<long>(file)) load_binary(file, l);
    @param stream The file stream being queried
    @return true if the file point to an object of type T (the templated type)
*/
template<typename T>
bool is_binary(FILE* stream) {
  char header;
  long current_position = ftell(stream);
  
  // read the header
  fread(&header, sizeof(char), 1, stream);
  fseek(stream, current_position, SEEK_SET);
  if (header == binaryTraits<T>::header) return true;
  return false;
}
};
#endif
//*****************************************************************************
//                                     E O F
//*****************************************************************************




