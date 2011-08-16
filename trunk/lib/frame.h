#ifndef _FRAME_H_
#define _FRAME_H_

typedef struct{
	Pic16s	b;			//Bayer image 16 bits
	Pic16s	d;			//Bayer image abter DWT -> quantization -> IDWT
	//Pic16s	C[4];	// C[0] - Y component after first BDWT, C[1-3] color component after first BDWT
	//Level	*L[4];		//The levels of DWT transform
	Image 	img[4];		//Pointer to image
	uint32 		qst;	//The number of quantization steps.


	//Old interface
	Pic8u rgb; // Scaled image for each color
	Pic8u Y;	// The Y color componets
	Pic8u grad;	// The gradient

	//For visualization only
	Pic8u line;	//Draw lines
	Pic8u edge;	//Draw edges
	Pic8u vec;	//Draw motion vectors
	//uchar 	*mmb;	//The motion much buffer

	Pixel	*pixs;		//The array of the pixeles
	Edge	*edges;		//The array of edges
	uint32 	nedge;		//The number of edges
	uint32 	size;	//The number of pixels.
	uint8 *buf;	//The pointer to temporary buffer in GOP structure
	uint32 	state;	//The state of frame
}	Frame;

#include <gop.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
//New interface

//Old interface

void 	frames_init			(WaletConfig *wc, Frame *f);
void 	frame_copy			(WaletConfig *wc, Frame *f, uint8 *y, uint8 *u, uint8 *v);
uint32 	frame_dwt			(WaletConfig *wc, Frame *f, uint8 *buf);
uint32 	frame_idwt			(WaletConfig *wc, Frame *f, uint8 *buf, uint32 isteps);
uint32 	frame_fill_subb		(WaletConfig *wc, Frame *f);
uint32	frame_bits_alloc	(WaletConfig *wc, Frame *f, uint32 times);
uint32 	frame_quantization	(WaletConfig *wc, Frame *f, uint8 *buf);
uint32 	frame_range_encode	(WaletConfig *wc,  Frame *f, uint32 *size, uint8 *buf, int *ibuf);
uint32 	frame_range_decode	(WaletConfig *wc,  Frame *f, uint32 *size, uint8 *buf, int *ibuf);
uint32 	frame_write			(WaletConfig *wc, Frame *f, FILE *wl);
void 	frame_compress		(WaletConfig *wc, Frame *f, uint32 times , FilterBank fb);
void 	frame_decompress	(WaletConfig *wc, Frame *f, uint32 isteps, FilterBank fb);

void 	frame_white_balance		(WaletConfig *wc, Frame *f,  uint32 out_bits, Gamma gamma);
uint32 	frame_median_filter	(WaletConfig *wc,  Frame *f, uint8 *buf);
uint32 	frame_subband_median_filter	(WaletConfig *wc, Frame *f);

void 	frame_segmetation	(WaletConfig *wc, Frame *f);
void 	frame_match			(WaletConfig *wc, Frame *f1, Frame *f2);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
