#include <walet.h>


/*	\brief	Check codec state
	\param	state	The current state.
	\param	check	The checking state.
*/
static uint32 check_state(uint32 state, uint32 check)
{
	if(state & check) {
		return 1;
	} else {
		printf("Can't make operation\n");
		return 0;
	}
}

static void resize_init(Pic8u **p, uint32 w, uint32 h, uint32 steps)
{
	uint32 i;
	*p = (Pic8u *)calloc(steps, sizeof(Pic8u));
	(*p)[0].w = (w>>1);
	(*p)[0].h = (h>>1);
	(*p)[0].pic = (uint8 *)calloc((*p)[0].w*(*p)[0].h, sizeof(uint8));
	for(i=1; i < steps; i++) {
		(*p)[i].w = ((*p)[i-1].w>>1);
		(*p)[i].h = ((*p)[i-1].h>>1);
		(*p)[i].pic = (uint8 *)calloc((*p)[i].w*(*p)[i].h, sizeof(uint8));
	}
}

static void resize_init_(Pic8u **p, uint32 w, uint32 h, uint32 steps)
//With w+1 space befor and after image and one more pixel before each row
{
	uint32 i;
	*p = (Pic8u *)calloc(steps, sizeof(Pic8u));
	(*p)[0].w = (w>>1)+2;
	(*p)[0].h = (h>>1)+2;
	(*p)[0].pic = (uint8 *)calloc((*p)[0].w*(*p)[0].h, sizeof(uint8));
	//(*p)[0].pic = &(*p)[0].pic[(*p)[0].w+1];
	for(i=1; i < steps; i++) {
		(*p)[i].w = (((*p)[i-1].w-2)>>1)+2;
		(*p)[i].h = (((*p)[i-1].h-2)>>1)+2;
		(*p)[i].pic = (uint8 *)calloc((*p)[i].w*(*p)[i].h, sizeof(uint8));
		//(*p)[i].pic = &(*p)[i].pic[(*p)[i].w+1];
	}
}

static void resize_init1(Pic8u **p, uint32 w, uint32 h, uint32 steps)
//With w+1 space befor and after image and one more pixel before each row
{
	uint32 i;
	*p = (Pic8u *)calloc(steps+1, sizeof(Pic8u));
	(*p)[0].w = w+2;
	(*p)[0].h = h+2;
	(*p)[0].pic = (uint8 *)calloc((*p)[0].w*(*p)[0].h, sizeof(uint8));
	//(*p)[0].pic = &(*p)[0].pic[(*p)[0].w+1];
	for(i=1; i < steps+1; i++) {
		(*p)[i].w = (((*p)[i-1].w-2)>>1)+2;
		(*p)[i].h = (((*p)[i-1].h-2)>>1)+2;
		(*p)[i].pic = (uint8 *)calloc((*p)[i].w*(*p)[i].h, sizeof(uint8));
		//(*p)[i].pic = &(*p)[i].pic[(*p)[i].w+1];
	}
}

static void resize_init32(Pic32u **p, uint32 w, uint32 h, uint32 steps)
//With w+1 space befor and after image and one more pixel before each row
{
	uint32 i;
	*p = (Pic32u *)calloc(steps, sizeof(Pic32u));
	(*p)[0].w = (w>>1)+2;
	(*p)[0].h = (h>>1)+2;
	(*p)[0].pic = (uint32 *)calloc((*p)[0].w*(*p)[0].h, sizeof(uint32));
	//(*p)[0].pic = &(*p)[0].pic[(*p)[0].w+1];
	for(i=1; i < steps; i++) {
		(*p)[i].w = (((*p)[i-1].w-2)>>1)+2;
		(*p)[i].h = (((*p)[i-1].h-2)>>1)+2;
		(*p)[i].pic = (uint32 *)calloc((*p)[i].w*(*p)[i].h, sizeof(uint32));
		//(*p)[i].pic = &(*p)[i].pic[(*p)[i].w+1];
	}
}

/*	\brief	Frame initialization.
	\param	g	The GOP structure.
	\param	fn	The frame number.
	\param	wc	The walet config structure.
*/
void frame_init(GOP *g, uint32 fn, WaletConfig *wc)
{
	uint32 i, w = wc->w, h = wc->h;
	Frame *f = &g->frames[fn];

	if (wc->icol == BAYER ){
	    f->b.w = w; f->b.h = h;
	    f->b.pic = (int16 *)calloc(f->b.w*f->b.h, sizeof(int16));
	    f->d.w = w; f->d.h = h;
	    f->d.pic = (int16 *)calloc(f->d.w*f->d.h, sizeof(int16));

	    f->Y16.w = w; f->Y16.h = h;
	    f->Y16.pic = (int16 *)calloc(f->Y16.w*f->Y16.h, sizeof(int16));
	    f->U16.w = w; f->U16.h = h;
	    f->U16.pic = (int16 *)calloc(f->U16.w*f->U16.h, sizeof(int16));
	    f->V16.w = w; f->V16.h = h;
	    f->V16.pic = (int16 *)calloc(f->V16.w*f->V16.h, sizeof(int16));

	    f->R16.w = w; f->R16.h = h;
	    f->R16.pic = (int16 *)calloc(f->R16.w*f->R16.h, sizeof(int16));
	    f->G16.w = w; f->G16.h = h;
	    f->G16.pic = (int16 *)calloc(f->G16.w*f->G16.h, sizeof(int16));
	    f->B16.w = w; f->B16.h = h;
	    f->B16.pic = (int16 *)calloc(f->B16.w*f->B16.h, sizeof(int16));

	    f->R8.w = w; f->R8.h = h;
	    f->R8.pic = (uint8 *)calloc(f->R8.w*f->R8.h, sizeof(uint8));
	    f->G8.w = w; f->G8.h = h;
	    f->G8.pic = (uint8 *)calloc(f->G8.w*f->G8.h, sizeof(uint8));
	    f->B8.w = w; f->B8.h = h;
	    f->B8.pic = (uint8 *)calloc(f->B8.w*f->B8.h, sizeof(uint8));

	    resize_init_(&f->R, w, h, wc->steps);
	    resize_init_(&f->G, w, h, wc->steps);
	    resize_init_(&f->B, w, h, wc->steps);
	    resize_init_(&f->R1, w, h, wc->steps);
	    resize_init_(&f->G1, w, h, wc->steps);
	    resize_init_(&f->B1, w, h, wc->steps);

	    resize_init1(&f->y, w, h, wc->steps);
	    resize_init_(&f->u, w, h, wc->steps);
	    resize_init_(&f->v, w, h, wc->steps);
	    resize_init1(&f->y1, w, h, wc->steps);
	    resize_init_(&f->u1, w, h, wc->steps);
	    resize_init_(&f->v1, w, h, wc->steps);

	    resize_init_(&f->dw, w, h, wc->steps);
	    resize_init_(&f->dm, w, h, wc->steps);
	    resize_init_(&f->dg, w, h, wc->steps);
	    resize_init_(&f->dc, w, h, wc->steps);
	    resize_init_(&f->di, w, h, wc->steps);
	    resize_init32(&f->rg, w, h, wc->steps);

	}

	if (wc->ccol == BAYER ){
	//Init ccol components
	    f->img = (Image *)calloc(4, sizeof(Image));
		image_init(&f->img[0], (w>>1) + (w&1), (h>>1) + (h&1), wc->bpp, wc->steps, wc->dec);
		image_init(&f->img[1], (w>>1)        , (h>>1) + (h&1), wc->bpp, wc->steps, wc->dec);
		image_init(&f->img[2], (w>>1) + (w&1), (h>>1)        , wc->bpp, wc->steps, wc->dec);
		image_init(&f->img[3], (w>>1)        , (h>>1)        , wc->bpp, wc->steps, wc->dec);
	} else if (wc->ccol == CS420){
	    f->img = (Image *)calloc(3, sizeof(Image));
		image_init(&f->img[0], w,    h,    wc->bpp, wc->steps, wc->dec);
		image_init(&f->img[1], w>>1, h>>1, wc->bpp, wc->steps, wc->dec);
		image_init(&f->img[2], w>>1, h>>1, wc->bpp, wc->steps, wc->dec);
	} else if (wc->ccol == CS444 || wc->ccol == RGB){
	    f->img = (Image *)calloc(3, sizeof(Image));
		image_init(&f->img[0], w, h, wc->bpp, wc->steps, wc->dec);
		image_init(&f->img[1], w, h, wc->bpp, wc->steps, wc->dec);
		image_init(&f->img[2], w, h, wc->bpp, wc->steps, wc->dec);
	} else if (wc->ccol == GREY){
	    f->img = (Image *)calloc(1, sizeof(Image));
		image_init(&f->img[0], w, h, wc->bpp, wc->steps, wc->dec);
	} else if (wc->ccol == RGB){
	    f->img = (Image *)calloc(3, sizeof(Image));
		image_init(&f->img[0], w, h, wc->bpp, wc->steps, wc->dec);
		image_init(&f->img[1], w, h, wc->bpp, wc->steps, wc->dec);
		image_init(&f->img[2], w, h, wc->bpp, wc->steps, wc->dec);
	} else {
		printf("Wrong color space.\n");
	}

	//For white balancing
	f->hist = (uint32 *)calloc((1<<wc->bpp)*4, sizeof(uint32));
	f->look = (uint16 *)calloc((1<<wc->bpp)*3, sizeof(uint16));

	//Init new sermentation
	f->ln = (Line *)calloc((w>>1)*(h>>1), sizeof(Line));
	f->lp = (Line **)calloc(w*h, sizeof(Line*));
	f->vx = (Vertex *)calloc(w*h, sizeof(Vertex));
	f->vp = (Vertex **)calloc(w*h, sizeof(Vertex*));

	//Old init
	//f->size = w*h;

	//f->pixs = (Pixel *)calloc(w*h, sizeof(Pixel));
	//f->edges = (Edge *)calloc((w>>2)*(h>>2), sizeof(Edge));

	//f->rgb.w  = w;
	//f->rgb.h = h;
	//f->rgb.pic = (uint8 *)calloc(f->rgb.w*f->rgb.h*3, sizeof(uint8));
	//f->Y.w  = w;
	//f->Y.h = h;
	//f->Y.pic = (uint8 *)calloc(f->Y.w*f->Y.h*3, sizeof(uint8));
	//f->grad.w  = w;
	//f->grad.h = h;
	//f->grad.pic = (uint8 *)calloc(f->grad.w*f->grad.h*3, sizeof(uint8));


	//f->line.w  = w;
	//f->line.h = h;
	//f->line.pic = (uint8 *)calloc(f->line.w*f->line.h, sizeof(uint8));
	//f->edge.w  = w;
	//f->edge.h = h;
	//f->edge.pic = (uint8 *)calloc(f->edge.w*f->edge.h, sizeof(uint8));
	//f->vec.w  = w;
	//f->vec.h = h;
	//f->vec.pic = (uint8 *)calloc(f->vec.w*f->vec.h, sizeof(uint8));
	f->state = 0;
}

