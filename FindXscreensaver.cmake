#Macro to find xscreensaver directory

# Copyright (c) 2006, Laurent Montel, <montel@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
# got from kde4

if (NOT XSCREENSAVER_FOUND)
  set(xscreensaver_alldirs)
  set(xscreensaver_checkdirs ${TDE_INCLUDE_DIR}
   	/usr/
	/usr/local/
   	/opt/local/
   	/usr/X11R6/
   	/opt/kde/
   	/opt/kde3/
   	/usr/kde/
   	/usr/local/kde/
   	/usr/local/xscreensaver/
   	/usr/openwin/lib/xscreensaver/
   	/etc/ )
  foreach(suffix lib${LIB_SUFFIX}/xscreensaver lib${LIB_SUFFIX}/misc/xscreensaver lib/xscreensaver lib64/xscreensaver libexec/xscreensaver 
bin/xscreensaver-hacks hacks)
     foreach(xscreensaver_path ${xscreensaver_checkdirs} )
        set(xscreensaver_alldirs ${xscreensaver_alldirs} ${xscreensaver_path}/${suffix})
     endforeach(xscreensaver_path ${xscreensaver_checkdirs} )
  endforeach(suffix lib${LIB_SUFFIX}/xscreensaver lib/xscreensaver lib64/xscreensaver libexec/xscreensaver bin/xscreensaver-hacks hacks)
  FIND_PATH(XSCREENSAVER_DIR deco ${xscreensaver_alldirs})

  set(XSCREENSAVER_CONFIG_DIR)
  FIND_PATH(XSCREENSAVER_CONFIG_DECO config/deco.xml
    ${TDE_INCLUDE_DIR}
    /usr/
    /usr/local/
    /opt/local/
    /usr/X11R6/
    /opt/kde/
    /opt/kde3/
    /usr/kde/
    /usr/share/xscreensaver/
    /usr/local/kde/
    /usr/local/xscreensaver/
    /usr/openwin/lib/xscreensaver/
    /etc/
  )
  #MESSAGE(STATUS "XSCREENSAVER_CONFIG_DIR :<${XSCREENSAVER_CONFIG_DIR}>")

  if(XSCREENSAVER_CONFIG_DECO)
	set(XSCREENSAVER_CONFIG_DIR "${XSCREENSAVER_CONFIG_DECO}/config/")
	#MESSAGE(STATUS "XSCREENSAVER_CONFIG_DIR <${XSCREENSAVER_CONFIG_DIR}>")
  endif(XSCREENSAVER_CONFIG_DECO)


  # Try and locate XScreenSaver config when path doesn't include config
  if(NOT XSCREENSAVER_CONFIG_DIR)
    FIND_PATH(XSCREENSAVER_CONFIG_DIR deco.xml
      /etc/xscreensaver
      )
  endif(NOT XSCREENSAVER_CONFIG_DIR)
endif(NOT XSCREENSAVER_FOUND)

#MESSAGE(STATUS "XSCREENSAVER_CONFIG_DIR :<${XSCREENSAVER_CONFIG_DIR}>")
#MESSAGE(STATUS "XSCREENSAVER_DIR :<${XSCREENSAVER_DIR}>")

# Need to fix hack
if(XSCREENSAVER_DIR AND XSCREENSAVER_CONFIG_DIR)
	set(XSCREENSAVER_FOUND TRUE)
endif(XSCREENSAVER_DIR AND XSCREENSAVER_CONFIG_DIR)

if (XSCREENSAVER_FOUND)
  if (NOT Xscreensaver_FIND_QUIETLY)
    message(STATUS "Found SCREENSAVER_CONFIG_DIR <${XSCREENSAVER_CONFIG_DIR}>")
  endif (NOT Xscreensaver_FIND_QUIETLY)
else (XSCREENSAVER_FOUND)
  if (Xscreensaver_FIND_REQUIRED)
    message(FATAL_ERROR "XScreenSaver not found")
  endif (Xscreensaver_FIND_REQUIRED)
endif (XSCREENSAVER_FOUND)


MARK_AS_ADVANCED(XSCREENSAVER_DIR XSCREENSAVER_CONFIG_DIR)
