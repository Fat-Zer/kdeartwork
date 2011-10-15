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
  ${TQT_INCLUDE_DIR}
  ${TDE_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
#   ${CMAKE_BINARY_DIR}/kdecore
#   ${CMAKE_SOURCE_DIR}/kdefx
#   ${CMAKE_SOURCE_DIR}/kdecore
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
