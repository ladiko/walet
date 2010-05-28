#include <walet_gtk.h>

//void draw_picture (GtkWidget* drawingarea, Pixbuf *orid, imgtype* img, guint width, guint height)
void scale(GtkWidget* viewport, Pixbuf *orid, Pixbuf *scal)
{
	//Pixbuf *scal;

	gdouble v_width 	= gtk_adjustment_get_page_size (gtk_viewport_get_hadjustment ((GtkViewport*)viewport));
	gdouble v_height 	= gtk_adjustment_get_page_size (gtk_viewport_get_vadjustment ((GtkViewport*)viewport));

	gdouble w = ((gdouble)orid->width)/v_width;
	gdouble h = ((gdouble)orid->height)/v_height;

	if (h > w) {
		scal->width = (int)(((gdouble)orid->width)/h);
		scal->height = (int)v_height;
	}
	else {
		scal->width = (int)v_width;
		scal->height = (int)(((gdouble)orid->width)/w);
	}

	if(scal->init) gdk_pixbuf_unref (scal->pxb);
	scal->pxb = gdk_pixbuf_scale_simple (orid->pxb, scal->width, scal->height, GDK_INTERP_BILINEAR);
	scal->init = 1;

	//return scal;
}

void draw_new_picture(guint n, imgtype* img, guint width, guint height, GtkWalet *gw)
{
	//GtkDrawingArea  *drawingarea;
	//GtkViewport		*viewport;
	GtkWidget  	*drawingarea;
	GtkWidget	*viewport;
	guint init, w, h;
	GdkPixbuf *pxb;

	switch(n){
		case(0) : { drawingarea = gw->drawingarea0; viewport = gw->viewport0; break;}
		case(1) : { drawingarea = gw->drawingarea1; viewport = gw->viewport1; break;}
		case(2) : { drawingarea = gw->drawingarea2; viewport = gw->viewport2; break;}
		case(3) : { drawingarea = gw->drawingarea3; viewport = gw->viewport3; break;}
		default : { g_printf("The numbers of drawing areas from 0 to 3\n"); return; }
	}
	g_printf("pxb_orig = %p pxb_scal = %p \n", gw->orig[n]->pxb, gw->scal[n]->pxb);
	if(gw->orig[n]->width != width || gw->orig[n]->height != height || !gw->orig[n]->init){
		gw->orig[n]->width = width; gw->orig[n]->height = height;
		if(gw->orig[n]->init) gdk_pixbuf_unref(gw->orig[n]->pxb); //gdk_pixbuf_unref(tp->pxb[1]);
		gw->orig[n]->pxb = gdk_pixbuf_new(GDK_COLORSPACE_RGB, 0, 8, width, height);
		gw->orig[n]->init = 1;
		//gtk_widget_set_size_request(drawingarea, width, height);
	}
	utils_grey_to_rgb(img, gdk_pixbuf_get_pixels(gw->orig[n]->pxb), width, height);

	if(gtk_toggle_tool_button_get_active((GtkToggleToolButton*)gw->feet_button)) {
		scale(viewport, gw->orig[n], gw->scal[n]);
		w = gw->scal[n]->width; h = gw->scal[n]->height; pxb = gw->scal[n]->pxb;
	} else {
		w = gw->orig[n]->width; h = gw->orig[n]->height; pxb = gw->orig[n]->pxb;
	}
	gtk_widget_set_size_request(drawingarea, w, h);
	g_printf("gtk_widget_set_size_request pxb_orig = %p pxb_scal = %p \n", gw->orig[n]->pxb, gw->scal[n]->pxb);
	//gdk_draw_pixbuf(gw->drawingarea0->window,
	//		gw->drawingarea0->style->black_gc,
	//		gw->orig[0]->pxb, 0 ,0 ,0 ,0 , -1,-1, GDK_RGB_DITHER_NONE, 0, 0);
	//gdk_draw_pixbuf(drawingarea->window,
	//		///drawingarea->style->fg_gc[GTK_WIDGET_STATE(drawingarea)],
	//		drawingarea->style->black_gc,
	//		pxb, 0 ,0 ,0 ,0 , -1,-1, GDK_RGB_DITHER_NONE, 0, 0);
	//g_printf("w = %d h = %d\n", gdk_pixbuf_get_width(tp->pxb[0]), gdk_pixbuf_get_height(tp->pxb[0]));
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
	if(!gtk_toggle_tool_button_get_active((GtkToggleToolButton*)object)){
	//	gtk_toggle_tool_button_set_active((GtkToggleToolButton*)gw->feet_button, TRUE);
	} else {
	//	gtk_toggle_tool_button_set_active((GtkToggleToolButton*)gw->full_button, FALSE);
	}
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
	//gtk_widget_set_size_request(widget, w, h);
	//g_printf("gtk_widget_set_size_request pxb_orig = %p pxb_scal = %p \n", gw->orig[n]->pxb, gw->scal[n]->pxb);
	//gdk_draw_pixbuf(widget->window,
	//		widget->style->black_gc,
	//		gw->orig[0]->pxb, 0 ,0 ,0 ,0 , -1,-1, GDK_RGB_DITHER_NONE, 0, 0);
	//return TRUE;
	gdk_draw_pixbuf(gw->drawingarea0->window,
			gw->drawingarea0->style->black_gc,
			gw->orig[0]->pxb, 0 ,0 ,0 ,0 , -1,-1, GDK_RGB_DITHER_NONE, 0, 0);
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

	gw->orig[0] = g_slice_new(Pixbuf); gw->orig[0]->init = 0;
	gw->orig[1] = g_slice_new(Pixbuf); gw->orig[1]->init = 0;
	gw->orig[2] = g_slice_new(Pixbuf); gw->orig[2]->init = 0;
	gw->orig[3] = g_slice_new(Pixbuf); gw->orig[3]->init = 0;

	gw->scal[0] = g_slice_new(Pixbuf); gw->scal[0]->init = 0;
	gw->scal[1] = g_slice_new(Pixbuf); gw->scal[1]->init = 0;
	gw->scal[2] = g_slice_new(Pixbuf); gw->scal[2]->init = 0;
	gw->scal[3] = g_slice_new(Pixbuf); gw->scal[3]->init = 0;


	//connect signals, don't work now
	//gtk_builder_connect_signals(builder, wlgtk);

    g_signal_connect(G_OBJECT(gw->window)		, "destroy", 		G_CALLBACK (on_quit_activate), gw);
    g_signal_connect(G_OBJECT(gw->menu_open)	, "activate", 		G_CALLBACK (on_open_button_clicked), gw);
    g_signal_connect(G_OBJECT(gw->menu_quit)	, "activate", 		G_CALLBACK (on_quit_activate), gw);

    g_signal_connect(G_OBJECT(gw->open_button)	, "clicked"	, 		G_CALLBACK (on_open_button_clicked), gw);
    g_signal_connect(G_OBJECT(gw->feet_button)	, "clicked"	, 		G_CALLBACK (on_feet_button_clicked), gw);
    g_signal_connect(G_OBJECT(gw->full_button)	, "clicked"	, 		G_CALLBACK (on_full_button_clicked), gw);

    g_signal_connect(G_OBJECT(gw->drawingarea0)	, "expose_event",	G_CALLBACK (on_drawingarea0_expose_event), gw);
    //g_signal_connect(G_OBJECT(gw->drawingarea1)	, "expose_event",	G_CALLBACK (on_drawingarea1_expose_event), gw);
    //g_signal_connect(G_OBJECT(gw->drawingarea2)	, "expose_event",	G_CALLBACK (on_drawingarea2_expose_event), gw);
    //g_signal_connect(G_OBJECT(gw->drawingarea3)	, "expose_event",	G_CALLBACK (on_drawingarea3_expose_event), gw);

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


