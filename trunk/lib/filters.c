#include <walet.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <math.h>

static  inline uchar*  sort_3(uchar *s)
{
	uchar tmp;
	if(s[0] > s[1]) { tmp = s[1]; s[1] = s[0]; s[0] = tmp; }
	if(s[1] > s[2]) { tmp = s[2]; s[2] = s[1]; s[1] = tmp; }
	return s;
}

static  inline uchar  max_3(uchar s0, uchar s1, uchar s2)
{
	return (s0 > s1) ? (s0 > s2 ? s0 : s2) : (s1 > s2 ? s1 : s2);
}

static  inline uchar  min_3(uchar s0, uchar s1, uchar s2)
{
	return (s0 > s1) ? (s1 > s2 ? s2 : s1) : (s0 > s2 ? s2 : s0);
}

static  inline uchar  median_3(uchar s0, uchar s1, uchar s2)
{
	return (s2 > s1) ? (s1 > s0 ? s1 : (s2 > s0 ? s0 : s2))
					 : (s2 > s0 ? s2 : (s1 > s0 ? s0 : s1));
}

void filter_median(uchar *img, uchar *img1, uint32 w, uint32 h)
{
	uint32 y, x, yx, i, sq = w*h - w, w1 = w-1;
	uchar s[3][3];
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
}

static inline int median_fast(imgtype *s, uint32 thresh)
///	\fn static inline int median_filter_3x3(imgtype *s)
///	\brief The not optimal algorithm finding median element.
///	\param s	 	Input array.
///	\param p	 	Temporary array.
{
	imgtype tmp;
	if(s[0] > s[1]) { tmp = s[1]; s[1] = s[0]; s[0] = tmp;}
	if(s[1] > s[2]) { tmp = s[2]; s[2] = s[1]; s[1] = tmp;}
	return s[4];
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

static void get_3(imgtype *img, imgtype *img1, uint32 w, uint32 h, uint32 param, FP func)
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

void filters_bayer_median_3x3(imgtype *img, imgtype *img1, uint32 w, uint32 h, BayerGrid bay)
{
	get_bayer_3x3(img, img1, w, h, bay, 0, &median);
}

void filters_median_3x3(imgtype *img, imgtype *img1, uint32 w, uint32 h)
{
	get_3x3(img, img1, w, h, 0, &median);
}

void filters_bayer_gradient(imgtype *img, imgtype *img1, uint32 w, uint32 h, BayerGrid bay, uint32 thresh)
{
	get_bayer_3x3(img, img1, w, h, bay, thresh, &gradient);
}

void filters_gradient(imgtype *img, imgtype *img1, uint32 w, uint32 h, uint32 thresh)
{
	get_3x3(img, img1, w, h, thresh, &gradient);
}

void filters_edge_detector(imgtype *img, imgtype *img1, uint32 w, uint32 h)
{
	get_3x3(img, img1, w, h, 0, &edge);
}


#define line(a,x)  (a*x)
#define bt709(maxx,maxy,x)	(((x) < (0.018*maxx)) ? 4.5*maxy*(x)/maxx : (1.099*pow(x/maxx,0.45)-0.099)*maxy)
#define srgb(maxx,maxy,x)	(((x) < (0.00313*maxx)) ? 12.92*maxy*(x)/maxx : (1.055*pow(x/maxx,0.417)-0.055)*maxy)

static void color_table(uint32 *hist, uint16 *look, uint32 in_bits, uint32 out_bits, Gamma gamma)
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

void filters_white_balance(imgtype *in, imgtype *out, uint32 w, uint32 h,  BayerGrid bay, uint32 *hist, uint16 *look,  uint32 in_bits, uint32 out_bits, Gamma gamma)
{
	uint32 i, x, y, size = h*w;
	uint16 *c[4];
	uint16 *l[3] = {look, &look[1<<in_bits], &look[1<<(in_bits+1)]};

	color_table(hist, look, in_bits, out_bits, gamma);

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

