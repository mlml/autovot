
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