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

void frames_init(WaletConfig *wc, Frame *f){
///	\fn	void frames_init(wc *wc, uint32 fr)
///	\brief	Frame initialization
///	\param	wc			The wc structure.
///	\param	fr			The frame number.

	uint32 w = wc->w, h = wc->h;
	//New init

	if(wc->color == BAYER ){
	    f->b.w = w; f->b.h = h;
	    f->b.pic = (int16 *)calloc(f->b.w*f->b.h, sizeof(int16));
	    f->d.w = w; f->d.h = h;
	    f->d.pic = (int16 *)calloc(f->d.w*f->d.h, sizeof(int16));
		//Init color components
		image_init(&f->img[0], (w>>1) + (w&1), (h>>1) + (h&1), wc->color, wc->bpp, wc->steps);
		image_init(&f->img[1], (w>>1)        , (h>>1) + (h&1), wc->color, wc->bpp, wc->steps);
		image_init(&f->img[2], (w>>1) + (w&1), (h>>1)        , wc->color, wc->bpp, wc->steps);
		image_init(&f->img[3], (w>>1)        , (h>>1)        , wc->color, wc->bpp, wc->steps);
	} else if(wc->color == CS420){
		image_init(&f->img[0], w,    h,    wc->color, wc->bpp, wc->steps);
		image_init(&f->img[1], w>>1, h>>1, wc->color, wc->bpp, wc->steps);
		image_init(&f->img[2], w>>1, h>>1, wc->color, wc->bpp, wc->steps);
	}

	f->size = w*h;

	f->pixs = (Pixel *)calloc((w>>1)*(h>>1), sizeof(Pixel));
	f->edges = (Edge *)calloc((w>>2)*(h>>2), sizeof(Edge));

	f->rgb.w  = w;
	f->rgb.h = h;
	f->rgb.pic = (uint8 *)calloc(f->rgb.w*f->rgb.h*3, sizeof(uint8));
	f->Y.w  = w;
	f->Y.h = h;
	f->Y.pic = (uint8 *)calloc(f->rgb.w*f->rgb.h*3, sizeof(uint8));
	f->grad.w  = w;
	f->grad.h = h;
	f->grad.pic = (uint8 *)calloc(f->rgb.w*f->rgb.h*3, sizeof(uint8));


	f->line.w  = w;
	f->line.h = h;
	f->line.pic = (uint8 *)calloc(f->line.w*f->line.h, sizeof(uint8));
	f->edge.w  = w;
	f->edge.h = h;
	f->edge.pic = (uint8 *)calloc(f->edge.w*f->edge.h, sizeof(uint8));
	f->vec.w  = w;
	f->vec.h = h;
	f->vec.pic = (uint8 *)calloc(f->vec.w*f->vec.h, sizeof(uint8));
	//f->pixp = (uint32 *)calloc(f->vec.width*f->vec.height, sizeof(uint32));
	/*
	if(wc->color == CS444 || wc->color == RGB) {
		image_init(&f->img[1], w, h, wc->color, wc->bpp, wc->steps);
		image_init(&f->img[2], w, h, wc->color, wc->bpp, wc->steps);
		f->size = f->size*3;
	}
	if(wc->color == CS422){
		image_init(&f->img[1], w>>1 , h, wc->color, wc->bpp, wc->steps);
		image_init(&f->img[2], w>>1 , h, wc->color, wc->bpp, wc->steps);
		f->size = f->size*2;
	}
	if(wc->color == CS420){
		image_init(&f->img[1], w>>1 , h>>1, wc->color, wc->bpp, wc->steps);
		image_init(&f->img[2], w>>1 , h>>1, wc->color, wc->bpp, wc->steps);
		f->size = (f->size*3)>>1;
	}*/
	f->state = 0;
}

