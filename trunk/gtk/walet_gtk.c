#include <walet_gtk.h>

//GTK
WaletGTK	*wlgtk;
TwoPixBuff	*twoimg;
//Walet
StreamData	*sd;
GOP			*gop;
//Gstreamet
GMainLoop *loop;
GstBus *bus;
GstElement *pipeline, *src, *dec, *fakesink, *pgmdec;


void  print_status(WaletGTK *wlgtk, const gchar *mesage)
{
	//gchar	*file;
	//gchar	*status;

	//if (wlgtk->filename == NULL)
	//{
	//	file = g_strdup ("NULL");
	//}
	//else file = g_path_get_basename (wlgtk->filename);

	//status = g_strdup_printf ("File: %s", file);
	gtk_statusbar_pop (GTK_STATUSBAR (wlgtk->statusbar), wlgtk->statusbar_context_id);
	gtk_statusbar_push(GTK_STATUSBAR (wlgtk->statusbar), wlgtk->statusbar_context_id, mesage);
	//g_free (status);
	//g_free (file);
}

void error_message(const gchar *message)
{
        GtkWidget               *dialog;

        /* log to terminal window */
        g_warning (message);

        /* create an error message dialog and display modally to the user */
        dialog = gtk_message_dialog_new(NULL,
										GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
										GTK_MESSAGE_ERROR,
										GTK_BUTTONS_OK,
										message);

        gtk_window_set_title (GTK_WINDOW (dialog), "Error!");
        gtk_dialog_run (GTK_DIALOG (dialog));
        gtk_widget_destroy (dialog);
}

void on_quit_activate(GtkObject *object, WaletGTK *wlgtk)
{
	//gst_element_set_state (pipeline, GST_STATE_NULL);
	//gst_object_unref (GST_OBJECT (pipeline));
	g_main_loop_quit (loop);
    gtk_main_quit ();
}


void on_open_activate(GtkObject *object)
{
	GtkWidget	*chooser;
	GError  	*err=NULL;
	guint		i, size;
	//gchar		**line;

	//GstCaps 			*caps = gst_pad_get_caps (gst_element_get_static_pad (pgmdec, "src"));
	//GstStructure 		*str;
	//GstPadLinkReturn	ret;
	g_printf("Opent file\n");

	chooser = gtk_file_chooser_dialog_new ("Open File...",
											GTK_WINDOW (wlgtk->window),
											GTK_FILE_CHOOSER_ACTION_OPEN,
											GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
											GTK_STOCK_OPEN, GTK_RESPONSE_OK,
											NULL);
	g_printf("Opent file\n");

	if (gtk_dialog_run(GTK_DIALOG (chooser)) == GTK_RESPONSE_OK) {
		wlgtk->filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
		print_status(wlgtk, wlgtk->filename);
	} else {
		g_error("Could'n open file\n");
	}
	gtk_widget_destroy (chooser);

	g_object_set (G_OBJECT(src), "location", wlgtk->filename, NULL);

	//gst_element_set_state (pipeline, GST_STATE_PLAYING);
	//caps = gst_pad_get_negotiated_caps (gst_element_get_static_pad (fakesink, "sink"));
	//g_print("Negotiated cap: %s\n", gst_structure_get_name(gst_caps_get_structure(gst_pad_get_negotiated_caps (gst_element_get_static_pad (fakesink, "sink")), 0)));

	//g_printf("width = %d  height = %d bpp = %d\n", wlgtk->width, wlgtk->height, wlgtk->bpp);

	// Play
	gst_element_set_state (pipeline, GST_STATE_PLAYING);
	g_main_loop_run (loop);
	//if (!gst_element_seek 	(pipeline, 1.0,
	//					GST_FORMAT_DEFAULT,
	//					GST_SEEK_FLAG_FLUSH,
	//					GST_SEEK_TYPE_SET, 0,
	//					GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE)) g_print ("Seek failed!\n");

}

gboolean on_expose_event (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	//g_printf("on_expose_event pixbuff = %p\n", twoimg->pixbuf);
	//g_printf("on_expose_event width = %d height = %d\n", sd->width, sd->height);
	if(twoimg->pixbuf){
		//gdk_pixbuf_render_to_drawable(	twoimg->pixbuf[0],
		gdk_draw_pixbuf(	widget->window,
							widget->style->fg_gc[gtk_widget_get_state (widget)],
							twoimg->pixbuf[0],
							0 ,0 ,0 ,0 ,
							-1,-1,
							GDK_RGB_DITHER_NONE,
							0, 0);
	}
	return TRUE;
}


