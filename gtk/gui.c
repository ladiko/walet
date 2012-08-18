#include <walet_gtk.h>

clock_t start, end;
struct timeval tv;
int nf = -1;
int hist = 0;
#define lb1(x) (((x) < 0) ? 0 : (((x) > 255) ? 255 : (x)))


void new_buffer(Pixbuf *orig, guint width, guint height)
{
	guchar *data;
	if(!orig->init) {
		data = (guchar *)g_malloc(width*height*3);
		orig->pxb = gdk_pixbuf_new_from_data (data, GDK_COLORSPACE_RGB,
					     FALSE, 8, width, height, width*3, NULL, NULL);
		//gw->orig[n]->pxb = gdk_pixbuf_new(GDK_COLORSPACE_RGB, 0, 8, width, height);
		orig->width = width; orig->height = height;
		orig->init = 1;
		return;
	}
	else { //(orig->width < width || orig->height < height){
		g_object_unref(orig->pxb); //gdk_pixbuf_unref(tp->pxb[1]);
		data = (guchar *)g_malloc(width*height*3);
		orig->pxb = gdk_pixbuf_new_from_data (data, GDK_COLORSPACE_RGB,
					     FALSE, 8, width, height, width*3, NULL, NULL);
		//gw->orig[n]->pxb = gdk_pixbuf_new(GDK_COLORSPACE_RGB, 0, 8, width, height);
		orig->width = width; orig->height = height;

	} //else data = (guchar *)g_malloc(width*height*3);*/

}

void new_buffer_size(Pixbuf *orig, guint width, guint height)
{
	orig->width = width; orig->height = height;
	orig->pxb = gdk_pixbuf_new_from_data (gdk_pixbuf_get_pixels(orig->pxb), GDK_COLORSPACE_RGB,
				     FALSE, 8, width, height, width*3, NULL, NULL);

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
	uint32 w, h, bpp, size;
	//uint8 *img;


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

		walet_read_stream(&gw->gop, &gw->wc, 1, gw->filename_open);
		frame_decompress(&gw->gop, gw->gop.cur_gop_frame, &gw->wc, gw->wc.steps);

		new_buffer (gw->orig[0], gw->gop.frames[0].b.w, gw->gop.frames[0].b.h);
		utils_grey_draw(gw->gop.frames[0].d.pic, gdk_pixbuf_get_pixels(gw->orig[0]->pxb), gw->gop.frames[0].d.w, gw->gop.frames[0].d.h, 0);
		gtk_widget_queue_draw(gw->drawingarea[0]);

		new_buffer (gw->orig[1], gw->gop.frames[0].b.w, gw->gop.frames[0].b.h);
		utils_bayer_draw(gw->gop.frames[0].d.pic, gdk_pixbuf_get_pixels(gw->orig[1]->pxb), gw->gop.frames[0].d.w, gw->gop.frames[0].d.h, gw->wc.bg);
		gtk_widget_queue_draw(gw->drawingarea[1]);

	}/*else if(!strcmp(&gw->filename_open[strlen(gw->filename_open)-4],".ppm")){
		printf("Open %s file\n", gw->filename_open);
		nf++;

		utils_read_ppm(gw->filename_open, &w, &h, &bpp, gw->gop.buf);

		if(!gw->walet_init){
			gw->wc.w 			= w;	/// Image width
			gw->wc.h			= h;	/// Image width
			gw->wc.icol			= RGB;	/// Color space
			gw->wc.ccol			= CS420;	/// Color space
			gw->wc.bg			= RGGB;		/// Bayer grid pattern
			gw->wc.bpp			= bpp;		/// Image bits per pixel.
			gw->wc.steps		= 4;  		/// DWT steps.
			gw->wc.gop_size		= 1;		/// GOP size
			gw->wc.rates		= 0;		/// Frame rates
			gw->wc.comp			= 20;		/// Compression in times to original image if 1 - lossless 0 - without any compression.
			gw->wc.fb			= FR_5_3;	/// Filters bank for wavelet transform.
			gw->wc.rt			= FAST;		/// Range coder type
			gw->wc.mv			= 12;		/// The motion vector search in pixeles.
			walet_encoder_init(&gw->gop, &gw->wc);
			gw->walet_init = 1;

		}
		printf("w = %d h = %d bpp = %d\n", w, h, bpp);
		utils_rgb2bayer(gw->gop.buf, gw->gop.frames[nf].b.pic, w, h);

		gw->gop.frames[nf].state = FRAME_COPY;
		//frame_copy(gw->gop, nf, uchar *y, uchar *u, uchar *v)

		new_buffer (gw->orig[nf], gw->wc.w-1, gw->wc.h-1);
		utils_bayer_draw(gw->gop.frames[nf].b.pic, gdk_pixbuf_get_pixels(gw->orig[nf]->pxb), gw->wc.w, gw->wc.h, gw->wc.bg);
		gtk_widget_queue_draw(gw->drawingarea[nf]);

		new_buffer (gw->orig[nf+1], gw->wc.w, gw->wc.h);
		utils_grey_draw(gw->gop.frames[nf].b.pic, gdk_pixbuf_get_pixels(gw->orig[nf+1]->pxb), gw->wc.w, gw->wc.h, 0);
		//utils_draw(img, gdk_pixbuf_get_pixels(gw->orig[0]->pxb), w, h);
		gtk_widget_queue_draw(gw->drawingarea[nf+1]);
		//free(img);

	} */else {
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
	    walet_write_stream(&gw->gop, &gw->wc, 1, gw->filename_save);
	    //save_to_file (filename);
	    //g_free (filename);
	  } else gw->new = 1;

	gtk_widget_destroy (dialog);
}

