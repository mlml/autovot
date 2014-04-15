/************************************************************************
 Copyright (c) 2014 Joseph Keshet, Morgan Sonderegger, Thea Knowles

This file is part of Autovot, a package for automatic extraction of
voice onset time (VOT) from audio files.

Autovot is free software: you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

Autovot is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with Autovot.  If not, see
<http://www.gnu.org/licenses/>.
************************************************************************/

#ifndef _WAV_FILE_H_
#define _WAV_FILE_H_

#include <cstdio>
#include <string>

// types
#define UINT16 unsigned short
#define UINT32 unsigned int
#define UNIT64 unsigned long long

typedef struct {
  UINT16 wFormatTag;
  UINT16 wChannels;
  UINT32 dwSamplesPerSec;
  UINT32 dwAvgBytesPerSec;
  UINT16 wBlockAlign;
  UINT16 wBitsPerSample;
}
PCMWAVEFORMAT;

#ifndef SWAB16
#define SWAB16(A)  ((((UINT16)(A) & 0xff00) >> 8) |     \
									 (((UINT16)(A) & 0x00ff) << 8))
#endif

#ifndef SWAB32
#define SWAB32(A)  ((((UINT32)(A) & 0xff000000) >> 24) | \
(((UINT32)(A) & 0x00ff0000) >> 8)  | \
(((UINT32)(A) & 0x0000ff00) << 8)  | \
(((UINT32)(A) & 0x000000ff) << 24))
#endif

class CWavFile
{
public:
	CWavFile();
	~CWavFile();
	bool          Open(std::string filename);
	unsigned long ReadHeader(void);
	void          PrintHeader(void);
	unsigned long LoadSamples(short *sBuffer, unsigned long nBufferLength) ;
	unsigned long GetRate();
	unsigned long GetFrameCount();
	void          Close();

private:
	unsigned long Read(void *vBuffer, unsigned long nBufferLength);
	unsigned long Seek(unsigned long lOff, unsigned long nFrom);

public:
	PCMWAVEFORMAT m_PcmWaveFormat;
	UINT32        m_ulDataLength;

private:
	std::string   m_filename;
	FILE*         m_pfile;
};

#endif // _WAV_FILE_H_
