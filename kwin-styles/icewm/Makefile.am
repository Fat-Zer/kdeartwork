INCLUDES = $(all_includes)

SUBDIRS = . config icewm-themes

kde_module_LTLIBRARIES = kwin3_icewm.la

kwin3_icewm_la_SOURCES = icewm.cpp
kwin3_icewm_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module $(LIB_QT) $(LIB_KDECORE) $(LIB_KDEUI) -lkdefx
kwin3_icewm_la_LIBADD  = $(LIB_KDEUI) -lkdecorations

METASOURCES = AUTO
noinst_HEADERS = icewm.h

lnkdir = $(kde_datadir)/kwin/
lnk_DATA = icewm.desktop

EXTRA_DIST = $(lnk_DATA)

###KMAKE-start (don't edit or delete this block)

###KMAKE-end
