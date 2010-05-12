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
 * SECTION:element-bay2rgb
 *
 * FIXME:Describe bay2rgb here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! bay2rgb ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>
#include <gst/video/video.h>
#include <gst/base/gsttypefindhelper.h>

#include "gstbay2rgb.h"

GST_DEBUG_CATEGORY_STATIC (gst_bay2rgb_debug);
#define GST_CAT_DEFAULT gst_bay2rgb_debug

#define oe(a,x)	(a ? x%2 : (x+1)%2)

/* Filter signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

enum
{
	PROP_WIDTH,
	PROP_HEIGHT,
	PROP_COLOR_SPACE,
	PROP_BPP,
	PROP_BAYER_GRID,
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
		//"endianness = (int) BYTE_ORDER, "
		"width = " GST_VIDEO_SIZE_RANGE ", "
		"height = " GST_VIDEO_SIZE_RANGE ", "
		"bpp = (int) [8,14], "
		"framerate = " GST_VIDEO_FPS_RANGE )
    );

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    //GST_STATIC_CAPS_ANY //("image/x-pgm")
    //GST_STATIC_CAPS (GST_VIDEO_CAPS_YUV ("I420"))
    GST_STATIC_CAPS (GST_VIDEO_CAPS_RGB";")
    );

GST_BOILERPLATE (Gstbay2rgb, gst_bay2rgb, GstElement, GST_TYPE_ELEMENT);

static void gst_bay2rgb_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec)
{
  Gstbay2rgb *filter = GST_BAY2RGB (object);
	switch (prop_id)
	{
	  case PROP_WIDTH:
		  filter->width = g_value_get_uint (value); break;
	  case PROP_HEIGHT:
		  filter->height = g_value_get_uint (value); break;
	  case PROP_COLOR_SPACE:
		  filter->color_space = g_value_get_boolean (value); break;
	  case PROP_BPP:
		  filter->bpp = g_value_get_uint (value); break;
	  case PROP_BAYER_GRID:
		  filter->bayer_grid = g_value_get_uint (value); break;
	  default:
		  G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec); break;
	}
}

static void gst_bay2rgb_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec)
{
  Gstbay2rgb *filter = GST_BAY2RGB (object);

	switch (prop_id)
	{
	  case PROP_WIDTH:
		  g_value_set_uint (value, filter->width); break;
	  case PROP_HEIGHT:
		  g_value_set_uint (value, filter->height); break;
	  case PROP_COLOR_SPACE:
		  g_value_set_boolean (value, filter->color_space); break;
	  case PROP_BPP:
		  g_value_set_uint(value, filter->bpp); break;
	  case PROP_BAYER_GRID:
		  g_value_set_uint (value, filter->bayer_grid); break;
	  default:
		  G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec); break;
	}
}

/* this function handles the link with other elements */