void frame_copy(WaletConfig *wc,  Frame *f, uint8 *y, uint8 *u, uint8 *v){
///	\fn	void frame_copy(wc *wc, uint32 fr, uint8 *y, uint8 *u, uint8 *v)
///	\brief	Fill frame from the stream.
///	\param	wc			The wc structure.
///	\param	fr			The frame number.
///	\param	y			The pointer to Bayer, gray, red or Y  image data
///	\param	u			The pointer to green or U  image data
///	\param	v			The pointer to blue or V  image data

	uint32 i, size = wc->w*wc->h, shift = 1<<(wc->bpp-1);
	if(wc == NULL ) return;


	if(wc->color == BAYER) {
		if(wc->bpp > 8) 	for(i=0; i<size; i++) f->b.pic[i] = ((v[i<<1]<<8) | v[(i<<1)+1]) - shift;
		else for(i=0; i<size; i++) f->b.pic[i] = y[i] - shift;
		//image_copy(f->b.pic, wc->bpp, y);

	} else {
		image_copy(&f->img[0], wc->bpp, y);
		if(wc->color != GREY  && wc->color != BAYER) {
			image_copy(&f->img[1], wc->bpp, u);
			image_copy(&f->img[2], wc->bpp, v);
		}
	}
	f->state = FRAME_COPY;
	printf("Finesh frame copy \n");
}

uint32 frame_dwt(WaletConfig *wc,  Frame *f, uint8 *buf){
///	\fn	void frame_dwt_53(wc *wc, uint32 fr)
///	\brief	Discrete wavelets frame transform.
///	\param	wc			The wc structure.
///	\param	fr			The frame number.
///	\retval				1 - if all OK, 0 - if not OK

	uint32 i;
	if(wc == NULL ) return 0;
	//DWT taransform
	if(check_state(f->state, FRAME_COPY | IDWT)){
		if (wc->color == GREY) image_dwt(&f->img[0], (int16*)buf, wc->fb, wc->steps);
		else if(wc->color == BAYER) {
			//Color transform
			dwt_53_2d_one(f->b.pic, f->img[0].p, f->img[1].p, f->img[2].p, f->img[3].p, (int16*)buf, f->b.w, f->b.h);
			for(i=0; i < 4; i++) {
				image_dwt(&f->img[i], (int16*)buf, wc->fb, wc->steps);
				//printf("img[%d]\n",i);
			}
		} else for(i=0; i < 3; i++) image_dwt(&f->img[i], (int16*)buf, wc->fb, wc->steps);

		f->state = DWT;
		//image_grad(&frame->img[0], BAYER, wc->steps, 2);
		return 1;
	} else return 0;
}

uint32 frame_idwt(WaletConfig *wc,  Frame *f, uint8 *buf, uint32 isteps){
///	\fn	void frame_idwt_53(wc *wc, uint32 fr, uint32 step)
///	\brief	Invert discrete wavelets frame transform.
///	\param	wc			The wc structure.
///	\param	fr			The frame number.
/// \param	isteps		The steps of IDWT should be lees or equal DWT steps
///	\retval				1 - if all OK, 0 - if not OK

	uint32 i;
	if(wc == NULL ) return 0;
	if(check_state(f->state, DWT)){
		if (wc->color == GREY) image_idwt(&f->img[0], (int16*)buf, wc->fb, wc->steps, isteps);
		else if(wc->color == BAYER ) {
			for(i=0; i < 4; i++) image_idwt(&f->img[i], (int16*)buf, wc->fb, wc->steps, isteps);
			//Color transform
			f->d.w = f->img[0].d.w + f->img[1].d.w;
			f->d.h = f->img[0].d.h + f->img[2].d.h;
			idwt_53_2d_one(f->d.pic, f->img[0].d.pic, f->img[1].d.pic, f->img[2].d.pic, f->img[3].d.pic,
					(int16*)buf, f->d.w, f->d.h);
		}
		else for(i=0; i < 3; i++) image_idwt(&f->img[i], (int16*)buf, wc->fb, wc->steps, isteps);

		f->state = IDWT;
		return 1;
	} else return 0;
}

uint32 frame_fill_subb(WaletConfig *wc,  Frame *f){
///	\fn	void frame_fill_subb(wc *wc, uint32 fr)
///	\brief	Fill distribution probability array.
///	\param	wc			The wc structure.
///	\param	fr			The frame number.
///	\retval				1 - if all OK, 0 - if not OK

	uint32 i;
	if(wc == NULL ) return 0;
	f->qst = 0;

	if(check_state(f->state, DWT)){
		if(wc->color == GREY) {
			image_fill_subb(&f->img[0], wc->steps);
			f->qst += f->img[0].qst;
		}
		else if(wc->color == BAYER){
			for(i=0; i < 4; i++)  {
			image_fill_subb(&f->img[i], wc->steps);
			f->qst += f->img[i].qst;
			}
			//printf("iframe->qst = %d\n", frame->qst);
		}
		else
			for(i=0; i < 3; i++)  {
			image_fill_subb(&f->img[i], wc->steps);
			f->qst += f->img[i].qst;
		}
		f->state |= FILL_SUBBAND;
		return 1;
	} else return 0;
}

