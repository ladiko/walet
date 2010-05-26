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



static inline void drawrect(uchar *rgb, imgtype *im, uint32 h0, uint32 w0, uint32 w, uint32 h, uint32 size, uint32 shift)
{
	uint32 x, y;
	for(y=0; y < h; y++ ){
		for(x=0; x < w; x++){
			rgb[3*((y+h0)*size+w0+x)]   = rnd(shift+im[y*w+x]);
			rgb[3*((y+h0)*size+w0+x)+1] = rnd(shift+im[y*w+x]);
			rgb[3*((y+h0)*size+w0+x)+2] = rnd(shift+im[y*w+x]);
		}
	}
}

uchar* utils_subband_draw(uchar *rgb, Image *img, ColorSpace color, uint32 steps)
//For vizualisation only
{
	uint32 k, h, w, h0, w0, h1, w1, st;
	int i;
	imgtype* im;
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
					im = &img->img[sub[k*st + 3*i-2].loc];
					drawrect(rgb, im, h1, w1+w0, h, w, img->width, 128);

					h = sub[k*st + 3*i-1].size.y; w = sub[k*st + 3*i-1].size.x;
					im = &img->img[sub[k*st + 3*i-1].loc];
					drawrect(rgb, im, h1+h0, w1, h, w, img->width, 128);

					h = sub[k*st + 3*i].size.y; w = sub[k*st + 3*i].size.x;
					im = &img->img[sub[k*st + 3*i].loc];
					drawrect(rgb, im, h1+h0, w1+w0, h, w, img->width, 128);
				}
				h = sub[k*st ].size.y; w = sub[k*st].size.x;
				im = &img->img[sub[k*st].loc];
				drawrect(rgb, im, h1, w1, h, w, img->width, 128);
			}
		} else {
			h0 = sub[0 ].size.y; w0 = sub[0 ].size.x;
			h  = sub[1].size.y; w  = sub[1].size.x;
			im = &img->img[sub[1].loc];
			drawrect(rgb, im, 0, w0, h, w, img->width, 128);

			h = sub[2].size.y; w = sub[2].size.x;
			im = &img->img[sub[2].loc];
			drawrect(rgb, im, h0, 0, h, w, img->width, 128);

			h = sub[3].size.y; w = sub[3].size.x;
			im = &img->img[sub[3].loc];
			drawrect(rgb, im, h0, w0, h, w, img->width, 128);

			h = sub[0].size.y; w = sub[0].size.x;
			im = &img->img[sub[0].loc];
			drawrect(rgb, im, 0, 0, h, w, img->width, 0);
		}
	} else {
		for(i=steps; i>0; i--){
			h0 = sub[3*i-2].size.y; w0 = sub[3*i-1].size.x;
			h  = sub[3*i-2].size.y; w  = sub[3*i-2].size.x;
			im = &img->img[sub[3*i-2].loc];
			drawrect(rgb, im, 0, w0, h, w, img->width, 128);

			h = sub[3*i-1].size.y; w = sub[3*i-1].size.x;
			im = &img->img[sub[3*i-1].loc];
			drawrect(rgb, im, h0, 0, h, w, img->width, 128);

			h = sub[3*i].size.y; w = sub[3*i].size.x;
			im = &img->img[sub[3*i].loc];
			drawrect(rgb, im, h0, w0, h, w, img->width, 128);
		}
		h = sub[0].size.y; w = sub[0].size.x;
		im = &img->img[sub[0].loc];
		drawrect(rgb, im, 0, 0, h, w, img->width, 0);
	}
	return rgb;
}

#define oe(a,x)	(a ? x&1 : (x+1)&1)
#define lb(x) (x&0xFF)

