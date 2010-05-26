#include <walet_gtk.h>

void draw_picture (GtkWidget* drawingarea, TwoPixbuf *tp, imgtype* img, guint width, guint height)
{
	if(tp->width != width || tp->height != height || !tp->init[0]){
		tp->width = width; tp->height = height;
		if(tp->init[0]) gdk_pixbuf_unref(tp->pxb[0]); //gdk_pixbuf_unref(tp->pxb[1]);
		tp->pxb[0] = gdk_pixbuf_new(GDK_COLORSPACE_RGB, 0, 8, width, height);
		tp->init[0] = 1;
		gtk_widget_set_size_request(drawingarea, width, height);
	}
	utils_grey_to_rgb(img, gdk_pixbuf_get_pixels(tp->pxb[0]), width, height);
	//g_printf("w = %d h = %d\n", gdk_pixbuf_get_width(tp->pxb[0]), gdk_pixbuf_get_height(tp->pxb[0]));
}

void scale(GtkWidget* drawingarea, GtkWidget* viewport, TwoPixbuf *tp)
{
	guint s_width, s_height;
	gdouble v_width 	= gtk_adjustment_get_page_size (gtk_viewport_get_hadjustment ((GtkViewport*)viewport));
	gdouble v_height 	= gtk_adjustment_get_page_size (gtk_viewport_get_vadjustment ((GtkViewport*)viewport));

	gdouble w = ((gdouble)tp->width)/v_width;
	gdouble h = ((gdouble)tp->height)/v_height;

	if (h > w) {
		s_width = (int)(((gdouble)tp->width)/h);
		s_height = (int)v_height;
	}
	else {
		s_width = (int)v_width;
		s_height = (int)(((gdouble)tp->width)/w);
	}

	if(tp->init[1]) gdk_pixbuf_unref (tp->pxb[1]);
	tp->pxb[1] = gdk_pixbuf_scale_simple (tp->pxb[0], s_width, s_height, GDK_INTERP_BILINEAR);
	gtk_widget_set_size_request(drawingarea, s_width, s_height);
	tp->init[1] = 1;
}

