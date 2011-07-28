#include <walet.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define lim(max,min, x)  	((x)>max ? max :((x)<min ? min : (x)))
#define max(x, m) 			((x>m) ? (m) : (x))


void dwt_haar_2d_one(int16 *in, int16 *ll, int16 *hl, int16 *lh, int16 *hh, int16 *buf, const uint32 w, const uint32 h)
///	\fn void dwt_2d_haar8(int8 *in, uint16 w, uint16 h, int8 *out0, int8 *out1, int8 *out2, int8 *out3)
///	\brief One step 2D Haar DWT transform.
///	\param in	 		The input image data.
///	\param w 			The width
///	\param h 			The height
///	\param ll	 		The pointer to LL subbabnd
///	\param hl	 		The pointer to HL subbabnd
///	\param lh	 		The pointer to LH subbabnd
///	\param hh	 		The pointer to HH subbabnd
{
	uint32 x, y, y1, y2, yx, yx0, yx1, yx2, sz = w*((h>>1)<<1);
	uint32 wy = w<<1, wy1 = (w>>1) + (w&1), wy2 = (w>>1), wx = ((w>>1)<<1);
	//uint32 yx0, yx01, yx11;
	int tmp[4];

	for(y=0, y1=0, y2 = 0; y < sz; y+=wy, y1+=wy1, y2+=wy2){
		for(x=0; x < wx; x+=2){
			yx = y + x; yx0 = yx + 1;
			tmp[0]	= (in[yx] + in[yx0]);
			tmp[1]	= (in[yx] - in[yx0]);
			yx = yx + w; yx0 = yx + 1;
			tmp[2]	= (in[yx] + in[yx0]);
			tmp[3]	= (in[yx] - in[yx0]);
			yx1 = y1 + (x>>1); yx2 = y2 + (x>>1);
			ll[yx1] = (tmp[0] + tmp[2])>>2;
			hl[yx2] = (tmp[1] + tmp[3])>>2;
			lh[yx1] = (tmp[0] - tmp[2])>>2;
			hh[yx2] = (tmp[1] - tmp[3])>>2;
		}
		if(w&1){
			yx = y + x;
			ll[yx1+1] = (in[yx] + in[yx+w])>>1;
			lh[yx1+1] = (in[yx] - in[yx+w])>>1;
		}
	}
	if(h&1){
		for(x=0; x < wx; x+=2){
			yx = y + x; yx1 = y1 + (x>>1); yx2 = y2 + (x>>1);
			ll[yx1] = (in[yx] + in[yx+1])>>1;
			hl[yx2] = (in[yx] - in[yx+1])>>1;
		}
		if(w&1){
			ll[yx1+1] = in[yx+2];
		}
	}
}

void idwt_haar_2d_one(int16 *out, int16 *ll, int16 *hl, int16 *lh, int16 *hh, int16 *buf, const uint32 w, const uint32 h)
///	\fn void idwt_2d_haar8(int8 *out, uint16 w, uint16 h, int8 *ll, int8 *hl, int8 *lh, int8 *hh)
///	\brief One step 2D Haar IDWT transform.
///	\param out	 		The input image data.
///	\param w 			The width
///	\param h 			The height
///	\param ll	 		The pointer to LL subbabnd
///	\param hl	 		The pointer to HL subbabnd
///	\param lh	 		The pointer to LH subbabnd
///	\param hh	 		The pointer to HH subbabnd
{
	uint32 x, y, y1, y2, yx, yx1, yx2, sz = w*((h>>1)<<1);
	uint32 wy = w<<1, wy1 = (w>>1) + (w&1), wy2 = (w>>1), wx = ((w>>1)<<1);
	int tmp[4], tm;

	for(y=0, y1=0, y2 = 0; y < sz; y+=wy, y1+=wy1, y2+=wy2){
		for(x=0; x < wx; x+=2){
			yx1 = y1 + (x>>1);
			tmp[0]	= ll[yx1] + lh[yx1];
			tmp[1]	= ll[yx1] - lh[yx1];
			yx2 = y2 + (x>>1);
			tmp[2]	= hl[yx2] + hh[yx2];
			tmp[3]	= hl[yx2] - hh[yx2];
			yx = y + x;
			out[yx  ] = (tmp[0] + tmp[2]);
			out[yx+1] = (tmp[0] - tmp[2]);
			yx = yx + w;
			out[yx  ] = (tmp[1] + tmp[3]);
			out[yx+1] = (tmp[1] - tmp[3]);
		}
		if(w&1){
			yx = y + x; yx1 = y1 + (x>>1);
			out[yx  ] = (ll[yx1] + lh[yx1]);
			out[yx+w] = (ll[yx1] - lh[yx1]);
		}
	}
	if(h&1){
		for(x=0; x < wx; x+=2){
			yx = y + x; yx1 = y1 + (x>>1);
			out[yx  ] = (ll[yx1] + hl[yx1]);
			out[yx+1] = (ll[yx1] - hl[yx1]);
		}
		if(w&1){
			yx = y + x;
			out[yx] = ll[yx1+1];
		}
	}
}

