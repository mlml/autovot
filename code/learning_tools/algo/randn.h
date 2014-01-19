
#include "infra.h"

// randomize a matrix with zero mean and unit convariance from Gaussian distribution
infra::matrix_view randn_matrix(unsigned int height, unsigned int width)
{
  infra::matrix output_matrix(height,width);
  double v1, v2, rsq;
  
  for (uint i=0; i < height; i++) {
    for (uint j = 0; j < width; j += 2) {
      
      // pick two uniform numbers in the square extending from -1 to +1 in each direction,
      // see if they are in the unit circle, or try again.
      do {
        v1 = 2.0*rand()/double(RAND_MAX)-1.0;
        v2 = 2.0*rand()/double(RAND_MAX)-1.0;
        rsq = v1*v1+v2*v2;
      } while (rsq >= 1.0 || rsq == 0.0);
      // Now make the Box-Muller transformation to get two normal deviates.
      output_matrix(i,j) = v1*sqrt(-2.0*log(rsq)/rsq);
      if (j+1 < width)
        output_matrix(i,j+1) = v2*sqrt(-2.0*log(rsq)/rsq);
    }
  }
	
	return output_matrix;
}