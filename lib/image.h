#ifndef IMAGE_H_
#define IMAGE_H_

typedef	void (*fundwt)(int16 *in, int16 *ll, int16 *hl, int16 *lh, int16 *hh, int16 *buf, const uint32 w, const uint32 h);
typedef	void (*funidwt)(int16 *out, int16 *ll, int16 *hl, int16 *lh, int16 *hh, int16 *buf, const uint32 w, const uint32 h);

typedef struct {
	int16		*p;		//Image befor DWT
	uint16 		w;		//Image width
	uint16 		h;		//Image width
	Pic16s		d;		//Image after DWT and IDWT for test only
	Level		*l;		//The levels of DWT transform
	uint32 		*qfl;		//The quantization floor
	uint32 		qst;		//The number of quantization steps.
	uint32 		isz;		//The image size after range coder compression.
	//Old interface
	Dim 		idwts;		//Image size after IDWT, if DWT steps is not equal IDWT steps
	//uint8 		*img;		//Pointer to image
	//int16		*iwt;		//DWT of image
	//uint32 		*hist;		//distribution probabilities array for white balancing
	//uint16 		*look;		//Look up table for white balancing and gamma correction.
	//Subband 	*sub;		//Pointer to subband array
	//uint32 		snum;	//Number of subbands.
}	Image;


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void prediction_encoder(int16 *in, int16 *out, uint32 w, uint32 h);
void prediction_decoder(int16 *in, int16 *out, uint32 w, uint32 h);

//New interface
void 	image_init			(Image *im, uint32 width, uint32 height, uint32 bpp, uint32 steps);
//void 	image_copy			(Image *im, uint32 bpp, uint8 *v);
void 	image_dwt			(Image *im, int16 *buf, FilterBank fb, uint32 steps);
void 	image_idwt			(Image *im, int16 *buf, FilterBank fb, uint32 steps, uint32 isteps);
void 	image_fill_subb		(Image *im, uint32 steps);
uint32 	image_size			(Image *im, uint32 steps, int qstep);
void 	image_quantization	(Image *im, uint32 steps, uint8 *buf);
uint32 	image_range_encode	(Image *im, uint32 steps, uint32 bpp, uint8 *buf, int *ibuf, RangeType rt);
uint32 	image_range_decode	(Image *im, uint32 steps, uint32 bpp, uint8 *buf, int *ibuf, RangeType rt);
void 	image_median_filter	(Image *im, uint8 *buf);
void 	image_suband_to_block(Image *img, int16 *out, uint32 steps);


void dwt_53_2d_one(int16 *in, int16 *ll, int16 *hl, int16 *lh, int16 *hh, int16 *buf, const uint32 w, const uint32 h);
void idwt_53_2d_one(int16 *out, int16 *ll, int16 *hl, int16 *lh, int16 *hh, int16 *buf, const uint32 w, const uint32 h);

//For testing only
uint32 image_size_test(Image *im, uint32 steps, uint32 start, uint32 end);
//Old interface


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* IMAGE_HH_ */
