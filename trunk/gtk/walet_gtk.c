#include <walet_gtk.h>

static void cb_newpad (GstElement *decodebin, GstPad *pad, gboolean last, GtkWalet *gw)
{
	//GstElement 		*linkElement 	= (GstElement *) data;
	GstPad 			*sinkpad 	= gst_element_get_static_pad (gw->fakesink, "sink");
	GstCaps 		*caps 		= gst_pad_get_caps (pad);
	GstStructure 	*str;
	GstPadLinkReturn	ret;

	//GError* err = g_error_new(G_FILE_ERROR, G_FILE_ERROR_BADF, "It's not video file");
	//GstMessage * message = gst_message_new_error((GstObject *)gw->fakesink ,err, NULL);


	guint  i;
	// Check media type
	if (!g_strrstr (gst_structure_get_name (gst_caps_get_structure (caps, 0)), "video")) {
		gst_caps_unref (caps);
		gst_object_unref (sinkpad);
		//gst_element_post_message(gw->fakesink, message);
		g_print("It's not video file\n");
		//gst_bus_post(gw->bus, message);
		gst_caps_unref (caps);
		gst_object_unref(sinkpad);
		//gst_object_unref (err);
		//gst_object_unref(message);
		return;
	}
	for(i=0; i < gst_caps_get_size(caps); i++) {
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
	//str = gst_caps_get_structure (caps, 0);
	//ret =  gst_structure_get_int (str, "width", &gw->gop->width);
	//ret &= gst_structure_get_int (str, "height", &gw->gop->height);
	//ret &= gst_structure_get_int (str, "bpp", &gw->gop->bpp);
	//if (!ret) g_critical("Can't get image parameter");

	//g_printf("width = %d  height = %d bpp = %d\n", gw->gop->width, gw->gop->height, gw->gop->bpp);


	gst_caps_unref (caps);
	gst_object_unref(sinkpad);
	//g_print("cb_newpad\n");
}

static void cb_handoff (GstElement *fakesink, GstBuffer *buffer, GstPad *pad, GtkWalet *gw)
{
	//GstPad 			*sinkpad 	= gst_element_get_static_pad (fakesink, "sink");
	//GstCaps 		*caps 		= gst_pad_get_caps (pad);
	GstCaps 		*caps 		= gst_buffer_get_caps (buffer);
	//GstStructure	*structure 	= gst_caps_get_structure (caps, 0);
	//gboolean ret;
	GstStructure	*str;
	//guint8	*pix = GST_BUFFER_DATA(buffer);
	GstStateChangeReturn ret;
	GstState state;
	gint width, height, bpp , fn = 0, w, h;
	//uchar *p[4];

	clock_t start, end;
	double time=0., tmp;
	struct timeval tv;

	//g_print("cb_handoff\n");
	//g_printf("cb_handoff buffer = %p  size = %d\n", GST_BUFFER_DATA(buffer), GST_BUFFER_SIZE (buffer));

	str = gst_caps_get_structure (caps, 0);
	g_print("%s\n", gst_caps_to_string(caps));
	ret  = gst_structure_get_int (str, "width"	, &width);
	ret &= gst_structure_get_int (str, "height"	, &height);
	ret &= gst_structure_get_int (str, "bpp"	, &bpp);
	if (!ret) g_critical("Can't get image parameter");
	printf("width = %d height = %d bpp = %d\n", width, height, bpp);

	//Init Walet decoder only at first call on cb_handoff
	if(!gw->walet_init){
		//gw->gop->bpp = 16;
		gw->wc.w 			= width;	/// Image width
		gw->wc.h			= height;	/// Image width
		gw->wc.color		= BAYER;	/// Color space
		gw->wc.bg			= RGGB;		/// Bayer grid pattern
		gw->wc.bpp			= bpp;		/// Image bits per pixel.
		gw->wc.steps		= 4;  		/// DWT steps.
		gw->wc.gop_size		= 1;		/// GOP size
		gw->wc.rates		= 0;		/// Frame rates
		gw->wc.comp			= 20;		/// Compression in times to original image if 1 - lossless 0 - without any compression.
		gw->wc.fb			= FR_5_3;	/// Filters bank for wavelet transform.
		gw->wc.rt			= FAST;		/// Range coder type
		gw->wc.mv			= 12;		/// The motion vector search in pixeles.
		walet_encoder_init(&gw->gop, &gw->wc);
		gw->walet_init = 1;
	}
	//printf("%d\n", strncmp("video/x-raw-rgb", gst_caps_to_string(caps),15));
	//printf("%d\n", strncmp("video/x-raw-bayer", gst_caps_to_string(caps),17));
	if(!strncmp("video/x-raw-rgb", gst_caps_to_string(caps),15)){
		//Copy frame 0 to decoder pipeline
		//frame_copy(gw->gop, 0, GST_BUFFER_DATA(buffer), NULL, NULL);
		//frame_copy(gw->gop, 1, GST_BUFFER_DATA(buffer), NULL, NULL);

		new_buffer (gw->orig[0], gw->wc.w, gw->wc.h);
		utils_draw(GST_BUFFER_DATA(buffer), gdk_pixbuf_get_pixels(gw->orig[0]->pxb), gw->wc.w, gw->wc.h);
		//utils_grey_draw(gw->gop->frames[gw->gop->cur_gop_frame].img[0].img, gdk_pixbuf_get_pixels(gw->orig[0]->pxb), gw->gop->width, gw->gop->height);
		gtk_widget_queue_draw(gw->drawingarea[0]);

		utils_ppm_to_bayer(GST_BUFFER_DATA(buffer), gw->gop.buf, gw->wc.w, gw->wc.h);
		frame_input(&gw->gop, 0, &gw->wc, gw->gop.buf, NULL, NULL);

		new_buffer (gw->orig[1], gw->wc.w, gw->wc.h);
		utils_grey_draw(gw->gop.frames[gw->gop.cur_gop_frame].b.pic, gdk_pixbuf_get_pixels(gw->orig[1]->pxb), gw->wc.w, gw->wc.h);
		gtk_widget_queue_draw(gw->drawingarea[1]);

		new_buffer (gw->orig[2], gw->wc.w-1, gw->wc.h-1);
		utils_bayer_draw(gw->gop.frames[gw->gop.cur_gop_frame].b.pic, gdk_pixbuf_get_pixels(gw->orig[2]->pxb), gw->wc.w, gw->wc.h, gw->wc.bg);
		gtk_widget_queue_draw(gw->drawingarea[2]);


	} else if (!strncmp("video/x-raw-bayer", gst_caps_to_string(caps),17)){

		//Copy frame 0 to decoder pipeline
		printf("video/x-raw-bayer\n");
		frame_input(&gw->gop, 0, &gw->wc, GST_BUFFER_DATA(buffer), NULL, NULL);

		new_buffer (gw->orig[0], gw->gop.frames[0].b.w, gw->gop.frames[0].b.h);
		utils_grey_draw(gw->gop.frames[0].b.pic, gdk_pixbuf_get_pixels(gw->orig[0]->pxb), gw->gop.frames[0].b.w, gw->gop.frames[0].b.h);
		gtk_widget_queue_draw(gw->drawingarea[0]);

		new_buffer (gw->orig[1], gw->gop.frames[0].b.w, gw->gop.frames[0].b.h);
		utils_bayer_draw(gw->gop.frames[0].b.pic, gdk_pixbuf_get_pixels(gw->orig[1]->pxb), gw->gop.frames[0].b.w, gw->gop.frames[0].b.h, gw->wc.bg);
		gtk_widget_queue_draw(gw->drawingarea[1]);


	}
	//ret = gst_element_get_state (GST_ELEMENT (gw->pipeline), &state, NULL, 0);
	//g_printf("The current state = %d ret = %d\n", state, ret);
	//gst_element_set_state (gw->pipeline, GST_STATE_NULL);
	ret = gst_element_set_state (gw->pipeline, GST_STATE_PAUSED);

	//ret = gst_element_get_state (GST_ELEMENT (gw->pipeline), &state, NULL, 0);
	g_printf("The current state = %d ret = %d\n", state, ret);

	//draw_image(gw->drawingarea1, gw->orig[1]);
	//g_signal_emit_by_name(G_OBJECT(gw->drawingarea1), "expose_event", NULL);

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
			g_print ("GST_MESSAGE_EOS\n");
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
	gw->gop.state = 0;
	//Walet structure allocate memory
	//gw->gop = g_slice_new(GOP);

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
	gw->msrc 		= gst_element_factory_make("multifilesrc", "source");
	gw->dec 		= gst_element_factory_make("decodebin2", "decoder");
	gw->pgmdec		= gst_element_factory_make("pgmdec", "decoder");
	gw->fakesink 	= gst_element_factory_make("fakesink", "sink");
	//appsink		= gst_element_factory_make(("appsink","appsink");

	if(!(gw->src && gw->msrc && gw->dec && gw->fakesink && gw->pgmdec)){
		g_critical("Could not create pipeline elements");
		return FALSE;
	}


	//gst_bin_add_many (GST_BIN (pipeline), src, dec, fakesink, NULL);
	//gst_element_link_many (src, dec, NULL);
	//gst_bin_add_many (GST_BIN (gw->pipeline), gw->src, gw->pgmdec, gw->fakesink, NULL);
	//gst_element_link_many (gw->src, gw->pgmdec, gw->fakesink, NULL);

	g_signal_connect (gw->dec, "new-decoded-pad", G_CALLBACK (cb_newpad), gw);
	//g_signal_connect (dec, "unknown-type",    G_CALLBACK (cb_error),  NULL);
	g_object_set (gw->fakesink, "signal-handoffs", TRUE, NULL);
	g_signal_connect (gw->fakesink, "handoff", G_CALLBACK (cb_handoff), gw);


	// Initialize GTK+ libraries
	gtk_init (&argc, &argv);
	//Init main window
	if (init_main_window(gw) == FALSE) return 1; // error loading UI
	// Show the window
	gtk_widget_show (gw->window);
	// Enter GTK+ main loop
	gtk_main ();
	// Free memory we allocated for TutorialTextEditor struct
	g_slice_free (GOP, &gw->gop);
	g_slice_free (WaletConfig, &gw->wc);

	// Clean up gstreamer
	//gst_element_set_state (gw->pipeline, GST_STATE_NULL);
	//g_main_loop_quit (gw->loop);
	//gst_object_unref (GST_OBJECT (gw->pipeline));
	g_slice_free (GtkWalet, gw);

	return 0;
}
