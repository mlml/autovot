
#include <iostream>
#include "HtkFile.h"

HtkFile::HtkFile(FILE *_stream)
{
  stream = _stream;
  // check endian type of the machine
  unsigned short s = 0x0001;
  big_endian = ! *(unsigned char*)&s;
}

HtkFile::~HtkFile()
{

}

void HtkFile::read_header() 
{
  // This function reads the header into the struct 
	long nSamples;
	long sampPeriod;
	short sampSize;
	short parmKind;

  if (fread(&nSamples, 4, 1, stream) != 1)
    throw std::bad_alloc();
  if (fread(&sampPeriod, 4, 1, stream) != 1)
    throw std::bad_alloc();
  if (fread(&sampSize, 2, 1, stream) != 1)
    throw std::bad_alloc();
  if (fread(&parmKind, 2, 1, stream) != 1)
    throw std::bad_alloc();

	long nSamples2;
	long sampPeriod2;
	short sampSize2;
	short parmKind2;
  if (!big_endian) {
    swap4Bytes(&nSamples, &nSamples2);
    swap4Bytes(&sampPeriod, &sampPeriod2);
    swap2Bytes(&sampSize, &sampSize2);
    swap2Bytes(&parmKind, &parmKind2);
  }
	else {
		nSamples2 = nSamples;
		sampPeriod2 = sampPeriod;
		sampSize2 = sampSize;
		parmKind2 = parmKind;
	}
  
  // Should never find EOF here since you would be expecting the data in 
  // the MultiFrame too.
  if (feof(stream) != 0 || ferror(stream) != 0) 
    throw std::bad_alloc();
  if (sampSize2 <= 0 || sampSize2 > 15000 || nSamples2 <= 0 ||
      sampPeriod2 <= 0 || sampPeriod2 > 1000000) {
    std::cerr << "Error: HTK header is not readable." << std::endl;
    throw std::exception();
  }

	header.nSamples = nSamples2;
	header.sampPeriod = sampPeriod2;
	header.sampSize = sampSize2;
	header.parmKind = parmKind2;
	
}

void HtkFile::write_header() 
{
	int nSamples;
	int sampPeriod;
	short sampSize;
	short parmKind;
	
  // swap bytes first
  if (!big_endian) {
    swap4Bytes(&header.nSamples, &nSamples);
    swap4Bytes(&header.sampPeriod, &sampPeriod);
    swap2Bytes(&header.sampSize, &sampSize);
    swap2Bytes(&header.parmKind, &parmKind);
  }
	else {
    nSamples = header.nSamples;
    sampPeriod = header.sampPeriod;
    sampSize = header.sampSize;
    parmKind = header.parmKind;
	}		

  // This function reads the header into the struct 
  if (fwrite(&nSamples, 4, 1, stream) != 1)
    throw std::bad_alloc();
  if (fwrite(&sampPeriod, 4, 1, stream) != 1)
    throw std::bad_alloc();
  if (fwrite(&sampSize, 2, 1, stream) != 1)
    throw std::bad_alloc();
  if (fwrite(&parmKind, 2, 1, stream) != 1)
    throw std::bad_alloc();
}


size_t HtkFile::num_coefs()
{
  return header.sampSize/sizeof(float);
}

