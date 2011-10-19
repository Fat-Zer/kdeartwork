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

add_subdirectory( kdesavers )
add_subdirectory( kpartsaver)
if( WITH_XSCREENSAVER )
  add_subdirectory( kxsconfig )
  add_subdirectory( xsavers )
endif( WITH_XSCREENSAVER )