static inline void dwt_53_1d(int16 *in, int16 *out, const uint32 w)
///	\fn static inline void dwt53_1d_1h(imgtype *in, imgtype *out, const uint32 w)
///	\brief 1D 5.3 wavelet transform.
///	\param in	 		The input line.
///	\param out 			The output line.
///	\param w 			The line width.
{
	int wt = w - 2, i, j;
	int16 *l, *h;
	l = out; h = &out[(w>>1) + (w&1)];
	h[0] = in[1] - ((in[0] + in[2])>>1);
	l[0] = in[0] + (h[0]>>1);
	for(i=2,j=1; i < wt; i+=2,j++){
		h[j] = in[i+1] - ((in[i] + in[i+2])>>1);
		l[j] = in[i] + ((h[j-1] + h[j])>>2);
	}
	if(w&1){
		l[j] = in[i] + (h[j-1]>>1);
	} else{
		h[j] = in[i+1] - in[i];
		l[j] = in[i] + ((h[j-1] + h[j])>>2);
	}
}

static inline void idwt_53_1d(int16 *in, int16 *out, const uint32 w)
///	\fn static inline void idwt53_1d_1h(imgtype *in, imgtype *out, const uint32 w)
///	\brief 1D 5.3 invert wavelet transform.
///	\param in	 		The input line.
///	\param out 			The output line.
///	\param w 			The line width.
{
	int wt = w - 2, i, j;
	int16 *l, *h;
	l = in; h = &in[(w>>1) + (w&1)];

	out[0] = l[0] - (h[0]>>1);
	for(i=2,j=0; i <= wt; i+=2,j++){
		out[i] = l[j+1] - ((h[j] + h[j+1])>>2);
		out[i-1] = h[j] + ((out[i-2] + out[i])>>1);
	}
	if(w&1){
		out[i] = l[j+1] - (h[j]>>1);
		out[i-1] = h[j] + ((out[i-2] + out[i])>>1);
	} else{
		out[i-1] = h[j] + out[i-2];
	}
}


