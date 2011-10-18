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

set(ICON_THEMES Locolor ikons kdeclassic slick kids )
set(ICON_TYPES actions apps devices filesystems mimetypes)
set(ICON_SIZES 16x16 22x22 32x32 48x48 64x64 128x128 scalable)
set(ICON_SUFFIXES png mng svg svgz)

foreach (theme ${ICON_THEMES})
  install(FILES ${theme}/index.theme DESTINATION ${ICON_INSTALL_DIR}/${theme} )
  foreach(size ${ICON_SIZES})
    foreach(type ${ICON_TYPES})
      foreach(suffix ${ICON_SUFFIXES})
        FILE(GLOB files  "${theme}/${size}/${type}/*.${suffix}" )
        install(FILES ${files} DESTINATION ${ICON_INSTALL_DIR}/${theme}/${size}/${type}/)
      endforeach(suffix ${ICON_SUFFIXES})
    endforeach(type ${ICON_TYPES})
  endforeach(size ${ICON_SIZES})
endforeach (theme ${ICON_THEMES})

