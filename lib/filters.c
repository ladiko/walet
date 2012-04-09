#include <walet.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <math.h>

static  inline void sort(uint8 *s, uint8 x1, uint8 x2, uint8 x3)
{
	if(x1 > x2){
		if(x2 > x3) { s[0] = x3; s[1] = x2; s[2] = x1; }
		else {
			if(x1 > x3) { s[0] = x2; s[1] = x3; s[2] = x1; }
			else 		{ s[0] = x2; s[1] = x1; s[2] = x3; }
		}
	} else {
		if(x1 > x3) { s[0] = x3; s[1] = x1; s[2] = x2; }
		else {
			if(x3 > x2) { s[0] = x1; s[1] = x2; s[2] = x3; }
			else 		{ s[0] = x1; s[1] = x3; s[2] = x2; }
		}
	}
	//return s;
}

static  inline uint8  max_3(uint8 s0, uint8 s1, uint8 s2)
{
	return (s0 > s1) ? (s0 > s2 ? s0 : s2) : (s1 > s2 ? s1 : s2);
}

static  inline uint8  min_3(uint8 s0, uint8 s1, uint8 s2)
{
	return (s0 > s1) ? (s1 > s2 ? s2 : s1) : (s0 > s2 ? s2 : s0);
}


static  inline uint8  median_3(uint8 s0, uint8 s1, uint8 s2)
{
	return (s2 > s1) ? (s1 > s0 ? s1 : (s2 > s0 ? s0 : s2))
					 : (s2 > s0 ? s2 : (s1 > s0 ? s0 : s1));
}

void filter_median_buf(uint8 *img, uint8 *img1, uint8 *buff, uint32 w, uint32 h)
{
	// s0    s1    s2
	//|-----|-----|-----|
	//|     |     |     |
	//|-----|-----|-----|
	//|     | yx  |     |
	//|-----|-----|-----|
	//|     |     |     |
	//|-----|-----|-----|
	uint32 y, x, x2, yx, yw, yw1, h1 = h-1, w1 = w+1;
	uint8 max;
	uint8 *s0 = buff, *s1 = &s0[3],  *s2 = &s1[3];
	uint8 *l0 = &s2[3], *l1 = &l0[w+2], *l2 = &l1[w+2], *tm;

	l0[0] = img[0]; for(x=0; x < w; x++) l0[x+1] = img[x]; l0[x+1] = img[x-1];
	l1[0] = img[0]; for(x=0; x < w; x++) l1[x+1] = img[x]; l1[x+1] = img[x-1];
	for(y=0; y < h; y++){
		yw = y*w;
		yw1 = y < h1 ? yw + w : yw;
		l2[0] = img[yw1]; for(x=0; x < w; x++) l2[x+1] = img[x+yw1]; l2[x+1] = img[x-1+yw1];
		sort(s0, l0[0], l1[0], l2[0]);
		sort(s1, l0[1], l1[1], l2[1]);

		for(x=0; x < w; x++){
			yx = yw + x;
			x2 = x+2;
			sort(s2, l0[x2], l1[x2], l2[x2]);
			img1[yx] = median_3(max_3	(s0[2], s1[2], s2[2]),
								median_3(s0[1], s1[1], s2[1]),
								min_3   (s0[0], s1[0], s2[0]));
			tm = s0; s0 = s1; s1 = s2; s2 = tm;
		}
		tm = l0; l0 = l1; l1 = l2; l2 = tm;
	}
}


static  inline uint8*  sort_3(uint8 *s)
{
	uint8 tmp;
	if(s[0] > s[1]) { tmp = s[1]; s[1] = s[0]; s[0] = tmp; }
	if(s[1] > s[2]) { tmp = s[2]; s[2] = s[1]; s[1] = tmp; }
	return s;
}

