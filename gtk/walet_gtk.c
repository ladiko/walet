#include <walet_gtk.h>

static void cb_newpad (GstElement *decodebin, GstPad *pad, gboolean last, GtkWalet *gw)
{
	//GstElement 		*linkElement 	= (GstElement *) data;
	GstPad 			*sinkpad 		= gst_element_get_static_pad (gw->fakesink, "sink");
	GstCaps 		*caps 			= gst_pad_get_caps (pad);
	GstStructure 	*str;
	GstPadLinkReturn	ret;

	guint  size = gst_caps_get_size(caps), i;
	// Check media type
	//if (!g_strrstr (gst_structure_get_name (str), "video")) {
	//	gst_caps_unref (caps);
	//	gst_object_unref (sinkpad);
	//	return;
	//}
	for(i=0; i < size; i++) {
		str = gst_caps_get_structure (caps, i);
		g_print("Decoded cap %d : %s\n", i , gst_structure_get_name(str));
	}
	g_print("%s\n", gst_caps_to_string (caps));

	//gst_caps_unref (caps);

	if (GST_PAD_IS_LINKED(sinkpad))
	{
		g_object_unref(sinkpad);
	    return;
	}
	  // Link pads (decoder --> <link Element>)

	if((ret = gst_pad_link(pad, sinkpad))){
		g_print("Can't link pads erorr : %d\n", ret);
		g_object_unref(sinkpad);
		return;
	} //else  g_print("Link pads OK : %d\n", ret);

	//caps = gst_pad_get_negotiated_caps (sinkpad);
	//caps = gst_pad_peer_get_caps (sinkpad);
	//g_print("Negotiated cap: %s\n", gst_structure_get_name(gst_caps_get_structure(caps, 0)));
	str = gst_caps_get_structure (caps, 0);
	ret =  gst_structure_get_int (str, "width", &gw->gop->width);
	ret &= gst_structure_get_int (str, "height", &gw->gop->height);
	ret &= gst_structure_get_int (str, "bpp", &gw->gop->bpp);
	if (!ret) g_critical("Can't get image parameter");

	g_printf("width = %d  height = %d bpp = %d\n", gw->gop->width, gw->gop->height, gw->gop->bpp);

	gst_caps_unref (caps);

	gst_object_unref(sinkpad);
}

static void cb_handoff (GstElement *fakesink, GstBuffer *buffer, GstPad *pad, GtkWalet *gw)
{
	//GstPad 			*sinkpad 	= gst_element_get_static_pad (fakesink, "sink");
	//GstCaps 		*caps 		= gst_pad_get_caps (pad);
	GstCaps 		*caps 		= gst_buffer_get_caps (buffer);
	//GstStructure	*structure 	= gst_caps_get_structure (caps, 0);
	gboolean ret;
	GstStructure	*str;
	//guint8	*pix = GST_BUFFER_DATA(buffer);
	guint width, height, bpp;

	//g_printf("cb_handoff buffer = %p  size = %d\n", GST_BUFFER_DATA(buffer), GST_BUFFER_SIZE (buffer));

	str = gst_caps_get_structure (caps, 0);
	g_print("%s\n", gst_caps_to_string (caps));
	ret  = gst_structure_get_int (str, "width"	, &width);
	ret &= gst_structure_get_int (str, "height"	, &height);
	ret &= gst_structure_get_int (str, "bpp"	, &bpp);
	if (!ret) g_critical("Can't get image parameter");

	//Init Walet decoder only at first call on cb_handoff
	gw->gop = walet_decoder_init(width, height, BAYER, GRBG, bpp, 5, 1, 0);
	//Copy frame 0 to decoder pipeline
	frame_copy(gw->gop, 0, GST_BUFFER_DATA(buffer), NULL, NULL);
	draw_picture(gw->drawingarea0, gw->tp[0], gw->gop->frames[0].img[0].img, gw->gop->width, gw->gop->height);
	draw_picture(gw->drawingarea1, gw->tp[1], gw->gop->frames[0].img[0].img, gw->gop->width, gw->gop->height);


	//twoimg[0]->pixbuf[0] = gdk_pixbuf_new(GDK_COLORSPACE_RGB, 0, 8, gw->gop->width, gw->gop->height);
	//utils_grey_to_rgb(gw->gop->frames[0].img[0].img, gdk_pixbuf_get_pixels(twoimg[0]->pixbuf[0]), gw->gop->width, gw->gop->height);
	//gtk_widget_set_size_request(gw->drawingarea1, gw->gop->width, gw->gop->height);
	//g_printf("w = %d h = %d\n", gdk_pixbuf_get_width(twoimg[0]->pixbuf[0]), gdk_pixbuf_get_height(twoimg[0]->pixbuf[0]));

	//twoimg[1]->pixbuf[0] = gdk_pixbuf_new(GDK_COLORSPACE_RGB, 0, 8, gw->gop->width-1, gw->gop->height-1);
	//utils_grey_to_rgb(gop->frames[0].img[0].img, gdk_pixbuf_get_pixels(twoimg[1]->pixbuf[0]), sd->width, sd->height);
	//utils_bayer_to_rgb(gw->gop->frames[0].img[0].img, gdk_pixbuf_get_pixels(twoimg[1]->pixbuf[0]), gw->gop->width, gw->gop->height, gw->gop->bg);
	//gtk_widget_set_size_request(gw->drawingarea2, gw->gop->width-1, gw->gop->height-1);
	//g_printf("w = %d h = %d\n", gdk_pixbuf_get_width(twoimg[1]->pixbuf[0]), gdk_pixbuf_get_height(twoimg[1]->pixbuf[0]));

	//g_printf("width = %d height = %d\n", sd->width, sd->height);
	//gst_element_set_state (pipeline, GST_STATE_PAUSED);
}

