//=============================================================================
// File Name: tester_ver2.cpp
// Written by: Shai Shalev-Shwartz (14.09.04)
//
// Tester for dense vector and matrix classes in infra version 2
//=============================================================================


#include "infra.h"
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <cstdio>
#include <fstream>
using namespace std;

#define SMALL_NUMBER 1e-20

#ifdef NDEBUG
//#define HERE_BLAS_BUG
#endif

#ifdef NDEBUG
#define check_exception(expression,counter) \
{counter++;}
#else 
#define check_exception(expression,counter) \
try { \
    (expression); \
} catch (infra::exception &ex) { \
  counter++; \
} 
#endif

#define check_bug(expression,msg) \
if (!(expression)) { \
  std::cerr << "Problem with "<< msg << "\n"; \
  return 0; \
} else { \
  std::cerr << "checking " << msg << "..............OK\n"; \
} 



int test_vector()
{
  // -----------------------------------------------------------
  // test constructors
  // -----------------------------------------------------------

  unsigned s = 0;

  // normal constructor
  infra::vector v1(100);
  for (int i=0;i<100;++i) {
    v1(i) = rand() / time(NULL);
  }
  check_exception(v1(100) = 1.0,s);

  // default constructor
  unsigned long len = 0;
  infra::vector v2(len); // empty vector
  check_exception(v2(0) = 1.0,s);
  check_bug(s == 2,"limits");
  
  // copy constructor
  v1(99) = 1.0;
  infra::vector v3(v1);
  v3(99) = 0.0;
  check_bug(v1(99) == 1.0,"copy constructor");
  

  // -----------------------------------------------------------
  // test resize, swap and files
  // -----------------------------------------------------------
  v3(99) = v1(99);
  v2.swap(v1);
  check_bug(!(v2 != v3 || v1.size() != 0),"swap");

  // check resize
  v3.resize(150);
  s = 0;
  s += !(v3.size() != 150 || v2.size() != 100);
  for (unsigned int i=0; i < v2.size(); ++i) {
    s += !(v2(i) != v3(i));
  }
  check_bug(s == v2.size()+1,"resize");
    
  // binary files
  FILE* fid;
  fid = fopen("/tmp/tmp_file.infra","wb");
  v2.ascending(); v3.descending();
  v2.save_binary(fid);
  v3.save_binary(fid);
  fclose(fid);

  FILE *fin;
  fin = fopen("/tmp/tmp_file.infra","rb");
  infra::vector v4(fin);
  infra::vector v5(fin);
  fclose(fin);
  check_bug(!(v4 != v2 || v5 != v3),"binary IO");
  
  // text files
  ofstream os("/tmp/tmp_file.txt");
  os << v2 << "\n" << v3 << "\n";
  os.close();
  ifstream is("/tmp/tmp_file.txt");
  is >> v4 >> v5;
  check_bug(!(v4 != v2 || v5 != v3),"text IO");
  is.close();

  ifstream is2("/tmp/tmp_file.txt");
  infra::vector v10(is2);
  check_bug(v10 == v4,"ifstream constructor");
  is2.close();
  
  return 1;
}



