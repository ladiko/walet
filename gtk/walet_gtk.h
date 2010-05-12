#include <gtk/gtk.h>

typedef struct
{
	GtkWidget               *window;
	GtkWidget               *statusbar;
	GtkWidget               *text_view;
	gchar                   *filename;
} WaletGTK;
