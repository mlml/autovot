//=============================================================================
// File Name: active_set.h
// Written by: Shai Shalev-Shwartz (shais@cs.huji.ac.il)
//
// Distributed as part of the active-set library
// Copyright (C) 2004 Shai Shalev-Shwartz
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//=============================================================================

//==================================================================================
// File Name: active_set.h
// Written by: Shai Shalev-Shwartz.
// This code implements a basic class for active set manipulations
//===================================================================================
#ifndef _SHAI_ACTIVE_SET_
#define _SHAI_ACTIVE_SET_

//==================================
// included files and typedefs
#include <infra.h>
typedef unsigned int uint;
//==================================




/** Implements a basic dataset for manipulating support vectors and their alpha
    for the Hildreth algorithm.
    @author Shai Shalev-Shwartz (shais@cs.huji.ac.il)
*/
class ActiveSet {
 public:
  
  /** Constructor. Allocate memory for the active set. The actual number of
      elements in the active set is initially zero.
      @param m The total number of examples in the learning problem
      @param n The dimension of each instance in the active set
      @param p We allocate an initial matrix of size (p*m,n). Default value of p is 0.1 
  */
  ActiveSet(uint m, uint n, double p = 0.1) :
    Data_((uint)(m*p),n) , svc_(0), Index_(m) , Inverse_Index_(m) , n_examples(m) { 
    for (uint i=0; i < m; ++i) 
      Index_[i] = m; // Index_[i] = m means that the example i is not in the set
  }

  /** Constructor. Read an active set from the disk
      @param fid A pointer to FILE in which an active set was previously written
      using save_binary method
  */
  ActiveSet(FILE * fid);


  /** parantheses operator. 
      @return A reference to a matrix_view of the actual current active set
  */
  inline infra::matrix_view operator() ();

  /** Insertion to the support.
      @param x A reference to a vector_view of the element to insert to active set
      @param ind The index of the instance (important for Hildreth-like procedure)
  */
  inline void insert(infra::vector_view & x,  uint ind);


  /** Remove from the support.
      @param ind The index of the instance to remove
  */
  inline void remove(uint ind);

  /** Get one row from the active set.
      @param ind The index of the instance to get
  */
  inline infra::vector_view get(uint ind);

  /** Check if an example is in the active set
      @param ind The index of the example to check
  */
  inline bool does_exist(uint ind);

  /** size. 
      @return The number of actual elements in the active set.
  */
  inline uint size() { return(svc_); }

  /** number of examples 
    @return The number of total examples introduced to the learning algorithm.
    */
  inline uint num_examples() { return(n_examples); }
  
  
  /** Physical to Logical index. 
      @param p_ind An index to a position in the Data_matrix
      @return The index of the example that store in Data_[p_ind]
  */
  inline uint get_logical_index(uint p_ind) { return(Inverse_Index_[p_ind]); }


  /** save the active-set to the disk.
      @param file_id A pointer to FILE, where the active set will be written to
  */
  inline void save_binary(FILE * fid);

  /** load the active-set from file.
      @param file_id A pointer to FILE, where the active set will be written to
  */
  inline void load_binary(FILE * fid);

  ~ActiveSet() { }
  
 protected:
  
  infra::matrix Data_; // the matrix with all the memory allocated for the support vectors
  uint svc_; // support vector counter. The actual number of support vectors
  std::vector<uint> Index_; // an stl vector. Index_[i] is the position of example i in the Data_ matrix
  std::vector<uint> Inverse_Index_; // an stl vector. Inverse_Index_[i] is the index of the example that located in the position i of the Data_ matrix
  uint n_examples; // number of examples in the learning set (this is the size of Index_)
};



#endif
