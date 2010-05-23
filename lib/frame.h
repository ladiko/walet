#ifndef _FRAME_H_
#define _FRAME_H_


typedef struct{
	Image 	img[3];	//Pointer to image
	uint32 	size;	//The image size
	imgtype *buf;	//The pointer to temporary buffer in GOP structure
}	Frame;

#include <gop.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void 	frames_init			(GOP *gop, uint32 fr);
void 	frame_copy			(GOP *gop, uint32 fr, uchar *y, uchar *u, uchar *v);
void 	frame_dwt_53		(GOP *gop, uint32 fr);
void 	frame_idwt_53		(GOP *gop, uint32 fr, uint32 step);
void 	frame_fill_subb		(GOP *gop, uint32 fr);
void	frame_bits_alloc	(GOP *gop, uint32 fr, uint32 times);
void 	frame_quantization	(GOP *gop, uint32 fr);
uint32 	frame_range_encode	(GOP *gop, uint32 fr);
uint32 	frame_range_decode	(GOP *gop, uint32 fr);
uint32 	frame_write			(GOP *gop, uint32 fr, const char *filename);
void 	frame_compress		(GOP *gop, uint32 fr, uint32 times);
void 	frame_decompress	(Frame *frame, Subband **sub, ColorSpace color, uint32 steps, uint32 bits, uint32 st);

void 	frame_white_balance	(GOP *gop, uint32 fr,  uint32 out_bits, Gamma gamma);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif