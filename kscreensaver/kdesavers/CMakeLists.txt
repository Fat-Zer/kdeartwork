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
  ${CMAKE_BINARY_DIR}
  ${TDE_INCLUDE_DIR}
  ${TQT_INCLUDE_DIRS}
)

link_directories(
  ${TQT_LIBRARY_DIRS}
)


#	$(X11_KSAVERS) $(LIBART_KSAVERS) $(ARTS_GL_KSAVERS) $(GL_KSAVERS) )


# The "normal" screensavers
set( kbanner_SOURCES    banner.cpp )
set( kpolygon_SOURCES   polygon.cpp )
set( klines_SOURCES     lines.cpp )
set( klorenz_SOURCES    lorenz.cpp )
set( kslideshow_SOURCES slideshow.cpp slideshowcfg.ui )
set( kblob_SOURCES      blob.cpp )
set( kvm_SOURCES        kvm.cpp vm.c vm_random.c )

set( kslideshow_LINK	kio-shared )

set( NORMAL_SCREENSAVERS 
	kbanner kpolygon kslideshow	klines klorenz kblob kvm )

# X11 screen savers
set( kscience_SOURCES   science.cpp )	
set( X11_SCREENSAVERS kscience)

# The GL screensavers
set( kfountain_SOURCES	fountain.cpp fountaincfg.ui )
set( kwave_SOURCES		wave.cpp wavecfg.ui )
set( kgravity_SOURCES	gravity.cpp gravitycfg.ui )
set( kflux_SOURCES		Flux.cpp )
set( keuphoria_SOURCES	Euphoria.cpp )
set( ksolarwinds_SOURCES SolarWinds.cpp )
set( krotation_SOURCES	rotation.cpp rotationcfg.ui sspreviewarea.cpp
   	rkodesolver.cpp vec3.cpp )
set( kpendulum_SOURCES	pendulum.cpp pendulumcfg.ui sspreviewarea.cpp
   	rkodesolver.cpp )
set( kfiresaver_SOURCES	firesaversetup.ui firesaverparticle.cpp
   	firesaverwriter.cpp	firesaver.cpp )
set( kfiresaver_LDADD -lartskde )

#set( kfountain_LDADD $(GL_KSAVERS_LDADD) -lm )

# The LIBART screensavers
set( kclock_SOURCES	kclock.cpp )
# kclock_LDADD $(LIBART_KSAVERS_LDADD)

if( WITH_LIBART )
	set( SCREENSAVERS ${SCREENSAVERS} )
endif( WITH_LIBART )

if( WITH_GL )
	
	if( WITH_ARTS )
		
	endif( WITH_ARTS )
endif( WITH_GL )
	
# include_directories(
#   ${CMAKE_CURRENT_BINARY_DIR}
#   ${CMAKE_BINARY_DIR}
#   ${TDE_INCLUDE_DIR}
#   ${TQT_INCLUDE_DIRS}
# )
# 
# link_directories(
#   ${TQT_LIBRARY_DIRS}
# )

foreach( saver ${NORMAL_SCREENSAVERS} )
  tde_add_kpart( ${saver} AUTOMOC
    SOURCES ${${saver}_SOURCES}
    LINK kscreensaver-shared kdeui-shared ${${saver}_LINK}
    DESTINATION ${PLUGIN_INSTALL_DIR}
  )
endforeach( saver ${NORMAL_SCREENSAVERS} )


# ##### other data ################################
# 
# install( FILES kwebdesktop.desktop DESTINATION ${DATA_INSTALL_DIR}/kdesktop/programs )
# install( FILES kwebdesktop.kcfg DESTINATION ${KCFG_INSTALL_DIR} )
# 
# 
# ##### kwebdesktop (executable) ##################
# 
# set( target kwebdesktop )
# 
# set( ${target}_SRCS
#   kwebdesktop.cpp kwebdesktopsettings.kcfgc
# )
# 
# tde_add_executable( ${target} AUTOMOC
#   SOURCES ${${target}_SRCS}
#   LINK khtml-shared
#   DESTINATION ${BIN_INSTALL_DIR}
# )