void dwt_53_2d_one(int16 *in, int16 *ll, int16 *hl, int16 *lh, int16 *hh, int16 *buf, const uint32 w, const uint32 h)
///	\fn static inline void dwt53_2d_v(imgtype *in, imgtype *out, const uint32 w, const uint32 h)
///	\brief 2D 5.3 vertical wavelet transform.
///	\param in	 		The input image data.
///	\param out 			The output image data.
///	\param w 			The image width.
///	\param h 			The image height.
{
	uint32 i,ik, ik1, k = 0, k1, k2, sz = (h-1)*w, hw = w<<1;
	uint32 h2 = (h>>1), h1 = (h>>1) + (h&1), w2 = (w>>1), w1 = (w>>1) + (w&1);
	//uint32 s[4];
	int16 *l[3], *tm;
	//int16 *ll, *hl, *lh, *hh, *l[3], *tm;
	//ll = out; hl = ll + w1*h1; lh = hl + w2*h1; hh = lh + w1*h2;
	l[0] = &buf[0]; l[1] = &buf[w]; l[2] = &buf[hw];

	dwt_53_1d(&in[0],  l[0], w);
	dwt_53_1d(&in[w],  l[1], w);
	dwt_53_1d(&in[hw], l[2], w);
	for(i=0; i<w1; i++){
		lh[i] = l[1][i] - ((l[0][i] + l[2][i])>>1);
		ll[i] = l[0][i] + (lh[i]>>1);
	}
	for(i=0; i<w2; i++){
		ik = w1+i;
		hh[i] = l[1][ik] - ((l[0][ik] + l[2][ik])>>1);
		hl[i] = l[0][ik]  + (hh[i]>>1);
	}
	for(k=3*w, k1=w1, k2=w2; k < sz; k+=hw, k1+=w1, k2+=w2){
		//l[0] = l[2];
		tm = l[0]; l[0] = l[2]; l[2] = tm;
		dwt_53_1d(&in[k], l[1], w);
		dwt_53_1d(&in[k+w], l[2], w);
		ik1 = k1;
		for(i=0; i<w1; i++){
			lh[ik1] = l[1][i] - ((l[0][i] + l[2][i])>>1);
			ll[ik1] = l[0][i] + ((lh[ik1-w1] + lh[ik1])>>2);
			ik1++;
		}
		ik1 = k2;
		for(i=0; i<w2; i++){
			ik = w1+i;
			hh[ik1] = l[1][ik] - ((l[0][ik] + l[2][ik])>>1);
			hl[ik1] = l[0][ik] + ((hh[ik1-w2] + hh[ik1])>>2);
			ik1++;
		}
	}
	if(h&1){
		ik1 = k1;
		for(i=0; i<w1; i++){
			ll[ik1] = l[2][i] + (lh[ik1-w1]>>1);
			ik1++;
		}
		ik1 = k2;
		for(i=0; i<w2; i++){
			ik = w1+i;
			hl[ik1] = l[2][ik] + (hh[ik1-w2]>>1);
			ik1++;
		}
	} else{
		l[0] = l[2];
		dwt_53_1d(&in[k], l[1], w);
		ik1 = k1;
		for(i=0; i<w1; i++){
			lh[ik1] = l[1][i] - l[0][i];
			ll[ik1] = l[0][i] + ((lh[ik1 -w1] + lh[ik1])>>2);
			ik1++;
		}
		ik1 = k2;
		for(i=0; i<w2; i++){
			ik = w1+i;
			hh[ik1] = l[1][ik] - l[0][ik];
			hl[ik1] = l[0][ik]  + ((hh[ik1-w2] + hh[ik1])>>2);
			ik1++;
		}
	}
}

void idwt_53_2d_one(int16 *out, int16 *ll, int16 *hl, int16 *lh, int16 *hh, int16 *buf, const uint32 w, const uint32 h)
///	\fn static inline void idwt53_2d_v(imgtype *in, imgtype *out, const uint32 w, const uint32 h)
///	\brief 2D 5.3 vertical invert wavelet transform.
///	\param in	 		The input image data.
///	\param out 			The output image data.
///	\param w 			The image width.
///	\param h 			The image height.
///	\param loc 			The subband location.
{

	uint32 i, ik, ik1, k, k1, k2, sz = (h-1)*w, hw = w<<1;
	uint32 h2 = (h>>1), h1 = (h>>1) + (h&1), w2 = (w>>1), w1 = (w>>1) + (w&1);
	//uint32 s[4];
	//int16 *ll, *hl, *lh, *hh, *l[3], *tm;
	//ll = in; hl = ll + w1*h1; lh = hl + w2*h1; hh = lh + w1*h2;
	int16 *l[3], *tm;
	l[0] = &buf[0]; l[1] = &buf[w]; l[2] = &buf[hw];


	for(i=0; i<w1; i++){
		l[0][i] = ll[i] - (lh[i]>>1);
	}
	for(i=0; i<w2; i++){
		l[0][w1+i] = hl[i] - (hh[i]>>1);
	}
	//idwt_53_1d(l[0],  out, w);
	for(k=hw, k1=w1, k2=w2; k < sz; k+=hw, k1+=w1, k2+=w2){
		ik1 = k1;
		for(i=0; i<w1; i++){
			l[2][i] = ll[ik1] - ((lh[ik1-w1] + lh[ik1])>>2);
			l[1][i] = lh[ik1-w1] + ((l[0][i] + l[2][i])>>1);
			ik1++;
		}
		ik1 = k2;
		for(i=0; i<w2; i++){
			ik = w1+i;
			l[2][ik] = hl[ik1] - ((hh[ik1-w2] + hh[ik1])>>2);
			l[1][ik] = hh[ik1-w2] + ((l[0][ik] + l[2][ik])>>1);
			ik1++;
		}
		idwt_53_1d(l[0], &out[k-hw], w);
		idwt_53_1d(l[1], &out[k-w], w);
		tm = l[0]; l[0] = l[2]; l[2] = tm;
		//l[0] = l[2];
	}
	if(h&1){
		ik1 = k1;
		for(i=0; i<w1; i++){
			l[2][i] = ll[ik1] - (lh[ik1-w1]>>1);
			l[1][i] = lh[ik1-w1] + ((l[0][i] + l[2][i])>>1);
			ik1++;
		}
		ik1 = k2;
		for(i=0; i<w2; i++){
			ik = w1+i;
			l[2][ik] = hl[ik1] - (hh[ik1-w2]>>1);
			l[1][ik] = hh[ik1-w2] + ((l[0][ik] + l[2][ik])>>1);
			ik1++;
		}
		idwt_53_1d(l[0], &out[k-hw], w);
		idwt_53_1d(l[1], &out[k-w], w);
		idwt_53_1d(l[2], &out[k], w);
	} else{
		ik1 = k1;
		for(i=0; i<w1; i++){
			l[1][i] = lh[ik1-w1] + l[0][i];
			ik1++;
		}
		ik1 = k2;
		for(i=0; i<w2; i++){
			ik = w1+i;
			l[1][ik] = hh[ik1-w2] + l[0][ik];
			ik1++;
		}
		idwt_53_1d(l[0], &out[k-hw], w);
		idwt_53_1d(l[1], &out[k-w], w);
	}
}

