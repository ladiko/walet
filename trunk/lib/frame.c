#include <walet.h>

static uint32 check_state(uint32 state, uint32 check)
{
	if(state & check) {
		return 1;
	} else {
		printf("Can't make operation\n");
		return 0;
	}
}

void frames_init(GOP *gop, uint32 fr)
///	\fn	void frames_init(GOP *gop, uint32 fr)
///	\brief	Frame initialization
///	\param	gop			The GOP structure.
///	\param	fr			The frame number.
{
	Frame *frame = &gop->frames[fr];
	uint32 i, j, w = gop->width, h = gop->height ;
	//(Image *im, uint32 width, uint32 height, ColorSpace color, uint32 bpp, uint32 steps)
	image_init(&frame->img[0], w, h, gop->color, gop->bpp, gop->steps);
	frame->size = w*h;

	frame->pixs = (Pixel *)calloc((w>>1)*(h>>1), sizeof(Pixel));
	//RGB scale images
	for(j=0; j < 4; j++){
		frame->rgb[j].width  = w>>(j+1);
		frame->rgb[j].height = h>>(j+1);
		frame->rgb[j].pic = (uchar *)calloc(frame->rgb[j].width*frame->rgb[j].height*3, sizeof(uchar));
	}
	//Y component scale images
	for(j=0; j < 4; j++){
		frame->Y[j].width  = w>>(j+1);
		frame->Y[j].height = h>>(j+1);
		frame->Y[j].pic = (uchar *)calloc(frame->Y[j].width*frame->Y[j].height, sizeof(uchar));
	}
	//Gradient of  scaled images
	for(j=0; j < 4; j++){
		frame->grad[j].width  = w>>(j+1);
		frame->grad[j].height = h>>(j+1);
		frame->grad[j].pic = (uchar *)calloc(frame->grad[j].width*frame->grad[j].height, sizeof(uchar));
	}
	/*
	//Contours of scaled images
	for(j=0; j < 4; j++){
		frame->con[j].width  = w>>(j+1);
		frame->con[j].height = h>>(j+1);
		frame->con[j].pic = (uchar *)calloc(frame->con[j].width*frame->con[j].height, sizeof(uchar));
	}*/
	//Pixels of scaled images
	for(j=0; j < 4; j++){
		frame->pix[j].width  = w>>(j+1);
		frame->pix[j].height = h>>(j+1);
		frame->pix[j].pic = (uchar *)calloc(frame->pix[j].width*frame->pix[j].height, sizeof(uchar));
	}
	//Pointers to pixels array
	//frame->pixs = (Pixel *)calloc(frame->pix[0].width*frame->pix[0].height, sizeof(Pixel));

	if(gop->color == CS444 || gop->color == RGB) {
		image_init(&frame->img[1], w, h, gop->color, gop->bpp, gop->steps);
		image_init(&frame->img[2], w, h, gop->color, gop->bpp, gop->steps);
		frame->size = frame->size*3;
	}
	if(gop->color == CS422){
		image_init(&frame->img[1], w>>1 , h, gop->color, gop->bpp, gop->steps);
		image_init(&frame->img[2], w>>1 , h, gop->color, gop->bpp, gop->steps);
		frame->size = frame->size*2;
	}
	if(gop->color == CS420){
		image_init(&frame->img[1], w>>1 , h>>1, gop->color, gop->bpp, gop->steps);
		image_init(&frame->img[2], w>>1 , h>>1, gop->color, gop->bpp, gop->steps);
		frame->size = (frame->size*3)>>1;
	}
	frame->state = 0;
}

void frame_copy(GOP *gop, uint32 fr, uchar *y, uchar *u, uchar *v)
///	\fn	void frame_copy(GOP *gop, uint32 fr, uchar *y, uchar *u, uchar *v)
///	\brief	Fill frame from the stream.
///	\param	gop			The GOP structure.
///	\param	fr			The frame number.
///	\param	y			The pointer to Bayer, gray, red or Y  image data
///	\param	u			The pointer to green or U  image data
///	\param	v			The pointer to blue or V  image data
{
	if(gop == NULL ) return;
	Frame *frame = &gop->frames[fr];

	printf("Start frame copy \n");
	image_copy(&frame->img[0], gop->bpp, y);
	if(gop->color != GREY  && gop->color != BAYER) {
		image_copy(&frame->img[1], gop->bpp, u);
		image_copy(&frame->img[2], gop->bpp, v);
	}
	frame->state = FRAME_COPY;
}

