#include <walet_gtk.h>

clock_t start, end;
struct timeval tv;
int nf = -1;


void new_buffer(Pixbuf *orig, guint width, guint height)
{
	guchar *data;
	if(orig->width != width || orig->height != height){
		if(orig->init) g_object_unref(orig->pxb); //gdk_pixbuf_unref(tp->pxb[1]);
		data = (guchar *)g_malloc(width*height*3);
		orig->pxb = gdk_pixbuf_new_from_data (data, GDK_COLORSPACE_RGB,
					     FALSE, 8, width, height, width*3, NULL, NULL);
		//gw->orig[n]->pxb = gdk_pixbuf_new(GDK_COLORSPACE_RGB, 0, 8, width, height);
		orig->width = width; orig->height = height;
		orig->init = 1;
	} else data = (guchar *)g_malloc(width*height*3);
}

void zoom(Pixbuf *orig, Pixbuf *scal, gdouble zoom)
{
	scal->width 	= (guint)(((gdouble)orig->width)*zoom);
	scal->height 	= (guint)(((gdouble)orig->height)*zoom);
	if(scal->init) 	g_object_unref (scal->pxb);
	if(zoom < 1.) 	scal->pxb = gdk_pixbuf_scale_simple (orig->pxb, scal->width, scal->height, GDK_INTERP_BILINEAR);
	else			scal->pxb = gdk_pixbuf_scale_simple (orig->pxb, scal->width, scal->height, GDK_INTERP_NEAREST);
	scal->init = 1;

}

void feet(GtkWidget* viewport, Pixbuf *orig, Pixbuf *scal)
{
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
		scal->height = (int)(((gdouble)orig->height)/w);
	}

	if(scal->init) g_object_unref (scal->pxb);
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