uint32 frame_bits_alloc(WaletConfig *wc,  Frame *f, uint32 times){
///	\fn	void frame_bits_alloc(wc *wc, uint32 fr, uint32 times)
///	\brief	Bits allocation for frame.
///	\param	wc			The wc structure.
///	\param	fr			The frame number.
///	\param	times		Compression times.
///	\retval				1 - if all OK, 0 - if not OK
// Energy of one level DWT transform
// |--------|--------|
// |   LL   |   HL   |
// |   4    |   2    |
// |        |        |
// |--------|--------|
// |   LH   |   HH   |
// |   2    |   1    |
// |        |        |
// |--------|--------|

	//The order of bit allocation for subband
	uint32 qo[9] = {0, 0, 0, 0, 1, 2, 1, 2, 3};
	uint32 i, j, k, size, qs, qs1, df, s, df1;
	uint32 bl[4];
	if(wc == NULL || times == 1) return 0;
	size = (wc->w*wc->h*wc->bpp)/times;
	df1 = size;

	if(check_state(f->state, FILL_SUBBAND)){
		if (wc->color == BAYER){
			qs = f->qst>>1;
			for(k=2;;k++){
				//printf("qs = %d\n", qs);
				for(i=0; i < 4; i++) bl[i] = 0;
				//Bits allocation between 4 color image
				for(i=0, j=0; i < qs; i++) {
					if(bl[qo[j]] < f->img[qo[j]].qst) bl[qo[j]]++;
					else i--;
					j = (j == 8) ? 0 : j + 1;
				}
				for(j=0; j < 4; j++) printf("bl[%d] = %d\n", j, bl[j]);
				s = 0;
				for(j=0; j < 4; j++) s += image_size(&f->img[j], wc->steps, bl[j]);
					//printf("img = %d\n", j);

				//printf("times = %d qst = %d size = %d qs = %d s = %d\n", times, frame->qst, size, qs, s);
				//printf("Frame size = %d now = %d\n", size, s);

				if(!(f->qst>>k)) {
					break;
					//Fine tuning of the bit allocation
					/*
					df = abs(size - s);
					qs1 = qs;
					qs = (s < size) ? qs + 1 : qs - 1;
					printf("qs = %d qs1 = %d\n", qs, qs1);
					if(df  > df1 ){
						qs = qs1;
						break;
					}
					df1 = df;*/
				}
				else qs = (s < size) ? qs + (f->qst>>k) : qs - (f->qst>>k);
			}
		}
		printf("Frame size = %d bits  %d bites qs = %d\n", s, s/8, qs);
		f->state |= BITS_ALLOCATION;
		return 1;
	} else return 0;
}

uint32 frame_quantization(WaletConfig *wc, Frame *f, uint8 *buf){
///	\fn	void frame_quantization(wc *wc, uint32 fr)
///	\brief	Frame quantization.
///	\param	wc			The wc structure.
///	\param	fr			The frame number.
///	\retval				1 - if all OK, 0 - if not OK

	uint32 i;
	if(wc == NULL ) return 0;

	if(check_state(f->state, BITS_ALLOCATION)){
		if(wc->color == GREY) image_quantization(&f->img[0], wc->steps, buf);
		else if(wc->color == BAYER)
			for(i=0; i < 4; i++)  image_quantization(&f->img[i], wc->steps, buf);
		else
			for(i=0; i < 3; i++)  image_quantization(&f->img[i], wc->steps, buf);

		f->state |= QUANTIZATION;
		return 1;
	} else return 0;
}

uint32 frame_range_encode(WaletConfig *wc,  Frame *f, uint32 *size, uint8 *buf, int *ibuf){
///	\fn	uint32 frame_range_encode(wc *wc, uint32 fr)
///	\brief	Frame range encoder.
///	\param	wc			The wc structure.
///	\param	fr			The frame number.
//	\param	size		The size of encoded frame in bytes.
///	\retval				1 - if all OK, 0 - if not OK

	uint32 i;
	if(wc == NULL ) return 0;

	if(check_state(f->state, FILL_SUBBAND)){
		*size = 0;
		if(wc->color == GREY) *size += image_range_encode(&f->img[0], wc->steps, wc->bpp, buf, ibuf, wc->rt);
		else if(wc->color == BAYER)
			for(i=0; i < 4; i++)  *size += image_range_encode(&f->img[i], wc->steps, wc->bpp, &buf[*size], ibuf, wc->rt);
		else
			for(i=0; i < 3; i++)  *size += image_range_encode(&f->img[i], wc->steps, wc->bpp, &buf[*size], ibuf, wc->rt);

		f->state |= RANGE_ENCODER;
		return 1;
	} else return 0;
}