void on_save_button_clicked(GtkObject *object, GtkWalet *gw)
{
	if(gw->new) on_save_as_button_clicked(object, gw);
	else	   walet_write_stream(&gw->gop, &gw->wc, 1, gw->filename_save);
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
	Frame *f0 = &gw->gop.frames[gw->gop.cur_gop_frame];
	int i;
	if(&gw->gop == NULL ) return;
	gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
	if(frame_transform(&gw->gop, gw->gop.cur_gop_frame, &gw->wc)) {
		gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
		printf("DWT time = %f\n",(double)(end-start)/1000000.);
		if(gw->wc.dec == WAVELET){
			for(i=0; i < 2; i++){
				new_buffer (gw->orig[i+1], f0->img[i].w, f0->img[i].h);
				utils_dwt_image_draw(&f0->img[i], gdk_pixbuf_get_pixels(gw->orig[i+1]->pxb), gw->wc.steps);
				gtk_widget_queue_draw(gw->drawingarea[i+1]);
			}

			image_suband_to_block(&f0->img[0], f0->img[0].d.pic, gw->wc.steps);

			new_buffer (gw->orig[3], f0->img[0].w, f0->img[0].h);
			utils_grey_draw(f0->img[0].d.pic, gdk_pixbuf_get_pixels(gw->orig[3]->pxb), f0->img[0].w, f0->img[0].h, 128);
			gtk_widget_queue_draw(gw->drawingarea[3]);
		} else if(gw->wc.dec == RESIZE){
			for(i=0; i < 3; i++){
				new_buffer (gw->orig[i+1], f0->img[i].w, f0->img[i].h);
				utils_resize_draw(f0->dw, gdk_pixbuf_get_pixels(gw->orig[i+1]->pxb), gw->wc.steps, f0->img[i].w);
				gtk_widget_queue_draw(gw->drawingarea[i+1]);
			}
		} else if(gw->wc.dec == VECTORIZE){
			if(gw->wc.ccol == RGBY){
				new_buffer (gw->orig[3], f0->dw[1].w<<1, f0->dw[1].h<<1);
				utils_contour(f0->dw, gdk_pixbuf_get_pixels(gw->orig[3]->pxb), 1);
				gtk_widget_queue_draw(gw->drawingarea[3]);


				new_buffer (gw->orig[0], f0->b.w, f0->b.h);
				utils_resize_draw_rgb(f0->R, f0->G, f0->B, gdk_pixbuf_get_pixels(gw->orig[0]->pxb), gw->wc.steps, f0->b.w);
				gtk_widget_queue_draw(gw->drawingarea[0]);
			}
			else if(gw->wc.ccol == CS420){
				new_buffer (gw->orig[3], f0->y[0].w<<1, f0->y[0].h<<1);
				utils_resize_draw(f0->y, gdk_pixbuf_get_pixels(gw->orig[3]->pxb), gw->wc.steps, f0->y[0].w<<1);
				gtk_widget_queue_draw(gw->drawingarea[3]);

				new_buffer (gw->orig[1], f0->u[0].w<<1, f0->u[0].h<<1);
				utils_resize_draw(f0->u, gdk_pixbuf_get_pixels(gw->orig[1]->pxb), gw->wc.steps, f0->u[0].w<<1);
				gtk_widget_queue_draw(gw->drawingarea[1]);

				new_buffer (gw->orig[2], f0->v[0].w<<1, f0->v[0].h<<1);
				utils_resize_draw(f0->v, gdk_pixbuf_get_pixels(gw->orig[2]->pxb), gw->wc.steps, f0->v[0].w<<1);
				gtk_widget_queue_draw(gw->drawingarea[2]);

			}
		}

		//new_buffer (gw->orig[1], f0->img[0].w, f0->img[0].h);
		//utils_dwt_image_draw(&f0->img[0], gdk_pixbuf_get_pixels(gw->orig[1]->pxb), gw->wc.steps);
		//gtk_widget_queue_draw(gw->drawingarea[1]);

	}
}