int test_vector_base()
{
  uint s = 0;

  // -----------------------------------------------------------
  // test constructors
  // -----------------------------------------------------------
  infra::vector u1(10);
  for (unsigned int i=0;i<u1.size();++i) {
    u1(i) = rand() / time(NULL);
  }

  infra::vector_base v1(u1);
  s += !(v1.size() != u1.size() || &v1(0) != &u1(0));
  infra::vector_base v2 = u1;
  s += !(v2.size() != u1.size() || &v1(0) != &u1(0));
  check_bug(s == 2,"constructor");


  FILE *fin = fopen("/tmp/tmp_file.infra","rb");
  s = 0;
  check_exception(v1.load_binary(fin),s);
  fclose(fin);
  fin = fopen("/tmp/tmp_file.infra","rb");
  check_exception(u1.load_binary(fin),s);
  fclose(fin);
  fin = fopen("/tmp/tmp_file.infra","rb");
  u1.resize(100);
  u1.load_binary(fin);
  fclose(fin);
  infra::vector u3(u1.size());
  infra::vector_base v3 = u3;
  fin = fopen("/tmp/tmp_file.infra","rb");
  v3.load_binary(fin);
  fclose(fin);
  s += (v3 == u1 && u3 == u1);
  check_bug(s == 3,"binary IO");

  infra::vector looloo(5);
  looloo.zeros();
  ifstream is("/tmp/tmp_file.txt");
  s = 0;
  check_exception(is >> looloo,s);
  is.close();
  ifstream is2("/tmp/tmp_file.txt");
  is2 >> v3;
  s += !(v3 != u1 || u3 != u1);
  check_bug(s == 2,"text IO");


  // Checking operator = and copy_neg
  s = 0;
  infra::vector u4(u3.size());
  u4 = u3;
  s += (u4 ==u3);
  check_exception(v1 = u4,s);
  check_bug(s == 2,"operator =");
  u4.copy_neg(u3);
  int equal_neg = 1;
  for (unsigned int i=0; i< u4.size(); ++i) {
    equal_neg *= (u3(i) == -u4(i));
  }
  s = equal_neg;
  check_exception(v1.copy_neg(u4),s);
  check_bug(s == 2,"copy_neg");

  // Checking operators [] and ()
  infra::vector u5(10);
  u5.ascending();
  infra::vector_base v5 = u5;
  s = 0;
  for (unsigned int i=0; i<u5.size();++i) {
    s += !(double(i) != u5(i) || u5(i) != v5(i));
  }
  check_bug(s == u5.size(),"operators() and []");
  
  
  // Check front and back
  check_bug(!(u5(0) != u5.front() || u5(u5.size()-1) != u5.back()),"front and back");

  // check iterators
  infra::vector_base::iterator iter = u5.begin();
  while (iter < u5.end()) {
    *iter *= 2.0;
    ++iter;
  }
  infra::vector u6(u5.size());
  u6.ascending(); u6 *= 2.0;
  check_bug(!(u5 != u6 || u5.back() != (u5.size()-1)*2.0),"begin and end");

  // checking == and !=
  bool equal = 1;
  for (unsigned int i=0; i<u5.size(); ++i) {
    equal = equal && (u5(i) == u6(i));
  }
  check_bug(((u5 == u6) == equal),"operator ==");
  s = 1;
  for (unsigned int i=0; i<u5.size();++i) {
    u5(i) = -1.0;
    if (!(u5 != u6) || (u5 == u6)) {
      s = 0;
    }
    u5(i) = u6(i);
  }
  check_bug(s,"operator !=");

  // checking zeros, ones and ascending
  infra::vector u7(10);
  infra::vector u8(10);
  u5.zeros(); u6.ones(); u7.ascending(); u8.descending();
  check_bug(!(u5.sum() != 0.0 || u6.sum() != u6.size()),"ones() and zeros()");
  s = 1;
  for (unsigned int i=0; i< u7.size();++i) {
    if (u7(i) != u8(u8.size()-1-i)) {
      s = 0;
    }
  }
  check_bug(s,"{as|de}cending");

  // checking arithmetic operators
  v5 = 1.0;
  u6.ones();
  check_bug(!(v5 != u6),"= scalar");
  v5.zeros() += 1.0;
  check_bug(!(v5 != u6),"+= scalar");
  (u5.ones() *= 2.0) -= u6;
  check_bug(!(v5 != u6),"*= scalar");
  (u5.ones() *= 3.0) /= 3.0;
  check_bug(!(v5 != u6),"/= scalar");
  (u5.ones() += u6) -= u6;
  check_bug(!(v5 != u6),"-= vector");
  u6 *= 2.0; u5.ones() *= 2.0;
  (u5 *= u6) /= u6;
  check_bug(!(v5 != u6),"*= vector and /= vector");
  u5.pow(2.0); u6 *= u6;
  check_bug(!(v5 != u6),"pow");
  u5.exp(); u6.exp(1.0); u5 *= exp(1.0);
  check_bug(!(v5 != u6),"exp");

  // check max etc.
  infra::vector_view v6(u6);
  v6.ascending(); v5.descending();
  check_bug(!(v6.max() != double(v6.size()-1) ||
      v6.min() != 0.0 ||
      v6.argmax() != v6.size()-1 ||
      v6.argmin() != 0 ||
      v5.max() != double(v5.size()-1) ||
      v5.min() != 0.0 ||
      v5.argmax() != 0 ||
      v5.argmin() != v5.size() - 1),"max,min,argmax,argmin");

  // checking sum, norm1 and norm2
  double sm = 0.0,l1 = 0.0, l2 =0.0;
  for (unsigned int i=0;i<v6.size();++i) {
        v6(i) = rand() / time(NULL);
	sm += v6(i);
	l1 += v6(i)>0? v6(i) : -v6(i);
	l2 += v6(i)*v6(i);
  }
  check_bug(!(sm != v6.sum() || l1 != v6.norm1() || l2 != v6.norm2()),"sum,norm1,norm2");
  
  infra::vector u10(100);
  u10.zeros();
  u10.subvector(0,10).ones();
  s = 0;
  s += !(u10.sum() != 10.0 || u10.subvector(5,10).sum() != 5.0);
  infra::vector_base v10 = u10.subvector(20,30);
  s += !(&v10(0) != &u10(20) || &v10(v10.size()-1) != &u10(49));
  check_bug(s == 2,"subvector");
  
  return 1;
}