void filter_median1(uint8 *img, uint8 *img1, uint32 w, uint32 h)
{
	// s[0]  s[1]  s[2]
	//|-----|-----|-----|
	//|     |     |     |
	//|-----|-----|-----|
	//|     | yx  |     |
	//|-----|-----|-----|
	//|     |     |     |
	//|-----|-----|-----|
	uint32 y, x, yx, i, sq = w*h - w, w1 = w-1;
	uint8 s[3][3];

	for(y=w; y < sq; y+=w){
		x = 1; i = 2;
		yx = y + x;
		s[0][0] = img[yx-1-w]; s[0][1] = img[yx-1]; s[0][2] = img[yx-1+w];
		s[1][0] = img[yx-1  ]; s[1][1] = img[yx  ]; s[1][2] = img[yx+1  ];
		sort_3(s[0]); sort_3(s[1]);
		for(; x < w1; x++){
			yx = y + x;
			s[i][0] = img[yx+1-w]; s[i][1] = img[yx+1]; s[i][2] = img[yx+1+w];
			sort_3(s[i]);
			img1[yx] = median_3(max_3   (s[0][0], s[1][0], s[2][0]),
								median_3(s[0][1], s[1][1], s[2][1]),
								min_3   (s[0][2], s[1][2], s[2][2]));
			i = (i == 2) ? 0 : i+1;
		}
	}
	//Copy one pixel border
	utils_copy_border(img, img1, 1, w, h);
}

static  inline int16*  sort_3_16(int16 *s)
{
	int16 tmp;
	if(s[0] > s[1]) { tmp = s[1]; s[1] = s[0]; s[0] = tmp; }
	if(s[1] > s[2]) { tmp = s[2]; s[2] = s[1]; s[1] = tmp; }
	return s;
}
static  inline int16  max_3_16(int16 s0, int16 s1, int16 s2)
{
	return (s0 > s1) ? (s0 > s2 ? s0 : s2) : (s1 > s2 ? s1 : s2);
}

static  inline int16  min_3_16(int16 s0, int16 s1, int16 s2)
{
	return (s0 > s1) ? (s1 > s2 ? s2 : s1) : (s0 > s2 ? s2 : s0);
}


static  inline int16  median_3_16(int16 s0, int16 s1, int16 s2)
{
	return (s2 > s1) ? (s1 > s0 ? s1 : (s2 > s0 ? s0 : s2))
					 : (s2 > s0 ? s2 : (s1 > s0 ? s0 : s1));
}

void filter_median_16(int16 *img, int16 *img1, uint32 w, uint32 h)
{
	// s[0]  s[1]  s[2]
	//|-----|-----|-----|
	//|     |     |     |
	//|-----|-----|-----|
	//|     | yx  |     |
	//|-----|-----|-----|
	//|     |     |     |
	//|-----|-----|-----|
	uint32 y, x, yx, yw, i, h1 = h-1, w1 = w-1;
	int16 s[3][3];

	for(y=1; y < h1; y++){
		i = 2; x = 2;
		yw = y*w;
		yx = yw + x;
		s[0][0] = img[yx-1-w]; s[0][1] = img[yx-1]; s[0][2] = img[yx-1+w];
		s[1][0] = img[yx-1  ]; s[1][1] = img[yx  ]; s[1][2] = img[yx+1  ];
		sort_3_16(s[0]); sort_3_16(s[1]);
		for(x=1; x < w1; x++){
			yx = yw + x;
			s[i][0] = img[yx+1-w]; s[i][1] = img[yx+1]; s[i][2] = img[yx+1+w];
			sort_3_16(s[i]);
			img1[yx] = median_3(max_3_16   (s[0][0], s[1][0], s[2][0]),
								median_3_16(s[0][1], s[1][1], s[2][1]),
								min_3_16   (s[0][2], s[1][2], s[2][2]));
			i = (i == 2) ? 0 : i+1;
		}
	}
	//Copy one pixel border
	//utils_copy_border(img, img1, 1, w, h);
}

void filter_median(uint8 *img, uint8 *img1, uint32 w, uint32 h)
{
	// s[0]  s[1]  s[2]
	//|-----|-----|-----|
	//|     |     |     |
	//|-----|-----|-----|
	//|     | yx  |     |
	//|-----|-----|-----|
	//|     |     |     |
	//|-----|-----|-----|
	uint32 y, x, yx, yw, i, h1 = h-1, w1 = w-1;
	uint8 s[3][3];

	for(y=1; y < h1; y++){
		i = 2; x = 2;
		yw = y*w;
		yx = yw + x;
		s[0][0] = img[yx-1-w]; s[0][1] = img[yx-1]; s[0][2] = img[yx-1+w];
		s[1][0] = img[yx-1  ]; s[1][1] = img[yx  ]; s[1][2] = img[yx+1  ];
		sort_3(s[0]); sort_3(s[1]);
		for(x=1; x < w1; x++){
			yx = yw + x;
			s[i][0] = img[yx+1-w]; s[i][1] = img[yx+1]; s[i][2] = img[yx+1+w];
			sort_3(s[i]);
			img1[yx] = median_3(max_3   (s[0][0], s[1][0], s[2][0]),
								median_3(s[0][1], s[1][1], s[2][1]),
								min_3   (s[0][2], s[1][2], s[2][2]));
			i = (i == 2) ? 0 : i+1;
		}
	}
	//Copy one pixel border
	//utils_copy_border(img, img1, 1, w, h);
}

