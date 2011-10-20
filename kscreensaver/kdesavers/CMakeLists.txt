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
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${TDE_INCLUDE_DIR}
  ${TQT_INCLUDE_DIRS}
)

link_directories(
  ${TQT_LIBRARY_DIRS}
  ${TDE_LIB_DIR}
)

# ##### The "normal" screensavers ###############

set( kbanner_SRCS    banner.cpp )
set( kbanner_DESKTOP    KBanner.desktop )
set( kpolygon_SRCS   polygon.cpp )
set( kpolygon_DESKTOP   KPolygon.desktop )
set( klines_SRCS     lines.cpp )
set( klines_DESKTOP     KLines-saver.desktop )
set( klorenz_SRCS    lorenz.cpp )
set( klorenz_DESKTOP    KLorenz.desktop )
set( kslideshow_SRCS slideshow.cpp slideshowcfg.ui )
set( kslideshow_DESKTOP KSlideshow.desktop )
set( kblob_SRCS      blob.cpp )
set( kblob_DESKTOP      KBlob.desktop )
set( kvm_SRCS        kvm.cpp vm.c vm_random.c )
set( kvm_DESKTOP        KVm.desktop )

set( kslideshow_LINK	kio-shared )

set( NORMAL_SCREENSAVERS 
	kbanner kpolygon kslideshow	klines klorenz kblob kvm )

##### X11 screensavers ##########################

set( kscience_SRCS   science.cpp )	
set( kscience_DESKTOP   KScience.desktop )

set( X11_SCREENSAVERS kscience)

##### The GL screensavers #######################

set( kfountain_SRCS  fountain.cpp fountaincfg.ui )
set( kfountain_DESKTOP  KFountain.desktop )
set( kwave_SRCS      wave.cpp wavecfg.ui )
set( kwave_DESKTOP      KWave.desktop )
set( kgravity_SRCS   gravity.cpp gravitycfg.ui )
set( kgravity_DESKTOP   KGravity.desktop )
set( kflux_SRCS      Flux.cpp )
set( kflux_DESKTOP      KFlux.desktop )
set( keuphoria_SRCS  Euphoria.cpp )
set( keuphoria_DESKTOP  KEuphoria.desktop )
set( ksolarwinds_SRCS SolarWinds.cpp )
set( ksolarwinds_DESKTOP KSolarWinds.desktop )
set( krotation_SRCS  rotation.cpp rotationcfg.ui sspreviewarea.cpp
   	rkodesolver.cpp vec3.cpp )
set( krotation_DESKTOP  KRotation.desktop )
set( kpendulum_SRCS  pendulum.cpp pendulumcfg.ui sspreviewarea.cpp
   	rkodesolver.cpp )
set( kpendulum_DESKTOP  KPendulum.desktop )
set( OPENGL_SCREENSAVERS kfountain kwave kgravity kflux keuphoria ksolarwinds
	krotation kpendulum )

##### The GL-Arts screensavers ##################

set( kfiresaver_SRCS firesaversetup.ui firesaverparticle.cpp
   	firesaverwriter.cpp	firesaver.cpp )
set( kfiresaver_DESKTOP KFiresaver.desktop )
set( kfiresaver_LINK artskde-shared )
set( GL_ARTS_SCREENSAVERS kfiresaver )

##### The LIBART screensavers ###################

set( kclock_SRCS	kclock.cpp )
set( kclock_DESKTOP	KClock.desktop )

set( LIBART_SCREENSAVERS kclock )

##### LIBART conditions #########################

if( WITH_LIBART )
  list( APPEND SCREENSAVERS ${LIBART_SCREENSAVERS} )

  include_directories( ${LIBART_INCLUDE_DIRS} )
  link_directories( ${LIBART_LIBRARY_DIRS} )
  foreach( saver ${LIBART_SCREENSAVERS} )
    list ( APPEND ${saver}_LINK ${LIBART_LIBRARIES} )
  endforeach( saver ${LIBART_SCREENSAVERS} )
endif( WITH_LIBART )

##### GL conditions #############################

if( WITH_OPENGL )
  if( WITH_ARTS )
    list( APPEND OPENGL_SCREENSAVERS ${GL_ARTS_SCREENSAVERS} )
  endif( WITH_ARTS )

  list( APPEND SCREENSAVERS ${OPENGL_SCREENSAVERS} )
  
  include_directories( ${OPENGL_INCLUDE_DIRS} )
  link_directories( ${OPENGL_LIBRARY_DIRS} )
  foreach( saver ${OPENGL_SCREENSAVERS} )
    list ( APPEND ${saver}_LINK ${OPENGL_LIBRARIES} )
  endforeach( saver ${OPENGL_SCREENSAVERS} )
    
endif( WITH_OPENGL )

##### add normal screensavers ###################

# FIXME: check for X includes/libraries
#        this screensavers will work only in X
list( APPEND SCREENSAVERS ${X11_SCREENSAVERS} )
list( APPEND SCREENSAVERS ${NORMAL_SCREENSAVERS} )
	
##### create install/compile rules ##############
foreach( saver ${SCREENSAVERS} )
  tde_add_executable( ${saver}.kss AUTOMOC
    SOURCES ${${saver}_SRCS}
    LINK kscreensaver-shared kdeui-shared ${${saver}_LINK}
    DESTINATION ${BIN_INSTALL_DIR}
  )
  install( FILES ${${saver}_DESKTOP} 
    DESTINATION ${APPS_INSTALL_DIR}/System/ScreenSavers )
endforeach( saver ${SCREENSAVERS} )

##### install various data ######################

install( FILES image.png kscience.png particle.png
    DESTINATION ${DATA_INSTALL_DIR}/kscreensaver )

# Actually the only screensaver tha uses this data is kfiresaver 
add_subdirectory( data )