/*	\brief	Copy and transform input frame
	\param	g	The GOP structure.
	\param	fn	The frame number.
	\param	wc	The walet config structure.
	\param	y	The pointer to Bayer, gray, red or Y  image data.
	\param	u	The pointer to green or U  image data.
	\param	v	The pointer to blue or V  image data.
*/
void frame_input(GOP *g, uint32 fn, WaletConfig *wc, uint8 *y, uint8 *u, uint8 *v)
{
	//uint32 i, size = wc->w*wc->h, size3 = size*3, shift = 1<<(wc->bpp-1);
	Frame *f = &g->frames[fn];

	if(wc->icol == BAYER) {
		utils_image_copy(y, f->b.pic, f->b.w, f->b.h, wc->bpp);
		if(wc->bpp == 8){
			if(wc->ccol == BAYER){
				dwt_53_2d_one(f->b.pic, f->img[0].p, f->img[1].p, f->img[2].p, f->img[3].p, (int16*)g->buf, f->b.w, f->b.h);
			} else if(wc->ccol == CS444) {
				utils_bayer_to_YUV444(f->b.pic, f->img[0].p,f->img[1].p, f->img[2].p, (int16*)g->buf, f->b.w, f->b.h, wc->bg);
			} else if(wc->ccol == CS420) {
				utils_bayer_to_YUV420(f->b.pic, f->y[0].pic, f->u[0].pic, f->v[0].pic, (int16*)g->buf, f->b.w, f->b.h, wc->bg);
				//utils_bayer_to_YUV420(f->b.pic, f->img[0].d.pic,f->img[1].d.pic, f->img[2].d.pic, (int16*)g->buf, f->b.w, f->b.h, wc->bg);
				//prediction_encoder(f->img[0].p, f->img[0].p, (int16*)g->buf, f->img[0].w, f->img[0].h);
				//prediction_encoder(f->img[1].p, f->img[1].p, (int16*)g->buf, f->img[1].w, f->img[1].h);
				//prediction_encoder(f->img[2].p, f->img[2].p, (int16*)g->buf, f->img[2].w, f->img[2].h);
			} else if(wc->ccol == RGB){
				utils_bayer_to_RGB(f->b.pic, f->img[0].p, f->img[1].p, f->img[2].p, (int16*)g->buf, f->b.w, f->b.h, wc->bg);
			} else if(wc->ccol == RGBY){
				utils_bayer_to_RGB_fast_(f->b.pic, f->R[0].pic, f->G[0].pic, f->B[0].pic, f->b.w, f->b.h, wc->bg, 128);
				utils_bayer_to_Y_fast_(f->b.pic, f->dw[0].pic, f->b.w, f->b.h, 128);
			}
		} else {

		}
	} else if(wc->icol == CS444 || wc->icol == CS420){
		utils_image_copy(y, f->img[0].p,  f->img[0].w, f->img[0].h, wc->bpp);
		utils_image_copy(u, f->img[1].p,  f->img[1].w, f->img[1].h, wc->bpp);
		utils_image_copy(v, f->img[2].p,  f->img[2].w, f->img[2].h, wc->bpp);
	} else if(wc->icol == GREY) {
		utils_image_copy(y, f->img[0].p,  f->img[0].w, f->img[0].h, wc->bpp);
	} else if(wc->icol == RGB) {
		if(wc->ccol == RGB){
			utils_RGB24_to_RGB(y, f->img[0].p, f->img[1].p, f->img[2].p, f->img[0].w, f->img[0].h, wc->bpp, 1);
		} else if(wc->ccol == CS444) {
			utils_RGB24_to_YUV444(y, f->img[0].p, f->img[1].p, f->img[2].p, f->img[0].w, f->img[0].h, wc->bpp, 1);
		} else if(wc->ccol == CS420) {
			//utils_RGB24_to_YUV420(y, f->img[0].p, f->img[1].p, f->img[2].p, f->img[0].w, f->img[0].h, wc->bpp, 1);
			utils_RGB24_to_YUV420(y, f->img[0].d.pic, f->img[1].d.pic, f->img[2].d.pic, f->img[0].w, f->img[0].h, wc->bpp, 1);
			//prediction_encoder(f->img[0].d.pic, f->img[0].p, f->img[0].w, f->img[0].h);
			//prediction_encoder(f->img[1].d.pic, f->img[1].p, f->img[1].w, f->img[1].h);
			//prediction_encoder(f->img[2].d.pic, f->img[2].p, f->img[2].w, f->img[2].h);
		}
	}
	f->state = FRAME_COPY;
	printf("Finish frame input \n");
}

/*	\brief	Transform the frame to RGB24 color space.
	\param	g	The GOP structure.
	\param	fn	The frame number.
	\param	wc	The walet config structure.
	\param	rgb	The pointer to RGB24 image.
	\param	isteps	The invert DWT transform steps.
	\param	w	The output image width.
	\param  h	The output image height.
*/
void frame_ouput(GOP *g, uint32 fn, WaletConfig *wc, uint8 *rgb, uint32 isteps, uint32 *w, uint32 *h)
{
	uint32 i;
	Frame *f = &g->frames[fn];

	if(wc->ccol == BAYER) {
		if(isteps == wc->steps) {
			f->d.w = f->img[0].d.w + f->img[1].d.w;
			f->d.h = f->img[0].d.h + f->img[2].d.h;
			idwt_53_2d_one(f->d.pic, f->img[0].d.pic, f->img[1].d.pic, f->img[2].d.pic, f->img[3].d.pic, (int16*)g->buf, f->d.w, f->d.h);
			utils_RGB_to_RGB24(rgb, f->img[0].p, f->img[1].p, f->img[2].p, f->img[0].w, f->img[0].h, wc->bpp);
			*w = f->d.w; *h = f->d.h;
		} else {
			i = wc->steps - isteps;
			f->d.w = f->img[0].l[i-1].s[0].w + f->img[1].l[i-1].s[0].w;
			f->d.h = f->img[0].l[i-1].s[0].h + f->img[2].l[i-1].s[0].h;
			idwt_53_2d_one(f->d.pic, f->img[0].l[i-1].s[0].pic, f->img[1].l[i-1].s[0].pic, f->img[2].l[i-1].s[0].pic, f->img[3].l[i-1].s[0].pic,
					(int16*)g->buf, f->d.w, f->d.h);
			*w = f->d.w; *h = f->d.h;
		}
		utils_bayer_to_RGB24(f->d.pic, rgb, (int16*)g->buf, f->d.w, f->d.h, wc->bg, wc->bpp);
	} else if(wc->ccol == CS444){
		if(isteps == wc->steps) {
			*w = f->img[0].w; *h = f->img[0].h;
			utils_YUV444_to_RGB24(rgb, f->img[0].d.pic, f->img[1].d.pic, f->img[2].d.pic, *w, *h, wc->bpp);
		} else {
			i = wc->steps - isteps;
			*w = f->img[0].l[i-1].s[0].w;
			*h = f->img[0].l[i-1].s[0].h;
			utils_YUV444_to_RGB24(rgb, f->img[0].l[i-1].s[0].pic, f->img[1].l[i-1].s[0].pic, f->img[2].l[i-1].s[0].pic, *w, *h, wc->bpp);
		}
	} else if(wc->ccol == CS420){
		if(isteps == wc->steps) {
			*w = f->img[0].w; *h = f->img[0].h;
			utils_YUV420_to_RGB24(rgb, f->img[0].d.pic, f->img[1].d.pic, f->img[2].d.pic, *w, *h, wc->bpp);
		} else {
			i = wc->steps - isteps;
			*w = f->img[0].l[i-1].s[0].w;
			*h = f->img[0].l[i-1].s[0].h;
			utils_YUV420_to_RGB24(rgb, f->img[0].l[i-1].s[0].pic, f->img[1].l[i-1].s[0].pic, f->img[2].l[i-1].s[0].pic, *w, *h, wc->bpp);
		}
	} else if(wc->ccol == GREY) {
		if(isteps == wc->steps) {
			*w = f->img[0].w; *h = f->img[0].h;
			utils_grey_draw(f->img[0].d.pic, rgb, *w, *h, 128);
		} else {
			i = wc->steps - isteps;
			*w = f->img[0].l[i-1].s[0].w;
			*h = f->img[0].l[i-1].s[0].h;
			utils_grey_draw(f->img[0].l[i-1].s[0].pic, rgb, *w, *h, 128);
		}
	} else if(wc->ccol == RGB) {
		if(isteps == wc->steps) {
			*w = f->img[0].w; *h = f->img[0].h;
			utils_RGB_to_RGB24(rgb, f->img[0].d.pic, f->img[1].d.pic, f->img[2].d.pic, *w, *h, wc->bpp);
		} else {
			i = wc->steps - isteps;
			*w = f->img[0].l[i-1].s[0].w;
			*h = f->img[0].l[i-1].s[0].h;
			utils_RGB_to_RGB24(rgb, f->img[0].l[i-1].s[0].pic, f->img[1].l[i-1].s[0].pic, f->img[2].l[i-1].s[0].pic, *w, *h, wc->bpp);
		}
	}
	f->state = FRAME_COPY;
	printf("Finish frame output \n");
}