uchar* utils_bayer_to_rgb(imgtype *img, uchar *rgb, uint32 w, uint32 h,  BayerGrid bay)
/*! \fn void bayer_to_rgb(uchar *rgb)
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
	/*
	for(y=0, yw=0, yw1=0 ; y < h1; y++, yw+=w, yw1+=w1){
		for(x=0; x < w1; x++){
			y2 = oe(a,y);
			x2 = oe(b,x);
			xwy = x + yw;
			wy = (x + yw1);
			xwy3 = wy + wy + wy;
			rgb[xwy3    ] = (y2 ? (x2 ?  img[xwy    ] : img[xwy+1]) : (x2 ? img[xwy+w] : img[xwy+w+1]))&0xFF;
			rgb[xwy3 + 1] = (y2 ? (x2 ? (img[xwy+w  ] + img[xwy+1])>>1 :   (img[xwy  ] + img[xwy+w+1])>>1) :
								  (x2 ? (img[xwy+w+1] + img[xwy  ])>>1 :   (img[xwy+1] + img[xwy+w  ])>>1))&0xFF;
			rgb[xwy3 + 2] = (y2 ? (x2 ?  img[xwy+w+1] : img[xwy+w]) : (x2 ? img[xwy+1] : img[xwy    ]))&0xFF;
		}
	}*/
	for(y=0, yw=0, yw1=0 ; y < h1; y++, yw+=w, yw1+=w1){
		for(x=0; x < w1; x++){
			y2 = oe(a,y);
			x2 = oe(b,x);
			xwy = x + yw;
			wy = (x + yw1);
			xwy3 = wy + wy + wy;
			rgb[xwy3    ] = y2 ? (x2 ?  lb(img[xwy    ]) : lb(img[xwy+1])) : (x2 ? lb(img[xwy+w]) : lb(img[xwy+w+1]));
			rgb[xwy3 + 1] = y2 ? (x2 ? (lb(img[xwy+w  ]) + lb(img[xwy+1]))>>1 :   (lb(img[xwy  ]) + lb(img[xwy+w+1]))>>1) :
								 (x2 ? (lb(img[xwy+w+1]) + lb(img[xwy  ]))>>1 :   (lb(img[xwy+1]) + lb(img[xwy+w  ]))>>1);
			rgb[xwy3 + 2] = y2 ? (x2 ?  lb(img[xwy+w+1]) : lb(img[xwy+w])) : (x2 ? lb(img[xwy+1]) : lb(img[xwy    ]));
		}
	}
	return rgb;
}

uchar* utils_grey_to_rgb(imgtype *img, uchar *rgb, uint32 w, uint32 h)
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

imgtype* utils_cat(imgtype *img, imgtype *img1, uint32 w, uint32 h, uint32 bits)
{
	int i, dim = h*w, sh = bits-8;
	for(i = 0; i < dim; i++) img1[i] = img[i]<0 ? 0 : rnd(img[i]>>sh);
	return img1;
}

void utils_bayer_to_Y(imgtype *img, imgtype *img1, uint32 w, uint32 h, BayerGrid bay)
{
	uint32 x, y, wy, xwy, y2, x2, a, b, h1 = h-1, w1 = w-1, yw, yw1;

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
			wy = (x + yw1);
			//xwy3 = wy + wy + wy;
			img1[wy] = 	((y2 ? (x2 ?  lb(img[xwy    ]) : lb(img[xwy+1])) : (x2 ? lb(img[xwy+w]) : lb(img[xwy+w+1])))>>2) +
						((y2 ? (x2 ? (lb(img[xwy+w  ]) + lb(img[xwy+1]))>>1 :   (lb(img[xwy  ]) + lb(img[xwy+w+1]))>>1) :
							   (x2 ? (lb(img[xwy+w+1]) + lb(img[xwy  ]))>>1 :   (lb(img[xwy+1]) + lb(img[xwy+w  ]))>>1))>>1) +
						((y2 ? (x2 ?  lb(img[xwy+w+1]) : lb(img[xwy+w])) : (x2 ? lb(img[xwy+1]) : lb(img[xwy    ])))>>2);
		}
	}
}