void dwt(Image *img, int16 *buf, fundwt dwt_one, uint32 steps)
//	\fn static void image_mallet_dwt(Image *im, uint8 *buf, uint32 steps, funwt dwt_one)
///	\brief Discrete wavelets transform.
///	\param im	 		The image structure.
///	\param buf 			The temporary buffer.
///	\param steps 		The steps of DWT transform.
/// \param funwt		The function for one step 2d DWT.
{
	uint32 k, i;
	//Color transform
	//printf("C[0].pic = %p\n", c[0].pic);
	//dwt_53_2d_one_new(b->pic, c[0].pic, c[1].pic, c[2].pic, c[3].pic, buf, w, h);
	if(steps){
		(*dwt_one)(img->p, img->l[0].s[0].pic, img->l[0].s[1].pic, img->l[0].s[2].pic, img->l[0].s[3].pic, buf, img->w, img->h);
		for(k=1; k < steps; k++){
			(*dwt_one)(img->l[k-1].s[0].pic, img->l[k].s[0].pic, img->l[k].s[1].pic, img->l[k].s[2].pic, img->l[k].s[3].pic, buf,
					img->l[k-1].s[0].w, img->l[k-1].s[0].h);

		}
	}
}

void idwt(Image *img, int16 *buf, funidwt idwt_one, uint32 steps, uint32 istep)
//	\fn static void image_mallet_dwt(Image *im, uint8 *buf, uint32 steps, funwt dwt_one)
///	\brief Discrete wavelets transform.
///	\param im	 		The image structure.
///	\param buf 			The temporary buffer.
///	\param steps 		The steps of DWT transform.
/// \param funwt		The function for one step 2d DWT.
{
	//uint8 *img = im->img;
	int j, k;
	if(istep > steps){
		printf("istep should be less or equal steps\n");
		return;
	}
	for(k=steps-1; k > steps-istep; k--){
		(*idwt_one)(img->l[k-1].s[0].pic, img->l[k].s[0].pic, img->l[k].s[1].pic, img->l[k].s[2].pic, img->l[k].s[3].pic,
				 buf, img->l[k-1].s[0].w, img->l[k-1].s[0].h);
	}
	img->d.w = img->l[k].s[0].w + img->l[k].s[1].w;
	img->d.h = img->l[k].s[0].h + img->l[k].s[2].h;
	(*idwt_one)(img->d.pic, img->l[k].s[0].pic, img->l[k].s[1].pic, img->l[k].s[2].pic, img->l[k].s[3].pic, buf, img->d.w, img->d.h);
}

void image_dwt(Image *im, int16 *buf, FilterBank fb, uint32 steps)
///	\fn void image_dwt_53(Image *im, ColorSpace color, uint32 steps, uint8 *buf)
///	\brief Discrete 5/3 filter wavelets transform of the image.
///	\param im	 		The image structure.
///	\param color		The color space of the stream.
///	\param steps 		The steps of DWT transform.
///	\param buf 			The temporary buffer.
{
	switch(fb){
		case(FR_HAAR):{
			dwt(im, buf, &dwt_haar_2d_one, steps);
			break;
		}
		case(FR_5_3):{
			dwt(im, buf, &dwt_53_2d_one, steps);
			break;
		}
		default:{
			printf("Dont support %d transform type\n", fb);
			return;
		}
	}
}

