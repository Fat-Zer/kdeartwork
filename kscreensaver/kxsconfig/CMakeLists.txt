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

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_BINARY_DIR}
  ${TDE_INCLUDE_DIR}
  ${TQT_INCLUDE_DIRS}
  ${X11_INCLUDE_DIR}
)

link_directories(
  ${TQT_LIBRARY_DIRS}
  ${X11_LIBRARY_DIR}
)

##### kxsconfig #################################
set( target kxsconfig )

set( ${target}_SRCS
   kxscontrol.cpp kxsitem.cpp kxsxml.cpp kxsconfig.cpp )

tde_add_executable( ${target} AUTOMOC
  SOURCES ${${target}_SRCS}
  LINK kio-shared ${X11_Xt_LIB}
  DESTINATION ${BIN_INSTALL_DIR}
)

##### kxsrun ####################################
set( target kxsrun )

set( ${target}_SRCS
   kxsrun.cpp kxscontrol.cpp kxsitem.cpp kxsxml.cpp )

tde_add_executable( ${target} AUTOMOC
  SOURCES ${${target}_SRCS}
  LINK kio-shared
  DESTINATION ${BIN_INSTALL_DIR}
)

##### install desktops ##########################
# code from kde4
FILE(GLOB _desktopfiles "ScreenSavers/*.desktop" )
foreach(_currentdesktopfile ${_desktopfiles})
  STRING(REGEX REPLACE ".desktop" "" _newitem "${_currentdesktopfile}" )
  #MESSAGE(STATUS "newItem <${_newitem}>")
  GET_FILENAME_COMPONENT(_screensaverName ${_newitem} NAME_WE)
  #MESSAGE(STATUS "name without extension
  find_file(XSCREENSAVER_FILE_FOUND ${_screensaverName}.xml PATHS ${XSCREENSAVER_CONFIG_DIR} )
  if(XSCREENSAVER_FILE_FOUND)
    MESSAGE(STATUS "xscreensaver name ${_screensaverName} found")
    install(FILES ${_currentdesktopfile} DESTINATION  ${APPS_INSTALL_DIR}/System/ScreenSavers )
  else(XSCREENSAVER_FILE_FOUND)
    MESSAGE(STATUS "xscreensaver name ${_screensaverName} not found")
  endif(XSCREENSAVER_FILE_FOUND)
  unset(XSCREENSAVER_FILE_FOUND CACHE)
endforeach(_currentdesktopfile ${_desktopfiles})