/**	\brief	The Frame discrete wavelet transform.
	\param	g	The GOP structure.
	\param	fn	The frame number.
	\param	wc	The walet config structure.
	\retval		1 - if all OK, 0 - if not
*/
uint32 frame_transform(GOP *g, uint32 fn, WaletConfig *wc)
{
	uint32 i;
	Frame *f = &g->frames[fn];
	//DWT taransform
	//if(check_state(f->state, FRAME_COPY | IDWT)){
	if(wc->dec == WAVELET){
		if (wc->ccol == GREY) image_dwt(&f->img[0], (int16*)g->buf, wc->fb, wc->steps);
		else if(wc->ccol == BAYER) for(i=0; i < 4; i++)  image_dwt(&f->img[i], (int16*)g->buf, wc->fb, wc->steps);
		else  for(i=0; i < 3; i++) image_dwt(&f->img[i], (int16*)g->buf, wc->fb, wc->steps);
	} else if(wc->dec == RESIZE){
		if (wc->ccol == GREY) image_resize_down_2x(&f->img[0], (int16*)g->buf, wc->steps);
		else if(wc->ccol == BAYER) for(i=0; i < 4; i++)  image_resize_down_2x(&f->img[i], (int16*)g->buf, wc->steps);
		else  for(i=0; i < 3; i++) image_resize_down_2x(&f->img[i], (int16*)g->buf, wc->steps);
	} else if(wc->dec == VECTORIZE){
		if(wc->ccol == RGBY){
			for(i=1; i < wc->steps; i++) {
				resize_down_2x(f->R[i-1].pic, f->R[i].pic, g->buf, f->R[i-1].w, f->R[i-1].h);
			}
			for(i=1; i < wc->steps; i++) {
				resize_down_2x(f->G[i-1].pic, f->G[i].pic, g->buf, f->G[i-1].w, f->G[i-1].h);
			}
			for(i=1; i < wc->steps; i++) {
				resize_down_2x(f->B[i-1].pic, f->B[i].pic, g->buf, f->B[i-1].w, f->B[i-1].h);
			}
			for(i=1; i < wc->steps; i++) {
				resize_down_2x_(f->dw[i-1].pic, f->dw[i].pic, g->buf, f->dw[i-1].w, f->dw[i-1].h);
			}
		}
		else if(wc->ccol == CS420){
			//for(i=1; i < wc->steps+1; i++) {
			//	resize_down_2x_(f->y[i-1].pic, f->y[i].pic, g->buf, f->y[i-1].w, f->y[i-1].h);
			//}
			for(i=1; i < wc->steps; i++) {
				resize_down_2x_(f->u[i-1].pic, f->u[i].pic, g->buf, f->u[i-1].w, f->u[i-1].h);
			}
			for(i=1; i < wc->steps; i++) {
				resize_down_2x_(f->v[i-1].pic, f->v[i].pic, g->buf, f->v[i-1].w, f->v[i-1].h);
			}
			resize_down_2x_(f->y[0].pic, f->dw[0].pic, g->buf, f->y[0].w, f->y[0].h);
			for(i=1; i < wc->steps; i++) {
				resize_down_2x_(f->dw[i-1].pic, f->dw[i].pic, g->buf, f->dw[i-1].w, f->dw[i-1].h);
			}

		}
	}
	f->state = DWT;
	//image_grad(&frame->img[0], BAYER, wc->steps, 2);
	return 1;
	//} else return 0;
}

/**	\brief	The Frame invert discrete wavelet transform.
	\param	g		The GOP structure.
	\param	fn		The frame number.
	\param	wc		The walet config structure.
	\param	isteps	The steps of IDWT (should be lees or equal DWT steps).
	\retval			1 - if all OK, 0 - if not
*/
uint32 frame_idwt(GOP *g, uint32 fn, WaletConfig *wc, uint32 isteps)
{
	uint32 i;
	Frame *f = &g->frames[fn];

	//if(check_state(f->state, DWT | RANGE_DECODER)){
		if (wc->ccol == GREY) image_idwt(&f->img[0], (int16*)g->buf, wc->fb, wc->steps, isteps);
		else if(wc->ccol == BAYER ) {
			for(i=0; i < 4; i++) image_idwt(&f->img[i], (int16*)g->buf, wc->fb, wc->steps, isteps);
			//ccol transform
			/*
			if(isteps == wc->steps) {
			f->d.w = f->img[0].d.w + f->img[1].d.w;
			f->d.h = f->img[0].d.h + f->img[2].d.h;
			idwt_53_2d_one(f->d.pic, f->img[0].d.pic, f->img[1].d.pic, f->img[2].d.pic, f->img[3].d.pic,
					(int16*)g->buf, f->d.w, f->d.h);
			} else {
				i = wc->steps - isteps;
				f->d.w = f->img[0].l[i].s[0].w + f->img[1].l[i].s[0].w;
				f->d.h = f->img[0].l[i].s[0].h + f->img[2].l[i].s[0].h;
				idwt_53_2d_one(f->d.pic, f->img[0].l[i].s[0].pic, f->img[1].l[i].s[0].pic,f->img[2].l[i].s[0].pic, f->img[3].l[i].s[0].pic,
						(int16*)g->buf, f->d.w, f->d.h);
			}*/
		} else for(i=0; i < 3; i++) image_idwt(&f->img[i], (int16*)g->buf, wc->fb, wc->steps, isteps);

		f->state = IDWT;
		return 1;
	//} else return 0;
}

/**	\brief	Fill distribution probability array for each subbands.
	\param	g		The GOP structure.
	\param	fn		The frame number.
	\param	wc		The walet config structure.
	\retval			1 - if all OK, 0 - if not OK
*/
uint32 frame_fill_subb(GOP *g, uint32 fn, WaletConfig *wc)
{
	uint32 i;
	Frame *f = &g->frames[fn];
	f->qst = 0;

	//if(check_state(f->state, DWT)){
		if(wc->ccol == GREY) {
			image_fill_subb(&f->img[0], wc->steps);
			f->qst += f->img[0].qst;
		}
		else if(wc->ccol == BAYER){
			for(i=0; i < 4; i++)  {
			image_fill_subb(&f->img[i], wc->steps);
			f->qst += f->img[i].qst;
			}
		}
		else
			for(i=0; i < 3; i++)  {
			image_fill_subb(&f->img[i], wc->steps);
			f->qst += f->img[i].qst;
		}
		f->state |= FILL_SUBBAND;
		return 1;
	//} else return 0;
}

/**	\brief	Fill distribution probability array for each subbands.
	\param	g		The GOP structure.
	\param	fn		The frame number.
	\param	wc		The walet config structure.
	\param	times	The compression ratio in times.
	\retval			1 - if all OK, 0 - if not OK
*/
uint32 frame_bits_alloc(GOP *g, uint32 fn, WaletConfig *wc, uint32 times)
{
	Frame *f = &g->frames[fn];

	// Energy of one level DWT transform
	// |--------|--------|
	// |   LL   |   HL   |
	// |   4    |   2    |
	// |  bl[0] |  bl[1] |
	// |--------|--------|
	// |   LH   |   HH   |
	// |   2    |   1    |
	// |  bl[2] |  bl[3] |
	// |--------|--------|
	//The order of bit allocation for subband
	//uint32 qo[9] = {0, 0, 0, 0, 1, 2, 1, 2, 3}, step = 8,  qs = f->qst, qs2 = qs>>1;
	// Energy of one level DWT transform
	// |--------|--------|
	// |   LL   |   HL   |
	// |   4    |   1    |
	// |  bl[0] |  bl[1] |
	// |--------|--------|
	// |   LH   |   HH   |
	// |   1    |   0    |
	// |  bl[2] |  bl[3] |
	// |--------|--------|
	//The order of bit allocation for subband
	uint32 qo[9] = {0, 0, 0, 0, 1, 2}, step = 6, qs = f->qst - f->img[3].qst, qs2 = qs>>1;

	uint32 i, j, k, size, qs1, df, s, df1;
	uint32 bl[4];
	if(times == 1) return 0;
	size = (wc->w*wc->h*wc->bpp)/times;
	//df1 = size;

	//for(j=0; j < 4; j++) printf("bl[%d] = %d ", j, f->img[j].qst); printf("\n");


	if(check_state(f->state, FILL_SUBBAND)){
		/*if (wc->ccol == BAYER){
			f->state |= BITS_ALLOCATION;

			//s += image_size_test(&f->img[3], wc->steps, 0, wc->steps);
			for(i=0; i < wc->steps; i++) for(j=1; j < 4; j++) f->img[3].l[i].s[j].q_bits = 0;
			for(i=0; i < wc->steps-2; i++) for(j=1; j < 4; j++) f->img[1].l[i].s[j].q_bits = 0;
			for(i=0; i < wc->steps-2; i++) for(j=1; j < 4; j++) f->img[2].l[i].s[j].q_bits = 0;

			f->img[1].l[1].s[1].q_bits = 5;
			f->img[1].l[1].s[2].q_bits = 5;
			f->img[1].l[1].s[3].q_bits = 0;

			f->img[2].l[1].s[1].q_bits = 5;
			f->img[2].l[1].s[2].q_bits = 5;
			f->img[2].l[1].s[3].q_bits = 0;

			f->img[0].l[0].s[1].q_bits = 6;
			f->img[0].l[0].s[2].q_bits = 6;
			f->img[0].l[0].s[3].q_bits = 0;
			*/
			//f->img[0].l[0].s[3].q_bits = f->img[0].l[0].s[3].q_bits>>1;
			//f->img[1].l[0].s[1].q_bits = f->img[1].l[0].s[1].q_bits>>1;
			//f->img[1].l[0].s[2].q_bits = f->img[1].l[0].s[2].q_bits>>1;
			/*
			f->img[1].l[1].s[1].q_bits = 6;
			f->img[1].l[1].s[2].q_bits = 6;
			f->img[1].l[1].s[3].q_bits = 5;

			f->img[1].l[0].s[1].q_bits = 3;
			f->img[1].l[0].s[2].q_bits = 3;
			f->img[1].l[0].s[3].q_bits = 2;
			//f->img[2].l[0].s[1].q_bits = f->img[2].l[0].s[1].q_bits>>1;
			//f->img[2].l[0].s[2].q_bits = f->img[2].l[0].s[2].q_bits>>1;

			f->img[2].l[1].s[1].q_bits = 6;
			f->img[2].l[1].s[2].q_bits = 6;
			f->img[2].l[1].s[3].q_bits = 5;

			f->img[2].l[0].s[1].q_bits = 3;
			f->img[2].l[0].s[2].q_bits = 3;
			f->img[2].l[0].s[3].q_bits = 2;
			//s += image_size_test(&f->img[2], wc->steps, 0, wc->steps-3);
			//s += image_size_test(&f->img[3], wc->steps, 0, wc->steps);
			*/


			//return 1;

			//Old algorithm bits allocation
			/*
			for(k=2;;k++){
				//printf("qs = %d\n", qs);
				for(i=0; i < 4; i++) bl[i] = 0;
				//Bits allocation between 4 ccol image
				for(i=0, j=0; i < qs2; i++) {
					if(bl[qo[j]] < f->img[qo[j]].qst) bl[qo[j]]++;
					else i--;
					j = (j == step) ? 0 : j + 1;
				}
				for(j=0; j < 4; j++) printf("bl[%d] = %d ", j, bl[j]); printf("\n");

				s = 0;
				for(j=0; j < 4; j++) s += image_size(&f->img[j], wc->steps, bl[j]);
					//printf("img = %d\n", j);

				//printf("times = %d qst = %d size = %d qs = %d s = %d\n", times, f->qst, size, qs, s);
				//printf("Frame size = %d now = %d\n", size, s);

				if(!(f->qst>>k)) {
				//TODO: Fine tuning of the bit allocation
					break;
				}
				else qs2 = (s < size) ? qs2 + (qs>>k) : qs2 - (qs>>k);
			}
			*/
			//New algorithm of bits allocatiom.
			//At first we should decide how many bits for each LL, HL, LH, HH image
			// |--------|--------|
			// |   LL   |   HL   |
			// |   4    |   1    |
			// |  bl[0] |  bl[1] |
			// |--------|--------|
			// |   LH   |   HH   |
			// |   1    |   0    |
			// |  bl[2] |  bl[3] |
			// |--------|--------|
			/*
			bl[0] = size*4/9;
			bl[1] = size*5/18;
			bl[2] = size*5/18;
			bl[3] = 0;


			bl[0] = size*4/9;
			bl[1] = size*2/9;
			bl[2] = size*2/9;
			bl[3] = size/9;


			bl[0] = size>>1;
			bl[1] = size>>2;
			bl[2] = size>>2;
			bl[3] = 0;

			s = 0;
			for(j=0; j < 4; j++) s += image_size(&f->img[j], wc->steps, bl[j]);

				//printf("img = %d\n", j);
		}*/
		if(wc->ccol == GREY) {
			s = image_size(&f->img[0], wc->steps, size);
		} else if(wc->ccol == BAYER){
			s  = image_size(&f->img[0], wc->steps, size*4/6);
			s += image_size(&f->img[1], wc->steps, size/6);
			s += image_size(&f->img[2], wc->steps, size/6);
			s += image_size(&f->img[3], wc->steps, 0);
		} else if (wc->ccol == CS444 || wc->ccol == CS420){
			s  = image_size(&f->img[0], wc->steps, size*22/24);
			s += image_size(&f->img[1], wc->steps, size/24);
			s += image_size(&f->img[2], wc->steps, size/24);
		} else if(wc->ccol == RGB){
			printf("img = %p steps = %d size = %d\n", &f->img[0], wc->steps, size/3);
			s  = image_size(&f->img[0], wc->steps, size/3);
			s += image_size(&f->img[1], wc->steps, size/3);
			s += image_size(&f->img[2], wc->steps, size/3);
		}
		printf("Frame size = %d bits  %d bites qs = %d\n", s, s/8, qs);
		f->state |= BITS_ALLOCATION;
		return 1;
	} else return 0;
}

