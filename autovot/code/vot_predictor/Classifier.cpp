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
 Module:   Classifier
 Purpose:  Training and prediction algorithms
 Date:     26 Feb., 2010
 Programmer: Joseph Keshet
 
 **************************** INCLUDE FILES *****************************/
#include <iostream>
#include "Classifier.h"
#include "Logger.h"

#define MISPAR_KATAN_MEOD (-1000000)
#define _min(a,b) ((a)>(b) ? (b) : (a))
#define _max(a,b) ((a)>(b) ? (a) : (b))

int Classifier::phi_pos_size = 77;
int Classifier::phi_neg_size = 59;



/************************************************************************
 Function:     Classifier::Classifier
 
 Description:  Constructor
 Inputs:       none.
 Output:       none.
 Comments:     none.
 ***********************************************************************/
Classifier::Classifier(int _min_vot_length, int _max_vot_length,
											 int _max_onset_time, double _C, double _loss_epsilon,
											 double _loss_ep_on, double _loss_ep_off, std::string _kernel_name, double _sigma) :
//Does this make phi_size so that it is positive if !_pos_or_neg and negative else
phi_size(phi_neg_size+phi_pos_size),
min_vot_length(_min_vot_length),
max_vot_length(_max_vot_length),
max_onset_time(_max_onset_time),
PA1_C(_C),
loss_epsilon(_loss_epsilon),
loss_ep_on(_loss_ep_on),
loss_ep_off(_loss_ep_off),
w_pos(phi_pos_size),
w_neg(phi_neg_size),
w_pos_sum(phi_pos_size),
w_neg_sum(phi_neg_size),
kernel(_kernel_name, phi_pos_size, _sigma)
{
	kernel_phi_pos_size = kernel.features_dim();
	LOG(DEBUG) << "size(w_pos)=" << w_pos.size();
	w_pos.resize(kernel_phi_pos_size);
	w_pos_sum.resize(kernel_phi_pos_size);
	phi_size = phi_neg_size + kernel_phi_pos_size;
	LOG(DEBUG) << "size(w_pos)=" << w_pos.size() ;
	w_pos.zeros();
	w_neg.zeros();
	w_pos_sum.zeros();
	w_neg_sum.zeros();
}

/************************************************************************
 Function:     Classifier::~Classifier
 
 Description:  Destructor
 Inputs:       none.
 Output:       none.
 Comments:     none.
 ***********************************************************************/
Classifier::~Classifier()
{
}

/************************************************************************
 Function:     Classifier::~Classifier
 
 Description:  Destructor
 Inputs:       none.
 Output:       none.
 Comments:     none.
 ***********************************************************************/
void Classifier::ignore_features(std::string &ignore_features_str)
{
	const std::string delimiters = ", ";
	// Skip delimiters at beginning.
	std::string::size_type last_pos = ignore_features_str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	std::string::size_type pos = ignore_features_str.find_first_of(delimiters, last_pos);
	
	while (std::string::npos != pos || std::string::npos != last_pos)
	{
		// Found a token, add it to the vector.
		std::string token = ignore_features_str.substr(last_pos, pos - last_pos);
		int feature_num = atoi(token.c_str());
		if (feature_num < 0 || feature_num >= Classifier::phi_size) {
			LOG(ERROR) << "Can only ignore features in the set of 0-" << (Classifier::phi_size-1) << ". "
			<< feature_num << " is not a valid feature number.";
			exit(-1);
		}
		else {
			features_ignored.push_back(feature_num);
		}
		// Skip delimiters.  Note the "not_of"
		last_pos = ignore_features_str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = ignore_features_str.find_first_of(delimiters, last_pos);
	}
	
}

/************************************************************************
 Function:     Classifier::update
 
 Description:  Train classifier with one example
 Inputs:       infra::vector& x - example instance
 int y - label
 Output:       double - squared loss
 Comments:     none.
 ***********************************************************************/
double Classifier::update( SpeechUtterance& x, VotLocation y, VotLocation &y_hat, bool vot_loss)
{
	double current_loss = 0.0;
	
	if (vot_loss)
		current_loss = loss_vot(y,y_hat);
	else
		current_loss = loss(y,y_hat);
	
	infra::vector_view phi_x_y = phi(x,y);
	infra::vector_view phi_x_y_hat = phi(x,y_hat);
	
	// std::cout << "phi_x_y=" << phi_x_y << std::endl;;
	// std::cout << "phi_x_y_hat=" << phi_x_y_hat << std::endl;
	
	infra::vector delta_phi(phi_size);
	delta_phi = phi_x_y - phi_x_y_hat;
	
	infra::vector w(phi_size);
	w.zeros();
	w.subvector(0, kernel_phi_pos_size) = w_pos;
	w.subvector(kernel_phi_pos_size, phi_neg_size) = w_neg;
	
	LOG(DEBUG) << "y=" << y << " y_hat=" << y_hat;
	LOG(DEBUG) << "w*phi(x,y)=" << w*phi_x_y << " w*phi(x,y_hat)=" << w*phi_x_y_hat
	<< " w*phi(x,y_hat)+loss(y,y_hat)=" << w*phi_x_y_hat + current_loss;
	LOG(DEBUG) << "gamma=" << current_loss;
	
	// changed 3/27/10 for consistency w/ matlab version -- MS
	// delta_phi /= 2.0;
	current_loss -= w*delta_phi;
	
	//std::cout << "phi(x,y)= " << phi_x_y << std::endl;
	//std::cout << "phi(x,y_hat)= " << phi_x_y_hat << std::endl;
	//  std::cout << "delta_phi = " << delta_phi << std::endl;
	LOG(DEBUG) << "hinge_loss=" << current_loss;
	LOG(DEBUG) << "delta_phi.norm2()="  << delta_phi.norm2();
	
	if (current_loss > 0.0)  {
		// update
		double tau = current_loss / delta_phi.norm2();
		if (tau > PA1_C) tau = PA1_C; // PA-I
		LOG(DEBUG) << "tau=" << tau;
		delta_phi *= tau;
		w += delta_phi;
		
		//  std::cout << "w = " << w << std::endl;
		w_changed = true;
	}
	else if (current_loss == 0.0) {
		LOG(DEBUG) << "No update. Hinge loss is zero. Wow!";
		w_changed = false;
	}
	else { // hing loss is less than zero, if we reach this point
		if (fabs(y.voice-y.burst) <= min_vot_length) {
			LOG(WARNING) << "Hinge loss is less than zero. This is due a short VOT in training data.";
		} else {
			LOG(ERROR) << "Hinge loss is less than zero. ";
			//exit(-1);
		}
		w_changed = false;
	}
	
	// split w to w_pos and w_neg
	w_pos = w.subvector(0, kernel_phi_pos_size);
	w_neg = w.subvector(kernel_phi_pos_size, phi_neg_size);
	
	// add this w_i to sum of all w
	w_pos_sum += w_pos;
	w_neg_sum += w_neg;
	
	return current_loss;
}