void on_idwt_button_clicked(GtkObject *object, GtkWalet *gw)
{
	Frame *f0 = &gw->gop.frames[0];
	uint32 i, w, h, isteps = gw->wc.steps;
	if(&gw->gop == NULL ) return;
	gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
    if(frame_idwt(&gw->gop, gw->gop.cur_gop_frame, &gw->wc, isteps )){
		gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
		printf("IDWT time = %f\n",(double)(end-start)/1000000.);

		new_buffer (gw->orig[1], gw->wc.w, gw->wc.h);
		frame_ouput	(&gw->gop, 0, &gw->wc, gdk_pixbuf_get_pixels(gw->orig[1]->pxb), isteps, &w, &h);
		new_buffer_size (gw->orig[1], w, h);
		gtk_widget_queue_draw(gw->drawingarea[1]);

		if(gw->wc.ccol == RGB || gw->wc.ccol == CS444 || gw->wc.ccol == CS420){
			printf("APE = %f\n",utils_ape_16(f0->img[0].p, f0->img[0].d.pic, f0->img[0].w*f0->img[0].h, 1));
			printf("PNSR = %f\n",utils_psnr_16(f0->img[0].p, f0->img[0].d.pic, f0->img[0].w*f0->img[0].h, 1));
			printf("SSIM = %f\n",utils_ssim_16(f0->img[0].p, f0->img[0].d.pic, f0->img[0].w, f0->img[0].h, 8, 3, 1));
		} else if (gw->wc.ccol == BAYER){
			printf("APE = %f\n",utils_ape_16(gw->gop.frames[0].b.pic, gw->gop.frames[0].d.pic, gw->gop.frames[0].b.w*gw->gop.frames[0].b.h, 1));
			printf("PNSR = %f\n",utils_psnr_16(gw->gop.frames[0].b.pic, gw->gop.frames[0].d.pic, gw->gop.frames[0].b.w*gw->gop.frames[0].b.h, 1));
			printf("SSIM = %f\n",utils_ssim_16(gw->gop.frames[0].b.pic, gw->gop.frames[0].d.pic, gw->gop.frames[0].b.w, gw->gop.frames[0].b.h, 8, 3, 1));
		}
    }
}

void on_fill_button_clicked(GtkObject *object, GtkWalet *gw)
{
	if(&gw->gop == NULL ) return;
	//frame_predict_subband(&gw->gop, gw->gop.cur_gop_frame, &gw->wc);

	gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
	frame_fill_subb(&gw->gop, gw->gop.cur_gop_frame, &gw->wc);
	gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;

	printf("Fill subband time = %f\n",(double)(end-start)/1000000.);
}

void on_bits_button_clicked(GtkObject *object, GtkWalet *gw)
{
	if(&gw->gop == NULL ) return;
	gw->wc.comp = gtk_spin_button_get_value_as_int ((GtkSpinButton *)gw->times_spinbutton);
	gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
	frame_bits_alloc(&gw->gop, gw->gop.cur_gop_frame, &gw->wc, gw->wc.comp);
	gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
	printf("Bits allocation time = %f\n",(double)(end-start)/1000000.);
}

