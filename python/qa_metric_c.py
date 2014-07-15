#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2014 <+YOU OR YOUR COMPANY+>.
# 
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

from gnuradio import gr, gr_unittest
from gnuradio import blocks, digital
import celec_swig as celec
import numpy

class qa_metric_c (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        K = 32
        table = [1+0j, 0+1j, -1+0j, 0-1j]
        data = numpy.zeros(K)
        expected_data = numpy.ones(K)

        data_src = blocks.vector_source_c(data)
        src_head = blocks.head(gr.sizeof_gr_complex*1, K)

        metrics = celec.metric_c(4, table)

        sink = blocks.vector_sink_f(1)

        self.tb.connect(data_src, src_head, metrics, sink);
        self.tb.run ()

        rx_output = numpy.array(sink.data())
        for k in range(0, K):
            #self.assertEqual(float(expected_data), float(rx_output))        
            print rx_output
        # check data


if __name__ == '__main__':
    gr_unittest.run(qa_metric_c, "qa_metric_c.xml")
