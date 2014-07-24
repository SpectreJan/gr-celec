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


#define P

#include <stdio.h>
#include <celec/bcjr.h>
#include <volk_fec/volk_fec.h>

namespace gr{
  namespace celec{
    namespace bcjr_f{
      void bcjr_f(const int S, const int k, const int o, const int K, 
                  const int S0, int SK, const std::vector<int> &OS,
                  decoder *dp,
                  const float *in, float *out)
      {
        int state; // Counting Variable for all states
        size_t align = volk_fec_get_alignment(); // Get Alignment for vectors
        for(int i = 0; i < S; i++)
          printf("Alpha[%d] = %f\n", i, dp->alpha[i]);
      }
    }
  }
}