void on_quant_button_clicked(GtkObject *object, GtkWalet *gw)
{
	if(&gw->gop == NULL ) return;
	Frame *f0 = &gw->gop.frames[0];
	uint32 i;
	gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
	if(frame_quantization(&gw->gop, gw->gop.cur_gop_frame, &gw->wc)){
		gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
		printf("Quantization time = %f\n",(double)(end-start)/1000000.);

		for(i=0; i < 3; i++){
			new_buffer (gw->orig[i+1], f0->img[i].w, f0->img[i].h);
			utils_dwt_image_draw(&f0->img[i], gdk_pixbuf_get_pixels(gw->orig[i+1]->pxb), gw->wc.steps);
			gtk_widget_queue_draw(gw->drawingarea[i+1]);
		}

		//new_buffer (gw->orig[3], f0->img[0].w, f0->img[0].h);
		//utils_dwt_bayer_draw(&gw->gop, gw->gop.cur_gop_frame, &gw->wc, gdk_pixbuf_get_pixels(gw->orig[3]->pxb), gw->wc.steps);
		//gtk_widget_queue_draw(gw->drawingarea[3]);
	}
}

void on_range_enc_button_clicked(GtkObject *object, GtkWalet *gw)
{
	uint32 size;
	if(&gw->gop == NULL ) return;

	gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
	frame_range_encode(&gw->gop, gw->gop.cur_gop_frame, &gw->wc, &size);
	//frame_range(&gw->gop, gw->gop.cur_gop_frame, &gw->wc, &size);
	gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;

	printf("Frame size  = %d Encoded frame size = %d compess = %f bits_per_pix = %f time = %f\n",
			gw->wc.w*gw->wc.h, size, (double)(gw->wc.w*gw->wc.h)/(double)size,
			(double)(gw->wc.bpp*size)/(double)(gw->wc.w*gw->wc.h), (double)(end-start)/1000000.);
}

void on_range_dec_button_clicked(GtkObject *object, GtkWalet *gw)
{
	uint32 size;
	if(&gw->gop == NULL ) return;
	gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
	frame_range_decode(&gw->gop, gw->gop.cur_gop_frame, &gw->wc, &size);
	gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;

	printf("Decoded frame size = %d time = %f\n", size, (double)(end-start)/1000000.);
}

void on_compress_button_clicked(GtkObject *object, GtkWalet *gw)
{
	if(&gw->gop == NULL ) return;
	//gw->wc.comp = gtk_spin_button_get_value_as_int ((GtkSpinButton *)gw->times_spinbutton);
	//frame_compress(&gw->gop, gw->gop.cur_gop_frame, &gw->wc, gw->wc.comp);
	//printf("DISTR = %f\n",utils_dist(gw->gop.frames[0].img[0].img, gw->gop.frames[1].img[0].img, gw->gop.w*gw->gop.h, 1));
	//printf("APE   = %f\n",utils_ape (gw->gop.frames[0].img[0].img, gw->gop.frames[1].img[0].img, gw->gop.w*gw->gop.h, 1));
	//printf("PSNR  = %f\n",utils_psnr(gw->gop.frames[0].img[0].img, gw->gop.frames[1].img[0].img, gw->gop.w*gw->gop.h, 1));
}

void on_median_button_clicked(GtkObject *object, GtkWalet *gw)
{
	Frame *f0 = &gw->gop.frames[0];
	int i, sz = f0->img[0].w*f0->img[0].h;
	uint32 *y = f0->hist, *r = &y[1<<gw->wc.bpp], *g = &r[1<<gw->wc.bpp], *b = &g[1<<gw->wc.bpp];
    uint32 h[4096], look[4096];

    //utils_bayer_to_RGB		(f0->b.pic, f0->R16.pic, f0->G16.pic, f0->B16.pic, (int16*)gw->gop.buf, f0->b.w, f0->b.h,  gw->wc.bg);
    //utils_bayer_to_YUV444	(f0->b.pic, f0->Y16.pic, f0->U16.pic, f0->V16.pic, (int16*)gw->gop.buf, f0->b.w, f0->b.h,  gw->wc.bg);
    //make_lookup(f0->img[0].p, h, look, f0->b.w*f0->b.h, 12, 12);
    make_lookup1(f0->img[0].p, h, look, f0->b.w, f0->b.h, 12, 12);
    //make_lookup1(f0->b.pic, h, look, f0->b.w, f0->b.h, 12, 12);
    bits12to8(f0->b.pic, f0->d.pic, look, f0->b.w, f0->b.h, 12, 12);

    new_buffer (gw->orig[1], f0->b.w, f0->b.h);
    utils_bayer_to_RGB24(f0->d.pic, gdk_pixbuf_get_pixels(gw->orig[1]->pxb), (int16*)gw->gop.buf, f0->b.w, f0->b.h, gw->wc.bg, 8);
    gtk_widget_queue_draw(gw->drawingarea[1]);

}

