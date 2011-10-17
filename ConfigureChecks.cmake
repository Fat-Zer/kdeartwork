#################################################
#
#  (C) 2011 Golubev Alexander
#  fatzer2 (AT) gmail.com
#
#  Improvements and feedback are welcome
#
#  This file is released under GPL >= 2
#
#################################################

# limits.h (kscreensaver/kdesavers) 
if( BUILD_KSCREENSAVER )
	check_include_file_cxx( limits HAVE_NUMERIC_LIMITS )
endif( )

# OpenGL(kscreensaver/kdesavers)
if( WITH_OPENGL )
  find_package( OpenGL REQUIRED )
  if( NOT OPENGL_FOUND )
    tde_message_fatal( "OpenGL is required, but was not found on your system" )
  endif( NOT OPENGL_FOUND  )
endif( WITH_OPENGL )

# libart(kscreensaver/kdesavers)
if( WITH_LIBART )
  pkg_search_module( LIBART libart libart_lgpl libart-2.0 )
  if( NOT LIBART_FOUND )
    tde_message_fatal( "libart is required, but was not found on your system" )
  endif( NOT LIBART_FOUND  )
endif( WITH_LIBART )

# arts(kscreensaver/kdesavers)
if( WITH_ARTS )
  pkg_search_module( ARTS arts )
  if( NOT ARTS_FOUND )
    message( FATAL_ERROR "\naRts is requested, but was not found on your system" )
  endif( )
endif( WITH_ARTS )

# required stuff
find_package( TQt )
find_package( TDE )
