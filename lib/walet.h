/** \file walet.h
    \brief The main include file of the walet codec
*/
#ifndef _WALET_H_
#define _WALET_H_

#define WALET_MAJOR_VERSION 0
#define WALET_MINOR_VERSION 5
#define WALET_MICRO_VERSION 1

#ifndef _WALET_TYPES_
#define _WALET_TYPES_
typedef char       			int8;
typedef unsigned char       uint8;
typedef short int 			int16;
typedef unsigned short int	uint16;
typedef unsigned int       	uint32;
typedef unsigned long long 	uint64;

#endif
/**
	\brief Compression type.
 */
typedef enum {
	LOSS,		///Loss compression.
	LOSSLESS,	///Lossless compression.
} Compression;

/**
	\brief The image format.
 */
typedef enum {
	CS420,	
  	CS444,
  	RGB,
  	RGBY,
  	GREY,
  	BAYER,
} ColorSpace;

/**
	\brief The Decorrelation  method.
 */
typedef enum {
	WAVELET,
	PREDICT,
  	RESIZE,
  	VECTORIZE,
} Decorrelation;

/**
	\brief The gamma correction method.
 */
typedef enum {
	LINEAR,
	BT709,
  	sRGB,
} Gamma;

/**
	\brief The frame type.
 */
typedef enum {
  	I_FRAME = 0,
  	P_FRAME = 1,	
  	B_FRAME = 2,	
} FrameType;

/**
	\brief The codec state.
 */
typedef enum {
	FRAME_COPY 		=	1,
	BUFFER_READ		=	1<<1,
  	DWT 			= 	1<<2,
  	IDWT 			= 	1<<3,
  	FILL_SUBBAND 	= 	1<<4,
  	QUANTIZATION	= 	1<<5,
  	RANGE_ENCODER	= 	1<<6,
  	RANGE_DECODER	= 	1<<7,
  	BITS_ALLOCATION	= 	1<<8,
  	MEDIAN_FILTER	=	1<<9,
  	COLOR_TRANSFORM = 	1<<10,
  	FILL_HISTOGRAM 	= 	1<<11,
  	WHITE_BALANCE	= 	1<<12,
  	SEGMENTATION	= 	1<<13,
  	MATCH			= 	1<<14,
}CodecState;

/**
	\brief The bayer grid pattern.

	All RGB cameras use one of these Bayer grids:\n
*/
/*
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
} BayerGrid;
/*
typedef enum {		//Wavelet transform type.
	CLASSIC	= 0,
	MALLET 	= 1,
	COLOR	= 2,
}	TransformType;
*/
/**
	\brief Wavelet transform filter banks.
 */
typedef enum {
	FR_HAAR	= 0,
	FR_5_3	= 1,
	FR_9_7	= 2,
} FilterBank;

/**
	\brief The range coder type
 */
typedef enum {
	ADAP	= 0,	///Adaptive range coder.
	NADAP	= 1,	///Nonadaptive range coder.
	FAST	= 2,	///Nonadaptive fast range coder.
} RangeType;

/*
typedef enum {		//The type of image
	U8	= 0,	// Unsigned 8 bits
	S8	= 1,	// Signed 8 bits
	U16	= 2,	// Unsigned 16 bits
	S16	= 3,	// Signed 16 bits
}	ImgType;
*/

typedef struct {
	uint32 x,y;
} Dim;

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

/**
	\brief The header of walet file
 */
typedef struct {
	//uint16		marker;			/// The walet codec marker.
	uint16 		w;				/// Image width
	uint16 		h;				/// Image width
	uint8		icol;			/// Input color space
	uint8		ccol;			/// Compresion color space
	//uint8		ocol;			/// Output color space
	uint8		dec;			/// Decorrelation method
	//uint8		color;			/// Color space
	uint8		bg;				/// Bayer grid pattern
	uint8		bpp;			/// Image bits per pixel.
	uint8 		steps;  		/// DWT steps.
	uint8		gop_size;		/// GOP size
	uint8		rates;			/// Frame rates
	uint8		comp;			/// Compression in times to original image if 1 - lossless 0 - without any compression.
	uint8		fb;				/// Filters bank for wavelet transform.
	uint8		rt;				/// Range coder type
	uint8		mv;				/// The motion vector search in pixeles.
} WaletConfig;

typedef struct Row Row;
typedef struct Region Region;
typedef struct Object Object;
typedef struct Corner Corner;
typedef struct Edge Edge;
typedef struct Edgelet Edgelet;
typedef struct Pixel Pixel;
typedef struct Reg Reg;
typedef struct Vector Vector;


//New segmentation interface -------------------------
typedef struct Vertex Vertex;
typedef struct Line Line;

struct Line {
	Vertex 	*vx[2];		//The pointer to vertex
	uint8 	pow;		//The line power
	//uint8 	l[3];	//Left color 0 - red, 1 - gren, 2 - blue
	//uint8 	r[3];	//Right color 0 - red, 1 - gren, 2 - blue
};

struct Reg {
	uint8 	c[3];	//Left color 0 - red, 1 - gren, 2 - blue
	uint16 	n;		//Number of lines
	Line	**lp;	//The pointer to the lines array
};

///Directions around the pixel
/// |1|2|3| uint8 01234567
/// |0|x|4|
/// |7|6|5|

struct Vertex {
	uint16 		x,y;	//X and Y coordinate
	uint8		n;		//The number of neighborhood vertex
	uint8		di;		//The direction round pixel to the neighborhood
	uint8		cn;		//The counter of the directions
	//Line		**lp;	//The pointer to the lines array
	Vertex		**vp;	//The pointer to Virtex array
	uint32		reg;	//The pointer region numbers
};

