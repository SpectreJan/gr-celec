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
      void traceback(unsigned char* trace, unsigned char *data, 
                     const int S, const int SK , const int K)
      {
        int s = SK;
        for(int k = K-1; k >= 0; k--)    // 8 Bit per char!!
        { 
          int input = trace[k*S+s];
          //printf("State at %d: %d\n", k, s);
          data[k] = (s >= (S/2));
          //printf("Data[%d]: %d\n", k, data[k]);
          //printf("Trace: %d Input[%d]: %d\n", trace[(k*S+s)], k, input);  
          s = ((s<<1)| input )&15;

        }
      }

      void viterbi_fi(const int S, const int K, const int S0, int SK, const std::vector<int> &OS, const float *in, unsigned char *out)
      {
        int s,k; // Counting variable for all States
        size_t align = volk_fec_get_alignment();
        float min = INF;

        // Allocate Memory for packed trace matrix
        unsigned char *trace;
        trace = (unsigned char*) volk_fec_malloc((sizeof(unsigned char)*S*K), align); // Pack 32 bit into one unsigned int
        // Initial alpha state that is given to the kernel (maybe^^)
        float *alpha;
        alpha = (float*) volk_fec_malloc(sizeof(float)*S*2, align);
        

        for(s = 0; s < S*2; s++)
        {
          alpha[s] = INF; // Initialize first Column of A;
        }
        alpha[S0] = 0;

        volk_fec_32f_x2_s32f_32i_viterbi_metric_32i_manual(trace, alpha, in ,K, &OS[0], S, "generic");

        if( SK == -1)
        {  
          for(s = 0; s < S; s++)
          {
            if(alpha[s] < min)
            {
              min = alpha[s];
              SK = s;
            }  
          }
        }

        // Traceback
        traceback(trace, out, S, SK, K);
        volk_fec_free(alpha);
        volk_fec_free(trace);
      }
    }
  }
}