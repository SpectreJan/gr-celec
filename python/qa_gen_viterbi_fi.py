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
from gnuradio import blocks, digital, analog, trellis
import celec_swig as celec
import numpy


fsm_args = {"awgn1o2_8": (2, 8, 4,
        (0, 4, 0, 4, 1, 5, 1, 5, 2, 6, 2, 6, 3, 7, 3, 7),
        (0, 3, 3, 0, 1, 2, 2, 1, 3, 0, 0, 3, 2, 1, 1, 2),
        ), 
      "awgn1o2_16": ( 2,16,4,
        (0, 8, 0, 8, 1, 9, 1, 9, 2, 10, 2, 10, 3, 11, 3, 11, 4, 12, 4, 12, 5, 13, 5, 13, 6, 14, 6, 14, 7, 15, 7, 15),
        (0, 3, 3, 0, 1, 2, 2, 1, 1, 2, 2, 1, 0, 3, 3, 0, 2, 1, 1, 2, 3, 0, 0, 3, 3, 0, 0, 3, 2, 1, 1, 2)),
        }  

class qa_gen_viterbi_fi (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # Trellislength
        K = 2044
        fsm = fsm_args["awgn1o2_16"]

        os = numpy.array(fsm[4], dtype=int) 
        # Setup dummy data
        #data = [1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0]
        #data = numpy.random.randint(0,2,K)
        #termination = numpy.zeros(4)
        #data = numpy.concatenate((data, termination),1)

        throt = blocks.throttle(1, 32000)
        #data = numpy.array([1,1,0,1,1,0,1,1])
        sym_table = digital.constellation_qpsk()
        #print sym_table.points()
        # Setup blocks
        #data_src = blocks.vector_source_s(map(int, data))
        data_src = blocks.file_source(1, "SG.jpeg", 0)
        #src_head = blocks.head(gr.sizeof_short*1, K)
        # TX Sim
        p2up = blocks.packed_to_unpacked_bb(1, 0)
        encoder = trellis.encoder_bb(trellis.fsm(*fsm), 0)
        modulator = digital.chunks_to_symbols_bc(sym_table.points(), 1)

        # Decoder
        viterbi_cel = celec.gen_viterbi_fi(2, 5, 9317*8, 0, -1, 
                                           sym_table.points(), os)

        # Sinks
        up2p = blocks.unpacked_to_packed_bb(1, 0)
        tx_sink = blocks.vector_sink_s(1)
        rx_sink = blocks.vector_sink_b(1)
        file_sink = blocks.file_sink(1, "decoded_SG.jpeg")
        # Connections
        self.tb.connect(data_src, throt, p2up, encoder)
        self.tb.connect(encoder, modulator)
        self.tb.connect(modulator, viterbi_cel)
        self.tb.connect(viterbi_cel, up2p, file_sink)
        #self.tb.connect(src_head, tx_sink);
        self.tb.run ()
        
        # # Check data
        #rx_output = numpy.array(rx_sink.data())
        #for k in range(0, K):
        #    self.assertEqual(int(rx_output[k]), int(data[k]))

if __name__ == '__main__':
    gr_unittest.run(qa_gen_viterbi_fi, "qa_gen_viterbi_fi.xml")