void filter_contrast(uint8 *img, uint8 *img1, uint32 w, uint32 h)
{
	// s[0]  s[1]  s[2]
	//|-----|-----|-----|
	//|     |     |     |
	//|-----|-----|-----|
	//|     | yx  |     |
	//|-----|-----|-----|
	//|     |     |     |
	//|-----|-----|-----|
	uint32 y, x, yx, yw, i, h1 = h-2, w1 = w-2, med, min, max;
	uint8 s[3][3];

	for(y=2; y < h1; y++){
		i = 2; x = 2;
		yw = y*w;
		yx = yw + x;
		s[0][0] = img[yx-1-w]; s[0][1] = img[yx-1]; s[0][2] = img[yx-1+w];
		s[1][0] = img[yx-1  ]; s[1][1] = img[yx  ]; s[1][2] = img[yx+1  ];
		sort_3(s[0]); sort_3(s[1]);
		for(x=2; x < w1; x++){
			yx = yw + x;
			s[i][0] = img[yx+1-w]; s[i][1] = img[yx+1]; s[i][2] = img[yx+1+w];
			sort_3(s[i]);
			max = max_3   (s[0][0], s[1][0], s[2][0]);
			min = min_3   (s[0][2], s[1][2], s[2][2]);
			med = median_3(max, median_3(s[0][1], s[1][1], s[2][1]), min);
			img1[yx] = abs(max-img[yx]) < abs(min-img[yx]) ? max : min;
			i = (i == 2) ? 0 : i+1;
		}
	}
	//Copy one pixel border
	//utils_copy_border(img, img1, 1, w, h);
}

void filter_median_bayer(int16 *img, int16 *img1, uint32 w, uint32 h)
{
	// s[0]  s[1]  s[2]
	//|-----|-----|-----|
	//|     |     |     |
	//|-----|-----|-----|
	//|     | yx  |     |
	//|-----|-----|-----|
	//|     |     |     |
	//|-----|-----|-----|
	uint32 y, x, yx, i, w2 = w<<1, sq = w*h - w2, w1 = w-2;
	uint8 s0[3][3], s1[3][3], s2[3][3], s3[3][3];
	for(y=w2; y < sq; y+=w2){
		x = 2; i = 2;
		yx = y + x;
		s0[0][0] = img[yx-2-w2]; s0[0][1] = img[yx-2]; s0[0][2] = img[yx-2+w2];
		s0[1][0] = img[yx-w2  ]; s0[1][1] = img[yx  ]; s0[1][2] = img[yx+w2  ];
		sort_3(s0[0]); sort_3(s0[1]);
		s1[0][0] = img[yx-1-w2]; s1[0][1] = img[yx-1]; s1[0][2] = img[yx-1+w2];
		s1[1][0] = img[yx+1-w2]; s1[1][1] = img[yx+1]; s1[1][2] = img[yx+1+w2];
		sort_3(s1[0]); sort_3(s1[1]);
		s2[0][0] = img[yx-2-w]; s2[0][1] = img[yx-2+w]; s2[0][2] = img[yx-2+w2+w];
		s2[1][0] = img[yx-w  ]; s2[1][1] = img[yx+w  ]; s2[1][2] = img[yx+w2+w  ];
		sort_3(s2[0]); sort_3(s2[1]);
		s3[0][0] = img[yx-1-w]; s3[0][1] = img[yx-1+w]; s3[0][2] = img[yx-1+w2+w];
		s3[1][0] = img[yx+1-w]; s3[1][1] = img[yx+1+w]; s3[1][2] = img[yx+1+w2+w];
		sort_3(s3[0]); sort_3(s3[1]);
		for(; x < w1; x+=2){
			yx = y + x;
			s0[i][0] = img[yx+2-w2]; s0[i][1] = img[yx+2]; 		s0[i][2] = img[yx+2+w2]; 	sort_3(s0[i]);
			s1[i][0] = img[yx+3-w2]; s1[i][1] = img[yx+3]; 		s1[i][2] = img[yx+3+w2]; 	sort_3(s1[i]);
			s2[i][0] = img[yx+2-w ]; s2[i][1] = img[yx+2+w]; 	s2[i][2] = img[yx+2+w2+w]; 	sort_3(s2[i]);
			s3[i][0] = img[yx+3-w ]; s3[i][1] = img[yx+3+w]; 	s3[i][2] = img[yx+3+w2+w]; 	sort_3(s3[i]);

			img1[yx] 		= median_3(	max_3   (s0[0][0], s0[1][0], s0[2][0]),
										median_3(s0[0][1], s0[1][1], s0[2][1]),
										min_3   (s0[0][2], s0[1][2], s0[2][2]));
			img1[yx+1] 		= median_3(	max_3   (s1[0][0], s1[1][0], s1[2][0]),
										median_3(s1[0][1], s1[1][1], s1[2][1]),
										min_3   (s1[0][2], s1[1][2], s1[2][2]));
			img1[yx+w] 		= median_3(	max_3   (s2[0][0], s2[1][0], s2[2][0]),
										median_3(s2[0][1], s2[1][1], s2[2][1]),
										min_3   (s2[0][2], s2[1][2], s2[2][2]));
			img1[yx+w+1]	= median_3(	max_3   (s3[0][0], s3[1][0], s3[2][0]),
										median_3(s3[0][1], s3[1][1], s3[2][1]),
										min_3   (s3[0][2], s3[1][2], s3[2][2]));
			i = (i == 2) ? 0 : i+1;
		}
	}
}