static inline int median(imgtype *s, uint32 thresh)
///	\fn static inline int median_filter_3x3(imgtype *s)
///	\brief The not optimal algorithm finding median element.
///	\param s	 	Input array.
///	\param p	 	Temporary array.
{
	uint32 j, p[9];
	for(j=0; j<8; j++){
		if(j&1) {
			s[0] = p[0];
			if(p[1] > p[2]) { s[2] = p[1]; s[1] = p[2];} else { s[1] = p[1]; s[2] = p[2];}
			if(p[3] > p[4]) { s[4] = p[3]; s[3] = p[4];} else { s[3] = p[3]; s[4] = p[4];}
			if(p[5] > p[6]) { s[6] = p[5]; s[5] = p[6];} else { s[5] = p[5]; s[6] = p[6];}
			if(p[7] > p[8]) { s[8] = p[7]; s[7] = p[8];} else { s[7] = p[7]; s[8] = p[8];}
		}
		else {
			if(s[0] > s[1]) { p[1] = s[0]; p[0] = s[1];} else { p[0] = s[0]; p[1] = s[1];}
			if(s[2] > s[3]) { p[3] = s[2]; p[2] = s[3];} else { p[2] = s[2]; p[3] = s[3];}
			if(s[4] > s[5]) { p[5] = s[4]; p[4] = s[5];} else { p[4] = s[4]; p[5] = s[5];}
			if(s[6] > s[7]) { p[7] = s[6]; p[6] = s[7];} else { p[6] = s[6]; p[7] = s[7];}
			p[8] = s[8];
		}
	}
	return s[4];
}

static inline int gradient(imgtype *s, uint32 thresh)
///	\fn static inline int edge_detect(imgtype *s, uint32 thresh)
///	\brief Color threshold edge detector.
///	\param s	 	Input array.
///	\param thresh	The color threshold.
{
	uint32 diff, tmp = 0;
	diff = (abs(s[1]-s[5])>thresh) ? abs(s[1]-s[5]) : 0; if(tmp < diff) tmp = diff | 1<<8;
	diff = (abs(s[2]-s[6])>thresh) ? abs(s[1]-s[5]) : 0; if(tmp < diff) tmp = diff | 2<<8;
	diff = (abs(s[3]-s[7])>thresh) ? abs(s[1]-s[5]) : 0; if(tmp < diff) tmp = diff | 3<<8;
	diff = (abs(s[0]-s[4])>thresh) ? abs(s[1]-s[5]) : 0; if(tmp < diff) tmp = diff | 4<<8;
	//diff = abs(s[1]-s[5]); if(tmp < diff) tmp = diff | 1<<8;
	//diff = abs(s[1]-s[5]); if(tmp < diff) tmp = diff | 2<<8;
	//diff = abs(s[1]-s[5]); if(tmp < diff) tmp = diff | 3<<8;
	//diff = abs(s[1]-s[5]); if(tmp < diff) tmp = diff | 4<<8;
	return tmp;
}

static inline int edge(imgtype *s, uint32 thresh)
///	\fn static inline int edge_filter(imgtype *s)
///	\brief Color threshold edge detector.
///	\param s	 	Input array.
{
	uint32 tmp = 0;
	/*if(s[8]){
		if((s[1]>>8) == 1) if(tmp < s[1]) tmp = s[1];
		if((s[5]>>8) == 1) if(tmp < s[5]) tmp = s[5];
		if((s[2]>>8) == 2) if(tmp < s[2]) tmp = s[2];
		if((s[6]>>8) == 2) if(tmp < s[6]) tmp = s[6];
		if((s[3]>>8) == 3) if(tmp < s[3]) tmp = s[3];
		if((s[7]>>8) == 3) if(tmp < s[7]) tmp = s[7];
		if((s[0]>>8) == 4) if(tmp < s[0]) tmp = s[0];
		if((s[4]>>8) == 4) if(tmp < s[4]) tmp = s[4];
		return tmp > s[8] ? 0 : 255;
	}*/
	if(s[8]){
		if((s[8]>>8) == 1){
			if((s[1]>>8) == 1) if(tmp < s[1]) tmp = s[1];
			if((s[5]>>8) == 1) if(tmp < s[5]) tmp = s[5];
		}
		if((s[8]>>8) == 2){
			if((s[2]>>8) == 2) if(tmp < s[2]) tmp = s[2];
			if((s[6]>>8) == 2) if(tmp < s[6]) tmp = s[6];
		}
		if((s[8]>>8) == 3){
			if((s[3]>>8) == 3) if(tmp < s[3]) tmp = s[3];
			if((s[7]>>8) == 3) if(tmp < s[7]) tmp = s[7];
		}
		if((s[8]>>8) == 4){
			if((s[0]>>8) == 4) if(tmp < s[0]) tmp = s[0];
			if((s[4]>>8) == 4) if(tmp < s[4]) tmp = s[4];
		}
		return tmp > s[8] ? 0 : 255;
	}
	return 0;
}


