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
#include "qa_max_log_map_f.h"
#include <celec/max_log_map_f.h>
#include <volk_fec/volk_fec.h>



namespace gr {
  namespace celec {

    void
    qa_max_log_map_f::t1()
    {
        int INF = 1e9;
        size_t align = volk_fec_get_alignment();
        float test_llr[10] = {2.7, -0.5, -0.1, -0.6, -0.4, 2.1, 1.1, 0.8, 0.2, -6.5};
        float *alligned_llr;
        alligned_llr = (float*) volk_fec_malloc(sizeof(float)* 10, align);
        memcpy(alligned_llr, test_llr, sizeof(float)*10);

        float *gamma;
        gamma = (float*) volk_fec_malloc(sizeof(float)*20, align);

        float *alpha;
        alpha = (float*) volk_fec_malloc(sizeof(float)*40, align);
        for(int i = 1; i < 8; i++)
          alpha[i] = -INF;

        int OS[16] = {0, 3, 0, 3, 1, 2, 1, 2, 1, 2, 1, 2, 0, 3, 0, 3};
        int *aligned_shuffle;
        aligned_shuffle = (int*) volk_fec_malloc(sizeof(int)*16, align);

        int shuffle[16] = {0,1,1,0,1,0,0,1,0,1,1,0,1,0,0,1};

        memcpy(aligned_shuffle, shuffle, sizeof(int)*16); 

        printf("\nRunning Test: Gamma Calculations\n");
        volk_fec_32f_s32f_32i_calc_branch_metric_32f_manual(alligned_llr, 
                                                            2,
                                                            gamma,
                                                            5,
                                                            "generic");
        volk_fec_32f_x2_s32f_32i_x2_forward_recursion_32f_manual(alpha, gamma,
                                                        5, &OS[0], aligned_shuffle,
                                                        8, "generic");

        for (int i = 0; i < 20; ++i)
        {
          printf("Gamma[%d] %f\n", i, gamma[i]);
        }
        printf("\n");
        for (int i = 0; i < 40; ++i)
        {
          printf("Alpha[%d] %f\n", i, alpha[i]);
        }

        volk_fec_free(alpha);
        volk_fec_free(aligned_shuffle);
        volk_fec_free(gamma);
        volk_fec_free(alligned_llr);
    }

  } /* namespace celec */
} /* namespace gr */