/**	\brief	Frame quantization.
	\param	g		The GOP structure.
	\param	fn		The frame number.
	\param	wc		The walet config structure.
	\retval			1 - if all OK, 0 - if not OK
*/
uint32 frame_quantization(GOP *g, uint32 fn, WaletConfig *wc)
{
	uint32 i;
	Frame *f = &g->frames[fn];

	//if(check_state(f->state, BITS_ALLOCATION)){
		if(wc->ccol == GREY) image_quantization(&f->img[0], wc->steps, g->buf);
		else if(wc->ccol == BAYER)
			for(i=0; i < 4; i++)  image_quantization(&f->img[i], wc->steps,  g->buf);
		else
			for(i=0; i < 3; i++)  image_quantization(&f->img[i], wc->steps,  g->buf);

		f->state |= QUANTIZATION;
		return 1;
	//} else return 0;
}

/**	\brief	Frame range encoder.
	\param	g		The GOP structure.
	\param	fn		The frame number.
	\param	wc		The walet config structure.
	\param	size	The size of encoded frame in bytes.
	\retval			1 - if all OK, 0 - if not OK
*/
uint32 frame_range_encode(GOP *g, uint32 fn, WaletConfig *wc,  uint32 *size)
{
	uint32 i;
	Frame *f = &g->frames[fn];
	*size = 0;

	if(check_state(f->state, FILL_SUBBAND)){
		*size = 0;
		if(wc->ccol == GREY) {
			f->img[0].isz = image_range_encode(&f->img[0], wc->steps, wc->bpp, g->buf, g->ibuf, wc->rt);
			*size = f->img[0].isz;
		}
		else if(wc->ccol == BAYER)
			for(i=0; i < 4; i++)  {
				printf("image %d\n", i);
				f->img[i].isz = image_range_encode(&f->img[i], wc->steps, wc->bpp, &g->buf[*size], g->ibuf, wc->rt);
				*size += f->img[i].isz;
			}
		else
			for(i=0; i < 3; i++)  {
				f->img[i].isz = image_range_encode(&f->img[i], wc->steps, wc->bpp, &g->buf[*size], g->ibuf, wc->rt);
				*size += f->img[i].isz;
			}
		f->state |= RANGE_ENCODER;
		return 1;
	} else return 0;
}

/**	\brief	Frame range decoder.
	\param	g		The GOP structure.
	\param	fn		The frame number.
	\param	wc		The walet config structure.
	\param	size	The size of encoded frame in bytes.
	\retval			1 - if all OK, 0 - if not OK
*/
uint32 frame_range_decode(GOP *g, uint32 fn, WaletConfig *wc, uint32 *size)
{
	uint32 i;
	Frame *f = &g->frames[fn];
	*size = 0;

	if(check_state(f->state, BUFFER_READ | RANGE_ENCODER)){
		*size = 0;
		if(wc->ccol == GREY) *size += image_range_decode(&f->img[0], wc->steps, wc->bpp, g->buf, g->ibuf, wc->rt);
		else if(wc->ccol == BAYER)
			for(i=0; i < 4; i++)  *size += image_range_decode(&f->img[i], wc->steps, wc->bpp, &g->buf[*size], g->ibuf, wc->rt);
		else
			for(i=0; i < 3; i++)  *size += image_range_decode(&f->img[i], wc->steps, wc->bpp, &g->buf[*size], g->ibuf, wc->rt);
		f->state |= RANGE_DECODER;
		return 1;
	} else return 0;
}

void frame_predict_subband(GOP *g, uint32 fn, WaletConfig *wc)
{
	uint32 i;
	Frame *f = &g->frames[fn];

	if(wc->ccol == GREY) image_predict_subband(&f->img[0], (int16*)g->buf, wc->steps);
	else if(wc->ccol == BAYER)
		for(i=0; i < 3; i++)  image_predict_subband(&f->img[i], (int16*)g->buf, wc->steps);
	else
		for(i=0; i < 3; i++)  image_predict_subband(&f->img[i], (int16*)g->buf, wc->steps);
}

/**	\brief	Median filter.
	\param	g		The GOP structure.
	\param	fn		The frame number.
	\param	wc		The walet config structure.
	\retval			1 - if all OK, 0 - if not OK
*/
uint32 frame_median_filter(GOP *g, uint32 fn, WaletConfig *wc)
{
	uint32 i;
	clock_t start, end;
	Frame *f = &g->frames[fn];

	if(wc == NULL ) return 0;

	if(check_state(f->state, IDWT | FRAME_COPY)){

		if(wc->ccol == GREY) image_median_filter(&f->img[0], g->buf);
		else if(wc->ccol == BAYER)
			filter_median_bayer(f->b.pic, (int16*)g->buf, wc->w, wc->h);
		else
			for(i=0; i < 3; i++)  image_median_filter(&f->img[i], g->buf);

		f->state |= MEDIAN_FILTER;

		return 1;
	} else return 0;
}

/**	\brief	Fill ccol histogram for white balancing (for bayer ccol space only).
	\param	g		The GOP structure.
	\param	fn		The frame number.
	\param	wc		The walet config structure.
	\retval			1 - if all OK, 0 - if not OK
*/
uint32 frame_fill_hist(GOP *g, uint32 fn, WaletConfig *wc)
{
	uint32 i, size = wc->w*wc->h, sz = 1<<wc->bpp, sum;
	uint32	tmp = size;
	Frame *f = &g->frames[fn];

	if(check_state(f->state, FRAME_COPY)){
		if(wc->ccol == BAYER) {
			fill_bayer_hist(f->b.pic, f->hist, &f->hist[sz], &f->hist[sz<<1], f->b.w, f->b.h, wc->bg, wc->bpp);
			/*
			sum = 0; for(i=0; i<sz; i++) sum +=f->hist[i]; tmp -= sum;
			printf("size = %d r = %d ", size, sum);
			sum = 0; for(i=0; i<sz; i++) sum +=f->hist[sz+i]; tmp -= sum;
			printf("g = %d ", sum);
			sum = 0; for(i=0; i<sz; i++) sum +=f->hist[(sz<<1)+i]; tmp -= sum;
			printf("b = %d  diff = %d\n", sum, tmp);*/
		}
		f->state |= FILL_HISTOGRAM;
		return 1;
	} else return 0;
}

void frame_range(GOP *g, uint32 fn, WaletConfig *wc,  uint32 *size)
{
	uint32 i;
	Frame *f = &g->frames[fn];
	*size = 0;

	*size = 0;
	if(wc->ccol == GREY) {
		f->img[0].isz = image_range(&f->img[0], wc->steps, wc->bpp, g->buf, g->ibuf, wc->rt);
		*size = f->img[0].isz;
	}
	else if(wc->ccol == BAYER)
		for(i=0; i < 4; i++)  {
			printf("image %d\n", i);
			f->img[i].isz = image_range(&f->img[i], wc->steps, wc->bpp, &g->buf[*size], g->ibuf, wc->rt);
			*size += f->img[i].isz;
		}
	else
		for(i=0; i < 3; i++)  {
			f->img[i].isz = image_range(&f->img[i], wc->steps, wc->bpp, &g->buf[*size], g->ibuf, wc->rt);
			*size += f->img[i].isz;
		}
}


/**	\brief	Make white balance and gamma correction of the frame (for bayer ccol space only).
	\param	g		The GOP structure.
	\param	fn		The frame number.
	\param	wc		The walet config structure.
	\param	bits	The bits per pixel for output frame.
	\param	gamma		The type of gamma correction.
	\retval			1 - if all OK, 0 - if not OK
*/
uint32 frame_white_balance(GOP *g, uint32 fn, WaletConfig *wc, uint32 bits, Gamma gamma)
{
	Frame *f = &g->frames[fn];
	if(check_state(f->state, FILL_HISTOGRAM)){
		if(wc->ccol == BAYER){
			filters_white_balance(f->b.pic, f->b.pic, f->hist, f->look, f->b.w, f->b.h, wc->bg, wc->bpp, bits, gamma);
		}
		f->state |= WHITE_BALANCE;
		return 1;
	} else return 0;
}