#define bay_switch(i,x,y)  ((i) ? ((!((y)&1) && (x)&1) || ((y)&1 && !((x)&1))) : ((!((y)&1) && !((x)&1)) || ((y)&1 && (x)&1)))

static void get_bayer_3x3(imgtype *img, imgtype *img1, uint32 w, uint32 h, BayerGrid bay, uint32 param, FP func)
///	\fn tatic void get_bayer_3x3(imgtype *img, imgtype *img1, uint32 h, uint32 w, BayerGrid bay, uint32 param, FP func)
///	\brief Get 3x3 matrix for each color from bayer pattern.
///	\param img	 		Input image.
///	\param img1 		Temporary bufer.
///	\param h 			Image height.
///	\param w 			Image width.
///	\param bay 			Bayer pattern.
/// \param param		Any parameter.
/// \param fuc			Pointer to function.
{
	uint32 x, y, sx = w-2, sy = h-2, wy, xwy, w1;
	imgtype s[9];

	//printf("Start median filter bay = %d\n",bay);
	for(y=2, wy = w+w; y < sy; y++, wy+=w){
		for(x=2; x < sx; x++){
			xwy = x + wy;
			if(bay_switch(bay == BGGR || bay == RGGB,x,y)) {
				w1 = w<<1;
				//for(i=0; i<9; i++) s[i] = 0;
				w1 = w<<1;
				s[0] = img[xwy - w1];		s[1] = img[xwy + 1 - w]; 	s[2] = img[xwy + 2];
				s[7] = img[xwy - 1 - w]; 	s[8] = img[xwy ]; 			s[3] = img[xwy + 1 + w];
				s[6] = img[xwy - 2 ]; 		s[5] = img[xwy -1 + w]; 	s[4] = img[xwy + w1];
			}
			else  	{
				w1 = w<<1;
				s[0] = img[xwy - 2 - w1]; 	s[1] = img[xwy - w1]; 	s[2] = img[xwy + 2 - w1];
				s[7] = img[xwy - 2 ]; 		s[8] = img[xwy ]; 		s[3] = img[xwy + 2];
				s[6] = img[xwy - 2 + w1]; 	s[5] = img[xwy + w1]; 	s[4] = img[xwy + 2 + w1];
			}
			img1[xwy] = (*func)(s,param);
		}
	}
	//Copy image back
	for(y=2, wy = w+w; y < sy; y++, wy+=w){
		for(x=2; x < sx; x++){
			xwy = x + wy;
			img[xwy] = img1[xwy];
		}
	}
}

void utils_bayer_median_filter_3x3(imgtype *img, imgtype *img1, uint32 w, uint32 h, BayerGrid bay)
{
	get_bayer_3x3(img, img1, w, h, bay, 0, &median);
}

void utils_bayer_gradient(imgtype *img, imgtype *img1, uint32 w, uint32 h, BayerGrid bay, uint32 thresh)
{
	get_bayer_3x3(img, img1, w, h, bay, thresh, &gradient);
}

static void get_3x3(imgtype *img, imgtype *img1, uint32 w, uint32 h, uint32 param, FP func)
///	\fn static void get_3x3(imgtype *img, imgtype *img1, uint32 h, uint32 w, uint32 param, FP func)
///	\brief Get 3x3 matrix of pixels.
///	\param img	 		Input image.
///	\param img1 		Temporary bufer.
///	\param h 			Image height.
///	\param w 			Image width.
/// \param param		Any parameter.
/// \param fuc			Pointer to function.
{
	uint32 x, y, sx = w-1, sy = h-1, wy, xwy;
	imgtype s[9];

	//printf("Start median filter bay = %d\n",bay);
	for(y=1, wy = w; y < sy; y++, wy+=w){
		for(x=1; x < sx; x++){
			xwy = x + wy;
			s[0] = img[xwy - 1 - w]; 	s[1] = img[xwy - w]; 	s[2] = img[xwy + 1 - w];
			s[7] = img[xwy - 1 ]; 		s[8] = img[xwy ]; 		s[3] = img[xwy + 1];
			s[6] = img[xwy - 1 + w]; 	s[5] = img[xwy + w]; 	s[4] = img[xwy + 1 + w];
			img1[xwy ] = (*func)(s, param);
		}
	}
	//Copy image back
	for(y=1, wy = w; y < sy; y++, wy+=w){
		for(x=1; x < sx; x++){
			xwy = x + wy;
			img[xwy] = img1[xwy];
		}
	}
}

