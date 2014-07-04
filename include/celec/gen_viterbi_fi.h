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


#ifndef INCLUDED_CELEC_GEN_VITERBI_FI_H
#define INCLUDED_CELEC_GEN_VITERBI_FI_H

#include <celec/api.h>
#include <gnuradio/block.h>
#include <celec/viterbi.h>

namespace gr {
  namespace celec {

    /*!
     * \brief <+description of block+>
     * \ingroup celec
     *
     */
    class CELEC_API gen_viterbi_fi : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<gen_viterbi_fi> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of celec::gen_viterbi_fi.
       *
       * To avoid accidental use of raw pointers, celec::gen_viterbi_fi's
       * constructor is in a private implementation
       * class. celec::gen_viterbi_fi::make is the public interface for
       * creating new instances.
       */
      static sptr make(int N, int S, int K, int S0, int SK, int *OS);
    };

  } // namespace celec
} // namespace gr

#endif /* INCLUDED_CELEC_GEN_VITERBI_FI_H */

