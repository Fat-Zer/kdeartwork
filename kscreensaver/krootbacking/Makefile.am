INCLUDES = $(all_includes)

bin_PROGRAMS = krootbacking

krootbacking_SOURCES = main.cpp krootbacking.cpp
krootbacking_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module $(LIB_QT) $(LIB_KDECORE) $(LIB_KDEUI)

METASOURCES = AUTO

noinst_HEADERS = krootbacking.h

hackdir = /usr/local/bin/
hack_DATA = xscreensaver-getimage xscreensaver-getimage-file

install-data-hook:
	chmod 755 $(DESTDIR)$(hackdir)/xscreensaver-getimage
	chmod 755 $(DESTDIR)$(hackdir)/xscreensaver-getimage-file
