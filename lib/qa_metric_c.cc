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
#include "qa_metric_c.h"
#include <celec/metric_c.h>
#include <volk_fec/volk_fec.h>
#include <gnuradio/random.h>

namespace gr {
  namespace celec {

    void
    qa_metric_c::t1()
    {
      int num_symbols = 128;
      int O = 4;
      gr_complex symbols[] = {0+1j,0-1j, 1+0j, -1+0j};
      int num_runs = 1;      
      const std::vector<gr_complex> table(symbols, symbols + sizeof(symbols) / sizeof(gr_complex));
      size_t align = volk_fec_get_alignment();

      gr_complex *rx_samples;
      rx_samples = (gr_complex*) volk_fec_malloc(sizeof(gr_complex)*num_symbols, align);
      for(int i = 0; i < num_symbols; i++)
      {
        // Create random samples ranging from -4 to 4
        float re = (float) (((float)::random()/RANDOM_MAX-0.5)*10);
        float im = (float) (((float)::random()/RANDOM_MAX-0.5)*10);
        rx_samples[i] = gr_complex(re,im);
        //printf("%f + j%f ", rx_samples[i].real(), rx_samples[i].imag());
      }

      printf("\n\n");
      // Allocate metric vectors
      float *metrics;
      metrics = (float*) volk_fec_malloc(sizeof(float)*num_symbols*O, align);
      float *metrics_volk;
      metrics_volk = (float*) volk_fec_malloc(sizeof(float*)*num_symbols*O, align);
      float time_volk = 0; 
      float time_generic = 0;   

      printf("Testing volk_32fc_x2_calc_euclidean_metric_32f with sse4\n\n");
      // Calculate actual metrics
      for(int i = 0; i < num_runs; i++)
      {
        clock_t t_volk, t;
        t = clock();
        volk_fec_32fc_x2_calc_euclidean_metric_32f_manual(metrics, rx_samples, 
                                      &table[0], O, num_symbols, "generic");
        t = clock() - t;
        time_generic += ((float) t);

        t_volk = clock();
        volk_fec_32fc_x2_calc_euclidean_metric_32f_manual(metrics_volk, rx_samples, 
                                      &table[0], O, num_symbols, "a_sse4");
        t_volk = clock() - t_volk;
        time_volk += ((float) t_volk);
        for(int i = 0; i < num_symbols*O; i++)
        {
          CPPUNIT_ASSERT_DOUBLES_EQUAL(metrics[i], metrics_volk[i],
              1e-4);
        }
      }
      printf("Time for sse4 : %f\n", time_volk/CLOCKS_PER_SEC/num_runs);
      printf("Time for generic: %f\n", time_generic/CLOCKS_PER_SEC/num_runs);

      volk_fec_free(rx_samples);
      volk_fec_free(metrics_volk);
      volk_fec_free(metrics);      
    }

  } /* namespace celec */
} /* namespace gr */