static gboolean my_bus_callback (GstBus *bus, GstMessage *message,  gpointer data) //GtkWalet *gw)
{
	//GMainLoop *loop = (GMainLoop*) data;
	GtkWalet *gw = (GtkWalet*) data;
	//g_print ("Got %s message\n", GST_MESSAGE_TYPE_NAME (message));

	switch (GST_MESSAGE_TYPE (message)) {
		case GST_MESSAGE_ERROR: {
			GError *err;
			gchar *debug;

			gst_message_parse_error (message, &err, &debug);
			g_print ("Error: %s\n", err->message);
			g_error_free (err);
			g_free (debug);

			g_main_loop_quit (gw->loop);
			break;
		}
		case GST_MESSAGE_EOS:
		// end-of-stream
			g_main_loop_quit (gw->loop);
			break;
		default:
		// unhandled message
			break;
	}

  /* we want to be notified again the next time there is a message
   * on the bus, so returning TRUE (FALSE means we want to stop watching
   * for messages on the bus and our callback should not be called again)
   */
  return TRUE;
}


int main (int argc, char *argv[])
{
	//GTK structure allocate memory
	GtkWalet *gw = g_slice_new(GtkWalet);
	//Walet structure allocate memory
	//gw->gop = g_slice_new(GOP);

	// Allocate the memory needed by structs
	//twoimg[0]	= g_slice_new(TwoPixBuff);
	//twoimg[1]	= g_slice_new(TwoPixBuff);
	//twoimg[2]	= g_slice_new(TwoPixBuff);
	//twoimg[3]	= g_slice_new(TwoPixBuff);

	if (!g_thread_supported ()) g_thread_init (NULL);

	// Initialize the Gsreamer libraries
	gst_init(&argc, &argv);
	gw->loop = g_main_loop_new (NULL, FALSE);

	// Setup Gsreamer libraries
	gw->pipeline = gst_pipeline_new ("pipeline");

	gw->bus = gst_pipeline_get_bus (GST_PIPELINE (gw->pipeline));
	gst_bus_add_watch (gw->bus, my_bus_callback, gw);
	gst_object_unref  (gw->bus);

	gw->src 		= gst_element_factory_make("filesrc", "source");
	gw->dec 		= gst_element_factory_make("decodebin", "decoder");
	gw->pgmdec		= gst_element_factory_make("pgmdec", "decoder");
	gw->fakesink 	= gst_element_factory_make("fakesink", "sink");
	//appsink		= gst_element_factory_make(("appsink","appsink");

	if(!(gw->src && gw->dec && gw->fakesink && gw->pgmdec)){
		g_critical("Could not create pipeline elements");
		return FALSE;
	}
	//gst_bin_add_many (GST_BIN (pipeline), src, dec, fakesink, NULL);
	//gst_element_link_many (src, dec, NULL);
	gst_bin_add_many (GST_BIN (gw->pipeline), gw->src, gw->pgmdec, gw->fakesink, NULL);
	gst_element_link_many (gw->src, gw->pgmdec, gw->fakesink, NULL);

	g_signal_connect (gw->dec, "new-decoded-pad", G_CALLBACK (cb_newpad), gw);
	//g_signal_connect (dec, "unknown-type",    G_CALLBACK (cb_error),  NULL);
	g_object_set (gw->fakesink, "signal-handoffs", TRUE, NULL);
	g_signal_connect (gw->fakesink, "handoff", G_CALLBACK (cb_handoff), gw);




	// Initialize GTK+ libraries
	gtk_init (&argc, &argv);
	if (init_gw(gw) == FALSE) return 1; // error loading UI
	// Show the window
	gtk_widget_show (gw->window);
	// Enter GTK+ main loop
	gtk_main ();
	// Free memory we allocated for TutorialTextEditor struct
	g_slice_free (GOP, gw->gop);

	// Clean up gstreamer
	gst_element_set_state (gw->pipeline, GST_STATE_NULL);
	gst_object_unref (GST_OBJECT (gw->pipeline));
	g_slice_free (GtkWalet, gw);

	return 0;
}
