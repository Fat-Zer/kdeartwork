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
  ${TQT_INCLUDE_DIRS}
  ${TDE_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)


link_directories(
  ${TQT_LIBRARY_DIRS}
)

add_subdirectory( config )

##### other data ################################

install( FILES phase.themerc DESTINATION ${DATA_INSTALL_DIR}/kstyle/themes )


##### phasestyle #################################

set( target phasestyle )

set( ${target}_SRCS
  phasestyle.cpp
)

tde_add_kpart( ${target} AUTOMOC
  SOURCES ${${target}_SRCS}
  LINK kdefx-shared
  DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/styles
)
