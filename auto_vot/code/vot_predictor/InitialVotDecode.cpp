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
 Project:  Initial VOT Detection
 Module:   Main entry point
 Purpose:  Predict the locationof initiat VOT (decoding)
 Date:     26 Feb., 2010
 Programmer: Joseph Keshet
 
 **************************** INCLUDE FILES *****************************/
#include <iostream>
#include <fstream>
#include <map>
#include <cmdline/cmd_line.h>
#include "Classifier.h"
#include "Dataset.h"
#include "Logger.h"

using namespace std;

static int loss_resolutions[] = {2,5,10,15,20,25,50};

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
	int min_vot_length;
	int max_vot_length;
	int max_onset_time;
	string instances_filelist;
	string labels_filename;
	string classifier_filename;
	string output_predictions_filename;
	string ignore_features_str;
	bool pos_only;
	string kernel_expansion_name;
	double sigma;
	string verbose;
	bool print_final_results;
	
	learning::cmd_line cmdline;
	cmdline.info("Initial VOT detection - Passive Aggressive decoding");
	cmdline.add("-min_vot_length", "min. phoneme duration in msec [15]", &min_vot_length, 15);
	cmdline.add("-max_vot_length", "max. phoneme duration in msec [200]", &max_vot_length, 200);
	cmdline.add("-max_onset", "min. phoneme duration in msec [150]", &max_onset_time, 150);
	cmdline.add("-output_predictions", "predictions and their confidences are written to the given file",
							&output_predictions_filename, "");
	cmdline.add("-ignore_features", "ignore the following features. E.g., \"3,7,19\".", &ignore_features_str, "");
	cmdline.add("-pos_only", "Assume only positive VOTs", &pos_only, false);
  cmdline.add("-kernel_expansion", "use kernel expansion of type 'poly2' or 'rbf2'", &kernel_expansion_name, "");
  cmdline.add("-sigma", "if kernel is rbf2 or rbf3 this is the sigma", &sigma, 1.0);
	cmdline.add("-verbose", "log reporting level [ERROR, WARNING, INFO, or DEBUG]", &verbose, "INFO");
	cmdline.add("-final_results", "print final results in INFO logging", &print_final_results, false);
	cmdline.add_master_option("instances_filelist", &instances_filelist);
	cmdline.add_master_option("labels_filename[can be `null` for no labels]", &labels_filename);
	cmdline.add_master_option("classifier_filename", &classifier_filename);
	int rc = cmdline.parse(argc, argv);
	if (rc < 3) {
		cmdline.print_help();
		return EXIT_FAILURE;
	}
	
	Log::ReportingLevel() = Log::FromString(verbose);
	Log::ExecutableName() = basename(argv[0]);

	// Initiate classifier
	Classifier classifier(min_vot_length, max_vot_length, max_onset_time, 0.0, 0.0, 0.0, 0.0, kernel_expansion_name, sigma);
	classifier.load(classifier_filename);
	if (ignore_features_str != "") {
		LOG(INFO) << "Ignoring features " << ignore_features_str << ".";
		classifier.ignore_features(ignore_features_str);
	}
	
	// begining of the training set
	Dataset test_dataset(instances_filelist, labels_filename);
	
	int num_boundaries = 0;
	int num_vots = 0;
	int num_pos = 0;
	int num_neg = 0;
	int neg_mislabeled_pos = 0;
	int pos_mislabeled_neg = 0;
	
	int cumulative_loss = 0;
	int *cum_loss_less_than = new int[sizeof(loss_resolutions)/sizeof(int)];
	
	int *cum_vot_loss_less_than = new int[sizeof(loss_resolutions)/sizeof(int)];
	int cumulative_vot_loss = 0;
	
	int *cum_corr_loss_less_than = new int[sizeof(loss_resolutions)/sizeof(int)];
	int cumulative_corr_loss = 0;
	
	for (uint j=0; j < sizeof(loss_resolutions)/sizeof(int); j++) {
		cum_loss_less_than[j] = 0;
		cum_vot_loss_less_than[j] = 0;
		cum_corr_loss_less_than[j] = 0;
	}
	
	double rms_onset_loss = 0;
	
	ofstream output_predictions_ofs;
	if (output_predictions_filename != "") {
		output_predictions_ofs.open(output_predictions_filename.c_str());
		if (!output_predictions_ofs.good()) {
			LOG(ERROR) << "Unable to open " << output_predictions_filename << "for writing.";
		}
	}
	
	// Run over all dataset
	for (uint i=0; i <  test_dataset.size(); i++) {
		
		SpeechUtterance x;
		VotLocation y;
		VotLocation y_hat;
		
		LOG(DEBUG) << "===========================================================";
		
		// read next example for dataset
		test_dataset.read(x, y);
		
		// predict label
		double confidence = classifier.predict(x, y_hat, pos_only);
		
		if (output_predictions_filename != "" && output_predictions_ofs.good())
			output_predictions_ofs << confidence << " " << y_hat.burst << " " << y_hat.voice << std::endl;
		
		// calculate the error
		if (test_dataset.labels_given()) {
			int onset_loss = abs(y.burst - y_hat.burst);
			for (uint j=0; j < sizeof(loss_resolutions)/sizeof(int); j++)
				if ( onset_loss <= loss_resolutions[j] ) cum_loss_less_than[j]++;
			
			
			rms_onset_loss += onset_loss*onset_loss;
			
			int offset_loss = abs(y.voice - y_hat.voice);
			for (uint j=0; j < sizeof(loss_resolutions)/sizeof(int); j++)
				if ( offset_loss <= loss_resolutions[j] ) cum_loss_less_than[j]++;
			
			LOG(DEBUG) << "burst onset err: " << y_hat.burst - y.burst;
			LOG(DEBUG) << "voice onset err: " << y_hat.voice - y.voice;
			
			int vot_loss = abs(y.voice-y.burst-(y_hat.voice-y_hat.burst));
			for (uint j=0; j < sizeof(loss_resolutions)/sizeof(int); j++)
				if ( vot_loss <= loss_resolutions[j] ) cum_vot_loss_less_than[j]++;
			
			cumulative_loss += onset_loss + offset_loss;
			num_boundaries += 2;
			
			cumulative_vot_loss += vot_loss;
			num_vots += 1;
			
			if(y.burst < y.voice) { //positive VOT example
				num_pos++;
				if(y_hat.burst < y_hat.voice) { //correctly predicited pos
					for (uint j=0; j < sizeof(loss_resolutions)/sizeof(int); j++)
						if ( vot_loss <= loss_resolutions[j] ) cum_corr_loss_less_than[j]++;
					cumulative_corr_loss += vot_loss;
				} else { //incorrectly predicted neg
					pos_mislabeled_neg++;
				}
			} else { //negative VOT example
				num_neg++;
				if(y_hat.burst > y_hat.voice) { //correctly predicted neg
					for (uint j=0; j < sizeof(loss_resolutions)/sizeof(int); j++)
						if ( vot_loss <= loss_resolutions[j] ) cum_corr_loss_less_than[j]++;
					cumulative_corr_loss += vot_loss;
				} else { //incorrectly predicted pos
					neg_mislabeled_pos++;
				}
			}
			
			// labeled and predicted VOT
			LOG(DEBUG) << "Labeled VOT: " << y.burst << " " << y.voice;
			LOG(DEBUG) << "Predicted VOT: " << y_hat.burst << " " << y_hat.voice << " conf: " << confidence;
			
			// boundaries t<= table
			LOG(DEBUG) << "Cum loss = " << cumulative_loss/double(num_boundaries);
			for (uint j=0; j < sizeof(loss_resolutions)/sizeof(int); j++) {
				LOG(DEBUG) << "% Boundaries (t <= " << loss_resolutions[j] << "ms) = "
				<< 100.0*cum_loss_less_than[j]/double(num_boundaries) ;
			}

			// VOT t<= table
			LOG(DEBUG) << "Cum VOT loss = " << cumulative_vot_loss/double(num_vots);
			for (uint j=0; j < sizeof(loss_resolutions)/sizeof(int); j++) {
				LOG(DEBUG) << "% VOT error (t <= " << loss_resolutions[j] << "ms) = "
				<< 100.0*cum_vot_loss_less_than[j]/double(num_vots);
			}

		}
	}
	
	// percent misclassified
	int num_misclassified = neg_mislabeled_pos + pos_mislabeled_neg;
	int num_corr = num_vots - num_misclassified;
	if (print_final_results) {
		LOG(INFO) << "Total num misclassified = " << double(num_misclassified)/double(num_vots);
		LOG(INFO) << "Num pos misclassified as neg = " << double(pos_mislabeled_neg)/double(num_pos);
		LOG(INFO) << "Num neg misclassified as pos = " << double(neg_mislabeled_pos)/double(num_neg);
		
		// VOT t<= table for correctly classified data only
		LOG(INFO) << "Cumulative VOT loss on correctly classified data = " << cumulative_corr_loss/double(num_corr);
		for (int j=0; j < sizeof(loss_resolutions)/sizeof(int); j++) {
			LOG(INFO) << "% corr VOT error (t <= " << loss_resolutions[j] << "ms) = "
			<< 100.0*cum_corr_loss_less_than[j]/double(num_corr);
		}
		
		rms_onset_loss /= double(num_vots);
		rms_onset_loss = sqrt(num_vots);
		
		LOG(INFO) << "RMS onset loss: " << rms_onset_loss;
	}
	else {
		LOG(DEBUG) << "Total num misclassified = " << double(num_misclassified)/double(num_vots);
		LOG(DEBUG) << "Num pos misclassified as neg = " << double(pos_mislabeled_neg)/double(num_pos);
		LOG(DEBUG) << "Num neg misclassified as pos = " << double(neg_mislabeled_pos)/double(num_neg);
		
		// VOT t<= table for correctly classified data only
		LOG(DEBUG) << "Cumulative VOT loss on correctly classified data = " << cumulative_corr_loss/double(num_corr);
		for (int j=0; j < sizeof(loss_resolutions)/sizeof(int); j++) {
			LOG(DEBUG) << "% corr VOT error (t <= " << loss_resolutions[j] << "ms) = "
			<< 100.0*cum_corr_loss_less_than[j]/double(num_corr);
		}
		
		rms_onset_loss /= double(num_vots);
		rms_onset_loss = sqrt(num_vots);
		
		LOG(DEBUG) << "RMS onset loss: " << rms_onset_loss;

	}
	if (output_predictions_filename != "" && output_predictions_ofs.good())
		output_predictions_ofs.close();
	
	delete [] cum_loss_less_than ;
	LOG(INFO) << "Decoding completed.";
	
	return EXIT_SUCCESS;
	
}

// ------------------------------- EOF -----------------------------//
