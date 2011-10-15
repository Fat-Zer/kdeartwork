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
  ${TQT_INCLUDE_DIRS}
  ${TDE_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

link_directories(
  ${TQT_LIBRARY_DIRS}
)

##### kstyle_phase_config ########################

set( target kstyle_phase_config )

set( ${target}_SRCS
  phasestyleconfig.cpp
  styledialog.ui
)

tde_add_kpart( ${target} AUTOMOC
  SOURCES ${${target}_SRCS}
  LINK kdeui-shared
  DESTINATION ${PLUGIN_INSTALL_DIR}
)