#if 0
/************************************************************************
 Function:     Classifier::update_direct_loss
 
 Description:  Train classifier with one example
 Inputs:       infra::vector& x - example instance
 int y - label
 Output:       double - squared loss
 Comments:     none.
 ***********************************************************************/
double Classifier::update_direct_loss(SpeechUtterance& x, VotLocation &y_hat_eps,
																			VotLocation &y_hat, VotLocation &y,
																			double epsilon)
{
	infra::vector delta_phi(phi_size);
	infra::vector_view phi_x_y_hat_eps = y_hat_eps.burst < y_hat_eps.voice ? phi_pos(x,y_hat_eps) : phi_neg(x,y_hat_eps);
	infra::vector_view phi_x_y_hat = y_hat.burst < y_hat.voice ? phi_pos(x,y_hat) : phi_neg(x,y_hat);
	
	if(y_hat_eps.label < 0) {
		phi_x_y_hat_eps.zeros();
	}
	if(y_hat.label < 0) {
		phi_x_y_hat.zeros();
	}
	delta_phi = phi_x_y_hat_eps - phi_x_y_hat;
	
	//std::cout << "phi(x,y_hat_eps)= " << phi_x_y_hat_eps << std::endl;
	//std::cout << "phi(x,y_hat)= " << phi_x_y_hat << std::endl;
	//std::cout << "delta_phi = " << delta_phi << std::endl;
	LOG(DEBUG) << "delta_phi.norm2() = "  << delta_phi.norm2();
	
	if (delta_phi.norm2() == 0) {
		w_changed = false;
		return 0;
	}
	
	// compute the PA update
	double loss_pa = sqrt( loss(y,y_hat) );
	loss_pa -= w*delta_phi;
	double tau_pa = 0.0;
	if (loss_pa > 0.0) {
		tau_pa = loss_pa / delta_phi.norm2();
		LOG(DEBUG) << "tau(PA) = " << tau_pa ;
	}
	
	// keep the loss and w before update
	infra::vector w_before_update(w);
	
	// start up values for update
	//VotLocation y_hat_eps_tmp(y_hat_eps);
	VotLocation y_hat_tmp(y_hat);
	double tau = tau_pa;
	double loss_before_update = loss(y,y_hat);
	double loss_after_update;
	
	if (loss_before_update == 0) {
		w = w_before_update;
		w_changed = false;
		return 0;
	}
	
	// update
	do {
		w = w_before_update;
		infra::vector delta_phi_temp(delta_phi);
		delta_phi_temp *= tau;
		w += delta_phi_temp;
		// check loss after update
		predict(x, y_hat_tmp);
		///predict_epsilon(x, y_hat_eps_tmp, y, epsilon);
		loss_after_update = loss(y,y_hat_tmp);
		LOG(DEBUG) << "tau(DA)=" << tau << " loss_before=" << loss_before_update << " loss_after=" << loss_after_update ;
		tau /= 2;
	} while (loss_after_update >= loss_before_update && tau > 0.05) ;
	
	
	if (loss_after_update >= loss_before_update) {
		w = w_before_update;
		w_changed = false;
	}
	else {
		// keep old w
		w_old = w_before_update;
		LOG(DEBUG) << "w = " << w ;
		w_changed = true;
	}
	
	return loss_after_update;
}
#endif

/************************************************************************
 Function:     mean_diff_feature_template
 
 Description:  feature helper function. computes the difference between
 the means before and after t. the mean is span over "window" samples.
 Inputs:       SpeechUtterance &x
 int feature_index,
 int t,
 int shift,
 int window
 Output:       double
 Comments:     none.
 ***********************************************************************/
double Classifier::mean_diff_feature_template(SpeechUtterance& x,
																							int feature_index,
																							int t,
																							int shift,
																							int window)
{
	double mean_1 = 0.0;
	double mean_2 = 0.0;
	
	for (int i = _max(0,t + shift - window); i <= _min(int(x.size()-1), t + shift); i++)
		mean_1 += x.scores(i,feature_index);
	mean_1 /= double(t-_max(0,t + shift - window)+1);
	
	for (int i = _max(0, t + shift) ; i <= _min(int(x.size()-1),t + shift + window); i++)
		mean_2 += x.scores(i,feature_index);
	mean_2 /= double(_min(int(x.size()-1), t + shift + window)-t+1);
	
	return (mean_1 - mean_2);
	
}

/************************************************************************
 Function:     phi
 
 Description:  calculate phi of x for both positive and negative update
 Inputs:       SpeechUtterance &x
 VotLocation &y
 Output:       infra::vector_view
 Comments:     Verify that all necessary features are included in VotFrontEnd
 and check that the corresponding numbers are correct, since
 this was modified for the negative case
 ***********************************************************************/
