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
add_subdirectory( themes )

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${TDE_INCLUDE_DIR}
  ${TQT_INCLUDE_DIRS}
)

link_directories(
  ${TQT_LIBRARY_DIRS}
  ${TDE_LIB_DIR}
)

##### other data ################################

install( FILES glow.desktop DESTINATION ${DATA_INSTALL_DIR}/kwin )

##### kwin3_glow ################################

tde_add_kpart( kwin3_glow AUTOMOC
  SOURCES glowbutton.cpp glowclient.cpp
  LINK kparts-shared kdecorations
  DESTINATION ${PLUGIN_INSTALL_DIR}
)
