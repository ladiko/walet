#include <walet.h>

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <math.h>

#define hsh(w,x) ((x == -2) ? -w-w :(x == 2))

// Red and blue pattern for bayer median filter
int rb[9][2] = {
		{-2,-2,},{ 0,-2,},{ 2,-2,},    	// R   R   R
		{-2, 0,},{ 0, 0,},{ 2, 0,},		//
		{-2, 2,},{ 0, 2,},{ 2, 2,},		// R   R   R
};										//
										// R   R   R
// Green pattern for bayer median filter
int g[9][2] = {
		{ 0,-2,},{ 1,-1,},{ 2, 0,},		//     G
		{-1,-1,},{ 0, 0,},{ 1, 1,},		//   G   G
		{-2, 0,},{-1, 1,},{ 0,-2,},		// G   G   G
};										//   G   G
										//     G


#define ll(step, x, y) img[x*step + y*step*width];
#define hl(step, x, y) img[x*step + (step>>1)  + y*step*width];
#define lh(step, x, y) img[x*step + (step>>1)*width  + y*step*width];
#define hh(step, x, y) img[x*step + (step>>1) + (step>>1)*width  + y*step*width];

#define clip(x)		abs(x);
#define lb(x) (((x+128) < 0) ? 0 : (((x+128) > 255) ? 255 : (x+128)))
#define lb1(x) (((x) < 0) ? 0 : (((x) > 255) ? 255 : (x)))
#define oe(a,x)	(a ? x&1 : (x+1)&1)
#define border(x,w)	(x) < 0 ? (-x) : (((x) < w) ? x : (w<<1) - (x) - 2)



static inline void drawrect(uint8 *rgb, int16 *pic, uint32 x0, uint32 y0, uint32 w, uint32 h, uint32 size, uint32 shift)
{
	uint32 x, y, tmp;
	for(y=0; y < h; y++ ){
		for(x=0; x < w; x++){
			//tmp = rnd(im[y*w+x] < 0 ? -im[y*w+x]<<1 : im[y*w+x]<<1);
			tmp = lb1(shift + pic[y*w+x]);
			//if(tmp > 255) printf("tmp = %d pic = %d\n", tmp, pic[y*w+x]);
			rgb[3*((y+y0)*size +x0 +x)]   = tmp; //rnd(shift+im[y*w+x]); //im[y*w+x] ? 255 : 0; //
			rgb[3*((y+y0)*size +x0 +x)+1] = tmp; //rnd(shift+im[y*w+x]); //im[y*w+x] ? 255 : 0; //
			rgb[3*((y+y0)*size +x0 +x)+2] = tmp; //rnd(shift+im[y*w+x]); //im[y*w+x] ? 255 : 0; //
		}
	}
}

static inline void drawrect_rgb(uint8 *rgb, uint8 *im, uint32 w0, uint32 h0, uint32 w, uint32 h, uint32 w1)
{
	uint32 x, y, tmp;
	for(y=0; y < h; y++ ){
		for(x=0; x < w; x++){
			rgb[3*((y+h0)*w1+w0+x)  ] = im[3*(y*w+x)  ];
			rgb[3*((y+h0)*w1+w0+x)+1] = im[3*(y*w+x)+1];
			rgb[3*((y+h0)*w1+w0+x)+2] = im[3*(y*w+x)+2];
		}
	}
}

uint8* utils_dwt_image_draw(Image *img, uint8 *rgb, uint32 steps)
{
	uint32 i, j, x, y, w = img->w;

	if(steps != 0){
		for(j=0; j < steps; j++){
			printf("img->l[j].s[0].w = %d img->l[j].s[0].h = %d\n", img->l[j].s[0].w, img->l[j].s[0].h);
			drawrect(rgb, img->l[j].s[1].pic, img->l[j].s[0].w,	0,                img->l[j].s[1].w, img->l[j].s[1].h, w, 128);
			drawrect(rgb, img->l[j].s[2].pic, 0,                img->l[j].s[0].h, img->l[j].s[2].w, img->l[j].s[2].h, w, 128);
			drawrect(rgb, img->l[j].s[3].pic, img->l[j].s[0].w, img->l[j].s[0].h, img->l[j].s[3].w, img->l[j].s[3].h, w, 128);
		}
		drawrect(rgb, img->l[steps-1].s[0].pic, 0, 0, img->l[steps-1].s[0].w, img->l[steps-1].s[0].h, w, 128);
	}
	return rgb;
}

uint8* utils_dwt_bayer_draw(GOP *g, uint32 fn, WaletConfig *wc, uint8 *rgb, uint8 steps)
{
	uint32 i, j, x, y, w;
	Frame *f = &g->frames[fn];

	w = f->img[0].w + f->img[1].w;
	if(steps == wc->steps && wc->steps){
		for(j=0; j < steps; j++){
			for(i=0; i < 4; i++){
				if		(i == 0){ x = 0; 			y = 0;}
				else if	(i == 1){ x = f->img[0].w; 	y = 0;}
				else if (i == 2){ x = 0; 			y = f->img[0].h;}
				else			{ x = f->img[0].w;	y = f->img[0].h;}
				if(j == steps-1){
					drawrect(rgb, f->img[i].l[j].s[0].pic, x, y, f->img[i].l[j].s[0].w, f->img[i].l[j].s[0].h, w, 128);
					//if(i) 	drawrect(rgb, f->img[i].l[j].s[0].pic, x, y, f->img[i].l[j].s[0].w, f->img[i].l[j].s[0].h, w, 128);
					//else 	drawrect(rgb, f->img[i].l[j].s[0].pic, x, y, f->img[i].l[j].s[0].w, f->img[i].l[j].s[0].h, w, 0);
				}
				drawrect(rgb, f->img[i].l[j].s[1].pic, x + f->img[i].l[j].s[0].w,	y,                     		f->img[i].l[j].s[1].w, f->img[i].l[j].s[1].h, w, 128);
				drawrect(rgb, f->img[i].l[j].s[2].pic, x,                     		y + f->img[i].l[j].s[0].h, 	f->img[i].l[j].s[2].w, f->img[i].l[j].s[2].h, w, 128);
				drawrect(rgb, f->img[i].l[j].s[3].pic, x + f->img[i].l[j].s[0].w, 	y + f->img[i].l[j].s[0].h, 	f->img[i].l[j].s[3].w, f->img[i].l[j].s[3].h, w, 128);
			}
		}
	} else {
		if(wc->steps == 0){
			drawrect(rgb, f->img[0].p, 0,         		0,         		f->img[0].w, f->img[0].h, w, 128);
			drawrect(rgb, f->img[1].p, f->img[0].w, 	0,         		f->img[1].w, f->img[1].h, w, 128);
			drawrect(rgb, f->img[2].p, 0,         		f->img[0].h, 	f->img[2].w, f->img[2].h, w, 128);
			drawrect(rgb, f->img[3].p, f->img[0].w, 	f->img[0].h, 	f->img[3].w, f->img[3].h, w, 128);
		} else {
			j = steps;
			for(i=0; i < 4; i++){
				if(steps == 0){
					if		(i == 0){ x = 0; 			y = 0;}
					else if	(i == 1){ x = f->img[0].w; 	y = 0;}
					else if (i == 2){ x = 0; 			y = f->img[0].h;}
					else			{ x = f->img[0].w; 	y = f->img[0].h;}
				} else {
					if		(i == 0){ x = 0; 						y = 0;}
					else if	(i == 1){ x = f->img[i].l[j-1].s[0].w; 	y = 0;}
					else if (i == 2){ x = 0; 						y = f->img[i].l[j-1].s[0].h;}
					else			{ x = f->img[i].l[j-1].s[0].w;	y = f->img[i].l[j-1].s[0].h;}
				}
				drawrect(rgb, f->img[i].l[j].s[0].pic, x, y, f->img[i].l[j].s[0].w, f->img[i].l[j].s[0].h, w, 128);
				//if(i)	drawrect(rgb, f->img[i].l[j].s[0].pic, x, y, f->img[i].l[j].s[0].w, f->img[i].l[j].s[0].h, w, 128);
				//else	drawrect(rgb, f->img[i].l[j].s[0].pic, x, y, f->img[i].l[j].s[0].w, f->img[i].l[j].s[0].h, w, 0);
				drawrect(rgb, f->img[i].l[j].s[0].pic, x,                     		y,                     		f->img[i].l[j].s[0].w, f->img[i].l[j].s[0].h, w, 128);
				drawrect(rgb, f->img[i].l[j].s[1].pic, x + f->img[i].l[j].s[0].w, 	y,                 			f->img[i].l[j].s[1].w, f->img[i].l[j].s[1].h, w, 128);
				drawrect(rgb, f->img[i].l[j].s[2].pic, x,                     		y + f->img[i].l[j].s[0].h, 	f->img[i].l[j].s[2].w, f->img[i].l[j].s[2].h, w, 128);
				drawrect(rgb, f->img[i].l[j].s[3].pic, x + f->img[i].l[j].s[0].w, 	y + f->img[i].l[j].s[0].h, 	f->img[i].l[j].s[3].w, f->img[i].l[j].s[3].h, w, 128);
			}

		}
	}

	return rgb;
}

void shift_b_to_w(uint8 *in, int8 *out, int shift, uint32 size)
{
	uint32 i;
	for(i=0; i < size; i++) out[i] = in[i] + shift;
}

void shift_w_to_b(int8 *in, uint8 *out, int shift, uint32 size)
{
	uint32 i;
	int tmp;
	for(i=0; i < size; i++) {
		tmp = in[i] + shift;
		if(in[i] < -128 || in[i] > 127) printf("out[i] = %d\n", in[i]);
		out[i] = in[i] + shift;
	}
}

uint8* utils_grey_draw(int16 *img, uint8 *rgb, uint32 w, uint32 h, uint32 sh)
{
	int i, j, dim = h*w*3;
	for(i = 0,  j= 0; j < dim; j+=3, i++){
		rgb[j]     = img[i] + sh;
		rgb[j + 1] = img[i] + sh;
		rgb[j + 2] = img[i] + sh;
	}
	return rgb;
}

uint8* utils_grey_draw8(uint8 *img, uint8 *rgb, uint32 w, uint32 h, uint32 sh)
{
	int i, j, dim = h*w*3;
	for(i = 0,  j= 0; j < dim; j+=3, i++){
		rgb[j]     = img[i] + sh;
		rgb[j + 1] = img[i] + sh;
		rgb[j + 2] = img[i] + sh;
	}
	return rgb;
}


//#define lb(x) (x&0xFF)

uint8* utils_bayer_draw(int16 *img, uint8 *rgb, uint32 w, uint32 h, BayerGrid bay){
/// \fn void bayer_to_rgb(uint8 *rgb)
///	\brief DWT picture transform.
///	\param	rgb 	The pointer to rgb array.

/*
   All RGB cameras use one of these Bayer grids:

	BGGR  0         GRBG 1          GBRG  2         RGGB 3
	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5
	0 B G B G B G	0 G R G R G R	0 G B G B G B	0 R G R G R G
	1 G R G R G R	1 B G B G B G	1 R G R G R G	1 G B G B G B
	2 B G B G B G	2 G R G R G R	2 G B G B G B	2 R G R G R G
	3 G R G R G R	3 B G B G B G	3 R G R G R G	3 G B G B G B
 */
	uint32 x, y, wy, xwy, xwy3, y2, x2, a, b, h1 = h, w1 = w, yw, yw1;

	switch(bay){
		case(BGGR):{ a = 1; b = 1; break;}
		case(GRBG):{ a = 0; b = 1; break;}
		case(GBRG):{ a = 1; b = 0; break;}
		case(RGGB):{ a = 0; b = 0; break;}
	}

	for(y=0, yw=0, yw1=0 ; y < h1; y++, yw+=w, yw1+=w1){
		for(x=0; x < w1; x++){
			y2 = oe(a,y);
			x2 = oe(b,x);
			xwy = x + yw;
			wy 	= x + yw1;
			xwy3 = wy + wy + wy;
			rgb[xwy3    ] = y2 ? (x2 ?  lb(img[xwy    ]) : lb(img[xwy+1])) : (x2 ? lb(img[xwy+w]) : lb(img[xwy+w+1]));
			rgb[xwy3 + 1] = y2 ? (x2 ? (lb(img[xwy+w  ]) + lb(img[xwy+1]))>>1 :   (lb(img[xwy  ]) + lb(img[xwy+w+1]))>>1) :
					(x2 ? (lb(img[xwy+w+1]) + lb(img[xwy  ]))>>1 :   (lb(img[xwy+1]) + lb(img[xwy+w  ]))>>1);
			rgb[xwy3 + 2] = y2 ? (x2 ?  lb(img[xwy+w+1]) : lb(img[xwy+w])) : (x2 ? lb(img[xwy+1]) : lb(img[xwy    ]));
		}
	}
	return rgb;
}

