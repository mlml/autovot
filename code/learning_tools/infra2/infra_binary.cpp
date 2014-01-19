//=============================================================================
// File Name: infra_binary.cpp
// Written by: Ofer Dekel (29.02.04)
//=============================================================================
//*****************************************************************************
// Included Files
//*****************************************************************************
#include "infra_binary.h"
#include "infra_vector.imp"
#include "infra_matrix.imp"

//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
void infra::save_binary(FILE* stream, const infra::vector& v) {
  v.save_binary(stream);
}

//----------------------------------------------------------------------------
void infra::save_binary(FILE* stream, const infra::vector_base& v) {
  v.save_binary(stream);
}

//----------------------------------------------------------------------------
void infra::save_binary(FILE* stream, const infra::matrix& M) {
  M.save_binary(stream);
}

//----------------------------------------------------------------------------
void infra::save_binary(FILE* stream, const infra::matrix_base& M) {
  M.save_binary(stream);
}

//----------------------------------------------------------------------------
void infra::load_binary(FILE* stream, infra::vector_base& v) {
  v.load_binary(stream);
}

//----------------------------------------------------------------------------
void infra::load_binary(FILE* stream, infra::matrix_base& M) {
   M.load_binary(stream);
}

//----------------------------------------------------------------------------

void infra::save_binary(FILE* stream, const double* v, size_t size) {
   
#ifdef _ENDIAN_SWAP_
   
   // write the header
   char header = 'v';
   fwrite(&header, sizeof(char), 1, stream);
   
   // write the block_size
   size_t block_size = sizeof(unsigned long) + sizeof(double) * size;
   size_t _block_size = SWAB32(block_size);
   fwrite(&_block_size, sizeof(size_t), 1, stream);
   
   // write the vector length
   unsigned long s = SWAB32(size);
   fwrite(&s, sizeof(unsigned long), 1, stream);
   
   // write the data
   double *v2 = new double[size];
   for (unsigned int i=0; i < size; i++) {
     unsigned long long a = SWAB64( *((unsigned long long *)&(v[i])) );
     v2[i] = *((double*)&a);
   }
   fwrite(v2, sizeof(double), size, stream);
   delete [] v2;

#else
   
   // write the header
   char header = 'v';
   fwrite(&header, sizeof(char), 1, stream);
   
   // write the block_size
   size_t block_size = sizeof(unsigned long) + sizeof(double) * size;
   fwrite(&block_size, sizeof(size_t), 1, stream);
   
   // write the vector length
   fwrite(&size, sizeof(unsigned long), 1, stream);
   
   // write the data
   fwrite(v, sizeof(double), size, stream);
   
#endif // _ENDIAN_SWAP_
 }  

//*****************************************************************************
//                                     E O F
//*****************************************************************************
