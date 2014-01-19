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

/************************************************************************
 Function:     Classifier::Classifier
 
 Description:  Constructor
 Inputs:       none.
 Output:       none.
 Comments:     none.
 ***********************************************************************/
Classifier::Classifier(int _min_vot_length, int _max_vot_length, 
											 int _max_onset_time, double _C, double _loss_epsilon,
                       double _min_loss_update) : 
min_vot_length(_min_vot_length),
max_vot_length(_max_vot_length),
max_onset_time(_max_onset_time),
PA1_C(_C),
loss_epsilon(_loss_epsilon),
min_loss_update(_min_loss_update),
w(phi_size), 
w_old(phi_size)
{
  w.zeros();
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
 Function:     Classifier::update
 
 Description:  Train classifier with one example 
 Inputs:       infra::vector& x - example instance 
 int y - label 
 Output:       double - squared loss
 Comments:     none.
 ***********************************************************************/
double Classifier::update( SpeechUtterance& x, VotLocation y, VotLocation &y_hat) 
{
  double current_loss = 0.0;
  
  current_loss = loss(y,y_hat) ;
  std::cout << "y=" << y << " y_hat=" << y_hat << " loss = " << current_loss << std::endl;
  
  if (current_loss < min_loss_update) {
    w_changed = false;
    return 0.0;
  }
  
  infra::vector delta_phi(phi_size);
  infra::vector_view phi_x_y = phi(x,y);
  infra::vector_view phi_x_y_hat = phi(x,y_hat);
  
  delta_phi = phi_x_y - phi_x_y_hat;
  delta_phi /= 2.0;
  current_loss -= w*delta_phi;
  
  //std::cout << "phi(x,y)= " << phi_x_y << std::endl;
  //std::cout << "phi(x,y_hat)= " << phi_x_y_hat << std::endl;
  //std::cout << "delta_phi = " << delta_phi << std::endl;
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
  
//  std::cout << "w = " << w << std::endl;  
  w_changed = true;
  
  return current_loss;
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
	
  // v(end+1)=mean(x(16,on:max(off-10,on)));
  int max_i = _max(y.onset,y.offset-10);
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
	
  int	max_onset = _min(max_onset_time, int(x.size()));
  for (int onset = 1; onset < max_onset; onset++) {
    int min_vot = _min(onset + min_vot_length, int(x.size()));
    int max_vot = _min(onset + max_vot_length, int(x.size()));
    for (int offset = min_vot; offset < max_vot; offset++) {
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
 Function:     gamma
 
 Description:  Distance between two labels
 Inputs:       VotLocation &y
 VotLocation &y_hat
 Output:       double - the resulted distance
 Comments:     none.
***********************************************************************/
double Classifier::loss(const VotLocation &y, const VotLocation &y_hat)
{
  double loss = _max(abs(y_hat.offset - y.offset),loss_epsilon) +
    _max(abs(y_hat.onset - y.onset),loss_epsilon);
  return loss;
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



// --------------------- EOF ------------------------------------//