/**	\brief Bilinear algorithm for bayer to RGB interpolation use 3 rows buffer.
    \param img	 	The input Bayer image.
 	\param rgb		The output RGB image.
 	\param buff		The temporary 3 rows buffer
	\param w		The image width.
	\param h		The image height.
	\param bay		The Bayer grids pattern.
	\param shift	The image shift for display.
	\retval			Output RGB image..
*/
uint8* utils_bayer_to_RGB24(int16 *img, uint8 *rgb, int16 *buff, uint32 w, uint32 h, BayerGrid bay, uint32 bpp){
/*
   All RGB cameras use one of these Bayer grids:

	BGGR  0         GRBG 1          GBRG  2         RGGB 3
	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5
	0 B G B G B G	0 G R G R G R	0 G B G B G B	0 R G R G R G
	1 G R G R G R	1 B G B G B G	1 R G R G R G	1 G B G B G B
	2 B G B G B G	2 G R G R G R	2 G B G B G B	2 R G R G R G
	3 G R G R G R	3 B G B G B G	3 R G R G R G	3 G B G B G B
 */
	int x, x1, x2, xs, ys, y = 0, wy, xwy3, w2 = w<<1, yw = 0, h1, w1, h2, shift = 1<<(bpp-1);;
	int16 *l0, *l1, *l2, *tm;
	l0 = buff; l1 = &buff[w+2]; l2 = &buff[(w+2)<<1];

	switch(bay){
		case(BGGR):{ x = 1; y = 1; w1 = w+1; h1 = h+1; break;}
		case(GRBG):{ x = 1; y = 0; w1 = w+1; h1 = h; break;}
		case(GBRG):{ x = 0; y = 1; w1 = w; h1 = h+1; break;}
		case(RGGB):{ x = 0; y = 0; w1 = w; h1 = h;   break;}
	}
	h2 = h1-1;
	//Create 3 rows buffer for transform
	l0[0] = img[w+1]; for(x=0; x < w; x++) l0[x+1] = img[w+x];  l0[w+1] = l0[w-1];
	l1[0] = img[1];   for(x=0; x < w; x++) l1[x+1] = img[x];    l1[w+1] = l1[w-1];

	for(y=ys, yw=0; y < h1; y++, yw+=w){
		wy = (y == h2) ? yw - w : yw + w;
		l2[0] = img[wy+1]; for(x=0; x < w; x++) l2[x+1] = img[wy + x];  l2[w+1] = l2[w-1];

		for(x=xs, x1=0; x < w1; x++, x1++){
			wy 	= x1 + yw;
			x2 = x1 + 1;
			xwy3 = wy + wy + wy;

			if(!(y&1) && !(x&1)){
				rgb[xwy3] 	= 	lb1(l1[x2] + shift);
				rgb[xwy3+1] = 	lb1(((l0[x2] + l2[x2] + l1[x2-1] + l1[x2+1])>>2) + shift);
				rgb[xwy3+2] = 	lb1(((l0[x2+1] + l2[x2-1] + l0[x2-1] + l2[x2+1])>>2) + shift);
			}else if (!(y&1) && (x&1)){
				rgb[xwy3] = 	lb1(((l1[x2-1] + l1[x2+1])>>1) + shift);
				rgb[xwy3+1] = 	lb1(l1[x2] + shift);
				rgb[xwy3+2] =	lb1(((l0[x2] + l2[x2])>>1) + shift);
			}else if ((y&1) && !(x&1)){
				rgb[xwy3] = 	lb1(((l0[x2] + l2[x2])>>1) + shift);
				rgb[xwy3+1] = 	lb1(l1[x2] + shift);
				rgb[xwy3+2] =	lb1(((l1[x2-1] + l1[x2+1])>>1) + shift);
			}else {
				rgb[xwy3] = 	lb1(((l0[x2+1] + l2[x2-1] + l0[x2-1] + l2[x2+1])>>2) + shift);
				rgb[xwy3+1] = 	lb1(((l0[x2] + l2[x2] + l1[x2-1] + l1[x2+1])>>2) + shift);
				rgb[xwy3+2] = 	lb1(l1[x2] + shift);
			}
		}
		tm = l0; l0 = l1; l1 = l2; l2 = tm;
	}
	return rgb;
}

/**	\brief Bilinear algorithm for bayer to 3 image R, G, B interpolation use 3 rows buffer.
    \param img	 	The input Bayer image.
 	\param R		The output red image.
 	\param G		The output green image.
 	\param b		The output blue image.
 	\param buff		The temporary 3 rows buffer
	\param w		The image width.
	\param h		The image height.
	\param bay		The Bayer grids pattern.
	\param shift	The image shift for display.
	\retval			Output RGB image..
*/
void utils_bayer_to_RGB(int16 *img, int16 *R, int16 *G, int16 *B, int16 *buff, uint32 w, uint32 h, BayerGrid bay){
	int x, x1, x2, xs, ys, y = 0, wy, w2 = w<<1, yw = 0, h1, w1, h2;
	int16 *l0, *l1, *l2, *tm;
	l0 = buff; l1 = &buff[w+2]; l2 = &buff[(w+2)<<1];

	switch(bay){
		case(BGGR):{ x = 1; y = 1; w1 = w+1; h1 = h+1; break;}
		case(GRBG):{ x = 1; y = 0; w1 = w+1; h1 = h; break;}
		case(GBRG):{ x = 0; y = 1; w1 = w; h1 = h+1; break;}
		case(RGGB):{ x = 0; y = 0; w1 = w; h1 = h;   break;}
	}
	h2 = h1-1;
	//Create 3 rows buffer for transform
	l0[0] = img[w+1]; for(x=0; x < w; x++) l0[x+1] = img[w+x];  l0[w+1] = l0[w-1];
	l1[0] = img[1];   for(x=0; x < w; x++) l1[x+1] = img[x];    l1[w+1] = l1[w-1];

	for(y=ys, yw=0; y < h1; y++, yw+=w){
		wy = (y == h2) ? yw - w : yw + w;
		l2[0] = img[wy+1]; for(x=0; x < w; x++) l2[x+1] = img[wy + x];  l2[w+1] = l2[w-1];

		for(x=xs, x1=0; x < w1; x++, x1++){
			wy 	= x1 + yw;
			x2 = x1 + 1;
			if(!(y&1) && !(x&1)){
				R[wy] = l1[x2];
				G[wy] = (l0[x2] + l2[x2] + l1[x2-1] + l1[x2+1])>>2;
				B[wy] = (l0[x2+1] + l2[x2-1] + l0[x2-1] + l2[x2+1])>>2;
			}else if (!(y&1) && (x&1)){
				R[wy] = (l1[x2-1] + l1[x2+1])>>1;
				G[wy] = l1[x2];
				B[wy] =	(l0[x2] + l2[x2])>>1;
			}else if ((y&1) && !(x&1)){
				R[wy] = (l0[x2] + l2[x2])>>1;
				G[wy] = l1[x2];
				B[wy] =	(l1[x2-1] + l1[x2+1])>>1;
			}else {
				R[wy] = (l0[x2+1] + l2[x2-1] + l0[x2-1] + l2[x2+1])>>2;
				G[wy] = (l0[x2] + l2[x2] + l1[x2-1] + l1[x2+1])>>2;
				B[wy] = l1[x2];
			}
			//if(x1 < 10) printf("%3d ", R[wy]);
		}
		//printf("\n");
		tm = l0; l0 = l1; l1 = l2; l2 = tm;
	}
}

/**	\brief Transform bayer image to YUV444 format.
    \param img	 	The input Bayer image.
    \param Y		The output Y image.
	\param U		The output U image.
	\param V		The output V image.
 	\param buff		The temporary 3 rows buffer.
	\param w		The image width.
	\param h		The image height.
	\param bay		The Bayer grids pattern.
*/
void utils_bayer_to_YUV444(int16 *img, int16 *Y, int16 *U, int16 *V, int16 *buff, uint32 w, uint32 h, BayerGrid bay){
/*
   All RGB cameras use one of these Bayer grids:

	BGGR  0         GRBG 1          GBRG  2         RGGB 3
	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5
	0 B G B G B G	0 G R G R G R	0 G B G B G B	0 R G R G R G
	1 G R G R G R	1 B G B G B G	1 R G R G R G	1 G B G B G B
	2 B G B G B G	2 G R G R G R	2 G B G B G B	2 R G R G R G
	3 G R G R G R	3 B G B G B G	3 R G R G R G	3 G B G B G B
 */
	int x, x1, x2, xs, ys, y = 0, wy, w2 = w<<1, yw = 0, h1, w1, h2;
	int16 *l0, *l1, *l2, *tm;
	int r, g, b;
	l0 = buff; l1 = &buff[w+2]; l2 = &buff[(w+2)<<1];

	switch(bay){
		case(BGGR):{ xs = 1; ys = 1; w1 = w+1; h1 = h+1; break;}
		case(GRBG):{ xs = 1; ys = 0; w1 = w+1; h1 = h; break;}
		case(GBRG):{ xs = 0; ys = 1; w1 = w; h1 = h+1; break;}
		case(RGGB):{ xs = 0; ys = 0; w1 = w; h1 = h;   break;}
	}
	h2 = h1-1;
	//Create 3 rows buffer for transform

	l0[0] = img[w+1]; for(x=0; x < w; x++) l0[x+1] = img[w+x];  l0[w+1] = l0[w-1];
	l1[0] = img[1];   for(x=0; x < w; x++) l1[x+1] = img[x];    l1[w+1] = l1[w-1];

	for(y=ys, yw=0; y < h1; y++, yw+=w){
		wy = (y == h2) ? yw - w : yw + w;
		l2[0] = img[wy+1]; for(x=0; x < w; x++) l2[x+1] = img[wy + x];  l2[w+1] = l2[w-1];

		for(x=xs, x1=0; x < w1; x++, x1++){
			wy = x1 + yw;
			x2 = x1 + 1;
			//xwy3 = wy + wy + wy;

			if(!(y&1) && !(x&1)){
				r = l1[x2];
				g = (l0[x2] + l2[x2] + l1[x2-1] + l1[x2+1])>>2;
				b = (l0[x2+1] + l2[x2-1] + l0[x2-1] + l2[x2+1])>>2;
			}else if (!(y&1) && (x&1)){
				r = (l1[x2-1] + l1[x2+1])>>1;
				g = l1[x2];
				b =	(l0[x2] + l2[x2])>>1;
			}else if ((y&1) && !(x&1)){
				r = (l0[x2] + l2[x2])>>1;
				g = l1[x2];
				b =	(l1[x2-1] + l1[x2+1])>>1;
			}else {
				r = (l0[x2+1] + l2[x2-1] + l0[x2-1] + l2[x2+1])>>2;
				g = (l0[x2] + l2[x2] + l1[x2-1] + l1[x2+1])>>2;
				b = l1[x2];
			}
			Y[wy] = ((306*(r - g) + 117*(b - g))>>10) + g;
			U[wy] = 578*(b - Y[wy])>>10;
			V[wy] = 730*(r - Y[wy])>>10;
		}
		tm = l0; l0 = l1; l1 = l2; l2 = tm;
	}
}

/**	\brief Transform bayer image to YUV420 format.
    \param img	 	The input Bayer image.
    \param Y		The output Y image.
	\param U		The output U image.
	\param V		The output V image.
 	\param buff		The temporary 3 rows buffer.
	\param w		The image width.
	\param h		The image height.
	\param bay		The Bayer grids pattern.
*/

void utils_bayer_to_YUV420(int16 *img, int16 *Y, int16 *U, int16 *V, int16 *buff, uint32 w, uint32 h, BayerGrid bay){
/*
   All RGB cameras use one of these Bayer grids:

	BGGR  0         GRBG 1          GBRG  2         RGGB 3
	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5
	0 B G B G B G	0 G R G R G R	0 G B G B G B	0 R G R G R G
	1 G R G R G R	1 B G B G B G	1 R G R G R G	1 G B G B G B
	2 B G B G B G	2 G R G R G R	2 G B G B G B	2 R G R G R G
	3 G R G R G R	3 B G B G B G	3 R G R G R G	3 G B G B G B
 */
	int x, x1, x2, xs, ys, y = 0, y1, wy, wy1, w2 = w<<1,  w3 = w>>1, yw, h1, w1, h2;
	int16 *l0, *l1, *l2, *tm;
	int r, g, b;
	l0 = buff; l1 = &buff[w+2]; l2 = &buff[(w+2)<<1];

	switch(bay){
		case(BGGR):{ xs = 1; ys = 1; w1 = w+1; h1 = h+1; break;}
		case(GRBG):{ xs = 1; ys = 0; w1 = w+1; h1 = h; break;}
		case(GBRG):{ xs = 0; ys = 1; w1 = w; h1 = h+1; break;}
		case(RGGB):{ xs = 0; ys = 0; w1 = w; h1 = h;   break;}
	}
	h2 = h1-1;
	//Create 3 rows buffer for transform

	l0[0] = img[w+1]; for(x=0; x < w; x++) l0[x+1] = img[w+x];  l0[w+1] = l0[w-1];
	l1[0] = img[1];   for(x=0; x < w; x++) l1[x+1] = img[x];    l1[w+1] = l1[w-1];

	for(y=ys, y1=0; y < h1; y++, y1++){
		yw = y1*w;
		wy = (y == h2) ? yw - w : yw + w;
		l2[0] = img[wy+1]; for(x=0; x < w; x++) l2[x+1] = img[wy + x];  l2[w+1] = l2[w-1];

		for(x=xs, x1=0; x < w1; x++, x1++){
			wy = x1 + yw;
			x2 = x1 + 1;
			wy1 = (x1>>1) + (y1>>1)*w3;

			if(!(y&1) && !(x&1)){
				r = l1[x2];
				g = (l0[x2] + l2[x2] + l1[x2-1] + l1[x2+1])>>2;
				b = (l0[x2+1] + l2[x2-1] + l0[x2-1] + l2[x2+1])>>2;
				Y[wy] = ((306*(r - g) + 117*(b - g))>>10) + g;
				V[wy1] = 730*(r - Y[wy])>>10;
			}else if (!(y&1) && (x&1)){
				r = (l1[x2-1] + l1[x2+1])>>1;
				g = l1[x2];
				b =	(l0[x2] + l2[x2])>>1;
				Y[wy] = ((306*(r - g) + 117*(b - g))>>10) + g;
			}else if ((y&1) && !(x&1)){
				r = (l0[x2] + l2[x2])>>1;
				g = l1[x2];
				b =	(l1[x2-1] + l1[x2+1])>>1;
				Y[wy] = ((306*(r - g) + 117*(b - g))>>10) + g;
			}else {
				r = (l0[x2+1] + l2[x2-1] + l0[x2-1] + l2[x2+1])>>2;
				g = (l0[x2] + l2[x2] + l1[x2-1] + l1[x2+1])>>2;
				b = l1[x2];
				Y[wy] = ((306*(r - g) + 117*(b - g))>>10) + g;
				U[wy1] = 578*(b - Y[wy])>>10;
			}
		}
		tm = l0; l0 = l1; l1 = l2; l2 = tm;
	}
}