int test_infra_vv_funcs()
{

  infra::vector u1(10);
  u1.ones();
  infra::vector u2(u1.size());
  for (unsigned int i=0;i<u2.size();++i) {
    u2(i) = rand() / time(NULL);
  }

  double outcome = 0.0;
  prod(u1,u2,outcome);
  check_bug(!(outcome != u2.sum() || u1*u2 != u2.sum()),"prod and *");
  
  add_prod(u1,u2,outcome);
  check_bug(!(outcome != 2*u2.sum()),"add_prod");

  infra::vector u3(u1.size());
  sum(u1,u2,u3);
  u3 -= 1.0;
  check_bug(!(u3 != u2),"vector sum");

  u3 += 1.0;
  check_bug(!(u3 != (u1+u2) || (u3-u1) != u2),"operator +,-");
  
  diff(u2,u1,u3);
  check_bug(!(u3 != (u2-u1)),"diff");

  coordwise_mult(u1,u2,u3);
  unsigned int s=0;
  for (unsigned int i=0;i<u1.size();++i) {
    s += (u3(i) == u1(i)*u2(i));
  }
  check_exception(coordwise_mult(u1,u2.subvector(0,3),u3),s);
  infra::vector_base v3 = u3.subvector(0,3);
  check_exception(coordwise_mult(u1,u2,v3),s);
  check_bug((s == u1.size()+2),"coordwise_mult");

  u2 += 1.0;
  coordwise_div(u1,u2,u3);
  s=0;
  for (unsigned int i=0;i<u1.size();++i) {
    s += (u3(i) == u1(i)/u2(i));
  }
  check_exception(coordwise_div(u1,u2.subvector(0,3),u3),s);
  check_exception(coordwise_mult(u1,u2,v3),s);
  check_bug((s == u1.size()+2),"coordwise_div");


  outcome = 0.0;
  for (unsigned int i =0;i < u2.size();++i) {
    outcome += (u2(i)-u1(i))*(u2(i)-u1(i));
  }
  double outcome2 = 0.0;
  dist2(u1,u2,outcome2);
  check_bug((outcome2 == outcome && dist2(u2,u1) == outcome),"dist2");

  s = 0;
  u3 = u1+2.0;
  for (uint i=0;i < u3.size(); ++i) s += (u3(i) == u1(i)+2.0);
  u3 = 2.0+u1;
  for (uint i=0;i < u3.size(); ++i) s += (u3(i) == u1(i)+2.0);
  check_bug(s == 2*u3.size(),"operator +");

  s = 0;
  u3 = u1*2.0;
  for (uint i=0;i < u3.size(); ++i) s += (u3(i) == u1(i)*2.0);
  u3 = 2.0*u1;
  for (uint i=0;i < u3.size(); ++i) s += (u3(i) == u1(i)*2.0);
  check_bug(s == 2*u3.size(),"operator *");
  
  s = 0;
  u3 = u1-2.0;
  for (uint i=0;i < u3.size(); ++i) s += (u3(i) == u1(i)-2.0);
  u3 = 2.0-u1;
  for (uint i=0;i < u3.size(); ++i) s += (u3(i) == 2.0-u1(i));
  check_bug(s == 2*u3.size(),"operator -");

  s = 0;
  u3 = u1/2.0;
  for (uint i=0;i < u3.size(); ++i) s += (u3(i) == u1(i)/2.0);
  u3 = 2.0/u1;
  for (uint i=0;i < u3.size(); ++i) s += (u3(i) == 2.0/u1(i));
  check_bug(s == 2*u3.size(),"operator /");

  return 1;
}



int test_matrix()
{
  // -----------------------------------------------------------
  // test constructors
  // -----------------------------------------------------------

  unsigned s = 0;

  // normal constructor
  infra::matrix M1(10,10);
  for (uint i=0;i<10;++i) {
    for (uint j=0;j<10;++j) {
      M1(i,j) = rand() / time(NULL);
    }
  }
  check_exception(M1(10,0) = 1.0,s);
  check_exception(M1(0,10) = 1.0,s);


  // default constructor
  infra::matrix M2(0,0); // empty vector
  check_exception(M2(0,0) = 1.0,s);
  check_bug(s == 3,"limits");
  
  // copy constructor
  M1(9,9) = 1.0;
  infra::matrix M3(M1);
  M3(9,9) = 0.0;
  check_bug(M1(9,9) == 1.0,"copy constructor");
  

  // -----------------------------------------------------------
  // test resize, reshape and files
  // -----------------------------------------------------------
  M1.resize(5,5); 
  M1.zeros();
  for (uint i=0; i < M1.height();++i) M1.row(i) += i;
  for (uint i=0; i < M1.width();++i) M1.column(i) += i;
  
  M1.resize(6,5);
  s = 0;
  for (uint i=0; i < 5; ++i) {
    for (uint j=0; j < 5; ++j) {
      s += (M1(i,j) == i+j);
    }
  }
  s = (s == 25 && M1.height() == 6 && M1.width() == 5);
  M1.resize(6,6);
  for (uint i=0; i < 5; ++i) {
    for (uint j=0; j < 5; ++j) {
      s += (M1(i,j) == i+j);
    }
  }
  s = (s == 26 && M1.height() == 6 && M1.width() == 6);
  M1.resize(3,3);
  for (uint i=0; i < 3; ++i) {
    for (uint j=0; j < 3; ++j) {
      s += (M1(i,j) == i+j);
    }
  }
  check_bug(s == 10 && M1.height() == 3 && M1.width() == 3,"resize");

  s = 0;
  M1.column(0).zeros();
  M1.column(1).ascending();
  M1.column(2).descending();
  infra::vector_base v = M1.reshape(1,9).row(0);
  for (uint i=0; i < 3; ++i) s += (v(i) == 0.0);
  for (uint i=0; i < 3; ++i) s += (v(3+i) == i);
  for (uint i=0; i < 3; ++i) s += (v(6+i) == 2-i);
  check_bug(s == 9 && v.size() == 9,"reshape");
  
  // binary files
  M3.row(0).ones();
  M3.row(1).ones() *= 2.0;
  M3.row(2).ones() *= 3.0;
  FILE* fid;
  fid = fopen("/tmp/tmp_file.infra","wb");
  M1.save_binary(fid);
  M3.save_binary(fid);
  fclose(fid);

  FILE *fin;
  fin = fopen("/tmp/tmp_file.infra","rb");
  infra::matrix M4(fin);
  infra::matrix M5(fin);
  fclose(fin);
  check_bug(!(M4 != M1 || M5 != M3),"binary IO");

  // text files
  ofstream os("/tmp/tmp_file.txt");
  os << M1 << "\n" << M3 << "\n";
  os.close();
  ifstream is("/tmp/tmp_file.txt");
  is >> M4 >> M5;
  check_bug(!(M4 != M1 || M5 != M3),"text IO");
  is.close();

  ifstream is2("/tmp/tmp_file.txt");
  infra::matrix M10(is2);
  check_bug(M10 == M4,"ifstream constructor");
  is2.close();

  
  return 1;
}



