#include <walet_gtk.h>

//GTK
WaletGTK	*wlgtk;
//Walet
StreamData	*sd;
GOP			*gop;
//Gstreamet
GMainLoop *loop;
GstBus *bus;
GstElement *pipeline, *src, *dec, *fakesink;


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


void on_open_activate(GtkObject *object, WaletGTK *wlgtk)
{
	GtkWidget	*chooser;
	GError  	*err=NULL;
	guint		i, size;
	gchar		**line;

	chooser = gtk_file_chooser_dialog_new ("Open File...",
											GTK_WINDOW (wlgtk->window),
											GTK_FILE_CHOOSER_ACTION_OPEN,
											GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
											GTK_STOCK_OPEN, GTK_RESPONSE_OK,
											NULL);

	if (gtk_dialog_run(GTK_DIALOG (chooser)) == GTK_RESPONSE_OK) {
		wlgtk->filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
		print_status(wlgtk, wlgtk->filename);
	} else {
		g_error("Could'n open file\n");
	}
	gtk_widget_destroy (chooser);

	g_object_set (G_OBJECT(src), "location", wlgtk->filename, NULL);
	// Play
	gst_element_set_state (pipeline, GST_STATE_PLAYING);
	g_main_loop_run (loop);


	/*
    // Get the file contents
    if (g_file_get_contents (wlgtk->filename, &wlgtk->buff, &size, &err) == FALSE) {
    	error_message (err->message);
    	g_error_free (err);
    	//g_free (wlgtk->filename);
    }
    //g_message("File size = %d\n", size);
    line[0] = g_strdup(wlgtk->buff);
    size = g_strv_length(line);
    g_message("srt = %s  size = %d\n",line[0], size);
    size = sscanf(wlgtk->buff, "%s%s%s%s", line[0], line[1], line[2], line[3]);
    if (strcmp(line[0], "P5") != 0) {
		  g_warning("It's not PGM file");
		  g_free(wlgtk->buff);
		  //return FALSE;
    }
    wlgtk->sd->width  	= atoi(line[1]);
    wlgtk->sd->height 	= atoi(line[2]);
    wlgtk->sd->bits 	= atoi(line[3]);
    size = 0;
    for(i=0; i<4; i++) size += strlen(line[i]);

    print_status(wlgtk, g_strdup_printf ("width = %d  height = %d bpp = %d offset = %d", wlgtk->sd->width, wlgtk->sd->height, wlgtk->sd->bits, size));
	*/
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
	wlgtk->window = GTK_WIDGET (gtk_builder_get_object (builder, "window"));
	wlgtk->statusbar = GTK_WIDGET (gtk_builder_get_object (builder, "statusbar"));
	// Menu buttons
	wlgtk->menu_new		=  GTK_WIDGET (gtk_builder_get_object (builder, "menu_new"));
	wlgtk->menu_open 	=  GTK_WIDGET (gtk_builder_get_object (builder, "menu_open"));
	wlgtk->menu_save	=  GTK_WIDGET (gtk_builder_get_object (builder, "menu_save"));
	wlgtk->menu_save_as =  GTK_WIDGET (gtk_builder_get_object (builder, "menu_save_as"));
	wlgtk->menu_quit 	=  GTK_WIDGET (gtk_builder_get_object (builder, "menu_quit"));
	// Toolbar buttons
	wlgtk->button_open =  GTK_WIDGET (gtk_builder_get_object (builder, "button_open"));

	//connect signals, don't work now
	//gtk_builder_connect_signals(builder, wlgtk);

    g_signal_connect(wlgtk->window, "destroy", G_CALLBACK (on_quit_activate), wlgtk);
    g_signal_connect(wlgtk->menu_open, "activate", G_CALLBACK (on_open_activate), wlgtk);
    g_signal_connect(wlgtk->menu_quit, "activate", G_CALLBACK (on_quit_activate), wlgtk);

    g_signal_connect(wlgtk->button_open, "clicked", G_CALLBACK (on_open_activate), wlgtk);

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
	gst_caps_unref (caps);

	if (GST_PAD_IS_LINKED(sinkpad))
	{
		g_object_unref(sinkpad);
	    return;
	}
	  // Link pads (decoder --> <link Element>)
	gst_pad_link (pad, sinkpad);

	gst_object_unref(sinkpad);
}

static void cb_handoff (GstElement *fakesink, GstBuffer *buffer, GstPad *pad, gpointer user_data)
{
	/* this makes the image black/white */
	//memset (GST_BUFFER_DATA (buffer), 0x0 , GST_BUFFER_SIZE (buffer));
	g_printf("cb_handoff buffer = %p  size = %d\n", buffer, GST_BUFFER_SIZE (buffer));
 }

static gboolean my_bus_callback (GstBus *bus, GstMessage *message, gpointer data)
{
	GMainLoop *loop = (GMainLoop*) data;
	//g_print ("Got %s message\n", GST_MESSAGE_TYPE_NAME (message));

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

	src = gst_element_factory_make ( "filesrc", "source");
	//g_object_set (G_OBJECT (src), "location", "rbc.ru", NULL);

	dec = gst_element_factory_make ("decodebin2", "decoder");
	fakesink = gst_element_factory_make ("fakesink", "sink");
	gst_bin_add_many (GST_BIN (pipeline), src, dec, NULL);
	gst_element_link_many (src, dec, NULL);


	//g_signal_connect (dec, "unknown-type",    G_CALLBACK (cb_error),  NULL);

	g_signal_connect (dec, "new-decoded-pad", G_CALLBACK (cb_newpad), fakesink);
	//g_signal_connect (dec, "unknown-type",    G_CALLBACK (cb_error),  NULL);
	g_object_set (G_OBJECT (fakesink), "signal-handoffs", TRUE, NULL);
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

	// Clean up gstreamer
	gst_element_set_state (pipeline, GST_STATE_NULL);
	gst_object_unref (GST_OBJECT (pipeline));

	return 0;
}