gboolean  init_wlgtk(WaletGTK *wlgtk)
{
	GtkBuilder              *builder;
	GError                  *err=NULL;
	guint                   id;
	PangoFontDescription    *font_desc;

	// use GtkBuilder to build our interface from the XML file
	builder = gtk_builder_new();
	if (gtk_builder_add_from_file(builder, "walet_gtk.xml", &err) == 0){
		error_message(err->message);
		g_error_free(err);
		return FALSE;
	}

	// get the widgets which will be referenced in callbacks
	wlgtk->window 		= GTK_WIDGET (gtk_builder_get_object (builder, "window"));
	wlgtk->statusbar 	= GTK_WIDGET (gtk_builder_get_object (builder, "statusbar"));
	// Menu buttons
	wlgtk->menu_new		= GTK_WIDGET (gtk_builder_get_object (builder, "menu_new"));
	wlgtk->menu_open 	= GTK_WIDGET (gtk_builder_get_object (builder, "menu_open"));
	wlgtk->menu_save	= GTK_WIDGET (gtk_builder_get_object (builder, "menu_save"));
	wlgtk->menu_save_as = GTK_WIDGET (gtk_builder_get_object (builder, "menu_save_as"));
	wlgtk->menu_quit 	= GTK_WIDGET (gtk_builder_get_object (builder, "menu_quit"));
	// Toolbar buttons
	wlgtk->button_open 	= GTK_WIDGET (gtk_builder_get_object (builder, "button_open"));
	// Drawingareas
	wlgtk->drawingarea1	= GTK_WIDGET (gtk_builder_get_object (builder, "drawingarea1"));

	//connect signals, don't work now
	//gtk_builder_connect_signals(builder, wlgtk);

    g_signal_connect(G_OBJECT(wlgtk->window)		, "destroy", 		G_CALLBACK (on_quit_activate), NULL);
    g_signal_connect(G_OBJECT(wlgtk->menu_open)		, "activate", 		G_CALLBACK (on_open_activate), NULL);
    g_signal_connect(G_OBJECT(wlgtk->menu_quit)		, "activate", 		G_CALLBACK (on_quit_activate), NULL);

    g_signal_connect(G_OBJECT(wlgtk->button_open)	, "clicked"	, 		G_CALLBACK (on_open_activate), NULL);
    g_signal_connect(G_OBJECT(wlgtk->drawingarea1)	, "expose_event",	G_CALLBACK (on_expose_event),  NULL);

	// free memory used by GtkBuilder object
	g_object_unref (G_OBJECT (builder));

	// set the default icon to the GTK "edit" icon
	gtk_window_set_default_icon_name (GTK_STOCK_EDIT);

	wlgtk->filename = NULL;
	// setup and initialize our statusbar
	wlgtk->statusbar_context_id = gtk_statusbar_get_context_id (GTK_STATUSBAR (wlgtk->statusbar), "Open file");
	print_status(wlgtk, "No file open");

	return TRUE;
}

static void cb_newpad (GstElement *decodebin, GstPad *pad, gboolean last, gpointer data)
{
	GstElement 		*linkElement 	= (GstElement *) data;
	GstPad 			*sinkpad 		= gst_element_get_static_pad (linkElement, "sink");
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
	ret =  gst_structure_get_int (str, "width", &wlgtk->width);
	ret &= gst_structure_get_int (str, "height", &wlgtk->height);
	ret &= gst_structure_get_int (str, "bpp", &wlgtk->bpp);
	if (!ret) g_critical("Can't get image parameter");

	g_printf("width = %d  height = %d bpp = %d\n", wlgtk->width, wlgtk->height, wlgtk->bpp);

	gst_caps_unref (caps);

	gst_object_unref(sinkpad);
}

