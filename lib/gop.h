#ifndef _GOP_H_
#define _GOP_H_

typedef struct {
	uint32 			width;			// Image width
	uint32 			height;			// Image width
	ColorSpace		color;			// Color space
	BayerGrid		bg;				// Bayer grid pattern
	uint32			bpp;			// Bits per pixel
	uint32 			steps;  		// Steps of DWT
	uint32			gop_size;		// GOP size
	uint32			rates;			// Frame rates
	imgtype			*buf;		//Temporal buffer for DWT, IDW, and range coder
	//StreamData		*sd;		//Pointer to stream data.
	Frame			*frames;	//Pointer to frames array
	Subband			*sub[3];	//Subband location and size structure
	int 			*q;			//Quatntization value array
}	GOP;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

GOP* walet_decoder_init(uint32 width, uint32 height, ColorSpace color, BayerGrid bg, uint32 bpp, uint32 steps, uint32 gop_size, uint32 rates);
GOP* walet_encoder_init(uint32 width, uint32 height, ColorSpace color, BayerGrid bg, uint32 bpp, uint32 steps, uint32 gop_size, uint32 rates);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //_GOP_HH_
