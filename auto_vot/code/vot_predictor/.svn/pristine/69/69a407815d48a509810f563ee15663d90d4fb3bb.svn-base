/************************************************************************
 Copyright (c) 2010 Joseph Keshet
 
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without 
 modification, are permitted provided that the following conditions are 
 met: Redistributions of source code must retain the above copyright 
 notice, this list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the 
 documentation and/or other materials provided with the distribution.
 The name of the author may not be used to endorse or promote products
 derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR 
 IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, 
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
 THE POSSIBILITY OF SUCH DAMAGE.
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

#define MISPAR_KATAN_MEOD (-1000000)
#define _min(a,b) ((a)>(b) ? (b) : (a))
#define _max(a,b) ((a)>(b) ? (a) : (b))

int Classifier::phi_size = 59; 
//int Classifier::phi_size = 77; 



/************************************************************************
 Function:     Classifier::Classifier
 
 Description:  Constructor
 Inputs:       none.
 Output:       none.
 Comments:     none.
 ***********************************************************************/
Classifier::Classifier(int _min_vot_length, int _max_vot_length, 
                       int _max_onset_time, double _C, double _loss_epsilon,
                       double _min_loss_update, 
                       std::string _kernel_str, double _sigma) : 
min_vot_length(_min_vot_length),
max_vot_length(_max_vot_length),
max_onset_time(_max_onset_time),
PA1_C(_C),
loss_epsilon(_loss_epsilon),
min_loss_update(_min_loss_update),
w(phi_size), 
w_sum(phi_size), 
w_old(phi_size),
kernel_expansion(_kernel_str,Classifier::phi_size,_sigma) 
{
  if (!kernel_expansion.is_linear_kernel()) {
    w.resize(kernel_expansion.features_dim());
    w_sum.resize(kernel_expansion.features_dim());
    w_old.resize(kernel_expansion.features_dim());
  }
  w.zeros();
  w_sum.zeros();
  w_old.zeros();
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
      std::cerr << "Error: can only ignore features in the set of 0-"
      << (Classifier::phi_size-1) << ". " << feature_num << " is not a valid feature number." 
      << std::endl;
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
		current_loss = loss_vot(y,y_hat) ;
	else 
		current_loss = loss(y,y_hat) ;
  std::cout << "y=" << y << " y_hat=" << y_hat << " loss = " << current_loss << std::endl;
  
  if (current_loss < min_loss_update) {
    w_changed = false;
    // add this w_i to sum of all w
    w_sum += w;
    return 0.0;
  }
  
  infra::vector delta_phi(phi_size);
  if (!kernel_expansion.is_linear_kernel()) 
    delta_phi.resize(kernel_expansion.features_dim());
  infra::vector_view phi_x_y = phi(x,y);
  infra::vector_view phi_x_y_hat = phi(x,y_hat);
  
  delta_phi = phi_x_y - phi_x_y_hat;
  // changed 3/27/10 for consistency w/ matlab version -- MS
  // delta_phi /= 2.0;
  current_loss -= w*delta_phi;
  
  //std::cout << "phi(x,y)= " << phi_x_y << std::endl;
  //std::cout << "phi(x,y_hat)= " << phi_x_y_hat << std::endl;
  //  std::cout << "delta_phi = " << delta_phi << std::endl;
  std::cout << "delta_phi.norm2() = "  << delta_phi.norm2() << std::endl;
  std::cout << "hinge_loss = " << current_loss << std::endl;
  
  if (current_loss > 0.0)  {
    // keep old w
    w_old = w;
    // update
    double tau = current_loss / delta_phi.norm2();
    if (tau > PA1_C) tau = PA1_C; // PA-I
    std::cout << "tau = " << tau << std::endl;
    delta_phi *= tau;
    w += delta_phi;
  }
  else {
    if (y.offset - y.onset <= min_vot_length) 
      std::cout << "Warning: hinge loss is less than zero because y.offset - y.onset <= min_vot_length" << std::endl;
    else
      std::cout << "Error: hinge loss is less than zero!!" << std::endl;
  }

  
  //  std::cout << "w = " << w << std::endl;  
  w_changed = true;
  
  // add this w_i to sum of all w
  w_sum += w;
  
  return current_loss;
}



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
  infra::vector_view phi_x_y_hat_eps = phi(x,y_hat_eps);
  infra::vector_view phi_x_y_hat = phi(x,y_hat);
  
  infra::vector delta_phi(phi_size);
  delta_phi = phi_x_y_hat_eps - phi_x_y_hat;
  
  //std::cout << "phi(x,y_hat_eps)= " << phi_x_y_hat_eps << std::endl;
  //std::cout << "phi(x,y_hat)= " << phi_x_y_hat << std::endl;
  //std::cout << "delta_phi = " << delta_phi << std::endl;
  std::cout << "delta_phi.norm2() = "  << delta_phi.norm2() << std::endl;
  
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
    std::cout << "tau(PA) = " << tau_pa << std::endl;
  }
	
  // keep the loss and w before update
  infra::vector w_before_update(w);
  
  // start up values for update
  VotLocation y_hat_eps_tmp(y_hat_eps);
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
    std::cout << "tau(DA)=" << tau 
    << " loss_before=" << loss_before_update
    << " loss_after=" << loss_after_update << std::endl;
    tau /= 2;
  } while (loss_after_update >= loss_before_update && tau > 0.05) ;
  
  
  if (loss_after_update >= loss_before_update) {
    w = w_before_update;  
    w_changed = false;
  }
  else {
    // keep old w
    w_old = w_before_update;
    std::cout << "w = " << w << std::endl;  
    w_changed = true;
  }
  
  return loss_after_update;
}


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
 
 Description:  calculate phi of x for update
 Inputs:       SpeechUtterance &x
 VotLocation &y
 Output:       infra::vector_view
 Comments:     none.
 ***********************************************************************/
