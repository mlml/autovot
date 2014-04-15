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

#include <stdlib.h>
#include <math.h>
#include <cfloat>
#include "infra_dsp.h"
#include "FFTReal/FFTReal.h"
//#include <audiofile.h>
# include "WavFile.h"
#include "fast_pitch_filters.h"

std::string prev_filename;
infra::vector prev_samples;
double prev_sampling_rate;

double read_samples_from_file(std::string filename, infra::vector &samples, double virtual_sampling_rate)
{
	if (filename == prev_filename) {
		// caching prev_filename
		samples.resize(prev_samples.size());
		samples = prev_samples;
		return prev_sampling_rate;
	}
	
	CWavFile wav_file;
	
	if (wav_file.Open(filename.c_str()) == false) {
		std::cerr << "Errror: Could not open file " << filename << " for reading." << std::endl;
		exit(-1);
	}
	
	// read header
	unsigned long num_samples_to_read = wav_file.ReadHeader();
	double sampling_rate = wav_file.GetRate();

	// read samples
	short *pbuffer = new short[num_samples_to_read];
	unsigned long num_samples_read = wav_file.LoadSamples(pbuffer, num_samples_to_read);
	samples.resize(num_samples_read);
	samples.zeros();
	for (int i=0; i < num_samples_read; i++)
		samples[i] = double(pbuffer[i]/32767.0);
	delete [] pbuffer;
	wav_file.Close();
	
	prev_filename = filename;
	prev_samples.resize(samples.size());
	prev_samples = samples;
	prev_sampling_rate = sampling_rate;
	
	return sampling_rate;
}

//double read_samples_from_file(std::string filename, infra::vector &x, double virtual_sampling_rate)
//{
//	AFfilehandle	file;
//	file = afOpenFile(filename.c_str(), "r", AF_NULL_FILESETUP);
//	if (file == AF_NULL_FILEHANDLE) {
//		std::cerr << "Could not open file " << filename << " for reading." << std::endl;
//		exit(-1);
//	}
//	double sampling_rate = afGetRate(file, AF_DEFAULT_TRACK);
//	/* Set virtual sample format to single-precision floating-point. */
//	// afSetVirtualSampleFormat(file, AF_DEFAULT_TRACK, AF_SAMPFMT_TWOSCOMP, 16); // load as shorts
//	//    short *buffer = new short[frameToRead];
//	afSetVirtualSampleFormat(file, AF_DEFAULT_TRACK, AF_SAMPFMT_FLOAT, 32); // load as floats
//	// should this work???? afSetVirtualRate(file, AF_DEFAULT_TRACK, virtual_sampling_rate); // load as floats
//	unsigned long samples_to_read = afGetFrameCount(file, AF_DEFAULT_TRACK);
//	float *buffer = new float[samples_to_read];
//	int samples_read = afReadFrames(file, AF_DEFAULT_TRACK, buffer, samples_to_read);
//	afCloseFile(file);
//	
//	x.resize(samples_read);
//	x.zeros();
//	for (int i=0; i < samples_read; i++)
//		x[i] = buffer[i];
//	
//	delete [] buffer;
//	
//	return sampling_rate;
//}

infra::vector hamming(infra::vector x)
{
  infra::vector y(x.size());
  y.zeros();
  
  infra::vector hamming_window(x.size());
  
  for (int i = 0; i < int(x.size()) / 2; i++)
    hamming_window[i] = 0.54f - 0.46f * (float)cos ((2*M_PI) * i / (x.size() - 1));
  for (int i = int(x.size() / 2); i < int(x.size()); i++)
    hamming_window[i] = hamming_window[x.size() - 1 - i];
  
  for (int i = 0; i < int(x.size()); i++)
    y[i] = x[i]*hamming_window[i];
  
  return y;
}



