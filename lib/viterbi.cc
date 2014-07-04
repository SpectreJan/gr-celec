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

#include <stdio.h>
#include <celec/viterbi.h>
#include <volk_fec/volk_fec.h>

#define INF 1e9

namespace gr {
  namespace celec{
    namespace viterbi_fi{
      void viterbi_fi(const int S, const int K, const int S0, const int *OS, const float *in, unsigned int *out)
      {
        int s,k; // Counting variable for all States
        size_t align = volk_fec_get_alignment();

        // Allocate Memory for packed trace matrix
        unsigned int *trace;
        trace = (unsigned int*) volk_fec_malloc(sizeof(unsigned int)*S*K/sizeof(unsigned int)/8, align); // Pack 32 bit into one unsigned int
        // Initial alpha state that is given to the kernel (maybe^^)
        float *alpha;
        alpha = (float*) volk_fec_malloc(sizeof(float)*S*2, align);
        

        for(s = 0; s < S; s++)
        {
          alpha[s] = INF; // Initialize first Column of A;
        }
        alpha[S0] = 0;

        volk_fec_32f_x2_s32f_32i_viterbi_metric_32i_manual(trace, alpha, in ,K, OS, S, "generic");
        volk_fec_free(alpha);
        volk_fec_free(trace);
      }
    }
  }
}