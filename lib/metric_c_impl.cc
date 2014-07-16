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
#include "metric_c_impl.h"
 #include <volk_fec/volk_fec.h>

namespace gr {
  namespace celec {

    metric_c::sptr
    metric_c::make(int O, const std::vector<gr_complex> &TABLE)
    {
      return gnuradio::get_initial_sptr
        (new metric_c_impl(O, TABLE));
    }

    /*
     * The private constructor
     */
    metric_c_impl::metric_c_impl(int O, const std::vector<gr_complex> &TABLE)
      : gr::block("metric_c",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(float))),
      d_O(O),
      d_Table(TABLE)
    {
      set_relative_rate (1.0 * d_O);
      set_output_multiple (d_O);      
    }

    /*
     * Our virtual destructor.
     */
    metric_c_impl::~metric_c_impl()
    {
    }

    void
    metric_c_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        ninput_items_required[0] = noutput_items/d_O;
    }

    int
    metric_c_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        const gr_complex *in = (const gr_complex *) input_items[0];
        float *out = (float *) output_items[0];
        volk_fec_32fc_x2_calc_euclidean_metric_32f_manual(&(out[0]), &(in[0]), 
                                             &d_Table[0], d_O, 
                                             noutput_items/d_O, "generic");
        consume_each (noutput_items/d_O);        

        // Tell runtime system how many output items we produced.
        return noutput_items;
    }

  } /* namespace celec */
} /* namespace gr */