static void cb_handoff (GstElement *fakesink, GstBuffer *buffer, GstPad *pad, gpointer user_data)
{
	//GstPad 			*sinkpad 	= gst_element_get_static_pad (fakesink, "sink");
	//GstCaps 		*caps 		= gst_pad_get_caps (pad);
	GstCaps 		*caps 		= gst_buffer_get_caps (buffer);
	//GstStructure	*structure 	= gst_caps_get_structure (caps, 0);
	gboolean ret;
	GstStructure	*str;
	guint8	*pix = GST_BUFFER_DATA(buffer);
	gint i;

	g_printf("cb_handoff buffer = %p  size = %d\n", GST_BUFFER_DATA(buffer), GST_BUFFER_SIZE (buffer));

	//for(i=0; i<12000; i++) g_printf("%2X ", pix[i]);


	str = gst_caps_get_structure (caps, 0);
	g_print("%s\n", gst_caps_to_string (caps));
	ret  = gst_structure_get_int (str, "width"	, &sd->width);
	ret &= gst_structure_get_int (str, "height"	, &sd->height);
	ret &= gst_structure_get_int (str, "bpp"	, &sd->bpp);
	if (!ret) g_critical("Can't get image parameter");

	//Init Walet decoder only at first call on cb_handoff
	sd->color 		= 	BAYER;
	sd->bg 			= 	GRBG;	//Sony Alpha pattern
	sd->steps		= 	5;
	sd->gop_size	=	1;
	walet_decoder_init(sd, gop);
	//Copy frame 0 to decoder pipeline
	frame_copy(gop, 0, GST_BUFFER_DATA(buffer), NULL, NULL);


	twoimg->pixbuf[0] = gdk_pixbuf_new(GDK_COLORSPACE_RGB, 0, 8, sd->width, sd->height);
	utils_grey_to_rgb(gop->frames[0].img[0].img, gdk_pixbuf_get_pixels(twoimg->pixbuf[0]), sd->width, sd->height);
	gtk_widget_set_size_request(wlgtk->drawingarea1, sd->width, sd->height);

	//g_printf("width = %d height = %d\n", sd->width, sd->height);
	//gst_element_set_state (pipeline, GST_STATE_PAUSED);

 }

static gboolean my_bus_callback (GstBus *bus, GstMessage *message, gpointer data)
{
	GMainLoop *loop = (GMainLoop*) data;
	g_print ("Got %s message\n", GST_MESSAGE_TYPE_NAME (message));

	switch (GST_MESSAGE_TYPE (message)) {
		case GST_MESSAGE_ERROR: {
			GError *err;
			gchar *debug;

			gst_message_parse_error (message, &err, &debug);
			g_print ("Error: %s\n", err->message);
			g_error_free (err);
			g_free (debug);

			g_main_loop_quit (loop);
			break;
		}
		case GST_MESSAGE_EOS:
		// end-of-stream
			g_main_loop_quit (loop);
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
	// Allocate the memory needed by our WaletGTK struct
	wlgtk 	= g_slice_new(WaletGTK);
	sd 		= g_slice_new(StreamData);
	gop		= g_slice_new(GOP);
	twoimg	= g_slice_new(TwoPixBuff);
	wlgtk->sd 	= sd;
	wlgtk->gop	= gop;

	if (!g_thread_supported ()) g_thread_init (NULL);

	// Initialize the Gsreamer libraries
	gst_init(&argc, &argv);
	loop = g_main_loop_new (NULL, FALSE);

	// Setup Gsreamer libraries
	pipeline = gst_pipeline_new ("pipeline");

	bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	gst_bus_add_watch (bus, my_bus_callback, loop);
	gst_object_unref  (bus);

	src 		= gst_element_factory_make("filesrc", "source");
	dec 		= gst_element_factory_make("decodebin", "decoder");
	pgmdec		= gst_element_factory_make("pgmdec", "decoder");
	fakesink 	= gst_element_factory_make("fakesink", "sink");
	//appsink		= gst_element_factory_make(("appsink","appsink");

	if(!(src && dec && fakesink && pgmdec)){
		g_critical("Could not create pipeline elements");
		return FALSE;
	}
	//gst_bin_add_many (GST_BIN (pipeline), src, dec, fakesink, NULL);
	//gst_element_link_many (src, dec, NULL);
	gst_bin_add_many (GST_BIN (pipeline), src, pgmdec, fakesink, NULL);
	gst_element_link_many (src, pgmdec, fakesink, NULL);

	g_signal_connect (dec, "new-decoded-pad", G_CALLBACK (cb_newpad), fakesink);
	//g_signal_connect (dec, "unknown-type",    G_CALLBACK (cb_error),  NULL);
	g_object_set (G_OBJECT (fakesink), "signal-handoffs", TRUE, NULL);
	//g_object_set (G_OBJECT (fakesink), "blocksize", 16777216, NULL);
	g_signal_connect (fakesink, "handoff", G_CALLBACK (cb_handoff), NULL);




	// Initialize GTK+ libraries
	gtk_init (&argc, &argv);
	if (init_wlgtk(wlgtk) == FALSE) return 1; // error loading UI
	// Show the window
	gtk_widget_show (wlgtk->window);
	// Enter GTK+ main loop
	gtk_main ();
	// Free memory we allocated for TutorialTextEditor struct
	g_slice_free (WaletGTK, wlgtk);
	g_slice_free (StreamData, sd);
	g_slice_free (GOP, gop);
	g_slice_free (TwoPixBuff, twoimg);

	// Clean up gstreamer
	gst_element_set_state (pipeline, GST_STATE_NULL);
	gst_object_unref (GST_OBJECT (pipeline));

	return 0;
}