void image_idwt(Image *im, int16 *buf, FilterBank fb, uint32 steps, uint32 isteps)
///	\fn void image_idwt_53(Image *im, ColorSpace color, uint32 steps, uint8 *buf, uint32 isteps)
///	\brief Discrete 5/3 filter invert wavelets transform of the image.
///	\param im	 		The image structure.
///	\param color		The color space of the stream.
///	\param steps 		The steps of DWT transform.
///	\param buf 			The temporary buffer.
///	\param isteps		The steps of IDWT transform.
{
	switch(fb){
		case(FR_HAAR):{
			idwt(im, buf, &idwt_haar_2d_one, steps, isteps);
			break;
		}
		case(FR_5_3):{
			idwt(im, buf, &idwt_53_2d_one, steps, isteps);
			break;
		}
		default:{
			printf("Dont support %d transform type\n", fb);
			return;
		}
	}
}

static void fill_bayer_hist(int16 *img, uint32 *r, uint32 *g, uint32 *b, uint32 w, uint32 h,  BayerGrid bay, uint32 bits)
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

void image_init(Image *img, uint32 w, uint32 h, ColorSpace color, uint32 bpp, uint32 steps)
///	\fn void image_init(Image *im, uint32 width, uint32 height, ColorSpace color, uint32 bpp, uint32 steps)
///	\brief Init image structure.
///	\param im	 		The image structure.
///	\param w 			The image width.
///	\param h 			The image height.
///	\param color		The color space of the stream.
///	\param bpp 			The bits per pixel.
///	\param steps 		The steps of DWT transform.

{
	int i, k, num;
	int16 *tmp;
	img->w = w; img->h = h;
	img->p = (int16 *)calloc(w*h, sizeof(int16));

	img->d.w = w; img->d.h = h;
	img->d.pic = (int16 *)calloc(img->d.w*img->d.h, sizeof(int16));

	/*
	if(steps){
		img->l = (Level *)calloc(steps, sizeof(Level));
		img->l[0].s[0].pic = (uint16 *)calloc(w*h, sizeof(uint16));
		for(i=0; i < 4; i++) {
			img->l[0].s[i].w = (w>>1) + bit_check(w, i);
			img->l[0].s[i].h = (h>>1) + bit_check(h, i>>1);
		}
		for(i=1; i < 4; i++) img->l[0].s[i].pic = img->l[0].s[i-1].pic + img->l[0].s[i-1].w*img->l[0].s[i-1].h;
		for(k=1; k < steps; k++){
			img->l[k].s[0].pic = (uint16 *)calloc(img->l[k-1].s[0].w*img->l[k-1].s[0].h, sizeof(uint16));
			for(i=0; i < 4; i++) {
				img->l[k].s[i].w = (img->l[k-1].s[0].w>>1) + bit_check(img->l[k-1].s[0].w, i);
				img->l[k].s[i].h = (img->l[k-1].s[0].h>>1) + bit_check(img->l[k-1].s[0].h, i>>1);
			}
			for(i=1; i < 4; i++) img->l[k].s[i].pic = img->l[k].s[i-1].pic + img->l[k].s[i-1].w*img->l[k].s[i-1].h;
		}
	}
	*/
	if(steps){
		img->l = (Level *)calloc(steps, sizeof(Level));
		tmp = (uint16 *)calloc(w*h, sizeof(uint16));
		for(i=0; i < 4; i++) {
			subband_init(&img->l[0].s[i], tmp, (w>>1) + bit_check(w, i), (h>>1) + bit_check(h, i>>1), bpp);
			tmp = tmp + img->l[0].s[i-1].w*img->l[0].s[i-1].h;
		}
		for(k=1; k < steps; k++){
			tmp = (uint16 *)calloc(img->l[k-1].s[0].w*img->l[k-1].s[0].h, sizeof(uint16));
			for(i=0; i < 4; i++) {
				subband_init(&img->l[k].s[i], tmp, (img->l[k-1].s[0].w>>1) + bit_check(img->l[k-1].s[0].w, i),
						(img->l[k-1].s[0].h>>1) + bit_check(img->l[k-1].s[0].h, i>>1), bpp);
				tmp = tmp + img->l[k].s[i-1].w*img->l[k].s[i-1].h;
			}
		}
	}

	//Old interface
	//img->imgg = (uint8 *)calloc(w*h, sizeof(uint8));
	//img->iwt = (int16 *)calloc(w*h, sizeof(int16));
	img->hist = (color == BAYER) ? (uint32 *)calloc((1<<bpp)*3, sizeof(uint32)) : (uint32 *)calloc(1<<bpp, sizeof(uint32));
	img->look = (color == BAYER) ? (uint16 *)calloc((1<<bpp)*3, sizeof(uint16)) : (uint16 *)calloc(1<<bpp, sizeof(uint16));
	//imgg->qfl[steps] = 1; for(i=steps-1; i; i--) imgg->qfl[i] += imgg->qfl[i+1]+3; imgg->qfl[0] = imgg->qfl[1]+2;
	num = (color == BAYER) ? steps : steps+1;
	img->qfl  = (uint32 *)calloc(num, sizeof(uint32));
	img->qfl[0] = 1; for(i=1; i< num-1; i++) img->qfl[i] += img->qfl[i-1]+3; img->qfl[num-1] = img->qfl[num-2]+2;
	for(i=0; i<steps; i++) printf("fl[%d] = %d \n", i, img->qfl[i]);

	printf("Create image x = %d y = %d p = %p\n", img->w, img->h, img->p);
}