void error_message(const gchar *message)
{
        GtkWidget               *dialog;

        g_warning ("%s",message);

        dialog = gtk_message_dialog_new(NULL,
										GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
										GTK_MESSAGE_ERROR,
										GTK_BUTTONS_OK,
										"%s",message);

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
	GtkWidget	*dialog;
	FILE wl;
	uint32 w, h, bpp;
	imgtype *img;


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
	////Look up tables
	//int i;
	//for(i=0; i<256; i++){
	//	printf("%4d, ", (int)(2048./(1.+(double)i/256.)-1024.));
	//	if((i&0x7) == 7) printf("\n");
	//}

	if(!strcmp(&gw->filename_open[strlen(gw->filename_open)-4],".pgm")){

		//Open with gstreamer plugin
		printf("Open pgm %s file\n", gw->filename_open);

		gst_bin_add_many (GST_BIN (gw->pipeline), gw->src, gw->pgmdec, gw->fakesink, NULL);
		gst_element_link_many (gw->src, gw->pgmdec, gw->fakesink, NULL);

		g_object_set (G_OBJECT(gw->src), "location", gw->filename_open, NULL);
		gst_element_set_state (gw->pipeline, GST_STATE_PLAYING);
		g_main_loop_run (gw->loop);

		gst_element_set_state (gw->pipeline, GST_STATE_NULL);
		g_main_loop_quit (gw->loop);
		gst_object_unref (GST_OBJECT (gw->pipeline));

	} else if (!strcmp(&gw->filename_open[strlen(gw->filename_open)-3],".wl")){
		printf("Open %s file\n", gw->filename_open);

		walet_read_stream(&gw->gop, 1, gw->filename_open);
		frame_decompress(gw->gop, gw->gop->cur_gop_frame, gw->gop->steps, FR_5_3);

		new_buffer (gw->orig[0], gw->gop->width, gw->gop->height);
		utils_grey_draw(gw->gop->frames[gw->gop->cur_gop_frame].img[0].img, gdk_pixbuf_get_pixels(gw->orig[0]->pxb), gw->gop->width, gw->gop->height);
		gtk_widget_queue_draw(gw->drawingarea[0]);

		new_buffer (gw->orig[1], gw->gop->width-1, gw->gop->height-1);
		utils_bayer_draw(gw->gop->frames[gw->gop->cur_gop_frame].img[0].img, gdk_pixbuf_get_pixels(gw->orig[1]->pxb), gw->gop->width, gw->gop->height, gw->gop->bg);
		gtk_widget_queue_draw(gw->drawingarea[1]);

	}else if(!strcmp(&gw->filename_open[strlen(gw->filename_open)-4],".ppm")){
		printf("Open %s file\n", gw->filename_open);
		nf++;

		utils_read_ppm(gw->filename_open, &wl,  &w, &h, &bpp, &img);
		//img = (imgtype *)calloc(w*h*bpp*3, sizeof(imgtype));
		//printf("img = %p size = %d\n", img, w*h*bpp*3);

		if(!gw->walet_init){
			gw->gop = walet_encoder_init(w<<1, h<<1, BAYER, RGGB, 8, 5, 2, 0, 0, FR_5_3, 12);
			gw->walet_init = 1;
		}
		printf("w = %d h = %d bpp = %d\n", w, h, bpp);
		//utils_read_ppm_image(&wl, w, h, bpp, img); //gw->gop->frames[0].img[0].img
		//printf("utils_read_ppm_image ok!\n");

		//new_buffer (gw->orig[0], w, h);
		//utils_grey_draw(gw->gop->frames[gw->gop->cur_gop_frame].img[0].img, gdk_pixbuf_get_pixels(gw->orig[0]->pxb), gw->gop->width, gw->gop->height);
		//utils_draw(img, gdk_pixbuf_get_pixels(gw->orig[0]->pxb), w, h);
		//gtk_widget_queue_draw(gw->drawingarea[0]);

		//printf("nf = %d\n", nf);
		utils_rgb2bayer(img, gw->gop->frames[nf].img[0].img, w, h);
		//frame_copy(gw->gop, nf, uchar *y, uchar *u, uchar *v)

		new_buffer (gw->orig[nf], gw->gop->width-1, gw->gop->height-1);
		utils_bayer_draw(gw->gop->frames[nf].img[0].img, gdk_pixbuf_get_pixels(gw->orig[nf]->pxb), gw->gop->width, gw->gop->height, gw->gop->bg);
		gtk_widget_queue_draw(gw->drawingarea[nf]);

		new_buffer (gw->orig[nf+1], gw->gop->width, gw->gop->height);
		utils_grey_draw(gw->gop->frames[nf].img[0].img, gdk_pixbuf_get_pixels(gw->orig[nf+1]->pxb), gw->gop->width, gw->gop->height);
		//utils_draw(img, gdk_pixbuf_get_pixels(gw->orig[0]->pxb), w, h);
		gtk_widget_queue_draw(gw->drawingarea[nf+1]);
		free(img);

	} else {
		//printf("Can't open file %s\n", gw->filename_open);
		printf("Open %s file\n", gw->filename_open);
		if(gw->gst_init){
			//gst_pad_push_event(gst_element_get_static_pad(gw->src, "src"), gst_event_new_eos());
			//g_printf("gst_pad_push_event\n");
			//g_main_loop_quit (gw->loop);
			//g_printf("g_main_loop_quit\n");
			//gst_bus_set_flushing (gw->bus, TRUE);
			gst_element_set_state (gw->pipeline, GST_STATE_READY);
			g_printf("gst_element_set_state\n");
			gst_element_unlink_many (gw->src, gw->dec, gw->fakesink, NULL);
			g_printf("gst_element_unlink\n");
			//gst_bin_remove(GST_BIN (gw->pipeline), gw->src);
			//gst_bin_remove_many(GST_BIN (gw->pipeline), gw->src, gw->dec, gw->fakesink, NULL);
			g_printf("g_object_set\n");
			//gst_bin_add(GST_BIN (gw->pipeline), gw->src);
			g_object_set (G_OBJECT(gw->src), "location", gw->filename_open, NULL);

		} else {
			gst_bin_add_many (GST_BIN (gw->pipeline), gw->src, gw->dec, gw->fakesink, NULL);
			g_object_set (G_OBJECT(gw->src), "location", gw->filename_open, NULL);
			gst_element_link(gw->src, gw->dec);
			gw->gst_init = 1;
		}
		gst_element_set_state (gw->pipeline, GST_STATE_PLAYING);
		g_main_loop_run (gw->loop);
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
	gint i;
	gw->feet = 1;
	for(i=0; i<4; i++){
		if(gw->orig[i]->init){
			feet(gw->viewport[i], gw->orig[i], gw->scal[i]);
			gtk_widget_queue_draw(gw->drawingarea[i]);
		}
	}
}

void on_full_button_clicked (GtkObject *object, GtkWalet *gw)
{
	gint i;
	gw->feet = 0;
	gw->zoom = 1.;
	for(i=0; i<4; i++) if(gw->orig[i]->init) gtk_widget_queue_draw(gw->drawingarea[i]);
}

void on_zoom_in_button_clicked (GtkObject *object, GtkWalet *gw)
{
	gint i;
	if(gw->feet == 1){
		for(i=0; i<4; i++) if(gw->orig[i]->init) gtk_widget_queue_draw(gw->drawingarea[i]);
		gw->feet = 0;
		gw->zoom = 1.;
	} else {
		gw->zoom *= 2.;
		for(i=0; i<4; i++){
			if(gw->orig[i]->init){
				if(gw->zoom == 1.) gtk_widget_queue_draw(gw->drawingarea[i]);
				else {
					zoom(gw->orig[i], gw->scal[i], gw->zoom);
					gtk_widget_queue_draw(gw->drawingarea[i]);
				}
			}
		}
	}
	//g_printf("zoom = %f feet = %d \n", gw->zoom, gw->feet);
}

void on_zoom_out_button_clicked (GtkObject *object, GtkWalet *gw)
{
	gint i;
	if(gw->feet == 1){
		gw->feet = 0;
		gw->zoom = 0.5;
		for(i=0; i<4; i++){
			if(gw->orig[i]->init) {
				zoom(gw->orig[i], gw->scal[i], gw->zoom);
				gtk_widget_queue_draw(gw->drawingarea[i]);
			}
		}
	} else {
		gw->zoom /= 2.;
		for(i=0; i<4; i++){
			if(gw->orig[i]->init){
				if(gw->zoom == 1.) gtk_widget_queue_draw(gw->drawingarea[i]);
				else {
					zoom(gw->orig[i], gw->scal[i], gw->zoom);
					gtk_widget_queue_draw(gw->drawingarea[i]);
				}
			}
		}
	}
}

void on_dwt_button_clicked(GtkObject *object, GtkWalet *gw)
{
	if(gw->gop == NULL ) return;
	//frame_dwt_53(gw->gop, gw->gop->cur_gop_frame);
	if(frame_dwt(gw->gop, gw->gop->cur_gop_frame, gw->gop->fb)) {
		new_buffer (gw->orig[2], gw->gop->width, gw->gop->height);
		utils_subband_draw(&gw->gop->frames[gw->gop->cur_gop_frame].img[0], gdk_pixbuf_get_pixels(gw->orig[2]->pxb), gw->gop->color, gw->gop->steps);
		gtk_widget_queue_draw(gw->drawingarea[2]);
    	//new_buffer (gw->orig[3], gw->gop->width, gw->gop->height);
    	//utils_grey_draw(gw->gop->frames[gw->gop->cur_gop_frame].img[0].img, gdk_pixbuf_get_pixels(gw->orig[3]->pxb), gw->gop->width, gw->gop->height);
    	//gtk_widget_queue_draw(gw->drawingarea[3]);

	}
}

void on_idwt_button_clicked(GtkObject *object, GtkWalet *gw)
{
	if(gw->gop == NULL ) return;
    if(frame_idwt(gw->gop, gw->gop->cur_gop_frame, gw->gop->steps, gw->gop->fb)){
    	new_buffer (gw->orig[0], gw->gop->frames[0].img[0].idwts.x-1, gw->gop->frames[0].img[0].idwts.y-1);
    	utils_bayer_draw(gw->gop->frames[0].img[0].img, gdk_pixbuf_get_pixels(gw->orig[0]->pxb),
    			gw->gop->frames[0].img[0].idwts.x, gw->gop->frames[0].img[0].idwts.y, gw->gop->bg);
    	//new_buffer (gw->orig[0], gw->gop->width, gw->gop->height);
    	//utils_grey_draw(gw->gop->frames[gw->gop->cur_gop_frame].img[0].img, gdk_pixbuf_get_pixels(gw->orig[0]->pxb), gw->gop->width, gw->gop->height);
    	//new_buffer (gw->orig[0], gw->gop->width, gw->gop->height);
        //utils_subband_draw(&gw->gop->frames[gw->gop->cur_gop_frame].img[0], gdk_pixbuf_get_pixels(gw->orig[0]->pxb), gw->gop->color, 1);
        //new_buffer (gw->orig[0], gw->gop->frames[0].img[0].idwts.x-1, gw->gop->frames[0].img[0].idwts.y-1);
        //utils_bayer_draw(gw->gop->frames[0].img[0].img, gdk_pixbuf_get_pixels(gw->orig[0]->pxb),
           //               gw->gop->frames[0].img[0].idwts.x, gw->gop->frames[0].img[0].idwts.y, gw->gop->bg);
        gtk_widget_queue_draw(gw->drawingarea[0]);
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
	gw->gop->comp = gtk_spin_button_get_value_as_int ((GtkSpinButton *)gw->times_spinbutton);
	frame_bits_alloc(gw->gop, gw->gop->cur_gop_frame, gw->gop->comp);
}

void on_quant_button_clicked(GtkObject *object, GtkWalet *gw)
{
	if(gw->gop == NULL ) return;
	if(frame_quantization(gw->gop, gw->gop->cur_gop_frame)){
		new_buffer (gw->orig[3], gw->gop->width, gw->gop->height);
		utils_subband_draw(&gw->gop->frames[gw->gop->cur_gop_frame].img[0], gdk_pixbuf_get_pixels(gw->orig[3]->pxb), gw->gop->color, gw->gop->steps);
		gtk_widget_queue_draw(gw->drawingarea[3]);
	}
}

void on_range_enc_button_clicked(GtkObject *object, GtkWalet *gw)
{
	uint32 size;
	if(gw->gop == NULL ) return;
	gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
	frame_range_encode(gw->gop, gw->gop->cur_gop_frame, &size);
	gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;

	printf("Frame size  = %d Encoded frame size = %d compess = %f time = %f\n",
			gw->gop->width*gw->gop->height, size, (double)(gw->gop->width*gw->gop->height)/(double)size, (double)(end-start)/1000000.);
}

void on_range_dec_button_clicked(GtkObject *object, GtkWalet *gw)
{
	uint32 size;
	if(gw->gop == NULL ) return;

	gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
	frame_range_decode(gw->gop, gw->gop->cur_gop_frame, &size);
	gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;

	printf("Decoded frame size = %d time = %f\n", size, (double)(end-start)/1000000.);
}

void on_compress_button_clicked(GtkObject *object, GtkWalet *gw)
{
	if(gw->gop == NULL ) return;
	gw->gop->comp = gtk_spin_button_get_value_as_int ((GtkSpinButton *)gw->times_spinbutton);
	frame_compress(gw->gop, gw->gop->cur_gop_frame, gw->gop->comp, FR_5_3);
	//printf("times = %d\n", gtk_spin_button_get_value_as_int ((GtkSpinButton *)gw->times_spinbutton));
	frame_quantization(gw->gop, gw->gop->cur_gop_frame);
	frame_idwt(gw->gop, gw->gop->cur_gop_frame, gw->gop->steps, gw->gop->fb);
	new_buffer (gw->orig[0], gw->gop->frames[0].img[0].idwts.x-1, gw->gop->frames[0].img[0].idwts.y-1);
	utils_bayer_draw(gw->gop->frames[0].img[0].img, gdk_pixbuf_get_pixels(gw->orig[0]->pxb),
			gw->gop->frames[0].img[0].idwts.x, gw->gop->frames[0].img[0].idwts.y, gw->gop->bg);
	gtk_widget_queue_draw(gw->drawingarea[0]);

	printf("DISTR = %f\n",utils_dist(gw->gop->frames[0].img[0].img, gw->gop->frames[1].img[0].img, gw->gop->width*gw->gop->height, 1));
	printf("APE   = %f\n",utils_ape (gw->gop->frames[0].img[0].img, gw->gop->frames[1].img[0].img, gw->gop->width*gw->gop->height, 1));
	printf("PSNR  = %f\n",utils_psnr(gw->gop->frames[0].img[0].img, gw->gop->frames[1].img[0].img, gw->gop->width*gw->gop->height, 1));
}

void on_median_button_clicked(GtkObject *object, GtkWalet *gw)
{
	if(gw->gop == NULL ) return;
	if(frame_median_filter(gw->gop, gw->gop->cur_gop_frame)){

		new_buffer (gw->orig[2], gw->gop->width-1, gw->gop->height-1);
		//utils_bayer_to_Y(gw->gop->frames[0].img[0].img, gw->gop->frames[0].img[0].img, gw->gop->width, gw->gop->height);
		//utils_grey_draw(gw->gop->frames[0].img[0].img, gdk_pixbuf_get_pixels(gw->orig[2]->pxb), gw->gop->width-1, gw->gop->height-1);
		new_buffer (gw->orig[2], gw->gop->width-1, gw->gop->height-1);
		utils_bayer_draw(gw->gop->frames[0].img[0].img, gdk_pixbuf_get_pixels(gw->orig[2]->pxb), gw->gop->width, gw->gop->height, gw->gop->bg);
		gtk_widget_queue_draw(gw->drawingarea[2]);
	}
}

void on_check_button_clicked(GtkObject *object, GtkWalet *gw)
{
	uint32 h, w, x, y, fn = 0, npix;
	imgtype *pic, *pic1;
	clock_t start, end;
	double time=0., tmp;
	struct timeval tv;
	Frame *frm1 = &gw->gop->frames[0];
	Frame *frm2 = &gw->gop->frames[1];

	if(gw->gop == NULL ) return;

	w = gw->gop->frames[0].Y[0].width;
	h = gw->gop->frames[0].Y[0].height;


	frame_match(gw->gop, 0, 1);
	//seg_draw_vec(frm1->pixs, npix, frm1->vec.pic, frm1->grad[0].width, frm1->grad[0].height);


	seg_draw_edges_des(frm1->pixs, frm1->edges, frm1->nedge, frm2->Y[0].pic, frm2->grad[0].width, frm2->grad[0].height, 0, 0);
	new_buffer (gw->orig[2], w, h);
	utils_grey_draw(frm2->Y[0].pic, gdk_pixbuf_get_pixels(gw->orig[2]->pxb), w, h);
	//utils_grey_draw(frm1->grad[0].pic, gdk_pixbuf_get_pixels(gw->orig[3]->pxb), w, h);
	gtk_widget_queue_draw(gw->drawingarea[2]);


	/*
	new_buffer (gw->orig[1], w, h);

	utils_color_draw(frm1->edge.pic, gdk_pixbuf_get_pixels(gw->orig[1]->pxb), w, h, 2);
	utils_color_draw(frm2->edge.pic, gdk_pixbuf_get_pixels(gw->orig[1]->pxb), w, h, 1);
	utils_color_draw(frm1->vec.pic , gdk_pixbuf_get_pixels(gw->orig[1]->pxb), w, h, 0);
	//utils_grey_draw(frm1->pix[0].pic, gdk_pixbuf_get_pixels(gw->orig[1]->pxb), w, h);

	gtk_widget_queue_draw(gw->drawingarea[1]);


	new_buffer (gw->orig[3], w, h);
	utils_grey_draw(frm2->line.pic, gdk_pixbuf_get_pixels(gw->orig[3]->pxb), w, h);
	//utils_grey_draw(frm1->grad[0].pic, gdk_pixbuf_get_pixels(gw->orig[3]->pxb), w, h);
	gtk_widget_queue_draw(gw->drawingarea[3]);
	*/

	//printf("DISTR = %f\n",utils_dist(gw->gop->frames[0].img[0].img, gw->gop->frames[1].img[0].img, gw->gop->width*gw->gop->height, 1));
	//printf("APE   = %f\n",utils_ape (gw->gop->frames[0].img[0].img, gw->gop->frames[1].img[0].img, gw->gop->width*gw->gop->height, 1));
	//printf("PSNR  = %f\n",utils_psnr(gw->gop->frames[0].img[0].img, gw->gop->frames[1].img[0].img, gw->gop->width*gw->gop->height, 1));
}

void on_next_button_clicked(GtkObject *object, GtkWalet *gw)
{
	GstStateChangeReturn ret;
	GstState state;
	uchar *buf;
	uint32 i, j, w, h, npix;
	Frame *frm[2];
	//uint32 i , j, sz = (gw->gop->width)*(gw->gop->height), nregs=0, nrows=0, npregs=0, nobjs=0, nprows=0, ncors, fn, w, h, sn;
	//clock_t start, end;
	//double time=0., tmp;
	//struct timeval tv;
	//Image *im = &gw->gop->frames[fn].img[0];
	//Frame *frm = &gw->gop->frames[fn];
	//imgtype *pic, *pic1;

	w = gw->gop->frames[0].Y[0].width;
	h = gw->gop->frames[0].Y[0].height;
	// Regions segmentation
	printf("nf = %d\n", nf);

	if(nf){
		for(i=0; i<= nf; i++) {
			frm[i] = &gw->gop->frames[i];
			frame_segmetation(gw->gop, i);
			seg_draw_lines(frm[i]->pixs, npix, frm[i]->line.pic, frm[i]->grad[0].width, frm[i]->grad[0].height);
			if(!i){
				seg_draw_edges(frm[i]->pixs, frm[i]->edges, frm[i]->nedge , frm[i]->edge.pic, frm[i]->grad[0].width, frm[i]->grad[0].height, 0, 0);
				seg_draw_edges(frm[i]->pixs, frm[i]->edges, frm[i]->nedge , frm[i]->Y[0].pic, frm[i]->grad[0].width, frm[i]->grad[0].height, 0, 0);
			} else {
				seg_draw_edges(frm[i]->pixs, frm[i]->edges, frm[i]->nedge , frm[i]->edge.pic, frm[i]->grad[0].width, frm[i]->grad[0].height, 0, 0);
				//seg_draw_edges(frm[i]->pixs, frm[i]->edges, frm[i]->nedge , frm[i]->Y[0].pic, frm[i]->grad[0].width, frm[i]->grad[0].height, 0, 0);
			}

			new_buffer (gw->orig[i*2], w, h);
			//utils_grey_draw(gw->gop->frames[i].pix[0].pic, gdk_pixbuf_get_pixels(gw->orig[i*2]->pxb), w, h);
			utils_grey_draw(frm[i]->Y[0].pic, gdk_pixbuf_get_pixels(gw->orig[i*2]->pxb), w, h);
			gtk_widget_queue_draw(gw->drawingarea[i*2]);

			new_buffer (gw->orig[i*2+1], w, h);
			utils_grey_draw(frm[i]->edge.pic, gdk_pixbuf_get_pixels(gw->orig[i*2+1]->pxb), w, h);
			//utils_grey_draw(gw->gop->frames[i].grad[0].pic, gdk_pixbuf_get_pixels(gw->orig[i*2+1]->pxb), w, h);
			gtk_widget_queue_draw(gw->drawingarea[i*2+1]);
		}
	} else {
		frm[0] = &gw->gop->frames[0];
		frame_segmetation(gw->gop, 0);
		//seg_draw_lines(frm[0]->pixs, npix, frm[0]->line.pic, frm[0]->grad[0].width, frm[0]->grad[0].height);
		//seg_draw_edges(frm[0]->pixs, frm[0]->edges, frm[0]->nedge , frm[0]->edge.pic, frm[0]->grad[0].width, frm[0]->grad[0].height, 0, 0);
		//seg_draw_pix(frm[0]->pixs, frm[0]->Y[0].pic, frm[0]->grad[0].pic, frm[0]->grad[0].width, frm[0]->grad[0].height, 0);


		new_buffer (gw->orig[1], w, h);
		utils_grey_draw(frm[0]->Y[0].pic, gdk_pixbuf_get_pixels(gw->orig[1]->pxb), w, h);
		gtk_widget_queue_draw(gw->drawingarea[1]);

		new_buffer (gw->orig[2], w, h);
		utils_grey_draw(frm[0]->grad[0].pic, gdk_pixbuf_get_pixels(gw->orig[2]->pxb), w, h);
		gtk_widget_queue_draw(gw->drawingarea[2]);

		//for(j=0; j < w*h; j++) frm[0]->line.pic[j] = frm[0]->line.pic[j] <= 32 ? 0 : 255;
		new_buffer (gw->orig[3], w, h);
		utils_grey_draw(frm[0]->line.pic, gdk_pixbuf_get_pixels(gw->orig[3]->pxb), w, h);
		gtk_widget_queue_draw(gw->drawingarea[3]);

		for(j=0; j < w*h; j++) frm[0]->edge.pic[j] = frm[0]->grad[0].pic[j] == 255 ? 255 : ( frm[0]->grad[0].pic[j] == 254 ? 100 : 0);
		//for(j=0; j < w*h; j++) frm[0]->edge.pic[j] = frm[0]->grad[0].pic[j] == 255 ? 255 : ( frm[0]->grad[0].pic[j] == 254 ? 255 : 0);
		new_buffer (gw->orig[0], w, h);
		utils_grey_draw(frm[0]->edge.pic, gdk_pixbuf_get_pixels(gw->orig[0]->pxb), w, h);
		gtk_widget_queue_draw(gw->drawingarea[0]);

		//new_buffer (gw->orig[3], w, h);
		//utils_grey_draw(gw->gop->frames[1].grad[0].pic, gdk_pixbuf_get_pixels(gw->orig[3]->pxb), w, h);
		//gtk_widget_queue_draw(gw->drawingarea[3]);

	}
}

gboolean on_drawingarea0123_expose_event (GtkWidget *widget, GdkEventExpose *event, GtkWalet *gw)
{
	//gtk_widget_set_size_request(widget, w, h);
	//g_printf("on_drawingarea0_expose_event \n");
	gint i;
	if(widget == gw->drawingarea[0]) i=0;
	if(widget == gw->drawingarea[1]) i=1;
	if(widget == gw->drawingarea[2]) i=2;
	if(widget == gw->drawingarea[3]) i=3;

	if(gw->orig[i]->init){
		if(gw->feet) {
			feet(gw->viewport[i], gw->orig[i], gw->scal[i]);
			draw_image(gw->drawingarea[i], gw->scal[i]);
		} else{
			if(gw->zoom == 1.) 	draw_image(gw->drawingarea[i], gw->orig[i]);
			else				draw_image(gw->drawingarea[i], gw->scal[i]);

		}
	}
	return TRUE;
}

gboolean on_drawingarea0123_button_press_event (GtkWidget *widget, GdkEventExpose *event, GtkWalet *gw)
{
	if(event->type==GDK_2BUTTON_PRESS){
		gint i;
		if(widget == gw->drawingarea[0]) i=0;
		if(widget == gw->drawingarea[1]) i=1;
		if(widget == gw->drawingarea[2]) i=2;
		if(widget == gw->drawingarea[3]) i=3;

		if(gw->orig[i]->init){
			gw->orig[4]->height = gw->orig[i]->height;
			gw->orig[4]->width  = gw->orig[i]->width;
			gw->orig[4]->init  = 1;
			gw->orig[4]->pxb = gw->orig[i]->pxb;

			//gtk_adjustment_set_page_size (gtk_viewport_get_hadjustment ((GtkViewport*)gw->viewport[4]), (gdouble)(gw->gop->width>>2));
			//gtk_adjustment_set_page_size (gtk_viewport_get_vadjustment ((GtkViewport*)gw->viewport[4]), (gdouble)(gw->gop->height>>2));
			gtk_widget_show (gw->window1);
			gtk_widget_queue_draw(gw->drawingarea[4]);
		}
	}
	return TRUE;
}

gboolean on_quit_windows1(GtkObject *object, GdkEventExpose *event, GtkWalet *gw)
{
	gtk_widget_hide (gw->window1);
	return TRUE;
	//g_object_unref(gw->orig[0]->pxb);
	//g_object_unref(gw->orig[4]->pxb);

}

void on_feet_button1_clicked (GtkObject *object, GtkWalet *gw)
{
	gw->feet1 = 1;
	if(gw->orig[4]->init){
		feet(gw->viewport[4], gw->orig[4], gw->scal[4]);
		gtk_widget_queue_draw(gw->drawingarea[4]);
	}
}

void on_full_button1_clicked (GtkObject *object, GtkWalet *gw)
{
	gw->feet1 = 0;
	//gw->zoom = 1.;
	if(gw->orig[4]->init) gtk_widget_queue_draw(gw->drawingarea[4]);
}

gboolean on_drawingarea4_expose_event (GtkWidget *widget, GdkEventExpose *event, GtkWalet *gw)
{
	//gtk_widget_set_size_request(widget, w, h);
	//g_printf("on_drawingarea0_expose_event \n");
	if(gw->orig[4]->init){
		if(gw->feet1) {
			feet(gw->viewport[4], gw->orig[4], gw->scal[4]);
			draw_image(gw->drawingarea[4], gw->scal[4]);
		} else{
			if(gw->zoom == 1.) 	draw_image(gw->drawingarea[4], gw->orig[4]);
			else				draw_image(gw->drawingarea[4], gw->scal[4]);

		}
	}
	return TRUE;
}

gboolean  init_main_window(GtkWalet *gw)
{
	GtkBuilder              *builder;
	GError                  *err=NULL;
	//guint                   id;
	//PangoFontDescription    *font_desc;

	// use GtkBuilder to build our interface from the XML file
	builder = gtk_builder_new();
	if (gtk_builder_add_from_file(builder, "walet_gtk.xml", &err) == 0){
		error_message(err->message);
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
	gw->open_button 		= GTK_WIDGET (gtk_builder_get_object (builder, "open_button"));
	gw->save_button 		= GTK_WIDGET (gtk_builder_get_object (builder, "save_button"));
	gw->feet_button 		= GTK_WIDGET (gtk_builder_get_object (builder, "feet_button"));
	gw->full_button 		= GTK_WIDGET (gtk_builder_get_object (builder, "full_button"));
	gw->zoom_in_button		= GTK_WIDGET (gtk_builder_get_object (builder, "zoom_in_button"));
	gw->zoom_out_button		= GTK_WIDGET (gtk_builder_get_object (builder, "zoom_out_button"));
	gw->dwt_button			= GTK_WIDGET (gtk_builder_get_object (builder, "dwt_button"));
	gw->idwt_button			= GTK_WIDGET (gtk_builder_get_object (builder, "idwt_button"));
	gw->fill_button			= GTK_WIDGET (gtk_builder_get_object (builder, "fill_button"));
	gw->times_spinbutton	= GTK_WIDGET (gtk_builder_get_object (builder, "times_spinbutton"));
	gw->bits_button			= GTK_WIDGET (gtk_builder_get_object (builder, "bits_button"));
	gw->quant_button		= GTK_WIDGET (gtk_builder_get_object (builder, "quant_button"));
	gw->range_enc_button	= GTK_WIDGET (gtk_builder_get_object (builder, "range_enc_button"));
	gw->range_dec_button	= GTK_WIDGET (gtk_builder_get_object (builder, "range_dec_button"));
	gw->compress_button		= GTK_WIDGET (gtk_builder_get_object (builder, "compress_button"));
	gw->decompress_button	= GTK_WIDGET (gtk_builder_get_object (builder, "compress_button"));
	gw->median_button		= GTK_WIDGET (gtk_builder_get_object (builder, "median_button"));
	gw->check_button		= GTK_WIDGET (gtk_builder_get_object (builder, "check_button"));
	gw->next_button			= GTK_WIDGET (gtk_builder_get_object (builder, "next_button"));
	// Drawingareas
	gw->drawingarea[0]	= GTK_WIDGET (gtk_builder_get_object (builder, "drawingarea0"));
	gw->drawingarea[1]	= GTK_WIDGET (gtk_builder_get_object (builder, "drawingarea1"));
	gw->drawingarea[2]	= GTK_WIDGET (gtk_builder_get_object (builder, "drawingarea2"));
	gw->drawingarea[3]	= GTK_WIDGET (gtk_builder_get_object (builder, "drawingarea3"));
	// Viewports
	gw->viewport[0]	= GTK_WIDGET (gtk_builder_get_object (builder, "viewport0"));
	gw->viewport[1]	= GTK_WIDGET (gtk_builder_get_object (builder, "viewport1"));
	gw->viewport[2]	= GTK_WIDGET (gtk_builder_get_object (builder, "viewport2"));
	gw->viewport[3]	= GTK_WIDGET (gtk_builder_get_object (builder, "viewport3"));

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
    g_signal_connect(G_OBJECT(gw->compress_button)	, "clicked"	, 		G_CALLBACK (on_compress_button_clicked), gw);
    g_signal_connect(G_OBJECT(gw->median_button)	, "clicked"	, 		G_CALLBACK (on_median_button_clicked), gw);
    g_signal_connect(G_OBJECT(gw->check_button)		, "clicked"	, 		G_CALLBACK (on_check_button_clicked), gw);
    g_signal_connect(G_OBJECT(gw->next_button)		, "clicked"	, 		G_CALLBACK (on_next_button_clicked), gw);
    //g_signal_connect(G_OBJECT(gw->compress_button)	, "clicked"	, 		G_CALLBACK (on_compress_button_clicked), gw);
    //g_signal_connect(G_OBJECT(gw->decompress_button), "clicked"	, 		G_CALLBACK (on_decompress_button_clicked), gw);

    g_signal_connect(G_OBJECT(gw->drawingarea[0])		, "expose_event",	G_CALLBACK (on_drawingarea0123_expose_event), gw);
    g_signal_connect(G_OBJECT(gw->drawingarea[1])		, "expose_event",	G_CALLBACK (on_drawingarea0123_expose_event), gw);
    g_signal_connect(G_OBJECT(gw->drawingarea[2])		, "expose_event",	G_CALLBACK (on_drawingarea0123_expose_event), gw);
    g_signal_connect(G_OBJECT(gw->drawingarea[3])		, "expose_event",	G_CALLBACK (on_drawingarea0123_expose_event), gw);
    g_signal_connect(G_OBJECT(gw->drawingarea[0])		, "button_press_event",	G_CALLBACK (on_drawingarea0123_button_press_event), gw);
    g_signal_connect(G_OBJECT(gw->drawingarea[1])		, "button_press_event",	G_CALLBACK (on_drawingarea0123_button_press_event), gw);
    g_signal_connect(G_OBJECT(gw->drawingarea[2])		, "button_press_event",	G_CALLBACK (on_drawingarea0123_button_press_event), gw);
    g_signal_connect(G_OBJECT(gw->drawingarea[3])		, "button_press_event",	G_CALLBACK (on_drawingarea0123_button_press_event), gw);

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

	gw->walet_init = 0; gw->gst_init = 0;

	//Image window
	gw->window1 		= GTK_WIDGET (gtk_builder_get_object (builder, "window1"));
	gw->drawingarea[4]	= GTK_WIDGET (gtk_builder_get_object (builder, "drawingarea4"));
	gw->viewport[4]		= GTK_WIDGET (gtk_builder_get_object (builder, "viewport4"));

	gw->feet_button1 	= GTK_WIDGET (gtk_builder_get_object (builder, "feet_button1"));
	gw->full_button1	= GTK_WIDGET (gtk_builder_get_object (builder, "full_button1"));

    //g_signal_connect(G_OBJECT(gw->window1)			, "destroy", 		G_CALLBACK (on_quit_windows1), gw);
    g_signal_connect(G_OBJECT(gw->window1)			, "delete_event", 	G_CALLBACK (on_quit_windows1), gw);
    g_signal_connect(G_OBJECT(gw->feet_button1)		, "clicked"	, 		G_CALLBACK (on_feet_button1_clicked), gw);
    g_signal_connect(G_OBJECT(gw->full_button1)		, "clicked"	, 		G_CALLBACK (on_full_button1_clicked), gw);
    g_signal_connect(G_OBJECT(gw->drawingarea[4])	, "expose_event",	G_CALLBACK (on_drawingarea4_expose_event), gw);

	gw->orig[4] = g_slice_new(Pixbuf); gw->orig[4]->init = 0;
	gw->scal[4] = g_slice_new(Pixbuf); gw->scal[4]->init = 0;
	gw->feet1 = 1;

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
