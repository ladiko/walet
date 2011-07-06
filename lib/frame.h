#ifndef _FRAME_H_
#define _FRAME_H_

typedef struct{
	BAY8	B8;
	BAY16	B16;
	YUV8 	YUV;
	/*
	Pic8u	BAY8;		//Bayer image 8 bits
	Pic16u	BAY16;		//Bayer image more than 8 bits
	Pic8u	Y;			//Y image for 420 422 444 or Y component after first bayer DWT color transform (BDWT)
	Pic8u	U;			//U image for 420 422 444 or color component after first BDWT
	Pic8u	V;			//V image for 420 422 444 or color component after first BDWT
	Pic8s	W;			//W The last color component after first BDWT
	Pic16s	Y;			//Y image for 420 422 444 or Y component after first bayer DWT color transform (BDWT)
	Pic16s	U;			//U image for 420 422 444 or color component after first BDWT
	Pic8u	V;			//V image for 420 422 444 or color component after first BDWT
	Pic8s	W;			//W The last color component after first BDWT
	//Pic8s	C2;			//C2 color component after first BDWT
	//Pic8s	C3;			//C3 color component after first BDWT

	 */

	Level8	**L8;		//The levels of DWT transform
	Level16	**L16;		//The levels of DWT transform

	//Old interface
	Image 	img[3];	//Pointer to image

	Pic8u rgb; // Scaled image for each color
	Pic8u Y;	// The Y color componets
	Pic8u grad;	// The gradient

	//For visualization only
	Pic8u line;	//Draw lines
	Pic8u edge;	//Draw edges
	Pic8u vec;	//Draw motion vectors
	//uchar 	*mmb;	//The motion much buffer

	Pixel	*pixs;		//The array of the pixeles
	Edge	*edges;		//The array of adges
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

uint32 frame_dwt_new(GOP *gop, uint32 fr, FilterBank fb);
uint32 frame_idwt_new(GOP *gop, uint32 fr, FilterBank fb, uint32 istep);

void 	frames_init			(GOP *gop, uint32 fr);
void 	frame_copy			(GOP *gop, uint32 fr, int8 *y, int8 *u, int8 *v);
uint32 	frame_dwt			(GOP *gop, uint32 fr, FilterBank fb);
uint32 	frame_idwt			(GOP *gop, uint32 fr, uint32 isteps, FilterBank fb);
uint32 	frame_fill_subb		(GOP *gop, uint32 fr);
uint32	frame_bits_alloc	(GOP *gop, uint32 fr, uint32 times);
uint32 	frame_quantization	(GOP *gop, uint32 fr);
uint32 	frame_range_encode	(GOP *gop, uint32 fr, uint32 *size);
uint32 	frame_range_decode	(GOP *gop, uint32 fr, uint32 *size);
uint32 	frame_write			(GOP *gop, uint32 fr, FILE *wl);
void 	frame_compress		(GOP *gop, uint32 fr, uint32 times , FilterBank fb);
void 	frame_decompress	(GOP *gop, uint32 fr, uint32 isteps, FilterBank fb);

void 	frame_white_balance		(GOP *gop, uint32 fr,  uint32 out_bits, Gamma gamma);
uint32 	frame_median_filter		(GOP *gop, uint32 fr);
uint32 	frame_subband_median_filter	(GOP *gop, uint32 fr);

void 	frame_segmetation	(GOP *gop, uint32 fr);
void 	frame_match			(GOP *gop, uint32 fr1, uint32 fr2);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