/**	\brief	Frame segmetation.
	\param	g		The GOP structure.
	\param	fn		The frame number.
	\param	wc		The walet config structure.
	\retval			1 - if all OK, 0 - if not OK
*/
uint32 frame_segmetation(GOP *g, uint32 fn, WaletConfig *wc)
{
	Frame *f = &g->frames[fn];
	uint32 i, j, npix, vxc, rgc;
	if(wc->ccol == CS420){
		resize_down_2x_(f->y[0].pic, f->y[1].pic, g->buf, f->y[0].w, f->y[0].h);
		filter_median(f->y[1].pic, f->dm[0].pic, f->y[1].w, f->y[1].h);
		seg_grad(f->dm[0].pic, f->dg[0].pic, f->dc[0].pic, f->di[i].pic, f->y[1].w, f->y[1].h, 3);
		seg_find_intersect4(f->dg[0].pic, f->dc[0].pic, f->di[0].pic, f->y[1].w, f->y[1].h);
		vxc = seg_vertex3(f->dc[0].pic, f->di[0].pic, f->vx, f->vp, g->buf, f->y[1].w, f->y[1].h);
		//seg_remove_virtex(f->vp, vxc, f->y[1].w, f->y[1].h);
		//seg_remove_loops(f->y1[1].pic, f->vp, &f->vp[wc->w*wc->h>>2], f->ln, f->lp, vxc, f->y[1].w, f->y[1].h);
		memset(f->di[0].pic, 0, f->dg[0].w*f->dg[0].h);
		//seg_vertex_draw3(f->di[0].pic, f->vp, (uint32*)g->buf, vxc, f->y[1].w, f->y[1].h, f->y[1].w, f->y[1].h);
		//seg_vertex_draw3(f->y1[0].pic, f->vp, (uint32*)g->buf, vxc, f->y[0].w, f->y[0].h, f->y[1].w, f->y[1].h);
		//seg_vertex_draw3(f->y1[1].pic, f->vp, (uint32*)g->buf, vxc, f->y[1].w, f->y[1].h, f->y[1].w, f->y[1].h);
		seg_remove_virtex(f->vp, vxc, f->y[1].w, f->y[1].h);
		seg_remove_loops(f->y1[1].pic, f->vp, &f->vp[wc->w*wc->h>>2], f->ln, vxc, f->y[1].w, f->y[1].h);
		seg_vertex_draw3(f->y1[1].pic, f->vp, (uint32*)g->buf, vxc, f->y[1].w, f->y[1].h, f->y[1].w, f->y[1].h);
		//for(i=0; i <  f->y[1].w*f->y[1].h; i++) f->y1[1].pic[i] = 0;
		/*
		seg_vertex_draw3(f->di[0].pic, f->vp, (uint32*)g->buf, vxc, f->y[1].w, f->y[1].h, f->y[1].w, f->y[1].h);

		seg_get_or_fill_color(f->y[1].pic, f->di[0].pic, f->dm[0].pic, (uint32*)&g->buf[f->dg[0].w*f->dg[0].h>>1], f->vp, &f->vp[wc->w*wc->h>>2],
				vxc, f->y[1].w, f->y[1].h, f->y[1].w, f->y[1].h, 1);
				*/
		/*
		seg_vertex_draw3(f->y1[0].pic, f->vp, (uint32*)g->buf, vxc, f->y[0].w, f->y[0].h, f->y[1].w, f->y[1].h);

		seg_get_or_fill_color(NULL, f->y1[0].pic, f->dm[0].pic, (uint32*)&g->buf[f->dg[0].w*f->dg[0].h>>1], f->vp, &f->vp[wc->w*wc->h>>2],
						vxc, f->y[0].w, f->y[0].h, f->y[1].w, f->y[1].h, 0);
		seg_draw_line_one(f->y1[0].pic, f->y[0].w, f->y[0].h);
		*/
		/*
		seg_vertex_draw3(f->y1[1].pic, f->vp, (uint32*)g->buf, vxc, f->y[1].w, f->y[1].h, f->y[1].w, f->y[1].h);
		seg_get_or_fill_color(NULL, f->y1[1].pic, f->dm[0].pic, (uint32*)&g->buf[f->dg[0].w*f->dg[0].h>>1], f->vp, &f->vp[wc->w*wc->h>>2],
						vxc, f->y[1].w, f->y[1].h, f->y[1].w, f->y[1].h, 0);
		seg_draw_line_one(f->y1[1].pic, f->y[1].w, f->y[1].h);
		*/
	}

	//image_gradient(&f->img[0], g->buf, wc->steps, 3);
	//for(i=1; i < wc->steps; i++) {
	for(i=3; i < 4; i++) {
		//filter_median(f->R[i].pic, f->R1[i].pic, f->dw[i].w, f->dw[i].h);
		//filter_median(f->G[i].pic, f->G1[i].pic, f->dw[i].w, f->dw[i].h);
		//filter_median(f->B[i].pic, f->B1[i].pic, f->dw[i].w, f->dw[i].h);
		//seg_grad_RGB(f->R1[i].pic, f->G1[i].pic, f->B1[i].pic, f->dg[i].pic, f->dw[i].w, f->dw[i].h, 1);
		//seg_grad_buf(f->dc[i].pic, f->dg[i].pic, g->buf, f->dw[i].w, f->dw[i].h, 1);
		//memset(f->dc[i].pic, 0, f->dg[i].w*f->dg[i].h);
		//memset(f->R1[i].pic, 0, f->dg[i].w*f->dg[i].h);
		//memset(f->G1[i].pic, 0, f->dg[i].w*f->dg[i].h);
		//memset(f->B1[i].pic, 0, f->dg[i].w*f->dg[i].h);

		/*
		filter_median(f->dw[i].pic, f->dm[i].pic, f->dw[i].w, f->dw[i].h);
		seg_grad(f->dm[i].pic, f->dg[i].pic, f->dc[i].pic, f->di[i].pic, f->dw[i].w, f->dw[i].h, 3);
		//memset(f->dc[i].pic, 0, f->dg[i].w*f->dg[i].h);

		seg_find_intersect4(f->dg[i].pic, f->dc[i].pic, f->di[i].pic, f->dg[i].w, f->dg[i].h);

		vxc = seg_vertex3(f->dc[i].pic, f->di[i].pic, f->vx, f->vp, (int8*)g->buf, f->dg[i].w, f->dg[i].h);
		seg_remove_virtex(f->vp, vxc, f->dg[i].w, f->dg[i].h);

		//seg_vertex_draw(f->R1[i].pic, f->G1[i].pic, f->B1[i].pic, f->vp, vxc, f->R1[i].w);
		//seg_vertex_draw2(f->dc[i].pic, f->vp, vxc, f->dc[i].w,f->dc[i].h, f->dc[i].w, f->dc[i].h);
		//seg_get_one_color(f->dw[i].pic,  f->dc[i].pic, g->buf, (uint32*)&g->buf[f->dg[i].w*f->dg[i].h>>2], f->dg[i].w, f->dg[i].h);

		memset(g->buf, 0, f->rg[i].w*f->rg[i].h*sizeof(uint32)>>4);
		//memset(f->di[i].pic, 0, f->dg[i].w*f->dg[i].h);
		//rgc = seg_vertex_draw3(f->di[i].pic, f->vp, (uint32*)g->buf, vxc, f->dc[i].w,f->dc[i].h, f->dc[i].w, f->dc[i].h);
		seg_get_or_fill_color(f->dw[i].pic, f->di[i].pic, f->dm[i].pic, (uint32*)&g->buf[f->dg[i].w*f->dg[i].h>>1], f->vp, &f->vp[wc->w*wc->h>>2],
				vxc, f->dg[i].w, f->dg[i].h, f->dg[i].w, f->dg[i].h, 1);
		//memset(f->di[i].pic, 0, f->dg[i].w*f->dg[i].h);
		//rgc = seg_vertex_draw3(f->di[i].pic, f->vp, (uint32*)g->buf, vxc, f->dc[i].w,f->dc[i].h, f->dc[i].w, f->dc[i].h);

		j = i-1;
		memset(g->buf, 0, f->rg[i].w*f->rg[i].h*sizeof(uint32)>>4);
		memset(f->di[j].pic, 0, f->dg[j].w*f->dg[j].h);
		rgc = seg_vertex_draw3(f->di[j].pic, f->vp, (uint32*)g->buf, vxc, f->dc[j].w, f->dc[j].h, f->dc[i].w, f->dc[i].h);
		seg_get_or_fill_color(f->dw[j].pic, f->di[j].pic, f->dm[i].pic, (uint32*)&g->buf[f->dg[j].w*f->dg[j].h>>1], f->vp, &f->vp[wc->w*wc->h>>2],
						vxc, f->dg[j].w, f->dg[j].h, f->dg[i].w, f->dg[i].h, 0);
		seg_draw_line_one(f->di[j].pic, f->dg[j].w, f->dg[j].h);
		//seg_vertex_draw4(f->dw[j].pic, f->dc[j].pic, g->buf, (uint32*)&g->buf[f->dg[j].w*f->dg[j].h>>1], f->vp, vxc, f->dg[j].w, f->dg[j].h, f->dg[i].w, f->dg[i].h);
		//seg_get_one_color2(f->dw[j].pic, f->dc[j].pic, &g->buf[f->dg[j].w*f->dg[j].h>>2], (uint32*)g->buf, (uint32*)&g->buf[f->dg[j].w*f->dg[j].h>>1], rgc, f->dg[j].w, f->dg[j].h);
		//seg_vertex_draw2(f->dc[j].pic, f->vp, vxc, f->dc[j].w, f->dc[j].h, f->dc[i].w, f->dc[i].h);
		//seg_get_one_color1(f->dw[j].pic,  f->dc[j].pic, g->buf, (uint32*)&g->buf[f->dg[j].w*f->dg[j].h>>2], f->dg[j].w, f->dg[j].h);

		j = i-2;
		memset(g->buf, 0, f->rg[i].w*f->rg[i].h*sizeof(uint32)>>4);
		memset(f->di[j].pic, 0, f->dg[j].w*f->dg[j].h);
		rgc = seg_vertex_draw3(f->di[j].pic, f->vp, (uint32*)g->buf, vxc, f->dc[j].w, f->dc[j].h, f->dc[i].w, f->dc[i].h);
		seg_get_or_fill_color(f->dw[j].pic, f->di[j].pic, f->dm[i].pic, (uint32*)&g->buf[f->dg[j].w*f->dg[j].h>>1], f->vp, &f->vp[wc->w*wc->h>>2],
						vxc, f->dg[j].w, f->dg[j].h, f->dg[i].w, f->dg[i].h, 0);
		seg_draw_line_one(f->di[j].pic, f->dg[j].w, f->dg[j].h);
		*/
		//seg_get_one_color2(f->dw[j].pic, f->dc[j].pic, &g->buf[f->dg[j].w*f->dg[j].h>>2], (uint32*)g->buf, (uint32*)&g->buf[f->dg[j].w*f->dg[j].h>>1], rgc, f->dg[j].w, f->dg[j].h);
		//seg_vertex_draw2(f->dc[j].pic, f->vp, vxc, f->dc[j].w, f->dc[j].h, f->dc[i].w, f->dc[i].h);
		//seg_get_one_color1(f->dw[j].pic,  f->dc[j].pic, g->buf, (uint32*)&g->buf[f->dg[j].w*f->dg[j].h>>2], f->dg[j].w, f->dg[j].h);
		j = i-3;
		//rgc = seg_vertex_draw4(f->dc[j].pic, f->vp, (uint32*)g->buf, vxc, f->dc[j].w, f->dc[j].h, f->dc[i].w, f->dc[i].h);
		//seg_get_one_color2(f->dw[j].pic, f->dc[j].pic, &g->buf[f->dg[j].w*f->dg[j].h>>2], (uint32*)g->buf, (uint32*)&g->buf[f->dg[j].w*f->dg[j].h>>1], rgc, f->dg[j].w, f->dg[j].h);
		//seg_vertex_draw2(f->dc[j].pic, f->vp, vxc, f->dc[j].w, f->dc[j].h, f->dc[i].w, f->dc[i].h);
		//seg_get_one_color1(f->dw[j].pic,  f->dc[j].pic, g->buf, (uint32*)&g->buf[f->dg[j].w*f->dg[j].h>>2], f->dg[j].w, f->dg[j].h);

		//seg_vertex_draw1(f->dc[j].pic, f->vp, vxc, f->dc[j].w, f->dc[j].h, 2);
		//seg_get_one_color(f->dw[j].pic,  f->dc[j].pic, g->buf, (uint32*)&g->buf[f->dg[j].w*f->dg[j].h>>2], f->dg[j].w, f->dg[j].h);
		//seg_draw_color_one(f->dc[i].pic, g->buf, (uint32*)&g->buf[f->dg[i].w*f->dg[i].h>>2], f->dg[i].w, f->dg[i].h);
		//seg_draw_line_one(f->dc[i-1].pic, f->dg[i-1].w, f->dg[i-1].h);


		/*
		filter_median(f->dw[i].pic, f->dc[i].pic, f->dw[i].w, f->dw[i].h);
		seg_grad(f->dc[i].pic, f->dg[i].pic, f->dw[i].w, f->dw[i].h, 3);
		memset(f->dc[i].pic, 0, f->dg[i].w*f->dg[i].h);


		seg_find_intersect3(f->dg[i].pic, f->rg[i].pic, f->di[i].pic, f->dg[i].w, f->dg[i].h);
		seg_fill_reg(f->rg[i].pic, (uint32*)g->buf, f->dg[i].w, f->dg[i].h);
		seg_remove_contour(f->rg[i].pic, f->dg[i].w, f->dg[i].h);
		vxc = seg_vertex2(f->rg[i].pic, f->di[i].pic, f->vx, f->vp, f->dg[i].w, f->dg[i].h);
		seg_remove_virtex(f->vp, vxc, f->dg[i].w, f->dg[i].h);
		//seg_vertex_draw(f->R1[i].pic, f->G1[i].pic, f->B1[i].pic, f->vp, vxc, f->R1[i].w);

		seg_vertex_draw1(f->dc[i].pic, f->vp, vxc, f->R1[i].w);
		seg_get_one_color(f->R[i].pic,  f->dc[i].pic, g->buf, (uint32*)&g->buf[f->dg[i].w*f->dg[i].h>>2], f->dg[i].w, f->dg[i].h);
		seg_vertex_draw1(f->R1[i].pic, f->vp, vxc, f->R1[i].w);
		seg_draw_color_one(f->R1[i].pic, g->buf, (uint32*)&g->buf[f->dg[i].w*f->dg[i].h>>2], f->dg[i].w, f->dg[i].h);
		seg_draw_line_one(f->R1[i].pic, f->dg[i].w, f->dg[i].h);
		printf("Entropy = %f\n", entropy8(f->R1[i].pic, (uint32*)g->buf, f->dg[i].w, f->dg[i].h, 8));

		memset(f->dc[i].pic, 0, f->dg[i].w*f->dg[i].h);
		seg_vertex_draw1(f->dc[i].pic, f->vp, vxc, f->R1[i].w);
		seg_get_one_color(f->G[i].pic,  f->dc[i].pic, g->buf, (uint32*)&g->buf[f->dg[i].w*f->dg[i].h>>2], f->dg[i].w, f->dg[i].h);
		seg_vertex_draw1(f->G1[i].pic, f->vp, vxc, f->R1[i].w);
		seg_draw_color_one(f->G1[i].pic, g->buf, (uint32*)&g->buf[f->dg[i].w*f->dg[i].h>>2], f->dg[i].w, f->dg[i].h);
		seg_draw_line_one(f->G1[i].pic, f->dg[i].w, f->dg[i].h);
		printf("Entropy = %f\n", entropy8(f->R1[i].pic, (uint32*)g->buf, f->dg[i].w, f->dg[i].h, 8));

		memset(f->dc[i].pic, 0, f->dg[i].w*f->dg[i].h);
		seg_vertex_draw1(f->dc[i].pic, f->vp, vxc, f->R1[i].w);
		seg_get_one_color(f->B[i].pic,  f->dc[i].pic, g->buf, (uint32*)&g->buf[f->dg[i].w*f->dg[i].h>>2], f->dg[i].w, f->dg[i].h);
		seg_vertex_draw1(f->B1[i].pic, f->vp, vxc, f->R1[i].w);
		seg_draw_color_one(f->B1[i].pic, g->buf, (uint32*)&g->buf[f->dg[i].w*f->dg[i].h>>2], f->dg[i].w, f->dg[i].h);
		seg_draw_line_one(f->B1[i].pic, f->dg[i].w, f->dg[i].h);
		printf("Entropy = %f\n", entropy8(f->R1[i].pic, (uint32*)g->buf, f->dg[i].w, f->dg[i].h, 8));
		*/


		/*
		seg_remove_line1(f->dg[i].pic, f->dg[i].w, f->dg[i].h);

		vxc = seg_vertex(f->dg[i].pic, f->vx, f->vp, f->ln, f->lp, f->dg[i].w, f->dg[i].h);
		seg_draw_line(f->R1[i].pic, f->G1[i].pic, f->B1[i].pic, f->ln, vxc, f->R1[i].w, f->R1[i].h);*/

		//rgc = seg_group_pixels(f->R[i].pic, f->G[i].pic, f->B[i].pic, f->dc[i].pic, f->dg[i].pic,
		//		(uint32*)&g->buf[f->dg[i].w*f->dg[i].h*3], g->buf,
		//		(uint32*)&g->buf[f->dg[i].w*f->dg[i].h], (uint32*)&g->buf[f->dg[i].w*f->dg[i].h*2], f->dg[i].w, f->dg[i].h);



		//memset(f->dg[i].pic, 0, f->dg[i].w*f->dg[i].h);
		//seg_remove_line(f->dc[i].pic, f->dg[i].pic, (uint32*) g->buf, f->dg[i].w, f->dg[i].h);

		//memset(f->dc[i].pic, 0, f->dg[i].w*f->dg[i].h);
		//seg_grad_max(f->dg[i].pic, f->dc[i].pic, f->dw[i].w, f->dw[i].h);

		//seg_grad_sub(f->dc[i].pic, f->dg[i].pic, f->dw[i].w, f->dw[i].h, 0);
		/*
		seg_fall_forest(f->dg[i].pic, f->dc[i].pic, f->dw[i].w, f->dw[i].h);
		//rgc =  seg_group_reg(f->dc[i].pic, f->dg[i].pic, (uint32*)g->buf, f->dw[i].w, f->dw[i].h);
		rgc = seg_group_pixels(f->R[i].pic, f->G[i].pic, f->B[i].pic, f->dc[i].pic, f->dg[i].pic,
				(uint32*)&g->buf[f->dg[i].w*f->dg[i].h*3], g->buf,
				(uint32*)&g->buf[f->dg[i].w*f->dg[i].h], (uint32*)&g->buf[f->dg[i].w*f->dg[i].h*2], f->dg[i].w, f->dg[i].h);
		seg_draw_grad(f->dg[i].pic, f->dc[i].pic, (uint32*)&g->buf[f->dg[i].w*f->dg[i].h*3], f->dg[i].w, f->dg[i].h);
		*/
		//memset(f->dc[i].pic, 0, f->dg[i].w*f->dg[i].h);
		//seg_new_contur(f->dw[i].pic, f->dc[i].pic, (uint32*)&g->buf[f->dg[i].w*f->dg[i].h], (uint32*)&g->buf[f->dg[i].w*f->dg[i].h*2], f->dw[i].w, f->dw[i].h);

		/*
		seg_fall_forest(f->dg[i].pic, f->dc[i].pic, f->dw[i].w, f->dw[i].h);

		rgc = seg_group_pixels(f->R[i].pic, f->G[i].pic, f->B[i].pic, f->dc[i].pic, f->dg[i].pic,
				(uint32*)&g->buf[f->dg[i].w*f->dg[i].h*3], g->buf,
				(uint32*)&g->buf[f->dg[i].w*f->dg[i].h], (uint32*)&g->buf[f->dg[i].w*f->dg[i].h*2], f->dg[i].w, f->dg[i].h);

		seg_draw_grad(f->dg[i].pic, f->dc[i].pic, (uint32*)&g->buf[f->dg[i].w*f->dg[i].h*3], f->dg[i].w, f->dg[i].h);
		memset(f->dg[i].pic, 0, f->dc[i].w*f->dc[i].h);
		seg_find_intersect(f->dc[i].pic, f->dg[i].pic, f->dg[i].w, f->dg[i].h);

		vxc = seg_vertex(f->dg[i].pic, f->R[i].pic, f->G[i].pic, f->B[i].pic, f->vx, f->vp, f->ln, f->lp, f->dg[i].w, f->dg[i].h);

		seg_draw_line(f->R1[i].pic, f->R1[i].pic, f->R1[i].pic, f->ln, vxc, f->dg[i].w, f->dg[i].h);

		seg_get_color2(f->R[i].pic, f->G[i].pic, f->B[i].pic, f->R1[i].pic, g->buf,
				(uint32*)&g->buf[f->dg[i].w*f->dg[i].h], (uint32*)&g->buf[f->dg[i].w*f->dg[i].h*2], f->dg[i].w, f->dg[i].h);
		memset(f->R1[i].pic, 0, f->dg[i].w*f->dg[i].h);

		seg_draw_line(f->R1[i].pic, f->G1[i].pic, f->B1[i].pic, f->ln, vxc, f->dg[i].w, f->dg[i].h);

		seg_draw_color2(f->R1[i].pic, f->G1[i].pic, f->B1[i].pic, g->buf,
				(uint32*)&g->buf[f->dg[i].w*f->dg[i].h], (uint32*)&g->buf[f->dg[i].w*f->dg[i].h*2], f->dg[i].w, f->dg[i].h);

		*/

		//seg_max_rise(f->dg[i].pic, f->dc[i].pic, f->dw[i].w, f->dw[i].h);
		//seg_max_con(f->dc[i].pic,f->R1[i].pic, f->dw[i].w, f->dw[i].h);
		//memset(f->G1[i].pic, 0, f->dc[i].w*f->dc[i].h);
		//seg_find_intersect(f->dg[i].pic, f->G1[i].pic, f->dg[i].w, f->dg[i].h);


		//rgc = seg_init_regs(f->R[i].pic, f->G[i].pic, f->B[i].pic, f->dg[i].pic, (uint16*)&g->buf[f->dg[i].w*f->dg[i].h*3], (uint32*)g->buf,
		//		(uint32*)&g->buf[f->dg[i].w*f->dg[i].h], (uint32*)&g->buf[f->dg[i].w*f->dg[i].h*2], f->dg[i].w, f->dg[i].h);
		//rgc = seg_fill_regs(f->R[i].pic, f->G[i].pic, f->B[i].pic, f->dg[i].pic, (uint16*)&g->buf[f->dg[i].w*f->dg[i].h*3], (uint32*)g->buf,
		//		(uint32*)&g->buf[f->dg[i].w*f->dg[i].h], rgc, f->dg[i].w, f->dg[i].h);
		//seg_get_color2(f->R[i].pic, f->G[i].pic, f->B[i].pic, f->dg[i].pic, g->buf,
		//		(uint32*)&g->buf[f->dg[i].w*f->dg[i].h], (uint32*)&g->buf[f->dg[i].w*f->dg[i].h*2], f->dg[i].w, f->dg[i].h);
		//seg_draw_color2(f->R1[i].pic, f->G1[i].pic, f->B1[i].pic, g->buf,
		//		(uint32*)&g->buf[f->dg[i].w*f->dg[i].h], (uint32*)&g->buf[f->dg[i].w*f->dg[i].h*2], f->dg[i].w, f->dg[i].h);
		/*
		memset(f->dc[i].pic, 0, f->dc[i].w*f->dc[i].h);
		//seg_find_intersect1(f->dg[i].pic, f->dc[i].pic, f->dg[i].w, f->dg[i].h, 3);

		//seg_find_intersect(f->dg[i].pic, f->dc[i].pic, f->dg[i].w, f->dg[i].h);
		seg_find_intersect1(f->dg[i].pic, f->dc[i].pic, f->dg[i].w, f->dg[i].h, 3);


		vxc = seg_vertex(f->dc[i].pic, f->R[i].pic, f->G[i].pic, f->B[i].pic, f->vx, f->vp, f->ln, f->lp, f->dg[i].w, f->dg[i].h);

		seg_draw_line(f->R1[i].pic, f->G1[i].pic, f->B1[i].pic, f->ln, vxc, f->dg[i].w, f->dg[i].h);

		//seg_get_color(f->R[i].pic, f->G[i].pic, f->B[i].pic, f->R1[i].pic, f->G1[i].pic, f->B1[i].pic, g->buf, f->dg[i].w, f->dg[i].h);
		seg_get_color2(f->R[i].pic, f->G[i].pic, f->B[i].pic, f->R1[i].pic, g->buf,
				(uint32*)&g->buf[f->dg[i].w*f->dg[i].h], (uint32*)&g->buf[f->dg[i].w*f->dg[i].h*2], f->dg[i].w, f->dg[i].h);


		memset(f->R1[i].pic, 0, f->dg[i].w*f->dg[i].h);
		memset(f->G1[i].pic, 0, f->dg[i].w*f->dg[i].h);
		memset(f->B1[i].pic, 0, f->dg[i].w*f->dg[i].h);
		//memset(f->R[i].pic, 0, f->dg[i].w*f->dg[i].h);
		//memset(f->G[i].pic, 0, f->dg[i].w*f->dg[i].h);
		//memset(f->B[i].pic, 0, f->dg[i].w*f->dg[i].h);
		seg_draw_line(f->R1[i].pic, f->G1[i].pic, f->B1[i].pic, f->ln, vxc, f->dg[i].w, f->dg[i].h);
		seg_draw_color2(f->R1[i].pic, f->G1[i].pic, f->B1[i].pic, g->buf,
				(uint32*)&g->buf[f->dg[i].w*f->dg[i].h], (uint32*)&g->buf[f->dg[i].w*f->dg[i].h*2], f->dg[i].w, f->dg[i].h);

		//seg_draw_color(f->R1[i].pic, f->G1[i].pic, f->B1[i].pic, g->buf, f->dg[i].w, f->dg[i].h);
		//seg_draw_color1(f->R1[i].pic, f->G1[i].pic, f->B1[i].pic, f->R[i].pic, (uint32*)&g->buf[f->dg[i].w<<3], (uint32*)g->buf, f->dg[i].w, f->dg[i].h);
		//seg_draw_line(f->R1[i].pic, f->G1[i].pic, f->B1[i].pic, f->ln, vxc, f->dg[i].w, f->dg[i].h);

		//rgc = seg_fill_region(f->R1[i].pic, f->G1[i].pic, f->B1[i].pic, f->dg[i].w, f->dg[i].h);
		memset(f->vx, 0, f->dg[i].w*f->dg[i].h*sizeof(Vertex));
		memset(f->ln, 0, f->dg[i].w*f->dg[i].h*sizeof(Line));
		*/
	}

	//image_segment(&f->img[0], f->vx, f->vp, f->ln, g->buf, wc->steps);
	f->state |= SEGMENTATION;
	return 1;
}