infra::vector_view Classifier::phi(SpeechUtterance& x, VotLocation& y)
{
	infra::vector v(phi_size);
	v.zeros();
	
	v.subvector(0, kernel_phi_pos_size) = phi_pos(x,y);
	v.subvector(kernel_phi_pos_size, phi_neg_size) = phi_neg(x,y);
	
	//	std::cout << "v_pos=" << phi_pos(x,y) << std::endl;
	//	std::cout << "v_neg=" << phi_neg(x,y) << std::endl;
	//	std::cout << "v=" << v << std::endl;;
	
	return v;
}

/************************************************************************
 Function:     phi_pos
 
 Description:  calculate phi of x for positive update
 Inputs:       SpeechUtterance &x
 VotLocation &y
 Output:       infra::vector_view
 Comments:     Verify that all necessary features are included in VotFrontEnd
 and check that the corresponding numbers are correct, since
 this was modified for the negative case
 ***********************************************************************/
infra::vector_view Classifier::phi_pos(SpeechUtterance& x, VotLocation& y)
{
	infra::vector v(phi_pos_size);
	v.zeros();
	
	if (y.is_neg()) return v;
	
	// burstInds=[1 3 4 12:26];
	// total_energy, high_energy, wiener_entropy
	// diff_means(total_energy, low_energy, high_energy, wiener_entropy,
	//            alpha_autocorrelation) using windows of 5,10,15
	int burst_indices[] = {1,3,4,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26};
	// this is 18: std::cout << "sizeof(burst_indices)= " << sizeof(burst_indices)/double(sizeof(int)) << std::endl;
	
	// % feats 19-38, 39-44
	// voiceInds=[1:12 16:18 19:22 24 45:47 49:51];
	// total_energy, low_energy, high_energy, wiener_entropy, alpha_autocorrelation
	// diff_means(total_energy, low_energy, high_energy, wiener_entropy,
	//            alpha_autocorrelation) using windows of 5,10,15
	// rms_diff_means(alpha_zc, rapt_voicing) using windows of 5,10,15
	int voice_indices[] = {1,2,3,4,5,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,33,34,35,60,61,62};
	// thsi is 26: std::cout << "sizeof(voice_indices)= " << sizeof(voice_indices)/double(sizeof(int)) << std::endl;
	
	int v_i = 0;

	// v_i of this group is from 0 to 17
	// v=[x(burstInds,on); x(voiceInds,off)];
	for (int i = 0; i < int(sizeof(burst_indices)/sizeof(int)); i++) {
		v[v_i] = x.scores(y.burst,burst_indices[i]);
		v_i++;
	}
	
	// v_i of this group is from 18 to 43
	for (int i = 0; i < int(sizeof(voice_indices)/sizeof(int)); i++) {
		v[v_i] = x.scores(y.voice,voice_indices[i]);
		v_i++;
	}
	
	// v_i is 44
	// v(end+1)=mean(x(25,on:off));
	for (int i = y.burst; i <= y.voice; i++)
		v[v_i] += x.scores(i,24);
	v[v_i] /= double(y.voice-y.burst+1);
	v_i++;
	
	
	// v_i is 45
	// v(end+1)=max(x(25,on:off));
	// initialize to -inf in case max is negative
	v[v_i] = double(MISPAR_KATAN_MEOD);
	for (int i = y.burst; i <= y.voice; i++)
		v[v_i] = _max(x.scores(i,24), v[v_i]);
	v_i++;
	
	int why_10 = 10;

	// v_i is 46
	// v(end+1)=mean(x(25,on:max(off-10,on)));
	int max_i = _max(y.burst,y.voice-why_10);
	for (int i = y.burst; i <= max_i; i++)
		v[v_i] += x.scores(i,24);
	v[v_i] /= double(max_i-y.burst+1);
	v_i++;

	// v_i is 47
	// v(end+1)=max(x(25,on:max(off-10,on)));
	// initialize to -inf in case max is negative
	v[v_i] = double(MISPAR_KATAN_MEOD);
	for (int i = y.burst; i <= max_i; i++)
		v[v_i] = _max(x.scores(i,24), v[v_i]);
	v_i++;

	// v_i is 48
	// v(end+1)=mean(x(26,on:off));
	for (int i = y.burst; i <= y.voice; i++)
		v[v_i] += x.scores(i,25);
	v[v_i] /= double(y.voice-y.burst+1);
	v_i++;

	// v_i is 49
	// v(end+1)=max(x(26,on:off));
	// initialize to -inf in case max is negative
	v[v_i] = double(MISPAR_KATAN_MEOD);
	for (int i = y.burst; i <= y.voice; i++)
		v[v_i] = _max(x.scores(i,25), v[v_i]);
	v_i++;

	// v_i is 50
	// v(end+1)=mean(x(26,on:max(off-10,on)));
	for (int i = y.burst; i <= max_i; i++)
		v[v_i] += x.scores(i,25);
	v[v_i] /= double(max_i-y.burst+1);
	v_i++;
	
	// v_i is 51
	// vv(end+1)=max(x(26,on:max(off-10,on)));
	// initialize to -inf in case max is negative
	v[v_i] = double(MISPAR_KATAN_MEOD);
	for (int i = y.burst; i <= max_i; i++)
		v[v_i] = _max(x.scores(i,25), v[v_i]);
	v_i++;
	
	// mean(x(4,on:off))
	double mean_3_vot = 0.0;
	for (int i = y.burst; i <= y.voice; i++)
		mean_3_vot += x.scores(i,3);
	mean_3_vot /= double(y.voice-y.burst+1);
	
	// mean(x(4,1:on)
	double mean_3_pre_vot = 0.0;
	for (int i = 0; i <= y.burst; i++)
		mean_3_pre_vot += x.scores(i,3);
	mean_3_pre_vot /= double(y.burst+1);
	
	// mean(x(5,on:off))
	double mean_4_vot = 0.0;
	for (int i = y.burst; i <= y.voice; i++)
		mean_4_vot += x.scores(i,4);
	mean_4_vot /= double(y.voice-y.burst+1);
	
	// mean(x(5,1:on)
	double mean_4_pre_vot = 0.0;
	for (int i = 0; i <= y.burst; i++)
		mean_4_pre_vot += x.scores(i,4);
	mean_4_pre_vot /= double(y.burst+1);
	
	// v_i is 52
	// % mean diff
	// v=[v; mean(x(4,on:off))-mean(x(4,1:on))];
	v[v_i] = mean_3_vot - mean_3_pre_vot;
	v_i++;
	
	// v_i is 53
	// v=[v; mean(x(5,on:off))-mean(x(5,1:on))];
	v[v_i] = mean_4_vot - mean_4_pre_vot;
	v_i++;
	
	// % max of high, WE from 1:burst-5
	// v=[v; max(x(4,1:max(on-5,1)))];
	
	// v_i is 54
	// initialize to -inf in case max is negative
	v[v_i] = double(MISPAR_KATAN_MEOD);
	max_i = _max(1,y.burst-5);
	for (int i = 0; i <= max_i; i++)
		v[v_i] = _max(x.scores(i,3), v[v_i]);
	v_i++;
	
	// v=[v; max(x(22,1:max(on-5,1)))];
	
	// v_i is 55
	// initialize to -inf in case max is negative
	v[v_i] = double(MISPAR_KATAN_MEOD);
	for (int i = 0; i <= max_i; i++)
		v[v_i] = _max(x.scores(i,4), v[v_i]);
	v_i++;
	
	// v_i is 56
	// % mean of high, WE from 1:burst-5
	// v=[v; mean(x(4,1:max(on-5,1)))];
	for (int i = 0; i <= max_i; i++)
		v[v_i] += x.scores(i,3);
	v[v_i] /= double(max_i+1);
	v_i++;
	
	// v_i is 57
	// v=[v; mean(x(5,1:max(on-5,1)))];
	for (int i = 0; i <= max_i; i++)
		v[v_i] += x.scores(i,4);
	v[v_i] /= double(max_i+1);
	v_i++;
	
	// v_i is 58
	// v=[v; mean(x(8,1:max(off-5,1)))];
	max_i = _max(1,y.voice-5);
	for (int i = 0; i <= max_i; i++)
		v[v_i] += x.scores(i,7);
	v[v_i] /= double(max_i+1);
	
#if 0
	v_i++;
	for (int window = 2; window <= 8; window++) {
		for (int shift = 0; shift <= 0; shift++) {
			v[v_i] = mean_diff_feature_template(x, 24, y.burst, shift, window);
			// std::cout << window << " " << shift << " " << v[v_i] << std::endl;
			v_i++;
			v[v_i] = mean_diff_feature_template(x, 24, y.voice, shift, window);
			// std::cout << window << " " << shift << " " << v[v_i] << std::endl;
			v_i++;
			v[v_i] = mean_diff_feature_template(x, 25, y.burst, shift, window);
			// std::cout << window << " " << shift << " " << v[v_i] << std::endl;
			v_i++;
			v[v_i] = mean_diff_feature_template(x, 25, y.voice, shift, window);
			// std::cout << window << " " << shift << " " << v[v_i] << std::endl;
			v_i++;
			v[v_i] = mean_diff_feature_template(x, 3, y.burst, shift, window);
			// std::cout << window << " " << shift << " " << v[v_i] << std::endl;
			v_i++;
			v[v_i] = mean_diff_feature_template(x, 3, y.voice, shift, window);
			// std::cout << window << " " << shift << " " << v[v_i] << std::endl;
			v_i++;
			v[v_i] = mean_diff_feature_template(x, 4, y.burst, shift, window);
			// std::cout << window << " " << shift << " " << v[v_i] << std::endl;
			v_i++;
			v[v_i] = mean_diff_feature_template(x, 4, y.voice, shift, window);
			// std::cout << window << " " << shift << " " << v[v_i] << std::endl;
			v_i++;
			v[v_i] = mean_diff_feature_template(x, 7, y.burst, shift, window);
			// std::cout << window << " " << shift << " " << v[v_i] << std::endl;
			v_i++;
			v[v_i] = mean_diff_feature_template(x, 7, y.voice, shift, window);
			// std::cout << window << " " << shift << " " << v[v_i] << std::endl;
			v_i++;
		}
	}
#endif
	
	// zero ignored features
	for (uint i = 0; i < features_ignored.size(); i++)
		v[ features_ignored[i] ] = 0.0;
	
	
	infra::vector v2(kernel_phi_pos_size);
	v2 = kernel.expand(v);

	return v2;
}