infra::vector_view Classifier::phi(SpeechUtterance& x, VotLocation& y) 
{
  infra::vector v(phi_size);
  v.zeros();
  
  // onsetInds=[1:12 16:18 19 21 22];
  int onset_indices[] = {0,1,2,3,4,5,6,7,8,9,10,11,15,16,17,18,20,21};
  
  // % feats 19-38, 39-44
  // offsetInds=[1:12 16:18 19:22 24 45:47 49:51];
  int offset_indices[] = {0,1,2,3,4,5,6,7,8,9,10,11,15,16,17,18,19,20,21,23,44,45,46,48,49,50};
  
  int v_i = 0;
  
  // v=[x(onsetInds,on); x(offsetInds,off)];
  for (int i = 0; i < int(sizeof(onset_indices)/sizeof(int)); i++) {
    v[v_i] = x.scores(y.onset,onset_indices[i]);
    v_i++;
  }
  for (int i = 0; i < int(sizeof(offset_indices)/sizeof(int)); i++) {
    v[v_i] = x.scores(y.offset,offset_indices[i]);
    v_i++;
  }
  
  // v(end+1)=mean(x(16,on:off));
  for (int i = y.onset; i <= y.offset; i++) 
    v[v_i] += x.scores(i,15);
  v[v_i] /= double(y.offset-y.onset+1);
  v_i++;
  
  // v(end+1)=max(x(16,on:off));
  // initialize to -inf in case max is negative
  v[v_i] = double(MISPAR_KATAN_MEOD);
  for (int i = y.onset; i <= y.offset; i++)
    v[v_i] = _max(x.scores(i,15), v[v_i]);
  v_i++;
  
  int why_10 = 10;
  
  // v(end+1)=mean(x(16,on:max(off-10,on)));
  int max_i = _max(y.onset,y.offset-why_10);
  for (int i = y.onset; i <= max_i; i++)
    v[v_i] += x.scores(i,15);
  v[v_i] /= double(max_i-y.onset+1);
  v_i++;
  
  // v(end+1)=max(x(16,on:max(off-10,on)));
  // initialize to -inf in case max is negative
  v[v_i] = double(MISPAR_KATAN_MEOD);
  for (int i = y.onset; i <= max_i; i++)
    v[v_i] = _max(x.scores(i,15), v[v_i]);
  v_i++;
  
  
  // v(end+1)=mean(x(17,on:off));
  for (int i = y.onset; i <= y.offset; i++)
    v[v_i] += x.scores(i,16);
  v[v_i] /= double(y.offset-y.onset+1);
  v_i++;
  
  // v(end+1)=max(x(17,on:off));
  // initialize to -inf in case max is negative
  v[v_i] = double(MISPAR_KATAN_MEOD);
  for (int i = y.onset; i <= y.offset; i++)
    v[v_i] = _max(x.scores(i,16), v[v_i]);
  v_i++;
  
  
  // v(end+1)=mean(x(17,on:max(off-10,on)));
  for (int i = y.onset; i <= max_i; i++)
    v[v_i] += x.scores(i,16);
  v[v_i] /= double(max_i-y.onset+1);
  v_i++;
  
  // vv(end+1)=max(x(17,on:max(off-10,on)));
  // initialize to -inf in case max is negative
  v[v_i] = double(MISPAR_KATAN_MEOD);
  for (int i = y.onset; i <= max_i; i++)
    v[v_i] = _max(x.scores(i,16), v[v_i]);
  v_i++;
  
  // mean(x(21,on:off))
  double mean_20_vot = 0.0;
  for (int i = y.onset; i <= y.offset; i++) 
    mean_20_vot += x.scores(i,20);
  mean_20_vot /= double(y.offset-y.onset+1);
  
  // mean(x(21,1:on)
  double mean_20_pre_vot = 0.0;
  for (int i = 0; i <= y.onset; i++) 
    mean_20_pre_vot += x.scores(i,20);
  mean_20_pre_vot /= double(y.onset+1);
  
  
  // mean(x(22,on:off))
  double mean_21_vot = 0.0;
  for (int i = y.onset; i <= y.offset; i++) 
    mean_21_vot += x.scores(i,21);
  mean_21_vot /= double(y.offset-y.onset+1);
  
  // mean(x(22,1:on)
  double mean_21_pre_vot = 0.0;
  for (int i = 0; i <= y.onset; i++) 
    mean_21_pre_vot += x.scores(i,21);
  mean_21_pre_vot /= double(y.onset+1);
  
  // % mean diff
  // v=[v; mean(x(21,on:off))-mean(x(21,1:on))];
  v[v_i] = mean_20_vot - mean_20_pre_vot;
  v_i++;
  
  // v=[v; mean(x(22,on:off))-mean(x(22,1:on))];
  v[v_i] = mean_21_vot - mean_21_pre_vot;
  v_i++;
  
  // % max of high, WE from 1:onset-5
  // v=[v; max(x(21,1:max(on-5,1)))];
  
  // initialize to -inf in case max is negative
  v[v_i] = double(MISPAR_KATAN_MEOD);
  max_i = _max(1,y.onset-5);
  for (int i = 0; i <= max_i; i++)
    v[v_i] = _max(x.scores(i,20), v[v_i]);
  v_i++;
  
  // v=[v; max(x(22,1:max(on-5,1)))];
  
  // initialize to -inf in case max is negative
  v[v_i] = double(MISPAR_KATAN_MEOD);
  for (int i = 0; i <= max_i; i++)
    v[v_i] = _max(x.scores(i,21), v[v_i]);
  v_i++;
  
  // % mean of high, WE from 1:onset-5
  // v=[v; mean(x(21,1:max(on-5,1)))];
  for (int i = 0; i <= max_i; i++)
    v[v_i] += x.scores(i,20);
  v[v_i] /= double(max_i+1);
  v_i++;
  
  // v=[v; mean(x(22,1:max(on-5,1)))];
  for (int i = 0; i <= max_i; i++)
    v[v_i] += x.scores(i,21);
  v[v_i] /= double(max_i+1);
  v_i++;
  
  // v=[v; mean(x(48,1:max(off-5,1)))];
  max_i = _max(1,y.offset-5);
  for (int i = 0; i <= max_i; i++)
    v[v_i] += x.scores(i,47);
  v[v_i] /= double(max_i+1);

#if 0
  v_i++;
  for (int window = 2; window <= 8; window++) {
    for (int shift = 0; shift <= 0; shift++) {
      v[v_i] = mean_diff_feature_template(x, 15, y.onset, shift, window);
      // std::cout << window << " " << shift << " " << v[v_i] << std::endl;
      v_i++;
      v[v_i] = mean_diff_feature_template(x, 15, y.offset, shift, window);
      // std::cout << window << " " << shift << " " << v[v_i] << std::endl;
      v_i++;
      v[v_i] = mean_diff_feature_template(x, 16, y.onset, shift, window);
      // std::cout << window << " " << shift << " " << v[v_i] << std::endl;
      v_i++;
      v[v_i] = mean_diff_feature_template(x, 16, y.offset, shift, window);
      // std::cout << window << " " << shift << " " << v[v_i] << std::endl;
      v_i++;
      v[v_i] = mean_diff_feature_template(x, 20, y.onset, shift, window);
      // std::cout << window << " " << shift << " " << v[v_i] << std::endl;
      v_i++;
      v[v_i] = mean_diff_feature_template(x, 20, y.offset, shift, window);
      // std::cout << window << " " << shift << " " << v[v_i] << std::endl;
      v_i++;
      v[v_i] = mean_diff_feature_template(x, 21, y.onset, shift, window);
      // std::cout << window << " " << shift << " " << v[v_i] << std::endl;
      v_i++;
      v[v_i] = mean_diff_feature_template(x, 21, y.offset, shift, window);
      // std::cout << window << " " << shift << " " << v[v_i] << std::endl;
      v_i++;
      v[v_i] = mean_diff_feature_template(x, 47, y.onset, shift, window);
      // std::cout << window << " " << shift << " " << v[v_i] << std::endl;
      v_i++;
      v[v_i] = mean_diff_feature_template(x, 47, y.offset, shift, window);
      // std::cout << window << " " << shift << " " << v[v_i] << std::endl;
      v_i++;
    }
  }
#endif
  
  // zero ignored features
  for (uint i = 0; i < features_ignored.size(); i++) 
    v[ features_ignored[i] ] = 0.0;
 
  // approdximate kernel by expanding the feature phi(x,y)
  if (!kernel_expansion.is_linear_kernel()) {
    infra::vector u(kernel_expansion.features_dim());
    u = kernel_expansion.expand(v);
    return u;
  }
  
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
double Classifier::predict(SpeechUtterance& x, VotLocation &y_hat)
{
  double D = MISPAR_KATAN_MEOD;
  
  // changed 3/29/10 for (0,length-1) basis
  int	max_onset = _min(max_onset_time, int(x.size()-1));
  
  //for (int onset = 1; onset < max_onset; onset++) {
  for (int onset = 0; onset < max_onset; onset++) {
    int min_vot = _min(onset + min_vot_length, int(x.size()-1));
    int max_vot = _min(onset + max_vot_length, int(x.size()-1));
    //    for (int offset = min_vot; offset < max_vot; offset++) {
    for (int offset = min_vot; offset <= max_vot; offset++) {
      VotLocation y_temp;
      y_temp.onset = onset;
      y_temp.offset = offset;
      if (w*phi(x,y_temp) > D) {
        y_hat.onset = y_temp.onset;
        y_hat.offset = y_temp.offset;
        D = w*phi(x,y_temp);
      }
    }
  }
  
  return D;
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
                           VotLocation &y, double epsilon)
{
  double D = MISPAR_KATAN_MEOD;
  
  // changed 3/29/10 for (0,length-1) basis
  int	max_onset = _min(max_onset_time, int(x.size()-1));
  
  //for (int onset = 1; onset < max_onset; onset++) {
  for (int onset = 0; onset < max_onset; onset++) {
    int min_vot = _min(onset + min_vot_length, int(x.size()-1));
    int max_vot = _min(onset + max_vot_length, int(x.size()-1));
    //    for (int offset = min_vot; offset < max_vot; offset++) {
    for (int offset = min_vot; offset <= max_vot; offset++) {
      VotLocation y_temp;
      y_temp.onset = onset;
      y_temp.offset = offset;
      if (w*phi(x,y_temp) - epsilon*loss(y_temp,y) > D) {
        //std::cout << "wx=" << w*phi(x,y_temp) << " (-eps)=" << -epsilon*loss(y_temp,y) << std::endl;
        y_hat.onset = y_temp.onset;
        y_hat.offset = y_temp.offset;
        D = w*phi(x,y_temp);
      }
    }
  }
  
  return D;
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
  //  double loss = _max(abs(y_hat.offset - y.offset),loss_epsilon) +
  //  _max(abs(y_hat.onset - y.onset),loss_epsilon);
  
  // changed 3/27/10 for consistency with matlab version -- MS
  double loss = _max(abs(y_hat.offset - y.offset)-loss_epsilon,0) +
  _max(abs(y_hat.onset - y.onset)-loss_epsilon,0);
  
  
  return loss;
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
  //  double loss = _max(abs(y_hat.offset - y.offset),loss_epsilon) +
  //  _max(abs(y_hat.onset - y.onset),loss_epsilon);
  
  // changed 3/27/10 for consistency with matlab version -- MS
  double loss_vot = _max(abs( (y_hat.offset - y_hat.onset) - (y.offset-y.onset) )-loss_epsilon,0);
  
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
  ifs.open(filename.c_str());
  if ( !ifs.good() ) {
    std::cerr << "Unable to open " << filename << std::endl;
    exit(-1);
  }
  
  ifs >> w;
  
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
  ifs.open(filename.c_str());
  if ( !ifs.good() ) {
    std::cerr << "Unable to open " << filename << std::endl;
    exit(-1);
  }
  
  ifs << w;
  
  ifs.close();
}

/************************************************************************
 Function:     Classifier::w_star_mean
 
 Description:  Set w to mean of all w_i, and print w
 Inputs:       string & filename
 Output:       none.
 Comments:     none.
 ***********************************************************************/
void Classifier::w_star_mean(int &N){
  w = w_sum/N; 
  std::cout << "w_star = " << w << std::endl;  
}

// --------------------- EOF ------------------------------------//
