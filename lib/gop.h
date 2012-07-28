/** \file gop.h
    \brief The parameters for decoder and encoder.
*/
#ifndef _GOP_H_
#define _GOP_H_

/**
	\brief The coder  parameters structure.
 */
typedef struct {

	Frame			*frames;	//Pointer to frames array
	uint8			*buf;		//Temporal buffer for DWT, IDW, and range coder
	int				*ibuf;		//Temporal buffer for range coder quantization tables and for cumulative frequency;
    uint8           *cbuf;      //Compressed data buffer

	uint32			cur_gop_frame;	// The current gop frame
	uint64			cur_stream_frame;	// The current stream frame
	uint32 			state;	//The state of GOP


	//Subband			*sub[3];	//Subband location and size structure
	//int 			*q;			//Quantization value array
	//Segmentation
	//uint8		mvs;			// Motion vector search from -mvs to mvs in x and y
	//Row 			*row;		//The rows array
	//Corner 			*cor;		//The corners  array
	//Region			*region;	//The region array
	//Object			*obj;		//Object array
	//Row				**prow;		//The array of pointers to Rows
	//Region			**preg;		//The array of pointers to Regions

	//Pic8u			subs;		//Images Substruction
	//Pic8u			grad;		//Images gradient of substruction
	//Pic8u			con;		//Images contur of substruction

	//Edge			*edg;		//The array of edges
	//Pixel			*pix;		//The array of pixeles
	//Edge			**pedg;		//The array of pointers to Edge
	//Edgelet			*edgel;
}	GOP;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void 	walet_decoder_init	(GOP *gop, WaletConfig *wc);
void 	walet_encoder_init	(GOP *gop, WaletConfig *wc);

uint32	walet_write_stream	(GOP *gop, WaletConfig *wc, uint32 num, const char *filename);
uint32 	walet_read_stream	(GOP *gop, WaletConfig *wc, uint32 num, const char *filename);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //_GOP_HH_