/************************************************************************
 Function:     phi_neg
 
 Description:  calculate phi of x for negative update
 Inputs:       SpeechUtterance &x
 VotLocation &y
 Output:       infra::vector_view
 Comments:     none.
 ***********************************************************************/
infra::vector_view Classifier::phi_neg(SpeechUtterance& x, VotLocation& y)
{
	infra::vector v(phi_neg_size);
	v.zeros();
	
	if (y.is_pos()) return v;
	
	// indices of features calculated in NegVotFrontEnd.cpp to be evaluated at voice (t_v)
	// energy and wiener entropy terms
	// diff_means(total_energy, 5/10/15)
	// diff_means(low_energy, 5/10/15)
	// diff_means(high_energy, 5/10/15)
	// diff_means(wiener_entropy, 5/10/15)
	int voice_indices[] = {1,2,3,4,7,12,13,14,18,19,20,21,22,23}; //13 features
	
	// indices of features calculated in NegVotFrontEnd.cpp to be evaluated at burst (t_b)
	// energy, wiener entropy, rapt_voicing
	// diff_means(total_energy, 5/10/15)
	// diff_means(low_energy, 5/10/15)
	// diff_means(high_energy, 5/10/15)
	// diff_means(wiener_entropy, 5/10/15)
	int burst_indices[] = {1,2,3,4,7,12,13,14,15,16,17,18,19,20,21,22,23}; //13 features
	
	int v_i = 0;
	
	// v=[x(voiceInds,on); x(burstInds,off)];
	for (int i = 0; i < int(sizeof(voice_indices)/sizeof(int)); i++) {
		v[v_i] = x.scores(y.voice,voice_indices[i]);
		v_i++;
	}
	for (int i = 0; i < int(sizeof(burst_indices)/sizeof(int)); i++) {
		v[v_i] = x.scores(y.burst,burst_indices[i]);
		v_i++;
	}
	
	// average voicing voice to voice+10
	for (int i = y.voice; i <= y.voice+10; i++)
		v[v_i] += x.scores(i,7);
	v[v_i] /= 10.0;
	v_i++;
	
	// average voicing voice to burst
	for (int i = y.voice; i <= y.burst; i++)
		v[v_i] += x.scores(i,7);
	v[v_i] /= y.burst - y.voice;
	v_i++;
	
	// average voicing voice-15 to voice
	for (int i = _max(y.voice-15,1); i <= y.voice; i++)
		v[v_i] += x.scores(i,7);
	v[v_i] /= 15.0;
	v_i++;
	
	
	
	// max voicing in intervals before voice
	double max_voix_1 = double(MISPAR_KATAN_MEOD);
	for (int i = _max(y.voice,1); i <= y.voice+15; i++)
		max_voix_1 = _max(x.scores(i,1), max_voix_1);
	v[v_i] = max_voix_1;
	v_i++;
		
	// mean(x(total_energy,on-50:on-5))
	double mean_1_pre_vot = 0.0;
	double cur_size = 0.0;
	for (int i = _max(y.voice-50, 1); i <= _max(y.voice-5,2); i++) {
		mean_1_pre_vot += x.scores(i,1);
		cur_size++;
	}
	mean_1_pre_vot /= cur_size;
	
	// mean(x(total_energy,on:off-10))
	double mean_1_vot = 0.0;
	for (int i = y.voice; i <= y.burst-10; i++)
		mean_1_vot += x.scores(i,1);
	mean_1_vot /= double((y.burst-10)-y.voice+1);
	v[v_i] = mean_1_vot;
	v_i++;
	
	// mean(x(total_energy,off:off+50))
	double mean_1_post_vot = 0.0;
	cur_size = 0.0;
	for (int i = y.burst; i <= _min(y.burst + 50, x.size()-1); i++) {
		mean_1_post_vot += x.scores(i,1);
		cur_size++;
	}
	mean_1_post_vot /= cur_size;
	// v[v_i] = mean_1_post_vot;
	// v_i++;
	
	// mean diff
	// v=[v; mean(x(total_energy,on:off-10)) - mean(x(total_energy,on-5:on))];
	v[v_i] = mean_1_vot - mean_1_pre_vot;
	v_i++;
	
	// mean diff
	// v=[v; mean(x(total_energy,off:off+10)) - mean(x(total_energy,on:off-10))];
	v[v_i] = mean_1_post_vot - mean_1_vot;
	v_i++;
	
	// v[v_i] = mean_1_post_vot - mean_1_pre_vot;
	// v_i++;
	
	// mean energy for diff means of low_energy, high_energy, wiener entropy
	// windows 5 and 10
	for (int i = y.voice; i <= y.burst; i++)
		v[v_i] += x.scores(i,15);
	v[v_i] /= double(y.burst-y.voice+1);
	v_i++;
	
	for (int i = y.voice; i <= y.burst; i++)
		v[v_i] += x.scores(i,16);
	v[v_i] /= double(y.burst-y.voice+1);
	v_i++;
	
	
	for (int i = y.voice; i <= y.burst; i++)
		v[v_i] += x.scores(i,18);
	v[v_i] /= double(y.burst-y.voice+1);
	v_i++;
	
	for (int i = y.voice; i <= y.burst; i++)
		v[v_i] += x.scores(i,19);
	v[v_i] /= double(y.burst-y.voice+1);
	v_i++;
	
	
	for (int i = y.voice; i <= y.burst; i++)
		v[v_i] += x.scores(i,21);
	v[v_i] /= double(y.burst-y.voice+1);
	v_i++;
	
	for (int i = y.voice; i <= y.burst; i++)
		v[v_i] += x.scores(i,22);
	v[v_i] /= double(y.burst-y.voice+1);
	v_i++;
	
	// max(x(high_energy,on:off))
	// initialize to -inf in case max is negative
	double max_3_vot = double(MISPAR_KATAN_MEOD);
	for (int i = y.voice; i <= _max(y.voice,y.burst-5); i++)
		max_3_vot = _max(x.scores(i,3), max_3_vot);
	// v[v_i] = max_3_vot;
	// v_i++;
	
	double max_3_vot_pre = double(MISPAR_KATAN_MEOD);
	for (int i = _max(y.voice-50,1); i <= _max(y.burst-5,2); i++)
		max_3_vot_pre = _max(x.scores(i,3), max_3_vot_pre);
	v[v_i]= max_3_vot - max_3_vot_pre;
	v_i++;
	
	double max_3_vot_post = double(MISPAR_KATAN_MEOD);
	for (int i = y.burst; i <= _min(y.burst+50,x.size()-1); i++)
		max_3_vot_post = _max(x.scores(i,3), max_3_vot_post);
	v[v_i] = max_3_vot_post - max_3_vot;
	v_i++;
	
	// max(x(wiener_entropy,on-50:on))
	// initialize to -inf in case max is negative
	double max_4_vot_pre = double(MISPAR_KATAN_MEOD);
	for (int i = _max(y.voice-50,1); i <= _max(y.burst-5,2); i++)
		max_4_vot_pre = _max(x.scores(i,4), max_4_vot_pre);
	// v[v_i] = max_4_vot - max_4_vot_pre;
	// v_i++;
	
	
	// max(x(wiener_entropy,on:off-5))
	// initialize to -inf in case max is negative
	double max_4_vot = double(MISPAR_KATAN_MEOD);
	for (int i = y.voice; i <= _max(y.voice,y.burst-5); i++)
		max_4_vot = _max(x.scores(i,4), max_4_vot);
	//v_i++;
	
	// max(x(wiener_entropy,off:off+50))
	// initialize to -inf in case max is negative
	double max_4_post_vot = double(MISPAR_KATAN_MEOD);
	for (int i = y.burst; i <= _min(y.burst+50,x.size()-1); i++)
		max_4_post_vot = _max(x.scores(i,4), max_4_post_vot);
	// v[v_i] = max_4_post_vot;
	// v_i++;
	
	v[v_i] = max_4_post_vot - max_4_vot;
	v_i++;
	
	// // diff max and mean
	// // v=[v; max(x(total_energy,on:off)) - mean(x(total_energy,on:off))];
	// v[v_i] = max_3_vot - mean_3_vot;
	// v_i++;
	
	// mean(x(low_energy,on-50:on))
	double mean_2_pre_vot = 0.0;
	cur_size = 0.0;
	for (int i=_max(y.voice-50,1); i<= _max(y.voice-5,2); i++) {
		mean_2_pre_vot += x.scores(i,2);
		cur_size++;
	}
	mean_2_pre_vot /= cur_size;
	
	// mean(x(low_energy,on:off))
	double mean_2_vot = 0.0;
	for (int i = y.voice; i <= y.burst; i++)
		mean_2_vot += x.scores(i,2);
	mean_2_vot /= double(y.burst-y.voice+1);
	
	// mean(x(low_energy,off:off+5))
	double mean_2_post_vot = 0.0;
	cur_size = 0.0;
	for (int i = y.burst; i <= _min(y.burst + 50, x.size()-1); i++) {
		mean_2_post_vot += x.scores(i,2);
		cur_size++;
	}
	mean_2_post_vot /= cur_size;
	
	// // mean diff
	// // v=[v; mean(x(low_energy,on:off)) - mean(x(low_energy,on-5:on))];
	// v[v_i] = mean_2_vot - mean_2_pre_vot;
	// v_i++;
	
	// mean diff
	// v=[v; mean(x(low_energy,off:off+5)) - mean(x(low_energy,on:off))];
	v[v_i] = mean_2_post_vot - mean_2_vot;
	v_i++;
	
	// max(x(low_energy,on:off))
	// initialize to -inf in case max is negative
	double max_2_vot = double(MISPAR_KATAN_MEOD);
	for (int i = y.voice; i <= _max(y.voice,y.burst-5); i++)
		max_2_vot = _max(x.scores(i,2), max_2_vot);
	v[v_i] = max_2_vot;
	v_i++;
	
	//  // diff max and mean
	//  // v=[v; max(x(low_energy,on:off)) - mean(x(low_energy,on:off)];
	//  v[v_i] = max_2_vot - mean_2_vot;
	//  v_i++;
	
	// mean(x(high_energy,on-5:on))
	double mean_3_pre_vot = 0.0;
	cur_size = 0.0;
	for (int i = _max(y.voice-50,1); i <= _max(y.voice-5,2); i++) {
		mean_3_pre_vot += x.scores(i,3);
		cur_size++;
	}
	mean_3_pre_vot /= cur_size;
	
	// mean(x(high_energy,on:off))
	double mean_3_vot = 0.0;
	for (int i = y.voice; i <=y.burst; i++)
		mean_3_vot += x.scores(i,3);
	mean_3_vot /= double(y.burst - y.voice +1);
	
	// mean(x(high_energy,off:off+5))
	double mean_3_post_vot = 0.0;
	cur_size = 0.0;
	for (int i = y.burst; i <= _min(y.burst + 50, x.size()-1); i++) {
		mean_3_post_vot += x.scores(i,3);
		cur_size++;
	}
	mean_3_post_vot /= cur_size;
	
	// diff mean
	// v=[v; mean(x(high_energy,on:off)) - mean(x(high_energy,on-5:on))];
	v[v_i] = mean_3_vot - mean_3_pre_vot;
	v_i++;
	
	// diff mean
	// v=[v; mean(x(high_energy,off:off+5)) - mean(x(high_energy,on:off))];
	v[v_i] = mean_3_post_vot - mean_3_vot;
	v_i++;
		
	// mean(x(wiener_entropy,on-50:on-5))
	double mean_4_pre_vot = 0.0;
	cur_size = 0.0;
	for (int i = _max(y.voice-50,1); i <= _max(y.voice-5,2); i++) {
		mean_4_pre_vot += x.scores(i,4);
		cur_size++;
	}
	mean_4_pre_vot /= cur_size;
	
	// mean(x(wiener_entropy,on:off-10))
	double mean_4_vot = 0.0;
	for (int i = y.voice; i <= y.burst ; i++)
		mean_4_vot += x.scores(i,4);
	mean_4_vot /= double(y.burst -y.voice);
	
	// mean(x(wiener_entropy,off:off+50))
	double mean_4_post_vot = 0.0;
	cur_size = 0.0;
	for (int i = y.burst; i <= _min(y.burst + 50,x.size()-1); i++) {
		mean_4_post_vot += x.scores(i,4);
		cur_size++;
	}
	mean_4_post_vot /= cur_size;
	
	// // mean diff
	// // v=[v; mean(x(wiener_entropy,on:off-10))-mean(x(wiener_entropy,on-15:on-5))];
	v[v_i] = mean_4_vot - mean_4_pre_vot;
	v_i++;
	
	// mean diff
	// v=[v; mean(x(wiener_entropy,off:off+10))-mean(x(wiener_entropy,on:off-10))];
	v[v_i] = mean_4_post_vot - mean_4_vot;
	v_i++;
	
	
#if 0
	v_i++;
	for (int window = 2; window <= 8; window++) {
		for (int shift = 0; shift <= 0; shift++) {
			v[v_i] = mean_diff_feature_template(x, 15, y.voice, shift, window);
			// std::cout << window << " " << shift << " " << v[v_i] << std::endl;
			v_i++;
			v[v_i] = mean_diff_feature_template(x, 15, y.burst, shift, window);
			// std::cout << window << " " << shift << " " << v[v_i] << std::endl;
			v_i++;
			v[v_i] = mean_diff_feature_template(x, 16, y.voice, shift, window);
			// std::cout << window << " " << shift << " " << v[v_i] << std::endl;
			v_i++;
			v[v_i] = mean_diff_feature_template(x, 16, y.burst, shift, window);
			// std::cout << window << " " << shift << " " << v[v_i] << std::endl;
			v_i++;
			v[v_i] = mean_diff_feature_template(x, 20, y.voice, shift, window);
			// std::cout << window << " " << shift << " " << v[v_i] << std::endl;
			v_i++;
			v[v_i] = mean_diff_feature_template(x, 20, y.burst, shift, window);
			// std::cout << window << " " << shift << " " << v[v_i] << std::endl;
			v_i++;
			v[v_i] = mean_diff_feature_template(x, 21, y.voice, shift, window);
			// std::cout << window << " " << shift << " " << v[v_i] << std::endl;
			v_i++;
			v[v_i] = mean_diff_feature_template(x, 21, y.burst, shift, window);
			// std::cout << window << " " << shift << " " << v[v_i] << std::endl;
			v_i++;
			v[v_i] = mean_diff_feature_template(x, 47, y.voice, shift, window);
			// std::cout << window << " " << shift << " " << v[v_i] << std::endl;
			v_i++;
			v[v_i] = mean_diff_feature_template(x, 47, y.burst, shift, window);
			// std::cout << window << " " << shift << " " << v[v_i] << std::endl;
			v_i++;
		}
	}
#endif
	
	// zero ignored features
	for (uint i = 0; i < features_ignored.size(); i++)
		v[ features_ignored[i] ] = 0.0;
	
	return v;
}


