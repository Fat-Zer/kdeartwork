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

cmake_minimum_required( VERSION 2.8 )


##### general package setup #####################

project( kdeartwork )

set( PACKAGE kdeartwork )
set( VERSION "3.5.13" )


##### include essential cmake modules ###########

include( FindPkgConfig )
include( CheckIncludeFile )
include( CheckIncludeFileCXX )
include( CheckCXXSourceCompiles )

##### include our cmake modules #################

set( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" )
include( TDEMacros )


##### setup install paths #######################

include( TDESetupPaths )
tde_setup_paths( )


##### optional stuff ############################

option( WITH_XSCREENSAVER "Enable xscreensavers support" ON )
option( WITH_LIBART "Enable libart support" ON )
option( WITH_OPENGL "Enable 3D GL modes" ON )
option( WITH_ARTS "Enable aRts support" OFF )

##### user requested modules ####################

option( BUILD_ALL "Build all" OFF )
option( BUILD_EMOTICONS "Build emoticons" ${BUILD_ALL} )
option( BUILD_ICEWM_THEMES "Build icewm-themes" ${BUILD_ALL} )
option( BUILD_ICON_THEMES "Build icon themes" ${BUILD_ALL} )
option( BUILD_KSCREENSAVER "Build kscreensaver" ${BUILD_ALL} )
option( BUILD_KWIN_STYLES "Build kwin styles" ${BUILD_ALL} )
option( BUILD_KWORLDCLOCK "Build kworldclock" ${BUILD_ALL} )
option( BUILD_SOUNDS "Build sounds" ${BUILD_ALL} )
option( BUILD_STYLES "Build styles" ${BUILD_ALL} )
option( BUILD_WALLPAPERS "Build wallpapers" ${BUILD_ALL} )

##### configure checks ##########################

include( ConfigureChecks.cmake )

###### global compiler settings #################

add_definitions(
  -DHAVE_CONFIG_H
)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TQT_CXX_FLAGS}" )
set( CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined" )
set( CMAKE_MODULE_LINKER_FLAGS "-Wl,--no-undefined" )


##### kdenetwork directories ####################

tde_conditional_add_subdirectory( BUILD_EMOTICONS emoticons )
tde_conditional_add_subdirectory( BUILD_ICEWM_THEMES icewm-themes )
tde_conditional_add_subdirectory( BUILD_ICON_THEMES IconThemes )
tde_conditional_add_subdirectory( BUILD_KSCREENSAVER kscreensaver )
tde_conditional_add_subdirectory( BUILD_KWIN_STYLES kwin-styles )
tde_conditional_add_subdirectory( BUILD_KWORLDCLOCK kworldclock )
tde_conditional_add_subdirectory( BUILD_SOUNDS sounds )
tde_conditional_add_subdirectory( BUILD_STYLES styles )
tde_conditional_add_subdirectory( BUILD_WALLPAPERS wallpapers )

##### write configure files #####################

configure_file( config.h.cmake config.h @ONLY )
