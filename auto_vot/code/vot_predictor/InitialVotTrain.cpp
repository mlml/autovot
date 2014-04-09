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
 Purpose:  Train the system
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
	unsigned int num_epochs;
	int min_vot_length;
	int max_vot_length;
	int max_onset_time;
	double C;
	double loss_epsilon;
	double loss_ep_on;
	double loss_ep_off;
	string train_instances_filelist;
	string train_labels_filename;
	string val_instances_filelist;
	string val_labels_filename;
	string classifier_filename;
	string ignore_features_str;
	string training_method;
	string init_classifier;
	double epsilon = 0.0;
	bool vot_loss;
	bool pos_only;
	string kernel_expansion_name;
	double sigma;
	string verbose;
	
	learning::cmd_line cmdline;
	cmdline.info("Initial VOT detection - Passive Aggressive training");
	cmdline.add("-val_instances_filelist", "validation instances filelist", &val_instances_filelist, "");
	cmdline.add("-val_labels_filename", "validation labels filename", &val_labels_filename, "");
	cmdline.add("-epochs", "number of epochs [1]", &num_epochs, 1);
	cmdline.add("-min_vot_length", "min. vot duration in msec [10]", &min_vot_length, 10);
	cmdline.add("-max_vot_length", "max. vot duration in msec [200]", &max_vot_length, 200);
	cmdline.add("-max_onset", "max. time to onset in msec [150]", &max_onset_time, 150);
	cmdline.add("-C", "Trade-off between regularization and loss (running PA)", &C, 5.0);
	cmdline.add("-loss_eps", "epsilon parameter of the loss", &loss_epsilon, 1.0);
	cmdline.add("-ep_on", "epsilon parameter of the onset loss", &loss_ep_on, 10.0);
	cmdline.add("-ep_off", "epsilon parameter of the offset loss", &loss_ep_off, 1.0);
	cmdline.add("-ignore_features", "ignore the following features. E.g., \"3,7,19\".", &ignore_features_str, "");
	cmdline.add("-direct_loss", "use direct-loss update with the given epsilon [0.0]", &epsilon, 0.0);
	cmdline.add("-vot_loss", "use the VOT loss instead of alignment loss", &vot_loss, false);
	cmdline.add("-training_method", "PA, Pegasos, Perceptron, DirectLossMin", &training_method, "PA");
	cmdline.add("-pos_only", "Assume only positive VOTs", &pos_only, false);
	cmdline.add("-load_classifier", "load classifier for adaptation", &init_classifier, "");
  cmdline.add("-kernel_expansion", "use kernel expansion of type 'poly2' or 'rbf2'",
              &kernel_expansion_name, "");
  cmdline.add("-sigma", "if kernel is rbf2 or rbf3 this is the sigma", &sigma, 1.0);
	cmdline.add("-verbose", "log reporting level [ERROR, WARNING, INFO, or DEBUG]", &verbose, "INFO");
	cmdline.add_master_option("train_instances_filelist", &train_instances_filelist);
	cmdline.add_master_option("train_labels_filename", &train_labels_filename);
	cmdline.add_master_option("classifier_filename", &classifier_filename);
	int rc = cmdline.parse(argc, argv);
	if (rc < 3) {
		cmdline.print_help();
		return EXIT_FAILURE;
	}
	
	Log::ReportingLevel() = Log::FromString(verbose);
	Log::ExecutableName() = basename(argv[0]);
	
	// Initiate classifier
	Classifier classifier(min_vot_length, max_vot_length, max_onset_time, C,
												loss_epsilon, loss_ep_on, loss_ep_off, kernel_expansion_name, sigma);
	
	if (init_classifier != "") {
		classifier.load(init_classifier);
	}
	
	if (ignore_features_str != "") {
		LOG(INFO) << "Ignoring features " << ignore_features_str << ".";
		classifier.ignore_features(ignore_features_str);
	}
	
	if (training_method != "") {
		LOG(DEBUG) << "Training method is " << training_method;
	}
	
	double loss;
	double cum_loss = 0.0;
	double best_validation_loss = 1e100;
	
	int num_training_examples = 0;
	
	for (uint epoch = 0; epoch < num_epochs; epoch++) {
		
		// begining of the training set
		Dataset training_dataset(train_instances_filelist, train_labels_filename);
		
		double max_loss_in_epoch = 0.0; // maximal loss value in this epoch
		double avg_loss_in_epoch = 0.0; // training loss value in this epoch
		
		num_training_examples += training_dataset.size();
		
		// Run over all dataset
		for (uint i=0; i <  training_dataset.size(); i++) {
			
			SpeechUtterance x;
			VotLocation y;
			
			LOG(DEBUG) << "=========================================================================";
			
			// read next example for dataset
			training_dataset.read(x, y);
			
			if (training_method == "PA") {
				// predict for large-margin (epsilon=1.0)
				VotLocation y_hat_loss;
				classifier.predict_epsilon(x, y_hat_loss, y, -1.0, vot_loss, pos_only);
				loss = classifier.update(x, y, y_hat_loss, vot_loss);
			}
			//      else if (training_method == "Pegasos") {
			//        // predict for large-margin (epsilon=1.0)
			//        VotLocation y_hat_loss;
			//        classifier.predict_epsilon(x, y_hat_loss, y, 1.0, vot_loss);
			//        loss = classifier.update_pegasos(x, y, y_hat_loss);
			//      }
			//      else if (training_method == "DirectLossMin") {
			//        // predict for direct-loss if epsilon is given (not zero)
			//        VotLocation y_hat_eps;
			//        classifier.predict_epsilon(x, y_hat_eps, y, epsilon, vot_loss);
			//        VotLocation y_hat;
			//        classifier.predict(x, y_hat);
			//        loss = classifier.update_direct_loss(x, y_hat_eps, y_hat, y, epsilon);
			//      }
			else if (training_method == "Perceptron" || training_method == "") {
				VotLocation y_hat;
				classifier.predict(x, y_hat, pos_only);
				loss = classifier.update(x, y, y_hat, vot_loss);
			}
			else {
				LOG(ERROR) << "Unsupported training method";
				return EXIT_FAILURE;
			}
			
			cum_loss += loss;
			
			if (max_loss_in_epoch < loss) max_loss_in_epoch = loss;
			avg_loss_in_epoch += loss;
			
			// now, check the validations error
			if ( val_instances_filelist != "" && classifier.was_changed() ) {
				LOG(DEBUG) << "Validation...";
				Dataset val_dataset(val_instances_filelist, val_labels_filename);
				double this_w_loss = 0.0;
				for (uint ii=0; ii < val_dataset.size(); ++ii) {
					SpeechUtterance xx;
					VotLocation yy;
					VotLocation yy_hat;
					val_dataset.read(xx, yy);
					classifier.predict(xx, yy_hat, pos_only);
					int onset_loss = abs(yy.voice - yy_hat.voice);
					int offset_loss = abs(yy.burst - yy_hat.burst);
					this_w_loss += onset_loss+offset_loss;
				}
				this_w_loss /= val_dataset.size();
				if (this_w_loss <= best_validation_loss) {
					best_validation_loss = this_w_loss;
					classifier.save(classifier_filename);
				}
				LOG(DEBUG) << "i = " << i << ", this validation error = " << this_w_loss
				<< ", best validation loss  = " << best_validation_loss;
				
				// stopping criterion for iterate until convergence
				//        if (best_validation_loss < 1.0)
				//          break;
			}
			
		} // end running over the dataset
		
		avg_loss_in_epoch /=  training_dataset.size();
		
		LOG(DEBUG) << " average normalized loss = " << avg_loss_in_epoch
		<< " best validation loss  = " << best_validation_loss;
	}
	if (val_instances_filelist == ""){
		// make w the mean of the w_i, over all examples and epochs:
		classifier.w_star_mean(num_training_examples);
		classifier.save(classifier_filename);
	}
	else {
		LOG(DEBUG) << "Did not save the averaged classifier";
	}
	
	LOG(INFO) << "Training completed.";
	
	return EXIT_SUCCESS;
	
}

// ------------------------------- EOF -----------------------------//