std::string HtkFile::parmKind2str()
{
/* EXPORT-> ParmKind2Str: convert given parm kind to string */
//  static char parmKind_map[][15] = {"WAVEFORM", "LPC", "LPREFC", "LPCEPSTRA", 
//				"LPDELCEP", "IREFC", 
//				"MFCC", "FBANK", "MELSPEC",
//				"USER", "DISCRETE", "PLP",
//		"ANON"};
//  str = parmKind_map[header.parmKind & BASEMASK];

  std::string str;
	short parmKind = header.parmKind & BASEMASK;
	if (parmKind == 0)
		str = "WAVEFORM";
	else if (parmKind == 1)
		str = "LPC";
	else if (parmKind == 2)
		str = "LPREFC";
	else if (parmKind == 3)
		str = "LPCEPSTRA";
	else if (parmKind == 4)
		str = "LPDELCEP";
	else if (parmKind == 5)
		str = "IREFC";
	else if (parmKind == 6)
		str = "MFCC";
	else if (parmKind == 7)
		str = "FBANK";
	else if (parmKind == 8)
		str = "MELSPEC";
	else if (parmKind == 9)
		str = "USER";
	else if (parmKind == 10)
		str = "DISCRETE";
	else if (parmKind == 11)
		str = "PLP";
	else if (parmKind == 12)
		str = "ANON";
  if (header.parmKind & HASENERGY) str += "_E";
  if (header.parmKind & HASDELTA) str += "_D";
  if (header.parmKind & HASACCS) str += "_N";
  if (header.parmKind & HASTHIRD) str += "_A";
  if (header.parmKind & HASNULLE) str += "_T";
  if (header.parmKind & HASCOMPX) str += "_C";
  if (header.parmKind & HASCRCC) str += "_K";
  if (header.parmKind & HASZEROM) str += "_Z";
  if (header.parmKind & HASZEROC) str += "_O";
  if (header.parmKind & HASVQ) str += "_V";
  return str;
}

void HtkFile::print_header()
{
  std::cout << "  nSamples: " << header.nSamples << std::endl;
  std::cout << "  sampPeriod: " << (header.sampPeriod/10.0) << " us" << std::endl;
  std::cout << "  SampSize: " << header.sampSize << std::endl;
  std::cout << "  parmKind: " << parmKind2str() << std::endl;
  std::cout << "  Num Coefs: " << num_coefs() << std::endl;
  std::cout << "  Machine type: " << (big_endian ? "big" : "little") << " endian." << std::endl;
}


size_t HtkFile::read_next_vector(float *data)
{
  size_t rc =  fread(data,sizeof(float),num_coefs(),stream);
  if (!big_endian) {
    for (size_t i = 0; i < rc; i++) {
      swap4Bytes(&(data[i]),&(data[i]));
    }
  }
  return rc;
}

size_t HtkFile::read_next_vector(double *data)
{
  float fdata[num_coefs()];

  size_t rc = fread(fdata,sizeof(float), num_coefs(),stream);
  for (size_t i = 0; i < rc; i++) {
    if (!big_endian) {
      swap4Bytes(&(fdata[i]),&(fdata[i]));
    }
    data[i] = fdata[i];
  }
  return rc;
}

size_t HtkFile::read_next_vector(infra::vector &data)
{
  float fdata[num_coefs()];
  data.resize(num_coefs());
  
  size_t rc = fread(fdata,sizeof(float), num_coefs(),stream);
  for (size_t i = 0; i < rc; i++) {
    if (!big_endian) {
      swap4Bytes(&(fdata[i]),&(fdata[i]));
    }
    data[i] = fdata[i];
  }
  return rc;
}


size_t HtkFile::write_next_vector(float *data)
{
  size_t rc ;
  if (!big_endian) {
    float fdata[num_coefs()];
    for (size_t i = 0; i < (unsigned int)num_coefs(); i++) {
      swap4Bytes(&(data[i]),&(fdata[i]));
    }
    rc =  fwrite(fdata,sizeof(float),num_coefs(),stream);
  }
  else {
    rc =  fwrite(data,sizeof(float),num_coefs(),stream);
  }
  
  return rc;
}

void HtkFile::swap2Bytes(void *src, void *dest)
{
  char *p1,*p2;
  char tmp;
  
  p1 = (char*)src;
  p2 = (char*)dest;
  tmp = p1[1]; /* pour pouvoir utiliser src == dest */
  p2[1] = p1[0];
  p2[0] = tmp;
}

void HtkFile::swap4Bytes(void *src, void *dest)
{
  char *p1,*p2;
  char tmp;
  p1 = (char*)src;
  p2 = (char*)dest;
  tmp = p1[3]; /* pour pouvoir utiliser src == dest */
  p2[3] = p1[0];
  p2[0] = tmp;
  tmp = p1[2]; /* pour pouvoir utiliser src == dest */
  p2[2] = p1[1];
  p2[1] = tmp;
}
