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
# include( CheckFunctionExists )
# include( CheckIncludeFile )
# include( CheckIncludeFileCXX )
include( CheckCXXSourceCompiles )
# include( CheckLibraryExists )


##### include our cmake modules #################

set( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" )
include( TDEMacros )


##### setup install paths #######################

include( TDESetupPaths )
tde_setup_paths( )


##### optional stuff ############################

option( WITH_XSCREENSAVER "Enable xscreensavers support" ON )
option( WITH_LIBART "Enable libart support" ON )
option( WITH_GL "Enable 3D GL modes" ON )
option( WITH_ARTS "Enable aRts support" OFF )

#  --enable-profile        creates profiling infos [default=no]
#  --enable-pch            enables precompiled header support (currently only
#                          KCC or gcc >=3.4+unsermake) [default=no]
#  --enable-coverage       use gcc coverage testing
#  --enable-new-ldflags    enable the new linker flags
#  --enable-final          build size optimized apps (experimental - needs lots
#                          of memory)
#  --enable-closure        delay template instantiation
#  --enable-nmcheck        enable automatic namespace cleanness check
#  --enable-shared[=PKGS]  build shared libraries [default=yes]
#  --enable-static[=PKGS]  build static libraries [default=no]
#  --enable-libsuffix      /lib directory suffix (64,32,none,auto[=default])
#  --enable-fast-install[=PKGS]
#                          optimize for fast installation [default=yes]
#  --disable-libtool-lock  avoid locking (might break parallel builds)
#  --enable-pie            platform supports PIE linking [default=detect]
#  --enable-embedded       link to Qt-embedded, don't use X
#  --enable-qtopia         link to Qt-embedded, link to the Qtopia Environment
#  --enable-mac            link to Qt/Mac (don't use X)
#  --disable-mt            link to non-threaded Qt (deprecated)
#  --disable-threading     disables threading even if libpthread found
#  --disable-rpath         do not use the rpath feature of ld
#  --disable-path-check    don't try to find out, where to install
#  --without-arts          build without aRts [default=no]
#  --without-gl            disable 3D GL modes
#  --with-libart           enable support for libart [default=check]
#  --without-dpms          disable DPMS power saving
#  --with-xscreensaver     enable support for xscreensaver [default=check]

##### user requested modules ####################

option( BUILD_ALL "Build all" OFF )
option( BUILD_EMOTICONS "Build emoticons" ${BUILD_ALL} )
option( BUILD_ICEWM_THEMES "Build icewm-themes" ${BUILD_ALL} )

option( BUILD_KSCREENSAVER "Build kscreensaver" ${BUILD_ALL} )
option( BUILD_KWIN_STYLES "Build kwin styles" ${BUILD_ALL} )
option( BUILD_KWORLDCLOCK "Build kworldclock" ${BUILD_ALL} )
option( BUILD_SOUNDS "Build sounds" ${BUILD_ALL} )
option( BUILD_STYLES "Build styles" ${BUILD_ALL} )
option( BUILD_WALLPAPERS "Build wallpapers" ${BUILD_ALL} )

# option( BUILD_DCOPRSS "Build dcoprss" ${BUILD_ALL} )
# option( BUILD_DOC "Build doc" ${BUILD_ALL} )
# option( BUILD_FILESHARING "Build filesharing" ${BUILD_ALL} )
# option( BUILD_KDICT "Build kdict" ${BUILD_ALL} )
# option( BUILD_KDNSSD "Build kdnssd" ${BUILD_ALL} )
# option( BUILD_KFILE_PLUGINS "Build kfile-plugins" ${BUILD_ALL} )
# option( BUILD_KGET "Build kget" ${BUILD_ALL} )
# option( BUILD_KNEWSTICKER "Build knewsticker" ${BUILD_ALL} )
# option( BUILD_KOPETE "Build kopete" ${BUILD_ALL} )
# option( BUILD_KPF "Build kpf" ${BUILD_ALL} )
# option( BUILD_KPPP "Build kppp" ${BUILD_ALL} )
# option( BUILD_KRDC "Build krdc" ${BUILD_ALL} )
# option( BUILD_KRFB "Build krfb" ${BUILD_ALL} )
# option( BUILD_KSIRC "Build ksirc" ${BUILD_ALL} )
# option( BUILD_KTALKD "Build ktalkd" ${BUILD_ALL} )
# option( BUILD_LANBROWSING "Build lanbrowsing" ${BUILD_ALL} )
# option( BUILD_LIBRSS "Build librss" ${BUILD_ALL} )
# option( BUILD_WIFI "Build wifi" ${BUILD_ALL} )


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

tde_conditional_add_subdirectory( BUILD_KSCREENSAVER kscreensaver )
tde_conditional_add_subdirectory( BUILD_KWIN_STYLES kwin-styles )
tde_conditional_add_subdirectory( BUILD_KWORLDCLOCK kworldclock )
tde_conditional_add_subdirectory( BUILD_SOUNDS sounds )
tde_conditional_add_subdirectory( BUILD_STYLES styles )
tde_conditional_add_subdirectory( BUILD_WALLPAPERS wallpapers )
# tde_conditional_add_subdirectory( BUILD_DCOPRSS dcoprss )
# tde_conditional_add_subdirectory( BUILD_DOC doc )
# tde_conditional_add_subdirectory( BUILD_FILESHARING filesharing )
# tde_conditional_add_subdirectory( BUILD_KDICT kdict )
# tde_conditional_add_subdirectory( BUILD_KDNSSD kdnssd )
# tde_conditional_add_subdirectory( BUILD_KFILE_PLUGINS kfile-plugins )
# tde_conditional_add_subdirectory( BUILD_KGET kget )
# tde_conditional_add_subdirectory( BUILD_KNEWSTICKER knewsticker )
# tde_conditional_add_subdirectory( BUILD_KOPETE kopete )
# tde_conditional_add_subdirectory( BUILD_KPF kpf )
# tde_conditional_add_subdirectory( BUILD_KPPP kppp )
# tde_conditional_add_subdirectory( BUILD_KRDC krdc )
# tde_conditional_add_subdirectory( BUILD_KRFB krfb )
# tde_conditional_add_subdirectory( BUILD_KSIRC ksirc )
# tde_conditional_add_subdirectory( BUILD_KTALKD ktalkd )
# tde_conditional_add_subdirectory( BUILD_LANBROWSING lanbrowsing )
# tde_conditional_add_subdirectory( BUILD_LIBRSS librss )
# tde_conditional_add_subdirectory( BUILD_WIFI wifi )


##### write configure files #####################

configure_file( config.h.cmake config.h @ONLY )
