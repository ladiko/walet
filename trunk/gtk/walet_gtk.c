
#include <walet_gtk.h>

void  reset_default_status (WaletGTK *wlgtk)
{
	gchar           *file;
	gchar           *status;

	if (wlgtk->filename == NULL)
	{
		file = g_strdup ("(UNTITLED)");
	}
	else file = g_path_get_basename (wlgtk->filename);

	status = g_strdup_printf ("File: %s", file);
	gtk_statusbar_pop (GTK_STATUSBAR (wlgtk->statusbar), wlgtk->statusbar_context_id);
	gtk_statusbar_push(GTK_STATUSBAR (wlgtk->statusbar), wlgtk->statusbar_context_id, status);
	g_free (status);
	g_free (file);
}

gboolean  init_wlgtk(WaletGTK *wlgtk)
{
	GtkBuilder              *builder;
	GError                  *err=NULL;
	guint                   id;
	PangoFontDescription    *font_desc;

	/* use GtkBuilder to build our interface from the XML file */
	builder = gtk_builder_new();
	if (gtk_builder_add_from_file(builder, "walet_gtk.xml", &err) == 0){
		error_message(err->message);
		g_error_free(err);
		return FALSE;
	}

	/* get the widgets which will be referenced in callbacks */
	wlgtk->window = GTK_WIDGET (gtk_builder_get_object (builder, "window1"));
	wlgtk->statusbar = GTK_WIDGET (gtk_builder_get_object (builder, "statusbar1"));
	//editor->text_view = GTK_WIDGET (gtk_builder_get_object (builder, "text_view"));

	/* connect signals, passing our TutorialTextEditor struct as user data */
	gtk_builder_connect_signals (builder, wlgtk);

	/* free memory used by GtkBuilder object */
	g_object_unref (G_OBJECT (builder));

	/* set the text view font */
	//font_desc = pango_font_description_from_string ("monospace 10");
	//gtk_widget_modify_font (editor->text_view, font_desc);
	//pango_font_description_free (font_desc);

	/* set the default icon to the GTK "edit" icon */
	gtk_window_set_default_icon_name (GTK_STOCK_EDIT);

	/* setup and initialize our statusbar */
	id = gtk_statusbar_get_context_id (GTK_STATUSBAR (wlgtk->statusbar), "Tutorial GTK+ Text Editor");
	wlgtk->statusbar_context_id = id;
	//reset_default_status (wlgtk);

	/* set filename to NULL since we don't have an open/saved file yet */
	wlgtk->filename = NULL;

	return TRUE;
}

void error_message (const gchar *message)
{
        GtkWidget               *dialog;

        /* log to terminal window */
        g_warning (message);

        /* create an error message dialog and display modally to the user */
        dialog = gtk_message_dialog_new (NULL,
                                         GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_MESSAGE_ERROR,
                                         GTK_BUTTONS_OK,
                                         message);

        gtk_window_set_title (GTK_WINDOW (dialog), "Error!");
        gtk_dialog_run (GTK_DIALOG (dialog));
        gtk_widget_destroy (dialog);
}

gchar * get_open_filename (WaletGTK *wlgtk)
{
	GtkWidget               *chooser;
	gchar                   *filename=NULL;

	chooser = gtk_file_chooser_dialog_new ("Open File...",
											GTK_WINDOW (wlgtk->window),
											GTK_FILE_CHOOSER_ACTION_OPEN,
											GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
											GTK_STOCK_OPEN, GTK_RESPONSE_OK,
											NULL);

	if (gtk_dialog_run (GTK_DIALOG (chooser)) == GTK_RESPONSE_OK){
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
	}
	gtk_widget_destroy (chooser);
	return filename;
}

void on_Open1_activate(GtkMenuItem *menuitem, WaletGTK *wlgtk)
{
	gchar	*filename;

	//if (check_for_save(wlgtk) == TRUE){
	//	on_save_menu_item_activate (NULL, editor);
	//}
	g_warning ("on_Open1_activate\n");
	filename = get_open_filename (wlgtk);
	//if (filename != NULL) load_file (wlgtk, filename);
}

int main (int argc, char *argv[])
{
	WaletGTK      *wlgtk;

	/* allocate the memory needed by our TutorialTextEditor struct */
	wlgtk = g_slice_new (WaletGTK);

	/* initialize GTK+ libraries */
	gtk_init (&argc, &argv);

	if (init_wlgtk(wlgtk) == FALSE) return 1; /* error loading UI */

	/* show the window */
	gtk_widget_show (wlgtk->window);

	/* enter GTK+ main loop */
	gtk_main ();

	/* free memory we allocated for TutorialTextEditor struct */
	g_slice_free (WaletGTK, wlgtk);

	return 0;
}