void image_copy(Image *im, uint32 bpp, uint8 *v)
///	\fn void image_copy(Image *im, uint32 bpp,  *v)
///	\brief Copy image from stream to image structure.
///	\param im	 		The image structure.
///	\param bpp 			The bits per pixel.
///	\param v 			The input stream buffer.
{
	uint32 i, size = im->w*im->h;
	printf("Start copy  x = %d y = %d p = %p \n", im->w, im->h, im->p);
	if(bpp > 8) for(i=0; i<size; i++) im->p[i] = (v[i<<1]<<8) | v[(i<<1)+1];
	else 		for(i=0; i<size; i++) im->p[i] = v[i];
}

void image_fill_subb(Image *im, uint32 steps)
///	\fn void image_fill_subb(Image *im, ColorSpace color, uint32 steps)
///	\brief Fill distribution probability array for each subband after DWT
///			and calculate the number of quantization steps.
///	\param im	 		The image structure.
///	\param color 		The color space of the stream.
///	\param steps 		The steps of DWT transform.
{
	uint32 i, j;
	for(i=0; i < steps; i++) {
		for(j=1; j < 4; j++) {
			subband_fill_prob(&im->l[i].s[j]);
			im->qst += im->l[i].s[j].a_bits-1;
		}
	}
}

void image_fill_hist(Image *im, ColorSpace color, BayerGrid bg, uint32 bpp)
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

void image_bits_per_subband(Image *im, uint32 steps, uint32 qstep)
///	\fn void image_bits_per_subband(Image *im, ColorSpace color, uint32 steps, uint32 qstep)
///	\brief Bits allocation for quantization algorithm.
///	\param im	 		The image structure.
///	\param color 		The color space of the stream.
///	\param steps 		The steps of DWT transform.
///	\param qstep 		The quantization step  (0 <= qstep < qst).
{
	uint32 i, j, k, sz, df;
	Subband *sub = im->sub;
	//qst = 0;
	for(i=0; i < (sz+1); i++) sub[i].q_bits = sub[i].a_bits;
	for(i=0; i < steps; i++) for(j=1; j < 4; j++) im->l[i].s[j].q_bits = im->l[i].s[j].a_bits;
	//printf("stmax = %d\n", stmax);
	qstep = max(qstep, im->qst);
	for(i=0; i < steps; i++) {
		for(j=1; j < 4; j++) {
			im->l[i].s[j].q_bits = im->l[i].s[j].a_bits;
		}
	}

	for(i=0; ; i++){
			for(j=0; j < im->qfl[max(i,steps)]; j++){
				if(sub[sz-j].q_bits > 1) { sub[sz-j].q_bits--; qstep--;}
				if(!qstep) break;
			}
			if(!qstep) break;
		}
		//for(i=0; i < sz+1; i++) printf("%2d ", sub[0][i].q_bits);
		//printf("\n");
}

//QI func = q_i_uniform;
//QI func = q_i_nonuniform;
//QI func = q_i_nonuniform1;