uint32 frame_dwt(GOP *gop, uint32 fr, FilterBank fb)
///	\fn	void frame_dwt_53(GOP *gop, uint32 fr)
///	\brief	Discrete wavelets frame transform.
///	\param	gop			The GOP structure.
///	\param	fr			The frame number.
///	\retval				1 - if all OK, 0 - if not OK
{
	if(gop == NULL ) return 0;
	Frame *frame = &gop->frames[fr];
	if(check_state(frame->state, FRAME_COPY | IDWT)){
		frame->img[0].sub = gop->sub[0]; 	//Set current image for DWT transform
		if		(gop->color == BAYER) 	image_dwt(&frame->img[0], gop->color, gop->steps, gop->buf, MALLET, fb); //MALLET
		else if (gop->color == GREY) 	image_dwt(&frame->img[0], gop->color, gop->steps, gop->buf, CLASSIC, fb);
		else {
			frame->img[1].sub = gop->sub[1]; 	//Set current image for DWT transform
			frame->img[2].sub = gop->sub[2]; 	//Set current image for DWT transform
			image_dwt(&frame->img[1], gop->color, gop->steps, gop->buf, CLASSIC, fb);
			image_dwt(&frame->img[2], gop->color, gop->steps, gop->buf, CLASSIC, fb);
		}
		frame->state = DWT;
		return 1;
	} else return 0;
}

uint32 frame_idwt(GOP *gop, uint32 fr, uint32 isteps, FilterBank fb)
///	\fn	void frame_idwt_53(GOP *gop, uint32 fr, uint32 step)
///	\brief	Invert discrete wavelets frame transform.
///	\param	gop			The GOP structure.
///	\param	fr			The frame number.
/// \param	isteps		The steps of IDWT should be lees or equal DWT steps
///	\retval				1 - if all OK, 0 - if not OK
{
	if(gop == NULL ) return 0;
	Frame *frame = &gop->frames[fr];
	if(check_state(frame->state, RANGE_DECODER | DWT | QUANTIZATION)){
		frame->img[0].sub = gop->sub[0]; //Set current image for IDWT transform
		if		(gop->color == BAYER) 	image_idwt(&frame->img[0], gop->color, gop->steps, gop->buf, isteps, MALLET, fb);
		else if (gop->color == GREY) 	image_idwt(&frame->img[0], gop->color, gop->steps, gop->buf, isteps, CLASSIC, fb);
		else {
			frame->img[1].sub = gop->sub[1]; //Set current image for IDWT transform
			frame->img[2].sub = gop->sub[2]; //Set current image for IDWT transform
			image_idwt(&frame->img[1], gop->color, gop->steps, gop->buf, isteps, CLASSIC, fb);
			image_idwt(&frame->img[2], gop->color, gop->steps, gop->buf, isteps, CLASSIC, fb);
		}
		frame->state = IDWT;
		return 1;
	} else return 0;
}

uint32 frame_fill_subb(GOP *gop, uint32 fr)
///	\fn	void frame_fill_subb(GOP *gop, uint32 fr)
///	\brief	Fill distribution probability array.
///	\param	gop			The GOP structure.
///	\param	fr			The frame number.
///	\retval				1 - if all OK, 0 - if not OK
{
	if(gop == NULL ) return 0;
	Frame *frame = &gop->frames[fr];

	if(check_state(frame->state, DWT)){
		image_fill_subb(&frame->img[0], gop->color, gop->steps);
		if(gop->color != GREY  && gop->color != BAYER) {
			image_fill_subb(&frame->img[1], gop->color, gop->steps);
			image_fill_subb(&frame->img[2], gop->color, gop->steps);
		}
		frame->state |= FILL_SUBBAND;
		return 1;
	} else return 0;
}

