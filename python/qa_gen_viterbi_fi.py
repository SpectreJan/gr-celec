#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2014 Jan Krämer.
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
from gnuradio import blocks, digital, analog
import celec_swig as celec
import numpy

class qa_gen_viterbi_fi (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # Trellislength
        K = 64
        # Setup dummy data
        data = numpy.random.randint(0,2, K*4)
        sym_table = digital.constellation_qpsk()
        expected_data = numpy.linspace(0,K-1,K) 

        # Setup blocks
        data_src = blocks.vector_source_f(map(int, data))
        src_head = blocks.head(gr.sizeof_float*1, K*4)

        # TX Sim
        # encoder = trellis.encoder_ss(trellis.fsm(*fsm), 0)
        # modulator = digital.chunks_to_symbols_sc(sym_table.points(), 1)
        
        # Decoder
        viterbi_cel = celec.gen_viterbi_fi(2, 8, K, 1,-1)

        # Sinks
        #tx_sink = blocks_vector_sink_s(1)
        rx_sink = blocks.vector_sink_i(1)
        
        # Connections
        self.tb.connect(data_src, src_head)
        self.tb.connect(src_head, viterbi_cel)
        self.tb.connect(viterbi_cel, rx_sink)
        self.tb.run ()
        
         
        # Check data
        rx_output = numpy.array(rx_sink.data())
        #print rx_output
        self.assertEqual(map(int, rx_output), map(int, expected_data))

if __name__ == '__main__':
    gr_unittest.run(qa_gen_viterbi_fi, "qa_gen_viterbi_fi.xml")
