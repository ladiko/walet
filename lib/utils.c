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

//#define clip(x)		x < 0 ? (x < -255 ? 255 : -x) : ( x > 255 ? 255 : x);
#define clip(x)		abs(x);

static inline void drawrect_8s(uint8 *rgb, int8 *pic, uint32 x0, uint32 y0, uint32 w, uint32 h, uint32 size, uint32 shift)
{
	uint32 x, y, tmp;
	for(y=0; y < h; y++ ){
		for(x=0; x < w; x++){
			//tmp = rnd(im[y*w+x] < 0 ? -im[y*w+x]<<1 : im[y*w+x]<<1);
			tmp = rnd(shift + pic[y*w+x]);
			rgb[3*((y+y0)*size +x0 +x)]   = tmp; //rnd(shift+im[y*w+x]); //im[y*w+x] ? 255 : 0; //
			rgb[3*((y+y0)*size +x0 +x)+1] = tmp; //rnd(shift+im[y*w+x]); //im[y*w+x] ? 255 : 0; //
			rgb[3*((y+y0)*size +x0 +x)+2] = tmp; //rnd(shift+im[y*w+x]); //im[y*w+x] ? 255 : 0; //
		}
	}
}

static inline void drawrect(uint8 *rgb, int16 *im, uint32 w0, uint32 h0, uint32 w, uint32 h, uint32 size, uint32 shift)
{
	uint32 x, y, tmp;
	for(y=0; y < h; y++ ){
		for(x=0; x < w; x++){
			//tmp = rnd(im[y*w+x] < 0 ? -im[y*w+x]<<1 : im[y*w+x]<<1);
			tmp = rnd(shift+im[y*w+x]);
			rgb[3*((y+h0)*size+w0+x)]   = tmp; //rnd(shift+im[y*w+x]); //im[y*w+x] ? 255 : 0; //
			rgb[3*((y+h0)*size+w0+x)+1] = tmp; //rnd(shift+im[y*w+x]); //im[y*w+x] ? 255 : 0; //
			rgb[3*((y+h0)*size+w0+x)+2] = tmp; //rnd(shift+im[y*w+x]); //im[y*w+x] ? 255 : 0; //
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

uint8* utils_one_dwt_draw_8(Pic8s *ll, Pic8s *hl, Pic8s *lh, Pic8s *hh, uint8 *rgb, uint32 x, uint32 y, uint32 w)
{
	drawrect_8s(rgb, ll->pic, x,         y,         ll->w, ll->h, w, 128);
	drawrect_8s(rgb, hl->pic, x + ll->w, y,         hl->w, hl->h, w, 128);
	drawrect_8s(rgb, lh->pic, x        , y + ll->h, lh->w, lh->h, w, 128);
	drawrect_8s(rgb, hh->pic, x + ll->w, y + ll->h, hh->w, hh->h, w, 128);
	return rgb;
}

uint8* utils_dwt_draw_8(BAY8 *b8, Level8 **l8, uint8 *rgb, uint32 steps)
{
	uint32 i, j, x, y, w = b8->C[0].w +  b8->C[1].w;
	if(steps){
		for(j=0; j < steps; j++){
			for(i=0; i < 4; i++){
				x = 0; y = 0;
				x = b8->C[0].w; y = 0;
				x = 0; y = b8->C[0].h;
				x = b8->C[0].w; y = b8->C[0].h;
				drawrect_8s(rgb, l8[j][i].s[0].pic, x,                   y,                   l8[j][i].s[0].w, l8[j][i].s[0].h, w, 128);
				drawrect_8s(rgb, l8[j][i].s[1].pic, x + l8[j][i].s[0].w, y,                   l8[j][i].s[1].w, l8[j][i].s[1].h, w, 128);
				drawrect_8s(rgb, l8[j][i].s[2].pic, x,                   y + l8[j][i].s[0].h, l8[j][i].s[2].w, l8[j][i].s[2].h, w, 128);
				drawrect_8s(rgb, l8[j][i].s[3].pic, x + l8[j][i].s[0].w, y + l8[j][i].s[0].h, l8[j][i].s[3].w, l8[j][i].s[3].h, w, 128);
			}
		}


		uint32 w = l8[0][0].s[0].w + l8[0][0].s[1].w + l8[0][1].s[0].w + l8[0][1].s[1].w;
		utils_one_dwt_draw_8(&l8[0][0].s[0], &l8[0][0].s[1], &l8[0][0].s[2], &l8[0][0].s[3], rgb, 0, 0, w);
		utils_one_dwt_draw_8(&l8[0][1].s[0], &l8[0][1].s[1], &l8[0][1].s[2], &l8[0][1].s[3], rgb, l8[0][0].s[0].w + l8[0][0].s[1].w, 0, w);
		utils_one_dwt_draw_8(&l8[0][2].s[0], &l8[0][2].s[1], &l8[0][2].s[2], &l8[0][2].s[3], rgb, 0, l8[0][0].s[0].h + l8[0][0].s[2].h, w);
		utils_one_dwt_draw_8(&l8[0][3].s[0], &l8[0][3].s[1], &l8[0][3].s[2], &l8[0][3].s[3], rgb, l8[0][0].s[0].w + l8[0][0].s[1].w, l8[0][0].s[0].h + l8[0][0].s[2].h, w);

	} else {
		drawrect_8s(rgb, b8->C[0].pic, 0,          0,          b8->C[0].w, b8->C[0].h, w, 128);
		drawrect_8s(rgb, b8->C[1].pic, b8->C[0].w, 0,          b8->C[1].w, b8->C[1].h, w, 128);
		drawrect_8s(rgb, b8->C[2].pic, 0,          b8->C[0].h, b8->C[2].w, b8->C[2].h, w, 128);
		drawrect_8s(rgb, b8->C[3].pic, b8->C[0].w, b8->C[0].h, b8->C[3].w, b8->C[3].h, w, 128);
	}

	return rgb;
}

uint8* utils_subband_draw(Image *img, uint8 *rgb, ColorSpace color, uint32 steps)
//For vizualisation only
{
	uint32 k, h, w, h0, w0, h1, w1, st;
	int i;
	int16* im;
	Subband *sub = img->sub;
	//for(i=0; i<=steps*3; i++) printf("sub %d h %d w %d p %p\n",sub[0][i]->subb, sub[0][i]->size.y, sub[0][i]->size.x,  sub[0][i ]);
	if(color == BAYER ){
		if(steps > 1){
			for(k=0; k<4; k++) {
				st = ((steps-1)*3+1);
				switch(k){
				case 0 : {h1 = 0; w1 = 0; break;}
				case 1 : {h1 = 0; w1 = sub[3*(steps-1)-1].size.x + sub[3*(steps-1)-2].size.x; break;}
				case 2 : {h1 = sub[3*(steps-1)-2].size.y + sub[3*(steps-1)-1].size.y; w1 = 0; break;}
				case 3 : {h1 = sub[3*(steps-1)-2].size.y + sub[3*(steps-1)-1].size.y;
						  w1 = sub[3*(steps-1)-1].size.x + sub[3*(steps-1)-2].size.x; break;}
				}
				for(i=(steps-1); i>0; i--){
					h0 = sub[k*st + 3*i-2].size.y; w0 = sub[k*st + 3*i-1].size.x;
					h  = sub[k*st + 3*i-2].size.y;  w = sub[k*st + 3*i-2].size.x;
					im = &img->iwt[sub[k*st + 3*i-2].loc];
					drawrect(rgb, im, w1+w0, h1, w, h, img->width, 128);

					h = sub[k*st + 3*i-1].size.y; w = sub[k*st + 3*i-1].size.x;
					im = &img->iwt[sub[k*st + 3*i-1].loc];
					drawrect(rgb, im, w1, h1+h0, w, h, img->width, 128);

					h = sub[k*st + 3*i].size.y; w = sub[k*st + 3*i].size.x;
					im = &img->iwt[sub[k*st + 3*i].loc];
					drawrect(rgb, im, w1+w0, h1+h0, w, h, img->width, 128);
				}
				h = sub[k*st ].size.y; w = sub[k*st].size.x;
				im = &img->iwt[sub[k*st].loc];
				drawrect(rgb, im, w1, h1, w, h, img->width, 128);
			}
		} else {
			h0 = sub[0].size.y; w0 = sub[0].size.x;
			h  = sub[1].size.y; w  = sub[1].size.x;
			im = &img->iwt[sub[1].loc];
			drawrect(rgb, im, w0, 0, w, h, img->width, 128);

			h = sub[2].size.y; w = sub[2].size.x;
			im = &img->iwt[sub[2].loc];
			drawrect(rgb, im, 0, h0, w, h, img->width, 128);

			h = sub[3].size.y; w = sub[3].size.x;
			im = &img->iwt[sub[3].loc];
			drawrect(rgb, im, w0, h0, w, h, img->width, 128);

			h = sub[0].size.y; w = sub[0].size.x;
			im = &img->iwt[sub[0].loc];
			drawrect(rgb, im, 0, 0, w, h, img->width, 0);
		}
	} else {
		for(i=steps; i>0; i--){
			h0 = sub[3*i-2].size.y; w0 = sub[3*i-1].size.x;
			h  = sub[3*i-2].size.y; w  = sub[3*i-2].size.x;
			im = &img->iwt[sub[3*i-2].loc];
			drawrect(rgb, im, w0, 0, w, h, img->width, 128);

			h = sub[3*i-1].size.y; w = sub[3*i-1].size.x;
			im = &img->iwt[sub[3*i-1].loc];
			drawrect(rgb, im, 0, h0, w, h, img->width, 128);

			h = sub[3*i].size.y; w = sub[3*i].size.x;
			im = &img->iwt[sub[3*i].loc];
			drawrect(rgb, im, w0, h0, w, h, img->width, 128);
		}
		h = sub[0].size.y; w = sub[0].size.x;
		im = &img->iwt[sub[0].loc];
		drawrect(rgb, im, 0, 0, w, h, img->width, 0);
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

#define oe(a,x)	(a ? x&1 : (x+1)&1)
//#define lb(x) (x&0xFF)
#define lb(x) (((x) < 0) ? 0 : (((x) > 255) ? 255 : (x)))

uint8* utils_bayer_draw(uint8 *img, uint8 *rgb, uint32 w, uint32 h,  BayerGrid bay)
/*! \fn void bayer_to_rgb(uint8 *rgb)
	\brief DWT picture transform.
  	\param	rgb 	The pointer to rgb array.
*/
{
/*
   All RGB cameras use one of these Bayer grids:

	BGGR  0         GRBG 1          GBRG  2         RGGB 3
	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5
	0 B G B G B G	0 G R G R G R	0 G B G B G B	0 R G R G R G
	1 G R G R G R	1 B G B G B G	1 R G R G R G	1 G B G B G B
	2 B G B G B G	2 G R G R G R	2 G B G B G B	2 R G R G R G
	3 G R G R G R	3 B G B G B G	3 R G R G R G	3 G B G B G B
 */
	uint32 x, y, wy, xwy, xwy3, y2, x2, a, b, h1 = h-1, w1 = w-1, yw, yw1;

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

uint8* utils_bayer_draw_16(int16 *img, uint8 *rgb, uint32 w, uint32 h,  BayerGrid bay)
/*! \fn void bayer_to_rgb(uint8 *rgb)
	\brief DWT picture transform.
  	\param	rgb 	The pointer to rgb array.
*/
{
/*
   All RGB cameras use one of these Bayer grids:

	BGGR  0         GRBG 1          GBRG  2         RGGB 3
	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5
	0 B G B G B G	0 G R G R G R	0 G B G B G B	0 R G R G R G
	1 G R G R G R	1 B G B G B G	1 R G R G R G	1 G B G B G B
	2 B G B G B G	2 G R G R G R	2 G B G B G B	2 R G R G R G
	3 G R G R G R	3 B G B G B G	3 R G R G R G	3 G B G B G B
 */
	uint32 x, y, wy, xwy, xwy3, y2, x2, a, b, h1 = h-1, w1 = w-1, yw, yw1;

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

uint8* utils_draw_bayer(uint8 *img, uint8 *rgb, uint32 w, uint32 h,  BayerGrid bay)
/*! \fn void bayer_to_rgb(uint8 *rgb)
	\brief DWT picture transform.
  	\param	rgb 	The pointer to rgb array.
*/
{
/*
   All RGB cameras use one of these Bayer grids:

	BGGR  0         GRBG 1          GBRG  2         RGGB 3
	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5
	0 B G B G B G	0 G R G R G R	0 G B G B G B	0 R G R G R G
	1 G R G R G R	1 B G B G B G	1 R G R G R G	1 G B G B G B
	2 B G B G B G	2 G R G R G R	2 G B G B G B	2 R G R G R G
	3 G R G R G R	3 B G B G B G	3 R G R G R G	3 G B G B G B
 */
	uint32 y, x, yx, yw, yx3, i, sq = w*h - w, w1 = w-1;
	switch(bay){
		case BGGR :
			for(yw=w, y=1; yw < sq; y++, yw+=w){
				for(x=1; x < w1; x++){
					yx = yw + x;
					yx3 = yx*3;
					rgb[yx3  ] = (x&1 && y&1) ? img[yx] : ((!(x&1) && !(y&1)) ? (img[yx-1-w] + img[yx+1-w] + img[yx-1+w] + img[yx+1+w])>>2 :
								 (!(x&1) && y&1 ? (img[yx-1] + img[yx+1])>>1 : (img[yx-w] + img[yx+w])>>1));
					rgb[yx3+1] = ((y&1 && !(x&1)) || (!(y&1) && x&1)) ? (img[yx-1] + img[yx-w] + img[yx+1] + img[yx+w])>>2 : img[yx];
					rgb[yx3+2] = (x&1 && y&1) ? img[yx] : ((!(x&1) && !(y&1)) ? (img[yx-1-w] + img[yx+1-w] + img[yx-1+w] + img[yx+1+w])>>2 :
								 (x&1 && !(y&1) ? (img[yx-1] + img[yx+1])>>1 : (img[yx-w] + img[yx+w])>>1));
				}
			}
		//case GRBG :
		//case GBRG :
		case RGGB :
			for(yw=w, y=1; yw < sq; y++, yw+=w){
				for(x=1; x < w1; x++){
					yx = yw + x;
					yx3 = yx*3;
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

uint8* utils_draw_scale_color(uint8 *rgb, uint8 *img,  uint32 w0, uint32 h0, uint32 w, uint32 h,   uint32 wp, BayerGrid bay)
/*! \fn void bayer_to_rgb(uint8 *rgb)
	\brief DWT picture transform.
  	\param	rgb 	The pointer to rgb array.
*/
{
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

uint8* utils_grey_draw(uint8 *img, uint8 *rgb, uint32 w, uint32 h)
{
	int i, j, dim = h*w*3;
	for(i = 0,  j= 0; j < dim; j+=3, i++){
		rgb[j]     = img[i];
		rgb[j + 1] = img[i];
		rgb[j + 2] = img[i];
		//printf("y_w[%d] = %4d\n",i,mod(yuv_buffer->y_w[i]));
	}
	return rgb;
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

void utils_resize_bayer_2x(uint8 *img, uint8 *img1, uint32 w, uint32 h)
{
	uint32 x, y, yx, yx1, h1 = ((h>>2)<<2)*w, w2 = w<<2, w1 = ((w>>2)<<2), wn = w>>1, i=0;
	for(y=0; y < h1; y+=w2){
		for(x=0; x < w1; x+=4){
			yx = y + x;
			yx1 = (y>>2) + (x>>1);
			img1[yx1] 		= (img[yx] 		+ img[yx+2] 	+ img[yx+w2] 		+ img[yx+w2+2])>>2;
			img1[yx1+1] 	= (img[yx+1] 	+ img[yx+3] 	+ img[yx+w2+1] 		+ img[yx+w2+3])>>2;
			img1[yx1+wn] 	= (img[yx+w] 	+ img[yx+2+w] 	+ img[yx+w2+w] 		+ img[yx+w2+2+w])>>2;
			img1[yx1+wn+1]	= (img[yx+w+1] 	+ img[yx+3+w]	+ img[yx+w2+w+1]	+ img[yx+w2+3+w])>>2;
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


double utils_dist(uint8 *before, uint8 *after, uint32 dim, uint32 d)
/// \fn double dist(uint8 *before, uint8 *after, uint32 dim, uint32 d)
/// \brief Calculate distortion of two image.
/// \param before	Pointer to first image.
/// \param after	Pointer to second image.
/// \param dim 		Size of image height*width.
/// \param d 		d = 1 if gray image, d = 3 if color image.
/// \retval 		The distortion.
{
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

double utils_ape(uint8 *before, uint8 *after, uint32 dim, uint32 d)
/// \fn double ape(uint8 *before, uint8 *after, uint32 dim, uint32 d)
/// \brief Calculate APE of two image.
/// \param before	Pointer to first image.
/// \param after	Pointer to second image.
/// \param dim 		Size of image height*width.
/// \param d 		d = 1 if gray image, d = 3 if color image.
/// \retval 		The APE.
{
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

double utils_psnr(uint8 *before, uint8 *after, uint32 dim, uint32 d)
/// \fn double psnr(uint8 *before, uint8 *after, uint32 dim, uint32 d)
/// \brief Calculate PSNR of two image.
/// \param before	Pointer to first image.
/// \param after	Pointer to second image.
/// \param dim 		Size of image height*width.
/// \param d 		d = 1 if gray image, d = 3 if color image.
/// \retval 		The PSNR.
{
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




void unifom_8bit(uint32 *distrib, uint32 bits, uint32 step, uint8 sub, uint32 size, uint32 *q, double *dis, double *e)
/*! \fn static inline int dist_unifom_8(uint32 *distrib, const uint32 bit)
	\brief Calculate distortion for the given uniform quantizer.
    \param distrib	 The pointer to array of distribution probabilities.
    \param bits 	The bits per pixel.
	\param step		The step of quantization.
	\param sub		The number of subband
	\param size		The number of pixels in subband
	\param q		The quntization array
	\param dis		The subband distortion
	\param en		The subband entropy
*/
{
	uint32 sum = 0, en, tot=0, HALF = (1<<(bits-1));
	int i, j, rest, last, lasth;
	int hstep = step>>1;  // The half size of interval
	*e=0.;

	if(!sub){
		rest = 256;
		for(i=0; rest >= step; i+=step){
			rest -= step; en = 0;
			for(j=0; j< step; j++) {
				sum += distrib[HALF+i+j]*(j-hstep)*(j-hstep);
				en += distrib[HALF+i+j];
				q[i+j] = i + hstep;
				//if(distrib[HALF+i+j]) printf("%d\n", distrib[HALF+i+j]);
			}
			if(en) *e -= ((double)en/(double)size)*log2((double)en/(double)size);
			tot += en;
			//printf("rest = %d decision[%d] = %d, value[%d] = %d step = %d e = %f \n", rest, i/step, i, i/step, i+hstep, step, ((double)en/(double)size)*log2((double)en/(double)size));
		}
		if(rest){
			last = rest; en = 0;
			lasth = (last>>1);
			for(j=0; j< last; j++) {
				sum += distrib[HALF+i+j]*(j-lasth)*(j-lasth);
				en += distrib[HALF+i+j];
				q[i+j] = i + lasth;
			}
			if(en) *e -= ((double)en/(double)size)*log2((double)en/(double)size);
			tot += en;
			//printf("rest = %d decision[%d] = %d, value[%d] = %d step = %d e = %f \n", rest, i/step, i, i/step, i+lasth, last, ((double)en/(double)size)*log2((double)en/(double)size));
		}
		//printf("distr = %f entrop = %f tot = %d size = %d\n",(double)sum/(double)size, e, tot, size);

	} else {
		rest = 128-step;  en = 0;
		for(j= (1-step); j< (int)step; j++) {
			sum += distrib[HALF+j]*j*j;
			en += distrib[HALF+j];
			q[128 +i+j] = 0;
			//printf("sum = %d en = %d\n", sum, en);
		}
		if(en) *e -= ((double)en/(double)size)*log2((double)en/(double)size);
		tot += en;
		//printf("rest = %d decision[%d] = %d, value[%d] = %d step = %d e = %f \n", rest, 0, 0, 0, 0, step, ((double)en/(double)size)*log2((double)en/(double)size));
		for(i=step; rest >= step; i+=step){
			rest -= step;  en = 0;
			for(j= 0; j< step; j++) {
				sum += distrib[HALF+i+j]*(j-hstep)*(j-hstep);
				en += distrib[HALF+i+j];
				q[128 +i+j] = i+hstep;
			}
			tot += en;
			if(en) *e -= ((double)en/(double)size)*log2((double)en/(double)size);
			//printf("rest = %d decision[%d] = %d, value[%d] = %d step = %d e = %f \n", rest, i/step, i, i/step, i+hstep, step, ((double)en/(double)size)*log2((double)en/(double)size));
			en=0;
			for(j= 0; j< step; j++) {
				sum += distrib[HALF-i-j]*(hstep-j)*(hstep-j);
				en += distrib[HALF-i-j];
				q[128 -i-j] = -i-hstep;
			}
			if(en) *e -= ((double)en/(double)size)*log2((double)en/(double)size);
			tot += en;
			//printf("rest = %d decision[%d] = %d, value[%d] = %d step = %d e = %f \n", rest, i/step, -i, i/step, -i-hstep, step, ((double)en/(double)size)*log2((double)en/(double)size));
		}
		if(rest){
			last = rest; en = 0;
			lasth = (last>>1);
			for(j= 0; j< last ; j++) {
				sum += distrib[HALF+i+j]*(j-lasth)*(j-lasth);
				en += distrib[HALF+i+j];
				q[128 +i+j] = i+lasth;
			}
			tot += en;
			if(en) *e -= ((double)en/(double)size)*log2((double)en/(double)size);
			//printf("rest = %d decision[%d] = %d, value[%d] = %d step = %d e = %f \n", rest, i/step, i, i/step, i+lasth, last, ((double)en/(double)size)*log2((double)en/(double)size));
			en=0;
			for(j= 0; j< last; j++) {
				sum += distrib[HALF-i-j]*(lasth-j)*(lasth-j);
				en += distrib[HALF-i-j];
				q[128 -i-j] = -i-lasth;
			}
			if(en) *e -= ((double)en/(double)size)*log2((double)en/(double)size);
			tot += en;
			//printf("rest = %d decision[%d] = %d, value[%d] = %d step = %d e = %f \n", rest, i/step, -i, i/step, -i-lasth, last, ((double)en/(double)size)*log2((double)en/(double)size));
		}
		printf("distr = %f entrop = %f tot = %d size = %d\n",(double)sum/(double)size, *e, tot, size);
	}
	*dis = (double)sum/(double)size;
	//for(i=0; i<256;i++) printf("q[%d]=%d\n",i,q[i]);
	//printf("distr = %f entrop = %f tot = %d size = %d\n",dis, e, tot, size);
	//return (double)sum/(double)size;
}

//For test only----------------------------------
void utils_unifom_dist_entr(uint32 *distrib, uint32 bits, uint32 step, uint32 size, uint32 *q, double *dis, double *e)
/*! \fn static inline int dist_unifom_8(uint32 *distrib, const uint32 bit)
	\brief Calculate distortion for the given uniform quantizer.
    \param distrib	 	The pointer to array of distribution probabilities.
    \param bits 		The size of distribution probabilities array.
	\param step			The step of quantization.
	\param sub			The number of subband
	\param size			The number of pixels in subband
	\param q			The quntization array
	\param dis			The subband distortion
	\param en			The subband entropy
 */
{

	uint32 sum = 0, en=0, tot=0;
	int i, j, rest;
	uint32  hstep = step>>1, num = (1<<(bits-1)), st;  // The half size of interval
	//printf("distrib = %p\n", distrib);
	//for(i=0; i< (1<<bits); i++)  printf("distrib[%d] = %d\n", i, distrib[i]); //if(distrib[i])
	*e=0.;
	for(j= (1-step); j< (int)step; j++) {
		sum += distrib[num+j]*j*j;
		en += distrib[num+j];
		q[num+i+j] = 0;
		//printf("sum = %d en = %d\n", sum, en);
	}
	if(en) *e -= ((double)en/(double)size)*log2((double)en/(double)size);
	tot += en;

	rest = num-step;
	for(i=step; rest > 0; i+=step){
		en = 0;
		st = (rest<step) ? rest : step;
		for(j= 0; j< st; j++) {
			//printf("distrib[%d] = %d\n", num+i+j, distrib[num+i+j]);
			sum += distrib[num+i+j]*(j-hstep)*(j-hstep);
			en += distrib[num+i+j];
			q[num +i+j] = i+(st>>1);
		}
		if(en) *e -= ((double)en/(double)size)*log2((double)en/(double)size);
		tot += en;
		//printf("tot = %d i = %d rest = %d en = %d st = %d e = %f num = %d\n", tot, i, rest, en, st, ((double)en/(double)size)*log2((double)en/(double)size), num-i-j);
		rest -= step;
	}
	rest = num-step+1;
	for(i=step; rest > 0; i+=step){
		en=0;
		st = (rest<step) ? rest : step;
		for(j= 0; j< st; j++) {
			//printf("distrib[%d] = %d\n", num-i-j, distrib[num-i-j]);
			sum += distrib[num-i-j]*(hstep-j)*(hstep-j);
			en += distrib[num-i-j];
			q[num -i-j] = -i-(st>>1);
		}
		if(en) *e -= ((double)en/(double)size)*log2((double)en/(double)size);
		tot += en;
		//printf("tot = %d i = %d rest = %d en = %d st = %d e = %f num = %d\n", tot, i, rest, en, st, ((double)en/(double)size)*log2((double)en/(double)size), num-i-j);
		rest -= step;
	}
	printf("total = %d size = %d\n", tot, size);
	*dis = (double)sum/(double)size;
	//for(i=0; i<256;i++) printf("q[%d]=%d\n",i,q[i]);
	//printf("distr = %f entrop = %f tot = %d size = %d\n",dis, e, tot, size);
	//return (double)sum/(double)size;
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


double dist3(uint8 *before, uint8 *after, uint32 dim, uint32 d)
/*! \fn double dist(uint8 *before, uint8 *after, uint32 dim, uint32 d)
    \brief Calculate distortion of two image.
    \param before	Pointer to first image.
    \param after	Pointer to second image.
    \param dim 		Size of image height*width.
    \param d 		d = 1 if gray image, d = 3 if color image.
    \retval 		The distortion.
*/
{
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

double ape(uint8 *before, uint8 *after, uint32 dim, uint32 d)
/*! \fn double ape(uint8 *before, uint8 *after, uint32 dim, uint32 d)
    \brief Calculate APE of two image.
    \param before	Pointer to first image.
    \param after	Pointer to second image.
    \param dim 		Size of image height*width.
    \param d 		d = 1 if gray image, d = 3 if color image.
    \retval 		The APE.
*/
{
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

double ape3(uint8 *before, uint8 *after, uint32 dim, uint32 d)
/*! \fn double ape(uint8 *before, uint8 *after, uint32 dim, uint32 d)
    \brief Calculate APE of two image.
    \param before	Pointer to first image.
    \param after	Pointer to second image.
    \param dim 		Size of image height*width.
    \param d 		d = 1 if gray image, d = 3 if color image.
    \retval 		The APE.
*/
{
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


double psnr3(uint8 *before, uint8 *after, uint32 dim, uint32 d)
/*! \fn double psnr(uint8 *before, uint8 *after, uint32 dim, uint32 d)
    \brief Calculate PSNR of two image.
    \param before	Pointer to first image.
    \param after	Pointer to second image.
    \param dim 		Size of image height*width.
    \param d 		d = 1 if gray image, d = 3 if color image.
    \retval 		The PSNR.
*/
{
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

uint32 utils_read_ppm(const char *filename, FILE *wl, uint32 *w, uint32 *h, uint32 *bpp, uint8 **img)
{
    //FILE *wl;
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
		fgets(line, 100, wl);
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

	*img = (uint8 *)calloc((*w)*(*h)*(*bpp)*3, sizeof(uint8));
	byts = fread(*img, sizeof(uint8), (*w)*(*h)*(*bpp)*3,  wl);
	if(byts != (*w)*(*h)*(*bpp)*3){ printf("Image read error\n");}
	//printf("byts = %d size = %d\n", byts, (*w)*(*h)*(*bpp)*3);
    fclose(wl);
	//fclose(wl);
	return byts;
}

void utils_rgb2bayer(uint8 *rgb, uint8 *bay, uint32 w, uint32 h)
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


