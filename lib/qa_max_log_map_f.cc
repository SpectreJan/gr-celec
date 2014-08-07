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
#include <gnuradio/random.h>



namespace gr {
  namespace celec {

    void
    qa_max_log_map_f::t1()
    {
        // System parameters
        int INF = 1e9;
        size_t align = volk_fec_get_alignment();
        
        // Allocate testdata and decoder->parameters
        float test_llr[10] = {-5,-5,-5,-5,5,5,-5,5,5,-5};
        float *alligned_llr;
        alligned_llr = (float*) volk_fec_malloc(sizeof(float)* 10, align);
        memcpy(alligned_llr, test_llr, sizeof(float)*10);

        float *gamma;
        gamma = (float*) volk_fec_malloc(sizeof(float)*20, align);

        float *alpha;
        alpha = (float*) volk_fec_malloc(sizeof(float)*24, align);
        for(int i = 1; i < 4; i++)
          alpha[i] = -INF;
        alpha[0] = 0;

        float *beta;
        beta = (float*) volk_fec_malloc(sizeof(float)*8, align);
        for(int i = 0; i < 3; i++)
          beta[i] = 0;
        beta[3] = 0;

        float *llr_out;
        llr_out = (float*) volk_fec_malloc(sizeof(float)*10, align);

        // Encoder outputs in format OS[State*number of encoder outputs + current output]
        int OS[16] = {0, 3, 0, 3, 1, 2, 1, 2};
        
        // Shufflevector for transitions 
        int *aligned_shuffle;
        aligned_shuffle = (int*) volk_fec_malloc(sizeof(int)*16, align);

        int shuffle[16] = {0,1,1,0,1,0,0,1};
        memcpy(aligned_shuffle, shuffle, sizeof(int)*16); 

        printf("\nRunning Test: Max-Log-Map\n");

        volk_fec_32f_s32f_32i_calc_branch_metric_32f_manual(alligned_llr, 
                                                            2,
                                                            gamma,
                                                            5,
                                                            "generic");
        
        volk_fec_32f_x2_s32f_32i_x2_forward_recursion_32f_manual(alpha, gamma,
                                                        5, &OS[0], aligned_shuffle,
                                                        4, "generic");
        
        volk_fec_32f_x4_32i_x4_llr_codebits_32f_manual(alpha, &gamma[0], beta, llr_out, 
                                                    2, 5, &OS[0], aligned_shuffle, 4, "generic");

        for (int i = 0; i < 10; ++i)
        {
          printf("LLR[%d] %f\n", i, llr_out[i]);
        }

        /*for (int i = 0; i < 20; ++i)
        {
          printf("Gamma[%d] %f\n", i, gamma[i]);
        }
        printf("\n");
        /*for (int i = 0; i < 24; ++i)
        {
          printf("Alpha[%d] %f\n", i, alpha[i]);
        }*/

        volk_fec_free(beta);
        volk_fec_free(llr_out);
        volk_fec_free(alpha);
        volk_fec_free(aligned_shuffle);
        volk_fec_free(gamma);
        volk_fec_free(alligned_llr);
    }

