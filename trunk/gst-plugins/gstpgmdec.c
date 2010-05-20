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
 * SECTION:element-pgmdec
 *
 * FIXME:Describe pgmdec here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! pgmdec ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>
#include <gst/base/gstadapter.h>
#include <gst/video/video.h>
#include <gst/base/gstbytereader.h>

#include "gstpgmdec.h"
#include <string.h>
//#include "range-coder/range-coder.h"

GST_DEBUG_CATEGORY_STATIC (gst_pgmdec_debug);
#define GST_CAT_DEFAULT gst_pgmdec_debug

GST_BOILERPLATE (Gstpgmdec, gst_pgmdec, GstElement,GST_TYPE_ELEMENT);

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
static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    //GST_STATIC_CAPS_ANY
    GST_STATIC_CAPS (
    		"video/x-raw-bayer,"
    		//"endianness = (int) BYTE_ORDER, "
    		"width = " GST_VIDEO_SIZE_RANGE ", "
    		"height = " GST_VIDEO_SIZE_RANGE ", "
    		"bpp = (int) [8,14], "
    		"framerate = " GST_VIDEO_FPS_RANGE "; "
    		//"video/x-raw-gray,"
    		//"endianness = (int) BYTE_ORDER, "
    		//"width = " GST_VIDEO_SIZE_RANGE ", "
    		//"height = " GST_VIDEO_SIZE_RANGE ", "
    		//"bpp = (int) [8,14], "
    		//"framerate = " GST_VIDEO_FPS_RANGE
    		)
    );

static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS_ANY //("image/x-pgm")
    );


static void gst_pgmdec_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
	Gstpgmdec *filter = GST_PGMDEC (object);
	switch (prop_id)
	{
	  case PROP_WIDTH:
		  filter->width = g_value_get_int (value); break;
	  case PROP_HEIGHT:
		  filter->height = g_value_get_int (value); break;
	  case PROP_COLOR_SPACE:
		  filter->color_space = g_value_get_boolean (value); break;
	  case PROP_BPP:
		  filter->bpp = g_value_get_int (value); break;
	  case PROP_BAYER_GRID:
		  filter->bayer_grid = g_value_get_int (value); break;
	  default:
		  G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec); break;
  }
}

static void gst_pgmdec_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
	Gstpgmdec *filter = GST_PGMDEC (object);
	switch (prop_id)
	{
	  case PROP_WIDTH:
		  g_value_set_int (value, filter->width); break;
	  case PROP_HEIGHT:
		  g_value_set_int (value, filter->height); break;
	  case PROP_COLOR_SPACE:
		  g_value_set_boolean (value, filter->color_space); break;
	  case PROP_BPP:
		  g_value_set_int(value, filter->bpp); break;
	  case PROP_BAYER_GRID:
		  g_value_set_int (value, filter->bayer_grid); break;
	  default:
		  G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec); break;
  }
}

static void gst_pgmdec_finalize (GObject * object)
{
	//Gstpgmdec *filter = GST_PGMDEC (object);

	//gst_bz2dec_decompress_end (b);

	G_OBJECT_CLASS (parent_class)->finalize (object);
}

/* this function handles the link with other elements */

static gboolean gst_pgmdec_set_caps (GstPad * pad, GstCaps * caps)
{
	Gstpgmdec *filter = GST_PGMDEC (gst_pad_get_parent (pad));
  //GstPad *otherpad;
	GstStructure *structure = gst_caps_get_structure (caps, 0);
	gboolean ret;

	ret = gst_structure_get_int (structure, "width", &filter->width);
	if (ret != GST_FLOW_OK) {
	      GST_DEBUG_OBJECT (filter, "gst_pgmdec_set_caps get width failed: %s", gst_flow_get_name (ret));
	      //return FALSE;
	      //gst_bz2dec_decompress_init (b);
	  }

	ret = gst_structure_get_int (structure, "height", &filter->height);
	if (ret != GST_FLOW_OK) {
	      GST_DEBUG_OBJECT (filter, "gst_pgmdec_set_caps get height failed: %s", gst_flow_get_name (ret));
	      //return FALSE;
	      //gst_bz2dec_decompress_init (b);
	  }
	GST_DEBUG_OBJECT (filter, "The file type is : %s", gst_structure_get_name (structure));
  //otherpad = (pad == filter->srcpad) ? filter->sinkpad : filter->srcpad;
  //gst_object_unref (filter);

  //return gst_pad_set_caps (otherpad, caps);
  return TRUE;
}


