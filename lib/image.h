#ifndef IMAGE_H_
#define IMAGE_H_

typedef void (*fun1d)(int16 *in, int16 *out, const uint32 w);
typedef void (*fun2d)(int16 *in, int16 *out, const uint32 w, const uint32 h);
typedef	void (*funwt)(int16 *in, int16 *out, const uint32 w, const uint32 h);
typedef	void (*funidwt)(int16 *in, int16 *out, const uint32 w, const uint32 h, uint32 *loc);

typedef struct {
	uint32 		width;		// Image width
	uint32 		height;		// Image width
	Dim 		idwts;		//Image size after IDWT, if DWT steps is not equal IDWT steps
	uint8 	*img;		//Pointer to image
	int16		*iwt;		//DWT of image
	uint32 		*hist;		//distribution probabilities array for white balancing
	uint16 		*look;		//Look up table for white balancing and gamma correction.
	uint32 		*qfl;		//The quantization floor
	uint32 		qst;		//The number of quantization steps.
	Subband 	*sub;		//Pointer to subband array
	uint32 		c_size;		//The size of compressed image in the  buffer.
	//uint32 		snum;	//Number of subbands.
}	Image;

/*
static uint16 eng[5][16] = {
	{   4,  2,  2,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, },
	{   8,  4,  4,  2,  2,  2,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0, },
	{  16,  8,  8,  4,  4,  4,  2,  2,  2,  1,  0,  0,  0,  0,  0,  0, },
	{  32, 16, 16,  8,  8,  8,  4,  4,  4,  2,  2,  2,  1,  0,  0,  0, },
	{  64, 32, 32, 16, 16, 16,  8,  8,  8,  4,  4,  4,  2,  2,  2,  1, },
};

static uint16 del[5][16] = {
	{   0,  1,  1,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, },
	{   0,  1,  1,  2,  2,  2,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0, },
	{   0,  1,  1,  2,  2,  2,  3,  3,  3,  4,  0,  0,  0,  0,  0,  0, },
	{   0,  1,  1,  2,  2,  2,  3,  3,  3,  4,  4,  4,  5,  0,  0,  0, },
	{   0,  1,  1,  2,  2,  2,  3,  3,  3,  4,  4,  4,  5,  5,  5,  6, },
};
*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void dwt_2d_haar8(int8 *in, uint16 w, uint16 h, int8 *ll, int8 *hl, int8 *lh, int8 *hh);
void dwt_2d_haar16(int16 *in, uint16 w, uint16 h, int16 *ll, int16 *hl, int16 *lh, int16 *hh);
void idwt_2d_haar8(int8 *out, uint16 w, uint16 h, int8 *ll, int8 *hl, int8 *lh, int8 *hh);
void idwt_2d_haar16(int16 *out, uint16 w, uint16 h, int16 *ll, int16 *hl, int16 *lh, int16 *hh);
void pic_copy(Pic8u *p, char *y);


void 	image_init			(Image *im, uint32 width, uint32 height, ColorSpace color, uint32 bpp, uint32 steps);
void 	image_copy			(Image *im, uint32 bpp, uint8 *v);
void 	image_dwt			(Image *im, ColorSpace color, uint32 steps, int16 *buf, TransformType tt, FilterBank fb);
void 	image_idwt			(Image *im, ColorSpace color, uint32 steps, int16 *buf, uint32 isteps, TransformType tt, FilterBank fb);
void 	image_fill_subb		(Image *im, ColorSpace color, uint32 steps);
void 	image_fill_hist		(Image *im, ColorSpace color, BayerGrid bg, uint32 bpp);
//void 	image_bits_per_subband(Image *im, ColorSpace color, uint32 steps, uint32 qstep);
//uint32 	image_size			(Image *im, ColorSpace color, uint32 steps, uint32 qstep);
void 	image_bits_alloc	(Image *im, ColorSpace color, uint32 steps, uint32 bpp, uint32 times);
void 	image_quantization	(Image *im, ColorSpace color, uint32 steps);
uint32 	image_range_encode	(Image *im, ColorSpace color, uint32 steps, uint32 bpp, uint8 *buf);
uint32	image_range_decode	(Image *im, ColorSpace color, uint32 steps, uint32 bpp, uint8 *buf);
void 	image_median_filter	(Image *im, ColorSpace color, BayerGrid bg, uint8 *buf);
void 	image_subband_median_filter	(Image *im, ColorSpace color, uint32 steps, uint8 *buf);

void image_grad(Image *im, ColorSpace color, uint32 steps, uint32 th);

//void	image_init				(Image *im, StreamData *sd, uint32 x, uint32 y);
//void 	image_copy				(Image *im, StreamData *sd, uint8 *v);
//void 	image_dwt_53 			(Image *im, StreamData *sd, uint8 *buf);
//void 	image_idwt_53			(Image *im, StreamData *sd, uint8 *buf, uint32 steps);
//void 	image_fill_subb			(Image *im, StreamData *sd);
//void 	image_fill_bayer_hist	(Image *im, StreamData *sd);
//uint32 	image_size				(Image *im, StreamData *sd, uint32 qstep);
//void 	image_bits_alloc		(Image *im, StreamData *sd, uint32 times);
//void 	image_quantization		(Image *im, StreamData *sd);
//uint32 	image_range_encode		(Image *im, StreamData *sd, uint8 *buf);
//uint32 	image_range_decode		(Image *im, StreamData *sd, uint8 *buf);

//double 	image_entropy		(Image *im, Subband **sub, uint32 num, uint32 bits, ColorSpace color, uint32 steps, uint32 st);

//void 	image_bits_alloc	(Image *im, Subband **sub, uint32 num, uint32 bits, ColorSpace color, uint32 steps, double per);
//void 	image_make_tables	(Subband **sub, uint32 bits, ColorSpace color, uint32 steps);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* IMAGE_HH_ */
