#include <walet_gtk.h>


int main (int argc, char *argv[])
{
	//GTK structure allocate memory
	GtkWalet *gw = g_slice_new(GtkWalet);
	gw->gop.state = 0;
	//Walet structure allocate memory
	//gw->gop = g_slice_new(GOP);

	if (!g_thread_supported ()) g_thread_init (NULL);

	// Initialize the Gsreamer libraries
    //gst_init(&argc, &argv);
	gw->loop = g_main_loop_new (NULL, FALSE);



	//gst_bin_add_many (GST_BIN (pipeline), src, dec, fakesink, NULL);
	//gst_element_link_many (src, dec, NULL);
	//gst_bin_add_many (GST_BIN (gw->pipeline), gw->src, gw->pgmdec, gw->fakesink, NULL);
	//gst_element_link_many (gw->src, gw->pgmdec, gw->fakesink, NULL);


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
