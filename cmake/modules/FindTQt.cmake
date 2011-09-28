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

macro( tqt_message )
  message( STATUS "${ARGN}" )
endmacro( )

pkg_search_module( TQT tqt )

if( NOT TQT_FOUND )
  tde_message_fatal( "Unable to find tqt!\n Try adding the directory in which the tqt.pc file is located\nto the PKG_CONFIG_PATH variable." )
endif( )

# tmoc_executable
execute_process(
  COMMAND pkg-config tqt --variable=tmoc_executable
  OUTPUT_VARIABLE TMOC_EXECUTABLE OUTPUT_STRIP_TRAILING_WHITESPACE )

if( TMOC_EXECUTABLE )
  tqt_message( "  tmoc path: ${TMOC_EXECUTABLE}" )
else( )
  tde_message_fatal( "Path to tmoc is not set.\n tqt is correctly installed?" )
endif( )


# moc_executable
execute_process(
  COMMAND pkg-config tqt --variable=moc_executable
  OUTPUT_VARIABLE MOC_EXECUTABLE OUTPUT_STRIP_TRAILING_WHITESPACE )

if( MOC_EXECUTABLE )
  tqt_message( "  moc path: ${MOC_EXECUTABLE}" )
else( )
  tde_message_fatal( "Path to moc is not set.\n tqt is correctly installed?" )
endif( )


# uic_executable
execute_process(
  COMMAND pkg-config tqt --variable=uic_executable
  OUTPUT_VARIABLE UIC_EXECUTABLE OUTPUT_STRIP_TRAILING_WHITESPACE )

if( UIC_EXECUTABLE )
  tqt_message( "  uic path: ${UIC_EXECUTABLE}" )
else( )
  tde_message_fatal( "Path to uic is not set.\n tqt is correctly installed?" )
endif( )


# check if tqt is usable
tde_save( CMAKE_REQUIRED_INCLUDES CMAKE_REQUIRED_LIBRARIES )
set( CMAKE_REQUIRED_INCLUDES ${TQT_INCLUDE_DIRS} )
foreach( _dirs ${TQT_LIBRARY_DIRS} )
  list( APPEND CMAKE_REQUIRED_LIBRARIES "-L${_dirs}" )
endforeach()
list( APPEND CMAKE_REQUIRED_LIBRARIES ${TQT_LIBRARIES} )

check_cxx_source_compiles("
    #include <tqapplication.h>
    int main(int argc, char **argv) { TQApplication app(argc, argv); return 0; } "
  HAVE_USABLE_TQT )

if( NOT HAVE_USABLE_TQT )
  tde_message_fatal( "Unable to build a simple tqt test." )
endif( )

tde_restore( CMAKE_REQUIRED_INCLUDES CMAKE_REQUIRED_LIBRARIES )


# TQT_CXX_FLAGS
foreach( _flag ${TQT_CFLAGS_OTHER} )
  set( TQT_CXX_FLAGS "${TQT_CXX_FLAGS} ${_flag}" )
endforeach()
