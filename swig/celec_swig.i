/* -*- c++ -*- */

#define CELEC_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "celec_swig_doc.i"

%{
#include "celec/gen_viterbi_fi.h"
#include "celec/metric_c.h"
%}

%include "celec/gen_viterbi_fi.h"
GR_SWIG_BLOCK_MAGIC2(celec, gen_viterbi_fi);
%include "celec/metric_c.h"
GR_SWIG_BLOCK_MAGIC2(celec, metric_c);