int test_matrix_base() {
  uint s = 0;
  
  // test constructors
  infra::matrix M1(10,5);
  for (uint i=0;i<M1.height();++i) {
    for (uint j=0;j<M1.width(); ++j) {
      M1(i,j) = rand() / time(NULL);
    }
  }
  infra::matrix_base vM1(M1);
  s += (vM1 == M1);
  infra::matrix_base vM2 = M1;
  s += ((vM2 == M1) && (&vM2(0,0) == &M1(0,0)));
  check_bug(s == 2,"constructor");

  // check swap
  infra::matrix M2(3,3);
  M2.zeros();
  M2.swap(M1);
  check_bug(vM1 == M2 && M1.width() == 3 && M1.height() == 3 &&
	    (M1.row(0).sum()+M1.row(1).sum()+M1.row(2).sum()) == 0,"swap");

  // check files
  FILE *fin = fopen("/tmp/tmp_file.infra","rb");
  s = 0;
  check_exception(M1.load_binary(fin),s);
  fclose(fin);
  fin = fopen("/tmp/tmp_file.infra","rb");
  check_exception(vM1.load_binary(fin),s);
  fclose(fin);
  fin = fopen("/tmp/tmp_file.infra","rb");
  M1.resize(1,9);
  M1.load_binary(fin);
  fclose(fin);
  infra::matrix M3(M1.height(),M1.width());
  infra::matrix_base vM3 = M3;
  fin = fopen("/tmp/tmp_file.infra","rb");
  vM3.load_binary(fin);
  fclose(fin);
  s += (vM3 == M1 && M3 == M1);
  check_bug(s == 3,"binary IO");


  infra::matrix looloo(5,5);
  looloo.zeros();
  ifstream is("/tmp/tmp_file.txt");
  s = 0;
  check_exception(is >> looloo,s);
  is.close();
  ifstream is2("/tmp/tmp_file.txt");
  is2 >> vM3;
  s += !(vM3 != M1 || M3 != M1);
  check_bug(s == 2,"text IO");

  // operator = 
  infra::matrix M4(21,21);
  s = 0;
  check_exception(M4 = M3,s);
  M4.submatrix(1,1,M3.height(),M3.width()) = M3;
  for (uint i=0;i<M3.height();++i) {
    for (uint j=0;j<M3.width();++j) {
      s = (s && M3(i,j)==M4(i+1,j+1));
    }
  }
  check_bug(s,"operator =");

  // check iterators and () operator
  infra::matrix_base::iterator this_iter = M4.submatrix(1,1,M3.height(),M3.width()).begin();
  infra::matrix_base::iterator this_end = M4.submatrix(1,1,M3.height(),M3.width()).end();
  s = 1;
  for (uint i=0;i<M3.height();++i) {
    for (uint j=0;j<M3.width();++j) {
      s = (s && (*this_iter==M4(i+1,j+1)) && (this_iter < this_end));
      this_iter++;
    }
  }
  check_bug(s,"iterators and operator()");

  // submatrix
  this_iter = M4.begin(); 
  this_end = M4.end();
  while (this_iter < this_end) {
    *this_iter = rand() / time(NULL);
    ++this_iter;
  }
  infra::matrix_base vM4 = M4.submatrix(2,3,3,3);
  s  = 1;
  for (uint i=0;i < 3;++i) {
    for (uint j=0;j < 3;++j) {
      s = (s && (vM4(i,j) == M4(2+i,3+j)));
    }
  }
  check_exception(M4.submatrix(M4.height(),0,2,2),s);
  check_exception(M4.submatrix(2,0,M4.height()-1,2),s);
  check_bug(s == 3,"submatrix");
  
  // check row
  s = 0;
  check_exception(M4.row(M4.height()),s);
  infra::vector_base vvv = M4.row(2);
  s = (s && vvv.size()==M4.width());
  for (uint i=0;i<M4.width();++i) s = (s && M4(2,i)==vvv(i));
  check_bug(s,"row");

  // check column
  s = 0;
  check_exception(M4.column(M4.width()),s);
  infra::vector_base vvv_col = M4.column(2);
  s = (s && vvv_col.size()==M4.height());
  for (uint i=0;i<M4.height();++i) s = (s && M4(i,2)==vvv_col(i));
  check_bug(s,"column");
  

  // check diagonal
  infra::matrix M5(5,3);
  for (uint i=0;i < M5.height();++i) {
    for (uint j=0;j < M5.width();++j) {
      M5(i,j) = (double)i- (double)j;
    }
  }
  s = (M5.diagonal(0).sum() == 0);
  s = (s && (M5.diagonal(1).norm2() == 2.0) && (M5.diagonal(1).sum() == -2.0));
  s = (s && (M5.diagonal(2).norm2() == 4.0) && (M5.diagonal(2).sum() == -2.0));
  s = (s && (M5.diagonal(-1).norm2() == 3.0) && 
       (M5.diagonal(-1).sum() == 3.0));
  s = (s && (M5.diagonal(-2).norm2() == 12.0) && 
       (M5.diagonal(-2).sum() == 6.0));
  s = (s && (M5.diagonal(-3).norm2() == 18.0) && 
       (M5.diagonal(-3).sum() == 6.0));
  s = (s && (M5.diagonal(-4).norm2() == 16.0) && 
       (M5.diagonal(-4).sum() == 4.0));
  check_exception(M5.diagonal(3),s);
  check_exception(M5.diagonal(-5),s);
  check_bug(s == 3,"diagonal");


  // check zeros
  M5.zeros(); s = 1;
  for (uint i=0;i<M5.height();++i) {
    for (uint j=0;j<M5.width();++j) {
      s = (s && (M5(i,j) == 0.0));
    }
  }
  M5.ones(); 
  for (uint i=0;i<M5.height();++i) {
    for (uint j=0;j<M5.width();++j) {
      s = (s && (M5(i,j) == 1.0));
    }
  }
  M5.eye();
  for (uint i=0;i<M5.height();++i) {
    for (uint j=0;j<M5.width();++j) {
      if (i==j) s = (s && (M5(i,j) == 1.0));
      else s = (s && (M5(i,j) == 0.0));
    }
  }
  check_bug(s,"zeros, ones, eye");


  // scalar operators
  M5 = 2.5;
  for (uint i=0;i<M5.height();++i) {
    for (uint j=0;j<M5.width();++j) {
      s = (s && (M5(i,j) == 2.5));
    }
  }
  M5 += 2.5;
  for (uint i=0;i<M5.height();++i) {
    for (uint j=0;j<M5.width();++j) {
      s = (s && (M5(i,j) == 5.0));
    }
  }
  M5 -= 0.5;
  for (uint i=0;i<M5.height();++i) {
    for (uint j=0;j<M5.width();++j) {
      s = (s && (M5(i,j) == 4.5));
    }
  }
  M5 *= 2.0;
  for (uint i=0;i<M5.height();++i) {
    for (uint j=0;j<M5.width();++j) {
      s = (s && (M5(i,j) == 9.0));
    }
  }
  M5 /= 3.0;
  for (uint i=0;i<M5.height();++i) {
    for (uint j=0;j<M5.width();++j) {
      s = (s && (M5(i,j) == 3.0));
    }
  }
  check_bug(s,"matrix-scalar operators");

  
  // matrix-matrix operators
  s = 0;
  check_exception(M5 += M5.submatrix(0,0,1,M5.width()) ,s );
  check_exception(M5 += M5.submatrix(0,0,M5.height(),1) , s );
  s = (s == 2);
  M5.ones();
  M5 += M5;
  for (uint i=0;i<M5.height();++i) {
    for (uint j=0;j<M5.width();++j) {
      s = (s && (M5(i,j) == 2.0));
    }
  }
  M5 *= M5;
  for (uint i=0;i<M5.height();++i) {
    for (uint j=0;j<M5.width();++j) {
      s = (s && (M5(i,j) == 4.0));
    }
  }
  M5 /= M5;
  for (uint i=0;i<M5.height();++i) {
    for (uint j=0;j<M5.width();++j) {
      s = (s && (M5(i,j) == 1.0));
    }
  }
  M5 -= M5;
  for (uint i=0;i<M5.height();++i) {
    for (uint j=0;j<M5.width();++j) {
      s = (s && (M5(i,j) == 0.0));
    }
  }
  check_bug(s,"matrix-matrix operators");


  // check exp and pow
  M5.zeros() += 2.5;
  M5.pow(3.5);
  for (uint i=0;i<M5.height();++i) {
    for (uint j=0;j<M5.width();++j) {
      s = (s && (M5(i,j) == pow(2.5,3.5)));
    }
  }
  M5.zeros() += 2.5;
  M5.exp(3.5);
  for (uint i=0;i<M5.height();++i) {
    for (uint j=0;j<M5.width();++j) {
      s = (s && (M5(i,j) == exp(2.5 + 3.5)));
    }
  }
  M5.zeros() += 2.5;
  M5.exp();
  for (uint i=0;i<M5.height();++i) {
    for (uint j=0;j<M5.width();++j) {
      s = (s && (M5(i,j) == exp(2.5)));
    }
  }

  check_bug(s,"pow and exp");
  

  // check max and min
  M5.zeros(); double tmp = 1.0;
  for (uint i=0;i<M5.height();++i) {
    for (uint j=0;j<M5.width();++j) {
      M5(i,j) = tmp;
      s = (s && (M5.max() == tmp));
      tmp++;
    }
  }
  M5.zeros(); tmp = -1.0;
  for (uint i=0;i<M5.height();++i) {
    for (uint j=0;j<M5.width();++j) {
      M5(i,j) = tmp;
      //s = (s && (M5.min() == tmp));
      tmp--;
    }
  }
  check_bug(s,"max and min");


  // check swap_rows and swap_columns
  s = 0;
  check_exception(M5.swap_rows(0,M5.height()),s);
  check_exception(M5.swap_rows(M5.height(),0),s);
  check_exception(M5.swap_columns(0,M5.width()),s);
  check_exception(M5.swap_columns(M5.width(),0) , s);
  s = (s == 4);
  infra::vector v_row0(M5.row(0));
  infra::vector v_row1(M5.row(1));
  M5.swap_rows(0,1);
  s = (s && (v_row0 == M5.row(1)) && (v_row1 == M5.row(0)));
  infra::vector v_col0(M5.column(0));
  infra::vector v_col1(M5.column(1));
  M5.swap_columns(0,1);
  s = (s && (v_col0 == M5.column(1)) && (v_col1 == M5.column(0)));
  check_bug(s,"swap_rows and swap_columns");

       
  return 1;
}