static GstFlowReturn gst_pgmdec_chain (GstPad * pad, GstBuffer * in)
{
	GstFlowReturn ret = GST_FLOW_OK;
	GstBuffer *out;
	Gstpgmdec *filter = GST_PGMDEC (GST_OBJECT_PARENT (pad));
	//GstCaps *caps = GST_PAD_CAPS (filter->srcpad);
	GstCaps *caps;
	guint byts=0;
	gchar line[4][20];
	//const gchar *l;
	gint  i;
	//GstStructure *structure = gst_caps_get_structure (caps, 0);

	gchar *outbuf;
	gchar *inbuf;

	//Get pgm header
	if(!GST_BUFFER_OFFSET(in)){
		inbuf = (char *) GST_BUFFER_DATA (in);
		byts = sscanf(inbuf, "%s%s%s%s", line[0], line[1], line[2], line[3]);
		if (strcmp(line[0], "P5") != 0) {
			GST_WARNING ("It's not PGM file");
			return FALSE;
		}
		filter->width = atoi(line[1]);
		filter->height = atoi(line[2]);
		//bpp = atoi(line[3]);
		filter->bpp = (atoi(line[3]) > 256) ? 16 : 8;
		for(i=0; i<4; i++) byts += strlen(line[i]);
		filter->size = (filter->bpp == 8) ? filter->width*filter->height : filter->width*filter->height*2;
		filter->buff = &inbuf[byts];

		GST_DEBUG_OBJECT (filter, "The file type is : %s width = %d height = %d bpp = %d",
				line[0], filter->width, filter->height, filter->bpp);
		//GST_DEBUG_OBJECT (filter, "DATA = %p SIZE = %d OFFSET = %d",GST_BUFFER_DATA (in), GST_BUFFER_SIZE (in),GST_BUFFER_OFFSET (in));
	}

	//Check for the buffer size
	if(GST_BUFFER_OFFSET(in)+GST_BUFFER_SIZE (in) < filter->size) {
		//GST_DEBUG_OBJECT (filter, "DATA = %p SIZE = %d OFFSET = %d",GST_BUFFER_DATA (in), GST_BUFFER_SIZE (in),GST_BUFFER_OFFSET (in));
		return GST_FLOW_OK;
	}
	gst_buffer_make_metadata_writable(in);
	GST_BUFFER_DATA (in) = filter->buff;
	GST_BUFFER_SIZE (in) = filter->size;
	GST_BUFFER_OFFSET(in)= 0;
	GST_DEBUG_OBJECT (filter, "DATA = %p SIZE = %d OFFSET = %d",GST_BUFFER_DATA(in), GST_BUFFER_SIZE(in), GST_BUFFER_OFFSET(in));

	caps = gst_caps_new_simple ("video/x-raw-bayer",
								"width", G_TYPE_INT, filter->width,
								"height", G_TYPE_INT, filter->height,
								"bpp", G_TYPE_INT, filter->bpp,
								"framerate", GST_TYPE_FRACTION, 0, 1,
								NULL);
	gst_buffer_set_caps(in, caps);
	gst_pad_set_caps (filter->srcpad, caps);
	gst_pad_use_fixed_caps (filter->srcpad);
	gst_caps_unref (caps);

	  /*
	  img_size = filter->width*filter->height;
	  src_buff_size = filter->bpp > 255 ? img_size<<1 : img_size;

	  ret = gst_pad_alloc_buffer (filter->srcpad, GST_BUFFER_OFFSET_NONE,
			  src_buff_size, GST_PAD_CAPS (filter->srcpad), &out);
	  if (ret != GST_FLOW_OK) {
		  GST_DEBUG_OBJECT (filter, "gst_pad_alloc_buffer failed: %s", gst_flow_get_name (ret));
		      //gst_bz2dec_decompress_init (b);
	  }
	 outbuf = (gint8 *) GST_BUFFER_DATA (out);

	 if(filter->bpp > 255) for(i=0; i< img_size; i++);
	 else for(i=0; i< img_size; i++) gst_byte_reader_get_int8 (&reader, &outbuf[i]); //outbuf[i] = inbuf[i];

	 caps = gst_caps_new_simple ("video/x-raw-bayer",
	        "width", G_TYPE_UINT, filter->width,
	        "height", G_TYPE_UINT, filter->height,
	        "bpp", G_TYPE_UINT, filter->bpp,
	        "framerate", G_TYPE_UINT, 1,NULL);
	  if (!gst_pad_set_caps (filter->srcpad, caps)) {
		  GST_ELEMENT_ERROR (filter, CORE, NEGOTIATION, (NULL), ("Some debug information here"));
		  return GST_FLOW_ERROR;
	  }
	  gst_buffer_set_caps (out, caps);
	  gst_pad_set_caps (filter->srcpad, caps);
      gst_pad_use_fixed_caps (filter->srcpad);
      gst_caps_unref (caps);

	*/
  //if (filter->silent == FALSE)
   // g_print ("I'm plugged, therefore I'm in.\n");

  /* just push out the incoming buffer without touching it */
	ret = gst_pad_push(filter->srcpad, in);
	//GST_DEBUG_OBJECT (filter, "gst_pad_push = %d",  ret);
	return ret;
}

