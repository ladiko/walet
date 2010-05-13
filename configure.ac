AC_INIT([walet], [0.20.0], [vadshe@gmail.com], [walet], [http://sites.google.com/site/waletcodec/])

dnl versions of gstreamer and plugins-base
GST_MAJORMINOR=0.10
GST_REQUIRED=0.10.0
GSTPB_REQUIRED=0.10.0

AC_PREREQ([2.59])
AM_INIT_AUTOMAKE([1.10 no-define])
AC_CONFIG_HEADERS([config.h])
#AC_CONFIG_FILES([Makefile])

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AM_PROG_LIBTOOL
AC_PROG_RANLIB
AC_PROG_LIBTOOL


PKG_CHECK_MODULES(GTK, gtk+-2.0 >= 2.16.0, HAVE_GTK=yes, HAVE_GTK=no)
if test "x$HAVE_GTK" = "xno"; then
  AC_MSG_ERROR(you need gtk+-2.0 >= 2.20.0 installed !)
fi
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

PKG_CHECK_MODULES(GST, gstreamer-$GST_MAJORMINOR >= $GST_REQUIRED, HAVE_GST=yes,HAVE_GST=no)
if test "x$HAVE_GST" = "xno"; then
  AC_MSG_ERROR(you need gstreamer development packages installed !)
fi
AC_SUBST(GST_CFLAGS)
AC_SUBST(GST_LIBS)
AC_SUBST(GST_MAJORMINOR)

dnl If we need them, we can also use the base class libraries
PKG_CHECK_MODULES(GST_BASE, gstreamer-base-$GST_MAJORMINOR >= $GST_REQUIRED, HAVE_GST_BASE=yes, HAVE_GST_BASE=no)
if test "x$HAVE_GST_BASE" = "xno"; then
  AC_MSG_NOTICE(no GStreamer base class libraries found (gstreamer-base-$GST_MAJORMINOR))
fi
AC_SUBST(GST_BASE_CFLAGS)
AC_SUBST(GST_BASE_LIBS)

dnl If we need them, we can also use the gstreamer-plugins-base libraries
PKG_CHECK_MODULES(GSTPB_BASE, gstreamer-plugins-base-$GST_MAJORMINOR >= $GSTPB_REQUIRED, HAVE_GSTPB_BASE=yes, HAVE_GSTPB_BASE=no)
if test "x$HAVE_GSTPB_BASE" = "xno"; then
  AC_MSG_NOTICE(no GStreamer Plugins Base libraries found (gstreamer-plugins-base-$GST_MAJORMINOR))
fi
AC_SUBST(GSTPB_BASE_CFLAGS)
AC_SUBST(GSTPB_BASE_LIBS)

dnl If we need them, we can also use the gstreamer-controller libraries
PKG_CHECK_MODULES(GSTCTRL, gstreamer-controller-$GST_MAJORMINOR >= $GSTPB_REQUIRED, HAVE_GSTCTRL=yes, HAVE_GSTCTRL=no)
if test "x$HAVE_GSTCTRL" = "xno"; then
  AC_MSG_NOTICE(no GStreamer Controller libraries found (gstreamer-controller-$GST_MAJORMINOR))
fi
AC_SUBST(GSTCTRL_CFLAGS)
AC_SUBST(GSTCTRL_LIBS)

dnl set the plugindir where plugins should be installed
if test "x${prefix}" = "x$HOME"; then
  plugindir="$HOME/.gstreamer-$GST_MAJORMINOR/plugins"
else
  plugindir="\$(libdir)/gstreamer-$GST_MAJORMINOR"
fi
AC_SUBST(plugindir)

dnl set proper LDFLAGS for plugins
GST_PLUGIN_LDFLAGS='-module -avoid-version -export-symbols-regex [_]*\(gst_\|Gst\|GST_\).*'
AC_SUBST(GST_PLUGIN_LDFLAGS)

AC_ARG_ENABLE(debug,[--enable-debug  Enable debug notices],[debug=$enableval], [debug=no])

#GCC_FLAGS="-Wall"

if test "x${debug}" = "xyes"; then
  CFLAGS="-Wall -O0 -g"
else
  CFLAGS="-O2"
fi
AC_SUBST(CFLAGS)


AC_OUTPUT(Makefile  lib/Makefile  gst-plugins/Makefile gtk/Makefile)