/**	\brief Simple gradient method of bayer interpolation algorithm.
    \param img	 	The input Bayer image.
 	\param rgb		The output RGB image.
	\param w		The image width.
	\param h		The image height.
	\retval			Output RGB image..
*/
uint8* utils_bayer_to_rgb_grad(int16 *img, uint8 *rgb, uint32 w, uint32 h, BayerGrid bay, int shift){
	//TODO: Work only for RGGB, need to make for all Bayer grids
	uint32 x, y, wy, xwy, xwy3, y2, x2, a, b, yw, w2 = w<<1;

	for(y=2, yw=w2; y < h-2; y++, yw+=w){
		for(x=2; x < w-2; x++){
			//y2 = oe(a,y);
			//x2 = oe(b,x);
			wy 	= x + yw;
			xwy3 = wy + wy + wy;
			if(!(y&1) && !(x&1)){
				rgb[xwy3] 	= 	lb1(img[wy] + shift);
				rgb[xwy3+1] = 	lb1((abs(img[wy-1] - img[wy+1]) > abs(img[wy-w] - img[wy+w]) ?
								(img[wy-w] + img[wy+w])>>1 : (img[wy-1] + img[wy+1])>>1) + shift);
				rgb[xwy3+2] = 	lb1((abs(img[wy-1-w] - img[wy+1+w]) > abs(img[wy+1-w] - img[wy-1+w]) ?
								(img[wy+1-w] + img[wy-1+w])>>1 : (img[wy-1-w] + img[wy+1+w])>>1) + shift);
			}else if (!(y&1) && (x&1)){
				rgb[xwy3] = 	lb1((abs(img[wy-1] - img[wy+1]) > abs(img[wy-w2-1] + img[wy-w2+1] - img[wy+w2-1] - img[wy+w2+1])>>1 ?
								(img[wy-w2-1] + img[wy-w2+1] + img[wy+w2-1] + img[wy+w2+1])>>2 : (img[wy-1] + img[wy+1])>>1) + shift);
				rgb[xwy3+1] = 	lb1(img[wy] + shift);
				rgb[xwy3+2] =	lb1((abs(img[wy-w] - img[wy+w]) > abs(img[wy-w-2] + img[wy+w-2] - img[wy-w+2] - img[wy+w+2])>>1 ?
								(img[wy-w-2] + img[wy+w-2] + img[wy-w+2] + img[wy+w+2])>>2 : (img[wy-w] + img[wy+w])>>1) + shift);
			}else if ((y&1) && !(x&1)){
				rgb[xwy3] = 	lb1((abs(img[wy-w] - img[wy+w]) > abs(img[wy-w-2] + img[wy+w-2] - img[wy-w+2] - img[wy+w+2])>>1 ?
								(img[wy-w-2] + img[wy+w-2] + img[wy-w+2] + img[wy+w+2])>>2 : (img[wy-w] + img[wy+w])>>1) + shift);
				rgb[xwy3+1] = 	lb1(img[wy] + shift);
				rgb[xwy3+2] =	lb1((abs(img[wy-1] - img[wy+1]) > abs(img[wy-w2-1] + img[wy-w2+1] - img[wy+w2-1] - img[wy+w2+1])>>1 ?
								(img[wy-w2-1] + img[wy-w2+1] + img[wy+w2-1] + img[wy+w2+1])>>2 : (img[wy-1] + img[wy+1])>>1) + shift);
			}else {
				rgb[xwy3] = 	lb1((abs(img[wy-1-w] - img[wy+1+w]) > abs(img[wy+1-w] - img[wy-1+w]) ?
								(img[wy+1-w] + img[wy-1+w])>>1 : (img[wy-1-w] + img[wy+1+w])>>1) + shift);
				rgb[xwy3+1] = 	lb1((abs(img[wy-1] - img[wy+1]) > abs(img[wy-w] - img[wy+w]) ?
								(img[wy-w] + img[wy+w])>>1 : (img[wy-1] + img[wy+1])>>1) + shift);
				rgb[xwy3+2] = 	lb1(img[wy] + shift);
			}
		}
	}
	return rgb;
}

/** \brief Convert RGB24 image to YUV444.
	\param rgb 	The input RGB image.
    \param y	The output Y image.
	\param u	The output U image.
	\param v	The output V image.
    \param w	The image width.
    \param h	The image height.
    \param pad		If pad = 1 need to pad each line to uint32 boundary, if pad = 0 no need
*/
void utils_RGB24_to_YUV444(uint8 *rgb, int16 *Y, int16 *U, int16 *V, uint32 w, uint32 h, uint32 bpp, uint32 pad)
{
	/*
	Y = 0.299*R + 0.587*G + 0.114*B
	U = -0.169*R – 0.331*G + 0.5*B
	V = 0.5*R - 0.419*G - 0.081*B
	*/
	int i, i3, sz = w*h, shift = 1<<(bpp-1);
	uint32 x, x1, y, yw, yw1 , yx3, yx1, w1 = (((w*3)>>2) + ((w*3)&3 ? 1 : 0))<<2;

	if(!pad){
		for(i=0; i < sz; i++){
			i3 = i*3;
			Y[i] = ((306*(rgb[i3]-rgb[i3 + 1]) + 117*(rgb[i3 + 2]-rgb[i3 + 1]))>>10) + rgb[i3 + 1] - shift;
			U[i] = 578*(rgb[i3 + 2]-Y[i]-shift)>>10;
			V[i] = 730*(rgb[i3]-Y[i]-shift)>>10;
		}
	} else {
		//For each line padded to a uint32 boundary.
		//printf("w = %d w1 = %d\n", w*3, w1);
		for(y=0; y < h; y++) {
			yw = y*w1;
			yw1 = y*w;
			for(x=0, x1 = 0; x < w1; x+=3, x1++) {
				yx3 = x+yw;
				yx1 = yw1 + x1;
				Y[yx1] = ((306*(rgb[yx3]-rgb[yx3 + 1]) + 117*(rgb[yx3 + 2]-rgb[yx3 + 1]))>>10) + rgb[yx3 + 1] - shift;
				U[yx1] = 578*(rgb[yx3 + 2]-Y[yx1]-shift)>>10;
				V[yx1] = 730*(rgb[yx3]-Y[yx1]-shift)>>10;
			}
		}
	}
	//y[i] = ((306*rgb[i3] + 601*rgb[i3 + 1] + 117*rgb[i3 + 2])>>10) - 128;
	//u[i] = (rgb[i3 + 2]>>1) - ((173*rgb[i3] + 339*rgb[i3 + 1])>>10);
	//v[i] = (rgb[i3    ]>>1) - ((429*rgb[i3 + 1] + 83*rgb[i3 + 2])>>10);
	//printf("%d %d %d  ", y[i], u[i], v[i]);
}

/** \brief Convert RGB24 image to YUV420.
	\param rgb 	The input RGB image.
    \param y	The output Y image.
	\param u	The output U image.
	\param v	The output V image.
    \param w	The image width.
    \param h	The image height.
    \param pad		If pad = 1 need to pad each line to uint32 boundary, if pad = 0 no need
*/
void utils_RGB24_to_YUV420(uint8 *rgb, int16 *Y, int16 *U, int16 *V, uint32 w, uint32 h, uint32 bpp, uint32 pad)
{
	/*
	Y = 0.299*R + 0.587*G + 0.114*B
	U = -0.169*R – 0.331*G + 0.5*B
	V = 0.5*R - 0.419*G - 0.081*B
	*/
	int y, x, yw, yx, yx2, yx3, w2 = w>>1, shift = 1<<(bpp-1);
	uint32  x1, yw1 , yx1, w1 = (((w*3)>>2) + ((w*3)&3 ? 1 : 0))<<2;

	if(!pad){
		for(y=0; y < h; y++){
			yw = y*w;
			for(x=0; x < w; x++){
				yx = yw + x;
				yx3 = yx*3;
				yx2 = (x>>1) + (y>>1)*w2;

				Y[yx] = ((306*(rgb[yx3]-rgb[yx3 + 1]) + 117*(rgb[yx3 + 2]-rgb[yx3 + 1]))>>10) + rgb[yx3 + 1] - shift;
				U[yx2] += 578*(rgb[yx3 + 2]-Y[yx]-shift)>>10;
				V[yx2] += 730*(rgb[yx3]-Y[yx]-shift)>>10;
				if(x&1 && y&1) {
					U[yx2]>>=2;
					V[yx2]>>=2;
				}
			}
		}
	} else {
		//For each line padded to a uint32 boundary.
		//printf("w = %d w1 = %d\n", w*3, w1);
		for(y=0; y < h; y++) {
			yw = y*w1;
			yw1 = y*w;
			for(x=0, x1 = 0; x < w1; x+=3, x1++) {
				yx3 = x+yw;
				yx1 = yw1 + x1;
				yx2 = (x1>>1) + (y>>1)*w2;
				Y[yx1] = ((306*(rgb[yx3]-rgb[yx3 + 1]) + 117*(rgb[yx3 + 2]-rgb[yx3 + 1]))>>10) + rgb[yx3 + 1] - shift;
				U[yx2] += 578*(rgb[yx3 + 2]-Y[yx1]-shift)>>10;
				V[yx2] += 730*(rgb[yx3]-Y[yx1]-shift)>>10;
				if(x1&1 && y&1) {
					U[yx2]>>=2;
					V[yx2]>>=2;
				}
			}
		}
	}
	//y[i] = ((306*rgb[i3] + 601*rgb[i3 + 1] + 117*rgb[i3 + 2])>>10) - 128;
	//u[i] = (rgb[i3 + 2]>>1) - ((173*rgb[i3] + 339*rgb[i3 + 1])>>10);
	//v[i] = (rgb[i3    ]>>1) - ((429*rgb[i3 + 1] + 83*rgb[i3 + 2])>>10);
	//printf("%d %d %d  ", y[i], u[i], v[i]);
}

/** \brief Copy 24 bits rgb format image to three images R, G, B
	\param img	 	The input RGB24 image.
    \param r		The output red image.
    \param g		The output green image.
    \param b		The output blue image.
    \param w		The image width.
    \param h		The image height.
    \param bpp		The bits per pixel.
    \param pad		If pad = 1 need to pad each line to uint32 boundary, if pad = 0 no need
*/
void utils_RGB24_to_RGB(uint8 *img, int16 *r, int16 *g, int16 *b, uint32 w, uint32 h, uint32 bpp, uint32 pad)
{
	uint32 i, i3, size = w*h, shift = 1<<(bpp-1);
	uint32 x, x1, y, yw, yw1 , yx3, yx1, w1 = (((w*3)>>2) + ((w*3)&3 ? 1 : 0))<<2;
	if(!pad){
		for(i=0; i<size; i++) {
			i3 = i*3;
			r[i] = img[i3]   - shift;
			g[i] = img[i3+1] - shift;
			b[i] = img[i3+2] - shift;
		}
	} else {
		//For each line padded to a uint32 boundary.
		//printf("w = %d w1 = %d\n", w*3, w1);
		for(y=0; y < h; y++) {
			yw = y*w1;
			yw1 = y*w;
			for(x=0, x1 = 0; x < w1; x+=3, x1++) {
				yx3 = x+yw;
				yx1 = yw1 + x1;
				r[yx1] = img[yx3]   - shift;
				g[yx1] = img[yx3+1] - shift;
				b[yx1] = img[yx3+2] - shift;
			}
		}
	}
}

/** \brief Copy three images R, G, B to 24 bits rgb format.
	\param buff 	The output RGB24 image.
    \param r		The input red image.
    \param g		The input green image.
    \param b		The input blue image.
    \param w		The image width.
    \param h		The image height.
    \param bpp		The bits per pixel.
*/
void utils_RGB_to_RGB24(uint8 *img, int16 *r, int16 *g, int16 *b, uint32 w, uint32 h, uint32 bpp)
{
	uint32 i, i3, size = w*h, shift = 1<<(bpp-1);
	for(i=0; i<size; i++) {
		i3 = i*3;
		img[i3]   = lb1(r[i] + shift);
		img[i3+1] = lb1(g[i] + shift);
		img[i3+2] = lb1(b[i] + shift);
	}
}

/** \brief Convert YUV444 image to RGB.
	\param rgb 	The output RGB image.
    \param Y	The output Y image.
	\param U	The output U image.
	\param V	The output V image.
    \param w	The image width.
    \param h	The image height.
    \param bpp	The bits per pixel.
    \retval	rgb	The output RGB image.
*/
uint8* utils_YUV444_to_RGB24(uint8 *rgb, int16 *Y, int16 *U, int16 *V, uint32 w, uint32 h, uint32 bpp)
{
	/*
	R = Y + 1.4026 * V
	G = Y – 0.3444 * U – 0.7144 * V
	B = Y + 1.7730 * U
	*/
	int i, i3, sz = w*h, shift = 1<<(bpp-1);
	//int R, G, B;
	for(i=0; i < sz; i++){
		i3 = i*3;
		rgb[i3    ] = lb1(shift + Y[i] + ((1436*V[i])>>10));
		rgb[i3 + 1] = lb1(shift + Y[i] - ((732*V[i] + 353*U[i])>>10));
		rgb[i3 + 2] = lb1(shift + Y[i] + ((1816*U[i])>>10));

		//if(R < 0 || R > 255) printf("R=%d ",R);
		//if(G < 0 || G > 255) printf("G=%d y = %d u = %d v = %d ",G, y[i], u[i], v[i]);
		//if(B < 0 || B > 255) printf("v=%d ",B);
	}
	return rgb;
}

