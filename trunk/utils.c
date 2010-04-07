#include <walet.h>

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <math.h>

#define ll(step, x, y) img[x*step + y*step*width];
#define hl(step, x, y) img[x*step + (step>>1)  + y*step*width];
#define lh(step, x, y) img[x*step + (step>>1)*width  + y*step*width];
#define hh(step, x, y) img[x*step + (step>>1) + (step>>1)*width  + y*step*width];

//#define clip(x)		x < 0 ? (x < -255 ? 255 : -x) : ( x > 255 ? 255 : x);
#define clip(x)		abs(x);



static inline void drawrect(uchar *rgb, imgtype *im, uint32 h0, uint32 w0, uint32 h, uint32 w, uint32 size, uint32 shift)
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

uchar* utils_subband_draw(uchar *rgb, Image *img, Subband **sub, ColorSpace color, uint32 steps)
//For vizualisation only
{
	uint32 k, h, w, h0, w0, h1, w1;
	int i;
	imgtype* im;
	//for(i=0; i<=steps*3; i++) printf("sub %d h %d w %d p %p\n",sub[0][i]->subb, sub[0][i]->size.y, sub[0][i]->size.x,  sub[0][i ]);
	if(color == BAYER ){
		if(steps > 1){
			//k=1;{
			for(k=0; k<4; k++) {
				switch(k){
					case 0 : {h1 = 0; w1 = 0; break;}
					case 1 : {h1 = 0; w1 = sub[k][3*(steps-1)-1].size.x + sub[k][3*(steps-1)-2].size.x; break;}
					case 2 : {h1 = sub[k][3*(steps-1)-2].size.y + sub[k][3*(steps-1)-1].size.y; w1 = 0; break;}
					case 3 : {h1 = sub[k][3*(steps-1)-2].size.y + sub[k][3*(steps-1)-1].size.y;
							  w1 = sub[k][3*(steps-1)-1].size.x + sub[k][3*(steps-1)-2].size.x; break;}
				}
				for(i=(steps-1); i>0; i--){
					h0 = sub[k][3*i-2].size.y; w0 = sub[k][3*i-1].size.x;
					h = sub[k][3*i-2].size.y; w = sub[k][3*i-2].size.x;
					im = &img->img[sub[k][3*i-2].loc];
					drawrect(rgb, im, h1, w1+w0, h, w, img->size.x, 128);

					h = sub[k][3*i-1].size.y; w = sub[k][3*i-1].size.x;
					im = &img->img[sub[k][3*i-1].loc];
					drawrect(rgb, im, h1+h0, w1, h, w, img->size.x, 128);

					h = sub[k][3*i].size.y; w = sub[k][3*i].size.x;
					im = &img->img[sub[k][3*i].loc];
					drawrect(rgb, im, h1+h0, w1+w0, h, w, img->size.x, 128);
				}
				h = sub[k][0].size.y; w = sub[k][0].size.x;
				im = &img->img[sub[k][0].loc];
				drawrect(rgb, im, h1, w1, h, w, img->size.x, 128);
			}
		} else {
			h0 = sub[1][0].size.y; w0 = sub[2][0].size.x;
			h  = sub[1][0].size.y; w  = sub[1][0].size.x;
			im = &img->img[sub[1][0].loc];
			drawrect(rgb, im, 0, w0, h, w, img->size.x, 128);

			h = sub[2][0].size.y; w = sub[2][0].size.x;
			im = &img->img[sub[2][0].loc];;
			drawrect(rgb, im, h0, 0, h, w, img->size.x, 128);

			h = sub[3][0].size.y; w = sub[3][0].size.x;
			im = &img->img[sub[3][0].loc];
			drawrect(rgb, im, h0, w0, h, w, img->size.x, 128);

			h = sub[0][0].size.y; w = sub[0][0].size.x;
			im = &img->img[sub[0][0].loc];
			drawrect(rgb, im, 0, 0, h, w, img->size.x, 0);
		}
	} else {
		for(i=steps; i>0; i--){
			h0 = sub[0][3*i-2].size.y; w0 = sub[0][3*i-1].size.x;
			h  = sub[0][3*i-2].size.y; w  = sub[0][3*i-2].size.x;
			im = &img->img[sub[0][3*i-2].loc];
			drawrect(rgb, im, 0, w0, h, w, img->size.x, 128);

			h = sub[0][3*i-1].size.y; w = sub[0][3*i-1].size.x;
			im = &img->img[sub[0][3*i-1].loc];
			drawrect(rgb, im, h0, 0, h, w, img->size.x, 128);

			h = sub[0][3*i].size.y; w = sub[0][3*i].size.x;
			im = &img->img[sub[0][3*i].loc];
			drawrect(rgb, im, h0, w0, h, w, img->size.x, 128);
		}
		h = sub[0][0].size.y; w = sub[0][0].size.x;
		im = &img->img[sub[0][0].loc];
		drawrect(rgb, im, 0, 0, h, w, img->size.x, 0);
	}
	return rgb;
}

