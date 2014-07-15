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
      void traceback(unsigned int* trace, unsigned char *data, 
                     const int k, const int end_state , const int frame_size)
      {
        int s = end_state;
        for(int i = frame_size-1; i >= 0; i--)
        { 
          int input = trace[i*(1<<(k-1))+s];
          data[i] = (s >> (k-2)) &1;  
          s = ((s<<1)| input )&15;

        }
      }

      void viterbi_fi(const int s, const int k, 
                      const int frame_size, const int start_state, 
                      int end_state, const std::vector<int> &OS, 
                      const float *in, unsigned char *out)
      {
        int state; // Counting variable for all States
        size_t align = volk_fec_get_alignment();
        float min = INF;

        // Allocate Memory for trace matrix
        unsigned int *trace;
        trace = (unsigned int*) volk_fec_malloc((sizeof(unsigned int)*s*frame_size), align);
        
        // Initial metrics state that is given to the kernel (maybe^^)
        float *alpha;
        alpha = (float*) volk_fec_malloc(sizeof(float)*s*2, align);
        
        // Initialize first Column of metrics;
        for(state = 0; state < s; state++)
        {
          alpha[state] = INF;
        }
        alpha[start_state] = 0;

        volk_fec_32f_x2_s32f_32i_viterbi_metric_32i_manual(trace, alpha, in , frame_size, &OS[0], s, "generic");

        // get endstate
        if( end_state == -1)
        {  
          for(state = 0; state < s; state++)
          {
            if(alpha[state] < min)
            {
              min = alpha[state];
              end_state = state;
            }  
          }
        }

        printf("EndState: %d\n", end_state);
        // Traceback
        traceback(trace, out, k, end_state, frame_size);
        for (int i = 0; i < s; ++i)
        {
          printf("Alpha_out[%d]: %f\n", i, alpha[i]);
        }
        volk_fec_free(alpha);
        volk_fec_free(trace);
      }
    }
  }
}