
/*
First run tutorial.glade through gtk-builder-convert with this command:
  gtk-builder-convert tutorial.glade tutorial.xml

Then save this file as main.c and compile it using this command
(those are backticks, not single quotes):
  gcc -Wall -g -o tutorial main.c `pkg-config --cflags --libs gtk+-2.0` -export-dynamic

Then execute it using:
  ./tutorial
*/
#include <gtk/gtk.h>

void  on_window1_destroy(GtkObject *object, gpointer user_data)
{
    gtk_main_quit ();
}

int main (int argc, char *argv[])
{
    GtkBuilder      *builder;
    GtkWidget       *window1, *window2, *window3;

    gtk_init (&argc, &argv);

    builder = gtk_builder_new ();
    gtk_builder_add_from_file (builder, "walet.xml", NULL);
    window1 = GTK_WIDGET (gtk_builder_get_object (builder, "window1"));
    window2 = GTK_WIDGET (gtk_builder_get_object (builder, "window2"));
    window3 = GTK_WIDGET (gtk_builder_get_object (builder, "window3"));
    gtk_builder_connect_signals (builder, NULL);

    g_object_unref (G_OBJECT (builder));

    gtk_widget_show (window1);
    gtk_main ();

    return 0;
}
