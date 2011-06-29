#ifndef _GOP_H_
#define _GOP_H_

typedef struct {
	//WaletHeader 	wh;
	//Paremeters for decoder and encoder
	uint16 			width;			// Image width
	uint16 			height;			// Image width
	ColorSpace		color;			// Color space
	BayerGrid		bg;				// Bayer grid pattern
	uint32			bpp;			// Bits per pixel
	uint32 			steps;  		// Steps of DWT
	uint32			gop_size;		// GOP size
	uint32			rates;			// Frame rates
	uint32			comp;			// Compression in times to original image if 1 - lossless
	FilterBank		fb;				// Filters bank for wavelet transform
	//Paremeters for encoder
	uint8		mvs;			// Motion vector search from -mvs to mvs in x and y

	uint32			cur_gop_frame;	// The current gop frame
	uint64			cur_stream_frame;	// The current stream frame

	uint8			*buf;		//Temporal buffer for DWT, IDW, and range coder
	uint8 			*mmb;	//The motion much buffer
	//uint32 			*row, *reg, *col;		//Buffer for image segmentation
	Frame			*frames;	//Pointer to frames array
	Subband			*sub[3];	//Subband location and size structure
	int 			*q;			//Quantization value array
	//Segmentation
	Row 			*row;		//The rows array
	Corner 			*cor;		//The corners  array
	Region			*region;	//The region array
	Object			*obj;		//Object array
	Row				**prow;		//The array of pointers to Rows
	Region			**preg;		//The array of pointers to Regions

	Pic8u			subs;		//Images Substruction
	Pic8u			grad;		//Images gradient of substruction
	Pic8u			con;		//Images contur of substruction

	Edge			*edg;		//The array of edges
	//Pixel			*pix;		//The array of pixeles
	Edge			**pedg;		//The array of pointers to Edge
	Edgelet			*edgel;
}	GOP;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

GOP* 	walet_decoder_init	(uint32 width, uint32 height, ColorSpace color, BayerGrid bg, uint32 bpp, uint32 steps, uint32 gop_size, uint32 rates, uint32 comp, FilterBank fb);
GOP* 	walet_encoder_init	(uint32 width, uint32 height, ColorSpace color, BayerGrid bg, uint32 bpp, uint32 steps, uint32 gop_size, uint32 rates, uint32 comp, FilterBank fb, uint8 mvs);

uint32 	walet_write_stream	(GOP *gop, uint32 num, const char *filename);
uint32 	walet_read_stream	(GOP **gop, uint32 num, const char *filename);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //_GOP_HH_