uchar* utils_bayer_to_rgb(imgtype *img, uchar *rgb, uint32 h, uint32 w,  BayerGrid bay)
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
	uint32 x, y, wy, xwy, xwy3, y2, x2, a, b, h1 = h-1, w1 = w-1;

	switch(bay){
		case(BGGR):{ a = 1; b = 1; break;}
		case(GRBG):{ a = 0; b = 1; break;}
		case(GBRG):{ a = 1; b = 0; break;}
		case(RGGB):{ a = 0; b = 0; break;}
	}

	for(y=0; y < h1; y++){
		for(x=0; x < w1; x++){
			y2 = oe(a,y);
			x2 = oe(b,x);
			wy = w*y;
			xwy = x + wy;
			xwy3 = (x + w1*y)*3;
			//rgb[xwy3    ] = y2 ? (x2 ?  img[xwy    ] : img[xwy+1]) : (x2 ? img[xwy+w] : img[xwy+w+1]);
			//rgb[xwy3 + 1] = y2 ? (x2 ? (img[xwy+w  ] + img[xwy+1])>>1 :   (img[xwy  ] + img[xwy+w+1])>>1) :
			//					 (x2 ? (img[xwy+w+1] + img[xwy  ])>>1 :   (img[xwy+1] + img[xwy+w  ])>>1);
			//rgb[xwy3 + 2] = y2 ? (x2 ?  img[xwy+w+1] : img[xwy+w]) : (x2 ? img[xwy+1] : img[xwy    ]);
			rgb[xwy3    ] = y2 ? (x2 ?  img[xwy    ] : img[xwy+1]) : (x2 ? img[xwy+w] : img[xwy+w+1]);
			rgb[xwy3 + 1] = y2 ? (x2 ? (img[xwy+w  ] + img[xwy+1])>>1 :   (img[xwy  ] + img[xwy+w+1])>>1) :
								 (x2 ? (img[xwy+w+1] + img[xwy  ])>>1 :   (img[xwy+1] + img[xwy+w  ])>>1);
			rgb[xwy3 + 2] = y2 ? (x2 ?  img[xwy+w+1] : img[xwy+w]) : (x2 ? img[xwy+1] : img[xwy    ]);
		}
	}
	return rgb;
}

uchar* utils_grey_to_rgb(imgtype *img, uchar *rgb, uint32 height, uint32 width)
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

imgtype* utils_cat(imgtype *img, imgtype *img1, uint32 height, uint32 width, uint32 bits)
{
	int i, dim = height*width, sh = bits-8;
	for(i = 0; i < dim; i++) img1[i] = img[i]<0 ? 0 : rnd(img[i]>>sh);
	return img1;
}


void utils_fill_hist(imgtype *img, uint32 *r, uint32 *g, uint32 *b, uint32 h, uint32 w,  BayerGrid bay, uint32 bits)
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

//#define line(x, l, h, a, c) ((x) < (l)) ? 0 :(((x) > (h)) ? 255 : ((a*x) + c)/(h-l))

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
		//p[j] = (avr[j]-min[j])*(max[ind]-min[ind])/(avr[ind]-min[ind]) + min[j];
		//a[j] = (double)sz1/(p[j]-min[j]); c[j] = - a[j]*min[j];
		//printf("min = %f max = %f avr = %f a = %f c = %f p = %f\n", min[j], max[j], avr[j], a[j], c[j], p[j]);
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

void utils_white_balance(imgtype *in, imgtype *out, uint32 *hist, uint16 *look, uint32 h, uint32 w, BayerGrid bay, uint32 in_bits, uint32 out_bits, Gamma gamma)
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
	int i, j, k, rest, last, lasth, last1, lasth1;
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
	int i, j, k, rest, last, lasth, last1, lasth1;
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
	int i, j, k, l, n, t, dim = height*width;
	int dim3 = dim*3, half_h = height/2, half_w = width/2, w3 = width*3, h3 = height*3;
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
	int i, j, k, l, n, t, dim = height*width;
	int dim3 = dim*3, half_h = height/2, half_w = width/2, w3 = width*3, h3 = height*3;
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
	uint32 i, j;
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
	uint32 i, j;
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
	uint32 i, j;
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
	uint32 i, j;
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
	uint32 i, j;
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
	uint32 i, j;
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