infra::vector powerspectrum(infra::vector xin, int nfft)
{
  const long nbr_points = nfft;			// Power of 2
  flt_t	* const	x = new flt_t [nbr_points];
  flt_t	* const	f = new flt_t [nbr_points];
  FFTReal	fft (nbr_points);					// FFT object initialized here
  for (int i = 0; i < int(xin.size()); ++ i)
    x[i] = xin(i);
  for (int i = int(xin.size()); i < nbr_points; ++ i)
    x[i] = 0.0;
  
  /* Compute FFT*/
  fft.do_fft (f, x);
  
  infra::vector s(nfft/2+1);
  s[0] = f[0]*f[0];
  for (int i = 1; i < nfft/2; i++) 
    s[i] = f[i]*f[i] + f[nfft/2+i]*f[nfft/2+i];
  s[nfft/2] = f[nfft/2]*f[nfft/2];
  
  delete [] x;
  delete [] f;
  
  return s;
}

infra::vector autocorrelation_function(infra::vector xin)
{
  // check what should be the size of the FFT
	int mantissa = 0;
	frexp(2*xin.size()-1,&mantissa);
	const long nbr_points = pow(2,mantissa);			// Power of 2
	
  flt_t	* const	x = new flt_t [nbr_points];
  flt_t	* const	f = new flt_t [nbr_points];
  flt_t	* const	F = new flt_t [nbr_points];
  flt_t	* const	c = new flt_t [nbr_points];
  FFTReal	fft (nbr_points);					// FFT object initialized here
  for (int i = 0; i < int(xin.size()); ++ i)
    x[i] = xin(i);
  for (int i = int(xin.size()); i < nbr_points; ++ i)
    x[i] = 0.0;
  
  /* Compute FFT*/
  fft.do_fft (f, x);
	F[0] = f[0]*f[0];
  for (int i = 1; i < nbr_points/2; i++) 
    F[i] = f[i]*f[i] + f[nbr_points/2+i]*f[nbr_points/2+i];
  F[nbr_points/2] = f[nbr_points/2]*f[nbr_points/2];
	// the following completion of the FFT is not needed to do_ifft function below
	//  for (int i = nbr_points/2+1; i < nbr_points; i++) 
	//    F[i] = F[nbr_points-i];
  
	// print F
	//	std::cout << "F= ";
	//	for (int i = 0; i <= nbr_points/2; i++) 
	//		std::cout << F[i] << " ";
	//	std::cout << std::endl << std::endl << std::endl;
	
	
	fft.do_ifft(F,c);
  fft.rescale(c);
  
	infra::vector acf(2*xin.size());
	acf.zeros();
	for (int i = 0; i < int(xin.size()); i++)
		acf[i] = c[xin.size()-1-i];
	for (int i = 0; i < int(xin.size()); i++)
		acf[i+xin.size()] = c[i+1];
	
  delete [] x;
  delete [] f;
	delete [] F;
	delete [] c;
	
	return acf;
}


double autocorrelation_features(infra::vector xin)
{
  // check what should be the size of the FFT
	int mantissa = 0;
	frexp(2*xin.size()-1,&mantissa);
	const long nbr_points = pow(2,mantissa);			// Power of 2
	
  flt_t	* const	x = new flt_t [nbr_points];
  flt_t	* const	f = new flt_t [nbr_points];
  flt_t	* const	F = new flt_t [nbr_points];
	
  FFTReal	fft (nbr_points);					// FFT object initialized here
  for (int i = 0; i < int(xin.size()); ++ i)
    x[i] = xin(i);
  for (int i = int(xin.size()); i < nbr_points; ++ i)
    x[i] = 0.0;
  
  /* Compute FFT*/
  fft.do_fft (f, x);
	F[0] = f[0]*f[0];
  for (int i = 1; i < nbr_points/2; i++) 
    F[i] = f[i]*f[i] + f[nbr_points/2+i]*f[nbr_points/2+i];
  F[nbr_points/2] = f[nbr_points/2]*f[nbr_points/2];
	// the following completion of the FFT is not needed to do_ifft function below
	//  for (int i = nbr_points/2+1; i < nbr_points; i++) 
	//    F[i] = F[nbr_points-i];
  
	// find maximum value
	double max_val = -100000000.0;
	for (int i = 0; i <= nbr_points/2; i++) 
		if (F[i] > max_val)
			max_val = F[i];
	
  delete [] x;
  delete [] f;
  delete [] F;
  
	return max_val;
}