uint32 frame_range_decode(WaletConfig *wc,  Frame *f, uint32 *size, uint8 *buf, int *ibuf){
///	\fn	uint32 frame_range_decode(wc *wc, uint32 fr)
///	\brief	Frame range decoder.
///	\param	wc			The wc structure.
///	\param	fr			The frame number.
//	\param	size		The size of decoded frame in bytes.
///	\retval				1 - if all OK, 0 - if not OK

	uint32 i;
	if(wc == NULL ) return 0;

	if(check_state(f->state, BUFFER_READ | RANGE_ENCODER)){
		*size = 0;
		if(wc->color == GREY) *size += image_range_decode(&f->img[0], wc->steps, wc->bpp, buf, ibuf, wc->rt);
		else if(wc->color == BAYER)
			for(i=0; i < 4; i++)  *size += image_range_decode(&f->img[i], wc->steps, wc->bpp, &buf[*size], ibuf, wc->rt);
		else
			for(i=0; i < 3; i++)  *size += image_range_decode(&f->img[i], wc->steps, wc->bpp, &buf[*size], ibuf, wc->rt);
		f->state |= RANGE_DECODER;
		return 1;
	} else return 0;
}

uint32 frame_median_filter(WaletConfig *wc,  Frame *f, uint8 *buf){
///	\fn	void frame_quantization(wc *wc, uint32 fr)
///	\brief	Frame quantization.
///	\param	wc			The wc structure.
///	\param	fr			The frame number.
///	\retval				1 - if all OK, 0 - if not OK

	uint32 i;
	clock_t start, end;
	double time=0., tmp;
	struct timeval tv;

	if(wc == NULL ) return 0;

	if(check_state(f->state, IDWT | FRAME_COPY)){
		gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;

		if(wc->color == GREY) image_median_filter(&f->img[0], buf);
		else if(wc->color == BAYER)
			filter_median_bayer(f->b.pic, (int16*)buf, wc->w, wc->h);
		else
			for(i=0; i < 3; i++)  image_median_filter(&f->img[i], buf);

		f->state |= MEDIAN_FILTER;
		gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
		printf("Median filter time  = %f\n", (double)(end-start)/1000000.);

		return 1;
	} else return 0;
}

/*
void frame_fill_hist(wc *wc, uint32 fr)
///	\fn void image_fill_hist(Image *im, ColorSpace color, BayerGrid bg, uint32 bpp)
///	\brief Fill color histogram for white balancing.
///	\param im	 		The image structure.
///	\param color 		The color space of the stream.
///	\param bpp 			The bits per pixel.
///	\param bg			The bayer grid pattern
///	\param bpp 			The bits per pixel.
{
	uint32 i, size = im->w*im->h, sz = 1<<bpp, sum;
	uint32	tmp = size;
	if(color == BAYER) {
		fill_bayer_hist(im->p, im->hist, &im->hist[sz], &im->hist[sz*2], im->w, im->h, bg, bpp);
		sum = 0; for(i=0; i<sz; i++) sum +=im->hist[i]; tmp -= sum;
		printf("size = %d r = %d ", size, sum);
		sum = 0; for(i=0; i<sz; i++) sum +=im->hist[sz+i]; tmp -= sum;
		printf("g = %d ", sum);
		sum = 0; for(i=0; i<sz; i++) sum +=im->hist[(sz<<1)+i]; tmp -= sum;
		printf("b = %d  diff = %d\n", sum, tmp);
	}
	else  for(i=0; i < size; i++) im->hist[im->p[i]]++;

}
*/


