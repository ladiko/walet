#include <walet_gtk.h>

//void draw_picture (GtkWidget* drawingarea, Pixbuf *orid, imgtype* img, guint width, guint height)
void new_buffer(guint n, guint width, guint height, GtkWalet *gw)
{
	guchar *data;
	if(gw->orig[n]->init) gdk_pixbuf_unref(gw->orig[n]->pxb); //gdk_pixbuf_unref(tp->pxb[1]);
	data = (guchar *)calloc(width*height*3, sizeof(guchar));
	gw->orig[n]->pxb = gdk_pixbuf_new_from_data (data, GDK_COLORSPACE_RGB,
					     FALSE, 8, width, height, width*3, NULL, NULL);
	//gw->orig[n]->pxb = gdk_pixbuf_new(GDK_COLORSPACE_RGB, 0, 8, width, height);
	gw->orig[n]->width = width; gw->orig[n]->height = height;
	gw->orig[n]->init = 1;
	//utils_grey_to_rgb(img, gdk_pixbuf_get_pixels(gw->orig[n]->pxb), width, height);
	//g_printf("width = %d height = %d rowstride = %d\n",
	//		gdk_pixbuf_get_width (gw->orig[n]->pxb),gdk_pixbuf_get_height (gw->orig[n]->pxb), gdk_pixbuf_get_rowstride (gw->orig[n]->pxb));
}

void zoom(Pixbuf *orig, Pixbuf *scal, gdouble zoom)
{
	scal->width 	= (guint)(((gdouble)orig->width)*zoom);
	scal->height 	= (guint)(((gdouble)orig->height)*zoom);
	if(scal->init) gdk_pixbuf_unref (scal->pxb);
	if(zoom < 1.) 	scal->pxb = gdk_pixbuf_scale_simple (orig->pxb, scal->width, scal->height, GDK_INTERP_BILINEAR);
	else			scal->pxb = gdk_pixbuf_scale_simple (orig->pxb, scal->width, scal->height, GDK_INTERP_NEAREST);
	scal->init = 1;

}

void feet(GtkWidget* viewport, Pixbuf *orig, Pixbuf *scal)
{
	//Pixbuf *scal;

	gdouble v_width 	= gtk_adjustment_get_page_size (gtk_viewport_get_hadjustment ((GtkViewport*)viewport));
	gdouble v_height 	= gtk_adjustment_get_page_size (gtk_viewport_get_vadjustment ((GtkViewport*)viewport));

	gdouble w = ((gdouble)orig->width)/v_width;
	gdouble h = ((gdouble)orig->height)/v_height;

	if (h > w) {
		scal->width = (int)(((gdouble)orig->width)/h);
		scal->height = (int)v_height;
	}
	else {
		scal->width = (int)v_width;
		scal->height = (int)(((gdouble)orig->width)/w);
	}

	if(scal->init) gdk_pixbuf_unref (scal->pxb);
	scal->pxb = gdk_pixbuf_scale_simple (orig->pxb, scal->width, scal->height, GDK_INTERP_BILINEAR);
	scal->init = 1;
}

