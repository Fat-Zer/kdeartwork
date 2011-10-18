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
  ${TDE_INCLUDE_DIR}
  ${TQT_INCLUDE_DIRS}
)

link_directories(
  ${TQT_LIBRARY_DIRS}
)

##### desktop files #############################

install( FILES kpartsaver.desktop 
    DESTINATION ${APPS_INSTALL_DIR}/System/ScreenSavers )

##### kpart_cde ##################################

tde_add_executable( kpartsaver.kss AUTOMOC
  SOURCES kpartsaver.cpp configwidget.ui
  LINK kscreensaver-shared kio-shared
  DESTINATION ${BIN_INSTALL_DIR}
)