/** \brief Convert YUV420 image to RGB.
	\param rgb 	The output RGB image.
    \param y	The output Y image.
	\param u	The output U image.
	\param v	The output V image.
    \param w	The image width.
    \param h	The image height.
    \retval	rgb	The output RGB image.
*/
uint8* utils_YUV420_to_RGB24(uint8 *rgb, int16 *Y, int16 *U, int16 *V, uint32 w, uint32 h, uint32 bpp)
{
	/*
	R = Y + 1.4026 * V
	G = Y – 0.3444 * U – 0.7144 * V
	B = Y + 1.7730 * U
	*/
	int y, x, yw, yx, yx2, yx3, w2 = w>>1, shift = 1<<(bpp-1);;
	for(y=0; y < h; y++){
		yw = y*w;
		for(x=0; x < w; x++){
			yx = yw + x;
			yx3 = yx*3;
			yx2 = (x>>1) + (y>>1)*w2;

			rgb[yx3    ] = lb1(shift + Y[yx] + ((1436*V[yx2])>>10));
			rgb[yx3 + 1] = lb1(shift + Y[yx] - ((732*V[yx2] + 353*U[yx2])>>10));
			rgb[yx3 + 2] = lb1(shift + Y[yx] + ((1816*U[yx2])>>10));
		}
	}
	return rgb;
}

/** \brief Make new image with specular borders.
	\param img 	The input image.
    \param img1	The output image.
    \param w	The image width.
    \param h	The image height.
    \param bor	The border size.
    \retval	img1	The output image.
*/
int16* utils_specular_border(int16 *img, int16 *img1, uint32 w, uint32 h, uint32 bor)
{
	int y, x, yw, ywb, w1 = w + (bor<<1), h1 = h + (bor<<1);
	for(y=0; y < h; y++){
		yw = y*w;
		ywb = (y+bor)*w1 + bor;
		for(x=0; x < w; x++) img1[ywb + x] = img[yw + x];
		for(x=0; x < bor; x++) img1[ywb - bor + x] = img1[ywb + bor - x];
		for(x=w+bor; x < w1; x++) img1[ywb - bor + x] = img1[ywb + bor + (w<<1) - x - 2 ];
	}
	for(y=0; y < bor; y++){
		ywb = y*w1;
		yw = ((bor<<1)-y)*w1;
		for(x=0; x < w1; x++) img1[ywb + x] = img1[yw + x];
	}
	for(y=h+bor; y < h1; y++){
		ywb = y*w1;
		yw = (((h+bor)<<1) - y - 2 )*w1;
		for(x=0; x < w1; x++) img1[ywb + x] = img1[yw + x];
	}

	return img1;
}

/** \brief Copy image from the buffer
	\param buff 	The input buffer.
    \param img		The output image.
    \param w		The image width.
    \param h		The image height.
    \param bpp		The bits per pixel.
*/
void utils_image_copy(uint8 *buff, int16 *img, uint32 w, uint32 h, uint32 bpp)
{
	uint32 i, size = w*h, shift = 1<<(bpp-1);;
	//printf("Start copy  x = %d y = %d p = %p \n", im->w, im->h, im->p);
	if(bpp > 8) for(i=0; i<size; i++) img[i] = ((buff[i<<1]<<8) | buff[(i<<1)+1]) - shift;
	else 		for(i=0; i<size; i++) img[i] = buff[i] - shift;
}

uint8* utils_draw_scale_color(uint8 *rgb, uint8 *img,  uint32 w0, uint32 h0, uint32 w, uint32 h,   uint32 wp, BayerGrid bay){
/*! \fn void bayer_to_rgb(uint8 *rgb)
	\brief DWT picture transform.
  	\param	rgb 	The pointer to rgb array.
*/

/*
   All RGB cameras use one of these Bayer grids:

	BGGR  0         GRBG 1          GBRG  2         RGGB 3
	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5
	0 B G B G B G	0 G R G R G R	0 G B G B G B	0 R G R G R G
	1 G R G R G R	1 B G B G B G	1 R G R G R G	1 G B G B G B
	2 B G B G B G	2 G R G R G R	2 G B G B G B	2 R G R G R G
	3 G R G R G R	3 B G B G B G	3 R G R G R G	3 G B G B G B
 */
	uint32 y, x, yx, yw, yx3, i, sq = w*h - w, w1 = w-1, h1 = h-1;
	switch(bay){
		case RGGB :
			for(y=1; y < h1; y++){
				for(x=1; x < w1; x++){
					yx = y*w+x;
					yx3 = ((y+h0)*wp + x + w0)*3;
					rgb[yx3  ] = (!(x&1) && !(y&1)) ? img[yx] : ((x&1 && y&1) ? (img[yx-1-w] + img[yx+1-w] + img[yx-1+w] + img[yx+1+w])>>2 :
					(x&1 && !(y&1) ? (img[yx-1] + img[yx+1])>>1 : (img[yx-w] + img[yx+w])>>1));
					rgb[yx3+1] = ((!(x&1) && y&1) || ( x&1 && !(y&1))) ? img[yx] : (img[yx-1] + img[yx-w] + img[yx+1] + img[yx+w])>>2;
					rgb[yx3+2] = (x&1 && y&1) ? img[yx] : ((!(x&1) && !(y&1)) ? (img[yx-1-w] + img[yx+1-w] + img[yx-1+w] + img[yx+1+w])>>2 :
					(!(x&1) && y&1 ? (img[yx-1] + img[yx+1])>>1 : (img[yx-w] + img[yx+w])>>1));
				}
			}
	}
	return rgb;
}

void fill_bayer_hist(int16 *img, uint32 *r, uint32 *g, uint32 *b, uint32 w, uint32 h,  BayerGrid bay, uint32 bits){
//
//   All RGB cameras use one of these Bayer grids:
//
//	BGGR  0         GRBG 1          GBRG  2         RGGB 3
//	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5
//	0 B G B G B G	0 G R G R G R	0 G B G B G B	0 R G R G R G
//	1 G R G R G R	1 B G B G B G	1 R G R G R G	1 G B G B G B
//	2 B G B G B G	2 G R G R G R	2 G B G B G B	2 R G R G R G
//	3 G R G R G R	3 B G B G B G	3 R G R G R G	3 G B G B G B
//
	uint32 x, y, i, size = h*w;
	uint32 *c[4];
	memset(r, 0, sizeof r);
	memset(g, 0, sizeof g);
	memset(b, 0, sizeof b);

	switch(bay){
		case(BGGR):{ c[0] = r; c[1] = g; c[2] = g; c[3] = b; break;}
		case(GRBG):{ c[0] = g; c[1] = b; c[2] = r; c[3] = g; break;}
		case(GBRG):{ c[0] = g; c[1] = r; c[2] = b; c[3] = g; break;}
		case(RGGB):{ c[0] = b; c[1] = g; c[2] = g; c[3] = r; break;}
	}

	for(i=0, x=0, y=0; i < size; i++, x++){
		if(x == w) { x=0; y++;}
		if(y&1)
			if(x&1) c[0][img[i]]++;
			else 	c[1][img[i]]++;
		else
			if(x&1)	c[2][img[i]]++;
			else 	c[3][img[i]]++;
	}
}

uint8* utils_color_draw(uint8 *img, uint8 *rgb, uint32 w, uint32 h, uint32 col)
{
	int i, j, dim = h*w*3;
	if(col ==  0) for(i=0, j=0; j < dim; j+=3, i++) rgb[j  ] = img[i]; // Red
	if(col ==  1) for(i=0, j=0; j < dim; j+=3, i++) rgb[j+1] = img[i]; // Red
	if(col ==  2) for(i=0, j=0; j < dim; j+=3, i++) rgb[j+2] = img[i]; // Red
	return rgb;
}

uint8* utils_draw(uint8 *img, uint8 *rgb, uint32 w, uint32 h)
{
	int j, dim = h*w*3;
	for(j= 0; j < dim; j+=3){
		rgb[j]     = img[j];
		rgb[j+1] = img[j+1];
		rgb[j+2] = img[j+2];
		//printf("y_w[%d] = %4d\n",i,mod(yuv_buffer->y_w[i]));
	}
	return rgb;
}

uint8* utils_reg_draw(uint32 *img, uint8 *rgb, uint32 w, uint32 h)
{
	int i, j, dim = h*w*3;
	for(i = 0,  j= 0; j < dim; j+=3, i++){
		rgb[j]     = img[i]&0xFF;
		rgb[j + 1] = img[i]&0xFF;
		rgb[j + 2] = img[i]&0xFF;
		//printf("y_w[%d] = %4d\n",i,mod(yuv_buffer->y_w[i]));
	}
	return rgb;
}

uint8* utils_cat(uint8 *img, uint8 *img1, uint32 w, uint32 h, uint32 bits)
{
	int i, dim = h*w, sh = bits-8;
	for(i = 0; i < dim; i++) img1[i] = img[i]<0 ? 0 : rnd(img[i]>>sh);
	return img1;
}

uint8* utils_bayer_to_Y1(uint8 *img, uint8 *img1, uint32 w, uint32 h)
{
	uint32 x, y, wy, xwy, y2, x2, a, b, h1 = h-1, w1 = w-1, yw, yw1;

	for(y=0, yw=0, yw1=0 ; y < h1; y++, yw+=w, yw1+=w1){
		for(x=0; x < w1; x++){
			xwy = x + yw;
			wy = x + yw1;
			//xwy3 = wy + wy + wy;
			img1[wy] = 	(img[xwy ] + img[xwy+1] + img[xwy+w] + img[xwy+w+1])>>2;
		}
	}
	return img1;
}

uint8* utils_ppm_to_bayer(uint8 *img, uint8 *img1, uint32 w, uint32 h)
{
	uint32 x, y, yx, h1 = w*h, w1 = w<<1;

	for(y=0; y < h1; y+=w1) {
		for(x=0; x < w; x+=2){
			yx = y+x;
			//img1[yx] = 255;
			//img1[yx+1] = 255;
			//img1[yx+w] = 255;
			//img1[yx+w+1] = 255;
			img1[yx] = img[yx*3];
			img1[yx+1] = img[(yx+1)*3+1];
			img1[yx+w] = img[(yx+w)*3+1];
			img1[yx+w+1] = img[(yx+w+1)*3+2];
		}
	}
	return img1;
}

void utils_bayer_to_4color(uint8 *img, uint32 w, uint32 h, uint8 *p0, uint8 *p1, uint8 *p2, uint8 *p3)
{
	uint32 i = 0, x, y, yx, h1 = w*((h>>1)<<1), w1 = w<<1;
	//uint8 *p[4];
	//p[0] = img1; p[1] = &img1[h1>>2]; p[2] = &img1[h1>>1]; p[3] = &img1[(h1>>2)*3];

	for(y=0; y < h1; y+=w1) {
		for(x=0; x < w; x+=2){
			yx = y+x;
			//img1[yx] = 255;
			//img1[yx+1] = 255;
			//img1[yx+w] = 255;
			//img1[yx+w+1] = 255;
			p0[i] = img[yx];
			p1[i] = img[yx+1];
			p2[i] = img[yx+w];
			p3[i] = img[yx+w+1];
			i++;
		}
	}
}

uint8* utils_4color_draw(uint8 *img, uint8 *rgb, uint32 w, uint32 h, uint8 *p0, uint8 *p1, uint8 *p2, uint8 *p3)
{
	uint32 i=0, j=0, x, y, yx, h1 = w*((h>>1)<<1), w1 = w<<1;
	//uint8 *p[4];
	//p[0] = img; p[1] = &img[h1>>2]; p[2] = &img[h1>>1]; p[3] = &img[(h1>>2)*3];


	for(y=0; y < (h1>>1); y+=w) {
		for(x=0; x < (w>>1); x++){
			yx = y+x;
			rgb[yx*3]     = p0[i];
			rgb[yx*3 + 1] = p0[i];
			rgb[yx*3 + 2] = p0[i]; i++;
		}
		for(x=w>>1; x < w; x++){
			yx = y+x;
			rgb[yx*3]     = p1[j];
			rgb[yx*3 + 1] = p1[j];
			rgb[yx*3 + 2] = p1[j]; j++;
		}
	}
	i=0, j=0;
	for(y=(h1>>1); y < h1; y+=w) {
		for(x=0; x < (w>>1); x++){
			yx = y+x;
			rgb[yx*3]     = p2[i];
			rgb[yx*3 + 1] = p2[i];
			rgb[yx*3 + 2] = p2[i]; i++;
		}
		for(x=w>>1; x < w; x++){
			yx = y+x;
			rgb[yx*3]     = p3[j];
			rgb[yx*3 + 1] = p3[j];
			rgb[yx*3 + 2] = p3[j]; j++;
		}
	}
	return rgb;
}
/*
uint8* utils_4color_scale_draw(uint8 *rgb, uint32 w, uint32 h, Picture *p0,  Picture *p1,  Picture *p2,  Picture *p3)
{
	uint32 i=0, sx, sy;
	drawrect(rgb, p0[0].pic, 0			, 0 							, p0[0].width, p0[0].height, w, 0);
	drawrect(rgb, p0[1].pic, p0[0].width, 0 							, p0[1].width, p0[1].height, w, 0);
	drawrect(rgb, p0[2].pic, p0[0].width, p0[1].height				 	, p0[2].width, p0[2].height, w, 0);
	drawrect(rgb, p0[3].pic, p0[0].width, p0[1].height+p0[2].height 	, p0[3].width, p0[3].height, w, 0);

	sx = p0[0].width + p0[1].width;
	drawrect(rgb, p1[0].pic, sx			, 0 							, p1[0].width, p1[0].height, w, 0);
	drawrect(rgb, p1[1].pic, p1[0].width+sx, 0 							, p1[1].width, p1[1].height, w, 0);
	drawrect(rgb, p1[2].pic, p1[0].width+sx, p1[1].height				, p1[2].width, p1[2].height, w, 0);
	drawrect(rgb, p1[3].pic, p1[0].width+sx, p1[1].height+p1[2].height 	, p1[3].width, p1[3].height, w, 0);

	sy = p0[0].height;
	drawrect(rgb, p1[0].pic, 0			, sy 							, p1[0].width, p1[0].height, w, 0);
	drawrect(rgb, p1[1].pic, p1[0].width, sy 							, p1[1].width, p1[1].height, w, 0);
	drawrect(rgb, p1[2].pic, p1[0].width, p1[1].height+sy				, p1[2].width, p1[2].height, w, 0);
	drawrect(rgb, p1[3].pic, p1[0].width, p1[1].height+p1[2].height+sy 	, p1[3].width, p1[3].height, w, 0);

	drawrect(rgb, p1[0].pic, sx			, sy 							, p1[0].width, p1[0].height, w, 0);
	drawrect(rgb, p1[1].pic, p1[0].width+sx, sy 							, p1[1].width, p1[1].height, w, 0);
	drawrect(rgb, p1[2].pic, p1[0].width+sx, p1[1].height+sy				, p1[2].width, p1[2].height, w, 0);
	drawrect(rgb, p1[3].pic, p1[0].width+sx, p1[1].height+p1[2].height+sy 	, p1[3].width, p1[3].height, w, 0);

	return rgb;
}

uint8* utils_scale_draw(uint8 *rgb, uint32 w, uint32 h, Picture *p)
{
	uint32 i=0, sx, sy;
	drawrect(rgb, p[0].pic, 0		  , 0 							, p[0].width, p[0].height, w, 0);
	drawrect(rgb, p[1].pic, p[0].width, 0 							, p[1].width, p[1].height, w, 0);
	drawrect(rgb, p[2].pic, p[0].width, p[1].height				 	, p[2].width, p[2].height, w, 0);
	drawrect(rgb, p[3].pic, p[0].width, p[1].height+p[2].height 	, p[3].width, p[3].height, w, 0);

	return rgb;
}
*/
uint8* utils_rgb_scale_draw(uint8 *rgb, uint32 w, uint32 h, Pic8u *p)
{

	drawrect_rgb(rgb, p[0].pic, 0		  , 0 						, p[0].w, p[0].h, w);
	drawrect_rgb(rgb, p[1].pic, p[0].w, 0 						, p[1].w, p[1].h, w);
	drawrect_rgb(rgb, p[2].pic, p[0].w, p[1].h				, p[2].w, p[2].h, w);
	drawrect_rgb(rgb, p[3].pic, p[0].w, p[1].h+p[2].h , p[3].w, p[3].h, w);

	return rgb;
}

