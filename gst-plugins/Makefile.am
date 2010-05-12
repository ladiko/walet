# plugindir is set in configure
lib_LTLIBRARIES = libgstpgmdec.la libgstpgmenc.la libgstbay2rgb.la

# sources used to compile this plug-in
libgstpgmdec_la_SOURCES = gstpgmdec.c
libgstpgmdec_la_CFLAGS = $(GST_CFLAGS)
libgstpgmdec_la_LIBADD = $(GST_LIBS) $(GST_BASE_LIBS) $(GSTCTRL_LIBS)
libgstpgmdec_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstpgmdec_la_LIBTOOLFLAGS = --tag=disable-static

libgstpgmenc_la_SOURCES = gstpgmenc.c
libgstpgmenc_la_CFLAGS = $(GST_CFLAGS)
libgstpgmenc_la_LIBADD = $(GST_LIBS) $(GST_BASE_LIBS) $(GSTCTRL_LIBS)
libgstpgmenc_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstpgmenc_la_LIBTOOLFLAGS = --tag=disable-static

libgstbay2rgb_la_SOURCES = gstbay2rgb.c
libgstbay2rgb_la_CFLAGS = $(GST_CFLAGS)
libgstbay2rgb_la_LIBADD = $(GST_LIBS) $(GST_BASE_LIBS) $(GSTCTRL_LIBS)
libgstbay2rgb_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstbay2rgb_la_LIBTOOLFLAGS = --tag=disable-static

# headers we need but don't want installed
noinst_HEADERS = gstpgmdec.h gstpgmenc.h gstbay2rgb.h
