INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_CELEC celec)

FIND_PATH(
    CELEC_INCLUDE_DIRS
    NAMES celec/api.h
    HINTS $ENV{CELEC_DIR}/include
        ${PC_CELEC_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    CELEC_LIBRARIES
    NAMES gnuradio-celec
    HINTS $ENV{CELEC_DIR}/lib
        ${PC_CELEC_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CELEC DEFAULT_MSG CELEC_LIBRARIES CELEC_INCLUDE_DIRS)
MARK_AS_ADVANCED(CELEC_LIBRARIES CELEC_INCLUDE_DIRS)