uint8* utils_color_scale_draw(uint8 *rgb, uint32 w, uint32 h, Pic8u *p)
{

	utils_draw_scale_color(rgb, p[0].pic, 0		  , 0 							, p[0].w, p[0].h, w, 3);
	utils_draw_scale_color(rgb, p[1].pic, p[0].w, 0 						, p[1].w, p[1].h, w, 3);
	utils_draw_scale_color(rgb, p[2].pic, p[0].w, p[1].h				, p[2].w, p[2].h, w, 3);
	utils_draw_scale_color(rgb, p[3].pic, p[0].w, p[1].h+p[2].h	, p[3].w, p[3].h, w, 3);

	return rgb;
}


void utils_resize_2x(uint8 *img, uint8 *img1, uint32 w, uint32 h)
{
	uint32 x, y, yx, h1 = ((h>>1)<<1)*w, w2 = w<<1, w1 = ((w>>1)<<1), i=0;
	for(y=0; y < h1; y+=w2){
		for(x=0; x < w1; x+=2){
			yx = y + x;
			img1[i++] = (img[yx] + img[yx+1] + img[yx+w] + img[yx+w+1])>>2;
		}
	}
}

/** \brief Resize bayer image down to two times on each axis.
    \param img 	The input image.
    \param img1	The output image.
    \param w	The image width.
    \param h	The image height.
*/
void utils_resize_bayer_2x(int16 *img, int16 *img1, uint32 w, uint32 h)
{
	uint32 x, y, yx, yx1, yx2, h1 = ((h>>2)<<2)*w, w2 = w<<2, w1 = ((w>>2)<<2), wn = w>>1, i=0;
	for(y=0; y < h1; y+=w2){
		for(x=0; x < w1; x+=4){
			yx = y + x;
			yx1 = (y>>2) + (x>>1);
			yx2 = yx+w2;
			img1[yx1] 		= (img[yx] 		+ img[yx+2] 	+ img[yx+w2] 		+ img[yx+w2+2])>>2;
			img1[yx1+1] 	= (img[yx+1] 	+ img[yx+3] 	+ img[yx+w2+1] 		+ img[yx+w2+3])>>2;
			img1[yx1+wn] 	= (img[yx+w] 	+ img[yx+2+w] 	+ img[yx+w2+w] 		+ img[yx+w2+2+w])>>2;
			img1[yx1+wn+1]	= (img[yx+w+1] 	+ img[yx+3+w]	+ img[yx+w2+w+1]	+ img[yx+w2+3+w])>>2;
			//if(yx1 >= wn*(h>>1)) printf(" ind = %d\n", yx1);
		}
		if((w>>1)&1){
			yx = y + x;
			yx1 = (y>>2) + (x>>1);
			img1[yx1] 		= (img[yx] 		+ img[yx+w2] 	)>>1;
			img1[yx1+1] 	= (img[yx+1] 	+ img[yx+w2+1] 	)>>1;
			img1[yx1+wn] 	= (img[yx+w] 	+ img[yx+w2+w] 	)>>1;
			img1[yx1+wn+1]	= (img[yx+w+1] 	+ img[yx+w2+w+1])>>1;
		}
	}
	if((h>>1)&1){
		for(x=0; x < w1; x+=4){
			yx = y + x;
			yx1 = (y>>2) + (x>>1);
			img1[yx1] 		= (img[yx] 		+ img[yx+2]  )>>1;
			img1[yx1+1] 	= (img[yx+1] 	+ img[yx+3]  )>>1;
			img1[yx1+wn] 	= (img[yx+w] 	+ img[yx+2+w])>>1;
			img1[yx1+wn+1]	= (img[yx+w+1] 	+ img[yx+3+w])>>1;
			//if(yx1 >= wn*(h>>1)) printf(" ind = %d\n", yx1);
		}
		if((w>>1)&1){
			yx = y + x;
			yx1 = (y>>2) + (x>>1);
			img1[yx1] 		= img[yx]    ;
			img1[yx1+1] 	= img[yx+1]  ;
			img1[yx1+wn] 	= img[yx+w]  ;
			img1[yx1+wn+1]	= img[yx+w+1];
		}
	}
}

void utils_resize_bayer_2x_new(int16 *img, int16 *img1, uint32 w, uint32 h)
{
	uint32 x, y, yx, yx1, yx2, h1 = ((h>>2)<<2)*w, w2 = w<<2, w1 = ((w>>2)<<2), wn = w>>1, i=0;
	for(y=0; y < h1; y+=w2){
		for(x=0; x < w1; x+=4){
			yx = y + x;
			yx1 = (y>>2) + (x>>1);
			img1[yx1] 		= ((img[yx]<<1) + img[yx+1+w] + ((img[yx+2] + img[yx+(w<<1)])>>1))>>2;
			yx2 = yx+3;
			img1[yx1+1] 	= ((img[yx2]<<1) + ((img[yx2-2] + img[yx2+(w<<1)])>>1) + (img[yx2] + img[yx2-2] + img[yx2+(w<<1)] + img[yx2+(w<<1)-2])>>2)>>2;
			yx2 = yx+3*w;
			img1[yx1+wn] 	= ((img[yx2]<<1) + ((img[yx2+2] + img[yx2+(w<<1)])>>1) + (img[yx2] + img[yx2+2] + img[yx2+(w<<1)] + img[yx2+(w<<1)+2])>>2)>>2;
			yx2 = yx+2+(w<<1);
			img1[yx1+wn+1]	= ((img[yx2]<<1) + img[yx2+1+w] + ((img[yx2+2] + img[yx2+(w<<1)])>>1))>>2;
			//if(yx1 >= wn*(h>>1)) printf(" ind = %d\n", yx1);
		}
	}
}

void utils_resize_rgb_2x(uint8 *img, uint8 *img1, uint32 w, uint32 h)
{
	uint32 x, y, yx, h1 = ((h>>1)<<1)*w*3, w2 = (w<<1)*3, w1 = ((w>>1)<<1)*3, i=0;
	for(y=0; y < h1; y+=w2){
		for(x=0; x < w1; x+=6){
			yx = y + x;
			img1[i++] = img[yx];
			img1[i++] =	img[yx+1];
			img1[i++] =	img[yx+2];
		}
	}
}

void utils_bayer_to_rgb(uint8 *img, uint8 *rgb, uint32 w, uint32 h)
{
	uint32 x, y, yx, yx3, h1 = ((h>>1)<<1)*w, w2 = w<<1, w1 = ((w>>1)<<1), wn = w>>1, i=0;
	for(y=0; y < h1; y+=w2){
		for(x=0; x < w1; x+=2){
			yx = y + x;
			yx3 = ((y>>2) + (x>>1))*3;
			rgb[yx3] 	= img[yx];
			rgb[yx3+1] 	= (img[yx+1] 	+ img[yx+w])>>1;
			rgb[yx3+2] 	= img[yx+w+1];
		}
	}
}

void utils_bayer_to_Y(uint8 *img, uint8 *img1, uint32 w, uint32 h)
{
	uint32 x, x1, y, y1, yx, yx1, h1 = ((h>>1)<<1)*w, w2 = w<<1, w1 = ((w>>1)<<1), w3 = w>>1;
	for(y=0, y1=0; y < h1; y+=w2, y1+=w3){
		for(x=0, x1=0; x < w1; x+=2, x1++){
			yx = y + x;
			yx1 = y1 + x1;
			img1[yx1] 	= (img[yx] + img[yx+1] + img[yx+w] + img[yx+w+1])>>2;
		}
	}
}

uint8* utils_bayer_to_gradient(uint8 *img, uint8 *img1, uint32 w, uint32 h, BayerGrid bay, uint32 thresh)
{
	uint32 x, y, wy, xwy, y2, x2, a, b, h1 = h-1, w1 = w-1, yw, yw1;

    switch(bay){
            case(BGGR) : { a = 1; b = 1; break; }
            case(GRBG) : { a = 0; b = 1; break; }
            case(GBRG) : { a = 1; b = 0; break; }
            case(RGGB) : { a = 0; b = 0; break; }
    }

//	for(y=0, yw=0, yw1=0 ; y < h1; y++, yw+=w, yw1+=w1){
//		for(x=0; x < w1; x++){
	for(y=1, yw=w, yw1=w1 ; y < h1-1; y++, yw+=w, yw1+=w1){
		for(x=1; x < w1-1; x++){
		//for(y=1, yw=w, yw1=w1 ; y < 2; y++, yw+=w, yw1+=w1){
		//	for(x=1; x < 2; x++){
			y2 = oe(a,y);
			x2 = oe(b,x);
			xwy = x + yw;
			wy = (x + yw1);
			//printf("y2 = %d %d x2 = %d %d\n", y2,!y2, x2, !x2);
			//printf("xwy+w+1 = %d xwy-(w<<1) = %d xwy = %d\n", xwy+w+1, xwy-(w<<1), xwy);

			//Green
            img1[wy] = y2 ? (x2 ? abs(img[xwy+w  ] - img[xwy+1  ]) : abs(img[xwy    ] - img[xwy+w+1])) :
                            (x2 ? abs(img[xwy+w+1] - img[xwy    ]) : abs(img[xwy+1  ] - img[xwy+w  ])) ;
            //Red
			img1[wy] += y2 ? (x2 ? abs(img[xwy  ]   - ((img[xwy]     + img[xwy+2]   + img[xwy+(w<<1)]   + img[xwy+(w<<1)+2])>>2)) :
								   abs(img[xwy+1]   - ((img[xwy+1]   + img[xwy-1]   + img[xwy+(w<<1)+1] + img[xwy+(w<<1)-1])>>2))):
							 (x2 ? abs(img[xwy+w]   - ((img[xwy+w]   + img[xwy+2+w] + img[xwy-w]        + img[xwy-w+2])>>2)) :
								   abs(img[xwy+w+1] - ((img[xwy+w+1] + img[xwy-1+w] + img[xwy-w+1]      + img[xwy-w-1])>>2)));
			//Blue
			img1[wy] += !y2 ? (!x2 ? abs(img[xwy  ]   - ((img[xwy]     + img[xwy+2]   + img[xwy+(w<<1)]   + img[xwy+(w<<1)+2])>>2)) :
								     abs(img[xwy+1]   - ((img[xwy+1]   + img[xwy-1]   + img[xwy+(w<<1)+1] + img[xwy+(w<<1)-1])>>2))):
							  (!x2 ? abs(img[xwy+w]   - ((img[xwy+w]   + img[xwy+2+w] + img[xwy-w]        + img[xwy-w+2])>>2)) :
								     abs(img[xwy+w+1] - ((img[xwy+w+1] + img[xwy-1+w] + img[xwy-w+1]      + img[xwy-w-1])>>2)));
			img1[wy] = img1[wy] > thresh ? img1[wy] : 0;
 		}
	}
	return img1;
}

uint8* utils_bayer_gradient(uint8 *img, uint8 *img1, uint32 w, uint32 h, BayerGrid bay, uint32 thresh)
{
	uint32 x, y, yx, h1 = (h-1)*w, w1 = w-1, yw;

	for(y=w ; y < h1; y+=w){
		for(x=1; x < w1; x++){
			yx = y + x;

			img1[yx] =   abs(img[yx-1] + img[yx-w] + img[yx-w-1] - img[yx+1] - img[yx+w] - img[yx+w+1])>>2;
			img1[yx] +=  abs(img[yx+1] + img[yx-w] + img[yx-w+1] - img[yx-1] - img[yx+w] - img[yx+w-1])>>2;
			//img1[yx] +=  abs(img[yx-1-w] + img[yx-w] + img[yx-w+1] - img[yx-1+w] - img[yx+w] - img[yx+w+1])>>2;
			//img1[yx] +=  abs(img[yx-1-w] + img[yx-1] + img[yx+w-1] - img[yx+1-w] - img[yx+1] - img[yx+w+1])>>2;

			img1[yx] = img1[yx] > thresh ? img1[yx] : 0;
 		}
	}
	return img1;
}

