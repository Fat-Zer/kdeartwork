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

if( WITH_OPENGL )
  pkg_search_module( OPENGL opengl )
  if( NOT OPENGL_FOUND )
    tde_message_fatal( "opengl is required, but was not found on your system" )
  endif( )
endif( )

# required stuff
find_package( TQt )
find_package( TDE )
