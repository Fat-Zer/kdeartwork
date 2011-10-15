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

add_definitions(
  -DQT_PLUGIN
)

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${TQT_INCLUDE_DIRS}
  ${TDE_INCLUDE_DIR}
)

link_directories(
  ${TQT_LIBRARY_DIRS}
)

##### other data ################################

install( FILES cde.desktop DESTINATION ${DATA_INSTALL_DIR}/kwin )


##### kwin_cde ###################################

tde_add_kpart( kwin3_cde AUTOMOC
  SOURCES cdeclient.cpp
  LINK kdecorations-shared kdecore-shared
  DESTINATION ${PLUGIN_INSTALL_DIR}
)
