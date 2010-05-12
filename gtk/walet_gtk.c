
/*
First run tutorial.glade through gtk-builder-convert with this command:
  gtk-builder-convert tutorial.glade tutorial.xml

Then save this file as main.c and compile it using this command
(those are backticks, not single quotes):
  gcc -Wall -g -o tutorial main.c `pkg-config --cflags --libs gtk+-2.0` -export-dynamic

Then execute it using:
  ./tutorial
*/
#include <walet_gtk.h>

gboolean  init_wlgtk(WaletGTK *editor)
{
        GtkBuilder              *wlgtk;
        GError                  *err=NULL;
        guint                   id;
        PangoFontDescription    *font_desc;

        /* use GtkBuilder to build our interface from the XML file */
        builder = gtk_builder_new ();
        if (gtk_builder_add_from_file (builder, BUILDER_XML_FILE, &err) == 0)
        {
                error_message (err->message);
                g_error_free (err);
                return FALSE;
        }

        /* get the widgets which will be referenced in callbacks */
        editor->window = GTK_WIDGET (gtk_builder_get_object (builder,
                                                             "window"));
        editor->statusbar = GTK_WIDGET (gtk_builder_get_object (builder,
                                                                "statusbar"));
        editor->text_view = GTK_WIDGET (gtk_builder_get_object (builder,
                                                                "text_view"));

        /* connect signals, passing our TutorialTextEditor struct as user data */
        gtk_builder_connect_signals (builder, editor);

        /* free memory used by GtkBuilder object */
        g_object_unref (G_OBJECT (builder));

        /* set the text view font */
        font_desc = pango_font_description_from_string ("monospace 10");
        gtk_widget_modify_font (editor->text_view, font_desc);
        pango_font_description_free (font_desc);

        /* set the default icon to the GTK "edit" icon */
        gtk_window_set_default_icon_name (GTK_STOCK_EDIT);

        /* setup and initialize our statusbar */
        id = gtk_statusbar_get_context_id (GTK_STATUSBAR (editor->statusbar),
                                           "Tutorial GTK+ Text Editor");
        editor->statusbar_context_id = id;
        reset_default_status (editor);

        /* set filename to NULL since we don't have an open/saved file yet */
        editor->filename = NULL;

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
