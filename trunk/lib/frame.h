#ifndef _FRAME_H_
#define _FRAME_H_


typedef struct{
	Image 	img[3];	//Pointer to image
	Picture rgb[4]; // Scaled image for each color
	Picture Y[4];	// The Y color componets
	Picture grad[4];	// The gradient
	Picture con[4];		// The contours
	Picture pix[4];		// The clustering pixels
	//Picture y[2]; 	//y[0] Y component y[1] gradient
	//Picture pic[4]; 	// Scaled image for each color
	uint32 	size;	//The number of pixels.
	imgtype *buf;	//The pointer to temporary buffer in GOP structure
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

void 	frames_init			(GOP *gop, uint32 fr);
void 	frame_copy			(GOP *gop, uint32 fr, uchar *y, uchar *u, uchar *v);
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

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