void utils_gradient(imgtype *img, imgtype *img1, uint32 w, uint32 h, uint32 thresh)
{
	get_3x3(img, img1, w, h, thresh, &gradient);
}

void utils_edge_detector(imgtype *img, imgtype *img1, uint32 w, uint32 h)
{
	get_3x3(img, img1, w, h, 0, &edge);
}

void utils_fill_bayer_hist(imgtype *img, uint32 *r, uint32 *g, uint32 *b, uint32 w, uint32 h,  BayerGrid bay, uint32 bits)
{
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

#define line(a,x)  (a*x)
#define bt709(maxx,maxy,x)	(((x) < (0.018*maxx)) ? 4.5*maxy*(x)/maxx : (1.099*pow(x/maxx,0.45)-0.099)*maxy)
#define srgb(maxx,maxy,x)	(((x) < (0.00313*maxx)) ? 12.92*maxy*(x)/maxx : (1.055*pow(x/maxx,0.417)-0.055)*maxy)

void utils_color_table(uint32 *hist, uint16 *look, uint32 in_bits, uint32 out_bits, Gamma gamma)
{
	uint32 i, j, sz = (1<<in_bits), sz1 = (1<<out_bits)-1, ind;
	uint32 *h[3] = {hist, &hist[1<<in_bits], &hist[1<<(in_bits+1)]};
	uint16 *l[3] = {look, &look[1<<in_bits], &look[1<<(in_bits+1)]};
	double avr[3], max[3], a[3], p[3], sum, sum1;  //min[3],c[3],

	for(j=0; j<3; j++) {
		sum = 0.; sum1 = 0.;
		for(i=0; i<sz; i++) sum += h[j][i]*i;
		for(i=0; i<sz; i++) sum1 += h[j][i];
		avr[j] = (double)sum/(double)sum1;
		//for(i=0; i<sz; i++) if(h[j][i]>0) break; min[j] = i;
		for(i=sz-1; i; i--) if(h[j][i]>0) break; max[j] = i;
	}
	//Find maximum of average
	ind = (avr[0]   < avr[1]) ? 1 : 0;
	ind = (avr[ind] < avr[2]) ? 2 : ind;

	for(j=0; j<3; j++) {
		p[j] = avr[j]*max[ind]/avr[ind];
		a[j] = (double)sz1/p[j];
		printf("max = %f avr = %f a = %f p = %f\n", max[j], avr[j], a[j], p[j]);

		//for(i=0; i<sz; i++) l[j][i] = (i<min[j]) ? 0 : ((i>p[j]) ? sz1 : (uint32)(a[j]*(double)i + c[j]));
		switch(gamma){
		case(LINEAR) : { for(i=0; i<sz; i++) l[j][i] = (i>p[j]) ? sz1 : (uint32)line(a[j],(double)i); break;}
		case(BT709)  : { for(i=0; i<sz; i++) l[j][i] = (i>p[j]) ? sz1 : (uint32)bt709(max[j],(double)sz1,(double)i); break;}
		case(sRGB)   : { for(i=0; i<sz; i++) l[j][i] = (i>p[j]) ? sz1 : (uint32)srgb(max[j],(double)sz1,(double)i); break;}
		}
	}
}

void utils_white_balance(imgtype *in, imgtype *out, uint32 *hist, uint16 *look, uint32 w, uint32 h, BayerGrid bay, uint32 in_bits, uint32 out_bits, Gamma gamma)
{
	uint32 i, x, y, size = h*w;
	uint16 *c[4];
	uint16 *l[3] = {look, &look[1<<in_bits], &look[1<<(in_bits+1)]};

	utils_color_table(hist, look, in_bits, out_bits, gamma);

	switch(bay){
		case(BGGR):{ c[0] = l[0]; c[1] = l[1]; c[2] = l[1]; c[3] = l[2]; break;}
		case(GRBG):{ c[0] = l[1]; c[1] = l[2]; c[2] = l[0]; c[3] = l[1]; break;}
		case(GBRG):{ c[0] = l[1]; c[1] = l[0]; c[2] = l[2]; c[3] = l[1]; break;}
		case(RGGB):{ c[0] = l[2]; c[1] = l[1]; c[2] = l[1]; c[3] = l[0]; break;}
	}

	for(i=0, x=0, y=0; i < size; i++, x++){
		if(x == w) { x=0; y++;}
		if(y&1)
			if(x&1) out[i] = c[0][in[i]];
			else 	out[i] = c[1][in[i]];
		else
			if(x&1)	out[i] = c[2][in[i]];
			else 	out[i] = c[3][in[i]];
	}
}

void unifom_8bit(uint32 *distrib, uint32 bits, uint32 step, uchar sub, uint32 size, uint32 *q, double *dis, double *e)
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

uchar* wavelet_to_rgb(imgtype *img, uchar *rgb, int height, int width, int step)
{ 
	int i, j, k, t, dim = height*width;
	int w3 = width*3;
	imgtype *p;
	
	if((p = (imgtype*)malloc(dim * sizeof(imgtype))) == NULL){
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

uchar* wavelet_to_rgb1(imgtype *img, uchar *rgb, int height, int width, int step)
{ 
	int i, j, k, t, dim = height*width;
	int w3 = width*3;
	imgtype *p;
	
	if((p = (imgtype*)malloc(dim*sizeof(imgtype))) == NULL){
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
			rgb[ i*w3 + 3*k]  			   = (uchar)clip(128 + p[i*width + k]);
			rgb[ i*w3 + 3*k + 1] 		   = (uchar)clip(128 + p[i*width + k]);
			rgb[ i*w3 + 3*k + 2] 		   = (uchar)clip(128 + p[i*width + k]);
		}
	}
	free(p);
	return rgb;
}

uchar* img_to_rgb128(imgtype *img, uchar *rgb, int height, int width)
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


uchar* uchar_to_rgb(uchar *img, uchar *rgb, int height, int width)
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
void copy(imgtype *in, imgtype *out, int w, int h)
{
	int i, dim = h*w;
	for(i = 0; i < dim; i++){
		out[i] = in[i];
	}
}

//Compare two image
void compare(imgtype *in, imgtype *out, int w, int h)
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
void resizeonepix(uchar *in, int w, int h)
{
	int x, y;
	for(y = 0; y < (h-1); y++){
		for(x=0; x < (w-1); x++){
			in[y*(w-1)+x] = in[y*w+x];
		}
	}
}

uchar* malet_to_rgb(imgtype *img, uchar *rgb, int h, int w, int step)
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

double dist(imgtype *before, imgtype *after, uint32 dim, uint32 d)
/*! \fn double dist(imgtype *before, imgtype *after, uint32 dim, uint32 d)
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

double dist3(uchar *before, uchar *after, uint32 dim, uint32 d)
/*! \fn double dist(imgtype *before, imgtype *after, uint32 dim, uint32 d)
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

double ape(imgtype *before, imgtype *after, uint32 dim, uint32 d)
/*! \fn double ape(imgtype *before, imgtype *after, uint32 dim, uint32 d)
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

double ape3(uchar *before, uchar *after, uint32 dim, uint32 d)
/*! \fn double ape(imgtype *before, imgtype *after, uint32 dim, uint32 d)
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

double psnr(imgtype *before, imgtype *after, uint32 dim, uint32 d)
/*! \fn double psnr(imgtype *before, imgtype *after, uint32 dim, uint32 d)
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
double psnr3(uchar *before, uchar *after, uint32 dim, uint32 d)
/*! \fn double psnr(imgtype *before, imgtype *after, uint32 dim, uint32 d)
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

uchar* YUV_to_RGB(uchar *rgb, imgtype *y, imgtype *u, imgtype *v, uint32 sq)
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

uchar* YUV420p_to_RGB(uchar *rgb, imgtype *y, imgtype *u, imgtype *v, uint32 w, uint32 h)
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
