INCLUDES = $(all_includes)

kde_module_LTLIBRARIES = kwin_icewm_config.la

kwin_icewm_config_la_SOURCES = config.cpp
kwin_icewm_config_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module $(LIB_QT) $(LIB_KDECORE) $(LIB_KDEUI) -lkdefx
kwin_icewm_config_la_LIBADD = $(LIB_KDEUI) $(LIB_KIO) 

METASOURCES = AUTO
noinst_HEADERS = config.h

lnkdir = $(kde_datadir)/kwin/

###KMAKE-start (don't edit or delete this block)

###KMAKE-end