void draw_image(GtkWidget* drawingarea, Pixbuf *orig)
{
	gtk_widget_set_size_request(drawingarea, orig->width, orig->height);
	gdk_draw_pixbuf(drawingarea->window,
			drawingarea->style->black_gc,
			orig->pxb, 0 ,0 ,0 ,0 , -1,-1, GDK_RGB_DITHER_NONE, 0, 0);

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
/*
void error_message(const gchar *message)
{
        GtkWidget               *dialog;

        g_warning (message);

        dialog = gtk_message_dialog_new(NULL,
										GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
										GTK_MESSAGE_ERROR,
										GTK_BUTTONS_OK,
										message);

        gtk_window_set_title (GTK_WINDOW (dialog), "Error!");
        gtk_dialog_run (GTK_DIALOG (dialog));
        gtk_widget_destroy (dialog);
}
*/
void on_quit_activate(GtkObject *object, GtkWalet *gw)
{
	//gst_element_set_state (pipeline, GST_STATE_NULL);
	//gst_object_unref (GST_OBJECT (pipeline));
	g_main_loop_quit (gw->loop);
    gtk_main_quit ();
}

void on_open_button_clicked(GtkObject *object, GtkWalet *gw)
{
	GtkWidget	*dialog;
	//GError  	*err=NULL;
	//guint		size;

	dialog = gtk_file_chooser_dialog_new ("Open File...",
											GTK_WINDOW (gw->window),
											GTK_FILE_CHOOSER_ACTION_OPEN,
											GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
											GTK_STOCK_OPEN, GTK_RESPONSE_OK,
											NULL);

    gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), "/home/vadim/raw/photo");
	if (gtk_dialog_run(GTK_DIALOG (dialog)) == GTK_RESPONSE_OK) {
		gw->filename_open = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		print_status(gw, gw->filename_open);
	} else {
		g_warning("Could'n open file\n");
		return;
	}
	gtk_widget_destroy (dialog);

	if(!strcmp(&gw->filename_open[strlen(gw->filename_open)-4],".pgm")){
		//Open with gstreamer plugin
		printf("Open pgm %s file\n", gw->filename_open);
		g_object_set (G_OBJECT(gw->src), "location", gw->filename_open, NULL);
		gst_element_set_state (gw->pipeline, GST_STATE_PLAYING);
		g_main_loop_run (gw->loop);
	} else if (!strcmp(&gw->filename_open[strlen(gw->filename_open)-3],".wl")){
		walet_read_stream(&gw->gop, 1, gw->filename_open);
		printf("Open wl %s file gop = %p\n", gw->filename_open, gw->gop);
	} else {
		printf("Can't open file %s\n", gw->filename_open);
	}
}

void on_save_as_button_clicked(GtkObject *object, GtkWalet *gw)
{
	GtkWidget	*dialog;
	//GError  	*err=NULL;
	//guint		size;
	//g_printf("new = %d\n", gw->new);

	dialog = gtk_file_chooser_dialog_new ("Save File...",
							GTK_WINDOW (gw->window),
							GTK_FILE_CHOOSER_ACTION_SAVE,
							GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
							GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
							NULL);

	gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);
    gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), "/home/vadim/raw/photo");
    if(gw->filename_open){
    	if (gw->new){
    		gw->new = 0;
    		gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (dialog), gw->filename_open);
    	} else gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (dialog), gw->filename_save);
    }

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT){
	    gw->filename_save = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
	    walet_write_stream(gw->gop, 1, gw->filename_save);
	    //save_to_file (filename);
	    //g_free (filename);
	  } else gw->new = 1;

	gtk_widget_destroy (dialog);
}

void on_save_button_clicked(GtkObject *object, GtkWalet *gw)
{
	if(gw->new) on_save_as_button_clicked(object, gw);
	else	   walet_write_stream(gw->gop, 1, gw->filename_save);
}

void on_feet_button_clicked (GtkObject *object, GtkWalet *gw)
{
	gw->feet = 1;
	if(gw->orig[0]->init){
		feet(gw->viewport0, gw->orig[0], gw->scal[0]);
		draw_image(gw->drawingarea0, gw->scal[0]);
	}
	if(gw->orig[1]->init){
		feet(gw->viewport1, gw->orig[1], gw->scal[1]);
		draw_image(gw->drawingarea1, gw->scal[1]);
	}
	if(gw->orig[2]->init){
		feet(gw->viewport2, gw->orig[2], gw->scal[2]);
		draw_image(gw->drawingarea2, gw->scal[2]);
	}
	if(gw->orig[3]->init){
		feet(gw->viewport3, gw->orig[3], gw->scal[3]);
		draw_image(gw->drawingarea3, gw->scal[3]);
	}
}

void on_full_button_clicked (GtkObject *object, GtkWalet *gw)
{
	gw->feet = 0;
	gw->zoom = 1.;
	if(gw->orig[0]->init) draw_image(gw->drawingarea0, gw->orig[0]);
	if(gw->orig[1]->init) draw_image(gw->drawingarea1, gw->orig[1]);
	if(gw->orig[2]->init) draw_image(gw->drawingarea2, gw->orig[2]);
	if(gw->orig[3]->init) draw_image(gw->drawingarea3, gw->orig[3]);
}

