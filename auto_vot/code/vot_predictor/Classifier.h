

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

#ifndef _CLASSIFIER_H
#define _CLASSIFIER_H

/************************************************************************
 Project:  Initial VOT Detection
 Module:   Classifier
 Purpose:  Training and prediction algorithms
 Date:     26 Feb., 2010
 Programmer: Joseph Keshet
 
 *************************** INCLUDE FILES ******************************/
#include "infra.h"
#include "Dataset.h"
#include "KernelExpansion.h"

class Classifier
  {
  public:
    Classifier(int _min_vot_length, int _max_vot_length, 
	       int _max_onset_time, double _C, double _loss_epsilon, 
               double _loss_ep_on, double _loss_ep_off, std::string _kernel_name, double _sigma);
    ~Classifier();
    void load(std::string &filename);
    void save(std::string &filename);
    bool was_changed() { return (w_changed); }
		double update( SpeechUtterance& x, VotLocation y, VotLocation &y_hat, bool vot_loss) ;
    double update_direct_loss(SpeechUtterance& x, VotLocation &y_hat_eps, 
                              VotLocation &y_hat, VotLocation &y,
                              double epsilon);
    double predict(SpeechUtterance& x, VotLocation &y_hat, bool pos_only=false);
    double predict_epsilon(SpeechUtterance& x, VotLocation &y_hat,
                           VotLocation &y, double epsilon, bool vot_loss, bool pos_only=false);
    double mean_diff_feature_template(SpeechUtterance& x, int feature_index,
                                      int t, int shift, int window);
    infra::vector_view phi(SpeechUtterance& x, VotLocation& y);
    infra::vector_view phi_pos(SpeechUtterance& x, VotLocation& y);
    infra::vector_view phi_neg(SpeechUtterance& x, VotLocation& y);
    double loss(const VotLocation &y, const VotLocation &y_hat);
    double loss_vot(const VotLocation &y, const VotLocation &y_hat);
    int get_phi_size_pos() { return(phi_pos_size); }
    int get_phi_size_neg() { return(phi_neg_size); }
    void w_star_mean(int &N);
    void print_w() { std::cout << "w_pos=" << w_pos << " w_neg=" << w_neg << std::endl; }
    void ignore_features(std::string &ignore_features_str);
    
  protected:
    static int phi_pos_size;
    static int phi_neg_size;
    int phi_size;
    bool is_neg;
    int min_vot_length;
    int max_vot_length;
    int max_onset_time;
    double PA1_C;
    double loss_epsilon;
    double loss_ep_on;
    double loss_ep_off;
    double min_loss_update;
    infra::vector w_pos;
    infra::vector w_neg;
    infra::vector w_pos_sum;
    infra::vector w_neg_sum;
    bool w_changed;
    std::vector<int> features_ignored;
		KernelExpansion kernel;
		int kernel_phi_pos_size;
  };

#endif // _CLASSIFIER_H