    // Test transition probabilities calculations
    void
    qa_max_log_map_f::t2()
    {
      // Set parameters and allocate memory
      int num_runs = 1000;
      int N = 6144; // Number of input LLRs
      int n_cbit = 2; // Number of Codebits, must be at least 2

      size_t align = volk_fec_get_alignment();
      float *input_llr;
      input_llr = (float*) volk_fec_malloc(sizeof(float)*N, align);

      float *log_prob_gen;
      log_prob_gen = (float*) volk_fec_malloc(sizeof(float)*N*(1<<n_cbit), align);

      float *log_prob_volk;
      log_prob_volk = (float*) volk_fec_malloc(sizeof(float)*N*(1<<n_cbit), align);

      for(int n = 0; n < N; n++)
      {
        input_llr[n] = (float) (((float)::random()/RANDOM_MAX-0.5)*10);
      }

      float time_volk = 0; 
      float time_generic = 0; 
      clock_t t_volk, t;

      for(int i = 0; i < num_runs; i++)
      {
        t = clock();
        volk_fec_32f_s32f_32i_calc_branch_metric_32f_manual(input_llr, 
                                                            n_cbit,
                                                            log_prob_gen,
                                                            6,
                                                            "generic");      

        t = clock()-t;
        t_volk = clock();
        volk_fec_32f_s32f_32i_calc_branch_metric_32f_manual(&input_llr[0], 
                                                            n_cbit,
                                                            log_prob_volk,
                                                            6,
                                                            "a_sse4");
        t_volk = clock()-t_volk;
        time_volk += (float) t_volk;
        time_generic += (float) t;
       
        for(int i = 0; i < N*(1<<n_cbit); i++)
        {
          CPPUNIT_ASSERT_DOUBLES_EQUAL(log_prob_volk[i], log_prob_gen[i],
              1e-4);
        }
      }
      printf("Time for sse4 : %f\n", time_volk/CLOCKS_PER_SEC);
      printf("Time for generic: %f\n", time_generic/CLOCKS_PER_SEC);
      volk_fec_free(log_prob_volk);
      volk_fec_free(log_prob_gen);
      volk_fec_free(input_llr);
    }
    
    void
    qa_max_log_map_f::t3()
    {
      float INF = 1e9;
      int N = 6144;  // Codebits
      int iters = 1000;
      int S = 16;  // 16 States
      int O = 4;
      
      double time_volk = 0; 
      double time_generic = 0; 
      clock_t t_volk, t;
      
      size_t align = volk_fec_get_alignment();
      float *alpha_generic;
      alpha_generic = (float*)volk_fec_malloc(sizeof(float)*S*(N/2+1), align);
      
      float *alpha_sse;
      alpha_sse = (float*)volk_fec_malloc(sizeof(float)*(N/2+1)*S, align);

      for(int i = 0; i < S; i++)
      {  
        alpha_generic[i] = -INF;
        alpha_sse[i] = -INF;
      }  
      alpha_sse[0]     = 0;
      alpha_generic[0] = 0;

      float *in;
      in = (float*) volk_fec_malloc(sizeof(float)*N*O, align);
      for(int i = 0; i < N*O; i++)
      {  
        // Create random samples ranging from -4 to 4
        in[i] = (float) (((float)::random()/RANDOM_MAX)*5);
      }
      
      int OS[] = {0, 3, 3, 0, 1, 2, 2, 1, 1, 2, 2, 1, 0, 3, 3, 0, 2, 1, 1, 2, 3, 0, 0, 3, 3, 0, 0, 3, 2, 1, 1, 2};
      int shuffle[] = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1};

      // Shufflevector for transitions 
      int *aligned_shuffle;
      aligned_shuffle = (int*) volk_fec_malloc(sizeof(int)*32, align);
      memcpy(aligned_shuffle, shuffle, sizeof(int)*32);      

      for(int i = 0; i < iters; i++)
      {
        t = clock();
        volk_fec_32f_x2_s32f_32i_x2_forward_recursion_32f_manual(alpha_generic, &in[0],
                                                        N/2, &OS[0], aligned_shuffle,
                                                        S, "generic");
        t = clock()- t;
        
        t_volk = clock();
        volk_fec_32f_x2_s32f_32i_x2_forward_recursion_32f_manual(alpha_sse, &in[0],
                                                        N/2, &OS[0], aligned_shuffle,
                                                        S, "a_sse4");            
        t_volk = clock()-t_volk;
          for(int i = 0; i < (N/2+1)*S; i++)
          {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(alpha_generic[i], alpha_sse[i],
                1e-4);
            //printf("alpha_gen[%d] %f  alpha_sse[%d] %f\n", i,alpha_generic[i], i, alpha_sse[i]);
          }

        time_volk += (double) t_volk;
        time_generic += (double) t;       
      
      }
      printf("Time for sse4 : %f\n", time_volk/CLOCKS_PER_SEC/iters);
      printf("Time for generic: %f\n", time_generic/CLOCKS_PER_SEC/iters);