/************************************************************************
 Function:     Classifier::predict
 
 Description:  Predict label of instance x
 Inputs:       SpeechUtterance &x
 VotLocation &y_hat
 Output:       void
 Comments:     none.
 ***********************************************************************/
double Classifier::predict(SpeechUtterance& x, VotLocation &y_hat, bool pos_only)
{
	double D_pos = MISPAR_KATAN_MEOD;
	double D_neg = MISPAR_KATAN_MEOD;
	double D;
	
	VotLocation y_hat_pos, y_hat_neg;
	
	int phi_span = 15;
	
	int	max_onset = _min(max_onset_time, int(x.size()-min_vot_length-phi_span-1));
	
	for (int onset = 0; onset < max_onset; onset++) {
		int min_vot = _min(onset + min_vot_length, int(x.size()-phi_span-1));
		int max_vot = _min(onset + max_vot_length, int(x.size()-phi_span-1));
		//std::cout << "onset= " << onset << " min_vot= " << min_vot << " max_vot= " << max_vot << " x.size= " << x.size() << std::endl;
		for (int offset = min_vot; offset <= max_vot; offset++) {
			VotLocation y_temp;
			y_temp.burst = onset;
			y_temp.voice = offset;

			if (w_pos*phi_pos(x,y_temp) > D_pos) {
				//std::cout << "burst= " << y_temp.burst << " voice= " << y_temp.voice << " wx=" << w_pos*phi_pos(x,y_temp) << std::endl;
				y_hat_pos.burst = y_temp.burst;
				y_hat_pos.voice = y_temp.voice;
				D_pos = w_pos*phi_pos(x,y_temp);
			}
			
			if (!pos_only) {
				y_temp.burst = offset;
				y_temp.voice = onset;
				if (w_neg*phi_neg(x,y_temp) > 1000 || w_neg*phi_neg(x,y_temp) < -1000) {
				LOG(DEBUG) << "phi_neg(x,y_temp)=" << phi_neg(x,y_temp);
				LOG(DEBUG) << "w_neg=" << w_neg;
				LOG(DEBUG) << "w_neg*phi_neg(x,y_temp)=" << w_neg*phi_neg(x,y_temp);
				}
				if (w_neg*phi_neg(x,y_temp) > D_neg) {
					y_hat_neg.burst = y_temp.burst;
					y_hat_neg.voice = y_temp.voice;
					D_neg = w_neg*phi_neg(x,y_temp);
				}
			}
		}
	}
	
	if (pos_only || D_neg < D_pos) {
		D = D_pos;
		y_hat = y_hat_pos;
		LOG(DEBUG) << "Predicted positive VOT";
	} else {
		D = D_neg;
		y_hat = y_hat_neg;
		LOG(DEBUG) << "Predicted negative VOT";
	}
	if (!pos_only) {
		LOG(DEBUG) << "Neg prediction: " << y_hat_neg.burst << " " << y_hat_neg.voice << " conf: " << D_neg;
	}
	LOG(DEBUG) << "Pos prediction: " << y_hat_pos.burst << " "	<< y_hat_pos.voice << " conf: " << D_pos;
	
	return ( D );
	
}