void frame_white_balance(WaletConfig *wc,  Frame *f, uint32 out_bits, Gamma gamma){
///	\fn	void frame_white_balance(wc *wc, uint32 fr,  uint32 out_bits, Gamma gamma)
///	\brief	Make white balance and gamma correction of the frame (now for bayer frames only).
///	\param	wc			The wc structure.
///	\param	fr			The frame number.
///	\param	out_bits	The bits per pixel for output frame.
///	\param	gamma		The type of gamma correction.

	Image *im = &wc->frames[fr].img[0];
	if(wc->color == BAYER){
		//image_fill_hist(wc->frames[fr].b.pic, wc->color, wc->bg, wc->bpp);
		filters_white_balance(wc->frames[fr].b.pic, wc->frames[fr].b.pic, wc->frames[fr].b.w, wc->frames[fr].b.h,
				wc->bg, im->hist, im->look, wc->bpp, out_bits, gamma);
	}
}

void frame_segmetation(WaletConfig *wc,  Frame *f){
///	\fn	void frame_white_balance(wc *wc, uint32 fr,  uint32 out_bits, Gamma gamma)
///	\brief	Make white balance and gamma correction of the frame (now for bayer frames only).
///	\param	wc			The wc structure.
///	\param	fr			The frame number.

	uint32 j, i, ncors=0, beg, diff, k, nedge;// sq = wc->width*wc->height;
	Image *im = &wc->frames[fr].img[0];
	Frame *frm = &wc->frames[fr];
	clock_t start, end;
	double time=0., tmp;
	struct timeval tv;
	uint32 npix, sq = frm->grad.w*frm->grad.h;


	if(wc->color == BAYER){
		gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;

		//utils_bayer_to_Y(im->p, wc->buf, wc->w, wc->h);
		//filter_median(wc->buf, frm->Y.pic, frm->Y.w, frm->Y.h);
		//seg_grad(frm->Y.pic, frm->grad.pic, frm->Y.w, frm->Y.h, 3);

		//seg_grad1(frm->Y[0].pic, frm->grad[0].pic, frm->edge.pic, frm->Y[0].width, frm->Y[0].height, 3);

		//seg_fall_forest(frm->grad[0].pic, frm->line.pic, frm->grad[0].width, frm->grad[0].height);
		//seg_fall_forest1(frm->grad[0].pic, frm->edge.pic, frm->line.pic, frm->grad[0].width, frm->grad[0].height);
		//frm->nedge = seg_line(frm->pixs, frm->edges, frm->grad[0].pic, frm->grad[0].width, frm->grad[0].height);
		seg_pixels(frm->pixs, frm->grad.pic, frm->grad.w, frm->grad.h);
		//seg_region(frm->pixs, frm->grad[0].pic, frm->grad[0].width, frm->grad[0].height);
		//seg_reduce_line(frm->pixs,  frm->grad[0].pic, frm->grad[0].width, frm->grad[0].height);


		gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
		printf("Segmentation time = %f\n", (double)(end-start)/1000000.);
	}
}

void frame_match(WaletConfig *wc,  Frame *f1,  Frame *f2){
///	\fn	void frame_white_balance(wc *wc, uint32 fr,  uint32 out_bits, Gamma gamma)
///	\brief	Make white balance and gamma correction of the frame (now for bayer frames only).
///	\param	wc			The wc structure.
///	\param	fr			The frame number.

	uint32 j, i, ncors=0, beg, diff, k, sq = wc->w*wc->h;
	Frame *frm1 = &wc->frames[fr1];
	Frame *frm2 = &wc->frames[fr2];
	clock_t start, end;
	double time=0., tmp;
	struct timeval tv;
	uint32 npix;


	if(wc->color == BAYER){
		gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;

		seg_compare(frm1->pixs,  frm1->edges, frm1->nedge, frm1->grad.pic, frm2->grad.pic, frm1->Y.pic, frm2->Y.pic, wc->buf, frm1->grad.w, frm1->grad.h, wc->mvs);
		//for(i=0; i < sq; i++) frm2->pix[0].pic[i] = 0;
		//seg_mvector_copy(frm1->pixs, frm1->grad[0].pic, frm1->Y[0].pic, frm2->line.pic, frm1->grad[0].width, frm1->grad[0].height);

		gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
		printf("Frame match time = %f\n", (double)(end-start)/1000000.);
	}
}

