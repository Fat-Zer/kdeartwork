INCLUDES = $(all_includes)

kde_module_LTLIBRARIES = kwin_glow_config.la

kwin_glow_config_la_SOURCES = glowconfigdialog.cpp
kwin_glow_config_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module $(LIB_QT) $(LIB_KDECORE) $(LIB_KDEUI) -lkdefx
kwin_glow_config_la_LIBADD = $(LIB_KDEUI)

METASOURCES = AUTO
noinst_HEADERS = glowconfigdialog.h

lnkdir = $(kde_datadir)/kwin/


###KMAKE-start (don't edit or delete this block)

###KMAKE-end