/************************************************************************
 Function:     Classifier::predict_epsilon
 
 Description:  Predict label of instance x
 Inputs:       SpeechUtterance &x
 VotLocation &y_hat
 Output:       void
 Comments:     none.
 ***********************************************************************/
double Classifier::predict_epsilon(SpeechUtterance& x, VotLocation &y_hat,
																	 VotLocation &y, double epsilon, bool vot_loss, bool pos_only)
{
	double D_pos = MISPAR_KATAN_MEOD;
	double D_neg = MISPAR_KATAN_MEOD;
	double D;
	
	VotLocation y_hat_pos, y_hat_neg;
	
	
	// changed 3/29/10 for (0,length-1) basis
	int	max_onset = _min(max_onset_time, int(x.size()-1));
	
	//for (int onset = 1; onset < max_onset; onset++) {
	for (int onset = 0; onset < max_onset; onset++) {
		int min_vot = _min(onset + min_vot_length, int(x.size()-1));
		int max_vot = _min(onset + max_vot_length, int(x.size()-1));
		//    for (int offset = min_vot; offset < max_vot; offset++) {
		for (int offset = min_vot; offset <= max_vot; offset++) {
			VotLocation y_temp;
			y_temp.burst = onset;
			y_temp.voice = offset;
			double my_loss = 0.0;
			if (vot_loss)
				my_loss = loss_vot(y_temp,y) ;
			else
				my_loss = loss(y_temp,y);
			if (w_pos*phi_pos(x,y_temp) - epsilon*my_loss > D_pos) {
				//std::cout << "wx=" << w*phi(x,y_temp) << " (-eps)=" << -epsilon*loss(y_temp,y) << std::endl;
				y_hat_pos.burst = y_temp.burst;
				y_hat_pos.voice = y_temp.voice;
				D_pos = w_pos*phi_pos(x,y_temp) - epsilon*my_loss;
			}
			
			if (!pos_only) {
				y_temp.burst = offset;
				y_temp.voice = onset;
				my_loss = 0.0;
				if (vot_loss)
					my_loss = loss_vot(y_temp,y) ;
				else
					my_loss = loss(y_temp,y);
				if (w_neg*phi_neg(x,y_temp) - epsilon*my_loss > D_neg) {
					y_hat_neg.burst = y_temp.burst;
					y_hat_neg.voice = y_temp.voice;
					D_neg = w_neg*phi_neg(x,y_temp) - epsilon*my_loss;
				}
			}
		}
	}
	
	if (pos_only || D_neg < D_pos) {
		D = D_pos;
		y_hat = y_hat_pos;
	} else {
		D = D_neg;
		y_hat = y_hat_neg;
	}
	
	LOG(DEBUG) << "D_neg=" << D_neg << " D_pos=" << D_pos;
	return ( D );
}