/**	\brief	Match the singular points in two frames
	\param	g		The GOP structure.
	\param	fn1		The first frame number.
	\param	fn2		The second frame number.
	\param	wc		The walet config structure.
	\retval			1 - if all OK, 0 - if not OK
*/
uint32 frame_match(GOP *g, uint32 fn1, uint32 fn2, WaletConfig *wc)
{
	uint32  sq = wc->w*wc->h;
	Frame *f1 = &g->frames[fn1];
	Frame *f2 = &g->frames[fn2];

	if(check_state(f1->state, SEGMENTATION) && check_state(f2->state, SEGMENTATION)){
		if(wc->ccol == BAYER){
			seg_compare(f1->pixs,  f1->edges, f1->nedge, f1->grad.pic, f2->grad.pic, f1->Y.pic, f2->Y.pic, g->buf, f1->grad.w, f1->grad.h, wc->mv);
			//for(i=0; i < sq; i++) frm2->pix[0].pic[i] = 0;
			//seg_mvector_copy(frm1->pixs, frm1->grad[0].pic, frm1->Y[0].pic, frm2->line.pic, frm1->grad[0].width, frm1->grad[0].height);
		}
		f1->state |= MATCH;
		return 1;
	} else return 0;
}

/**	\brief	Write frame in file.
	\param	g		The GOP structure.
	\param	fn		The frame number.
	\param	wc		The walet config structure.
	\param	wl		The file descriptor.
	\retval			1 - if all OK, 0 - if not OK
*/
uint32 frame_write(GOP *g, uint32 fn, WaletConfig *wc, FILE *wl)
{
    uint8 *bits;
    uint32 i, lv, im, sb, sz, size=0, ic, bc[4], is[4];
	Frame *f = &g->frames[fn];

	bc[0] = 0; bc[1] = 0; bc[2] = 0; bc[3] = 0;
	//The images position in the buffer
	is[0] = 0;
	is[1] = is[0] + f->img[0].isz;
	is[2] = is[1] + f->img[1].isz;
	is[3] = is[2] + f->img[2].isz;
	//printf("is[1] = %d is[2] = %d is[3] = %d \n", is[1], is[2], is[3]);

    if(fwrite (&is[1], sizeof(uint32), 3 , wl) != 3) { printf("The images position  write error\n"); return 0; }
    size += sizeof(uint32)*3;

    if(wc->ccol == BAYER) {
    	ic = 4; sz = (wc->steps*3 + 1)*ic; }
    else if(wc->ccol == CS420 || wc->ccol == CS444){
    	ic = 3; sz = (wc->steps*3 + 1)*ic; }
    else {
    	ic = 1; sz =  wc->steps*3 + 1; }

	bits = (uint8 *)calloc(sz, sizeof(uint8));
	i=0;
   	for(lv = wc->steps-1; lv+1; lv--){
   		for(im = 0; im < ic; im++){
   			for(sb = (lv == wc->steps-1) ? 0 : 1; sb < 4; sb++){
   				bits[i++] = (f->img[im].l[lv].s[sb].a_bits<<4) | f->img[im].l[lv].s[sb].q_bits;
   				//printf("img[%d].l[%d].s[%d] a_bits = %d q_bits = %d\n",im, lv, sb, f->img[im].l[lv].s[sb].a_bits, f->img[im].l[lv].s[sb].q_bits);
   			}
   		}
   	}
   	// Write bits array
    if(fwrite (bits, 1, sz , wl) != sz) { printf("Bits array write error\n"); return 0; }
    size += sz;

    /// Write subbands
    for(lv = wc->steps-1; lv+1; lv--){
    	for(im = 0; im < ic; im++){
    		for(sb = (lv == wc->steps-1) ? 0 : 1; sb < 4; sb++){
    			if(fwrite (&f->img[im].l[lv].s[sb].ssz, sizeof(uint32), 1, wl) != 1) { printf("Size write error\n"); return 0; }
    			//printf("img[%d].l[%d].s[%d] = %d\n", im, lv, sb, f->img[im].l[lv].s[sb].ssz);
    			size += sizeof(uint32);
    			if(fwrite (&g->buf[is[im] + bc[im]], 1, f->img[im].l[lv].s[sb].ssz, wl) != f->img[im].l[lv].s[sb].ssz)
					{ printf("Subband  img[%d].l[%d].s[%d] write error\n", im, lv, sb); return 0; }
    			size += f->img[im].l[lv].s[sb].ssz;
    			bc[im] += f->img[im].l[lv].s[sb].ssz;
    		}
    	}
    }
    free(bits);
    return size;
}