void on_zoom_in_button_clicked (GtkObject *object, GtkWalet *gw)
{
	if(gw->feet == 1){
		if(gw->orig[0]->init) draw_image(gw->drawingarea0, gw->orig[0]);
		if(gw->orig[1]->init) draw_image(gw->drawingarea1, gw->orig[1]);
		if(gw->orig[2]->init) draw_image(gw->drawingarea2, gw->orig[2]);
		if(gw->orig[3]->init) draw_image(gw->drawingarea3, gw->orig[3]);
		gw->feet = 0;
		gw->zoom = 1.;
	} else {
		gw->zoom *= 2.;
		if(gw->orig[0]->init){
			if(gw->zoom == 1.) {
				draw_image(gw->drawingarea0, gw->orig[0]);
			} else {
				zoom(gw->orig[0], gw->scal[0], gw->zoom);
				draw_image(gw->drawingarea0, gw->scal[0]);
			}
		}
		if(gw->orig[1]->init){
			if(gw->zoom == 1.) {
				draw_image(gw->drawingarea1, gw->orig[1]);
			} else {
				zoom(gw->orig[1], gw->scal[1], gw->zoom);
				draw_image(gw->drawingarea1, gw->scal[1]);
			}
		}
		if(gw->orig[2]->init){
			if(gw->zoom == 1.) {
				draw_image(gw->drawingarea2, gw->orig[2]);
			} else {
				zoom(gw->orig[2], gw->scal[2], gw->zoom);
				draw_image(gw->drawingarea2, gw->scal[2]);
			}
		}
		if(gw->orig[3]->init){
			if(gw->zoom == 1.) {
				draw_image(gw->drawingarea3, gw->orig[3]);
			} else {
				zoom(gw->orig[3], gw->scal[3], gw->zoom);
				draw_image(gw->drawingarea3, gw->scal[3]);
			}
		}
	}
	//g_printf("zoom = %f feet = %d \n", gw->zoom, gw->feet);
}

void on_zoom_out_button_clicked (GtkObject *object, GtkWalet *gw)
{
	if(gw->feet == 1){
		gw->feet = 0;
		gw->zoom = 0.5;
		if(gw->orig[0]->init) {
			zoom(gw->orig[0], gw->scal[0], gw->zoom);
			draw_image(gw->drawingarea0, gw->scal[0]);
		}
		if(gw->orig[1]->init) {
			zoom(gw->orig[1], gw->scal[1], gw->zoom);
			draw_image(gw->drawingarea1, gw->scal[1]);
		}
		if(gw->orig[2]->init) {
			zoom(gw->orig[2], gw->scal[2], gw->zoom);
			draw_image(gw->drawingarea2, gw->scal[2]);
		}
		if(gw->orig[3]->init) {
			zoom(gw->orig[3], gw->scal[3], gw->zoom);
			draw_image(gw->drawingarea3, gw->scal[3]);
		}
	} else {
		gw->zoom /= 2.;
		if(gw->orig[0]->init){
			if(gw->zoom == 1.) {
				draw_image(gw->drawingarea0, gw->orig[0]);
			} else {
				zoom(gw->orig[0], gw->scal[0], gw->zoom);
				draw_image(gw->drawingarea0, gw->scal[0]);
			}
		}
		if(gw->orig[1]->init){
			if(gw->zoom == 1.) {
				draw_image(gw->drawingarea1, gw->orig[1]);
			} else {
				zoom(gw->orig[1], gw->scal[1], gw->zoom);
				draw_image(gw->drawingarea1, gw->scal[1]);
			}
		}
		if(gw->orig[2]->init){
			if(gw->zoom == 1.) {
				draw_image(gw->drawingarea2, gw->orig[2]);
			} else {
				zoom(gw->orig[2], gw->scal[2], gw->zoom);
				draw_image(gw->drawingarea2, gw->scal[2]);
			}
		}
		if(gw->orig[3]->init){
			if(gw->zoom == 1.) {
				draw_image(gw->drawingarea3, gw->orig[3]);
			} else {
				zoom(gw->orig[3], gw->scal[3], gw->zoom);
				draw_image(gw->drawingarea3, gw->scal[3]);
			}
		}
	}
	//g_printf("zoom = %f feet = %d \n", gw->zoom, gw->feet);
}

