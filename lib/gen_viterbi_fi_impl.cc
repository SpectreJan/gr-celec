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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "gen_viterbi_fi_impl.h"
#include <stdio.h>

namespace gr {
  namespace celec {

    gen_viterbi_fi::sptr
    gen_viterbi_fi::make(const int n, const int k, const int frame_size, 
                         const int start_state, const  int end_state,  
                         const std::vector<gr_complex> &Table,
                         const std::vector<int> &OS)
    {
      return gnuradio::get_initial_sptr
        (new gen_viterbi_fi_impl(n, k, frame_size, 
                                 start_state, end_state,
                                 Table, OS));
    }

    /*
     * The private constructor
     */
    gen_viterbi_fi_impl::gen_viterbi_fi_impl(const int n, const int k,
                                             const int frame_size, const int start_state,
                                             const int end_state, 
                                             const std::vector<gr_complex> &Table,
                                             const std::vector<int> &OS)
      : gr::block("gen_viterbi_fi",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(unsigned char))),
        d_n(n), // Number of Codebits in Codeword
        d_s(1<<(k-1)), // Number of Encoderstates
        d_k(k),   // constraint length
        d_frame_size(frame_size), // Trellislength
        d_start_state(start_state), // Initial State
        d_end_state(end_state), // Termination State
        d_OS(OS), // Output Matrix
        d_Table(Table)
    {
      set_output_multiple(d_frame_size); // Make sure that output is multiple of the packet length
      //set_relative_rate(1/((1<<d_n)));
    }

    /*
     * Our virtual destructor.
     */
    gen_viterbi_fi_impl::~gen_viterbi_fi_impl()
    {
    }

    void
    gen_viterbi_fi_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        ninput_items_required[0] = noutput_items;
    }

    int
    gen_viterbi_fi_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        const gr_complex *in = (gr_complex*) input_items[0];
        unsigned char *out = (unsigned char *) output_items[0];

        //printf("Nooutput length: %d\n", noutput_items);
        int nblocks = noutput_items/d_frame_size; // Number of complete Packets 
        int ncode_words = 1<<d_n; 
        for(int n = 0; n < nblocks; n++)
        {
          viterbi_fi::viterbi_fi(d_s, d_k, ncode_words, d_frame_size, 0, 
                                 d_end_state, d_OS, d_Table, 
                                 &in[n*d_frame_size], out);
        }

        // Tell runtime system how many input items we consumed on
        // each input stream.
        consume_each (noutput_items);

        // Tell runtime system how many output items we produced.
        return noutput_items;
    }

  } /* namespace celec */
} /* namespace gr */

