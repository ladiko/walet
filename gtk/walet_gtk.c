#include <walet_gtk.h>


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
	wlgtk->menu_new =  GTK_WIDGET (gtk_builder_get_object (builder, "menu_new"));
	wlgtk->menu_open =  GTK_WIDGET (gtk_builder_get_object (builder, "menu_open"));
	wlgtk->menu_save =  GTK_WIDGET (gtk_builder_get_object (builder, "menu_save"));
	wlgtk->menu_save_as =  GTK_WIDGET (gtk_builder_get_object (builder, "menu_save_as"));
	wlgtk->menu_quit =  GTK_WIDGET (gtk_builder_get_object (builder, "menu_quit"));

	//connect signals, don't work now
	//gtk_builder_connect_signals(builder, wlgtk);

    g_signal_connect(wlgtk->window, "destroy", G_CALLBACK (on_quit_activate), wlgtk);
    g_signal_connect(wlgtk->menu_open, "activate", G_CALLBACK (on_open_activate), wlgtk);
    g_signal_connect(wlgtk->menu_quit, "activate", G_CALLBACK (on_quit_activate), wlgtk);

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


int main (int argc, char *argv[])
{
	WaletGTK	*wlgtk;
	StreamData	*sd;
	GOP			*gop;
	// Allocate the memory needed by our WaletGTK struct
	wlgtk 	= g_slice_new(WaletGTK);
	sd 		= g_slice_new(StreamData);
	gop		= g_slice_new(GOP);
	wlgtk->sd 	= sd;
	wlgtk->gop	= gop;
	// Initialize GTK+ libraries
	gtk_init (&argc, &argv);
	if (init_wlgtk(wlgtk) == FALSE) return 1; // error loading UI
	// Show the window
	gtk_widget_show (wlgtk->window);
	// Enter GTK+ main loop
	gtk_main ();
	// Free memory we allocated for TutorialTextEditor struct
	g_slice_free (WaletGTK, wlgtk);
	return 0;
}
