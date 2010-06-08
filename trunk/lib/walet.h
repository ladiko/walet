// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

//           image.cc
//  Mon Feb 27 16:32:52 2006
//  Copyright  2006  vadim
//  <vadim@vadim.sigrand.lcl>


#ifndef _WALET_H_
#define _WALET_H_

#define WALET_MAJOR_VERSION 0
#define WALET_MINOR_VERSION 5
#define WALET_MICRO_VERSION 1

#ifndef _WALET_TYPES_
#define _WALET_TYPES_
typedef unsigned char       uchar;
typedef char       			int8;
typedef unsigned int       	uint32;
typedef unsigned short int 	uint16;
typedef unsigned long long 	uint64;
typedef short int           int16;
typedef short int			imgtype;
#endif

typedef enum {
	LESS,
	LOSSLESS,
} Compression;

typedef enum {
	CS420,	
  	CS422,	
  	CS444,
  	RGB,
  	GREY,
  	BAYER,
} ColorSpace;

typedef enum {
	LINEAR,
	BT709,
  	sRGB,
} Gamma;

typedef enum {
  	MS1_1 = 0,	
  	MS1_2 = 1,	
  	MS1_4 = 2,
  	MS1_8 = 3,	
}	MotionSubsamp;

typedef enum {
  	I_FRAME = 0,	
  	P_FRAME = 1,	
  	B_FRAME = 2,	
} FrameType;

typedef enum {
	FRAME_COPY		=	1,
	BUFFER_READ		=	1<<1,
  	DWT 			= 	1<<2,
  	IDWT 			= 	1<<3,
  	FILL_SUBBAND 	= 	1<<4,
  	QUANTIZATION	= 	1<<5,
  	RANGE_ENCODER	= 	1<<6,
  	RANGE_DECODER	= 	1<<7,
  	BITS_ALLOCATION	= 	1<<8,
  	MEDIAN_FILTER	=	1<<9,
}	CodecState;

/*
   All RGB cameras use one of these Bayer grids:

	BGGR  0         GRBG 1          GBRG  2         RGGB 3
	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5
	0 B G B G B G	0 G R G R G R	0 G B G B G B	0 R G R G R G
	1 G R G R G R	1 B G B G B G	1 R G R G R G	1 G B G B G B
	2 B G B G B G	2 G R G R G R	2 G B G B G B	2 R G R G R G
	3 G R G R G R	3 B G B G B G	3 R G R G R G	3 G B G B G B
 */
typedef enum {
	BGGR = 0,
	GRBG = 1,
	GBRG = 2,
	RGGB = 3,
}	BayerGrid;

typedef enum {		//Wavelet transform type
	CLASSIC	= 0,
	MALLET 	= 1,
}	TransformType;

typedef enum {		//Wavelet transform filter banks
	FR_HAAR	= 0,
	FR_5_3	= 1,
	FR_9_7	= 2,
}	FilterBank;

typedef struct {
	int x,y;
}	Vector;
//File header
// -----------------------------------------------------------------------------------------
// |offset | size | description
// ------------------------------------------------------------------------------------------
// |  0    |  2   | Walet marker 0x776C
// ------------------------------------------------------------------------------------------
// |  2    |  2   | Image width in pixels
// -------------------------------------------------------------------------------------------
// |  4    |  2   | Image height in pixels
// -------------------------------------------------------------------------------------------
// |  6    |  1   | Color space 0 - CS420, 1 - CS422, 2 - CS444, 3 - RGB, 4 - GREY, 5 - BAYER
// --------------------------------------------------------------------------------------------
// |  7    |  1   | Bayer grid pattern 0 - BGGR, 1 - GRBG, 2 - GBRG, 3 - RGGB
// --------------------------------------------------------------------------------------------
// |  8    |  1   | Bits per pixel
// --------------------------------------------------------------------------------------------
// |  9    |  1   | Steps of DWT
// --------------------------------------------------------------------------------------------
// |  10   |  1   | GOP size
// --------------------------------------------------------------------------------------------
// |  11   |  1   | Frame rates

typedef struct {
	uint16		marker;			// The walet codec marker
	uint16 		width;			// Image width
	uint16 		height;			// Image width
	uchar		color;			// Color space
	uchar		bg;				// Bayer grid pattern
	uchar		bpp;			// Bits per pixel
	uchar 		steps;  		// Steps of DWT
	uchar		gop_size;		// GOP size
	uchar		rates;			// Frame rates
	uchar		comp;			// Compression in times to original image if 1 - lossless
	uchar		fb;				// Filters for wavelet transform
} WaletHeader;




#define rnd(x)	((x) < 0 ? 0 : ( (x) > 255 ? 255 : (x) ))
#define ruc(x)	((x) < 0 ? 0 : ( (x) > 255 ? 255 : (x) ))
#define rc(x)	((x) < -127 ? -127 : ( (x) > 127 ? 127 : (x) ))
//#define oe(a,x)	(a ? x%2 : (x+1)%2)


#include <subband.h>
#include <image.h>
#include <frame.h>
#include <gop.h>
#include <utils.h>
#include <rangecoder.h>
#include <filters.h>

#endif //_WALET_HH_