/************************************************************************
 Function:     loss
 
 Description:  Distance between two labels
 Inputs:       VotLocation &y
 VotLocation &y_hat
 Output:       double - the resulted distance
 Comments:     none.
 ***********************************************************************/
double Classifier::loss(const VotLocation &y, const VotLocation &y_hat)
{
	// double new_loss = _max(abs(y_hat.offset - y.offset)-loss_ep_off,0) +
	// _max(abs(y_hat.onset - y.onset)-loss_ep_on,0);
	
	double new_loss = _max(abs(y_hat.voice - y.voice)-loss_ep_off,0) +
	_max(abs(y_hat.burst - y.burst)-loss_ep_on,0);
	
	return new_loss;
}



/************************************************************************
 Function:     loss_vot
 
 Description:  Distance between two labels
 Inputs:       VotLocation &y
 VotLocation &y_hat
 Output:       double - the resulted distance
 Comments:     none.
 ***********************************************************************/
double Classifier::loss_vot(const VotLocation &y, const VotLocation &y_hat)
{
	// double loss_vot = _max(abs( (y_hat.offset - y_hat.onset)
	// - (y.offset-y.onset) )-loss_epsilon,0);
	
	// changed 3/27/10 for consistency with matlab version -- MS
	double loss_vot = _max(abs( (y_hat.voice - y_hat.burst) - (y.voice-y.burst) )-loss_epsilon,0);
	
	return loss_vot;
}