static gboolean gst_bay2rgb_set_caps (GstPad * pad, GstCaps * caps)
{
	Gstbay2rgb *filter = GST_BAY2RGB (gst_pad_get_parent (pad));
	GstStructure *structure = gst_caps_get_structure (caps, 0);

	gboolean ret;

	//if (!gst_pad_set_caps (filter->srcpad, caps))
	//    return FALSE;
	if (!gst_pad_set_caps (filter->srcpad, caps)) {
		GST_ELEMENT_ERROR (filter, CORE, NEGOTIATION, (NULL), ("Some debug information here"));
		return GST_FLOW_ERROR;
	}


	GST_DEBUG_OBJECT (filter, "The file type is : %s", gst_structure_get_name (structure));
	ret = gst_structure_get_int (structure, "width", &filter->width);
	if (ret != GST_FLOW_OK) GST_DEBUG_OBJECT (filter, "gst_pgmdec_set_caps get width failed: %s", gst_flow_get_name (ret));

	ret = gst_structure_get_int (structure, "height", &filter->height);
	if (ret != GST_FLOW_OK) GST_DEBUG_OBJECT (filter, "gst_pgmdec_set_caps get height failed: %s", gst_flow_get_name (ret));

	ret = gst_structure_get_int(structure, "bpp", &filter->bpp);
	if (ret != GST_FLOW_OK) GST_DEBUG_OBJECT (filter, "gst_pgmdec_set_caps get height failed: %s", gst_flow_get_name (ret));

	GST_DEBUG_OBJECT (filter, "width = %d height = %d bpp = %d",
			  filter->width, filter->height, filter->bpp);
	return TRUE;
}
/*
static guint8* bayer_to_rgb(guint8 *img, guint8 *rgb, guint32 w, guint32 h, BayerGrid bay)
{
	guint32 x, y, wy, xwy, xwy3, y2, x2, a, b, w1 = w-1, h1 = h-1;

	switch(bay){
		case(BGGR):{ a = 1; b = 1; break;}
		case(GRBG):{ a = 0; b = 1; break;}
		case(GBRG):{ a = 1; b = 0; break;}
		case(RGGB):{ a = 0; b = 0; break;}
	}
	printf("a = %d b = %d\n",a,b);

	for(y=0; y < h1; y++){
		for(x=0; x < w1; x++){
			y2 = oe(a,y);
			x2 = oe(b,x);
			wy = w*y;
			xwy = x + wy;
			xwy3 = (x + w1*y)*3;
			rgb[xwy3    ] = y2 ? (x2 ?  img[xwy    ] : img[xwy+1]) : (x2 ? img[xwy+w] : img[xwy+w+1]);
			rgb[xwy3 + 1] = y2 ? (x2 ? (img[xwy+w  ] + img[xwy+1])>>1 :   (img[xwy  ] + img[xwy+w+1])>>1) :
								 (x2 ? (img[xwy+w+1] + img[xwy  ])>>1 :   (img[xwy+1] + img[xwy+w  ])>>1);
			rgb[xwy3 + 2] = y2 ? (x2 ?  img[xwy+w+1] : img[xwy+w]) : (x2 ? img[xwy+1] : img[xwy    ]);
		}
	}
	return rgb;
}
*/
static GstFlowReturn gst_bay2rgb_chain (GstPad * pad, GstBuffer * in)
{
	Gstbay2rgb *filter = GST_BAY2RGB (GST_OBJECT_PARENT (pad));
	GstFlowReturn ret;
	GstBuffer *out;
	GstCaps *caps;
	GstStructure *structure = gst_caps_get_structure (caps, 0);
	//gint8 *outbuf;


	//ret = gst_structure_get_int (structure, "width", &filter->width);
	//if (ret != GST_FLOW_OK) GST_DEBUG_OBJECT (filter, "gst_pgmdec_set_caps get width failed: %s", gst_flow_get_name (ret));

	//ret = gst_structure_get_int (structure, "height", &filter->height);
	//if (ret != GST_FLOW_OK) GST_DEBUG_OBJECT (filter, "gst_pgmdec_set_caps get height failed: %s", gst_flow_get_name (ret));

	//ret = gst_structure_get_int(structure, "bpp", &filter->bpp);
	//if (ret != GST_FLOW_OK) GST_DEBUG_OBJECT (filter, "gst_pgmdec_set_caps get height failed: %s", gst_flow_get_name (ret));

	//GST_DEBUG_OBJECT (filter, "width = %d height = %d bpp = %d",
	//		  filter->width, filter->height, filter->bpp);

	caps = gst_caps_new_simple ("video/x-raw-rgb",
	        "width", G_TYPE_UINT, filter->width,
	        "height", G_TYPE_UINT, filter->height,NULL);

	ret = gst_pad_alloc_buffer_and_set_caps(filter->srcpad, GST_BUFFER_OFFSET_NONE,
			filter->width*filter->height*3, caps, &out);
	if (ret != GST_FLOW_OK) GST_DEBUG_OBJECT (filter, "gst_pad_alloc_buffer failed: %s", gst_flow_get_name (ret));
	//gst_caps_unref (caps);


	//ret = gst_pad_alloc_buffer_and_set_caps (filter->srcpad, GST_BUFFER_OFFSET_NONE,
	//		filter->width*filter->height*3, GST_PAD_CAPS (filter->srcpad), &out);
	//ret = gst_pad_alloc_buffer_and_set_caps (filter->srcpad, GST_BUFFER_OFFSET_NONE,
	//		filter->width*filter->height*3, caps, &out);


	//ret = gst_pad_alloc_buffer (filter->srcpad, GST_BUFFER_OFFSET_NONE,
	//		1000000, GST_PAD_CAPS (filter->srcpad), &out); //filter->width*filter->height*3
	//gst_buffer_unref (in);
	//gst_buffer_unref (in);

    //caps = gst_type_find_helper_for_buffer (GST_OBJECT (filter), out, NULL);
    //if (caps) {
    //  gst_buffer_set_caps (out, caps);
    //  gst_pad_set_caps (filter->srcpad, caps);
   //   gst_pad_use_fixed_caps (filter->srcpad);
   //   gst_caps_unref (caps);
   // } else {
      /* FIXME: shouldn't we queue output buffers until we have a type? */
  //  }
	//outbuf = (gint8 *) GST_BUFFER_DATA (out);
	//caps = gst_caps_new_simple (
	//		"video/x-raw-rgb",
	//		"width", G_TYPE_UINT, filter->width,
	//		"height", G_TYPE_UINT, filter->height,NULL);
	//if (!gst_pad_set_caps (filter->srcpad, caps)) {
	//		  GST_ELEMENT_ERROR (filter, CORE, NEGOTIATION, (NULL),
	//				  ("Some debug information here"));
	//		  return GST_FLOW_ERROR;
	//}
    //gst_pad_use_fixed_caps (filter->srcpad);
   // gst_caps_unref (caps);



	/* just push out the incoming buffer without touching it */
	return gst_pad_push (filter->srcpad, out);
	//return gst_pad_push (filter->srcpad, in);
}