uint32 frame_bits_alloc(GOP *gop, uint32 fr, uint32 times)
///	\fn	void frame_bits_alloc(GOP *gop, uint32 fr, uint32 times)
///	\brief	Bits allocation for frame.
///	\param	gop			The GOP structure.
///	\param	fr			The frame number.
///	\param	times		Compression times.
///	\retval				1 - if all OK, 0 - if not OK
{
	if(gop == NULL || times == 1) return 0;
	Frame *frame = &gop->frames[fr];

	if(check_state(frame->state, FILL_SUBBAND)){
		image_bits_alloc(&frame->img[0], gop->color, gop->steps, gop->bpp, times);
		if(gop->color != GREY  && gop->color != BAYER) {
			image_bits_alloc(&frame->img[1], gop->color, gop->steps, gop->bpp, times);
			image_bits_alloc(&frame->img[2], gop->color, gop->steps, gop->bpp, times);
		}
		frame->state |= BITS_ALLOCATION;
		return 1;
	} else return 0;
}

uint32 frame_quantization(GOP *gop, uint32 fr)
///	\fn	void frame_quantization(GOP *gop, uint32 fr)
///	\brief	Frame quantization.
///	\param	gop			The GOP structure.
///	\param	fr			The frame number.
///	\retval				1 - if all OK, 0 - if not OK
{
	if(gop == NULL ) return 0;
	Frame *frame = &gop->frames[fr];

	if(check_state(frame->state, BITS_ALLOCATION)){
		image_quantization(&frame->img[0], gop->color, gop->steps);
		if(gop->color != GREY  && gop->color != BAYER) {
			image_quantization(&frame->img[1], gop->color, gop->steps);
			image_quantization(&frame->img[2], gop->color, gop->steps);
		}
		frame->state |= QUANTIZATION;
		return 1;
	} else return 0;
}

uint32 frame_range_encode(GOP *gop, uint32 fr, uint32 *size)
///	\fn	uint32 frame_range_encode(GOP *gop, uint32 fr)
///	\brief	Frame range encoder.
///	\param	gop			The GOP structure.
///	\param	fr			The frame number.
//	\param	size		The size of encoded frame in bytes.
///	\retval				1 - if all OK, 0 - if not OK
{
	if(gop == NULL ) return 0;
	Frame *frame = &gop->frames[fr];

	if(check_state(frame->state, FILL_SUBBAND)){
		*size = 0;
		frame->img[0].c_size = image_range_encode(&frame->img[0], gop->color, gop->steps, gop->bpp, (uchar*)gop->buf);
		*size += frame->img[0].c_size;
		if(gop->color != GREY  && gop->color != BAYER) {
			frame->img[1].c_size = image_range_encode(&frame->img[1], gop->color, gop->steps, gop->bpp, (uchar*)gop->buf);
			frame->img[2].c_size = image_range_encode(&frame->img[2], gop->color, gop->steps, gop->bpp, (uchar*)gop->buf);
			*size += frame->img[1].c_size + frame->img[2].c_size;
		}
		frame->state |= RANGE_ENCODER;
		return 1;
	} else return 0;
}

uint32 frame_range_decode(GOP *gop, uint32 fr, uint32 *size)
///	\fn	uint32 frame_range_decode(GOP *gop, uint32 fr)
///	\brief	Frame range decoder.
///	\param	gop			The GOP structure.
///	\param	fr			The frame number.
//	\param	size		The size of decoded frame in bytes.
///	\retval				1 - if all OK, 0 - if not OK
{
	if(gop == NULL ) return 0;
	Frame *frame = &gop->frames[fr];

	if(check_state(frame->state, BUFFER_READ | RANGE_ENCODER)){
		*size = 0;
		*size += image_range_decode(&frame->img[0], gop->color, gop->steps, gop->bpp, (uchar*)gop->buf);
		if(gop->color != GREY  && gop->color != BAYER) {
			*size += image_range_decode(&frame->img[1], gop->color, gop->steps, gop->bpp, (uchar*)gop->buf);
			*size += image_range_decode(&frame->img[2], gop->color, gop->steps, gop->bpp, (uchar*)gop->buf);
		}
		frame->state |= RANGE_DECODER;
		return 1;
	} else return 0;
}

uint32 frame_median_filter(GOP *gop, uint32 fr)
///	\fn	void frame_quantization(GOP *gop, uint32 fr)
///	\brief	Frame quantization.
///	\param	gop			The GOP structure.
///	\param	fr			The frame number.
///	\retval				1 - if all OK, 0 - if not OK
{
	clock_t start, end;
	double time=0., tmp;
	struct timeval tv;

	if(gop == NULL ) return 0;
	Frame *frame = &gop->frames[fr];

	if(check_state(frame->state, IDWT | FRAME_COPY)){
		gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
		image_median_filter(&frame->img[0], gop->color, gop->bg, gop->buf);
		if(gop->color != GREY  && gop->color != BAYER) {
			image_median_filter(&frame->img[1], gop->color, gop->bg, gop->buf);
			image_median_filter(&frame->img[2], gop->color, gop->bg, gop->buf);
		}
		gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
		printf("Median filter time  = %f\n", (double)(end-start)/1000000.);

		frame->state |= MEDIAN_FILTER;
		return 1;
	} else return 0;
}

