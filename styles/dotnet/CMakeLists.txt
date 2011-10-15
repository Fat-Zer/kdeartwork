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
#   ${CMAKE_CURRENT_BINARY_DIR}
#   ${CMAKE_BINARY_DIR}/kdecore
#   ${CMAKE_SOURCE_DIR}/kdefx
#   ${CMAKE_SOURCE_DIR}/kdecore
)

link_directories(
  ${TQT_LIBRARY_DIRS}
)


##### other data ################################

install( FILES dotnet.themerc DESTINATION ${DATA_INSTALL_DIR}/kstyle/themes )


##### asteroid ###################################

set( target dotnet )

set( ${target}_SRCS
  dotnet.cpp
)

tde_add_kpart( ${target} AUTOMOC
  SOURCES ${${target}_SRCS}
  LINK kdefx-shared
  DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/styles
)
