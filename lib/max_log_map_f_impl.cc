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

#define INF 1e9

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "max_log_map_f_impl.h"
#include <celec/bcjr.h>
#include <stdio.h>
#include <volk_fec/volk_fec.h>
#include <math.h>
 
namespace gr {
  namespace celec {

    max_log_map_f::sptr
    max_log_map_f::make(const int n, const int k, const int frame_size, 
                        const int start_state, const int end_state, 
                        const char produce_bits, const std::vector<int> &OS)
    {
      return gnuradio::get_initial_sptr
        (new max_log_map_f_impl(n, k, frame_size, start_state, end_state, 
                                produce_bits, OS));
    }

    /*
     * The private constructor
     */
    max_log_map_f_impl::max_log_map_f_impl(const int n, const int k, 
                                           const int frame_size, 
                                           const int start_state, 
                                           const int end_state, 
                                           const char produce_bits, 
                                           const std::vector<int> &OS)
      : gr::block("max_log_map_f",
              gr::io_signature::make(1, 1, sizeof(float)),
              gr::io_signature::make(1, 1, sizeof(float))),
        d_n(n), // Number of Codebits in Codeword
        d_s(1<<(k-1)), // Number of Encoderstates
        d_k(k),   // constraint length
        d_frame_size(frame_size), // Trellislength
        d_start_state(start_state), // Initial State
        d_end_state(end_state), // Termination State
        d_produce_bits(produce_bits), // Produce codebit LLR => d_produce_bits == 1
        d_OS(OS) // Output Matrix
    {
      // Set scheduler behaviour, we want Blocks of predefined length as we are doing truncated codes
      set_output_multiple(d_frame_size);
      size_t align = volk_fec_get_alignment();
      
      int shuffle[16] = {0,1,1,0,1,0,0,1,0,1,1,0,1,0,0,1};
      // Set all neccessary metrics
      d_dp = new struct decoder;
      d_dp->gamma = (float*) volk_fec_malloc(sizeof(float)*(1<<d_n)*(d_frame_size), align);
      d_dp->alpha = (float*) volk_fec_malloc(sizeof(float)*d_s*(d_frame_size+1), align);
      d_dp->beta = (float*) volk_fec_malloc(sizeof(float)*d_s*(d_frame_size+1), align);
      
      memcpy(d_dp->shuffle, shuffle, sizeof(int)*16);

      float log_equal = -log(d_s);
      printf("d_s: %d    log_equal: %f\n", d_s, log_equal);
      for(int i = 0; i < d_s; i++)
      {
        d_dp->alpha[i] = -INF;
        d_dp->beta[d_s*d_frame_size+i] = log_equal;
      }
      d_dp->alpha[d_start_state] = 0;
    }

    /*
     * Our virtual destructor.
     */
    max_log_map_f_impl::~max_log_map_f_impl()
    {
      volk_fec_free(d_dp->gamma);
      volk_fec_free(d_dp->alpha);
      volk_fec_free(d_dp->beta);

      delete d_dp;
    }

    void
    max_log_map_f_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      // Demand enough samples from the scheduler to produce output (Hopefully the scheduler obeys to this command!)
      ninput_items_required[0] = noutput_items;
    }

    int
    max_log_map_f_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        const float *in = (const float *) input_items[0];
        float *out = (float *) output_items[0];
        printf("hey noutput_items: %d\n", noutput_items);
        
        int nblocks = noutput_items / d_frame_size;
        printf("hey nblocks: %d\n", nblocks);
        for(int i = 0; i < nblocks; i++)
        { 
          bcjr_f::bcjr_f(d_s, d_k, d_n, d_frame_size, 
                         d_start_state, d_end_state, d_OS,
                         d_dp, 
                         &in[i*d_frame_size*(d_produce_bits*d_n)], // Get to right position in LLR Buffer
                         &out[i*d_frame_size]);    // Get to the right position in 
        }
        // Tell runtime system how many input items we consumed on
        // each input stream.
        consume_each (noutput_items);

        // Tell runtime system how many output items we produced.
        return noutput_items;
    }

  } /* namespace celec */
} /* namespace gr */

