/************************************************************************
 Project:  VOT Detection front end
 Module:   Main entry point
 Purpose:  Front end features
 Date:     July 8, 2011
 Programmer: Joseph Keshet
 
 **************************** INCLUDE FILES *****************************/
#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
#include <cfloat>
#include <infra.h>
#include <cmd_line.h>
#include "Dataset.h"
#include "infra_dsp.h"
#include "get_f0s.h"

#include "Timer.h"

using namespace std;

#define WIN_SIZE 0.005
#define FRAME_SIZE 0.001
#define USE_FAST_PITCH
#define FAST_PITCH_WIN_SIZE 0.025
#define RAPT_PITCH_WIN_DUR 0.0075
#define RAPT_PITCH_FRAME_STEP 0.01
#define ACORR_LEFT 100
#define ACORR_RIGHT 300
#define SAMPLING_RATE 16000
#define NUM_FEATURES 51
#define ZC_THRESHOLD 0.001

//#define PRINT_TIMING

/************************************************************************
 Function:     main
 
 Description:  Main entry point
 Inputs:       int argc, char *argv[] - main input params
 Output:       int - EXIT_SUCCESS or EXIT_FAILURE
 Comments:     none.
 ***********************************************************************/
int main(int argc, char **argv) 
{
  // Parse command line
  string input_filelist;
  string output_features_filelist;
  string output_labels;
  
  learning::cmd_line cmdline;
  cmdline.info("Front end for VOT detection");
  cmdline.add_master_option("input_filelist", &input_filelist);
  cmdline.add_master_option("output_features_filelist", &output_features_filelist);
  cmdline.add_master_option("output_labels", &output_labels);
  int rc = cmdline.parse(argc, argv);
  if (rc < 3) {
    cmdline.print_help();
    return EXIT_FAILURE;
  }
  
  
  // read input filelist
  Instances instances;
  instances.read(input_filelist);
  
  // read output filelist
  StringVector output_features_filenames;
  output_features_filenames.read(output_features_filelist);
  
  if (output_features_filenames.size() != instances.size()) {
    cerr << "Error: number of input files does not dont match the number of output features files" << endl;
    return EXIT_FAILURE;
  }
  
  std::ofstream ofs_y(output_labels.c_str());
  ofs_y << instances.size() << " " << 2 << endl;
  
  // process each line
  for (unsigned int i=0; i < instances.size(); i++) {
    
    cout << "Processing " << (i+1) << " of " << instances.size() << endl;
    
    // read samples
    infra::vector samples;
    
#ifdef PRINT_TIMING
    Timer local_timer;
    local_timer.start();
#endif
    double sampling_rate = read_samples_from_file(instances.file_list[i], samples, SAMPLING_RATE);
    
    if (instances.word_end[i] >= int(samples.size())) {
      cerr << "Error: word end (" << instances.word_end[i] << ") is larger than " ;
      cerr << "the number of samples in " << instances.file_list[i] << endl;
      return EXIT_FAILURE;
    }
    
    if (instances.vot_offset[i] >= int(samples.size())) {
      cerr << "Error: voice onset (" << instances.vot_offset[i] << ") is larger than " ;
      cerr << "the number of samples in " << instances.file_list[i] << endl;
      return EXIT_FAILURE;
    }
#ifdef PRINT_TIMING
    local_timer.stop();
    local_timer.print("read_samples_from_file");
#endif
    
#ifdef PRINT_TIMING
    local_timer.start();
#endif
    // buffering
    int frame_length = sampling_rate*WIN_SIZE;
    int overlap = sampling_rate*WIN_SIZE-sampling_rate*FRAME_SIZE;
    int num_frames = floor((samples.size()-frame_length)/(frame_length-overlap))+1;
    infra::matrix alpha(frame_length,num_frames);
    int offset = 0;
    for (int j=0; j < num_frames; j++) {
      for (int k=0; k < frame_length; k++)
        alpha(k,j) = samples[offset+k];
      offset += frame_length-overlap;
    }
#ifdef PRINT_TIMING
    local_timer.stop();
    local_timer.print("buffering");
#endif
    
    // vector of frame times
    infra::vector t(num_frames);
    t[0]=WIN_SIZE/2;
    for (int j=0; j < num_frames-1; j++)
      t[j+1] = t[j] + FRAME_SIZE;
    
    // vector of frequencies
    int nfft = 256;
    infra::vector f(nfft/2+1);
    for (int j=0; j < nfft/2+1; j++)
      f[j] = sampling_rate/double(nfft)*j;
    
    // word-begin frame
    int ind1 = 0; 
    while (t[ind1] < double(instances.word_start[i])/sampling_rate)
      ind1++;
    
    // word-end frame
    int ind2 = t.size()-1;
    while (t[ind2] > double(instances.word_end[i])/sampling_rate)
      ind2--;
    
    int net_num_frames = ind2-ind1+1;
    
#ifdef PRINT_TIMING
    local_timer.start();
#endif
    // windowing 
    infra::matrix alpha_windowed(alpha.height(), net_num_frames);
    alpha_windowed.zeros();
    for (int j=ind1; j <= ind2; j++)
      alpha_windowed.column(j-ind1) = hamming(alpha.column(j));
#ifdef PRINT_TIMING
    local_timer.stop();
    local_timer.print("windowing");
#endif
    
#ifdef PRINT_TIMING
    local_timer.start();
#endif
    // power spectrum
    infra::matrix alpha_powerspectrum(nfft/2+1, net_num_frames);
    for (int j=0; j < net_num_frames; j++) 
      alpha_powerspectrum.column(j) = powerspectrum(alpha_windowed.column(j),nfft);
#ifdef PRINT_TIMING
    local_timer.stop();
    local_timer.print("powerspectrum");
#endif
    
#ifdef PRINT_TIMING
    local_timer.start();
#endif
    // short-term energy
    infra::vector short_term_energy(net_num_frames);
    for (int j=0; j < net_num_frames; j++) 
      short_term_energy[j] = alpha_windowed.column(j).norm2();
#ifdef PRINT_TIMING
    local_timer.stop();
    local_timer.print("short_term_energy");
#endif
    
#ifdef PRINT_TIMING
    local_timer.start();
#endif
    // total energy
    infra::vector total_energy(net_num_frames);
    total_energy.zeros();
    for (int j=0; j < net_num_frames; j++)
      total_energy[j] = log(alpha_powerspectrum.column(j).sum());
#ifdef PRINT_TIMING
    local_timer.stop();
    local_timer.print("total_energy");
#endif
    
#ifdef PRINT_TIMING
    local_timer.start();
#endif
    // low frequency E
    infra::vector low_energy(net_num_frames);
    low_energy.zeros();
    for (int j=0; j < net_num_frames; j++) {
      for (int k=0; k < nfft/2+1; k++) { 
        if (f[k] > 50 && f[k] < 1000) 
          low_energy[j] += alpha_powerspectrum(k,j);
      }
      low_energy[j]= log(low_energy[j]);
    }
#ifdef PRINT_TIMING
    local_timer.stop();
    local_timer.print("low_frequency_energy");
#endif
    
#ifdef PRINT_TIMING
    local_timer.start();
#endif
    // high frequency E
    infra::vector high_energy(net_num_frames);
    high_energy.zeros();
    for (int j=0; j < net_num_frames; j++) {
      for (int k=0; k < nfft/2+1; k++) { 
        if (f[k] > 3000) 
          high_energy[j] += alpha_powerspectrum(k,j);
      }
      high_energy[j]= log(high_energy[j]);
    }
#ifdef PRINT_TIMING
    local_timer.stop();
    local_timer.print("high_frequency_energy");
#endif
    
#ifdef PRINT_TIMING
    local_timer.start();
#endif
    // wiener entropy: sum(log(power_spectrum),1)-log(sum(power_spectrum,1));
    infra::vector wiener_entropy(net_num_frames);
    wiener_entropy.zeros();
    for (int j=0; j < net_num_frames; j++) {
      double acc1 = 0.0;
      double acc2 = 0.0;
      for (int k=0; k < int(alpha_powerspectrum.height()); k++) { 
        acc1 += alpha_powerspectrum(k,j);
        acc2 += log(alpha_powerspectrum(k,j));
      }
      wiener_entropy[j] = acc2 - log(acc1);
    }
#ifdef PRINT_TIMING
    local_timer.stop();
    local_timer.print("wiener_entropy");
#endif
    
#ifdef PRINT_TIMING
    local_timer.start();
#endif
    // autocorrelation features
    infra::vector alpha_autocorrelation(net_num_frames);
    alpha_autocorrelation.zeros();
    for (int j=0; j < net_num_frames; j++) {
      // index of the current time
      int ind3 = ceil(t[j+ind1]*sampling_rate);
      
      // index of ACORR_LEFT samples before 
      int ind4 = _max(ind3-ACORR_LEFT,1);
      // index of ACORR_RIGHTT samples after
      int ind5 = _min(ind3+ACORR_RIGHT, int(samples.size()));
      
      double temp;// = autocorrelation_features(samples.subvector(ind4,ind5-ind4+1));
      /////// debug purposes
      /////// debug purposes
      ind4 -= 2; 
      ind5 -= 2;
      /////// debug purposes
      /////// debug purposes
      
      temp = autocorrelation_features(samples.subvector(ind4,ind5-ind4+1));
      alpha_autocorrelation(j) = temp;
    }	
    //cout << "alpha_autocorrelation=" << alpha_autocorrelation << std::endl;
#ifdef PRINT_TIMING
    local_timer.stop();
    long lt = local_timer.print("autocorrelation");
    std::cout << "-->   net_num_frames=" << net_num_frames << "  rate=" << double(lt)/double(net_num_frames) << std::endl; 
#endif
    
    
#ifdef PRINT_TIMING
    local_timer.start();
#endif
    // allocate feature matrix
    infra::matrix features(NUM_FEATURES, net_num_frames);
    features.zeros();
#ifdef PRINT_TIMING
    local_timer.stop();
    local_timer.print("allocate_feature_matrix");
#endif
    
#ifdef PRINT_TIMING
    local_timer.start();
#endif
    // feats 1-18: 'local differences' using windows of 5, 10, 15 ms
    // for energy features, wiener entropy, st energy, autocor feature
    features.row(0) = diff_means(total_energy, 5);
    features.row(1) = diff_means(total_energy, 10);
    features.row(2) = diff_means(total_energy, 15);
    features.row(3) = diff_means(low_energy, 5);
    features.row(4) = diff_means(low_energy, 10);
    features.row(5) = diff_means(low_energy, 15);
    features.row(6) = diff_means(high_energy, 5);
    features.row(7) = diff_means(high_energy, 10);
    features.row(8) = diff_means(high_energy, 15);
    features.row(9) = diff_means(wiener_entropy, 5);
    features.row(10) = diff_means(wiener_entropy, 10);
    features.row(11) = diff_means(wiener_entropy, 15);
    features.row(12) = diff_means(short_term_energy, 5);
    features.row(13) = diff_means(short_term_energy, 10);
    features.row(14) = diff_means(short_term_energy, 15);
    features.row(15) = diff_means(alpha_autocorrelation, 5);
    features.row(16) = diff_means(alpha_autocorrelation, 10);
    features.row(17) = diff_means(alpha_autocorrelation, 15);
    
    // feats 19-24: energy, wiener entropy, ST energy, autocor feature
    features.row(18) = total_energy;
    features.row(19) = low_energy;
    features.row(20) = high_energy;
    features.row(21) = wiener_entropy;
    features.row(22) = short_term_energy;
    features.row(23) = alpha_autocorrelation;
    
    // feats 25-32: mean of feature 15 (short-term energy local difference 
    // with 15 ms window) up to time t
    features.row(24) = cummulative_features(features.row(14),"mean",0);
    features.row(25) = cummulative_features(features.row(14),"max",0);
    features.row(26) = cummulative_features(features.row(14),"mean",-5);
    features.row(27) = cummulative_features(features.row(14),"max",-5);
    
    features.row(28) = cummulative_features(features.row(15),"mean",0);
    features.row(29) = cummulative_features(features.row(15),"max",0);
    features.row(30) = cummulative_features(features.row(15),"mean",-5);
    features.row(31) = cummulative_features(features.row(15),"max",-5);
    
    // feats 33-40: similar to features 25-32, but for different functions/features..
    features.row(32) = cummulative_features(features.row(16),"mean",0);
    features.row(33) = cummulative_features(features.row(16),"max",0);
    features.row(34) = cummulative_features(features.row(16),"mean",-10);
    features.row(35) = cummulative_features(features.row(16),"max",-10);
    
    features.row(36) = cummulative_features(features.row(17),"mean",0);
    features.row(37) = cummulative_features(features.row(17),"max",0);
    features.row(38) = cummulative_features(features.row(17),"mean",-10);
    features.row(39) = cummulative_features(features.row(17),"max",-10);
#ifdef PRINT_TIMING
    local_timer.stop();
    local_timer.print("feature_assign");
#endif
    
#ifdef PRINT_TIMING
    local_timer.start();
#endif
    // feats 41-43: find the net difference of features 1-24 between (t,t+5) and (t,t-5).
    features.row(40) = rms_diff_means(features.submatrix(0,0,24,features.width()), 5);
    // do the same for (t,t+10) and (t,t-10)
    features.row(41) = rms_diff_means(features.submatrix(0,0,24,features.width()), 10);
    // do the same for (t,t+15) and (t,t-15)
    features.row(42) = rms_diff_means(features.submatrix(0,0,24,features.width()), 15);
    
    // extract pitch: Fei Sha & Lawrence Saul's algortihm
    infra::vector word_samples = samples.subvector(instances.word_start[i]-1,
                                                   instances.word_end[i]-instances.word_start[i]+1);
#ifdef PRINT_TIMING
    local_timer.stop();
    local_timer.print("rms_diff_means+samples.subvector");
#endif
    
#ifdef USE_FAST_PITCH
    infra::vector f0;
    infra::vector cost;
    fast_pitch(word_samples, FAST_PITCH_WIN_SIZE, 0.2, 0.0, sampling_rate, f0, cost);
    
    //  no pitch above a thresh level
    for (int j=0; j < int(f0.size()); j++)
      if (f0[j] > 350) f0[j] = 0;
    
    // average pitch values to have the same frame lengths
    offset = -overlap;
    for (int j=0; j < int(features.width()); j++) {
      features(43,j) = 0.0;
      for (int k=0; k < frame_length; k++) {
        if (offset+k < int(f0.size()) && offset+k >= 0)
          features(43,j) += f0[offset+k];
      }
      features(43,j) /= double(frame_length);
      offset += frame_length-overlap;
    }
    
    //cout << "f0(sha)=" << features.row(43) << endl;
    
    
    // feat 44-47: Local diffs of pitch track gotten by using sha/saul algorithm, 
    // then a little custom processing:don't allow pitch to be above a threshold level,
    // and get one pitch measurement per frame
    features.row(44) = rms_diff_means(features.submatrix(43,0,1,features.width()), 5);
    features.row(45) = rms_diff_means(features.submatrix(43,0,1,features.width()), 10);
    features.row(46) = rms_diff_means(features.submatrix(43,0,1,features.width()), 15);
#endif
    
    // autocorrelation features
    infra::vector alpha_zc(net_num_frames);
    alpha_zc.zeros();
    for (int j=0; j < net_num_frames; j++)
      alpha_zc[j] = zero_crossing(alpha_windowed.column(j));
    
    features.row(43) = alpha_zc;
    features.row(44) = rms_diff_means(features.submatrix(43,0,1,features.width()), 5);
    features.row(45) = rms_diff_means(features.submatrix(43,0,1,features.width()), 10);
    features.row(46) = rms_diff_means(features.submatrix(43,0,1,features.width()), 15);

    
#ifdef PRINT_TIMING
    local_timer.start();
#endif
    // feat 48-51: voicing track from fxrapt pitch detector (voicebox version)
    infra::vector rapt_f0;
    infra::vector rapt_vuv;
    infra::vector rapt_rms_speech;
    infra::vector rapt_acpkp;
    get_f0s_main(word_samples, rapt_f0, rapt_vuv, rapt_rms_speech, rapt_acpkp, 
                 RAPT_PITCH_FRAME_STEP, RAPT_PITCH_WIN_DUR,sampling_rate);
#ifdef PRINT_TIMING
    local_timer.stop();
    local_timer.print("get_f0s");
#endif
    
#ifdef PRINT_TIMING
    local_timer.start();
#endif
    // sub-sample the pitch and the voiced-unvoiced decisions
    infra::vector vuv_sample_based(word_samples.size());
    vuv_sample_based.zeros();
    unsigned int rapt_num_frames = rapt_f0.size();
    offset = RAPT_PITCH_FRAME_STEP*sampling_rate/2.0;
    for (int j=0; j < int(rapt_num_frames); j++) {
      int frame_begin = offset + j*RAPT_PITCH_FRAME_STEP*sampling_rate;
      int frame_end = frame_begin + RAPT_PITCH_FRAME_STEP*sampling_rate - 1;
      for (int k=frame_begin; k <= frame_end; k++)
        vuv_sample_based[k] = rapt_vuv[j];
      //cout << j << " " << frame_begin << " " << frame_end << " " << rapt_vuv[j] << endl;
    }
    
    offset = -overlap;
    for (int j=0; j < int(features.width()); j++) {
      features(47,j) = 0.0;
      for (int k=0; k < frame_length; k++) {
        if (offset+k < int(vuv_sample_based.size()) && offset+k >= 0)
          features(47,j) += vuv_sample_based[offset+k];
      }
      features(47,j) /= double(frame_length);
      offset += frame_length-overlap;
    }
#ifdef PRINT_TIMING
    local_timer.stop();
    local_timer.print("sub-sample_pitch+V/UV");
#endif
    
#ifdef PRINT_TIMING
    local_timer.start();
#endif
    features.row(48) = rms_diff_means(features.submatrix(47,0,1,features.width()), 5);
    features.row(49) = rms_diff_means(features.submatrix(47,0,1,features.width()), 10);
    features.row(50) = rms_diff_means(features.submatrix(47,0,1,features.width()), 15);
#ifdef PRINT_TIMING
    local_timer.stop();
    local_timer.print("rms_diff_means");
#endif
    
#ifdef PRINT_TIMING
    local_timer.start();
#endif
    // z-score: all features *except pitch* - zsInds=[1:43 45:51];
    for (int j=0; j < int(features.height()); j++) {
      if (j == 43) continue;
      double mean = features.row(j).sum()/double(features.width());
      double std = sqrt( features.row(j).norm2()/double(features.width()-1) - double(features.width())*mean*mean/double(features.width()-1) );
      if (std == 0) continue;
      features.row(j) -= mean;
      features.row(j) /= std;
    }
#ifdef PRINT_TIMING
    local_timer.stop();
    local_timer.print("z_score");
#endif
    
#ifdef PRINT_TIMING
    local_timer.start();
#endif
    // save features
    std::ofstream ofs_x(output_features_filenames[i].c_str());
    ofs_x << features.width() << " " << features.height() << endl;
    for (int j=0; j < int(features.width()); j++) {
      for (int k=0; k < int(features.height()); k++) 
        ofs_x << features(k,j) << " ";
      ofs_x << endl;
    }
    ofs_x.close();
    
    // save labels: VOT onset and offset
    // onset frame
    int ind3 = 0; 
    while (t[ind3] < double(instances.vot_onset[i])/sampling_rate)
      ind3++;
    
    // word-end frame
    int ind4 = t.size()-1;
    while (t[ind4] > double(instances.vot_offset[i])/sampling_rate)
      ind4--;
    
    ofs_y << ind3-ind1+1 << " " << ind4-ind1+1 << endl;
#ifdef PRINT_TIMING
    local_timer.stop();
    local_timer.print("save_features");
#endif
    
  }
  
  ofs_y.close();
  
  return EXIT_SUCCESS;
  
}

// ------------------------------- EOF -----------------------------//
