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

namespace gr {
  namespace celec {

    gen_viterbi_fi::sptr
    gen_viterbi_fi::make(int N, int S, int K, int S0, int SK, int *OS)
    {
      return gnuradio::get_initial_sptr
        (new gen_viterbi_fi_impl(N, S, K, S0, SK, OS));
    }

    /*
     * The private constructor
     */
    gen_viterbi_fi_impl::gen_viterbi_fi_impl(int N, int S, int K, int S0, int SK, int *OS)
      : gr::block("gen_viterbi_fi",
              gr::io_signature::make(1, 1, sizeof(float)),
              gr::io_signature::make(1, 1, sizeof(unsigned int))),
        d_N(N), // Number of Codebits in Codeword
        d_S(S), // Number of Encoderstates
        d_K(K), // Trellislength
        d_S0(S0), // Initial State
        d_SK(SK), // Termination State
        d_OS(OS) // Output Matrix
    {
      
      set_output_multiple(d_K);
      set_relative_rate(1/(1<<d_N));
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
        ninput_items_required[0] = noutput_items * (1<<d_N);
    }

    int
    gen_viterbi_fi_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        const float *in = (float*) input_items[0];
        unsigned int *out = (unsigned int *) output_items[0];

        int nblocks = noutput_items / d_K; // Number of complete Packets  
        for(int n = 0; n < nblocks; n++)
        {
          viterbi_fi::viterbi_fi(d_S, d_K, 0, d_OS, in, out);
        }
        // Tell runtime system how many input items we consumed on
        // each input stream.
        consume_each (noutput_items);

        // Tell runtime system how many output items we produced.
        return noutput_items;
    }

  } /* namespace celec */
} /* namespace gr */

