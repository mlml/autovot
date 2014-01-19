//=============================================================================
// File Name: tester_endian.cpp
// Written by: Joseph Keshet
//
// Tester for checking writing of native endian
//=============================================================================


#include "infra.h"
#include <iostream>
#include <cstdio>
using namespace std;

void open_write(FILE **stream)
{
  *stream = fopen("/tmp/tmp","w");
  if (*stream == NULL) {
    cerr << "Error: unable to open file /tmp/tmp for writing\n" << endl;
    exit(EXIT_FAILURE);
  }
}

void open_read(FILE **stream)
{
  *stream = fopen("/tmp/tmp","r");
  if (*stream == NULL) {
    cerr << "Error: unable to open file /tmp/tmp for reading\n" << endl;
    exit(EXIT_FAILURE);
  }
}


int main()
{
  // binary files
  FILE* fid;
  fid = fopen("/tmp/tmp_file.infra","wb");
  infra::vector v2(100); 
  v2.ascending();
  infra::vector v3(150); 
  v3.descending();
  v2.save_binary(fid);
  v3.save_binary(fid);
  fclose(fid);

  FILE *fin;
  fin = fopen("/tmp/tmp_file.infra","rb");
  infra::vector v4(fin);
  infra::vector v5(fin);
  fclose(fin);
  
  std::cout << "char=" << sizeof(char) << std::endl;
  std::cout << "double=" << sizeof(double) << std::endl;
  std::cout << "ulong=" << sizeof(unsigned long) << std::endl;
  std::cout << "ull=" << sizeof(unsigned long long) << std::endl;
  std::cout << "uint=" << sizeof(unsigned int) << std::endl;
  std::cout << "size_t=" << sizeof(size_t) << std::endl;

  exit(1);

  FILE *stream;
  unsigned long ul = 89722323;
  unsigned long ulr = 0;
  open_write(&stream);
  infra::save_binary(stream,ul);
  fclose(stream);
  open_read(&stream);
  infra::load_binary(stream,ulr);
  fclose(stream);
  printf("unsigned long write/read = %d\n", ul==ulr);

  long l = -218972;
  long lr = 0;
  open_write(&stream);
  infra::save_binary(stream,l);
  fclose(stream);
  open_read(&stream);
  infra::load_binary(stream,lr);
  fclose(stream);
  printf("long write/read = %d\n", l==lr);

  short s = -8972;
  short sr = 0;
  open_write(&stream);
  infra::save_binary(stream,s);
  fclose(stream);
  open_read(&stream);
  infra::load_binary(stream,sr);
  fclose(stream);
  printf("short write/read = %d\n", s==sr);

  unsigned short us = 8972;
  unsigned short usr = 0;
  open_write(&stream);
  infra::save_binary(stream,us);
  fclose(stream);
  open_read(&stream);
  infra::load_binary(stream,usr);
  fclose(stream);
  printf("unsigned short write/read = %d\n", us==usr);

  unsigned int is = 8972;
  unsigned int isr = 0;
  open_write(&stream);
  infra::save_binary(stream,is);
  fclose(stream);
  open_read(&stream);
  infra::load_binary(stream,isr);
  fclose(stream);
  printf("unsigned int write/read = %d\n", is==isr);

  double d = 101.8972;
  double dr = 0;
  open_write(&stream);
  infra::save_binary(stream,d);
  fclose(stream);
  open_read(&stream);
  infra::load_binary(stream,dr);
  fclose(stream);
  printf("double write/read = %d  (%lf=%lf) \n", d==dr,d,dr);

  infra::vector v(3);
  v(0) = 2.3; v(1) = 52334.1; v(2) = 1.34e12;
  open_write(&stream);
  infra::save_binary(stream,v);
  fclose(stream);
  v.zeros();
  open_read(&stream);
  v.load_binary(stream);
  fclose(stream);
  cout << v << endl;

  infra::matrix m(3,2);
  m(0,1) = 2.3; m(2,0) = 52334.1; m(1,1) = 1.34e12;
  open_write(&stream);
  infra::save_binary(stream,m);
  fclose(stream);
  m.zeros();
  open_read(&stream);
  m.load_binary(stream);
  fclose(stream);
  cout << m << endl;

  return EXIT_SUCCESS;
};