void print_status (GtkWalet *gw, const gchar *mesage)
{
	//gchar	*file;
	//gchar	*status;

	//if (wlgtk->filename == NULL)
	//{
	//	file = g_strdup ("NULL");
	//}
	//else file = g_path_get_basename (wlgtk->filename);

	//status = g_strdup_printf ("File: %s", file);
	gtk_statusbar_pop  (GTK_STATUSBAR (gw->statusbar), gw->statusbar_context_id);
	gtk_statusbar_push (GTK_STATUSBAR (gw->statusbar), gw->statusbar_context_id, mesage);
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

void on_quit_activate(GtkObject *object, GtkWalet *gw)
{
	//gst_element_set_state (pipeline, GST_STATE_NULL);
	//gst_object_unref (GST_OBJECT (pipeline));
	g_main_loop_quit (gw->loop);
    gtk_main_quit ();
}

void on_open_button_clicked(GtkObject *object, GtkWalet *gw)
{
	GtkWidget	*chooser;
	GError  	*err=NULL;
	guint		i, size;

	chooser = gtk_file_chooser_dialog_new ("Open File...",
											GTK_WINDOW (gw->window),
											GTK_FILE_CHOOSER_ACTION_OPEN,
											GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
											GTK_STOCK_OPEN, GTK_RESPONSE_OK,
											NULL);

	if (gtk_dialog_run(GTK_DIALOG (chooser)) == GTK_RESPONSE_OK) {
		gw->filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
		print_status(gw, gw->filename);
	} else {
		g_warning("Could'n open file\n");
		return;
	}
	gtk_widget_destroy (chooser);

	g_object_set (G_OBJECT(gw->src), "location", gw->filename, NULL);

	// Play
	gst_element_set_state (gw->pipeline, GST_STATE_PLAYING);
	g_main_loop_run (gw->loop);
}

void on_feet_button_clicked (GtkObject *object, GtkWalet *gw)
{
	g_printf("feet_button = %d\n", gtk_toggle_tool_button_get_active((GtkToggleToolButton*)object));
	//if(!gtk_toggle_tool_button_get_active((GtkToggleToolButton*)object)){
	//	gtk_toggle_tool_button_set_active((GtkToggleToolButton*)gw->feet_button, TRUE);
	//} else {
	//	gtk_toggle_tool_button_set_active((GtkToggleToolButton*)gw->full_button, FALSE);
	//}
}

void on_full_button_clicked (GtkObject *object, GtkWalet *gw)
{
	g_printf("full_button = %d\n", gtk_toggle_tool_button_get_active((GtkToggleToolButton*)object));
	if(!gtk_toggle_tool_button_get_active((GtkToggleToolButton*)object)){
		gtk_toggle_tool_button_set_active((GtkToggleToolButton*)gw->full_button, TRUE);
	} else {
		gtk_toggle_tool_button_set_active((GtkToggleToolButton*)gw->feet_button, FALSE);
	}
}

gboolean on_drawingarea0_expose_event (GtkWidget *widget, GdkEventExpose *event, GtkWalet *gw)
{
	guint init;
	GdkPixbuf *pxb;
	if(gtk_toggle_tool_button_get_active((GtkToggleToolButton*)gw->feet_button)){
		if(gw->tp[0]->init[0]) scale(gw->drawingarea0, gw->viewport0, gw->tp[0]);
		init = gw->tp[0]->init[1]; pxb = gw->tp[0]->pxb[1];
	} else {
		init = gw->tp[0]->init[0]; pxb = gw->tp[0]->pxb[0];
	}

	if(init){
		gdk_draw_pixbuf(	widget->window,
							widget->style->fg_gc[gtk_widget_get_state (widget)],
							pxb,
							0 ,0 ,0 ,0 ,
							-1,-1,
							GDK_RGB_DITHER_NONE,
							0, 0);
	}
	return TRUE;
}

gboolean on_drawingarea1_expose_event (GtkWidget *widget, GdkEventExpose *event, GtkWalet *gw)
{
	if(gw->tp[1]->init[0]){
		gdk_draw_pixbuf(	widget->window,
							widget->style->fg_gc[gtk_widget_get_state (widget)],
							gw->tp[1]->pxb[0],
							0 ,0 ,0 ,0 ,
							-1, -1,
							GDK_RGB_DITHER_NONE,
							0, 0);
	}
	return TRUE;
}

gboolean on_drawingarea2_expose_event (GtkWidget *widget, GdkEventExpose *event, GtkWalet *gw)
{
	if(gw->tp[2]->init[0]){
		gdk_draw_pixbuf(	widget->window,
							widget->style->fg_gc[gtk_widget_get_state (widget)],
							gw->tp[2]->pxb[0],
							0 ,0 ,0 ,0 ,
							-1, -1,
							GDK_RGB_DITHER_NONE,
							0, 0);
	}
	return TRUE;
}

gboolean on_drawingarea3_expose_event (GtkWidget *widget, GdkEventExpose *event, GtkWalet *gw)
{
	if(gw->tp[3]->init[0]){
		gdk_draw_pixbuf(	widget->window,
							widget->style->fg_gc[gtk_widget_get_state (widget)],
							gw->tp[3]->pxb[0],
							0 ,0 ,0 ,0 ,
							-1, -1,
							GDK_RGB_DITHER_NONE,
							0, 0);
	}
	return TRUE;
}

gboolean  init_gw(GtkWalet *gw)
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
	gw->window 			= GTK_WIDGET (gtk_builder_get_object (builder, "window"));
	gw->statusbar 		= GTK_WIDGET (gtk_builder_get_object (builder, "statusbar"));
	// Menu buttons
	gw->menu_new		= GTK_WIDGET (gtk_builder_get_object (builder, "menu_new"));
	gw->menu_open 		= GTK_WIDGET (gtk_builder_get_object (builder, "menu_open"));
	gw->menu_save		= GTK_WIDGET (gtk_builder_get_object (builder, "menu_save"));
	gw->menu_save_as 	= GTK_WIDGET (gtk_builder_get_object (builder, "menu_save_as"));
	gw->menu_quit 		= GTK_WIDGET (gtk_builder_get_object (builder, "menu_quit"));
	// Toolbar buttons
	gw->open_button 	= GTK_WIDGET (gtk_builder_get_object (builder, "open_button"));
	gw->feet_button 	= GTK_WIDGET (gtk_builder_get_object (builder, "feet_button"));
	gw->full_button 	= GTK_WIDGET (gtk_builder_get_object (builder, "full_button"));
	// Drawingareas
	gw->drawingarea0	= GTK_WIDGET (gtk_builder_get_object (builder, "drawingarea0"));
	gw->drawingarea1	= GTK_WIDGET (gtk_builder_get_object (builder, "drawingarea1"));
	gw->drawingarea2	= GTK_WIDGET (gtk_builder_get_object (builder, "drawingarea2"));
	gw->drawingarea3	= GTK_WIDGET (gtk_builder_get_object (builder, "drawingarea3"));
	// Viewports
	gw->viewport0	= GTK_WIDGET (gtk_builder_get_object (builder, "viewport0"));
	gw->viewport1	= GTK_WIDGET (gtk_builder_get_object (builder, "viewport1"));
	gw->viewport2	= GTK_WIDGET (gtk_builder_get_object (builder, "viewport2"));
	gw->viewport3	= GTK_WIDGET (gtk_builder_get_object (builder, "viewport3"));

	gw->tp[0] = g_slice_new(TwoPixbuf); gw->tp[0]->init[0] = 0; gw->tp[0]->init[1] = 0;
	gw->tp[1] = g_slice_new(TwoPixbuf); gw->tp[1]->init[0] = 0; gw->tp[1]->init[1] = 0;
	gw->tp[2] = g_slice_new(TwoPixbuf); gw->tp[2]->init[0] = 0; gw->tp[2]->init[1] = 0;
	gw->tp[3] = g_slice_new(TwoPixbuf); gw->tp[3]->init[0] = 0; gw->tp[3]->init[1] = 0;

	//connect signals, don't work now
	//gtk_builder_connect_signals(builder, wlgtk);

    g_signal_connect(G_OBJECT(gw->window)		, "destroy", 		G_CALLBACK (on_quit_activate), gw);
    g_signal_connect(G_OBJECT(gw->menu_open)	, "activate", 		G_CALLBACK (on_open_button_clicked), gw);
    g_signal_connect(G_OBJECT(gw->menu_quit)	, "activate", 		G_CALLBACK (on_quit_activate), gw);

    g_signal_connect(G_OBJECT(gw->open_button)	, "clicked"	, 		G_CALLBACK (on_open_button_clicked), gw);
    g_signal_connect(G_OBJECT(gw->feet_button)	, "clicked"	, 		G_CALLBACK (on_feet_button_clicked), gw);
    g_signal_connect(G_OBJECT(gw->full_button)	, "clicked"	, 		G_CALLBACK (on_full_button_clicked), gw);

    g_signal_connect(G_OBJECT(gw->drawingarea0)	, "expose_event",	G_CALLBACK (on_drawingarea0_expose_event), gw);
    g_signal_connect(G_OBJECT(gw->drawingarea1)	, "expose_event",	G_CALLBACK (on_drawingarea1_expose_event), gw);
    g_signal_connect(G_OBJECT(gw->drawingarea2)	, "expose_event",	G_CALLBACK (on_drawingarea2_expose_event), gw);
    g_signal_connect(G_OBJECT(gw->drawingarea3)	, "expose_event",	G_CALLBACK (on_drawingarea3_expose_event), gw);

	// free memory used by GtkBuilder object
	g_object_unref (G_OBJECT (builder));

	// set the default icon to the GTK "edit" icon
	gtk_window_set_default_icon_name (GTK_STOCK_EDIT);

	gw->filename = NULL;
	// setup and initialize our statusbar
	gw->statusbar_context_id = gtk_statusbar_get_context_id (GTK_STATUSBAR (gw->statusbar), "Open file");
	print_status(gw, "No file open");

	return TRUE;
}