/**	\brief	Read frame from file.
	\param	g		The GOP structure.
	\param	fn		The frame number.
	\param	wc		The walet config structure.
	\param	wl		The file descriptor.
	\retval			1 - if all OK, 0 - if not OK
*/
uint32 frame_read(GOP *g, uint32 fn, WaletConfig *wc, FILE *wl)
{
    uint8 *bits;
    uint32 i, lv, im, sb, sz, size=0, ic, bc[4], is[4];
	Frame *f = &g->frames[fn];
	//printf("fn = %d %p\n", fn, f);

	bc[0] = 0; bc[1] = 0; bc[2] = 0; bc[3] = 0;
	//The position images in the buffer
	is[0] = 0;
	//is[1] = is[0] + f->img[0].isz;
	//is[2] = is[1] + f->img[1].isz;
	//is[3] = is[2] + f->img[2].isz;

    if(fread (&is[1], sizeof(uint32), 3 , wl) != 3) { printf("The images position read error\n"); return 0; }
    size += sizeof(uint32)*3;
    //printf("is[1] = %d is[2] = %d is[3] = %d \n", is[1], is[2], is[3]);

    if(wc->ccol == BAYER) {
    	ic = 4; sz = (wc->steps*3 + 1)*ic; }
    else if(wc->ccol == CS420 || wc->ccol == CS444){
    	ic = 3; sz = (wc->steps*3 + 1)*ic; }
    else {
    	ic = 1; sz =  wc->steps*3 + 1; }

	bits = (uint8 *)calloc(sz, sizeof(uint8));
  	// Read bits array
    if(fread(bits, 1, sz, wl)!= sz) { printf("Bits array read error\n"); return 0; }
    size += sz;
    i = 0;
   	for(lv = wc->steps-1; lv+1; lv--){
   		for(im = 0; im < ic; im++){
   			for(sb = (lv == wc->steps-1) ? 0 : 1; sb < 4; sb++){
   				f->img[im].l[lv].s[sb].a_bits = (bits[i]>>4);
   				f->img[im].l[lv].s[sb].q_bits = (bits[i++]&15);
   				//printf("img[%d].l[%d].s[%d] a_bits = %d q_bits = %d\n",im, lv, sb, f->img[im].l[lv].s[sb].a_bits, f->img[im].l[lv].s[sb].q_bits);
   			}
   		}
   	}
    /// Read subbands

    for(lv = wc->steps-1; lv+1; lv--){
    	for(im = 0; im < ic; im++){
    		for(sb = (lv == wc->steps-1) ? 0 : 1; sb < 4; sb++){
    			if(fread(&f->img[im].l[lv].s[sb].ssz, sizeof(uint32), 1, wl) != 1) { printf("Size read error\n"); return 0; }
       			//printf("img[%d].l[%d].s[%d] = %d\n", im, lv, sb, f->img[im].l[lv].s[sb].ssz);
    			size += sizeof(uint32);
    			if(fread(&g->buf[is[im] + bc[im]], 1, f->img[im].l[lv].s[sb].ssz, wl) != f->img[im].l[lv].s[sb].ssz)
					{ printf("Subband  img[%d].l[%d].s[%d] read error\n", im, lv, sb); return 0; }
    			size += f->img[im].l[lv].s[sb].ssz;
    			bc[im] += f->img[im].l[lv].s[sb].ssz;
    		}
    	}
    }

    f->state = BUFFER_READ;

    free(bits);
    return size;
 }

