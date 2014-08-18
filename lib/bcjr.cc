/* -*- c++ -*- */
/* 
 * Copyright 2014 Jan Krämer.
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

#include <stdio.h>
#include <celec/bcjr.h>
#include <volk_fec/volk_fec.h>

namespace gr{
  namespace celec{
    namespace bcjr_f{
      /*! \brief Basic BCJR algorithm that uses volk_fec library
          \param S Number of States
          \param k Constraint length
          \param n Number of codebits in codeword
          \param K Packetlength (Infobit)
          \param S0 Start State
          \param SK End State
          \param shuffle Shuffle Matrix for recursive codes
          \param OS Encoder Outputs
          \param dp Decoder struct
          \param in Input LLR
          \param out Output LLR
      */

      void bcjr_f(const int S, const int k, const int n, const int K, 
                  const int S0, int SK, const std::vector<int> &shuffle,
                  const std::vector<int> &OS, decoder *dp,
                  const float *in, float *out)
      {
        volk_fec_32f_s32f_32i_calc_branch_metric_32f_manual(in, n,
                                                            &dp->gamma[0], K, 
                                                            "a_sse4");
        volk_fec_32f_x2_s32f_32i_x2_forward_recursion_32f_manual(dp->alpha,
                                              dp->gamma, K, &OS[0], 
                                              &shuffle[0],
                                              S, "a_sse4");
        volk_fec_32f_x4_32i_x4_llr_codebits_32f_manual(dp->alpha, dp->gamma,
                                                       dp->beta, out,
                                                       n, K, &OS[0], &shuffle[0],
                                                       S, "a_sse4");
      }
    }
  }
}