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
  ${TDE_INCLUDE_DIR}
  ${TQT_INCLUDE_DIRS}
)

link_directories(
  ${TQT_LIBRARY_DIRS}
)

##### kwin_icewm_config (module) ################

tde_add_kpart( kwin_icewm_config AUTOMOC
  SOURCES config.cpp
  LINK kdeui-shared kio-shared
  DESTINATION ${PLUGIN_INSTALL_DIR}
)