void on_dwt_button_clicked(GtkObject *object, GtkWalet *gw)
{
	if(gw->gop == NULL ) return;
	if(frame_dwt_53(gw->gop, gw->gop->cur_gop_frame)) {
		new_buffer (2, gw->gop->width, gw->gop->height, gw);
		utils_subband_draw(&gw->gop->frames[gw->gop->cur_gop_frame].img[0], gdk_pixbuf_get_pixels(gw->orig[2]->pxb), gw->gop->color, gw->gop->steps);
		gtk_widget_queue_draw(gw->drawingarea2);
		//draw_image(gw->drawingarea2, gw->orig[2]);
	}
}

void on_idwt_button_clicked(GtkObject *object, GtkWalet *gw)
{
	if(gw->gop == NULL ) return;
	if(frame_idwt_53(gw->gop, gw->gop->cur_gop_frame, gw->gop->steps)){
		new_buffer (0, gw->gop->width-1, gw->gop->height-1, gw);
		utils_bayer_draw(gw->gop->frames[0].img[0].img, gdk_pixbuf_get_pixels(gw->orig[0]->pxb), gw->gop->width, gw->gop->height, gw->gop->bg);
		gtk_widget_queue_draw(gw->drawingarea0);
		//draw_image(gw->drawingarea0, gw->orig[0]);
	}
}

void on_fill_button_clicked(GtkObject *object, GtkWalet *gw)
{
	if(gw->gop == NULL ) return;
	frame_fill_subb(gw->gop, gw->gop->cur_gop_frame);
}

void on_bits_button_clicked(GtkObject *object, GtkWalet *gw)
{
	if(gw->gop == NULL ) return;
	frame_bits_alloc(gw->gop, gw->gop->cur_gop_frame, gtk_spin_button_get_value_as_int ((GtkSpinButton *)gw->times_spinbutton));
}

void on_quant_button_clicked(GtkObject *object, GtkWalet *gw)
{
	if(gw->gop == NULL ) return;
	if(frame_quantization(gw->gop, gw->gop->cur_gop_frame)){
		new_buffer (3, gw->gop->width, gw->gop->height, gw);
		utils_subband_draw(&gw->gop->frames[gw->gop->cur_gop_frame].img[0], gdk_pixbuf_get_pixels(gw->orig[3]->pxb), gw->gop->color, gw->gop->steps);
		gtk_widget_queue_draw(gw->drawingarea3);
		//draw_image(gw->drawingarea3, gw->orig[3]);
	}
}

void on_range_enc_button_clicked(GtkObject *object, GtkWalet *gw)
{
	uint32 size;
	if(gw->gop == NULL ) return;
	frame_range_encode(gw->gop, gw->gop->cur_gop_frame, &size);
	printf("Encoded frame size = %d\n", size);
}

void on_range_dec_button_clicked(GtkObject *object, GtkWalet *gw)
{
	uint32 size;
	if(gw->gop == NULL ) return;
	frame_range_decode(gw->gop, gw->gop->cur_gop_frame, &size);
	printf("Decoded frame size = %d\n", size);
}

gboolean on_drawingarea0_expose_event (GtkWidget *widget, GdkEventExpose *event, GtkWalet *gw)
{
	//gtk_widget_set_size_request(widget, w, h);
	//g_printf("on_drawingarea0_expose_event \n");
	if(gw->orig[0]->init){
		if(gw->feet) {
			feet(gw->viewport0, gw->orig[0], gw->scal[0]);
			draw_image(gw->drawingarea0, gw->scal[0]);
		} else{
			if(gw->zoom == 1.) 	draw_image(gw->drawingarea0, gw->orig[0]);
			else				draw_image(gw->drawingarea0, gw->scal[0]);

		}
	}
	return TRUE;
}

