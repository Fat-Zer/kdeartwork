INCLUDES = $(all_includes)

bin_PROGRAMS = krootbacking

krootbacking_SOURCES = main.cpp krootbacking.cpp
krootbacking_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module $(LIB_QT) $(LIB_KDECORE) $(LIB_KDEUI)

METASOURCES = AUTO

noinst_HEADERS = krootbacking.h

hackdir = $(DESTDIR)/usr/local/bin/
hack_DATA = xscreensaver-getimage xscreensaver-getimage-file