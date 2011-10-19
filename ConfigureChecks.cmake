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

if( BUILD_KSCREENSAVER )
  # limits.h (kscreensaver/kdesavers) 
  check_include_file_cxx( limits HAVE_NUMERIC_LIMITS )
  
  # memory.h (kscreensaver/xsavers) 
  check_include_file( memory.h HAVE_MEMORY_H )

  # X11/dirent.h (kscreensaver/xsavers)
  check_include_file( X11/dirent.h HAVE_DIRENT_H )

  # sys/ndir.h (kscreensaver/xsavers)
  check_include_file( sys/ndir.h HAVE_SYS_NDIR_H )

  # sys/dir.h (kscreensaver/xsavers)
  check_include_file( sys/dir.h HAVE_SYS_DIR_H )

  # ndir.h (kscreensaver/xsavers)
  check_include_file( ndir.h HAVE_NDIR_H )
  
  # OpenGL(kscreensaver/kdesavers)
  if( WITH_OPENGL )
    find_package( OpenGL REQUIRED )
    if( NOT OPENGL_FOUND )
      tde_message_fatal( "OpenGL is required, but was not found on your system" )
    endif( NOT OPENGL_FOUND )
    
	# for kscreensaver/xsavers
	set( HAVE_GL ${OPENGL_FOUND} )
    
	# GL/xmesa.h (kscreensaver/xsavers)
    check_include_file( GL/xmesa.h HAVE_GL_XMESA_H )

    # GL/glut.h (kscreensaver/xsavers)
    check_include_file( GL/glut.h HAVE_GL_XMESA_H )

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
  
  # xscreensavers(kscreensaver/kxsconfig)
  if( WITH_XSCREENSAVER )
    find_package( X11 )
    if( NOT X11_FOUND OR NOT X11_Xt_FOUND )
      message( FATAL_ERROR 
  		"\nX11 and Xt library is required for xscreensaver support, but it was not found on your system" )
    endif( )
    include( FindXscreensaver.cmake ) # not really good practise
    if( NOT XSCREENSAVER_FOUND )
      message( FATAL_ERROR "\nxscreensaver is requested, but was not found on your system" )
    endif( )
  endif( WITH_XSCREENSAVER )
endif( BUILD_KSCREENSAVER )

# required stuff
find_package( TQt )
find_package( TDE )