gboolean on_drawingarea1_expose_event (GtkWidget *widget, GdkEventExpose *event, GtkWalet *gw)
{
	//gtk_widget_set_size_request(widget, w, h);
	//g_printf("on_drawingarea0_expose_event \n");
	if(gw->orig[1]->init){
		if(gw->feet) {
			feet(gw->viewport1, gw->orig[1], gw->scal[1]);
			draw_image(gw->drawingarea1, gw->scal[1]);
		} else{
			if(gw->zoom == 1.) 	draw_image(gw->drawingarea1, gw->orig[1]);
			else				draw_image(gw->drawingarea1, gw->scal[1]);

		}
	}
	return TRUE;
}

gboolean on_drawingarea2_expose_event (GtkWidget *widget, GdkEventExpose *event, GtkWalet *gw)
{
	//gtk_widget_set_size_request(widget, w, h);
	//g_printf("on_drawingarea0_expose_event \n");
	if(gw->orig[2]->init){
		if(gw->feet) {
			feet(gw->viewport2, gw->orig[2], gw->scal[2]);
			draw_image(gw->drawingarea2, gw->scal[2]);
		} else{
			if(gw->zoom == 1.) 	draw_image(gw->drawingarea2, gw->orig[2]);
			else				draw_image(gw->drawingarea2, gw->scal[2]);

		}
	}
	return TRUE;
}

gboolean on_drawingarea3_expose_event (GtkWidget *widget, GdkEventExpose *event, GtkWalet *gw)
{
	//gtk_widget_set_size_request(widget, w, h);
	//g_printf("on_drawingarea0_expose_event \n");
	if(gw->orig[3]->init){
		if(gw->feet) {
			feet(gw->viewport3, gw->orig[3], gw->scal[3]);
			draw_image(gw->drawingarea3, gw->scal[3]);
		} else{
			if(gw->zoom == 1.) 	draw_image(gw->drawingarea3, gw->orig[3]);
			else				draw_image(gw->drawingarea3, gw->scal[3]);

		}
	}
	return TRUE;
}

