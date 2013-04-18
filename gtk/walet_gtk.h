#include 	<gtk/gtk.h>
//#include 	<gst/gst.h>
#include 	<gdk-pixbuf/gdk-pixbuf.h>
#include 	<gdk/gdk.h>

#include	<walet.h>

typedef struct  {
	guint 		width;			// Image width
	guint 		height;			// Image width
	//GtkWidget	*da;		//Drawingarea
	gboolean	init;
	GdkPixbuf 	*pxb;	//Two pix buffers one real image second scale image if need.
	//GtkWidget	*da;
	//GtkWidget	*vp;
}Pixbuf;

typedef struct  {
	//Main window
	//GTK widgets
	GtkWidget	*window;
	GtkWidget	*statusbar;
	//Menu widgets
	GtkWidget	*menu_new;
	GtkWidget	*menu_open;
	GtkWidget	*menu_save;
	GtkWidget	*menu_save_as;
	GtkWidget	*menu_quit;
	//Toolbar widgets
	GtkWidget 	*open_button;
	GtkWidget	*save_button;
	GtkWidget	*feet_button;
	GtkWidget	*full_button;
	GtkWidget	*zoom_in_button;
	GtkWidget	*zoom_out_button;
	GtkWidget	*dwt_button;
	GtkWidget	*idwt_button;
	GtkWidget	*fill_button;
	GtkWidget	*times_spinbutton;
	GtkWidget	*bits_button;
	GtkWidget	*quant_button;
	GtkWidget	*range_enc_button;
	GtkWidget	*range_dec_button;
	GtkWidget	*compress_button;
	GtkWidget	*decompress_button;
	GtkWidget	*median_button;
	GtkWidget	*check_button;
	GtkWidget	*next_button;

	// Drawingareas
	Pixbuf		*orig[5];		//Original image
	Pixbuf		*scal[5];		//Scaled image
	GtkWidget	*drawingarea[5];
	GtkWidget	*viewport[5];

	gchar		*filename_open;
	gchar		*filename_save;
	guint		statusbar_context_id;
	gboolean	new; 	// If file was not saved
	gboolean 	feet;
	gdouble		zoom;

	gboolean 	walet_init;
	gboolean 	gst_init;


	//Image window
	//GdkDrawable *drawable;
	GdkPixmap 	*pixmap;
	GtkWidget	*window1;
	GtkWidget	*feet_button1;
	GtkWidget	*full_button1;
	GtkWidget	*fill_hist;
	gboolean 	feet1;
	gboolean 	hist;

	//Walet structure
	GOP				gop;
	WaletConfig 	wc;
	//Gstreamer structures
	GMainLoop *loop;
    //GstBus *bus;
    //GstElement *pipeline, *msrc, *src, *dec, *fakesink, *pgmdec;

} GtkWalet;

#include <gui.h>
// File menu callbacks prototypes
//void on_new_activate 		(GtkObject *object, WaletGTK *wlgtk);
//void on_open_activate 		(GtkObject *object, WaletGTK *wlgtk);
//void on_save_activate 		(GtkObject *object, WaletGTK *wlgtk);
//void on_save_as_activate 	(GtkObject *object, WaletGTK *wlgtk);
//void on_quit_activate 		(GtkObject *object, WaletGTK *wlgtk);