static inline uint8 filter(int16 *img, uint32 yx, uint32 w, uint32 thresh)
{
	uint32 sum;
	sum = 	(img[yx-w-1] + img[yx-w  ] + img[yx-w+1] +
			 img[yx-1  ] + img[yx    ] + img[yx+1  ] +
			 img[yx+w-1] + img[yx+w  ] + img[yx+w+1])>>3;
	return sum > thresh ? sum : 0;
}

void filter_average(int16 *img, int16 *img1, uint32 w, uint32 h, uint32 thresh)
{
	uint32 y, x, yx, i, sq = w*h - w, w1 = w-1;
	for(y=w; y < sq; y+=w){
		for(x=1; x < w1; x++){
			yx = y + x;
			img1[yx] = filter(img, yx, w, thresh);
			//if(img[yx]) img1[yx] = filter(img, yx, w, thresh);
			//else img1[yx] = 0;
		}
	}
}

void filter_noise(uint8 *img, uint8 *img1, uint32 w, uint32 h, uint32 th)
{
	uint32 y, x, yx, yw,  h1 = h-2, w1 = w-2, av;
	for(y=2; y < h1; y++){
		yw = w*y;
		for(x=2; x < w1; x++){
			yx = yw + x;
			av = (	img[yx-w-1] + img[yx-w  ] + img[yx-w+1] +
					img[yx-1  ] + img[yx+1  ] +
					img[yx+w-1] + img[yx+w  ] + img[yx+w+1])>>3;
			 img1[yx] = (av +  img[yx])>>1;
		}
	}
}

#define line(a,x)  (a*x)
#define bt709(maxx,maxy,x)	(((x) < (0.018*maxx)) ? 4.5*maxy*(x)/maxx : (1.099*pow(x/maxx,0.45)-0.099)*maxy)
#define srgb(maxx,maxy,x)	(((x) < (0.00313*maxx)) ? 12.92*maxy*(x)/maxx : (1.055*pow(x/maxx,0.417)-0.055)*maxy)
#define cut_max(x, max) (((x) < max) ? (x) : max - 1)

static void  multiply_color (int16 *R, double c, uint32 size, uint32 in_bits)
{
	int i,shift = 1<<(in_bits-1), max = (1<<in_bits);

	for(i=0; i<size; i++){
		//R[i] = cut_max((int)((double)(R[i] + shift)*c) - shift, max);
		R[i] = cut_max((int)((double)(R[i])*c), max);
	}
}

