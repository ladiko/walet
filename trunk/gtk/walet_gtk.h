#include 	<gtk/gtk.h>
#include 	<gst/gst.h>
#include	<walet.h>

typedef struct
{
	GtkWidget	*window;
	GtkWidget	*statusbar;
	//Menu widgets
	GtkWidget	*menu_new;
	GtkWidget	*menu_open;
	GtkWidget	*menu_save;
	GtkWidget	*menu_save_as;
	GtkWidget	*menu_quit;
	// Toolbar widgets
	GtkWidget 	*button_open;

	gchar		*filename;
	guint		statusbar_context_id;
	gchar		*buff; //Buffer for image loading.

	StreamData	*sd;
	GOP			*gop;
} WaletGTK;

// File menu callbacks prototypes
void on_new_activate 		(GtkObject *object, WaletGTK *wlgtk);
void on_open_activate 		(GtkObject *object, WaletGTK *wlgtk);
void on_save_activate 		(GtkObject *object, WaletGTK *wlgtk);
void on_save_as_activate 	(GtkObject *object, WaletGTK *wlgtk);
void on_quit_activate 		(GtkObject *object, WaletGTK *wlgtk);
