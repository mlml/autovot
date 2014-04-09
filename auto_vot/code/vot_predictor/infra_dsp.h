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

#include <infra.h>

#define _max(x,y) ( (x)>(y) ? (x) : (y) )
#define _min(x,y) ( (x)<(y) ? (x) : (y) )


double read_samples_from_file(std::string filename, infra::vector &x, double virtual_sampling_rate);
infra::vector hamming(infra::vector x);
infra::vector powerspectrum(infra::vector x, int nfft);
infra::vector autocorrelation_function(infra::vector xin);
double autocorrelation_features(infra::vector xin);
infra::vector diff_means(const infra::vector x, int offset);
infra::vector cummulative_features(const infra::vector x, std::string type, int offset);
infra::vector rms_diff_means(const infra::matrix X, int offset);
infra::vector fir_filter(double b[], int nb, const infra::vector x);
void fast_pitch(infra::vector x, double window_size, double voicing_threshold, double silence_threshold, 
                double sampling_rate, infra::vector &f0, infra::vector &cost);
double zero_crossing(const infra::vector x);