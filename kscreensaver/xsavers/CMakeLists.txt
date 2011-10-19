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
  ${TQT_INCLUDE_DIRS}
  ${TDE_INCLUDE_DIR}
)

link_directories(
  ${TQT_LIBRARY_DIRS}
)

##### shared source ##########################

set( BASE_SRCS main.cpp demowin.cpp saver.cpp helpers.cpp )
set( XL_SRCS xlock.cpp )

##### kswarm #################################

tde_add_executable( kswarm.kss AUTOMOC
  SOURCES swarm.cpp ${BASE_SRCS} ${XL_SRCS}
  LINK kdeui-shared
  DESTINATION ${BIN_INSTALL_DIR}
)

install( FILES KSwarm.desktop DESTINATION ${APPS_INSTALL_DIR}/System/ScreenSavers )

##### kswarm #################################

if( WITH_OPENGL )
  include_directories( ${OPENGL_INCLUDE_DIRS} )
  link_directories( ${OPENGL_LIBRARY_DIRS} )
  tde_add_executable( kspace.kss AUTOMOC
    SOURCES space.cpp ${BASE_SRCS} ${XL_SRCS}
	LINK kdeui-shared ${OPENGL_LIBRARIES}
    DESTINATION ${BIN_INSTALL_DIR}
  )
  
  install( FILES KSpace.desktop DESTINATION ${APPS_INSTALL_DIR}/System/ScreenSavers )
endif( WITH_OPENGL )