void utils_copy_border(uint8 *img, uint8 *img1, uint32 b, uint32 w, uint32 h)
{
	uint32 x, y, yx, sq = h*w, l = b*w, l1 = sq-l, l2 = w-b;
	//Top border
	for(y=0; y < l; y+=w)
		for(x=0; x < w; x++){
			yx = y + x;
			//Segmentation fault !!!!!!!!!!!!!!!!!!
			img1[yx] = img[yx];
		}
	//Bottom border
	for(y=l1; y < sq; y+=w)
		for(x=0; x < w; x++){
			yx = y + x;
			img1[yx] = img[yx];
		}
	//Left border
	for(y=l; y < l1; y+=w)
		for(x=0; x < b; x++){
			yx = y + x;
			img1[yx] = img[yx];
		}
	//Right border
	for(y=l; y < l1; y+=w)
		for(x=l2; x < w; x++){
			yx = y + x;
			img1[yx] = img[yx];
		}
}

void inline local_max(uint8 *img, uint8 *img1, uint32 w)
{
	uint32 x, w1 = w-1;
	for(x=1; x < w1; x++){
		if(img[x-1])
		img1[x] = (img[x-1] <= img[x] && img[x] >= img[x+1]) ? img[x] : 0;
	}
}

static inline void check_min(uint8 *img, uint32 x, int w , uint32 *min)
{
	uint32 y = x+w;
	if(img[x] > img[y]) if(img[*min] > img[y]) *min = y;
}

uint8* utils_watershed(uint8 *img, uint8 *img1, uint32 w, uint32 h)
{
	uint32 y=0, sq = w*(h-1), x, w1 = w-1, min, yx;
	//img[0] = 255;
	y=0;
	for(y=w; y < sq; y+=w) {
		x = 0;
		for(x=1; x < w1; x++){
			yx = y+x;
			if(img[yx]){
				min = yx;
				check_min(img, yx, -1 , &min);
				check_min(img, yx, -w , &min);
				check_min(img, yx,  1 , &min);
				check_min(img, yx,  w , &min);
				//check_min(img, yx, -1-w , &min);
				//check_min(img, yx,  1-w , &min);
				//check_min(img, yx,  w+1 , &min);
				//check_min(img, yx,  w-1 , &min);
				img1[min] = 0;
				//if(min != yx ) img1[min] = 0;

				//if(min) { img1[min] = 255; img1[yx] = img[yx]; }
				//else if (img[yx] > 0 ) img1[yx] = 255;
			}
		}
	}
	return img1;
}


void utils_min_region(uint8 *img, uint32 *ind, uint32 *arg, uint32 w, uint32 h)
{
	uint32 y=0, sq = w*(h-1), x, w1 = w-1, min, yx, zc = 0, mc = sq>>1 ;

	ind[0] = img[0] ? mc++ : zc++;
	for(x=1; x < w1; x++){
		yx = y+x;
		ind[yx] = img[yx] ? mc++ : (img[yx-1] ? zc++ : ind[yx-1]);
	}
	//printf("ind[%d] = %d\n", yx, ind[yx]);
	for(y=w; y < sq; y+=w) {
		for(x=1; x < w1; x++){
			yx = y+x;
			if(img[yx] == 0) {
				if(img[yx-1] == 0 ) {
					ind[yx] = ind[yx-1];
				} else ind[yx] = zc++;

				if (img[yx-w] == 0 && img[yx-1] == 0 ) {
					//if(img[yx-w-1]) ind[yx] = zc++;
					if(ind[yx] != ind[yx-w]) {
						arg[ind[yx-w]] = ind[yx];
						//if( ind[yx] > ind[yx-w]) arg[ind[yx]] = ind[yx-w];
						//else arg[ind[yx-w]] = ind[yx];
					}
					//ind[yx] = ind[yx-w];
				}
			} else {
				//ind[yx] = mc++;
				ind[yx] = zc++;
			}
		}
	}
}

void utils_steep_descent(uint8 *img, uint32 *ind, uint32 *arg, uint32 w, uint32 h)
{
	uint32 y, sq = w*(h-1), x, w1 = w-1, min, yx, tmp;
	//y=w;{
	for(y=w; y < sq; y+=w) {
		for(x=1; x < w1; x++){
			yx = y+x;
			min = yx;
			if(img[yx] != 0) {
				check_min(img, yx, -1 , &min);
				check_min(img, yx, -w , &min);
				check_min(img, yx,  1 , &min);
				check_min(img, yx,  w , &min);
				arg[ind[yx]] = (min == yx) ? ind[yx] : ind[min];
			}
		}
	}
}

void utils_connect_region(uint8 *img, uint32 *ind, uint32 *arg, uint32 w, uint32 h)
{
	uint32 y, sq = w*(h-1), x, w1 = w-1, min, yx, tmp;
	//y=w;{
	for(y=w; y < sq; y+=w) {
		for(x=1; x < w1; x++){
			yx = y+x;
			//if(img[yx] == 0) {
			for(tmp = arg[ind[yx]]; arg[tmp] != tmp; tmp = arg[tmp]);
				//printf("yx = %d ind[yx] = %d arg[ind] = %d arg[arg[ind]] = %d  \n", yx, ind[yx], arg[ind[yx]], arg[tmp]);
			ind[yx] = tmp;
			//}
		}
	}
}

void utils_print_img(uint8* img, uint32* ind, uint32 w, uint32 h,  uint32 bx, uint32 by,  uint32 lx, uint32 ly)
{
	uint32 x, y;
	for(y=by; y < ly+by; y++) {
		printf("%5d ", y);
		for(x=bx; x < lx+bx; x++){
			printf("%5d ", img[y*w+x]);
		}
		printf("\n");
	//}for(y=by; y < ly+by; y++) {
		printf("%5d ", y);
		for(x=bx; x < lx+bx; x++){
			printf("%5d ", ind[y*w+x]);
		}
		printf("\n");
	}
	printf("\n");
}

void utils_print_ind(uint8* img, uint32 w, uint32 h,  uint32 bx, uint32 by,  uint32 lx, uint32 ly)
{
	uint32 x, y;
	for(y=by; y < ly; y++) {
		for(x=bx; x < lx; x++){
			printf("%3d ", img[y*w+x]);
		}
		printf("\n");
	}
}


double utils_dist(uint8 *before, uint8 *after, uint32 dim, uint32 d){
/// \fn double dist(uint8 *before, uint8 *after, uint32 dim, uint32 d)
/// \brief Calculate distortion of two image.
/// \param before	Pointer to first image.
/// \param after	Pointer to second image.
/// \param dim 		Size of image height*width.
/// \param d 		d = 1 if gray image, d = 3 if color image.
/// \retval 		The distortion.

	uint32 i;
	double ape;
	unsigned long long r = 0, g = 0, b = 0;
	if(d==3){
		for(i = 0; i < dim; i = i + 3){
			//if(before[i]     - after[i]) printf("x = %4d y = %4d r = %4d\n",i/3,i/3,before[i]     - after[i]);
			r += (before[i]     - after[i])*(before[i]     - after[i]);
			g += (before[i+1] - after[i+1])*(before[i+1] - after[i+1]);
			b += (before[i+2] - after[i+2])*(before[i+2] - after[i+2]);
		}
		ape = (double)(r + g + b)/((double)dim*3.);
		return ape;
	}
	if(d==1){
		for(i = 0; i < dim; i++){
			//if(before[i]- after[i]) printf("i = %9d b = %4d a = %4d r = %4d\n",i,before[i], after[i], before[i] - after[i] );
			r += (before[i] - after[i])*(before[i] - after[i]);
			//printf("a = %3d  b = %3d ", before[i], after[i]);
		}
		//printf("r = %d  dim = %d ", r, dim);
		ape = (double)r/(double)dim;
		//printf(" dist = %f\n",ape);
		return ape;
	}
	else{printf("ape: ERROR\n");
		 return 0.;
	}
}

double utils_ape(uint8 *before, uint8 *after, uint32 dim, uint32 d){
/// \fn double ape(uint8 *before, uint8 *after, uint32 dim, uint32 d)
/// \brief Calculate APE of two image.
/// \param before	Pointer to first image.
/// \param after	Pointer to second image.
/// \param dim 		Size of image height*width.
/// \param d 		d = 1 if gray image, d = 3 if color image.
/// \retval 		The APE.

	uint32 i;
	double ape;
	unsigned long long r = 0, g = 0, b = 0;
	if(d==3){
		for(i = 0; i < dim; i = i + 3){
			//if(before[i]     - after[i]) printf("x = %4d y = %4d r = %4d\n",i/3,i/3,before[i]     - after[i]);
			r += abs((int)(before[i]     - after[i]));
			g += abs((int)(before[i+1] - after[i+1]));
			b += abs((int)(before[i+2] - after[i+2]));
		}
		ape = (double)(r + g + b)/((double)dim*3.);
		return ape;
	}
	if(d==1){
		for(i = 0; i < dim; i++){
			//if(before[i] - after[i] ) printf("i = %4d b = %4d a = %4d diff = %4d\n", i, before[i], after[i], before[i] - after[i]);
			r += abs((int)(before[i] - after[i] ));
		}
		ape = (double)r/(double)dim;
		return ape;
	}
	else{printf("ape: ERROR\n");
		 return 0.;
	}
}

double utils_ape_16(int16 *before, int16 *after, uint32 dim, uint32 d){
/// \fn double ape(uint8 *before, uint8 *after, uint32 dim, uint32 d)
/// \brief Calculate APE of two image.
/// \param before	Pointer to first image.
/// \param after	Pointer to second image.
/// \param dim 		Size of image height*width.
/// \param d 		d = 1 if gray image, d = 3 if color image.
/// \retval 		The APE.

	uint32 i;
	double ape;
	unsigned long long r = 0, g = 0, b = 0;
	if(d==3){
		for(i = 0; i < dim; i = i + 3){
			//if(before[i]     - after[i]) printf("x = %4d y = %4d r = %4d\n",i/3,i/3,before[i]     - after[i]);
			r += abs((int)(before[i]     - after[i]));
			g += abs((int)(before[i+1] - after[i+1]));
			b += abs((int)(before[i+2] - after[i+2]));
		}
		ape = (double)(r + g + b)/((double)dim*3.);
		return ape;
	}
	if(d==1){
		for(i = 0; i < dim; i++){
			//if(before[i] - after[i] ) printf("i = %4d b = %4d a = %4d diff = %4d\n", i, before[i], after[i], before[i] - after[i]);
			r += abs(before[i] - after[i]);
		}
		ape = (double)r/(double)dim;
		return ape;
	}
	else{printf("ape: ERROR\n");
		 return 0.;
	}
}