      volk_fec_free(alpha_generic);
      volk_fec_free(alpha_sse);
      volk_fec_free(in);
      volk_fec_free(aligned_shuffle);
    }

    void
    qa_max_log_map_f::t4()
    {
      float INF = 1e9;
      int N = 6144;  // Codebits
      int iters = 1000;
      int S = 16;  // 16 States
      int O = 4;
      
      double time_volk = 0; 
      double time_generic = 0; 
      clock_t t_volk, t;
      
      size_t align = volk_fec_get_alignment();
      float *beta_generic;
      beta_generic = (float*)volk_fec_malloc(sizeof(float)*S*(N/2+1), align);
      
      float *beta_sse;
      beta_sse = (float*)volk_fec_malloc(sizeof(float)*(N/2+1)*S, align);
      
      float *alpha;
      alpha = (float*) volk_fec_malloc(sizeof(float)*S*(N/2+1), align);

      float *llr_sse;
      llr_sse = (float*) volk_fec_malloc(sizeof(float)*N, align);

      float *llr_gen;
      llr_gen = (float*) volk_fec_malloc(sizeof(float)*N, align);

      for(int i = 0; i < S; i++)
      {  
        beta_generic[i] = 0;
        beta_sse[i] = 0;
        alpha[i] = -INF;
      }  
      alpha[0] = 0;

      for(int i = S; i < S*(N/2+1); i++)
      {  
        // Create random samples ranging from -4 to 4
        alpha[i] = (float) (((float)::random()/RANDOM_MAX)*5);
      }

      float *in;
      in = (float*) volk_fec_malloc(sizeof(float)*N*O, align);
      for(int i = 0; i < N*O; i++)
      {  
        // Create random samples ranging from -4 to 4
        in[i] = (float) (((float)::random()/RANDOM_MAX)*5);
      }
      
      int OS[] = {0, 3, 3, 0, 1, 2, 2, 1, 1, 2, 2, 1, 0, 3, 3, 0, 2, 1, 1, 2, 3, 0, 0, 3, 3, 0, 0, 3, 2, 1, 1, 2};
      int shuffle[] = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1};

      // Shufflevector for transitions 
      int *aligned_shuffle;
      aligned_shuffle = (int*) volk_fec_malloc(sizeof(int)*32, align);
      memcpy(aligned_shuffle, shuffle, sizeof(int)*32);      

      printf("\nRunning Backward Recursion and LLR tests\n");
      for(int i = 0; i < iters; i++)
      {
        t = clock();
        volk_fec_32f_x4_32i_x4_llr_codebits_32f_manual(alpha, &in[0], beta_generic, llr_gen,
                                                        2, N/2, &OS[0], aligned_shuffle,
                                                        S, "generic");
        t = clock()- t;
        
        t_volk = clock();
        volk_fec_32f_x4_32i_x4_llr_codebits_32f_manual(alpha, &in[0], beta_sse, llr_sse,
                                                        2, N/2, &OS[0], aligned_shuffle,
                                                        S, "a_sse4");            
        t_volk = clock()-t_volk;

        for(int i = 0; i < 2*S; i++)
        {
          CPPUNIT_ASSERT_DOUBLES_EQUAL(beta_generic[i], beta_sse[i],
              1e-4);
        }

        for(int i = 0; i < N; i++)
        {
          CPPUNIT_ASSERT_DOUBLES_EQUAL(llr_sse[i], llr_gen[i],
              1e-4);
        }

        time_volk += (double) t_volk;
        time_generic += (double) t;       
      
      }
      
      printf("Time for sse4 : %f\n", time_volk/CLOCKS_PER_SEC/iters);
      printf("Time for generic: %f\n", time_generic/CLOCKS_PER_SEC/iters);

      volk_fec_free(beta_generic);
      volk_fec_free(beta_sse);
      volk_fec_free(llr_gen);
      volk_fec_free(llr_sse);
      volk_fec_free(alpha);
      volk_fec_free(in);
      volk_fec_free(aligned_shuffle);
    }

  } /* namespace celec */
} /* namespace gr */