uint32 frame_subband_median_filter(GOP *gop, uint32 fr)
///	\fn	void frame_quantization(GOP *gop, uint32 fr)
///	\brief	Frame quantization.
///	\param	gop			The GOP structure.
///	\param	fr			The frame number.
///	\retval				1 - if all OK, 0 - if not OK
{
	if(gop == NULL ) return 0;
	Frame *frame = &gop->frames[fr];

	if(check_state(frame->state, DWT )){
		image_subband_median_filter(&frame->img[0], gop->color, gop->steps, gop->buf);
		if(gop->color != GREY  && gop->color != BAYER) {
			image_subband_median_filter(&frame->img[1], gop->color, gop->steps, gop->buf);
			image_subband_median_filter(&frame->img[2], gop->color, gop->steps, gop->buf);
		}
		frame->state |= MEDIAN_FILTER;
		return 1;
	} else return 0;
}

void frame_white_balance(GOP *gop, uint32 fr,  uint32 out_bits, Gamma gamma)
///	\fn	void frame_white_balance(GOP *gop, uint32 fr,  uint32 out_bits, Gamma gamma)
///	\brief	Make white balance and gamma correction of the frame (now for bayer frames only).
///	\param	gop			The GOP structure.
///	\param	fr			The frame number.
///	\param	out_bits	The bits per pixel for output frame.
///	\param	gamma		The type of gamma correction.
{
	Image *im = &gop->frames[fr].img[0];
	if(gop->color == BAYER){
		image_fill_hist(im, gop->color, gop->bg, gop->bpp);
		filters_white_balance(im->img, im->img, im->width, im->height, gop->bg, im->hist, im->look, gop->bpp, out_bits, gamma);
	}
}

void frame_segmetation(GOP *gop, uint32 fr)
///	\fn	void frame_white_balance(GOP *gop, uint32 fr,  uint32 out_bits, Gamma gamma)
///	\brief	Make white balance and gamma correction of the frame (now for bayer frames only).
///	\param	gop			The GOP structure.
///	\param	fr			The frame number.
{
	uint32 j, i, ncors=0, beg, diff, k, sq = gop->width*gop->height;
	Image *im = &gop->frames[fr].img[0];
	Frame *frm = &gop->frames[fr];
	clock_t start, end;
	double time=0., tmp;
	struct timeval tv;
	uint32 npix;


	if(gop->color == BAYER){
		gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;

		util_bayer_to_Y(im->img, gop->buf, gop->width, gop->height);
		filter_median(gop->buf, frm->Y[0].pic, frm->Y[0].width, frm->Y[0].height);
		//util_bayer_to_Y(im->img, frm->Y[0].pic, gop->width, gop->height);
		seg_grad(frm->Y[0].pic, frm->grad[0].pic, gop->buf, frm->Y[0].width, frm->Y[0].height, 4);
		//seg_local_max(frm->pixs, &npix, frm->grad[0].pic, frm->grad[0].width, frm->grad[0].height);
		seg_line(frm->pixs,  frm->grad[0].pic, frm->grad[0].width, frm->grad[0].height);
		seg_draw_lines(frm->pixs, npix, frm->pix[0].pic, frm->grad[0].width, frm->grad[0].height);
		//seg_draw_lines(frm->pixs, npix, frm->Y[0].pic, frm->grad[0].width, frm->grad[0].height);
		//seg_intersect_pix(frm->grad[0].pic, frm->pix[0].pic, frm->grad[0].width, frm->grad[0].height);

		gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
		printf("Segmentation time = %f\n", (double)(end-start)/1000000.);
	}
}

