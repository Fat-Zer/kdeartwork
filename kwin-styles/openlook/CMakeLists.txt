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
)

link_directories(
  ${TQT_LIBRARY_DIRS}
)

##### other data ################################

install( FILES openlook.desktop DESTINATION ${DATA_INSTALL_DIR}/kwin )

##### kwin3_openlook ############################

tde_add_kpart( kwin3_openlook AUTOMOC
  SOURCES OpenLook.cpp
  LINK kdecorations-shared
  DESTINATION ${PLUGIN_INSTALL_DIR}
)