int test_infra_vm_funcs() {

  infra::matrix M1(3,5);
  infra::vector v1(3);
  infra::vector v2(3);
  infra::vector v3(5);
  infra::vector v4(5);

  for (uint i=0;i<M1.height();++i) {
    v1(i) = rand() / time(NULL);
    for (uint j=0;j<M1.width();++j) {
      M1(i,j) = rand() / time(NULL);
      v3(j) = rand() / time(NULL);
    }
  }

  int s = 0;
  check_exception(prod(M1,v3,v4),s);
  check_exception(prod(M1,v1,v2),s);
  check_exception(v4 = M1*v3,s);
  check_exception(v2 = M1*v1,s);
  check_exception(prod(v1,M1,v2),s);
  check_exception(prod(v3,M1,v4),s);
  check_exception(v2 = v1*M1,s);
  check_exception(v4 = v3*M1,s);
  s = (s == 8);
  prod(v1,M1,v4);
  for (uint i=0;i<M1.width();++i) {
    s = (s && (v4(i) == M1.column(i)*v1));
  }
  v4.zeros();
  v4 = v1*M1;
  for (uint i=0;i<M1.width();++i) {
    s = (s && (v4(i) == M1.column(i)*v1));
  }
  prod(M1,v3,v2);
  for (uint i=0;i<M1.height();++i) {
#ifndef HERE_BLAS_BUG
    s = (s && (v2(i) == (M1.row(i)*v3)) );
#endif    
  }

  check_bug(s,"prod and operator *");
  v2.zeros();
  v2 = M1*v3;
  for (uint i=0;i<M1.height();++i) {
#ifndef HERE_BLAS_BUG
    s = (s && (v2(i) == M1.row(i)*v3));
#endif
  }
  
  check_bug(s,"prod and operator *");

  

  // check add_prod and subtract_prod
  s = 0;
  check_exception(add_prod(M1,v3,v4),s);
  check_exception(add_prod(M1,v1,v2),s);
  check_exception(add_prod(v1,M1,v2),s);
  check_exception(add_prod(v3,M1,v4),s);
  check_exception(subtract_prod(M1,v3,v4),s);
  check_exception(subtract_prod(M1,v1,v2),s);
  check_exception(subtract_prod(v1,M1,v2),s);
  check_exception(subtract_prod(v3,M1,v4),s);
  s = (s == 8);
  v4.ones();
  add_prod(v1,M1,v4);
  for (uint i=0;i<M1.width();++i) {
    s = (s && (v4(i) == 1+M1.column(i)*v1));
  }
  v4.ones();
  subtract_prod(v1,M1,v4);
  for (uint i=0;i<M1.width();++i) {
    s = (s && (v4(i) == 1-M1.column(i)*v1));
  }
  v2.ones();
  add_prod(M1,v3,v2);
  for (uint i=0;i<M1.height();++i) {
#ifndef HERE_BLAS_BUG
    s = (s && (v2(i) == 1+M1.row(i)*v3));
#endif
  }
  v2.ones();
  subtract_prod(M1,v3,v2);
  for (uint i=0;i<M1.height();++i) {
#ifndef HERE_BLAS_BUG
    s = (s && (v2(i) == 1-M1.row(i)*v3));
#endif
  }
  check_bug(s,"add_prod and subtract_prod");
  
  // check row_min
  s = 0;
  check_exception(row_min(M1,v4),s);
  check_exception(row_max(M1,v4),s);
  check_exception(row_sum(M1,v4),s);
  check_exception(column_min(M1,v2),s);
  check_exception(column_max(M1,v2),s);
  check_exception(column_sum(M1,v2),s);
  s = (s == 6);
  row_min(M1,v2);
  for (uint i=0;i<v2.size();++i) {
    s = (s && (v2(i) == M1.row(i).min()));
  }
  row_max(M1,v2);
  for (uint i=0;i<v2.size();++i) {
    s = (s && (v2(i) == M1.row(i).max()));
  }
  row_sum(M1,v2);
  for (uint i=0;i<v2.size();++i) {
    s = (s && (v2(i) == M1.row(i).sum()));
  }
  column_min(M1,v4);
  for (uint i=0;i<v4.size();++i) {
    s = (s && (v4(i) == M1.column(i).min()));
  }
  column_max(M1,v4);
  for (uint i=0;i<v4.size();++i) {
    s = (s && (v4(i) == M1.column(i).max()));
  }
  column_sum(M1,v4);
  for (uint i=0;i<v4.size();++i) {
    s = (s && (v4(i) == M1.column(i).sum()));
  }

  check_bug(s,"row/column min/max/sum");

  // add_row_vector
  s = 0;
  check_exception(add_row_vector(M1,v2),s);
  check_exception(subtract_row_vector(M1,v2),s);
  check_exception(add_column_vector(M1,v4),s);
  check_exception(subtract_column_vector(M1,v4),s);
  s = (s == 4);
  M1.ones();
  add_row_vector(M1,v3);
  v4 = v3; v4 += 1.0;
  for (uint i=0;i<M1.height();++i) {
    s = (s && (M1.row(i) == v4));
  }
  subtract_row_vector(M1,v3);
  v4.ones();
  for (uint i=0;i<M1.height();++i) {
    s = (s && (M1.row(i) == v4));
  }

  M1.ones();
  add_column_vector(M1,v1);
  v2 = v1; v2 += 1.0;
  for (uint i=0;i<M1.width();++i) {
    s = (s && (M1.column(i) == v2));
  }
  subtract_column_vector(M1,v1);
  v2.ones();
  for (uint i=0;i<M1.width();++i) {
    s = (s && (M1.column(i) == v2));
  }
  
  check_bug(s,"add/subtract_row/column_vector");
  
  return 1;
}