struct Vector {
	 uint16		x1, y1;		//X coordinate
	 uint16		x2, y2;		//X coordinate
};
//----------------------------------------------------------------------


struct Pixel {
	 uint16		x;		//X coordinate
	 uint16		y;		//Y coordinate
	 uint16		mach;
	 uint16		npix;	//Number of pixels this pixel to out pixel
	 uint16 	yx;		//The neighborhood pixel
	 //uint32 	yx2;

	 char		vx;		//The motion vector
	 char		vy;		//The motion vector
	 uint8		nnei;	//Number of neighborhood
	 Pixel 		*nei[5];	//Pointer to neighborhood
	 Pixel		*cp;	//Pointer to the center of cluster NULL if centre
	 uint8 		dir;	// Direction for neighborhood
						// 1 2 3
						// 0   4
						// 7 6 5
	 uint8		nout;	//If 0 - end pint if 1 - vector
	 uint8		nin;	//Number of neighborhood
	 Pixel 		*out;
	 uint8		pow;	//The gradient energy
};

struct Edge {
	uint32 	yxs;	//The start point
	uint32 	yxe;	//The end point
	uint16	pixs;	//The number of pixels
	uint16	lines;	//The number of lines
};

struct Edgelet {
	uint32 	yx;		//The start point
	uint8 	len;	//The length of edgelet
	uint32	pow;	//The power of edgelet
	uint32	dir;	//The direction 00 - left; 01 - top; 10 - right; 11 - bottom; max 16 dot per Edgelet
};

struct Object{
	uint8 		c[4];		//The colors
	uint32		ac[4];		//The average colors
	uint32 		nregs;		//Number of regions in the object
	uint32 		regc;		//Number of regions in the object
	uint32		npixs;		//The numbers of pixels in the object
	Region		**reg;		//Pointer to the regions included in object
};

struct Region{
	//uint16 		x;			//The top left point
	//uint16 		y;			//The top left point
	uint8 		c[3];		//The colors
	uint32		ac[4];		//The average colors
	uint32 		nrows;		//The numbers of rows in the region
	uint32		npixs;		//The numbers of pixels in the region
	uint32		rowc;		//The rows counter
	uint32		nneis;		//The numbers of neighborhood regions
	uint32		neic;		//The counter of neighborhood regions
	Row			**row;		//The pointer to the rows array
	Region		**reg;		//The pointer to the  neighborhood regions array
	Corner		**con;		//The pointer to the  coners array
	Object		*obj;		//the pointer to the object
	//uint32		diff;
	//Chain		**chain;	//The pointer to array of chains around region
};

struct Row {
	uint32 		yx;			//The start point
	//uint16		x;			//The start row on axis X
	//uint32 		y;			//The start point
	uint8 		c[3];		//The colors
	//uint32		ac[4];		//The average colors
	uint16 		length;		//The length of row
	Region		*reg;		//Pointer to the region
	Row			**rown;		//The pointer to the  neighborhood row array
	uint32 		nrown;		//Number of neighborhood rows
	uint32 		rownc;		//Counter of neighborhood rows
};

struct Corner {
	uint32 		yx;			//The start point
	uint8 		c[4];		//The colors
	uint32 		diff;		//The different from neighborhood pixeles
	Region		*reg;		//Pointer to the region
};


typedef struct { 	// Unsigned picture 8bit
	uint16 w;
	uint16 h;
	uint8 *pic;
} Pic8u;

typedef struct {	// Signed picture 8bit
	uint16 w;
	uint16 h;
	char *pic;
} Pic8s;

typedef struct {	// Signed picture 16bit
	uint16 w;
	uint16 h;
	short int *pic;
} Pic16s;

typedef struct {	// Unsigned picture 16bit
	uint16 w;
	uint16 h;
	short unsigned int *pic;
} Pic16u;

typedef struct {	// Unsigned picture 16bit
	uint16 w;
	uint16 h;
	unsigned int *pic;
} Pic32u;

typedef struct {	//One DWT decomposition level
	Pic8s s[4];		//Subband array s[0] - LL, s[1] - HL, s[2] - LH, s[3] - HH
} Level8;


//	|---------|            |---------|    |---------|
//	|         |	2d wavelet |  Y | C1 |    | LL | HL |
//	|  Bayer  |	transform  |---------|    |---------|
//	|  bay8u  |	---------> | C2 | C3 |    | LH | HH |
//	|---------|            |---------|    |---------|

typedef struct{
	Pic8u	B;			//Bayer image 8 bits
	Pic8s	C[4];		// C[0] - Y component after first BDWT, C[1-3] color component after first BDWT
}BAY8;

typedef struct{
	Pic16s	B;			//Bayer image 16 bits
	Pic16s	C[4];		// C[0] - Y component after first BDWT, C[1-3] color component after first BDWT
}BAY16;

typedef struct{
	Pic8u	Y;			//Y image for 420 422 444
	Pic8u	U;			//U image for 420 422 444
	Pic8u	V;			//V image for 420 422 444
}YUV8;

typedef struct{
	int	x;
	int	y;
	int	z;
} p3d;


#define rnd(x)	((x) < 0 ? 0 : ( (x) > 255 ? 255 : (x) ))
#define ruc(x)	((x) < 0 ? 0 : ( (x) > 255 ? 255 : (x) ))
#define rc(x)	((x) < -128 ? -128 : ( (x) > 127 ? 127 : (x) ))
#define bit_check(w, i) (((w)&1)&(!((i)&1)))	//Check odd width or height of subband


//#define oe(a,x)	(a ? x%2 : (x+1)%2)


#include <subband.h>
#include <image.h>
#include <frame.h>
#include <gop.h>
#include <utils.h>
#include <rangecoder.h>
#include <filters.h>
#include <motion.h>
#include <segmentation.h>

#endif //_WALET_HH_
