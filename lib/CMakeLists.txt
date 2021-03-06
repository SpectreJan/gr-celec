# Copyright 2011,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX

include_directories(${Boost_INCLUDE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}/../include
                    ${GR_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})
list(APPEND celec_sources
    viterbi.cc
    bcjr.cc
    gen_viterbi_fi_impl.cc
    metric_c_impl.cc
    max_log_map_f_impl.cc )

set(celec_sources "${celec_sources}" PARENT_SCOPE)
if(NOT celec_sources)
	MESSAGE(STATUS "No C++ sources... skipping lib/")
	return()
endif(NOT celec_sources)

add_library(gnuradio-celec SHARED ${celec_sources})
target_link_libraries(gnuradio-celec ${Boost_LIBRARIES} ${GNURADIO_ALL_LIBRARIES} /usr/local/lib/libvolk_fec.so)
set_target_properties(gnuradio-celec PROPERTIES DEFINE_SYMBOL "gnuradio_celec_EXPORTS")

if(APPLE)
    set_target_properties(gnuradio-celec PROPERTIES
        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    )
endif(APPLE)

########################################################################
# Install built library files
########################################################################
install(TARGETS gnuradio-celec
    LIBRARY DESTINATION lib${LIB_SUFFIX} # .so/.dylib file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin              # .dll file
)

########################################################################
# Build and register unit test
########################################################################
include(GrTest)

include_directories(${CPPUNIT_INCLUDE_DIRS})
list(APPEND test_celec_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/test_celec.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_celec.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_max_log_map_f.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_metric_c.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gen_viterbi_fi.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/)

add_executable(test-celec ${test_celec_sources})

target_link_libraries(
  test-celec
  ${GNURADIO_RUNTIME_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  gnuradio-celec
  /usr/local/lib/libvolk_fec.so
)

GR_ADD_TEST(test_celec test-celec)