infra::vector diff_means(const infra::vector x, int offset)
{
  infra::vector y(x.size());
  for (int i = 0; i < int(x.size()); i++) {
    int ind1 = _max(i-offset,0);
    int ind2 = _max(i-1,0);
    int ind3 = _min(i+1,int(x.size())-1);
    int ind4 = _min(i+offset, int(x.size())-1);
    double lMean = x.subvector(ind1, ind2-ind1+1).sum()/double(ind2-ind1+1);
    double rMean = x.subvector(ind3,ind4-ind3+1).sum()/double(ind4-ind3+1);
    y[i] = rMean - lMean;
    //std::cout << ind1 << " " << ind2 << " " << ind3 << " " << ind4 << " " << lMean << " " << rMean << " " << y[i] << std::endl;
  }
  //std::cout << "y=" << y << std::endl;
  return y;
}

infra::vector cummulative_features(const infra::vector x, std::string type, int offset)
{
  infra::vector y(x.size());
  y.zeros();
  
	for (int i = 0; i < int(x.size()); i++) {
		int len = _min( _max(i+1+offset,1), x.size()-1);
		if (type == "mean") 
			y[i] = x.subvector(0,len).sum()/double(len);
		else if (type == "max") 
			y[i] = x.subvector(0,len).max();
		else if (type == "min") 
			y[i] = x.subvector(0,len).min();
	}
	
  //std::cout << "y=" << y << std::endl;
  return y;
}

infra::vector rms_diff_means(const infra::matrix X, int offset)
{
  infra::vector y(X.width());
  y.zeros();
	
	for (int i=0; i < int(X.width()); i++) {
    
    int ind1 = _max(i-offset,0);
    int ind2 = _max(i-1,0);
    int ind3 = _min(i+1,int(X.width())-1);
    int ind4 = _min(i+offset, int(X.width())-1);
		
		double sum_diff = 0.0;
		for (int j=0; j < int(X.height()); j++) {
			double lMean = 0.0;
			for (int k=ind1; k <= ind2; k++) 
				lMean += X(j,k);
			lMean /= double(ind2-ind1+1);
			double rMean = 0.0;
			for (int k=ind3; k <= ind4; k++) 
				rMean += X(j,k);
			rMean /= double(ind4-ind3+1);
			
			sum_diff += (lMean-rMean)*(lMean-rMean);
		}
		y[i] = sqrt(sum_diff);
		
	}
  
	return y;
  
}


infra::vector fir_filter(double b[], int nb, const infra::vector x)
{
  infra::vector y(x.size());
  y.zeros();
  
  for (int n=0; n < int(x.size()); n++) {
    for (int i=0; i < _min(nb,n+1); i++) 
      y[n] += b[i]*x[n-i];
  }
  return y;
}