void frame_match(GOP *gop, uint32 fr1, uint32 fr2)
///	\fn	void frame_white_balance(GOP *gop, uint32 fr,  uint32 out_bits, Gamma gamma)
///	\brief	Make white balance and gamma correction of the frame (now for bayer frames only).
///	\param	gop			The GOP structure.
///	\param	fr			The frame number.
{
	uint32 j, i, ncors=0, beg, diff, k, sq = gop->width*gop->height;
	Frame *frm1 = &gop->frames[fr1];
	Frame *frm2 = &gop->frames[fr2];
	clock_t start, end;
	double time=0., tmp;
	struct timeval tv;
	uint32 npix;


	if(gop->color == BAYER){
		gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;

		seg_compare(frm1->pixs, frm2->pixs, frm1->grad[0].pic, frm2->grad[0].pic, frm1->Y[0].pic, frm2->Y[0].pic, frm1->grad[0].width, frm1->grad[0].height);

		seg_draw_vec(frm1->pixs, npix, frm1->pix[0].pic, frm1->grad[0].width, frm1->grad[0].height);

		seg_mvector_copy(frm1->pixs, frm1->grad[0].pic, frm1->Y[0].pic, frm2->pix[0].pic, frm1->grad[0].width, frm1->grad[0].height);
		//seg_draw_lines(frm2->pixs, npix, frm2->pix[0].pic, frm1->grad[0].width, frm1->grad[0].height);

		gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
		printf("Frame match time = %f\n", (double)(end-start)/1000000.);
	}
}

uint32 frame_write(GOP *gop, uint32 fr, FILE *wl)
///	\fn	uint32 frame_write(GOP *gop, uint32 fr, const char *filename)
///	\brief	Write frame in file.
///	\param	gop			The pointer to the GOP structure.
///	\param	fr			The frame number.
///	\param	filename	The file name.
///	\retval				The size of file.
{
    //FILE *wl;
    //WaletHeader wh;
    uchar *bits;
    uint32 i, sz, size=0, fsize;
    Frame *frame = &gop->frames[fr];
    uint32  rgb = (gop->color != GREY  && gop->color != BAYER);

    //Fill header
    //wh.marker	= 0x776C;
    //wh.width	= gop->width;
    //wh.height	= gop->height;
    //wh.color	= gop->color;
    //wh.bg		= gop->bg;
    //wh.bpp		= gop->bpp;
    //wh.steps	= gop->steps;
    //wh.gop_size	= gop->gop_size;
    //wh.rates	= gop->rates;

    //Fill and write bits array
    sz = (gop->color == BAYER) ? ((gop->steps-1)*3+1)<<2 : (gop->steps*3 + 1);
    bits = (uchar *)calloc(sz, sizeof(uchar));
    for(i=0; i<sz; i++)  {
    	bits[i] = (frame->img[0].sub[i].a_bits<<4) | frame->img[0].sub[i].q_bits;
    	//printf("%d a_bits = %d q_bits = %d bits = %d\n", i, frame->img[0].sub[i].a_bits, frame->img[0].sub[i].q_bits, bits[i]);
    }
    fwrite (bits, 1, sz , wl); size += sz;
    fsize = frame->img[0].c_size;

    if(rgb) {
     	for(i=0; i<sz; i++)  bits[i] = (frame->img[1].sub[i].a_bits<<4) | frame->img[1].sub[i].q_bits;
    	fwrite (bits, 1, sz , wl); size += sz ;
    	for(i=0; i<sz; i++)  bits[i] = (frame->img[2].sub[i].a_bits<<4) | frame->img[2].sub[i].q_bits;
    	fwrite (bits, 1, sz , wl); size += sz ;
    	fsize += frame->img[1].c_size + frame->img[2].c_size;
    }

    //Write compress image size
    fwrite (&fsize, 4, 1, wl); size += 4;
    //Write data
    fwrite (gop->buf, 1, fsize, wl); size += fsize;
    //printf("File size = %d fist = %2X %2X\n", fsize, ((char*)gop->buf)[0], ((char*)gop->buf)[1]);

    //fwrite (gop->buf, 1, frm->img[0].c_size, wl); size += frm->img[0].c_size;

   // if(rgb) {
    	//fwrite (&frm->img[1].c_size, 4, 1, wl); size += 4;
    	//fwrite (&gop->buf[frm->img[0].c_size], 1, frm->img[1].c_size, wl); size += frm->img[1].c_size;
    	//fwrite (&frm->img[2].c_size, 4, 1, wl); size += 4;
    	//fwrite (&gop->buf[frm->img[0].c_size+frm->img[1].c_size], 1, frm->img[2].c_size, wl); size += frm->img[2].c_size;
    //}
    free(bits);
    return size;
}

