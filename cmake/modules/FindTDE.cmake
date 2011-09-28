#################################################
#
#  (C) 2010-2011 Serghei Amelian
#  serghei (DOT) amelian (AT) gmail.com
#
#  Improvements and feedback are welcome
#
#  This file is released under GPL >= 2
#
#################################################

if( NOT TDE_FOUND )

  message( STATUS "checking for 'TDE'")

  # if the path is not already defined by user,
  # find kde-config executable
  if( NOT DEFINED KDECONFIG_EXECUTABLE )
    find_program( KDECONFIG_EXECUTABLE
      NAMES kde-config
      HINTS ${BIN_INSTALL_DIR} )
    if( NOT KDECONFIG_EXECUTABLE )
      tde_message_fatal( "kde-config are NOT found." )
    endif( NOT KDECONFIG_EXECUTABLE )
  endif( NOT DEFINED KDECONFIG_EXECUTABLE )

  # check for installed trinity version
  execute_process(
    COMMAND ${KDECONFIG_EXECUTABLE} --version
    OUTPUT_VARIABLE _version
    RESULT_VARIABLE _result
    OUTPUT_STRIP_TRAILING_WHITESPACE )
  if( _result )
    tde_message_fatal( "Unable to run kde-config!\n KDELIBS are correctly installed?\n Path to kde-config are corect?" )
  endif( _result )

  # parse kde-config output, to extract TDE version
  string( REGEX MATCH "KDE: ([0-9\\.]+)" __dummy "${_version}" )
  set( TDE_VERSION "${CMAKE_MATCH_1}" )

  # ask kde-config for few paths
  macro( __internal_get_path __type __var )
    execute_process(
      COMMAND ${KDECONFIG_EXECUTABLE} --expandvars --install ${__type}
      OUTPUT_VARIABLE ${__var}
      OUTPUT_STRIP_TRAILING_WHITESPACE )
  endmacro( __internal_get_path )

  __internal_get_path( include TDE_INCLUDE_DIR )
  __internal_get_path( lib TDE_LIB_DIR )
  __internal_get_path( exe TDE_BIN_DIR )
  __internal_get_path( data TDE_DATA_DIR )
  __internal_get_path( config TDE_CONFIG_DIR )
  __internal_get_path( html TDE_HTML_DIR )
  __internal_get_path( cmake TDE_CMAKE_DIR )
  __internal_get_path( qtplugins TDE_QTPLUGINS_DIR )

  # find kde tools
  macro( __internal_find_program __prog __var )
    find_program( ${__var}
      NAMES ${__prog}
      HINTS ${TDE_BIN_INSTALL_DIR}
      OUTPUT_STRIP_TRAILING_WHITESPACE )
    if( NOT ${__var} )
      tde_message_fatal( "${__prog} are NOT found.\n KDELIBS are correctly installed?" )
    endif( NOT ${__var} )
  endmacro( __internal_find_program )

  __internal_find_program( dcopidl KDE3_DCOPIDL_EXECUTABLE )
  __internal_find_program( dcopidlng KDE3_DCOPIDLNG_EXECUTABLE )
  __internal_find_program( dcopidl2cpp KDE3_DCOPIDL2CPP_EXECUTABLE )
  __internal_find_program( meinproc KDE3_MEINPROC_EXECUTABLE )
  __internal_find_program( kconfig_compiler KDE3_KCFGC_EXECUTABLE )
  __internal_find_program( makekdewidgets KDE3_MAKEKDEWIDGETS_EXECUTABLE )

  # dcopidlng is a bash script which using kde-config;
  # if PATH to kde-config is not set, dcopidlng will fail;
  # for this reason we set KDECONFIG environment variable before running dcopidlng
  set( KDE3_DCOPIDLNG_EXECUTABLE env KDECONFIG=${KDECONFIG_EXECUTABLE} ${KDE3_DCOPIDLNG_EXECUTABLE}
    CACHE INTERNAL KDE3_DCOPIDLNG_EXECUTABLE FORCE )

  message( STATUS "  found 'TDE', version ${TDE_VERSION}" )

endif( NOT TDE_FOUND )

include( "${TDE_CMAKE_DIR}/kdelibs.cmake" )
