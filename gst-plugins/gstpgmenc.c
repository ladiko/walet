/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2010 Vadim <<user@hostname.org>>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:element-pgmenc
 *
 * FIXME:Describe pgmenc here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! pgmenc ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>
#include <gst/base/gstadapter.h>
#include <gst/video/video.h>

#include "gstpgmenc.h"

GST_DEBUG_CATEGORY_STATIC (gst_pgmenc_debug);
#define GST_CAT_DEFAULT gst_pgmenc_debug

/* Filter signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

enum
{
  PROP_0,
  PROP_WIDTH,
  PROP_HEIGHT,
  PROP_COLOR_SPACE,
  PROP_BPP,
  PROP_BAYER_GRID,
  PROP_SILENT
};

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (
		"video/x-raw-bayer,"
		"bpp = (int) [8,14], "
		//"endianness = (int) BYTE_ORDER, "
		"width = " GST_VIDEO_SIZE_RANGE ", "
		"height = " GST_VIDEO_SIZE_RANGE ", "
		"framerate = " GST_VIDEO_FPS_RANGE "; "
		"video/x-raw-gray,"
		"bpp = (int) [8,14], "
		//"endianness = (int) BYTE_ORDER, "
		"width = " GST_VIDEO_SIZE_RANGE ", "
		"height = " GST_VIDEO_SIZE_RANGE ", "
		"framerate = " GST_VIDEO_FPS_RANGE
		)
    );

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("ANY")
    );

GST_BOILERPLATE (Gstpgmenc, gst_pgmenc, GstElement,
    GST_TYPE_ELEMENT);

static void gst_pgmenc_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
	Gstpgmenc *filter = GST_PGMENC (object);
	switch (prop_id)
	{
	  //case PROP_WIDTH:
		//  filter->width = g_value_get_int (value); break;
	 // case PROP_HEIGHT:
	//	  filter->height = g_value_get_int (value); break;
	  case PROP_COLOR_SPACE:
		  filter->color_space = g_value_get_int (value); break;
	 // case PROP_BPP:
	//	  filter->bpp = g_value_get_int (value); break;
	  case PROP_BAYER_GRID:
		  filter->bayer_grid = g_value_get_int (value); break;
	  default:
		  G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec); break;
  }
}

static void gst_pgmenc_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
	Gstpgmenc *filter = GST_PGMENC (object);
	switch (prop_id)
	{
	 // case PROP_WIDTH:
	//	  g_value_set_enum (value, filter->width); break;
	 // case PROP_HEIGHT:
	//	  g_value_set_enum (value, filter->height); break;
	  case PROP_COLOR_SPACE:
		  g_value_set_enum (value, filter->color_space); break;
	 // case PROP_BPP:
	//	  g_value_set_enum (value, filter->bpp); break;
	  case PROP_BAYER_GRID:
		  g_value_set_enum (value, filter->bayer_grid); break;
	  default:
		  G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec); break;
  }
}

static gboolean gst_pgmenc_set_caps (GstPad * pad, GstCaps * caps)
{
	Gstpgmenc *filter = GST_PGMENC (gst_pad_get_parent (pad));;
	GstPad *otherpad;
	GstStructure *structure = gst_caps_get_structure (caps, 0);

	GST_DEBUG_OBJECT (filter, "The file type is : %s", gst_structure_get_name (structure));
	otherpad = (pad == filter->srcpad) ? filter->sinkpad : filter->srcpad;
	gst_object_unref (filter);

  return gst_pad_set_caps (otherpad, caps);
}

static GstFlowReturn gst_pgmenc_chain (GstPad * pad, GstBuffer * buf)
{
  Gstpgmenc *filter;
  filter = GST_PGMENC (GST_OBJECT_PARENT (pad));
  //GstCaps *caps = GST_PAD_CAPS(pad);
  GstCaps *caps = gst_pad_get_caps(pad);
  GstStructure *structure = gst_caps_get_structure (caps, 0);

  //if (filter->silent == FALSE)
    //g_print ("I'm plugged, therefore I'm in.\n");

  GST_DEBUG_OBJECT (filter, "The file type is : %s", gst_structure_get_name (structure));

  /* just push out the incoming buffer without touching it */
  return gst_pad_push (filter->srcpad, buf);
}

/* GObject vmethod implementations */

static void
gst_pgmenc_base_init (gpointer gclass)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS (gclass);

  gst_element_class_set_details_simple(element_class,
			"PGM file writer",
			"Encoder/Image",
			"Read PGM files",
			"Vadim Shcherbakov<<vadshe@gmail.com>>");

  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&src_factory));
  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&sink_factory));
}

/* initialize the pgmenc's class */
static void
gst_pgmenc_class_init (GstpgmencClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;

  gobject_class->set_property = gst_pgmenc_set_property;
  gobject_class->get_property = gst_pgmenc_get_property;

  g_object_class_install_property (gobject_class, PROP_SILENT,
      g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",
          FALSE, G_PARAM_READWRITE));
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void gst_pgmenc_init (Gstpgmenc * filter, GstpgmencClass * gclass)
{
	filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
	gst_pad_set_setcaps_function (filter->sinkpad, GST_DEBUG_FUNCPTR(gst_pgmenc_set_caps));
	gst_pad_set_getcaps_function (filter->sinkpad, GST_DEBUG_FUNCPTR(gst_pad_proxy_getcaps));
	gst_pad_set_chain_function   (filter->sinkpad, GST_DEBUG_FUNCPTR(gst_pgmenc_chain));

	filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
	gst_pad_set_getcaps_function (filter->srcpad, GST_DEBUG_FUNCPTR(gst_pad_proxy_getcaps));

	gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);
	gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);
	filter->silent = FALSE;
}

/* GstElement vmethod implementations */

/* this function handles the link with other elements */

/* chain function
 * this function does the actual processing
 */


/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean
pgmenc_init (GstPlugin * pgmenc)
{
  /* debug category for fltering log messages
   *
   * exchange the string 'Template pgmenc' with your description
   */
  GST_DEBUG_CATEGORY_INIT (gst_pgmenc_debug, "pgmenc",
      0, "Template pgmenc");

  return gst_element_register (pgmenc, "pgmenc", GST_RANK_NONE,
      GST_TYPE_PGMENC);
}

/* PACKAGE: this is usually set by autotools depending on some _INIT macro
 * in configure.ac and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use autotools to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "myfirstpgmenc"
#endif

/* gstreamer looks for this structure to register pgmencs
 *
 * exchange the string 'Template pgmenc' with your pgmenc description
 */
GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    "pgmenc",
    "Write pgm file ",
    pgmenc_init,
    PACKAGE_VERSION,
    "LGPL",
    "GStreamer",
    "http://gstreamer.net/"
)
