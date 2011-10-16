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

add_subdirectory( config)

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${TDE_INCLUDE_DIR}
  ${TQT_INCLUDE_DIRS}
)

link_directories(
  ${TQT_LIBRARY_DIRS}
)

##### other data ################################

install( FILES cde.desktop DESTINATION ${DATA_INSTALL_DIR}/kwin )

##### kwin_cde ###################################

set( kwin3_cde dotnet )

set( ${target}_SRCS
  cdeclient.cpp
)

tde_add_kpart( ${target} AUTOMOC
  SOURCES ${${target}_SRCS}
  LINK kdecorations kdecore
  DESTINATION ${PLUGIN_INSTALL_DIR}
)
