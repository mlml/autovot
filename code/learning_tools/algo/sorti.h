//=============================================================================
// File Name: sorti.h
// Written by: Joseph Keshet & Shai Shalev-Shwartz ({jkeshet,shais}@cs.huji.ac.il)
//
// Distributed as part of the sorti library
// Copyright (C) 2004 Joseph Keshet & Shai Shalev-Shwartz
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

#include <algorithm>
#include <vector>
#include <iostream>

namespace std {
template <class RandomAccessIterator>
class iterator_with_index
{
 public:
  RandomAccessIterator I;
  unsigned int i;
};

template <class RandomAccessIterator>
bool iterator_with_index_sort_criterion (const iterator_with_index<RandomAccessIterator>& p1, 
					 const iterator_with_index<RandomAccessIterator>& p2)
{
  RandomAccessIterator t1 = p1.I;  RandomAccessIterator t2 = p2.I;
  return (*(t1) < *(t2));
}


template <class RandomAccessIterator>
void sorti(RandomAccessIterator first, RandomAccessIterator last, std::vector<unsigned int> &indices) 
{
  RandomAccessIterator _iterator = first;
  unsigned int _size = 0;
  for (_iterator = first; _iterator != last; _iterator++)
    _size++;
  std::vector<iterator_with_index<RandomAccessIterator> > tmp(_size);
  _iterator = first;
  for (unsigned int j=0; j < _size; j++, _iterator++) {
    tmp[j].I = _iterator;
    tmp[j].i = j;
  }
  std::sort(tmp.begin(), tmp.end(), iterator_with_index_sort_criterion<RandomAccessIterator>);
  if (indices.size() != tmp.size()) 
    indices.resize(tmp.size());
  for (unsigned int j=0; j < tmp.size(); j++) {
    indices[j] = tmp[j].i;
  }
}

}
