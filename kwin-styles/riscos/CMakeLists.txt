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
  ${CMAKE_BINARY_DIR}
  ${TDE_INCLUDE_DIR}
  ${TQT_INCLUDE_DIRS}
)

link_directories(
  ${TQT_LIBRARY_DIRS}
)

##### other data ################################

install( FILES riscos.desktop DESTINATION ${DATA_INSTALL_DIR}/kwin )

##### kwin3_riscos ###############################


set( target kwin3_riscos )

set( ${target}_SRCS
  AboveButton.cpp Button.cpp
  CloseButton.cpp
  HelpButton.cpp
  IconifyButton.cpp
  LowerButton.cpp
  Manager.cpp
  MaximiseButton.cpp
  Static.cpp
  StickyButton.cpp
)

tde_add_kpart( ${target} AUTOMOC
  SOURCES ${${target}_SRCS}
  LINK kdecorations-shared
  DESTINATION ${PLUGIN_INSTALL_DIR}
)