static uint32 get_grey_pixels(int16 *R, int16 *G, int16 *B, double *dr, double *db, uint32 size, uint32 in_bits)
{
	uint32 i, th = 5, shift = 1<<(in_bits-1), max = (1<<in_bits)-1-10, sh = (in_bits - 8);
	int16 r, g, b;
	int cn = 0;
	*dr = 0; *db = 0;
	for(i=0; i<size; i++){
		r = (R[i] + shift)>>sh;
		g = (G[i] + shift)>>sh;
		b = (B[i] + shift)>>sh;
		//if(G[i] > 10 && G[i] < 4095 && R[i] > 10 && R[i] < 4095 && B[i] > 10 && B[i] < 4095){
		//if(g > th && r > th && b > th ){
			if(abs(g- r) < th && abs(g- r) < th) {
				cn++;
				(*dr) += (double)(G[i]- R[i])/(double)R[i];
				(*db) += (double)(G[i]- B[i])/(double)B[i];
			}
		//}
	}
	//printf("dr = %d db = %d cn = %d\n", *dr, *db, cn);
	*dr = 1. - (*dr)/(double)cn; *db = 1. - (*db)/(double)cn;
	//printf("dr = %d db = %d\n", *dr, *db);
	return cn;
}

static void color_table(uint32 *hist, uint16 *look, double *avr, uint32 in_bits, uint32 out_bits, uint32 size, Gamma gamma)
{
	uint32 i, j, sz = (1<<in_bits), sz1 = (1<<out_bits)-1, ind, sh = in_bits - out_bits;
	uint32 *yh = hist, tot;
	uint32 *h[3] = {&hist[1<<in_bits], &hist[(1<<in_bits)*2], &hist[(1<<in_bits)*3]};
	uint16 *l[3] = {look, &look[1<<in_bits], &look[1<<(in_bits+1)]};
	double max[3], a[3], p[3], sum, sum1, lim = size*0.01;  //min[3],c[3],
	double yavr;
	uint32 ymin, ymax, sm, st;

	//Calculate statistic for Y component
	sum = 0.; sum1 = 0.;
	for(i=0; i<sz; i++) sum += yh[i]*i;
	yavr = (double)sum/(double)size;
	/*
	sum = 0.;
	for(i=0; i<sz; i++) { sum += yh[i]; if(sum > lim) break; }
	ymin = i;
	sum = 0.;
	for(i=sz-1; i; i--) { sum += yh[i]; if(sum > lim) break;  }
	ymax = i;
	*/
	tot = size - yh[0] - yh[sz-1];
	st = tot/(1<<out_bits);
	sm = 0; l[0][0] = 0;
	for(i=1; i<sz-1; i++){
		sm += yh[i];
		l[0][i] = sm/st;
		if(l[0][i] > sz1) l[0][i] = sz1;
		//printf("%4d = %d\n", i, l[0][i]);
	}
	//Calculate statistic for RGB components
	for(j=0; j<3; j++) {
		sum = 0.;
		for(i=0; i<sz; i++) sum += h[j][i]*i;
		avr[j] = (double)sum/(double)size;
	}
	printf("ymin = %d ymax = %d y = %f r = %f g = %f b = %f\n",ymin, ymax, yavr, avr[0], avr[1], avr[2]);
	/*
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
	*/
}

void filters_white_balance(int16 *in, int16 *out, uint32 *hist, uint16 *look, uint32 w, uint32 h,  BayerGrid bay,  uint32 in_bits, uint32 out_bits, Gamma gamma)
{
	uint32 i, x, y, size = h*w, shift = 1<<(in_bits-1), shift1 = 1<<(out_bits-1);
	uint16 *c[4];
	uint16 *l[3] = {look, &look[1<<in_bits], &look[1<<(in_bits+1)]};
	double avr[3];

	color_table(hist, look, avr, in_bits, out_bits, w*h, gamma);

	switch(bay){
		case(BGGR):{ c[0] = l[0]; c[1] = l[1]; c[2] = l[1]; c[3] = l[2]; break;}
		case(GRBG):{ c[0] = l[1]; c[1] = l[2]; c[2] = l[0]; c[3] = l[1]; break;}
		case(GBRG):{ c[0] = l[1]; c[1] = l[0]; c[2] = l[2]; c[3] = l[1]; break;}
		case(RGGB):{ c[0] = l[2]; c[1] = l[1]; c[2] = l[1]; c[3] = l[0]; break;}
	}

	for(i=0, x=0, y=0; i < size; i++, x++){
		if(x == w) { x=0; y++;}
		if(y&1)
			if(x&1) out[i] = c[0][in[i]+shift] - shift1;
			else 	out[i] = c[1][in[i]+shift] - shift1;
		else
			if(x&1)	out[i] = c[2][in[i]+shift] - shift1;
			else 	out[i] = c[3][in[i]+shift] - shift1;
	}
}

