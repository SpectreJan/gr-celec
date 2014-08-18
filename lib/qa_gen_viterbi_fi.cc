/* -*- c++ -*- */
/* 
 * Copyright 2014 <+YOU OR YOUR COMPANY+>.
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


#include <gnuradio/attributes.h>
#include <cppunit/TestAssert.h>
#include "qa_gen_viterbi_fi.h"
#include <celec/gen_viterbi_fi.h>
#include <stdio.h>
#include <volk_fec/volk_fec.h>
#include <gnuradio/random.h>
#include <stdlib.h>

#define INF 1e9

namespace gr {
  namespace celec {

    void
    qa_gen_viterbi_fi::t1()
    {
      int vlen = 3072;  // 3072 Infowords
      int iters = 1;
      int S = 16;  // 16 States
      int O = 4;  // 4 Output Symbols
      
      double time_volk, time_generic;
      
      size_t align = volk_fec_get_alignment();
      float *alpha_generic;
      alpha_generic = (float*)volk_fec_malloc(sizeof(float)*2*S, align);
      
      float *alpha_sse;
      alpha_sse = (float*)volk_fec_malloc(sizeof(float)*2*S, align);

      for(int i = 0; i < 2*S; i++)
      {  
        alpha_generic[i] = INF;
        alpha_sse[i] = INF;
      }  
      alpha_sse[0]     = 0;
      alpha_generic[0] = 0;

      unsigned int *trace_generic;
      trace_generic = (unsigned int*)volk_fec_malloc(sizeof(int)*vlen*S, align);
      
      unsigned int *trace_sse;
      trace_sse = (unsigned int*)volk_fec_malloc(sizeof(int)*vlen*S, align);
      
      int OS[] = {0, 3, 3, 0, 1, 2, 2, 1, 1, 2, 2, 1, 0, 3, 3, 0, 2, 1, 1, 2, 3, 0, 0, 3, 3, 0, 0, 3, 2, 1, 1, 2}; 
      
      float *in;
      in = (float*) volk_fec_malloc(sizeof(float)*vlen*O, align);
      for(int i = 0; i < vlen*O; i++)
      {  
        // Create random samples ranging from -4 to 4
        in[i] = (float) (((float)::random()/RANDOM_MAX)*5);
      }
      printf("\nRunning Test: volk_fec_32f_x2_s32f_x3_32i_x3_viterbi_metric_32i_32f ...\n");
      clock_t t_volk, t;
      for(int i = 0; i < iters; i++)
      {
        t_volk = clock();
        volk_fec_32f_x2_s32f_32i_viterbi_metric_32i_manual(&trace_sse[0], &alpha_sse[0], &in[0], vlen,
                                          &OS[0],  S, "a_sse4");

        t_volk = clock() - t_volk;
        time_volk += ((float) t_volk);
  
        t = clock();
       
        volk_fec_32f_x2_s32f_32i_viterbi_metric_32i_manual(&trace_generic[0], &alpha_generic[0], &in[0], vlen,
                                                 &OS[0], S, "generic");
      
        t = clock() - t;
        time_generic += ((float) t);
        for(int i = 0; i < vlen*S; i++)
        {
          //printf("trace gen[%d]: %d trace sse[%d]: %d\n", i, trace_generic[i],i, trace_sse[i]);
          CPPUNIT_ASSERT_DOUBLES_EQUAL((double)trace_generic[i], (double)trace_sse[i],
          0.1);
          // CPPUNIT_ASSERT_DOUBLES_EQUAL(alpha_generic[i], alpha_sse[i],
          // 0.1);
        }

      }
      printf("Time for SSE: %f\n", time_volk/CLOCKS_PER_SEC/iters);
      printf("Time for generic: %f\n", time_generic/CLOCKS_PER_SEC/iters);
    }

  } /* namespace celec */
} /* namespace gr */

