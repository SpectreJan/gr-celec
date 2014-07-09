/* -*- c++ -*- */
/* 
 * Copyright 2014 Jan Krämer.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

 #ifndef INCLUDED_VITERBI_H
 #define INCLUDED_VITERBI_H
 #include <vector>

namespace gr{
  namespace celec{
    namespace viterbi_fi{
      void viterbi_fi(const int S, const int K, const int S0, int SK, const std::vector<int> &OS, const float *in, unsigned char *out);
    }
  } // namespace celec
} // namespace gr
#endif /* INCLUDED_VITERBI_H */