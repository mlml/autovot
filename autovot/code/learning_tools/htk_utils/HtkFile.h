
#ifndef _JKESHET_HTKFILE_H_
#define _JKESHET_HTKFILE_H_

#include <stdio.h>
#include <string>
#include <stdint.h>

#define HASENERGY  0100       /* _E log energy included */
#define HASNULLE   0200       /* _N absolute energy suppressed */
#define HASDELTA   0400       /* _D delta coef appended */
#define HASACCS   01000       /* _A acceleration coefs appended */
#define HASCOMPX  02000       /* _C is compressed */
#define HASZEROM  04000       /* _Z zero meaned */
#define HASCRCC  010000       /* _K has CRC check */
#define HASZEROC 020000       /* _0 0'th Cepstra included */
#define HASVQ    040000       /* _V has VQ index attached */
#define HASTHIRD 0100000       /* _T has Delta-Delta-Delta index attached */

#define BASEMASK  077         /* Mask to remove qualifiers */

#include <infra.h>

class HtkFile 
{
 public:
  HtkFile(FILE *stream);
  ~HtkFile();
  size_t num_coefs();
  std::string parmKind2str();
  bool read_header();
  void write_header();
  void print_header();
  size_t read_next_vector(float *data);
  size_t read_next_vector(double *data);
  size_t read_next_vector(infra::vector &data);
  size_t write_next_vector(float *data);
  int nSamples() { return header.nSamples; }
  int sampPeriod() { return header.sampPeriod; }
  short sampSize() { return header.sampSize; }
  short parmKind() { return header.parmKind; }
//  void swap2Bytes(void *src, void *dest);
 // void swap4Bytes(void *src, void *dest);

  struct htk_header {
    int32_t nSamples;
    int32_t sampPeriod;
    short sampSize;
    short parmKind;
  };
  htk_header header;
  FILE *stream;
  bool big_endian;
};


#endif // _JKESHET_HTKFILE_H_