uint32 frame_write(WaletConfig *wc,  Frame *f, FILE *wl){
///	\fn	uint32 frame_write(wc *wc, uint32 fr, const char *filename)
///	\brief	Write frame in file.
///	\param	wc			The pointer to the wc structure.
///	\param	fr			The frame number.
///	\param	filename	The file name.
///	\retval				The size of file.

    //FILE *wl;
    //WaletHeader wh;
    uint8 *bits;
    uint32 i, sz, size=0, fsize;
    Frame *frame = &wc->frames[fr];
    uint32  rgb = (wc->color != GREY  && wc->color != BAYER);

    //Fill header
    //wh.marker	= 0x776C;
    //wh.width	= wc->width;
    //wh.height	= wc->height;
    //wh.color	= wc->color;
    //wh.bg		= wc->bg;
    //wh.bpp		= wc->bpp;
    //wh.steps	= wc->steps;
    //wh.wc_size	= wc->wc_size;
    //wh.rates	= wc->rates;

    //Fill and write bits array
    sz = (wc->color == BAYER) ? ((wc->steps-1)*3+1)<<2 : (wc->steps*3 + 1);
    bits = (uint8 *)calloc(sz, sizeof(uint8));
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
    fwrite (wc->buf, 1, fsize, wl); size += fsize;
    //printf("File size = %d fist = %2X %2X\n", fsize, ((char*)wc->buf)[0], ((char*)wc->buf)[1]);

    //fwrite (wc->buf, 1, frm->img[0].c_size, wl); size += frm->img[0].c_size;

   // if(rgb) {
    	//fwrite (&frm->img[1].c_size, 4, 1, wl); size += 4;
    	//fwrite (&wc->buf[frm->img[0].c_size], 1, frm->img[1].c_size, wl); size += frm->img[1].c_size;
    	//fwrite (&frm->img[2].c_size, 4, 1, wl); size += 4;
    	//fwrite (&wc->buf[frm->img[0].c_size+frm->img[1].c_size], 1, frm->img[2].c_size, wl); size += frm->img[2].c_size;
    //}
    free(bits);
    return size;
}

uint32 frame_read(WaletConfig *wc,  Frame *f, FILE *wl){
///	\fn	uint32 frame_write(wc *wc, uint32 fr, const char *filename)
///	\brief	Read frame from file.
///	\param	wc			The pointer to the wc structure.
///	\param	fr			The frame number.
//	\param	filename	The file name.
///	\retval				The size of file.

    uint8 *bits;
    uint32 i, sz, size = 0, fsize;
    Frame *frame = &wc->frames[fr];
    uint32  rgb = (wc->color != GREY  && wc->color != BAYER);

    sz = (wc->color == BAYER) ? ((wc->steps-1)*3+1)<<2 : (wc->steps*3 + 1);
    bits = (uint8 *)calloc(sz, sizeof(uint8));

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
    if(fread (wc->buf, 1, fsize, wl)!= fsize) { printf("data read error\n"); return 0; }
    size += fsize;

    free(bits);
    //printf("File size = %d fist = %2X %2X\n", fsize, ((char*)wc->buf)[0], ((char*)wc->buf)[1]);

    frame->state = BUFFER_READ;

    return size;
 }

void frame_compress(WaletConfig *wc,  Frame *f, uint32 times, FilterBank fb)
{
	clock_t start, end;
	double time=0., tmp;
	struct timeval tv;
	uint32 size;

	gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
	frame_dwt			(wc, fr);
	gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
	tmp = (double)(end-start)/1000000.; time +=tmp;
	printf("DWT time             = %f\n", tmp);

	gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
	frame_fill_subb		(wc, fr);
	gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
	tmp = (double)(end-start)/1000000.; time +=tmp;
	printf("Fill subband time    = %f\n", tmp);

	gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
	frame_bits_alloc	(wc, fr, times);
	gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
	tmp = (double)(end-start)/1000000.; time +=tmp;
	printf("Bits allocation time = %f\n", tmp);

	gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
	frame_range_encode	(wc, fr, &size);
	gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
	tmp = (double)(end-start)/1000000.; time +=tmp;
	printf("Range coder time     = %f\n", tmp);

	printf("Frame time = %f size  = %d\n", time, size);
	//frame_write
}

void frame_decompress(WaletConfig *wc,  Frame *f, uint32 isteps, FilterBank fb)
{
	uint32 size;
	clock_t start, end;
	struct timeval tv;
	double time=0., tmp;

	gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
	frame_range_decode	(wc, fr, &size);
	gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
	tmp = (double)(end-start)/1000000.; time +=tmp;
	printf("Range decoder time    = %f\n", tmp);

	gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
	frame_idwt			(wc, fr, isteps);
	gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
	tmp = (double)(end-start)/1000000.; time +=tmp;
	printf("IDWT time             = %f\n", tmp);

	printf("Frame time = %f size  = %d\n", time, size);
}
