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
#include <cmdline/cmd_line.h>
#include "Logger.h"
#include "Dataset.h"
#include "infra_dsp.h"
#include "get_f0s.h"

#include "Timer.h"

using namespace std;

#define WIN_SIZE 0.005
#define FRAME_SIZE 0.001
#define FAST_PITCH_WIN_SIZE 0.025
#define RAPT_PITCH_WIN_DUR 0.0075
#define RAPT_PITCH_FRAME_STEP 0.01
#define ACORR_LEFT 100
#define ACORR_RIGHT 300
#define SAMPLING_RATE 16000
#define NUM_FEATURES 63
#define ZC_THRESHOLD 0.001
#define LOG_NATURAL(x) (((x) < 1.0E-20) ? -99.9900 : log(x))

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
	bool labels_given;
	bool dont_normalize;
	int limit_instances;
	string verbose;
	
	learning::cmd_line cmdline;
	cmdline.info("Front end for VOT detection");
	cmdline.add("-dont_normalize", "don't normalize features", &dont_normalize, false);
	cmdline.add("-limit_instances", "number of instances to extract", &limit_instances, -1);
	cmdline.add("-verbose", "log reporting level [ERROR, WARNING, INFO, or DEBUG]", &verbose, "INFO");
	cmdline.add_master_option("input_filelist", &input_filelist);
	cmdline.add_master_option("output_features_filelist", &output_features_filelist);
	cmdline.add_master_option("output_labels (\"null\" if no labels are given) ", &output_labels);
	int rc = cmdline.parse(argc, argv);
	if (rc < 2) {
		cmdline.print_help();
		return EXIT_FAILURE;
	}
	
	Log::ReportingLevel() = Log::FromString(verbose);
	Log::ExecutableName() = basename(argv[0]);
	
	labels_given = (output_labels != "null");
	
	// read input filelist
	NewInstances instances;
	instances.read(input_filelist, labels_given);
	
	// read output filelist
	StringVector output_features_filenames;
	output_features_filenames.read(output_features_filelist);
	
	if (output_features_filenames.size() != instances.size()) {
		LOG(ERROR) << "Number of input files does not match the number of output feature files";
		return EXIT_FAILURE;
	}
	
	std::ofstream ofs_y(output_labels.c_str());
	if (!ofs_y.good() && labels_given) {
		LOG(ERROR) << "Unable to open " << output_labels;
		return EXIT_FAILURE;
	}
	if (labels_given)
		ofs_y << instances.size() << " " << 2 << endl;
	
	// process each line
	if (limit_instances < 0)
		limit_instances = instances.size();
	LOG(INFO) << "Processing " << _min(limit_instances,instances.size()) << " instances.";
	for (unsigned int i=0; i < _min(limit_instances,instances.size()); i++) {
		
		LOG(DEBUG) << "Processing " << instances.file_list[i] << " (" << (i+1) << " of " << instances.size() << ")";

		// if (instances.word_end[i] - instances.word_start[i] < 0.6) {
		// 	LOG(ERROR) << "Window size is less than 600 msec (word_start=" << instances.word_start[i]
		// 	<< " word_end=" << instances.word_end[i] << ")";
		// 	return EXIT_FAILURE;
		// }

			// read samples
		infra::vector samples;
		
		double sampling_rate = read_samples_from_file(instances.file_list[i], samples, SAMPLING_RATE);
		
		if (instances.word_end[i] == -1) {
			instances.word_end[i] = (samples.size()-1)/double(sampling_rate);
		}

		// round the wav file duration up to 3 figures after the point
		double wav_duration = round(1000*samples.size()/double(sampling_rate))/1000.0;
		if (instances.word_end[i] > wav_duration) {
			LOG(ERROR) << "Word end (" << instances.word_end[i] << ") is greater than "
			<< "the length of " << instances.file_list[i];
			return EXIT_FAILURE;
		}
		
		if (labels_given) {
			int vot_offset = instances.vot_burst[i] > instances.vot_voice[i] ? instances.vot_burst[i] : instances.vot_voice[i];
			if (vot_offset >= samples.size()/double(sampling_rate)) {
				LOG(ERROR) << "Burst onset or voice onset: (" << vot_offset << ") is greater than the length of "
				<< instances.file_list[i];
				return EXIT_FAILURE;
			}
		}
		
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
		while (t[ind1] < instances.word_start[i])
			ind1++;
		
		// word-end frame
		int ind2 = t.size()-1;
		while (t[ind2] > instances.word_end[i])
			ind2--;
		
		int net_num_frames = ind2-ind1+1;
		
		// windowing
		infra::matrix alpha_windowed(alpha.height(), net_num_frames);
		alpha_windowed.zeros();
		for (int j=ind1; j <= ind2; j++)
			alpha_windowed.column(j-ind1) = hamming(alpha.column(j));
		
		// power spectrum
		infra::matrix alpha_powerspectrum(nfft/2+1, net_num_frames);
		for (int j=0; j < net_num_frames; j++)
			alpha_powerspectrum.column(j) = powerspectrum(alpha_windowed.column(j),nfft);
		
		// short-term energy
		infra::vector short_term_energy(net_num_frames);
		for (int j=0; j < net_num_frames; j++)
			short_term_energy[j] = alpha_windowed.column(j).norm2();
		
		// total energy
		infra::vector total_energy(net_num_frames);
		total_energy.zeros();
		for (int j=0; j < net_num_frames; j++)
			total_energy[j] = LOG_NATURAL(alpha_powerspectrum.column(j).sum());
		
		// low frequency E
		infra::vector low_energy(net_num_frames);
		low_energy.zeros();
		for (int j=0; j < net_num_frames; j++) {
			for (int k=0; k < nfft/2+1; k++) {
				if (f[k] > 50 && f[k] < 1000)
					low_energy[j] += alpha_powerspectrum(k,j);
			}
			low_energy[j]= LOG_NATURAL(low_energy[j]);
		}
		
		// high frequency E
		infra::vector high_energy(net_num_frames);
		high_energy.zeros();
		for (int j=0; j < net_num_frames; j++) {
			for (int k=0; k < nfft/2+1; k++) {
				if (f[k] > 3000)
					high_energy[j] += alpha_powerspectrum(k,j);
			}
			high_energy[j]= LOG_NATURAL(high_energy[j]);
		}
		
		// wiener entropy: sum(log(power_spectrum),1)-log(sum(power_spectrum,1));
		infra::vector wiener_entropy(net_num_frames);
		wiener_entropy.zeros();
		for (int j=0; j < net_num_frames; j++) {
			double acc1 = 0.0;
			double acc2 = 0.0;
			for (int k=0; k < int(alpha_powerspectrum.height()); k++) {
				acc1 += alpha_powerspectrum(k,j);
				acc2 += LOG_NATURAL(alpha_powerspectrum(k,j));
			}
			wiener_entropy[j] = acc2 - LOG_NATURAL(acc1);
		}
		
		
		// autocorrelation features
		infra::vector alpha_autocorrelation(net_num_frames);
		alpha_autocorrelation.zeros();
		
		for (int j=0; j < net_num_frames; j++) {
			// index of the current time
			int ind3 = ceil(t[j+ind1]*sampling_rate);
			
			// index of ACORR_LEFT samples before
			int ind4 = _max(ind3-ACORR_LEFT,2);
			// index of ACORR_RIGHTT samples after
			int ind5 = _min(ind3+ACORR_RIGHT, int(samples.size()));
			
			double temp;// = autocorrelation_features(samples.subvector(ind4,ind5-ind4+1));
			/////// debug purposes
			/////// debug purposes
			ind4 -= 2;
			ind5 -= 2;
			/////// debug purposes
			/////// debug purposes
			//std::cout << ind4 << " " << ind5 << " " << (ind5-ind4+1) << std::endl;
			temp = autocorrelation_features(samples.subvector(ind4,ind5-ind4+1));
			alpha_autocorrelation(j) = temp;
		}
		
		// extract pitch: Fei Sha & Lawrence Saul's algortihm
		infra::vector fast_pitch_detect(net_num_frames);
		fast_pitch_detect.zeros();
		infra::vector word_samples = samples.subvector(int(sampling_rate*instances.word_start[i]),
																									 int(sampling_rate*(instances.word_end[i]-instances.word_start[i])));
		infra::vector f0;
		infra::vector cost;
		//cout << word_samples.size() << endl;
		fast_pitch(word_samples, FAST_PITCH_WIN_SIZE, 0.2, 0.0, sampling_rate, f0, cost);
		
		//  no pitch above a thresh level
		for (int j=0; j < int(f0.size()); j++)
			if (f0[j] > 350) f0[j] = 0;
		
		// average pitch values to have the same frame lengths
		offset = -overlap;
		for (int j=0; j < net_num_frames; j++) {
			fast_pitch_detect[j] = 0.0;
			for (int k=0; k < frame_length; k++) {
				if (offset+k < int(f0.size()) && offset+k >= 0)
					fast_pitch_detect[j] += f0[offset+k];
			}
			fast_pitch_detect[j] /= double(frame_length);
			offset += frame_length-overlap;
		}
		
		// voicing track from fxrapt pitch detector (voicebox version)
		infra::vector rapt_voicing(net_num_frames);
		rapt_voicing.zeros();
		infra::vector rapt_f0;
		infra::vector rapt_vuv;
		infra::vector rapt_rms_speech;
		infra::vector rapt_acpkp;
		get_f0s_main(word_samples, rapt_f0, rapt_vuv, rapt_rms_speech, rapt_acpkp,
								 RAPT_PITCH_FRAME_STEP, RAPT_PITCH_WIN_DUR,sampling_rate);
		
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
		}
		
		offset = -overlap;
		for (int j=0; j < net_num_frames; j++) {
			rapt_voicing[j] = 0.0;
			for (int k=0; k < frame_length; k++) {
				if (offset+k < int(vuv_sample_based.size()) && offset+k >= 0)
					rapt_voicing[j] += vuv_sample_based[offset+k];
			}
			rapt_voicing[j] /= double(frame_length);
			offset += frame_length-overlap;
		}
		
		// autocorrelation features -- zero crossings
		infra::vector alpha_zc(net_num_frames);
		alpha_zc.zeros();
		for (int j=0; j < net_num_frames; j++)
			alpha_zc[j] = zero_crossing(alpha_windowed.column(j));
		
		
		// allocate feature matrix
		infra::matrix features(NUM_FEATURES, net_num_frames);
		features.zeros();
		
		// feats 1-9: energy, wiener entropy, autocor feature,
		// pitch, voicing, zero-crossings
		features.row(0) = short_term_energy;
		features.row(1) = total_energy;
		features.row(2) = low_energy;
		features.row(3) = high_energy;
		features.row(4) = wiener_entropy;
		features.row(5) = alpha_autocorrelation;
		features.row(6) = fast_pitch_detect;
		features.row(7) = rapt_voicing;
		features.row(8) = alpha_zc;
		
		// feats 10-30: 'local differences' using windows of 5, 10, 15 ms
		// for energy features, wiener entropy, autocor feature, pitch feature,
		// voicing feature, zero-crossing feature
		features.row(9) = diff_means(short_term_energy, 5);
		features.row(10) = diff_means(short_term_energy, 10);
		features.row(11) = diff_means(short_term_energy, 15);
		features.row(12) = diff_means(total_energy, 5);
		features.row(13) = diff_means(total_energy, 10);
		features.row(14) = diff_means(total_energy, 15);
		features.row(15) = diff_means(low_energy, 5);
		features.row(16) = diff_means(low_energy, 10);
		features.row(17) = diff_means(low_energy, 15);
		features.row(18) = diff_means(high_energy, 5);
		features.row(19) = diff_means(high_energy, 10);
		features.row(20) = diff_means(high_energy, 15);
		features.row(21) = diff_means(wiener_entropy, 5);
		features.row(22) = diff_means(wiener_entropy, 10);
		features.row(23) = diff_means(wiener_entropy, 15);
		features.row(24) = diff_means(alpha_autocorrelation, 5);
		features.row(25) = diff_means(alpha_autocorrelation, 10);
		features.row(26) = diff_means(alpha_autocorrelation, 15);
		features.row(27) = diff_means(fast_pitch_detect, 5);
		features.row(28) = diff_means(fast_pitch_detect, 10);
		features.row(29) = diff_means(fast_pitch_detect, 15);
		features.row(30) = diff_means(rapt_voicing, 5);
		features.row(31) = diff_means(rapt_voicing, 10);
		features.row(32) = diff_means(rapt_voicing, 15);
		features.row(33) = rms_diff_means(features.submatrix(8,0,1,features.width()), 5);
		features.row(34) = rms_diff_means(features.submatrix(8,0,1,features.width()), 10);
		features.row(35) = rms_diff_means(features.submatrix(8,0,1,features.width()), 15);
		
		// feats 37-40: mean/max of feature 11 (short-term energy local difference
		// with 15 ms window) up to time t
		features.row(36) = cummulative_features(features.row(1),"mean",0);
		features.row(37) = cummulative_features(features.row(1),"max",0);
		features.row(38) = cummulative_features(features.row(1),"mean",-5);
		features.row(39) = cummulative_features(features.row(1),"max",-5);
		
		// feats 41-48: similar to features 37-40, but for auto corr local difference
		// with windows of 5 and 10
		features.row(40) = cummulative_features(features.row(24),"mean",0);
		features.row(41) = cummulative_features(features.row(24),"max",0);
		features.row(42) = cummulative_features(features.row(24),"mean",-10);
		features.row(43) = cummulative_features(features.row(24),"max",-10);
		
		features.row(44) = cummulative_features(features.row(25),"mean",0);
		features.row(45) = cummulative_features(features.row(25),"max",0);
		features.row(46) = cummulative_features(features.row(25),"mean",-10);
		features.row(47) = cummulative_features(features.row(25),"max",-10);
		
		// feats 49-60 similar to feats 37-40, but for log high energy,
		// wiener entropy, pitch
		features.row(48) = cummulative_features(features.row(3),"mean",0);
		features.row(49) = cummulative_features(features.row(3),"max",0);
		features.row(50) = cummulative_features(features.row(3),"mean",-5);
		features.row(51) = cummulative_features(features.row(3),"max",-5);
		
		features.row(52) = cummulative_features(features.row(4),"mean",0);
		features.row(53) = cummulative_features(features.row(4),"max",0);
		features.row(54) = cummulative_features(features.row(4),"mean",-5);
		features.row(55) = cummulative_features(features.row(4),"max",-5);
		
		features.row(56) = cummulative_features(features.row(6),"mean",0);
		features.row(57) = cummulative_features(features.row(6),"max",0);
		features.row(58) = cummulative_features(features.row(6),"mean",-5);
		features.row(59) = cummulative_features(features.row(6),"max",-5);
		
		// feats 65-67 rms_diff_means of rapt_voicing using windows 5,10,15
		features.row(60) = rms_diff_means(features.submatrix(7,0,1,features.width()), 5);
		features.row(61) = rms_diff_means(features.submatrix(7,0,1,features.width()), 10);
		features.row(62) = rms_diff_means(features.submatrix(7,0,1,features.width()), 15);
		
		
		// z-score: all features *except pitch* - zsInds=[1:43 45:51];
		if (!dont_normalize) {
			for (int j=0; j < int(features.height()); j++) {
				if (j == 6) continue;
				double mean = features.row(j).sum()/double(features.width());
				double std = sqrt( features.row(j).norm2()/double(features.width()-1) -
													double(features.width())*mean*mean/double(features.width()-1) );
				if (std == 0) continue;
				features.row(j) -= mean;
				features.row(j) /= std;
			}
		}
		
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
		
		if (labels_given) {
			// onset frame
			int ind3 = 0;
			while (t[ind3] < instances.vot_burst[i])
				ind3++;
			
			// word-end frame
			int ind4 = t.size()-1;
			while (t[ind4] > instances.vot_voice[i])
				ind4--;
			
			ofs_y << ind3-ind1+1 << " " << ind4-ind1+1  << endl;
			
		}
		
	}
	
	if (ofs_y.good()) 
		ofs_y.close();
	
	LOG(INFO) << "Features extraction completed.";
	
	return EXIT_SUCCESS;
	
}

// ------------------------------- EOF -----------------------------//