void filters_wb(int16 *Y, int16 *R, int16 *G, int16 *B, uint8 *r, uint8 *g, uint8 *b, uint8 *buff,
		uint32 *hist, uint16 *look, uint32 w, uint32 h,  BayerGrid bay,  uint32 in_bits, uint32 out_bits, Gamma gamma)
{
	uint32 i, size = h*w, shift = 1<<(in_bits-1), shift1 = 1<<(out_bits-1), ngray;
	//uint16 *c[4];
	uint16 *l[3] = {look, &look[1<<in_bits], &look[1<<(in_bits+1)]};
	uint32 *yh = hist, *rh = &yh[1<<in_bits], *gh = &rh[1<<in_bits], *bh = &gh[1<<in_bits];
	//double dr, db;
	double rc, bc;
	double avr[3];

	fill_hist(Y, yh, size, in_bits);
	fill_hist(R, rh, size, in_bits);
	fill_hist(G, gh, size, in_bits);
	fill_hist(B, bh, size, in_bits);

	for(i=0; i < size; i++) R[i] = R[i] + shift;
	for(i=0; i < size; i++) G[i] = G[i] + shift;
	for(i=0; i < size; i++) B[i] = B[i] + shift;
	/*
	for(i=0; i < size; i++) Y[i] = R[i] + shift;
	filter_median_16(Y, R, w, h);
	for(i=0; i < size; i++) Y[i] = G[i] + shift;
	filter_median_16(Y, G, w, h);
	for(i=0; i < size; i++) Y[i] = B[i] + shift;
	filter_median_16(Y, B, w, h);
	*/


	color_table(hist, look, avr, in_bits, out_bits, size, gamma);


	//ngray = get_grey_pixels(R, G, B, &rc, &bc, size, in_bits);
	rc = avr[1]/avr[0]; bc = avr[1]/avr[2];
	printf("cn = %d gray = %f DR = %f DB = %f\n", ngray, (double)ngray/(double)size, rc, bc);

	multiply_color (R, rc, size, in_bits);
	multiply_color (B, bc, size, in_bits);
	/*
	for(i=0; i < size; i++){
		r[i] = l[0][R[i]+shift] - shift1;
		g[i] = l[0][G[i]+shift] - shift1;
		b[i] = l[0][B[i]+shift] - shift1;

	}
	*/
	for(i=0; i < size; i++){
		r[i] = l[0][R[i]];
		g[i] = l[0][G[i]];
		b[i] = l[0][B[i]];

	}
	/*
	filter_median(r, buff, w, h);
	for(i=0; i < size; i++) r[i] = buff[i];
	filter_median(g, buff, w, h);
	for(i=0; i < size; i++) g[i] = buff[i];
	filter_median(b, buff, w, h);
	for(i=0; i < size; i++) b[i] = buff[i];
	*/
}

void HDR_12bits_to_8bits(int *hist, int w, int h, int low_threshold, int high_threshold, float *gain, int *offset)
{
// |           *
// |         *    *
// |       *           *
// |     *                 *
// |    *                   *
// | * *                      * *
// |---|------------------------|------->
//   low                       high      10 bits hist

	int i, sum, sz = w*h;
	int low_thr = sz*low_threshold/100, high_thr = sz*high_threshold/100;
	int low, high;

	//Finding low threshold
	sum = 0;
	for(i=0; ; i++) {
		sum += hist[i];
		if(sum > low_thr) {
			low = i;
			break;
		}
	}
	//Finding high threshold
	sum = 0;
	for(i=511; ; i--) {
		sum += hist[i];
		if(sum > high_thr) {
			high = i;
			break;
		}
	}
	*offset = -(low<<2); // from 10 bits to 12 bits
	*gain = (float)((high - low)<<2)/(float)(1<<12);

}
