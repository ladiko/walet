#include <walet.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//|----|----|---------|
//|    | 4  |         |
//|----|----|    2    |
//|  4 | 2  |         |
//|----|----|---------|
//|         |         |
//|    2    |    1    |
//|         |         |
//|---------|---------|



#define lim(max,min, x)  	((x) > max ? max :((x) < min ? min : (x)))
#define max(x, m) 			(((x) > m) ? (m) : (x))


void dwt_haar_2d_one(int16 *in, int16 *ll, int16 *hl, int16 *lh, int16 *hh, int16 *buf, const uint32 w, const uint32 h){
///	\fn void dwt_2d_haar8(int8 *in, uint16 w, uint16 h, int8 *out0, int8 *out1, int8 *out2, int8 *out3)
///	\brief One step 2D Haar DWT transform.
///	\param in	 		The input image data.
///	\param w 			The width
///	\param h 			The height
///	\param ll	 		The pointer to LL subbabnd
///	\param hl	 		The pointer to HL subbabnd
///	\param lh	 		The pointer to LH subbabnd
///	\param hh	 		The pointer to HH subbabnd

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

void idwt_haar_2d_one(int16 *out, int16 *ll, int16 *hl, int16 *lh, int16 *hh, int16 *buf, const uint32 w, const uint32 h){
///	\fn void idwt_2d_haar8(int8 *out, uint16 w, uint16 h, int8 *ll, int8 *hl, int8 *lh, int8 *hh)
///	\brief One step 2D Haar IDWT transform.
///	\param out	 		The input image data.
///	\param w 			The width
///	\param h 			The height
///	\param ll	 		The pointer to LL subbabnd
///	\param hl	 		The pointer to HL subbabnd
///	\param lh	 		The pointer to LH subbabnd
///	\param hh	 		The pointer to HH subbabnd

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

/**	\brief One step 1D 5/3 discrete wavelet transform.
	\param in	The input data array.
	\param out 	The output data array.
	\param w 	The length.
*/
static inline void dwt_53_1d(int16 *in, int16 *out, const uint32 w)
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

/**	\brief One step 1D 5/3 invert discrete wavelet transform.
	\param in	The input data array.
	\param out 	The output data array.
	\param w 	The length.
*/
static inline void idwt_53_1d(int16 *in, int16 *out, const uint32 w){

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

/**	\brief One step 2D 5/3 discrete wavelet transform.
	\param in	The input image.
	\param ll 	The LL low-low frequency subband.
	\param hl 	The HL height-low frequency subband.
	\param lh 	The LH low-height frequency subband.
	\param hh 	The HH low-low frequency subband.
	\param buf	The temporal buffer.
	\param w 	The image width.
	\param h 	The image height.
*/
void dwt_53_2d_one(int16 *in, int16 *ll, int16 *hl, int16 *lh, int16 *hh, int16 *buf, const uint32 w, const uint32 h)
{
	uint32 i,ik, ik1, k = 0, k1, k2, sz = (h-1)*w, hw = w<<1;
	uint32 h2 = (h>>1), h1 = (h>>1) + (h&1), w2 = (w>>1), w1 = (w>>1) + (w&1);
	int16 *l[3], *tm;
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

/**	\brief One step 2D 5/3 invert discrete wavelet transform.
	\param out	The output image.
	\param ll 	The LL low-low frequency subband.
	\param hl 	The HL height-low frequency subband.
	\param lh 	The LH low-height frequency subband.
	\param hh 	The HH low-low frequency subband.
	\param buf	The temporal buffer.
	\param w 	The image width.
	\param h 	The image height.
*/
void idwt_53_2d_one(int16 *out, int16 *ll, int16 *hl, int16 *lh, int16 *hh, int16 *buf, const uint32 w, const uint32 h)
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

/**	\brief Discrete wavelet transform.
	\param img	 		The pointer to input image structure.
	\param buf 			The temporary buffer.
	\param dwt_one		The pointer to function for one step 2d DWT.
	\param steps 		The steps of transform.
 */
void dwt(Image *img, int16 *buf, fundwt dwt_one, uint32 steps)
{
	uint32 k, i;
	if(steps){
		(*dwt_one)(img->p, img->l[0].s[0].pic, img->l[0].s[1].pic, img->l[0].s[2].pic, img->l[0].s[3].pic, buf, img->w, img->h);
		for(k=1; k < steps; k++){
			(*dwt_one)(img->l[k-1].s[0].pic, img->l[k].s[0].pic, img->l[k].s[1].pic, img->l[k].s[2].pic, img->l[k].s[3].pic, buf,
					img->l[k-1].s[0].w, img->l[k-1].s[0].h);

		}
	}
}

/**	\brief Invert discrete wavelet transform.
	\param img	 		The pointer to output  image structure.
	\param buf 			The temporary buffer.
	\param idwt_one		The pointer to function for one step 2d IDWT.
	\param steps 		The steps of DWT transform.
	\param isteps 		The steps of IDWT transform.
 */
void idwt(Image *img, int16 *buf, funidwt idwt_one, uint32 steps, uint32 isteps)
{
	int j, k;
	if(isteps > steps){
		printf("istep should be less or equal steps\n");
		return;
	}
	for(k=steps-1; k > steps-isteps; k--){
		(*idwt_one)(img->l[k-1].s[0].pic, img->l[k].s[0].pic, img->l[k].s[1].pic, img->l[k].s[2].pic, img->l[k].s[3].pic,
				 buf, img->l[k-1].s[0].w, img->l[k-1].s[0].h);
	}
	img->d.w = img->l[k].s[0].w + img->l[k].s[1].w;
	img->d.h = img->l[k].s[0].h + img->l[k].s[2].h;
	(*idwt_one)(img->d.pic, img->l[k].s[0].pic, img->l[k].s[1].pic, img->l[k].s[2].pic, img->l[k].s[3].pic, buf, img->d.w, img->d.h);
}

void image_init(Image *img, uint32 w, uint32 h, ColorSpace color, uint32 bpp, uint32 steps){
///	\fn void image_init(Image *im, uint32 width, uint32 height, ColorSpace color, uint32 bpp, uint32 steps)
///	\brief Init image structure.
///	\param im	 		The image structure.
///	\param w 			The image width.
///	\param h 			The image height.
///	\param color		The color space of the stream.
///	\param bpp 			The bits per pixel.
///	\param steps 		The steps of DWT transform.

	int i, k, num;
	int16 *tmp;
	img->w = w; img->h = h;
	img->p = (int16 *)calloc(w*h, sizeof(int16));

	img->d.w = w; img->d.h = h;
	img->d.pic = (int16 *)calloc(img->d.w*img->d.h, sizeof(int16));

	if(steps){
		img->l = (Level *)calloc(steps, sizeof(Level));
		tmp = (uint16 *)calloc(w*h, sizeof(uint16));
		for(i=0; i < 4; i++) {
			subb_init(&img->l[0].s[i], tmp, (w>>1) + bit_check(w, i), (h>>1) + bit_check(h, i>>1), bpp);
			//printf("l[%d].s[%d] = %p w = %d h = %d\n", 0, i, tmp,  img->l[0].s[i].w, img->l[0].s[i].h);
			tmp = tmp + img->l[0].s[i].w*img->l[0].s[i].h;
		}
		for(k=1; k < steps; k++){
			tmp = (uint16 *)calloc(img->l[k-1].s[0].w*img->l[k-1].s[0].h, sizeof(uint16));
			for(i=0; i < 4; i++) {
				subb_init(&img->l[k].s[i], tmp, (img->l[k-1].s[0].w>>1) + bit_check(img->l[k-1].s[0].w, i),
						(img->l[k-1].s[0].h>>1) + bit_check(img->l[k-1].s[0].h, i>>1), bpp);
				//printf("l[%d].s[%d] = %p w = %d h = %d\n", 0, i, tmp,  img->l[k].s[i].w, img->l[k].s[i].h);
				tmp = tmp + img->l[k].s[i].w*img->l[k].s[i].h;
			}
		}
	}

	img->qfl  = (uint32 *)calloc(steps, sizeof(uint32));
	//Old interface
	//img->imgg = (uint8 *)calloc(w*h, sizeof(uint8));
	//img->iwt = (int16 *)calloc(w*h, sizeof(int16));
	//img->hist = (color == BAYER) ? (uint32 *)calloc((1<<bpp)*3, sizeof(uint32)) : (uint32 *)calloc(1<<bpp, sizeof(uint32));
	//img->look = (color == BAYER) ? (uint16 *)calloc((1<<bpp)*3, sizeof(uint16)) : (uint16 *)calloc(1<<bpp, sizeof(uint16));
	//imgg->qfl[steps] = 1; for(i=steps-1; i; i--) imgg->qfl[i] += imgg->qfl[i+1]+3; imgg->qfl[0] = imgg->qfl[1]+2;
	//num = (color == BAYER) ? steps : steps+1;
	//img->qfl[0] = 1; for(i=1; i< num-1; i++) img->qfl[i] += img->qfl[i-1]+3; img->qfl[num-1] = img->qfl[num-2]+2;
	//for(i=0; i<steps; i++) printf("fl[%d] = %d \n", i, img->qfl[i]);

	printf("Create image x = %d y = %d p = %p\n", img->w, img->h, img->p);
}

void image_dwt(Image *im, int16 *buf, FilterBank fb, uint32 steps){
///	\fn void image_dwt_53(Image *im, ColorSpace color, uint32 steps, uint8 *buf)
///	\brief Discrete 5/3 filter wavelets transform of the image.
///	\param im	 		The image structure.
///	\param color		The color space of the stream.
///	\param steps 		The steps of DWT transform.
///	\param buf 			The temporary buffer.

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
			printf("Dont support %d wavelet transform bank\n", fb);
			return;
		}
	}
}

void image_idwt(Image *im, int16 *buf, FilterBank fb, uint32 steps, uint32 isteps){
///	\fn void image_idwt_53(Image *im, ColorSpace color, uint32 steps, uint8 *buf, uint32 isteps)
///	\brief Discrete 5/3 filter invert wavelets transform of the image.
///	\param im	 		The image structure.
///	\param color		The color space of the stream.
///	\param steps 		The steps of DWT transform.
///	\param buf 			The temporary buffer.
///	\param isteps		The steps of IDWT transform.

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

void image_copy(Image *im, uint32 bpp, uint8 *v){
///	\fn void image_copy(Image *im, uint32 bpp,  *v)
///	\brief Copy image from stream to image structure.
///	\param im	 		The image structure.
///	\param bpp 			The bits per pixel.
///	\param v 			The input stream buffer.

	uint32 i, size = im->w*im->h;
	printf("Start copy  x = %d y = %d p = %p \n", im->w, im->h, im->p);
	if(bpp > 8) for(i=0; i<size; i++) im->p[i] = (v[i<<1]<<8) | v[(i<<1)+1];
	else 		for(i=0; i<size; i++) im->p[i] = v[i];
}

void image_fill_subb(Image *im, uint32 steps){
///	\fn void image_fill_subb(Image *im, ColorSpace color, uint32 steps)
///	\brief Fill distribution probability array for each subband after DWT
///			and calculate the number of quantization steps.
///	\param im	 		The image structure.
///	\param color 		The color space of the stream.
///	\param steps 		The steps of DWT transform.

	uint32 i, j;
	for(i=0; i < steps; i++) {
		for(j = (i == steps-1) ? 0 : 1; j < 4; j++) {
			subb_fill_prob(&im->l[i].s[j]);
			im->qst += im->l[i].s[j].a_bits-1;
			//printf("l[%2d][%2d] a_bits = %d\n", i, j, im->l[i].s[j].a_bits);
		}
	}
	//Remove LL subband from quantization
	im->qst -= (im->l[steps-1].s[0].a_bits-1);
	//printf("im->qst = %d\n", im->qst);
}

uint32 image_size1(Image *im, uint32 steps, int qstep){
///	\fn void image_bits_per_subband(Image *im, ColorSpace color, uint32 steps, uint32 qstep)
///	\brief Bits allocation for quantization algorithm.
///	\param im	 		The image structure.
///	\param color 		The color space of the stream.
///	\param steps 		The steps of DWT transform.
///	\param qstep 		The quantization step  (0 <= qstep < qst).

//The quantization sequence
//|----|----|---------| |----|----|---------| |----|----|---------| |----|----|---------|-------------------|
//|    | 1  |         | |    | 2  |         | |    | 3  |         | |    | 4  |         |                   |
//|----|----|         | |----|----|    1    | |----|----|    1    | |----|----|    2    |                   |
//|  1 |    |         | |  2 | 1  |         | |  3 | 1  |         | |  4 | 2  |         |                   |
//|----|----|---------| |----|----|---------| |----|----|---------| |----|----|---------|         1         |
//|         |         | |         |         | |         |         | |         |         |                   |
//|         |         | |    1    |         | |    1    |         | |    2    |    1    |                   |
//|         |         | |         |         | |         |         | |         |         |                   |
//|---------|---------| |---------|---------| |---------|---------| |---------|---------|-------------------|
//                                                                  |                   |                   |
//                                                                  |                   |                   |
//                                                                  |                   |                   |
//                                                                  |         1         |                   |
//                                                                  |                   |                   |
//                                                                  |                   |                   |
//                                                                  |                   |                   |
//                                                                  |-------------------|-------------------|

//|----|----|---------| |----|----|---------| |----|----|---------| |----|----|---------|-------------------|
//|    | 5  |         | |    | 6  |         | |    | 7  |         | |    | 8  |         |                   |
//|----|----|    2    | |----|----|    3    | |----|----|    3    | |----|----|    4    |                   |
//|  5 | 2  |         | |  6 | 3  |         | |  7 | 3  |         | |  8 | 4  |         |                   |
//|----|----|---------| |----|----|---------| |----|----|---------| |----|----|---------|         2         |
//|         |         | |         |         | |         |         | |         |         |                   |
//|    2    |    1    | |    3    |    1    | |    3    |         | |    4    |    2    |                   |
//|         |         | |         |         | |         |         | |         |         |                   |
//|---------|---------| |---------|---------| |---------|---------| |---------|---------|-------------------|
//                                                                  |                   |                   |
//                                                                  |                   |                   |
//                                                                  |                   |                   |
//                                                                  |         2         |         1         |
//                                                                  |                   |                   |
//                                                                  |                   |                   |
//                                                                  |                   |                   |
//                                                                  |-------------------|-------------------|


	//The order of subband bits allocation
	uint32 qo[5] = { 1, 2, 1, 2, 3};
	uint32 i, j, k, l, size = 0, size1;
	int *st, tmp;
	st = im->qfl;
	// Levels bits counter should be less than 5
	for(i=0; i < steps; i++) st[i] = 0;
	for(i=0; i < steps; i++) for(j=1; j < 4; j++) im->l[i].s[j].q_bits = 0;
	//printf("stmax = %d\n", stmax);

	//Bits allocation for each subband old algorithm
	/*
	for(k=0, i = 0; k < qstep; k++){
		j = qo[st[i]];
		l = steps-1-i;
		if(im->l[l].s[j].q_bits < im->l[l].s[j].a_bits) im->l[l].s[j].q_bits = im->l[l].s[j].q_bits ? im->l[l].s[j].q_bits + 1 : 2;
		else k--;
		//st[i]++;
		//printf("k = %d st[%d] = %d  l[%d].s[%d].q_bits = %d\n", k, i, st[i], i, j, im->l[i].s[j].q_bits);

		if(st[i] == 1 ){
			if(i) { st[0] = -1; st[i]++; }
			i = 0;
		}
		if(st[i] == 4){
			st[i] = 0;
			if(i == steps - 1) i = 0;
			else i++;
		} else st[i]++;

		//st[i] = (st[i] == 4) ? 0 : st[i] + 1;
	}*/

	//Bits allocation for each subband new algorithm
	if(qstep){
		for(l = steps-1; ; l--){
			qstep = qstep - (im->l[l].s[1].a_bits + im->l[l].s[2].a_bits - 2);
			if (qstep >= 0){
				im->l[l].s[1].q_bits = im->l[l].s[1].a_bits;
				im->l[l].s[2].q_bits = im->l[l].s[2].a_bits;
				//printf("l[%d].s[%d] = %d %d\n", l, 1, im->l[l].s[1].q_bits, im->l[l].s[1].a_bits);
				//printf("l[%d].s[%d] = %d %d\n", l, 2, im->l[l].s[2].q_bits, im->l[l].s[2].a_bits);
				if(!qstep) break;
			} else {
				tmp = qstep + (im->l[l].s[1].a_bits + im->l[l].s[2].a_bits - 2);
				im->l[l].s[1].q_bits = (tmp>>1) + (tmp&1);
				im->l[l].s[2].q_bits = (tmp>>1);
				im->l[l].s[1].q_bits = im->l[l].s[1].q_bits ? im->l[l].s[1].q_bits + 1 : im->l[l].s[1].q_bits;
				im->l[l].s[2].q_bits = im->l[l].s[2].q_bits ? im->l[l].s[2].q_bits + 1 : im->l[l].s[2].q_bits;
				//printf("l[%d].s[%d] = %d %d\n", l, 1, im->l[l].s[1].q_bits, im->l[l].s[1].a_bits);
				//printf("l[%d].s[%d] = %d %d\n", l, 2, im->l[l].s[2].q_bits, im->l[l].s[2].a_bits);
				break;
			}
			qstep = qstep - im->l[l].s[3].a_bits + 1;
			if (qstep >= 0){
				im->l[l].s[3].q_bits = im->l[l].s[3].a_bits;
				//printf("l[%d].s[%d] = %d %d\n", l, 3, im->l[l].s[3].q_bits, im->l[l].s[3].a_bits);
				if(!qstep) break;
			} else {
				tmp = qstep + im->l[l].s[3].a_bits - 1;
				im->l[l].s[3].q_bits = tmp + 1;
				//printf("l[%d].s[%d] = %d %d\n", l, 3, im->l[l].s[3].q_bits, im->l[l].s[3].a_bits);
				break;
			}
		}
	}

	//Calculate image size for given quantization step (qstep)
	for(i=0; i < steps; i++) for(j = (i == steps-1) ? 0 : 1; j < 4; j++) {
		size1 = subb_size(&im->l[i].s[j]);
		size += size1;
		printf("a_bits = %2d q_bits  = %2d l[%d].s[%d] = %d\n", im->l[i].s[j].a_bits, im->l[i].s[j].q_bits, i, j, size1);
	}
	printf("Image size = %d\n", size);
	return size;
}

uint32 image_size(Image *im, uint32 steps, int sz){
///	\fn void image_bits_per_subband(Image *im, ColorSpace color, uint32 steps, uint32 qstep)
///	\brief Bits allocation for quantization algorithm.
///	\param im	 		The image structure.
///	\param color 		The color space of the stream.
///	\param steps 		The steps of DWT transform.
///	\param qstep 		The quantization step  (0 <= qstep < qst).

	//The order of subband bits allocation
	uint32 i, j, k, l, size = 0, size1, min, in, jn;
	int *st, tmp;
	st = im->qfl;
	// Levels bits counter should be less than 5
	for(i=0; i < steps; i++) st[i] = 0;
	for(i=0; i < steps; i++) for(j=1; j < 4; j++) {
		im->l[i].s[j].q_bits = 2;
		im->l[i].s[j].ssq = subb_size(&im->l[i].s[j]);
		im->l[i].s[j].ssd = im->l[i].s[j].ssq;
		im->l[i].s[j].q_bits = 0;
		//printf("%3d %3d q = %3d a = %3d size = %d dif = %d\n", i, j, im->l[i].s[j].q_bits, im->l[i].s[j].a_bits, im->l[i].s[j].ssq, im->l[i].s[j].ssd);
	}

	//Calculate image size for given quantization step (qstep)
	size = subb_size(&im->l[steps-1].s[0]);
	while(size < sz){
		min = 0xFFFFFFFF;
		//printf("\n");
		for(i=0; i < steps; i++) for(j = 1; j < 4; j++) {

			if(im->l[i].s[j].ssd < min) { min = im->l[i].s[j].ssd; in = i; jn = j; }


			//printf("a_bits = %2d q_bits  = %2d l[%d].s[%d] = %d\n", im->l[i].s[j].a_bits, im->l[i].s[j].q_bits, i, j, size1);
		}
		size += min;
		im->l[in].s[jn].q_bits = im->l[in].s[jn].q_bits ? im->l[in].s[jn].q_bits + 1 : 2;

		if(im->l[in].s[jn].q_bits < im->l[in].s[jn].a_bits) {

			im->l[in].s[jn].q_bits++;
			im->l[in].s[jn].ssd = subb_size(&im->l[in].s[jn]) - im->l[in].s[jn].ssq;
			im->l[in].s[jn].ssq = im->l[in].s[jn].ssq + im->l[in].s[jn].ssd;
			im->l[in].s[jn].q_bits--;
		} else {
			im->l[in].s[jn].ssd = 0xFFFFFFFF;
		}
	}

	for(i=0; i < steps; i++) for(j = 1; j < 4; j++)
		printf("%3d %3d q = %3d a = %3d size = %d dif = %d\n", i, j,  im->l[i].s[j].q_bits, im->l[i].s[j].a_bits, im->l[i].s[j].ssq, im->l[i].s[j].ssd);

	printf("size = %d\n", size);
	return size;
}

uint32 image_size_test(Image *im, uint32 steps, uint32 start, uint32 end){
///	\fn void image_bits_per_subband(Image *im, ColorSpace color, uint32 steps, uint32 qstep)
///	\brief Bits allocation for quantization algorithm.
///	\param im	 		The image structure.
///	\param color 		The color space of the stream.
///	\param steps 		The steps of DWT transform.
///	\param qstep 		The quantization step  (0 <= qstep < qst).

	//The order of subband bits allocation
	uint32 qo[5] = { 1, 2, 1, 2, 3};
	uint32 i, j, k, l, size = 0, size1;
	int *st;
	st = im->qfl;
	// Levels bits counter should be less than 5
	for(i=0; i < steps; i++) st[i] = 0;
	for(i=start; i < end; i++) for(j=1; j < 4; j++) im->l[i].s[j].q_bits = 0;
	//printf("stmax = %d\n", stmax);

	/*
	for(i=0; i < steps; i++) for(j=1; j < 4; j++)			//subb_fill_prob(&im->l[i].s[j]);
			im->l[i].s[j].q_bits = (j == 3) ? 0 : im->l[i].s[j].a_bits;
			//printf("l[%2d][%2d] a_bits = %d\n", i, j, im->l[i].s[j].a_bits);

	*/

	//Calculate image size for given quantization step (qstep)
	for(i=0; i < steps; i++) for(j = (i == steps-1) ? 0 : 1; j < 4; j++) {
		size1 = subb_size(&im->l[i].s[j]);
		size += size1;
		//printf("a_bits = %2d q_bits  = %2d l[%d].s[%d] = %d\n", im->l[i].s[j].a_bits, im->l[i].s[j].q_bits, i, j, size1);
	}
	printf("size = %d\n", size);
	return size;
}

//QI func = q_i_uniform;
//QI func = q_i_nonuniform;
//QI func = q_i_nonuniform1;
/*
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
*/
/*
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
*/
void image_quantization(Image *im, uint32 steps, uint8 *buf){
///	\fn void image_quantization(Image *im, ColorSpace color, uint32 steps)
///	\brief Image quantization.
///	\param im	 		The image structure.
///	\param color 		The color space of the stream.
///	\param steps 		The steps of DWT transform.

	uint32 i, j;
	//Subband *sub = im->sub;
	//uint8 *img = im->img;
	for(i=0; i < steps; i++) for(j=1; j < 4; j++) subb_quantization(&im->l[i].s[j], (int *)buf);

}

/**	\brief Image range encoder.
	\param im	 		The image structure.
	\param steps 		The steps of DWT transform.
	\param bpp 			The image bits per pixel.
	\param buf	 		The buffer for encoded data.
	\param ibuf 		The pointer to temporal buffer.
	\param rt 			The type of range encoder
	\retval				The size of encoded image in bytes.
*/
uint32 image_range_encode(Image *im, uint32 steps, uint32 bpp, uint8 *buf, int *ibuf, RangeType rt){
	int i, j, k, sq;
	uint32 size = 0;
	int *q = ibuf;
	for(i=steps-1; i+1; i--)
		//for(j = 1; j < 4; j++) {
		for(j = (i == steps-1) ? 0 : 1; j < 4; j++) {
			if(im->l[i].s[j].q_bits >1){
				sq = im->l[i].s[j].w*im->l[i].s[j].h;
				subb_encode_table(&im->l[i].s[j], q);
				//printf("l[%d].s[%d] a_bits = %d q_bits = %d\n",i, j, im->l[i].s[j].a_bits,  im->l[i].s[j].q_bits);
				switch(rt){
					case(ADAP):{
						im->l[i].s[j].ssz = range_encoder_ad(im->l[i].s[j].pic, sq, im->l[i].s[j].a_bits,
								im->l[i].s[j].q_bits, &buf[size], q, im->l[i].s[j].dist);
						break; }
					case(NADAP):{
						im->l[i].s[j].ssz = range_encoder(im->l[i].s[j].pic, sq, im->l[i].s[j].a_bits,
								im->l[i].s[j].q_bits, &buf[size], q, im->l[i].s[j].dist, &ibuf[1<<(bpp+2)]);
						break; }
					case(FAST):{
						if(j == 0) im->l[i].s[j].ssz = write_array(im->l[i].s[j].pic, sq, im->l[i].s[j].a_bits, &buf[size], q);
						else	im->l[i].s[j].ssz = range_encoder_fast(im->l[i].s[j].pic, sq, im->l[i].s[j].a_bits,
								im->l[i].s[j].q_bits, &buf[size], q, im->l[i].s[j].dist, &ibuf[1<<(bpp+2)]);
						break; }
					default:{
						printf("Don't support %d range coder type\n", rt);
						return; }
				}
				//size1 = range_encoder(im->l[i].s[j].pic, sq, im->l[i].s[j].a_bits, im->l[i].s[j].q_bits, &buf[size], q, (uint32*)&ibuf[1<<(bpp+2)]);
				size += im->l[i].s[j].ssz;
				printf("l[%d].s[%d] a_bits = %d q_bits = %d comp = %d decom = %d entropy = %d copm = %f ef = %f\n",
						i, j, im->l[i].s[j].a_bits,  im->l[i].s[j].q_bits, im->l[i].s[j].ssz, sq, subb_size(&im->l[i].s[j])>>3,
						((float)im->l[i].s[j].ssz/(float)sq), ((float)(subb_size(&im->l[i].s[j])>>3)/(float)im->l[i].s[j].ssz));
			}
	}
	printf("Image size = %d\n", size);
	return size;
}

/**	\brief Image range encoder.
	\param im	 		The image structure.
	\param steps 		The steps of DWT transform.
	\param bpp 			The image bits per pixel.
	\param buf	 		The buffer for encoded data.
	\param ibuf 		The pointer to temporal buffer.
	\param rt 			The type of range decoder
	\retval				The size of encoded image in bytes.
*/
uint32 image_range_decode(Image *im, uint32 steps, uint32 bpp, uint8 *buf, int *ibuf, RangeType rt){
	int i, j, k,sq;
	uint32 size = 0, size1=0;
	int *q = ibuf;

	for(i=steps-1; i+1; i--)
		for(j = (i == steps-1) ? 0 : 1; j < 4; j++) {
			sq = im->l[i].s[j].w*im->l[i].s[j].h;
			if(im->l[i].s[j].q_bits >1){
				subb_decode_table(&im->l[i].s[j], q);
				switch(rt){
					case(ADAP):{
						size1 = range_decoder_ad(im->l[i].s[j].pic, sq, im->l[i].s[j].a_bits,
								im->l[i].s[j].q_bits, &buf[size], q, &ibuf[1<<(bpp+2)]);
						break; }
					case(NADAP):{
						size1 = range_decoder(im->l[i].s[j].pic, sq, im->l[i].s[j].a_bits,
								im->l[i].s[j].q_bits, &buf[size], q, &ibuf[1<<(bpp+2)]);
						break; }
					case(FAST):{
						if(j == 0) size1 = read_array(im->l[i].s[j].pic, sq, im->l[i].s[j].a_bits, &buf[size], q);
						else	size1 = range_decoder_fast(im->l[i].s[j].pic, sq, im->l[i].s[j].a_bits,
								im->l[i].s[j].q_bits, &buf[size], q, &ibuf[1<<(bpp+2)]);
						break; }
					default:{
						printf("Don't support %d range coder type\n", rt);
						return; }
				}
				size += size1;
				printf("l[%d].s[%d] a_bits = %d q_bits = %d comp = %d decom = %d entropy = %d copm = %f ef = %f\n",
						i, j, im->l[i].s[j].a_bits,  im->l[i].s[j].q_bits, size1, sq, subb_size(&im->l[i].s[j])>>3,
						((float)size1/(float)sq), ((float)(subb_size(&im->l[i].s[j])>>3)/(float)size1));
			} else for(k=0; k < sq; k++) im->l[i].s[j].pic[k] = 0;
	}
	return size;
}

void image_median_filter(Image *im, uint8 *buf){
///	\fn void image_median_filter(Image *im, ColorSpace color, BayerGrid bg, uint8 *buf)
///	\brief Image median filter.
///	\param im	 		The image structure.
///	\param color 		The color space of the stream.
///	\param steps 		The steps of DWT transform.

	filter_median(im->p, (int16*)buf, im->w, im->h);
}

uint32 image_write_level(Image *im, FILE *wl, uint32 steps, uint32 lev)
{

}