/**	\brief	Frame compression
	\param	g		The GOP structure.
	\param	fn		The frame number.
	\param	wc		The walet config structure.
	\param	times	The compression ratio in times.
*/
void frame_compress(GOP *g, uint32 fn, WaletConfig *wc, uint32 times)
{
	clock_t start, end;
	double time=0., tmp;
	struct timeval tv;
	uint32 size;

	gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
	frame_transform(g, fn, wc);
	gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
	tmp = (double)(end-start)/1000000.; time +=tmp;
	printf("DWT time             = %f\n", tmp);

	gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
	frame_fill_subb	(g, fn, wc);
	gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
	tmp = (double)(end-start)/1000000.; time +=tmp;
	printf("Fill subband time    = %f\n", tmp);

	gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
	frame_bits_alloc(g, fn, wc, times);
	gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
	tmp = (double)(end-start)/1000000.; time +=tmp;
	printf("Bits allocation time = %f\n", tmp);

	gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
	frame_range_encode(g, fn, wc, &size);
	gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
	tmp = (double)(end-start)/1000000.; time +=tmp;
	printf("Range coder time     = %f\n", tmp);

	printf("Frame time = %f size  = %d\n", time, size);
	//frame_write
}

/**	\brief	Frame decompression
	\param	g		The GOP structure.
	\param	fn		The frame number.
	\param	wc		The walet config structure.
	\param	isteps	The steps of IDWT.
*/
void frame_decompress(GOP *g, uint32 fn, WaletConfig *wc, uint32 isteps)
{
	uint32 size;
	clock_t start, end;
	struct timeval tv;
	double time=0., tmp;

	gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
	printf("start frame_range_decode \n");

	frame_range_decode(g, fn, wc, &size);
	gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
	tmp = (double)(end-start)/1000000.; time +=tmp;
	printf("Range decoder time    = %f\n", tmp);

	gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
	frame_idwt(g, fn, wc, isteps);
	gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
	tmp = (double)(end-start)/1000000.; time +=tmp;
	printf("IDWT time             = %f\n", tmp);

	printf("Frame time = %f size  = %d\n", time, size);
}

void frame_test(GOP *g, uint32 fn, WaletConfig *wc, uint32 times)
{
	clock_t start, end;
	double time=0., tmp, max;
	struct timeval tv;
	uint32 size, i, j, im, in, jn, imn, k =0;
	uint32 sz = (wc->w*wc->h*wc->bpp)/times;

	fn = 0;
	frame_transform(g, fn, wc);
	frame_fill_subb	(g, fn, wc);
	frame_transform(g, 1, wc);

	for(im = 0; im < 4; im++) for(i=0; i < wc->steps; i++) for(j = (i == wc->steps-1) ? 0 : 1; j < 4; j++)
		size += subb_size(&g->frames[fn].img[im].l[i].s[j]);
	printf("sz = %d size = %d\n", sz, size);
	max = 0.; k = 0;
	while(size > sz){
		for(im = 0; im < 4; im++){
			for(i=0; i < wc->steps; i++){
				for(j = 1; j < 4; j++) {
					if(g->frames[fn].img[im].l[i].s[j].q_bits){
						//g->frames[fn].img[im].l[i].s[j].q_bits--;
						g->frames[fn].img[im].l[i].s[j].q_bits = g->frames[fn].img[im].l[i].s[j].q_bits == 2 ? 0 : g->frames[fn].img[im].l[i].s[j].q_bits - 1;
						//subb_quantization(&g->frames[fn].img[im].l[i].s[j], g->buf);
						frame_quantization(g, fn, wc);
						g->frames[fn].img[im].l[i].s[j].q_bits = g->frames[fn].img[im].l[i].s[j].q_bits == 0 ? 2 : g->frames[fn].img[im].l[i].s[j].q_bits + 1;
						//g->frames[fn].img[im].l[i].s[j].q_bits++;
						//frame_test(g, fn, wc);
						frame_idwt(g, fn, wc, wc->steps);
						subb_copy(&g->frames[1].img[im].l[i].s[j], &g->frames[fn].img[im].l[i].s[j]);
						g->frames[fn].img[im].l[i].s[j].ssim =
								utils_psnr_16(g->frames[fn].b.pic, g->frames[fn].d.pic, g->frames[fn].b.w*g->frames[fn].b.h, 1);
						//utils_ssim_16(g->frames[fn].b.pic, g->frames[fn].d.pic, g->frames[fn].b.w, g->frames[fn].b.h, 8, 3, 1);
					} else {
						g->frames[fn].img[im].l[i].s[j].ssim = 0.;
					}

					//printf("%d ssim = %f\n", k, g->frames[fn].img[im].l[i].s[j].ssim);
				}
			}
		}

		max = 0.;
		for(im = 0; im < 4; im++){
			for(i=0; i <  wc->steps; i++){
				for(j = 1; j < 4; j++) {
					if(g->frames[fn].img[im].l[i].s[j].ssim > max) {
						max = g->frames[fn].img[im].l[i].s[j].ssim;
						imn = im; in = i; jn = j;
					}
				}
			}
		}
		g->frames[fn].img[imn].l[in].s[jn].q_bits = g->frames[fn].img[imn].l[in].s[jn].q_bits == 2 ? 0 : g->frames[fn].img[imn].l[in].s[jn].q_bits - 1;
		//subb_copy(&g->frames[1].img[imn].l[in].s[jn], &g->frames[fn].img[imn].l[in].s[jn]);
		frame_quantization(g, fn, wc);
		//subb_quantization(&g->frames[fn].img[imn].l[in].s[jn], g->buf);
		frame_idwt(g, fn, wc, wc->steps);
		g->frames[fn].img[imn].l[in].s[jn].ssim =
				utils_psnr_16(g->frames[fn].b.pic, g->frames[fn].d.pic, g->frames[fn].b.w*g->frames[fn].b.h, 1);
		//utils_ssim_16(g->frames[fn].b.pic, g->frames[fn].d.pic, g->frames[fn].b.w, g->frames[fn].b.h, 8, 3, 1);
		subb_copy(&g->frames[1].img[imn].l[in].s[jn], &g->frames[fn].img[imn].l[in].s[jn]);

		/*
		//printf("img = %d l = %d s = %d ssim = %f\n", imn, in, jn, max);
		if(g->frames[fn].img[imn].l[in].s[jn].q_bits > 2){
			g->frames[fn].img[imn].l[in].s[jn].q_bits-=2;
			g->frames[fn].img[imn].l[in].s[jn].q_bits = g->frames[fn].img[imn].l[in].s[jn].q_bits == 1 ? 0 : g->frames[fn].img[imn].l[in].s[jn].q_bits;
			subb_copy(&g->frames[1].img[imn].l[in].s[jn], &g->frames[fn].img[imn].l[in].s[jn]);
			subb_quantization(&g->frames[fn].img[imn].l[in].s[jn], g->buf);
			frame_idwt(g, fn, wc, wc->steps);
			g->frames[fn].img[imn].l[in].s[jn].ssim =
					utils_ssim_16(g->frames[fn].b.pic, g->frames[fn].d.pic, g->frames[fn].b.w, g->frames[fn].b.h, 8, 3, 1);
			//printf("utils_ssim_16 = %f\n", g->frames[fn].img[imn].l[in].s[jn].ssim);

			g->frames[fn].img[imn].l[in].s[jn].q_bits = g->frames[fn].img[imn].l[in].s[jn].q_bits ?
					g->frames[fn].img[imn].l[in].s[jn].q_bits + 1 : g->frames[fn].img[imn].l[in].s[jn].q_bits + 2;
			//printf("q_bits = %d a_bits = %d\n", g->frames[fn].img[imn].l[in].s[jn].q_bits, g->frames[fn].img[imn].l[in].s[jn].a_bits);
			subb_copy(&g->frames[1].img[imn].l[in].s[jn], &g->frames[fn].img[imn].l[in].s[jn]);
			subb_quantization(&g->frames[fn].img[imn].l[in].s[jn], g->buf);

		} else {
			g->frames[fn].img[imn].l[in].s[jn].q_bits = 0;
			g->frames[fn].img[imn].l[in].s[jn].ssim = 0.;
			//printf("q_bits = %d a_bits = %d ssim = %f\n", g->frames[fn].img[imn].l[in].s[jn].q_bits, g->frames[fn].img[imn].l[in].s[jn].a_bits, g->frames[fn].img[imn].l[in].s[jn].ssim);
			subb_copy(&g->frames[1].img[imn].l[in].s[jn], &g->frames[fn].img[imn].l[in].s[jn]);
			subb_quantization(&g->frames[fn].img[imn].l[in].s[jn], g->buf);
		}
		*/
		size = 0;
		for(im = 0; im < 4; im++) for(i=0; i < wc->steps; i++) for(j = (i == wc->steps-1) ? 0 : 1; j < 4; j++)
			size += subb_size(&g->frames[fn].img[im].l[i].s[j]);

		printf("%4d %2d %2d %2d q_bits = %2d a_bits = %2d ssim = %f size  = %d\n", k, imn, in, jn,
				g->frames[fn].img[imn].l[in].s[jn].q_bits, g->frames[fn].img[imn].l[in].s[jn].a_bits, g->frames[fn].img[imn].l[in].s[jn].ssim, size);
		k++;
	}

	for(im = 0; im < 4; im++) for(i=0; i < wc->steps; i++) for(j = (i == wc->steps-1) ? 0 : 1; j < 4; j++)
		printf("%2d %2d %2d q_bits = %2d a_bits = %2d\n",im, i, j, g->frames[fn].img[im].l[i].s[j].q_bits, g->frames[fn].img[im].l[i].s[j].a_bits);
	//printf("Frame time = %f size  = %d\n", time, size);
	//frame_write
}