/* GObject vmethod implementations */

static void gst_bay2rgb_base_init (gpointer gclass)
{
	GstElementClass *element_class = GST_ELEMENT_CLASS (gclass);

	gst_element_class_set_details_simple(element_class,
			"PGM file format reader",
			"Filter/Effect/Video",
			"Converts raw bayer data to RGB images with selectable demosaicing methods",
			"Vadim Shcherbakov<<vadshe@gmail.com>>");

	gst_element_class_add_pad_template (element_class,
			gst_static_pad_template_get (&src_factory));
	gst_element_class_add_pad_template (element_class,
			gst_static_pad_template_get (&sink_factory));
}

/* initialize the bay2rgb's class */
static void gst_bay2rgb_class_init (Gstbay2rgbClass * klass)
{
	GObjectClass *gobject_class;
	GstElementClass *gstelement_class;

	gobject_class = (GObjectClass *) klass;
	gstelement_class = (GstElementClass *) klass;

	gobject_class->set_property = gst_bay2rgb_set_property;
	gobject_class->get_property = gst_bay2rgb_get_property;

	g_object_class_install_property (gobject_class, PROP_COLOR_SPACE,
		  g_param_spec_boolean ("color_space", "Color Space", "FALSE = GREY, TRUE = BAYER",
        FALSE, G_PARAM_READWRITE));
	g_object_class_install_property (gobject_class, PROP_BAYER_GRID,
		  g_param_spec_uint  ("bayer_grid", "Bayer Grid", "0 = BGGR, 1 = GRBG, 2 = GBRG, 3 = RGGB",
		  0, 3, 0, G_PARAM_READWRITE));
//  g_object_class_install_property (gobject_class, PROP_SILENT,
 //     g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",
 //         FALSE, G_PARAM_READWRITE));
	GST_DEBUG_CATEGORY_INIT (gst_bay2rgb_debug, "bay2rgb", 0,
			    "Bayer to rgb transform");
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void gst_bay2rgb_init (Gstbay2rgb * filter, Gstbay2rgbClass * gclass)
{
  filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
  //gst_pad_set_setcaps_function (filter->sinkpad, GST_DEBUG_FUNCPTR(gst_bay2rgb_set_caps));
  gst_pad_set_getcaps_function (filter->sinkpad, GST_DEBUG_FUNCPTR(gst_pad_proxy_getcaps));
  gst_pad_set_chain_function (filter->sinkpad, GST_DEBUG_FUNCPTR(gst_bay2rgb_chain));

  filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
  gst_pad_set_getcaps_function (filter->srcpad, GST_DEBUG_FUNCPTR(gst_pad_proxy_getcaps));

  gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);
  gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);
  //filter->silent = FALSE;
}

/* chain function
 * this function does the actual processing
 */

/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean bay2rgb_init (GstPlugin * bay2rgb)
{
  /* debug category for fltering log messages
   *
   * exchange the string 'Template bay2rgb' with your description
   */
  GST_DEBUG_CATEGORY_INIT (gst_bay2rgb_debug, "bay2rgb",
      0, "Template bay2rgb");

  return gst_element_register (bay2rgb, "bay2rgb", GST_RANK_NONE,
      GST_TYPE_BAY2RGB);
}

/* PACKAGE: this is usually set by autotools depending on some _INIT macro
 * in configure.ac and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use autotools to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "myfirstbay2rgb"
#endif

/* gstreamer looks for this structure to register bay2rgbs
 *
 * exchange the string 'Template bay2rgb' with your bay2rgb description
 */
GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    "bay2rgb",
    "Fast bayer to RGB transform",
    bay2rgb_init,
    PACKAGE_VERSION,
    "LGPL",
    "GStreamer",
    "http://gstreamer.net/"
)