void on_check_button_clicked(GtkObject *object, GtkWalet *gw)
{
	uint32 h, w, x, y, fn = 0, npix;
	uint8 *pic, *pic1;
	clock_t start, end;
	double time=0., tmp;
	struct timeval tv;
	Frame *frm1 = &gw->gop.frames[0];
	Frame *frm2 = &gw->gop.frames[1];

	if(&gw->gop == NULL ) return;

	frame_test(&gw->gop, 0, &gw->wc, 12);

	new_buffer (gw->orig[2], gw->gop.frames[0].d.w, gw->gop.frames[0].d.h);
	utils_grey_draw(gw->gop.frames[0].d.pic, gdk_pixbuf_get_pixels(gw->orig[2]->pxb), gw->gop.frames[0].d.w, gw->gop.frames[0].d.h, 0);
	gtk_widget_queue_draw(gw->drawingarea[2]);


	new_buffer (gw->orig[3], gw->gop.frames[0].d.w, gw->gop.frames[0].d.h);
	utils_bayer_to_RGB24(gw->gop.frames[0].d.pic, gdk_pixbuf_get_pixels(gw->orig[3]->pxb), (int16*)gw->gop.buf, gw->gop.frames[0].d.w, gw->gop.frames[0].d.h, gw->wc.bg, 128);
	gtk_widget_queue_draw(gw->drawingarea[3]);

	printf("APE = %f  ",utils_ape_16(gw->gop.frames[0].b.pic, gw->gop.frames[0].d.pic, gw->gop.frames[0].b.w*gw->gop.frames[0].b.h, 1));
	printf("PNSR = %f\n",utils_psnr_16(gw->gop.frames[0].b.pic, gw->gop.frames[0].d.pic, gw->gop.frames[0].b.w*gw->gop.frames[0].b.h, 1));
	//printf("SSIM = %f\n",utils_ssim_16(gw->gop.frames[0].b.pic, gw->gop.frames[0].d.pic, gw->gop.frames[0].b.w*gw->gop.frames[0].b.h, 8, 1));
	printf("SSIM = %f\n",utils_ssim_16(gw->gop.frames[0].b.pic, gw->gop.frames[0].d.pic, gw->gop.frames[0].b.w, gw->gop.frames[0].b.h, 8, 3, 1));
}