uint32 image_size(Image *im, ColorSpace color, uint32 steps, uint32 qstep)
///	\fn uint32 image_size(Image *im, ColorSpace color, uint32 steps, uint32 qstep)
///	\brief Estimate the image size after quantization and entropy encoder.
///	\param im	 		The image structure.
///	\param color 		The color space of the stream.
///	\param steps 		The steps of DWT transform.
///	\param qstep 		The quantization step  (0 <= qstep < qst).
///	\retval				The size of image in bits.
{
	uint32 i, j, sz, s=0, s1;
	Subband *sub = im->sub;

	if(qstep) image_bits_per_subband(im, color, steps,  qstep);

	sz = (color == BAYER) ? ((steps-1)*3+1)<<2 : steps*3 + 1;
	for(i=0; i < sz; i++) {
		if(sub[i].q_bits > 1) {
			s1 = subband_size(&sub[i]);
			s += s1;
		}
	}
	return s;
}

void image_bits_alloc(Image *im, ColorSpace color, uint32 steps, uint32 bpp, uint32 times)
///	\fn void image_bits_alloc(Image *im, ColorSpace color, uint32 steps, uint32 bpp, uint32 times)
///	\brief Bits allocation for subbands for given compression times.
///	\param im	 		The image structure.
///	\param color 		The color space of the stream.
///	\param steps 		The steps of DWT transform.
///	\param bpp 			The bits per pixel.
///	\param times		Compression times.
{
	uint32 i, s, qstep, size;

	size = (im->w*im->h*bpp)/times;
	qstep = (im->qst>>1);

	for(i=2;;i++){
		s = image_size(im, color, steps, qstep);
		printf("qst = %d size = %d qstep = %d s = %d\n", im->qst, size>>3, qstep, s>>3);
		qstep = (s < size) ? qstep - (im->qst>>i) : qstep + (im->qst>>i);
		if(!(im->qst>>i)) break;
	}

	//printf("q_bits = %d\n", i*((steps-1)*3+1));
	//For test only-----------------------------------------
	//for(i=1; i<5; i++) im->sub[i*((steps-1)*3+1)-1].q_bits = 1;
	Subband *sub = im->sub;
	uint32 sz;
	sz = (color == BAYER) ? ((steps-1)*3+1)<<2 : steps*3 + 1;
	for(i=0; i < sz; i++) {
		if(sub[i].q_bits > 1) printf("%2d %d size = %d entropy = %f q_bits = %d\n", i,
				sub[i].w*sub[i].h, subband_size(&sub[i])>>3, (double)subband_size(&sub[i])/(double)(sub[i].w*sub[i].h), sub[i].q_bits);
	}
	//--------------------------------------------------------
}

void image_quantization(Image *im, ColorSpace color, uint32 steps)
///	\fn void image_quantization(Image *im, ColorSpace color, uint32 steps)
///	\brief Image quantization.
///	\param im	 		The image structure.
///	\param color 		The color space of the stream.
///	\param steps 		The steps of DWT transform.
{
	uint32 i, sz;
	Subband *sub = im->sub;
	//uint8 *img = im->img;

	sz = (color == BAYER) ? ((steps-1)*3+1)<<2 : steps*3 + 1;
	for(i=0; i < sz; i++) {
		//printf("%2d bits = %d q_bits = %d \n", i, sub[i].a_bits, sub[i].q_bits);
		subband_quantization(&im->p[sub[i].loc], &sub[i], func);
	}
}

uint32 image_range_encode(Image *im, ColorSpace color, uint32 steps, uint32 bpp, uint8 *buf)
///	\fn uint32 image_range_encode(Image *im, ColorSpace color, uint32 steps, uint32 bpp,  *buf)
///	\brief Image range encoder.
///	\param im	 		The image structure.
///	\param color 		The color space of the stream.
///	\param steps 		The steps of DWT transform.
///	\param bpp 			The bits per pixel.
///	\param buf 			The buffer for encoded data.
///	\retval				The size of encoded image in bytes.
{
	uint32 i, sq, sz;
	uint32 size = 0, size1=0;
	//uint8 *img = im->img;
	Subband *sub = im->sub;

	sz = (color == BAYER) ? ((steps-1)*3+1)<<2 : steps*3 + 1;
	//i=0; {
	for(i=0; i < sz; i++) {
		sq = sub[i].size.x*sub[i].size.y;
		//printf("%d a_bits = %d q_bits = %d bits = %d\n", i, sub[i].a_bits, sub[i].q_bits, (sub[i].a_bits<<4) | sub[i].q_bits);
		if(sub[i].q_bits >1){
			subband_encode_table(&sub[i], func);
			size1 = range_encoder1(&im->p[sub[i].loc], &sub[i].dist[1<<(bpp+2)],sq, sub[i].a_bits, sub[i].q_bits, &buf[size], sub[i].q);
			size += size1;
			printf("Decode %d a_bits = %d q_bits = %d size = %d comp = %d decom = %d entropy = %d\n",
					i, sub[i].a_bits,  sub[i].q_bits, size, size1, sub[i].size.x*sub[i].size.y, subband_size(&sub[i], func)/8 );
		}
	}
	//printf("Finish range_encoder\n");
	return size;
}