gboolean  init_gw(GtkWalet *gw)
{
	GtkBuilder              *builder;
	GError                  *err=NULL;
	//guint                   id;
	//PangoFontDescription    *font_desc;

	// use GtkBuilder to build our interface from the XML file
	builder = gtk_builder_new();
	if (gtk_builder_add_from_file(builder, "walet_gtk.xml", &err) == 0){
		//error_message(err->message);
		//g_warning (message);
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
	gw->save_button 	= GTK_WIDGET (gtk_builder_get_object (builder, "save_button"));
	gw->feet_button 	= GTK_WIDGET (gtk_builder_get_object (builder, "feet_button"));
	gw->full_button 	= GTK_WIDGET (gtk_builder_get_object (builder, "full_button"));
	gw->zoom_in_button	= GTK_WIDGET (gtk_builder_get_object (builder, "zoom_in_button"));
	gw->zoom_out_button	= GTK_WIDGET (gtk_builder_get_object (builder, "zoom_out_button"));
	gw->dwt_button		= GTK_WIDGET (gtk_builder_get_object (builder, "dwt_button"));
	gw->idwt_button		= GTK_WIDGET (gtk_builder_get_object (builder, "idwt_button"));
	gw->fill_button		= GTK_WIDGET (gtk_builder_get_object (builder, "fill_button"));
	gw->times_spinbutton= GTK_WIDGET (gtk_builder_get_object (builder, "times_spinbutton"));
	gw->bits_button		= GTK_WIDGET (gtk_builder_get_object (builder, "bits_button"));
	gw->quant_button	= GTK_WIDGET (gtk_builder_get_object (builder, "quant_button"));
	gw->range_enc_button= GTK_WIDGET (gtk_builder_get_object (builder, "range_enc_button"));
	gw->range_dec_button= GTK_WIDGET (gtk_builder_get_object (builder, "range_dec_button"));
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

	gw->feet = 1; gw->zoom = 1.;
	gw->new = 1;


	//connect signals, don't work now
	//gtk_builder_connect_signals(builder, wlgtk);

    g_signal_connect(G_OBJECT(gw->window)			, "destroy", 		G_CALLBACK (on_quit_activate), gw);
    g_signal_connect(G_OBJECT(gw->menu_open)		, "activate", 		G_CALLBACK (on_open_button_clicked), gw);
    g_signal_connect(G_OBJECT(gw->menu_save)		, "activate", 		G_CALLBACK (on_save_button_clicked), gw);
    g_signal_connect(G_OBJECT(gw->menu_save_as)		, "activate", 		G_CALLBACK (on_save_as_button_clicked), gw);
    g_signal_connect(G_OBJECT(gw->menu_quit)		, "activate", 		G_CALLBACK (on_quit_activate), gw);

    g_signal_connect(G_OBJECT(gw->open_button)		, "clicked"	, 		G_CALLBACK (on_open_button_clicked), gw);
    g_signal_connect(G_OBJECT(gw->save_button)		, "clicked"	, 		G_CALLBACK (on_save_button_clicked), gw);
    g_signal_connect(G_OBJECT(gw->feet_button)		, "clicked"	, 		G_CALLBACK (on_feet_button_clicked), gw);
    g_signal_connect(G_OBJECT(gw->full_button)		, "clicked"	, 		G_CALLBACK (on_full_button_clicked), gw);
    g_signal_connect(G_OBJECT(gw->zoom_in_button)	, "clicked"	, 		G_CALLBACK (on_zoom_in_button_clicked), gw);
    g_signal_connect(G_OBJECT(gw->zoom_out_button)	, "clicked"	, 		G_CALLBACK (on_zoom_out_button_clicked), gw);
    g_signal_connect(G_OBJECT(gw->dwt_button)		, "clicked"	, 		G_CALLBACK (on_dwt_button_clicked), gw);
    g_signal_connect(G_OBJECT(gw->idwt_button)		, "clicked"	, 		G_CALLBACK (on_idwt_button_clicked), gw);
    g_signal_connect(G_OBJECT(gw->fill_button)		, "clicked"	, 		G_CALLBACK (on_fill_button_clicked), gw);
    g_signal_connect(G_OBJECT(gw->bits_button)		, "clicked"	, 		G_CALLBACK (on_bits_button_clicked), gw);
    g_signal_connect(G_OBJECT(gw->quant_button)		, "clicked"	, 		G_CALLBACK (on_quant_button_clicked), gw);
    g_signal_connect(G_OBJECT(gw->range_enc_button)	, "clicked"	, 		G_CALLBACK (on_range_enc_button_clicked), gw);
    g_signal_connect(G_OBJECT(gw->range_dec_button)	, "clicked"	, 		G_CALLBACK (on_range_dec_button_clicked), gw);

    g_signal_connect(G_OBJECT(gw->drawingarea0)		, "expose_event",	G_CALLBACK (on_drawingarea0_expose_event), gw);
    g_signal_connect(G_OBJECT(gw->drawingarea1)		, "expose_event",	G_CALLBACK (on_drawingarea1_expose_event), gw);
    g_signal_connect(G_OBJECT(gw->drawingarea2)		, "expose_event",	G_CALLBACK (on_drawingarea2_expose_event), gw);
    g_signal_connect(G_OBJECT(gw->drawingarea3)		, "expose_event",	G_CALLBACK (on_drawingarea3_expose_event), gw);

	// free memory used by GtkBuilder object
	g_object_unref (G_OBJECT (builder));

	// set the default icon to the GTK "edit" icon
	gtk_window_set_default_icon_name (GTK_STOCK_EDIT);

	gw->filename_open = NULL;
	gw->filename_save = NULL;

	gtk_spin_button_set_value((GtkSpinButton *) gw->times_spinbutton, 20.);
	// setup and initialize our statusbar
	gw->statusbar_context_id = gtk_statusbar_get_context_id (GTK_STATUSBAR (gw->statusbar), "Open file");
	print_status(gw, "No file open");

	return TRUE;
}