/** \brief Calculate SSIM of two gray image.
	\param im1	Pointer to first image.
	\param im2	Pointer to second image.
	\param size		Size of image height*width.
	\param bbp 		Bits per pixel.
	\param d 		d = 1 if gray image, d = 3 if color image.
	\retval 		The SSIM.
*/
double utils_ssim_16(int16 *im1, int16 *im2, uint32 w, uint32 h, uint32 bbp, int bd, uint32 d){

	int x, y, i, ix, iy, size, yx, xy, wc = bd*2 + 1, wc2 = wc*wc;
	double ssim = 0, ss, c1, c2, *c;
	double av1=0., av2=0., d1=0., d2=0., d12=0., t1, t2;
	double ssimr, ssimg, ssimb, tmp = 0;
	double avr1=0., avg1=0., avb1=0., avr2=0., avg2=0., avb2=0.;
	double dr1=0., dg1=0.,db1=0., dr2=0., dg2=0., db2=0., dr12=0., dg12=0., db12=0.;
	uint8 *img1, *img2;
	img1 = (uint8*)im1; img2 = (uint8*)im2;

	c = (double *)calloc(wc*wc, sizeof(double));

	c1 = (0.01*((1<<bbp)-1))*(0.01*((1<<bbp)-1));
	c2 = (0.03*((1<<bbp)-1))*(0.03*((1<<bbp)-1));

	//Gaussian Blur kernel
	for(y = 0; y < wc; y++) for(x = 0; x < wc; x++) c[y*wc + x] = 1./(double)wc2;

	//SSIM calculation
	for(y = bd; y < h-bd; y+=wc){
		for(x = bd; x < w-bd; x+=wc){
			av1 = 0.; av2 = 0.;
			for(iy=-bd; iy <= bd; iy++) {
				yx = (y + iy)*w;
				for(ix=-bd; ix <= bd; ix++) {
					xy = yx + x + ix;
					//av1 += im1[xy]*c[(iy+bd)*wc + ix+bd];
					//av2 += im2[xy]*c[(iy+bd)*wc + ix+bd];
					av1 += im1[xy];
					av2 += im2[xy];
					//printf("av1 = %f av2 = %f c[%d][%d] = %f\n", av1, av2, ix+bd, iy+bd, c[(iy+bd)*wc + ix+bd]);
				}
			}
			av1 = av1/(double)wc2;
			av2 = av2/(double)wc2;
			d1=0.; d2=0.; d12=0.;
			for(iy=-bd; iy <= bd; iy++) {
				yx = (y + iy)*w;
				for(ix=-bd; ix <= bd; ix++) {
					xy = yx + x + ix;
					t1 = (av1 - im1[xy]);
					t2 = (av2 - im2[xy]);
					//d1 += t1*t1*c[(iy+bd)*wc + ix+bd];
					//d2 += t2*t2*c[(iy+bd)*wc + ix+bd];
					//d12 += t1*t2*c[(iy+bd)*wc + ix+bd];
					d1 += t1*t1;
					d2 += t2*t2;
					d12 += t1*t2;
				}
			}
			d1 = d1/(double)wc2;
			d2 = d2/(double)wc2;
			d12 = d12/(double)wc2;
			ss = ((2.*av1*av2 + c1)*(2.*d12 + c2))/((av1*av1 + av2*av2 + c1)*(d1 + d2 + c2));
			//printf("SSIM = %f av1 = %f av2 = %f d1 = %f d2 = %f d12 = %f \n", ss, av1, av2, d1, d2, d12);
			ssim += ss;
		}
	}
	//return ssim/(double)((h-2*bd)*(w-2*bd));
	return ssim/(double)((h/wc)*(w/wc));


	if(d==1){
		size = w*h;
		for(i = 0; i < size; i++) av1 += im1[i];
		for(i = 0; i < size; i++) av2 += im2[i];
		av1 = av1/(double)size;
		av2 = av2/(double)size;
		for(i = 0; i < size; i++) d1 += (av1 - im1[i])*(av1 - im1[i]);
		for(i = 0; i < size; i++) d2 += (av2 - im2[i])*(av2 - im2[i]);
		for(i = 0; i < size; i++) d12 += (av1 - im1[i])*(av2 - im2[i]);
		d1 = d1/(double)size;
		d2 = d2/(double)size;
		d12 = d12/(double)size;
		printf("av1 = %f av2 = %f d1 = %f d2 = %f d12 = %f \n", av1, av2, d1, d2, d12);
		ssim = ((2.*av1*av2 + c1)*(2.*d12 + c2))/((av1*av1 + av2*av2 + c1)*(d1 + d2 + c2));
		return ssim;
	}
	if(d==3){
		size = w*h*3;
		//for(i = 0; i < size; i+=3) {tmp += img1[i] - img2[i]; tmp += img1[i+1] - img2[i+1]; tmp += img1[i+2] - img2[i+2]; }
		//printf("diff = %f\n", tmp);

		for(i = 0; i < size; i+=3) { avr1 += img1[i]; avg1 += img1[i+1]; avb1 += img1[i+2]; }//printf("%d %d %d  ", img1[i], img1[i+1], img1[i+2]); }
		for(i = 0; i < size; i+=3) { avr2 += img2[i]; avg2 += img2[i+1]; avb2 += img2[i+2]; }
		avr1 = avr1*3./(double)size; avg1 = avg1*3./(double)size; avb1 = avb1*3./(double)size;
		avr2 = avr2*3./(double)size; avg2 = avg2*3./(double)size; avb2 = avb2*3./(double)size;

		for(i = 0; i < size; i+=3) {
			dr1 += (avr1 - img1[i]  )*(avr1 - img1[i]  );
			dg1 += (avg1 - img1[i+1])*(avg1 - img1[i+1]);
			db1 += (avb1 - img1[i+2])*(avb1 - img1[i+2]);
		}
		for(i = 0; i < size; i+=3) {
			dr2 += (avr2 - img2[i]  )*(avr2 - img2[i]  );
			dg2 += (avg2 - img2[i+1])*(avg2 - img2[i+1]);
			db2 += (avb2 - img2[i+2])*(avb2 - img2[i+2]);
		}
		for(i = 0; i < size; i+=3) {
			dr12 += (avr1 - img1[i]  )*(avr2 - img2[i]  );
			dg12 += (avg1 - img1[i+1])*(avg2 - img2[i+1]);
			db12 += (avb1 - img1[i+2])*(avb2 - img2[i+2]);
		}
		dr1 = dr1*3./(double)size; dg1 = dg1*3./(double)size; db1 = db1*3./(double)size;
		dr2 = dr2*3./(double)size; dg2 = dg2*3./(double)size; db2 = db2*3./(double)size;
		dr12 = dr12*3./(double)size; dg12 = dg12*3./(double)size; db12 = db12*3./(double)size;

		ssimr = ((2.*avr1*avr2 + c1)*(2.*dr12 + c2))/((avr1*avr1 + avr2*avr2 + c1)*(dr1 + dr2 + c2));
		printf("Red   ssim = %f av1 = %f av2 = %f d1 = %f d2 = %f d12 = %f\n", ssimr, avr1, avr2, dr1, dr2, dr12);

		ssimg = ((2.*avg1*avg2 + c1)*(2.*dg12 + c2))/((avg1*avg1 + avg2*avg2 + c1)*(dg1 + dg2 + c2));
		printf("Green ssim = %f av1 = %f av2 = %f d1 = %f d2 = %f d12 = %f\n", ssimg, avg1, avg2, dg1, dg2, dg12);

		ssimb = ((2.*avb1*avb2 + c1)*(2.*db12 + c2))/((avb1*avb1 + avb2*avb2 + c1)*(db1 + db2 + c2));
		printf("Blue  ssim = %f av1 = %f av2 = %f d1 = %f d2 = %f d12 = %f\n", ssimb, avb1, avb2, db1, db2, db12);

		ssim = (ssimr + ssimg + ssimb)/3.;

		return ssim;
	}
}

double utils_psnr(uint8 *before, uint8 *after, uint32 dim, uint32 d){
/// \fn double psnr(uint8 *before, uint8 *after, uint32 dim, uint32 d)
/// \brief Calculate PSNR of two image.
/// \param before	Pointer to first image.
/// \param after	Pointer to second image.
/// \param dim 		Size of image height*width.
/// \param d 		d = 1 if gray image, d = 3 if color image.
/// \retval 		The PSNR.

	uint32 i;
	double psnr;
	unsigned long long r = 0, g = 0, b = 0;
	if(d==3){
		for(i = 0; i< dim; i = i + 3){
			r += (before[i]   - after[i]  )*(before[i]   - after[i]  );
			g += (before[i+1] - after[i+1])*(before[i+1] - after[i+1]);
			b += (before[i+2] - after[i+2])*(before[i+2] - after[i+2]);
		}

		if((r + g + b) == 0){
			return 0.;
		}

		psnr = 10.*log10((255.*255.)/((double)(r + g + b)/((double)dim*3.)));
		return psnr;
	}
	if(d==1){
		for(i = 0; i< dim; i++){
			r += (before[i]   - after[i] )*(before[i]   - after[i]);
		}

		if(r  == 0){
			return 0.;
		}

		psnr = 10.*log10((255.*255.)/((double)r/(double)dim));
		return psnr;
	}
	else{
		printf("psnr: ERROR\n");
		return 0.;
	}
}

double utils_psnr_16(int16 *before, int16 *after, uint32 dim, uint32 d){
/// \fn double psnr(uint8 *before, uint8 *after, uint32 dim, uint32 d)
/// \brief Calculate PSNR of two image.
/// \param before	Pointer to first image.
/// \param after	Pointer to second image.
/// \param dim 		Size of image height*width.
/// \param d 		d = 1 if gray image, d = 3 if color image.
/// \retval 		The PSNR.

	uint32 i;
	double psnr;
	unsigned long long r = 0, g = 0, b = 0;
	if(d==3){
		for(i = 0; i< dim; i = i + 3){
			r += (before[i]   - after[i]  )*(before[i]   - after[i]  );
			g += (before[i+1] - after[i+1])*(before[i+1] - after[i+1]);
			b += (before[i+2] - after[i+2])*(before[i+2] - after[i+2]);
		}

		if((r + g + b) == 0){
			return 0.;
		}

		psnr = 10.*log10((255.*255.)/((double)(r + g + b)/((double)dim*3.)));
		return psnr;
	}
	if(d==1){
		for(i = 0; i< dim; i++){
			r += (before[i]   - after[i] )*(before[i]   - after[i]);
		}

		if(r  == 0){
			return 0.;
		}

		psnr = 10.*log10((255.*255.)/((double)r/(double)dim));
		return psnr;
	}
	else{
		printf("psnr: ERROR\n");
		return 0.;
	}
}

uint8* wavelet_to_rgb(uint8 *img, uint8 *rgb, int height, int width, int step)
{ 
	int i, j, k, t, dim = height*width;
	int w3 = width*3;
	uint8 *p;
	
	if((p = (uint8*)malloc(dim * sizeof(uint8))) == NULL){
		printf("Out of memory\n");
		return NULL;
	}
	t = 1 << step;
	for(j = 0; j < height/t; j++){
		for(i = 0; i < width/t; i++){
			p[width*j + i] = ll(t, i, j);
		} 
	}
	for(k=0; k < step; k++){
		for(j = 0 ; j < height/t; j++){
			for(i = 0; i < width/t; i++){
				p[width*j + i+ width/t] = hl(t, i, j);
			}
		}
		t>>=1;
	}
	t = 1 << step;
	for(k=0; k < step; k++){
		for(j = 0 ; j < height/t; j++){
			for(i = 0; i < width/t; i++){
				p[width*j + (height*width)/t + i] = lh(t, i, j);
			}
		}
		t>>=1;
	}
	t = 1 << step;
	for(k=0; k < step; k++){
		for(j = 0 ; j < height/t; j++){
			for(i = 0; i < width/t; i++){
				p[width*j  + (height*width)/t + i + width/t] = hh(t, i, j);
			}
		}
		t>>=1;
	}
	for(k = 0; k < width; k++){
		for(i = 0; i < height; i++){
			//rgb[ i*w3 + 3*k]  		   = top(low(128 + p[i*width + k]));
			//rgb[ i*w3 + 3*k + 1] 		   = top(low(128 + p[i*width + k]));
			//rgb[ i*w3 + 3*k + 2] 		   = top(low(128 + p[i*width + k]));
			rgb[ i*w3 + 3*k]  			   = p[i*width + k];//+128
			rgb[ i*w3 + 3*k + 1] 		   = p[i*width + k];//+128
			rgb[ i*w3 + 3*k + 2] 		   = p[i*width + k];//+128
		}
	}
	free(p);
	return rgb;
}

uint8* wavelet_to_rgb1(uint8 *img, uint8 *rgb, int height, int width, int step)
{ 
	int i, j, k, t, dim = height*width;
	int w3 = width*3;
	uint8 *p;
	
	if((p = (uint8*)malloc(dim*sizeof(uint8))) == NULL){
		printf("Out of memory\n");
		return NULL;
	}
	t = 1 << step;
	for(j = 0; j < height/t; j++){
		for(i = 0; i < width/t; i++){
			p[width*j + i] = ll(t, i, j);
		} 
	}
	for(k=0; k < step; k++){
		for(j = 0 ; j < height/t; j++){
			for(i = 0; i < width/t; i++){
				p[width*j + i+ width/t] = hl(t, i, j);
			}
		}
		t>>=1;
	}
	t = 1 << step;
	for(k=0; k < step; k++){
		for(j = 0 ; j < height/t; j++){
			for(i = 0; i < width/t; i++){
				p[width*j + (height*width)/t + i] = lh(t, i, j);
			}
		}
		t>>=1;
	}
	t = 1 << step;
	for(k=0; k < step; k++){
		for(j = 0 ; j < height/t; j++){
			for(i = 0; i < width/t; i++){
				p[width*j  + (height*width)/t + i + width/t] = hh(t, i, j);
			}
		}
		t>>=1;
	}
	for(k = 0; k < width; k++){
		for(i = 0; i < height; i++){
			/*rgb[ i*w3 + 3*k]  			   = top(low(128 + p[i*width + k]));
			rgb[ i*w3 + 3*k + 1] 		   = top(low(128 + p[i*width + k]));
			rgb[ i*w3 + 3*k + 2] 		   = top(low(128 + p[i*width + k]));*/
			rgb[ i*w3 + 3*k]  			   = (uint8)clip(128 + p[i*width + k]);
			rgb[ i*w3 + 3*k + 1] 		   = (uint8)clip(128 + p[i*width + k]);
			rgb[ i*w3 + 3*k + 2] 		   = (uint8)clip(128 + p[i*width + k]);
		}
	}
	free(p);
	return rgb;
}

uint8* img_to_rgb128(uint8 *img, uint8 *rgb, int height, int width)
{ 
	int i, j, dim = height*width*3;
	
	for(i = 0,  j= 0; j < dim; j+=3, i++){
		rgb[j]     = rnd(img[i]+128);
		rgb[j + 1] = rnd(img[i]+128);
		rgb[j + 2] = rnd(img[i]+128);
		//printf("img[%d] = %4d\n",i,img[i]);
	}
	return rgb;
}


uint8* uint8_to_rgb(uint8 *img, uint8 *rgb, int height, int width)
{ 
	int i, j, dim = height*width*3;
	
	for(i = 0,  j= 0; j < dim; j+=3, i++){
		rgb[j]     = img[i];
		rgb[j + 1] = img[i];
		rgb[j + 2] = img[i];
		//printf("y_w[%d] = %4d\n",i,mod(yuv_buffer->y_w[i]));
	}
	return rgb;
}

//Copy image
void copy(uint8 *in, uint8 *out, int w, int h)
{
	int i, dim = h*w;
	for(i = 0; i < dim; i++){
		out[i] = in[i];
	}
}

//Compare two image
void compare(uint8 *in, uint8 *out, int w, int h)
{
	int x, y, c=0;
	for(y = 0; y < h; y++){
		for(x=0; x<w; x++){
			if(in[y*w+x] - out[y*w+x]) c++;
		}
		printf("y = %4d c = %4d\n", y, c);
		c=0;
	}
}

//Make image less for one pix horizontal and for one vertical 
void resizeonepix(uint8 *in, int w, int h)
{
	int x, y;
	for(y = 0; y < (h-1); y++){
		for(x=0; x < (w-1); x++){
			in[y*(w-1)+x] = in[y*w+x];
		}
	}
}