int test_infra_mm_funcs() {

  int s;
  infra::matrix M1(3,5);
  infra::matrix M2(5,4);
  infra::matrix M3(3,4);

  for (uint i=0;i<M1.height();++i) {
    for (uint j=0;j<M1.width();++j) {
      M1(i,j) = rand() / time(NULL);
    }
  }
  for (uint i=0;i<M2.height();++i) {
    for (uint j=0;j<M2.width();++j) {
      M2(i,j) = rand() / time(NULL);
    }
  }
  for (uint i=0;i<M3.height();++i) {
    for (uint j=0;j<M3.width();++j) {
      M3(i,j) = rand() / time(NULL);
    }
  }


 
  // check prod
  s = 0;
  infra::matrix looolooo(2,2);
  check_exception(prod(M1,M2,looolooo),s);
  check_exception(prod(M1,looolooo,M3),s);
  check_exception(prod(looolooo,M2,M3),s);
  check_exception(looolooo = M1*M2,s);
  check_exception(M3 = M1*looolooo,s);
  check_exception(M3 = looolooo*M2,s);
  s = (s == 6);

  infra::matrix M4(M3);
  M4.zeros();
#ifndef HERE_BLAS_BUG  
  prod(M1,M2,M4);
  for (uint i=0;i<M4.height();++i) {
    for (uint j=0;j<M4.width();++j) {
      s = (s && (M4(i,j) == M1.row(i)*M2.column(j)));
    }
  }
  M3 = M4;
  M4 = M1*M2;
  s = (s && (M3 == M4));
  check_bug(s,"prod and operator *");
  
  
  // check prod_t and t_prod
  infra::matrix M5(M1.width(),M1.height());
  for (uint i=0;i<M1.height();++i) {
    for (uint j=0;j<M1.width();++j) {
      M5(j,i) = M1(i,j);
    }
  }
  infra::matrix M6(M2.width(),M2.height());
  for (uint i=0;i<M2.height();++i) {
    for (uint j=0;j<M2.width();++j) {
      M6(j,i) = M2(i,j);
    }
  }
  s = 0;
  check_exception(t_prod(M5,looolooo,M3),s);
  check_exception(t_prod(M5,M2,looolooo),s);
  check_exception(t_prod(M1,M2,M3),s);
  check_exception(prod_t(looolooo,M6,M3),s);
  check_exception(prod_t(M1,M6,looolooo),s);
  check_exception(prod_t(M1,M2,M3),s);
  s = (s == 6);
  M4.zeros(); M3.zeros();
  t_prod(M5,M2,M3); prod(M1,M2,M4);
  s = (s && (M3 == M4));
  M4.zeros(); M3.zeros();
  prod_t(M1,M6,M3); prod(M1,M2,M4);
  s = (s && (M3 == M4));
  
  check_bug(s,"prod_t and t_prod");
#endif
  

  // check opeartors
  infra::matrix A(7,8);
  infra::matrix B(A);
  infra::matrix C(A);

  s = 0;
  check_exception(sum(A,B,looolooo),s);
  check_exception(sum(A,looolooo,B),s);
  check_exception(sum(looolooo,B,C),s);
  check_exception(looolooo = A+B,s);
  check_exception(C = looolooo +B,s);
  check_exception(C = B + looolooo,s);
  check_exception(diff(A,B,looolooo),s);
  check_exception(diff(A,looolooo,B),s);
  check_exception(diff(looolooo,B,C),s);
  check_exception(looolooo = A-B,s);
  check_exception(C = looolooo -B,s);
  check_exception(C = B - looolooo,s);
  check_exception(coordwise_mult(A,B,looolooo),s);
  check_exception(coordwise_mult(A,looolooo,B),s);
  check_exception(coordwise_mult(looolooo,B,C),s);
  check_exception(coordwise_div(A,B,looolooo),s);
  check_exception(coordwise_div(A,looolooo,B),s);
  check_exception(coordwise_div(looolooo,B,C),s);
  s = (s == 18);

  A.ones() *= 2.0;
  A.row(0).zeros();
  B.ones();

  sum(A,B,C);
  for (uint i=0;i<A.height();++i) {
    for (uint j=0;j<A.width();++j) {
      s = (s && (C(i,j) == A(i,j)+B(i,j)));
    }
  }
  infra::matrix D(A);
  D = A + B;
  s = (s && (D == C));
  C = D - A;
  s = (s && (B == C));
  C.zeros();
  diff(D,A,C);
  s = (s && (B == C));

  coordwise_mult(A,B,C);
  coordwise_mult(A,B,D);
  for (uint i=0;i<A.height();++i) {
    for (uint j=0;j<A.width();++j) {
      s = (s && (C(i,j) == A(i,j)*B(i,j)));
      s = (s && (D(i,j) == A(i,j)/B(i,j)));
    }
  }

  check_bug(s,"coordwise operators");
  

  // matrix-scalar operators
  C = A + 2.0;
  B.ones(); B *= 2.0;  D = A+B;
  s = (s && (C == D));
  C.zeros();
  C = 2.0 + A;
  s = (s && (C == D));

  C = A - 2.0;
  B.ones(); B *= 2.0;  D = A-B;
  s = (s && (C == D));
  C.zeros();
  C = 2.0 - A;
  D = B-A;
  s = (s && (C == D));
  
  C = A * 2.0;
  B.ones(); B *= 2.0;  coordwise_mult(A,B,D);
  s = (s && (C == D));
  C.zeros();
  C = 2.0 * A;
  coordwise_mult(B,A,D);
  s = (s && (C == D));

  A += 100.0;
  C = A / 2.0;
  B.ones(); B *= 2.0;  coordwise_div(A,B,D);
  s = (s && (C == D));
  C.zeros();
  C = 2.0 / A;
  coordwise_div(B,A,D);
  s = (s && (C == D));
    
  check_bug(s,"matrix-scalar operators");
  
#ifdef LALA  
#endif

  return 1;

}