void fast_pitch(infra::vector x, double window_size, double voicing_threshold, double silence_threshold, 
                double sampling_rate, infra::vector &f0, infra::vector &cost)
{
  f0.resize(x.size());
  cost.resize(x.size());          
  
  infra::vector envA(x.size());
  envA = fir_filter(bl, bl_len, x);

  infra::matrix f0_candidates(nBands,x.size());
  infra::matrix cost_candidates(nBands,x.size());
  
  for (int band=0; band < nBands; band++) {
    infra::vector xx(x.size());
    xx = fir_filter(bf[band],bf_len,envA);

    // track pitch
    infra::vector mm(int(xx.size()));
    mm[0] = 2*xx[0];
    for (int i=0; i < int(xx.size())-2; i++)
      mm[i+1] = xx[i] + xx[i+2];
    mm[int(xx.size())-1] = 2*xx[int(xx.size())-1];
    
    infra::vector xm(int(xx.size()));
    xm[0] = xx[0]*mm[0];
    for (int i=1; i < int(xx.size()); i++) 
      xm[i] = xm[i-1] + (xx[i]*mm[i]);
    
    infra::vector m2(mm.size());
    m2[0] = mm[0]*mm[0];
    for (int i=1; i < int(xx.size()); i++) 
      m2[i] = m2[i-1] + (mm[i]*mm[i]);
    
    infra::vector x2(int(xx.size()));
    x2[0] = xx[0]*xx[0];
    for (int i=1; i < int(xx.size()); i++) 
      x2[i] = x2[i-1] + (xx[i]*xx[i]);
    
    int nn = int(window_size*sampling_rate);
		
		infra::vector xmu(xx.size());
		for (int i=0; i < nn; i++) 
			xmu[i] = xm[i];
    for (int i=nn; i < int(xx.size()); i++) 
      xmu[i] = xm[i] - xm[i-nn];
    
		infra::vector m2u(xx.size());
		for (int i=0; i < nn; i++) 
			m2u[i] = m2[i];
    for (int i=nn; i < int(xx.size()); i++) 
      m2u[i] = m2[i] - m2[i-nn];

		infra::vector x2u(xx.size());
		for (int i=0; i < nn; i++) 
			x2u[i] = x2[i];
    for (int i=nn; i < int(xx.size()); i++) 
      x2u[i] = x2[i] - x2[i-nn];

    infra::vector aa(int(xx.size()));
    for (int i=0; i < int(xx.size()); i++) {
      if (m2u[i] == 0)
        aa[i] = 0.5;
      else 
        aa[i] = xmu[i]/(m2u[i] + DBL_MIN);
      // BELOW MINIMUM FREQUENCY?
      if (fabsl(aa[i]) < 0.5/cos(2*M_PI*fMin[band]/sampling_rate))
        aa[i] = 0.5;
    }
    

    for (int i=0; i < int(xx.size()); i++) {
      // PITCH
      f0_candidates(band,i) = acos(0.5/aa[i]) * sampling_rate / (2.0*M_PI);
      f0_candidates(band,i) = _min(f0_candidates(band,i),sampling_rate-f0_candidates(band,i));

      double sp = sin(2*M_PI*f0_candidates(band,i)/sampling_rate);
      double cp = cos(2*M_PI*f0_candidates(band,i)/sampling_rate);
      
      // COST
      cost_candidates(band,i) = sqrt(fabsl(x2u[i]+aa[i]*aa[i]*m2u[i]-2*aa[i]*xmu[i])/fabsl(m2u[i]+DBL_MIN));
      cost_candidates(band,i) = cost_candidates(band,i)*(cp*cp*sampling_rate)/(M_PI*sp+DBL_MIN);
      cost_candidates(band,i) = cost_candidates(band,i)/(f0_candidates(band,i)+DBL_MIN);
      if (f0_candidates(band,i)==0 || m2u[i]==0 || x2u[i]==0)
				cost_candidates(band,i) = 1.0/DBL_MIN; // which is basically inf
    }
    
  }
  
  for (int i=0; i < int(x.size()); i++) {
    // SELECT BEST CANDIDATE FROM BANDS (IMPROVE BY VITERBI??)
    unsigned int best_f0_band = cost_candidates.column(i).argmin();
    f0[i] = f0_candidates(best_f0_band,i);
    cost[i] = cost_candidates(best_f0_band,i);
      
    // VOICED/UNVOICED DETERMINATION
    bool voiced = (cost[i] < voicing_threshold && x[i]*x[i] > silence_threshold);
    if (!voiced) f0[i] = 0;
  }
	
}



double zero_crossing(const infra::vector x)
{
  double zc = 0.0;
  
  for (int i = 0; i < int(x.size())-1; i++) {
    if (x[i]*x[i+1] < 0) zc++;
  }
  
  return zc;
}

