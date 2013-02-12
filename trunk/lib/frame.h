#ifndef _FRAME_H_
#define _FRAME_H_

typedef struct{
	Pic16s		b;		//Bayer image 16 bits
    Pic16s		d;		//Bayer image abter DWT -> quantization -> IDWT for test only
    Pic16s		g;		//Bayer image abter DWT -> quantization -> IDWT for test only
    //Pic32u      in;     //The integral image
    //Pic32u      hs;     //Determinat of Hessian

    Pic16s		R16, G16, B16;	//Bayer image 16 bits
	Pic8u		R8, G8, B8;		//Bayer image 16 bits
    Pic16s		Y16, U16, V16;	//Bayer image abter DWT -> quantization -> IDWT for test only

	Pic8u 		*R;		//Red color image
	Pic8u 		*G;		//Green color image
	Pic8u 		*B;		//Blue color image
    Pic8u 		*R1;	//Red color image for test only.
    Pic8u 		*G1;	//Green color image for test only.
    Pic8u 		*B1;	//Blue color image for test only.

	Pic8u 		*y;		//Y color image
	Pic8u 		*u;		//U color image
	Pic8u 		*v;		//V color image
    Pic8u 		*y1;	//Y color image
    Pic8u 		*u1;	//U color image
    Pic8u 		*v1;	//V color image

	Pic8u		*dw;	//The down sampling Y images for resize only
	Pic8u		*dm;	//The image after median filter.
	Pic8u		*dg;	//The gradients of down sampling images.
	Pic8u		*dc;	//The contour of down sampling images.
	Pic8u		*di;	//The map of directions
	Pic32u		*rg;	//For region counting
	//Pic16s	C[4];	// C[0] - Y component after first BDWT, C[1-3] color component after first BDWT
	//Level	*L[4];		//The levels of DWT transform
    Image 		*img;		//Pointer to images
	uint32 		qst;	//The number of quantization steps.

	//For white balancing
	uint32 		*hist;		//distribution probabilities array for white balancing
	uint16 		*look;		//Look up table for white balancing and gamma correction.

	// New segmentation interface
    //Line	 	*ln;		// The Line array
    //Line	 	**lp;		// The pointer to Line array
    Line_buff   *lbuf;      // The line buffer for segmentation
	Vertex		*vx;		// The vertex array
    Vertex		**vp;		// The pointer to Vertex array
	Vertex		**vpn;		// The pointer to neighbor Vertex array
	Vertex		**vpt;		// The pointer to temporary Vertex array
    uint32      *yx;        //Buffer for store first line pixel

	//Old interface
	//Pic8u rgb; // Scaled image for each color
    //Pic8u Y;	// The Y color componets
    //Pic8u grad;	// The gradient

	//For visualization only
    //Pic8u line;	//Draw lines
    //Pic8u edge;	//Draw edges
    //Pic8u vec;	//Draw motion vectors
	//uchar 	*mmb;	//The motion much buffer

    //Pixel	*pixs;		//The array of the pixeles
    //Edge	*edges;		//The array of edges
    //uint32 	nedge;		//The number of edges
    //uint32 	size;	//The number of pixels.
    //uint8 *buf;	//The pointer to temporary buffer in GOP structure
	uint32 	state;	//The state of frame
	// For test only

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

void 	frame_init			(GOP *g, uint32 fn, WaletConfig *wc);
void 	frame_input			(GOP *g, uint32 fn, WaletConfig *wc, uint8 *y, uint8 *u, uint8 *v);
void 	frame_ouput			(GOP *g, uint32 fn, WaletConfig *wc, uint8 *rgb, uint32 isteps, uint32 *w, uint32 *h);
uint32 	frame_transform		(GOP *g, uint32 fn, WaletConfig *wc);
uint32 	frame_idwt			(GOP *g, uint32 fn, WaletConfig *wc, uint32 isteps);
uint32 	frame_fill_subb		(GOP *g, uint32 fn, WaletConfig *wc);
uint32	frame_bits_alloc	(GOP *g, uint32 fn, WaletConfig *wc, uint32 times);
uint32 	frame_quantization	(GOP *g, uint32 fn, WaletConfig *wc);
uint32 	frame_range_encode	(GOP *g, uint32 fn, WaletConfig *wc, uint32 *size);
uint32 	frame_range_decode	(GOP *g, uint32 fn, WaletConfig *wc, uint32 *size);

uint32 	frame_write			(GOP *g, uint32 fn, WaletConfig *wc, FILE *wl);
uint32 	frame_read			(GOP *g, uint32 fn, WaletConfig *wc, FILE *wl);

uint32 	frame_median_filter	(GOP *g, uint32 fn, WaletConfig *wc);
uint32 	frame_fill_hist		(GOP *g, uint32 fn, WaletConfig *wc);
uint32 	frame_white_balance	(GOP *g, uint32 fn, WaletConfig *wc, uint32 bits, Gamma gamma);

uint32 	frame_segmetation	(GOP *g, uint32 fn, WaletConfig *wc);
uint32 	frame_match			(GOP *g, uint32 fn1, uint32 fn2, WaletConfig *wc);

void 	frame_compress		(GOP *g, uint32 fn, WaletConfig *wc, uint32 times);
void 	frame_decompress	(GOP *g, uint32 fn, WaletConfig *wc, uint32 isteps);

void 	frame_predict_subband(GOP *g, uint32 fn, WaletConfig *wc);
void 	frame_range			(GOP *g, uint32 fn, WaletConfig *wc,  uint32 *size);

void 	frame_test			(GOP *g, uint32 fn, WaletConfig *wc, uint32 times);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