void on_next_button_clicked(GtkObject *object, GtkWalet *gw)
{
	GstStateChangeReturn ret;
	GstState state;
	uint32 i, j, w = gw->wc.w, h = gw->wc.h, bpp = gw->wc.bpp;
	clock_t start, end;
	double time=0., tmp;
	struct timeval tv;
	Frame *fr = &gw->gop.frames[0];
	int16 *r = fr->img[0].p, *g = fr->img[1].p, *b = fr->img[2].p;

	//utils_shift16(r, r, w, h, 128);
	//utils_shift(f->img[0].p, g->buf, f->Y.w,f->Y.h, 128);
	//filter_median(r, fr->Y.pic , fr->Y.w, fr->Y.h);


	gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
	frame_segmetation(&gw->gop, 0, &gw->wc);
	gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
	printf("seg_find_clusters_2d time = %f\n",(double)(end-start)/1000000.);

    //new_buffer (gw->orig[0], fr->y[0].w, fr->y[0].h);
    //utils_grey_draw8(fr->y[0].pic, gdk_pixbuf_get_pixels(gw->orig[0]->pxb), fr->y[0].w, fr->y[0].h, 0);
    //gtk_widget_queue_draw(gw->drawingarea[0]);

    new_buffer (gw->orig[0], fr->y[1].w, fr->y[1].h);
    utils_grey_draw8(fr->dm[0].pic, gdk_pixbuf_get_pixels(gw->orig[0]->pxb), fr->y[1].w, fr->y[1].h, 0);
    gtk_widget_queue_draw(gw->drawingarea[0]);

    new_buffer (gw->orig[1], fr->dg[0].w, fr->dg[0].h);
    utils_grey_draw8(fr->dg[0].pic, gdk_pixbuf_get_pixels(gw->orig[1]->pxb), fr->dg[0].w, fr->dg[0].h, 0);
    gtk_widget_queue_draw(gw->drawingarea[1]);

    new_buffer (gw->orig[2], fr->dc[0].w, fr->dc[0].h);
    utils_grey_draw8(fr->dc[0].pic, gdk_pixbuf_get_pixels(gw->orig[2]->pxb), fr->dc[0].w, fr->dc[0].h, 0);
    gtk_widget_queue_draw(gw->drawingarea[2]);

/*
        new_buffer (gw->orig[1], fr->y[1].w, fr->y[1].h);
        utils_grey_draw8(fr->y[1].pic, gdk_pixbuf_get_pixels(gw->orig[1]->pxb), fr->y[1].w, fr->y[1].h, 0);
        gtk_widget_queue_draw(gw->drawingarea[1]);

        new_buffer (gw->orig[2], fr->y1[0].w, fr->y1[0].h);
        utils_grey_draw8(fr->y1[0].pic, gdk_pixbuf_get_pixels(gw->orig[2]->pxb), fr->y1[0].w, fr->y1[0].h, 0);
        gtk_widget_queue_draw(gw->drawingarea[2]);
*/
        //utils_grey_draw8(fr->y1[1].pic, gdk_pixbuf_get_pixels(gw->orig[3]->pxb), fr->y1[1].w, fr->y1[1].h, 0);
	//utils_contour(fr->y1, gdk_pixbuf_get_pixels(gw->orig[3]->pxb), 1);
    /*
	new_buffer (gw->orig[3], fr->y1[1].w<<1, fr->y1[1].h<<1);
	utils_contour(fr->y1, gdk_pixbuf_get_pixels(gw->orig[3]->pxb), 1);
    gtk_widget_queue_draw(gw->drawingarea[3]);
    */
    /*
    new_buffer (gw->orig[3], fr->y1[1].w, fr->y1[1].h);
    utils_grey_draw8(fr->y1[1].pic, gdk_pixbuf_get_pixels(gw->orig[3]->pxb), fr->y1[1].w, fr->y1[1].h, 0);
    gtk_widget_queue_draw(gw->drawingarea[3]);
    */
    new_buffer (gw->orig[3], fr->y1[0].w, fr->y1[0].h);
    utils_grey_draw8(fr->y1[0].pic, gdk_pixbuf_get_pixels(gw->orig[3]->pxb), fr->y1[0].w, fr->y1[0].h, 0);
    gtk_widget_queue_draw(gw->drawingarea[3]);

	/*
	for(i=0; i < 4; i++){
		new_buffer (gw->orig[i], fr->dc[i].w<<i, fr->dc[i].h<<i);
		//utils_contour(fr->dc, gdk_pixbuf_get_pixels(gw->orig[i]->pxb), i);
		utils_contour(fr->di, gdk_pixbuf_get_pixels(gw->orig[i]->pxb), i);
		//utils_contour32(fr->rg, gdk_pixbuf_get_pixels(gw->orig[i]->pxb), i);
		//new_buffer (gw->orig[i], fr->R1[i].w<<i, fr->R1[i].h<<i);
		//utils_contour_rgb(fr->R1, fr->G1, fr->B1, gdk_pixbuf_get_pixels(gw->orig[i]->pxb), i);
		gtk_widget_queue_draw(gw->drawingarea[i]);
	}
	*/

	//new_buffer (gw->orig[2], fr->img[0].dc[0].w, fr->img[0].dc[0].h);
	//utils_contour_rgb_draw(&fr->img[0], gdk_pixbuf_get_pixels(gw->orig[2]->pxb));
	//gtk_widget_queue_draw(gw->drawingarea[2]);
	/*
	new_buffer (gw->orig[2], fr->img[0].w, fr->img[0].h);
	utils_gradient_draw(&fr->img[0], gdk_pixbuf_get_pixels(gw->orig[2]->pxb), gw->wc.steps);
	gtk_widget_queue_draw(gw->drawingarea[2]);

	new_buffer (gw->orig[3], fr->img[0].w, fr->img[0].h);
	utils_contour_draw(&fr->img[0], gdk_pixbuf_get_pixels(gw->orig[3]->pxb), gw->wc.steps);
	gtk_widget_queue_draw(gw->drawingarea[3]);
	*/
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

			//gtk_adjustment_set_page_size (gtk_viewport_get_hadjustment ((GtkViewport*)gw->viewport[4]), (gdouble)(gw->gop.w>>2));
			//gtk_adjustment_set_page_size (gtk_viewport_get_vadjustment ((GtkViewport*)gw->viewport[4]), (gdouble)(gw->gop.h>>2));
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

void on_fill_hist_clicked(GtkObject *object, GtkWalet *gw)
{
	Frame *fr = &gw->gop.frames[0];
	uint32 sz = 256, sz2 = sz<<1, width, height, hh = 200, sum, mx[4];
	uint32 rgb[sz*4], max=0, maxy=0, i, size = gw->wc.w*gw->wc.h;
	uint8 *img = gdk_pixbuf_get_pixels(gw->orig[4]->pxb);
	GdkGC *gc = gdk_gc_new (GDK_DRAWABLE (gw->drawingarea[4]->window));
	GdkColor color;


	//gw->hist = 1;
	hist = hist ? 0 : 1;
	if(hist){
		height = hh*4;  width = 256;

		//gw->drawingarea[4] = gtk_drawing_area_new();
		memset(rgb, 0, sizeof(uint32)*sz*4);

		gtk_widget_set_size_request(gw->drawingarea[4],  width, height);

		gw->pixmap = gdk_pixmap_new(gw->drawingarea[4]->window, width, height, 24);

		for(i=0; i< size; i++){
			rgb[      img[i*3  ]]++;
			rgb[sz  + img[i*3+1]]++;
			rgb[sz*2+ img[i*3+2]]++;
			rgb[sz*3+ lb1((306*img[i*3] + 601*img[i*3+1] + 117*img[i*3+2])>>10) ]++;
		}
		max = 0;
		for(i=0; i< sz*3; i++){
			if(rgb[i] > max) max = rgb[i];
			//printf("%d r = %3d \n", i, rgb[i]);
		}
		maxy = 0;
		for(i=sz*3; i< sz*4; i++){
			if(rgb[i] > maxy) maxy = rgb[i];
			//printf("%d r = %3d \n", i, rgb[i]);
		}

		sum = 0;
		for(i=0; i< sz; i++) {
			sum += rgb[i];
			//printf("R[%3d] = %7d\n", i, rgb[i]);
		}
		for(i=sz-1; i >= 0; i--) if(rgb[i])  {mx[0] = i; break;  }
		printf("R = %d max = %d\n", sum, mx[0]);

		sum = 0;
		for(i=sz; i< sz*2; i++) {
			sum += rgb[i];
			//printf("G[%3d] = %7d\n", i-sz, rgb[i]);
		}
		for(i=sz*2-1; i >= sz; i--) if(rgb[i])  {mx[1] = i-sz; break;  }
		printf("G = %d max = %d\n", sum, mx[1]);

		sum = 0;
		for(i=sz*2; i< sz*3; i++) {
			sum += rgb[i];
			//printf("B[%3d] = %7d\n", i-sz*2, rgb[i]);
		}
		for(i=sz*3-1; i >= sz*2; i--) if(rgb[i])  {mx[2] = i-2*sz; break;  }
		printf("B = %d max = %d\n", sum, mx[2]);

		sum = 0;
		for(i=sz*3; i< sz*4; i++) {
			sum += rgb[i];
			//printf("Y[%3d] = %7d\n", i-sz*3, rgb[i]);
		}
		for(i=sz*4-1; i >= sz*3; i--) if(rgb[i])  { mx[3] = i-3*sz; break;  }
		printf("Y = %d max = %d\n", sum, mx[3]);


		//new_buffer (gw->orig[4], 256, 256*3);
		//gdk_draw_line(gw->pixmap, gw->window1->style->black_gc, 0 , 0, 256, 256*3);
		color.pixel = 0; color.red = 0x9fff; color.green = 0x9fff; color.blue = 0x9fff;
		gdk_gc_set_rgb_fg_color (gc, &color);
		gdk_draw_rectangle(gw->pixmap, gc, TRUE, 0, 0, width, height);

		//Draw histogram
		color.pixel = 0; color.red = 0xffff; color.green = 0x0000; color.blue = 0x0000;
		gdk_gc_set_rgb_fg_color (gc, &color);
		for(i=0; i < 256; i++ ) gdk_draw_line(gw->pixmap, gc, i , hh, i, hh - (hh*rgb[i]/max));

		color.pixel = 0; color.red = 0x0000; color.green = 0xffff; color.blue = 0x0000;
		gdk_gc_set_rgb_fg_color (gc, &color);
		for(i=0; i < 256; i++ ) gdk_draw_line(gw->pixmap, gc, i , hh*2, i, hh*2 - (hh*rgb[sz+i]/max));

		color.pixel = 0; color.red = 0x0000; color.green = 0x0000; color.blue = 0xffff;
		gdk_gc_set_rgb_fg_color (gc, &color);
		for(i=0; i < 256; i++ ) gdk_draw_line(gw->pixmap, gc, i , hh*3, i, hh*3 - (hh*rgb[sz*2+i]/max));

		color.pixel = 0; color.red = 0x0000; color.green = 0x0000; color.blue = 0x0000;
		gdk_gc_set_rgb_fg_color (gc, &color);
		for(i=0; i < 256; i++ ) gdk_draw_line(gw->pixmap, gc, i , hh*4, i, hh*4 - (hh*rgb[sz*3+i]/maxy));


		gdk_draw_drawable(gw->drawingarea[4]->window,
				gw->drawingarea[5]->style->black_gc,
				gw->pixmap, 0 ,0 ,0 ,0 , -1,-1);


		//utils_grey_draw8(fr->y[0].pic, gdk_pixbuf_get_pixels(gw->orig[4]->pxb), fr->y[0].w, fr->y[0].h, 0);
		//gtk_widget_queue_draw(gw->drawingarea[4]);
	}
	gtk_widget_queue_draw(gw->drawingarea[4]);
}

gboolean on_drawingarea4_expose_event (GtkWidget *widget, GdkEventExpose *event, GtkWalet *gw)
{
	//gtk_widget_set_size_request(widget, w, h);
	//g_printf("on_drawingarea0_expose_event \n");
	if(hist == 1){
		gtk_widget_set_size_request(gw->drawingarea[4],  256, 256*3);
		gdk_draw_drawable(gw->drawingarea[4]->window,
				gw->drawingarea[5]->style->black_gc,
				gw->pixmap, 0 ,0 ,0 ,0 , -1,-1);

	} else {
		if(gw->orig[4]->init){
			if(gw->feet1) {
				feet(gw->viewport[4], gw->orig[4], gw->scal[4]);
				draw_image(gw->drawingarea[4], gw->scal[4]);
			} else{
				if(gw->zoom == 1.) 	draw_image(gw->drawingarea[4], gw->orig[4]);
				else				draw_image(gw->drawingarea[4], gw->scal[4]);

			}
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
	gw->fill_hist		= GTK_WIDGET (gtk_builder_get_object (builder, "fill_hist"));

    //g_signal_connect(G_OBJECT(gw->window1)			, "destroy", 		G_CALLBACK (on_quit_windows1), gw);
    g_signal_connect(G_OBJECT(gw->window1)			, "delete_event", 	G_CALLBACK (on_quit_windows1), gw);
    g_signal_connect(G_OBJECT(gw->feet_button1)		, "clicked"	, 		G_CALLBACK (on_feet_button1_clicked), gw);
    g_signal_connect(G_OBJECT(gw->full_button1)		, "clicked"	, 		G_CALLBACK (on_full_button1_clicked), gw);
    g_signal_connect(G_OBJECT(gw->fill_hist)		, "clicked"	, 		G_CALLBACK (on_fill_hist_clicked), gw);
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
