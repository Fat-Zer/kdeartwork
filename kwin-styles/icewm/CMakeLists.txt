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

add_subdirectory( config )
add_subdirectory( icewm-themes )

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${TDE_INCLUDE_DIR}
  ${TQT_INCLUDE_DIRS}
)

link_directories(
  ${TQT_LIBRARY_DIRS}
)

##### other data ################################

install( FILES icewm.desktop DESTINATION ${DATA_INSTALL_DIR}/kwin )

##### kwin3_icewm ###############################

tde_add_kpart( kwin3_icewm AUTOMOC
  SOURCES icewm.cpp
  LINK kdecorations-shared
  DESTINATION ${PLUGIN_INSTALL_DIR}
)