static gboolean gst_pgmdec_sink_event (GstPad * pad, GstEvent * event)
{
	gboolean ret = TRUE;
	Gstpgmdec *filter = GST_PGMDEC (GST_OBJECT_PARENT (pad));

	GST_DEBUG_OBJECT (filter, "event : %s", GST_EVENT_TYPE_NAME (event));

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_FLUSH_STOP:
      GST_DEBUG_OBJECT (filter, "Aborting decompress");
      //jpeg_abort_decompress (&dec->cinfo);
      gst_segment_init (&filter->segment, GST_FORMAT_UNDEFINED);
      //gst_jpeg_dec_reset_qos (dec);
      break;
      /*
    case GST_EVENT_NEWSEGMENT:{
      gboolean update;
      gdouble rate, applied_rate;
      GstFormat format;
      gint64 start, stop, position;

      gst_event_parse_new_segment_full (event, &update, &rate, &applied_rate,
          &format, &start, &stop, &position);

      GST_DEBUG_OBJECT (filter, "Got NEWSEGMENT [%" GST_TIME_FORMAT
          " - %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT "]",
          GST_TIME_ARGS (start), GST_TIME_ARGS (stop),
          GST_TIME_ARGS (position));

      gst_segment_set_newsegment_full (&filter->segment, update, rate,
          applied_rate, format, start, stop, position);

      break;
    }*/
    default:
      break;
  }

  ret = gst_pad_push_event (filter->srcpad, event);

  return ret;
}
/* GObject vmethod implementations */

static void gst_pgmdec_base_init (gpointer gclass)
{
	GstElementClass *element_class = GST_ELEMENT_CLASS (gclass);
	gst_element_class_set_details_simple(element_class,
			"PGM file format reader",
			"Decoder/Image",
			"Read PGM files",
			"Vadim Shcherbakov<<vadshe@gmail.com>>");

	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&src_factory));
	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&sink_factory));
}