/************************************************************************
 Function:     Classifier::load
 
 Description:  Loads a classifier
 Inputs:       string & filename
 Output:       none.
 Comments:     none.
 ***********************************************************************/
void Classifier::load(std::string &filename)
{
	std::ifstream ifs;
	
	// pos
	std::string filename_pos = filename + ".pos";
	ifs.open(filename_pos.c_str());
	if ( !ifs.good() ) {
		LOG(ERROR) << "Unable to open " << filename_pos;
		exit(-1);
	}
	ifs >> w_pos;
	ifs.close();
	
	// neg
	std::string filename_neg = filename + ".neg";
	ifs.open(filename_neg.c_str());
	if ( !ifs.good() ) {
		LOG(ERROR) << "Unable to open " << filename_neg;
		exit(-1);
	}
	ifs >> w_neg;
	ifs.close();
}


/************************************************************************
 Function:     Classifier::save
 
 Description:  Saves a classifier 
 Inputs:       string & filename
 Output:       none.
 Comments:     none.
 ***********************************************************************/
void Classifier::save(std::string &filename)
{
	std::ofstream ifs;
	
	std::string filename_pos = filename + ".pos";
	ifs.open(filename_pos.c_str());
	if ( !ifs.good() ) {
		LOG(ERROR) << "Unable to open " << filename_pos;
		exit(-1);
	}
	ifs << w_pos;
	ifs.close();
	
	std::string filename_neg = filename + ".neg";
	ifs.open(filename_neg.c_str());
	if ( !ifs.good() ) {
		LOG(ERROR) << "Unable to open " << filename_neg;
		exit(-1);
	}
	ifs << w_neg;
	ifs.close();
	
}

/************************************************************************
 Function:     Classifier::w_star_mean
 
 Description:  Set w to mean of all w_i, and print w
 Inputs:       string & filename
 Output:       none.
 Comments:     none.
 ***********************************************************************/
void Classifier::w_star_mean(int &N)
{
	w_pos = w_pos_sum/N; 
	LOG(DEBUG) << "w_pos_star = " << w_pos ;
	
	w_neg = w_neg_sum/N; 
	LOG(DEBUG) << "w_neg_star = " << w_neg ;
}

// --------------------- EOF ------------------------------------//
