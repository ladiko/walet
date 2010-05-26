bin_PROGRAMS = walet_gtk

walet_gtk_SOURCES = walet_gtk.c gui.c walet_gtk.h
walet_gtk_CFLAGS = $(GTK_CFLAGS) $(GST_CFLAGS)
walet_gtk_LDADD = $(GTK_LIBS) $(GST_LIBS) ../lib/libwalet.la

INCLUDES = -I../lib/
noinst_HEADERS = gui.h