uint8* malet_to_rgb(uint8 *img, uint8 *rgb, int h, int w, int step)
{ 
	int x, y, k, t;
	int ws[4], hs[4], s[4], tlx[4], tly[4];
	
	ws[0] = (w>>1) + w%2; hs[0] = (h>>1) + h%2;
	ws[1] = (w>>1); 	  hs[1] = (h>>1) + h%2;
	ws[2] = (w>>1) + w%2; hs[2] = (h>>1);
	ws[3] = (w>>1); 	  hs[3] = (h>>1);
	s[0] = 0; s[1] = ws[0]*hs[0]; s[2] = s[1] + ws[1]*hs[1]; s[3] = s[2] + ws[2]*hs[2];
	tlx[0] = 0    ; tly[0] = 0;
	tlx[1] = ws[0]; tly[1] = 0;
	tlx[2] = 0    ; tly[2] = hs[0];
	tlx[3] = ws[0]; tly[3] = hs[0];
		
	for(t=0; t<step; t++){
		ws[0] = (ws[0]>>1) + ws[0]%2; 	hs[0] = (hs[0]>>1) + hs[0]%2;
		ws[1] = (ws[0]>>1); 	  		hs[1] = (hs[0]>>1) + hs[0]%2;
		ws[2] = (ws[0]>>1) + ws[0]%2; 	hs[2] = (hs[0]>>1);
		ws[3] = (ws[0]>>1); 	  		hs[3] = (hs[0]>>1);
		s[0] = 0; s[1] = ws[0]*hs[0]; s[2] = s[1] + ws[1]*hs[1]; s[3] = s[2] + ws[2]*hs[2];
		tlx[0] = 0    ; tly[0] = 0;
		tlx[1] = ws[0]; tly[1] = 0;
		tlx[2] = 0    ; tly[2] = hs[0];
		tlx[3] = ws[0]; tly[3] = hs[0];
		
		
		for(k=1; k < 4; k++){
			for(y=0; y < hs[k]; y++ ){
				for(x=0; x < ws[k]; x++){
					//if(y==0) printf("rgb[%d] = %d img[%d] = %d \n", 3*((y+tly[k])*w + tlx[k] +x), rgb[3*((y+tly[k])*w + tlx[k] +x)], s[k] + y*ws[k] + x, img[s[k] + y*ws[k] + x]);
					rgb[3*((y+tly[k])*w + tlx[k] +x)]   = rnd(128+img[s[k] + y*ws[k] + x]);
					rgb[3*((y+tly[k])*w + tlx[k] +x)+1] = rnd(128+img[s[k] + y*ws[k] + x]);
					rgb[3*((y+tly[k])*w + tlx[k] +x)+2] = rnd(128+img[s[k] + y*ws[k] + x]);
				}
			}
		}
	}
	k=0;
	for(y=0; y < hs[k]; y++ ){
		for(x=0; x < ws[k]; x++){
			rgb[3*((y+tly[k])*w + tlx[k] +x)]   = rnd(img[s[k] + y*ws[k] + x]);
			rgb[3*((y+tly[k])*w + tlx[k] +x)+1] = rnd(img[s[k] + y*ws[k] + x]);
			rgb[3*((y+tly[k])*w + tlx[k] +x)+2] = rnd(img[s[k] + y*ws[k] + x]);
		}
	}
	
	return rgb;
}


double dist3(uint8 *before, uint8 *after, uint32 dim, uint32 d){
/*! \fn double dist(uint8 *before, uint8 *after, uint32 dim, uint32 d)
    \brief Calculate distortion of two image.
    \param before	Pointer to first image.
    \param after	Pointer to second image.
    \param dim 		Size of image height*width.
    \param d 		d = 1 if gray image, d = 3 if color image.
    \retval 		The distortion.
*/

	uint32 i;
	double ape;
	unsigned long long r = 0, g = 0, b = 0;
	if(d==3){
		for(i = 0; i < dim; i = i + 3){
			//if(before[i]     - after[i]) printf("x = %4d y = %4d r = %4d\n",i/3,i/3,before[i]     - after[i]);
			r += (before[i]     - after[i])*(before[i]     - after[i]);
			g += (before[i+1] - after[i+1])*(before[i+1] - after[i+1]);
			b += (before[i+2] - after[i+2])*(before[i+2] - after[i+2]);
		}
		ape = (double)(r + g + b)/((double)dim*3.);
		return ape;
	}
	if(d==1){
		for(i = 0; i < dim; i++){
			//if(before[i]- after[i]) printf("i = %9d b = %4d a = %4d r = %4d\n",i,before[i], after[i], before[i] - after[i] );
			r += (before[i] - after[i])*(before[i] - after[i]);
			//printf("a = %3d  b = %3d ", before[i], after[i]);
		}
		//printf("r = %d  dim = %d ", r, dim);
		ape = (double)r/(double)dim;
		//printf(" dist = %f\n",ape);
		return ape;
	}
	else{printf("ape: ERROR\n");
		 return 0.;
	}
}

double ape(uint8 *before, uint8 *after, uint32 dim, uint32 d){
/*! \fn double ape(uint8 *before, uint8 *after, uint32 dim, uint32 d)
    \brief Calculate APE of two image.
    \param before	Pointer to first image.
    \param after	Pointer to second image.
    \param dim 		Size of image height*width.
    \param d 		d = 1 if gray image, d = 3 if color image.
    \retval 		The APE.
*/

	uint32 i;
	double ape;
	unsigned long long r = 0, g = 0, b = 0;
	if(d==3){
		for(i = 0; i < dim; i = i + 3){
			//if(before[i]     - after[i]) printf("x = %4d y = %4d r = %4d\n",i/3,i/3,before[i]     - after[i]);
			r += abs((int)(before[i]     - after[i]));
			g += abs((int)(before[i+1] - after[i+1]));
			b += abs((int)(before[i+2] - after[i+2]));
		}
		ape = (double)(r + g + b)/((double)dim*3.);
		return ape;
	}
	if(d==1){
		for(i = 0; i < dim; i++){
			//if(before[i] - after[i] && i < 1920*2) printf("i = %4d b = %4d a = %4d\n", i, before[i], after[i]);
			r += abs((int)(before[i] - after[i] ));
		}
		ape = (double)r/(double)dim;
		return ape;
	}
	else{printf("ape: ERROR\n");
		 return 0.;
	}
}

double ape3(uint8 *before, uint8 *after, uint32 dim, uint32 d){
/*! \fn double ape(uint8 *before, uint8 *after, uint32 dim, uint32 d)
    \brief Calculate APE of two image.
    \param before	Pointer to first image.
    \param after	Pointer to second image.
    \param dim 		Size of image height*width.
    \param d 		d = 1 if gray image, d = 3 if color image.
    \retval 		The APE.
*/

	uint32 i;
	double ape;
	unsigned long long r = 0, g = 0, b = 0;
	if(d==3){
		for(i = 0; i < dim; i = i + 3){
			//if(before[i]     - after[i]) printf("x = %4d y = %4d r = %4d\n",i/3,i/3,before[i]     - after[i]);
			r += abs((int)(before[i]     - after[i]));
			g += abs((int)(before[i+1] - after[i+1]));
			b += abs((int)(before[i+2] - after[i+2]));
		}
		ape = (double)(r + g + b)/((double)dim*3.);
		return ape;
	}
	if(d==1){
		for(i = 0; i < dim; i++){
			//if(before[i]- after[i]) printf("i = %4d r = %4d\n",i,before[i] - after[i]);
			r += abs((int)(before[i] - after[i] ));
		}
		ape = (double)r/(double)dim;
		return ape;
	}
	else{printf("ape: ERROR\n");
		 return 0.;
	}
}


double psnr3(uint8 *before, uint8 *after, uint32 dim, uint32 d){
/*! \fn double psnr(uint8 *before, uint8 *after, uint32 dim, uint32 d)
    \brief Calculate PSNR of two image.
    \param before	Pointer to first image.
    \param after	Pointer to second image.
    \param dim 		Size of image height*width.
    \param d 		d = 1 if gray image, d = 3 if color image.
    \retval 		The PSNR.
*/

	uint32 i;
	double psnr;
	unsigned long long r = 0, g = 0, b = 0;
	if(d==3){
		for(i = 0; i< dim; i = i + 3){
			r += (before[i]   - after[i]  )*(before[i]   - after[i]  );
			g += (before[i+1] - after[i+1])*(before[i+1] - after[i+1]);
			b += (before[i+2] - after[i+2])*(before[i+2] - after[i+2]);
		}
		
		if((r + g + b) == 0){
			return 0.;
		}
		
		psnr = 10.*log10((255.*255.)/((double)(r + g + b)/((double)dim*3.)));
		return psnr;
	}
	if(d==1){
		for(i = 0; i< dim; i++){
			r += (before[i]   - after[i] )*(before[i]   - after[i]);
		}
		
		if(r  == 0){
			return 0.;
		}
		
		psnr = 10.*log10((255.*255.)/((double)r/(double)dim));
		return psnr;
	}
	else{
		printf("psnr: ERROR\n");
		return 0.;
	}
}

uint8* YUV_to_RGB(uint8 *rgb, uint8 *y, uint8 *u, uint8 *v, uint32 sq)
{
	int C, D, E, i;
	for(i=0; i<sq; i++){
		C = y[i] - 16;
		D = u[i] - 128;
		E = v[i] - 128;
		rgb[i*3    ] = clip(( 298*C         + 409*E + 128) >> 8);
		rgb[i*3 + 1] = clip(( 298*C - 100*D - 208*E + 128) >> 8);
		rgb[i*3 + 2] = clip(( 298*C + 516*D         + 128) >> 8);
	}
	return rgb;
}

uint8* YUV420p_to_RGB(uint8 *rgb, uint8 *y, uint8 *u, uint8 *v, uint32 w, uint32 h)
{
	int C, D, E, i, j;
	for(i=0; i<h; i++){
		for(j=0; j< w; j++){
			C = y[j + w*i] - 16;
			D = u[(j>>1) + (w>>1)*(i>>1)] - 128;
			E = v[(j>>2) + (w>>1)*(i>>2)] - 128;
			rgb[(j+w*i)*3    ] = clip(( 298*C         + 409*E + 128) >> 8);
			rgb[(j+w*i)*3 + 1] = clip(( 298*C - 100*D - 208*E + 128) >> 8);
			rgb[(j+w*i)*3 + 2] = clip(( 298*C + 516*D         + 128) >> 8);
		}
	}
	return rgb;
}

uint32 utils_read_ppm(const char *filename, uint32 *w, uint32 *h, uint32 *bpp, uint8 *img)
{
    FILE *wl;
    uint8 line[100];
    uint32 byts;

	wl = fopen(filename, "rb");
    if(wl == NULL) {
    	printf("Can't open file %s\n", filename);
    	return 0;
    }
	//byts = fscanf(wl, "%s%s%s%s%s%s%s", line[0], line[1], line[2], line[3], line[4], line[5], line[6]);
	byts = fscanf(wl, "%s", line);
	if (strcmp(line, "P6") != 0) {
		printf ("It's not PPM file");
		return 0;
	}
	byts = fscanf(wl, "%s", line);
	if(line[0] == '#'){
		if(fgets(line, 100, wl) == NULL) { printf("Can't get header\n"); return; }
	} else {
		*w = atoi(line);
		byts = fscanf(wl, "%s", line); *h = atoi(line);
		byts = fscanf(wl, "%s", line); *bpp = (atoi(line) > 256) ? 2 : 1;
	}

	byts = fscanf(wl, "%s", line); *w = atoi(line);
	byts = fscanf(wl, "%s", line); *h = atoi(line);
	byts = fscanf(wl, "%s", line); *bpp = (atoi(line) > 256) ? 2 : 1;
	printf("w = %d h = %d bpp = %d\n", *w, *h, *bpp);
	fgetc(wl);

	//*img = (uint8 *)calloc((*w)*(*h)*(*bpp)*3, sizeof(uint8));
	byts = fread(img, sizeof(uint8), (*w)*(*h)*(*bpp)*3,  wl);
	if(byts != (*w)*(*h)*(*bpp)*3){ printf("Image read error\n");}
	//printf("byts = %d size = %d\n", byts, (*w)*(*h)*(*bpp)*3);
    fclose(wl);
	//fclose(wl);
	return byts;
}

void utils_rgb2bayer(uint8 *rgb, int16 *bay, uint32 w, uint32 h)
{
	uint32 w1 = w<<1, h1 = h<<1, w3 = w*3, x, x1, y, y1, yx, yx1;
	for(y=0, y1=0; y < h1; y+=2, y1++){
		for(x=0, x1=0; x < w1; x+=2, x1+=3){
			yx = y*w1 + x;
			yx1 = y1*w3 + x1;
			bay[yx] = rgb[yx1];
			bay[yx+1] = rgb[yx1+1];
			bay[yx+w1] = rgb[yx1+1];
			bay[yx+w1+1] = rgb[yx1+2];
		}
	}
}

void utils_subtract(uint8 *img1, uint8 *img2, uint8 *sub, uint32 w, uint32 h)
{
	uint32 sq = w*h, x, y, yx;
	for(y=0; y < sq; y+=w){
		for(x=0; x < w; x++){
			yx = y + x;
			sub[yx] = abs(img2[yx] - img1[yx]);
		}
	}

}

/* \brief Calculate image entropy.
	\param img 	The input image.
	\param buf 	The buffer for probability distribution.
	\param w 	The image width.
	\param h 	The image height.
	\retval 	The entropy (bits per pixel).
 */
double entropy(int16 *img, uint32 *buf, uint32 w, uint32 h, uint32 bpp)
{
	double s = 0., s0 = log2(w*h);
	uint32  half = 1<<(bpp-1), size = 1<<bpp, sz = w*h;
	int i;

	for(i=0; i < size; i++) buf[i] = 0;
	for(i=0; i < sz; i++) buf[img[i]+half]++;
	//for(i=0; i < size; i++) printf("%d ",  buf[i]);
	//printf("\n");

	for(i=0; i < size; i++){
		if(buf[i]) s -= buf[i]*(log2(buf[i]) - s0);
	}
	return s/sz;
}