static GstStateChangeReturn gst_pgmdec_change_state (GstElement * element, GstStateChange transition)
{
	 //Gstpgmdec *filter = GST_PGMDEC (element);
	 GstStateChangeReturn ret;

	 ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);
	 if (ret != GST_STATE_CHANGE_SUCCESS) return ret;

	 switch (transition) {
		 case GST_STATE_CHANGE_PAUSED_TO_READY:
			 //gst_bz2dec_decompress_init (b);
			 break;
		 default:
			 break;
	 }
  return ret;
}

/* initialize the pgmdec's class */
static void gst_pgmdec_class_init (GstpgmdecClass * klass)
{
	GObjectClass *gobject_class;
	GstElementClass *gstelement_class;

	gobject_class = (GObjectClass *) klass;
	gstelement_class = (GstElementClass *) klass;

	gstelement_class->change_state = GST_DEBUG_FUNCPTR (gst_pgmdec_change_state);

	gobject_class->finalize = gst_pgmdec_finalize;
	gobject_class->set_property = gst_pgmdec_set_property;
	gobject_class->get_property = gst_pgmdec_get_property;

	g_object_class_install_property (gobject_class, PROP_COLOR_SPACE,
		  g_param_spec_boolean ("color_space", "Color Space", "FALSE = GREY, TRUE = BAYER",
          FALSE, G_PARAM_READWRITE));
	g_object_class_install_property (gobject_class, PROP_BAYER_GRID,
		  g_param_spec_uint  ("bayer_grid", "Bayer Grid", "0 = BGGR, 1 = GRBG, 2 = GBRG, 3 = RGGB",
		  0, 3, 0, G_PARAM_READWRITE));
	//g_object_class_install_property (gobject_class, PROP_SILENT,
	//    g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",
	//        FALSE, G_PARAM_READWRITE));
	GST_DEBUG_CATEGORY_INIT (gst_pgmdec_debug, "pgmdec", 0,
  			    "PGM file reader");
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void gst_pgmdec_init (Gstpgmdec * filter, GstpgmdecClass * gclass)
{
	filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
	//gst_pad_set_setcaps_function (filter->sinkpad, GST_DEBUG_FUNCPTR(gst_pgmdec_set_caps));
	//gst_pad_set_getcaps_function (filter->sinkpad, GST_DEBUG_FUNCPTR(gst_pad_proxy_getcaps));
	gst_pad_set_chain_function (filter->sinkpad, GST_DEBUG_FUNCPTR(gst_pgmdec_chain));
	gst_pad_set_event_function (filter->sinkpad, GST_DEBUG_FUNCPTR (gst_pgmdec_sink_event));
	gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);

	filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
	//gst_pad_set_getcaps_function (filter->srcpad, GST_DEBUG_FUNCPTR(gst_pad_proxy_getcaps));
	//gst_pad_use_fixed_caps (filter->srcpad);
	gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);

  //filter->silent = FALSE;
}

/* GstElement vmethod implementations */

/* chain function
 * this function does the actual processing
 */

/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean
pgmdec_init (GstPlugin * pgmdec)
{
  /* debug category for fltering log messages
   *
   * exchange the string 'Template pgmdec' with your description
   */
  GST_DEBUG_CATEGORY_INIT (gst_pgmdec_debug, "pgmdec",
      0, "Template pgmdec");

  return gst_element_register (pgmdec, "pgmdec", GST_RANK_NONE,
      GST_TYPE_PGMDEC);
}

/* PACKAGE: this is usually set by autotools depending on some _INIT macro
 * in configure.ac and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use autotools to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "myfirstpgmdec"
#endif

/* gstreamer looks for this structure to register pgmdecs
 *
 * exchange the string 'Template pgmdec' with your pgmdec description
 */
GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    "pgmdec",
    "Read pgm file format",
    pgmdec_init,
    PACKAGE_VERSION,
    "LGPL",
    "GStreamer",
    "http://gstreamer.net/"
)