int main()
{

  // Testing vector
  std::cerr << "TESTING vector class\n"
	    << "====================\n";
  if (!test_vector()) {
    std::cerr << "Problem with test_vector\n";
    return 0;
  }


  // Testing vector_base
  std::cerr << "TESTING vector_base class\n"
	    << "=========================\n";
  if (!test_vector_base()) {
    std::cerr << "Problem with test_vector_base\n";
    return 0;
  }

  // Testing infra_vv_funcs
  std::cerr << "TESTING vector-vector operations\n"
	    << "================================\n";
  if (!test_infra_vv_funcs()) {
    std::cerr << "Problem with test_infra_vv_funcs\n";
    return 0;
  }
  

  // Test matrix
  std::cerr << "TESTING matrix class\n"
	    << "====================\n";
  if (!test_matrix()) {
    std::cerr << "Problem with test_matrix\n";
    return 0;
  }

  // Test matrix_base
  std::cerr << "TESTING matrix_base class\n"
	    << "====================\n";
  if (!test_matrix_base()) {
    std::cerr << "Problem with test_matrix_base\n";
    return 0;
  }


  // Test infra_vm_funcs
  std::cerr << "TESTING infra_vm_funcs\n"
	    << "====================\n";
  if (!test_infra_vm_funcs()) {
    std::cerr << "Problem with test_infra_vm_funcs\n";
    return 0;
  }

  // Test infra_mm_funcs
  std::cerr << "TESTING infra_mm_funcs\n"
	    << "====================\n";
  if (!test_infra_mm_funcs()) {
    std::cerr << "Problem with test_infra_mm_funcs\n";
    return 0;
  }


  
  std::cerr << "\nDONE.\n";

  
  return 0;
}




/*
Bugs:
- There is no default constructor for vector.
- copy_neg doesn't check size (like operator =).

Questions:
- Why resize returns vector_base?
- Same question about load_binary.


*/