uint32 image_range_decode(Image *im, ColorSpace color, uint32 steps, uint32 bpp, uint8 *buf)
///	\fn uint32 image_range_decode(Image *im, ColorSpace color, uint32 steps, uint32 bpp,  *buf)
///	\brief Image range decoder.
///	\param im	 		The image structure.
///	\param color 		The color space of the stream.
///	\param steps 		The steps of DWT transform.
///	\param bpp 			The bits per pixel.
///	\param buf 			The buffer for encoded data.
///	\retval				The size of decoded image in bytes.
{
	uint32 i, j, sq, sz;
	uint32 size = 0;
	//uint8 *img = im->img;
	Subband *sub = im->sub;

	sz = (color == BAYER) ? ((steps-1)*3+1)<<2 : steps*3 + 1;
	//i=0; {
	for(i=0; i < sz; i++) {
		sq = sub[i].size.x*sub[i].size.y;
		if(sub[i].q_bits >1){
			subband_decode_table(&sub[i], func);
			size += range_decoder1(&im->p[sub[i].loc], &sub[i].dist[1<<(bpp+2)],sq, sub[i].a_bits, sub[i].q_bits, &buf[size], sub[i].q);
			printf("Decode %d a_bits = %d q_bits = %d size = %d\n", i, sub[i].a_bits,  sub[i].q_bits, size);
		} else for(j=0; j<sq; j++) im->p[sub[i].loc+j] = 0;
		//printf("%d a_bits = %d q_bits = %d size = %d\n", i, sub[i].a_bits, sub[i].q_bits, size);
	}
	return size;
}

void image_median_filter(Image *im, ColorSpace color, BayerGrid bg, uint8 *buf)
///	\fn void image_median_filter(Image *im, ColorSpace color, BayerGrid bg, uint8 *buf)
///	\brief Image median filter.
///	\param im	 		The image structure.
///	\param color 		The color space of the stream.
///	\param steps 		The steps of DWT transform.
{
	//if(color == BAYER) filters_bayer_median_3x3(im->img, buf, im->width, im->height, bg);
	//else filters_median_3x3(im->img, buf, im->width, im->height);
}

void image_subband_median_filter(Image *im, ColorSpace color, uint32 steps, uint8 *buf)
///	\fn uint32 image_range_decode(Image *im, ColorSpace color, uint32 steps, uint32 bpp,  *buf)
///	\brief Image range decoder.
///	\param im	 		The image structure.
///	\param color 		The color space of the stream.
///	\param steps 		The steps of DWT transform.
///	\param bpp 			The bits per pixel.
///	\param buf 			The buffer for encoded data.
///	\retval				The size of decoded image in bytes.
{
	uint32 i, sz;
	//uint8 *img = im->img;
	Subband *sub = im->sub;

	sz = (color == BAYER) ? ((steps-1)*3+1)<<2 : steps*3 + 1;
	//for(i=0; i < sz; i++) filters_median_3x3(&img[sub[i].loc], &buf[sub[i].loc], sub[i].size.x, sub[i].size.y);
}

static void grad(int16 *img, int16 *img1, uint32 w, uint32 h, uint32 th)
{
	uint32 sz = w*h, i, mod;
	for(i=0; i < sz; i++){
		mod = abs(img[i]);
		img1[i] = (mod>>th) ? mod<<1 : 0;
	}
}

void image_grad(Image *im, ColorSpace color, uint32 steps, uint32 th)
{
	uint32 i, sz;
	Subband *sub = im->sub;
	//int16 *img = im->iwt;

	sz = (color == BAYER) ? ((steps-1)*3+1)<<2 : steps*3 + 1;
	for(i=1; i < sz; i++) {
		grad(&im->p[sub[i].loc], &im->p[sub[i].loc], sub[i].size.x, sub[i].size.y, th);
	}
}