uint32 frame_read(GOP *gop, uint32 fr, FILE *wl)
///	\fn	uint32 frame_write(GOP *gop, uint32 fr, const char *filename)
///	\brief	Read frame from file.
///	\param	gop			The pointer to the GOP structure.
///	\param	fr			The frame number.
//	\param	filename	The file name.
///	\retval				The size of file.
{
    uchar *bits;
    uint32 i, sz, size = 0, fsize;
    Frame *frame = &gop->frames[fr];
    uint32  rgb = (gop->color != GREY  && gop->color != BAYER);

    sz = (gop->color == BAYER) ? ((gop->steps-1)*3+1)<<2 : (gop->steps*3 + 1);
    bits = (uchar *)calloc(sz, sizeof(uchar));

     //Read bits array
    if(fread (bits, 1, sz, wl)!= sz) { printf("Bits array read error\n"); return 0; }
    size += sz;
   //Fill bits array;
    for(i=0; i<sz; i++)  {
    	frame->img[0].sub[i].a_bits = (bits[i]>>4);
    	frame->img[0].sub[i].q_bits = (bits[i]&15);
    	//printf("%d a_bits = %d q_bits = %d bits = %d \n", i, frame->img[0].sub[i].a_bits, frame->img[0].sub[i].q_bits, bits[i]);
    }

    if(rgb) {
        if(fread (bits, 1, sz, wl)!= sz) { printf("Bits array read error\n"); return 0; }
        size += sz;
        for(i=0; i<sz; i++)  { frame->img[1].sub[i].a_bits = (bits[i]>>4);  frame->img[1].sub[i].q_bits = (bits[i]&0xF); }
        if(fread (bits, 1, sz, wl)!= sz) { printf("Bits array read error\n"); return 0; }
        size += sz;
        for(i=0; i<sz; i++)  { frame->img[2].sub[i].a_bits = (bits[i]>>4);  frame->img[2].sub[i].q_bits = (bits[i]&0xF); }
    }
    //Read compress image size
    if(fread (&fsize, 4, 1, wl)!= 1) { printf("compress image size read error\n"); return 0; }
    size += 4;
    //Read data
    if(fread (gop->buf, 1, fsize, wl)!= fsize) { printf("data read error\n"); return 0; }
    size += fsize;

    free(bits);
    //printf("File size = %d fist = %2X %2X\n", fsize, ((char*)gop->buf)[0], ((char*)gop->buf)[1]);

    frame->state = BUFFER_READ;

    return size;
 }

void frame_compress(GOP *gop, uint32 fr, uint32 times, FilterBank fb)
{
	clock_t start, end;
	double time=0., tmp;
	struct timeval tv;
	uint32 size;

	gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
	frame_dwt			(gop, fr, fb);
	gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
	tmp = (double)(end-start)/1000000.; time +=tmp;
	printf("DWT time             = %f\n", tmp);

	gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
	frame_fill_subb		(gop, fr);
	gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
	tmp = (double)(end-start)/1000000.; time +=tmp;
	printf("Fill subband time    = %f\n", tmp);

	gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
	frame_bits_alloc	(gop, fr, times);
	gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
	tmp = (double)(end-start)/1000000.; time +=tmp;
	printf("Bits allocation time = %f\n", tmp);

	gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
	frame_range_encode	(gop, fr, &size);
	gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
	tmp = (double)(end-start)/1000000.; time +=tmp;
	printf("Range coder time     = %f\n", tmp);

	printf("Frame time = %f size  = %d\n", time, size);
	//frame_write
}

void frame_decompress(GOP *gop, uint32 fr, uint32 isteps, FilterBank fb)
{
	uint32 size;
	clock_t start, end;
	struct timeval tv;
	double time=0., tmp;

	gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
	frame_range_decode	(gop, fr, &size);
	gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
	tmp = (double)(end-start)/1000000.; time +=tmp;
	printf("Range decoder time    = %f\n", tmp);

	gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
	frame_idwt			(gop, fr, isteps, fb);
	gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
	tmp = (double)(end-start)/1000000.; time +=tmp;
	printf("IDWT time             = %f\n", tmp);

	printf("Frame time = %f size  = %d\n", time, size);
}
