#ifndef _GUI_H_
#define _GUI_H_

void new_buffer	(Pixbuf *orig, guint width, guint height);
void new_size	(Pixbuf *orig, guint width, guint height);
void feet(GtkWidget* viewport, Pixbuf *orig, Pixbuf *scal);
void draw_image(GtkWidget* drawingarea, Pixbuf *orig);

void print_status(GtkWalet *gw, const gchar *mesage);
void error_message(const gchar *message);
void on_quit_activate(GtkObject *object, GtkWalet *gw);
void on_open_button_clicked(GtkObject *object, GtkWalet *gw);
gboolean on_drawingarea1_expose_event (GtkWidget *widget, GdkEventExpose *event, GtkWalet *gw);
gboolean on_drawingarea2_expose_event (GtkWidget *widget, GdkEventExpose *event, GtkWalet *gw);

void on_feet_button_clicked (GtkObject *object, GtkWalet *gw);
void on_full_button_clicked (GtkObject *object, GtkWalet *gw);

gboolean  init_main_window(GtkWalet *gw);

#endif //_GUI_H_
