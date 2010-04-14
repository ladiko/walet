#ifndef _FRAME_H_
#define _FRAME_H_


typedef struct{
	Image 	img[3];	//Pointer to image
	uint32 	size;	//The image size
	imgtype *buf;	//The pointer to temporary buffer
}	Frame;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void 	frames_init			(Frame *frame, Vector *sz, ColorSpace color, uint32 bits, uint32 steps, imgtype *buf);
void 	frame_copy			(Frame *frame, ColorSpace color, uint32 bits, uchar *y, uchar *u, uchar *v);
void 	frame_dwt_53 		(Frame *frame, Subband **sub, ColorSpace color, uint32 steps);
void 	frame_idwt_53		(Frame *frame, Subband **sub, ColorSpace color, uint32 steps, uint32 st);
void 	frame_fill_subb		(Frame *frame, Subband **sub, ColorSpace color, uint32 steps, uint32 bits);
void	frame_bits_alloc	(Frame *frame, Subband **sub, ColorSpace color, uint32 steps, uint32 bits, double per);
void 	frame_quantization	(Frame *frame, Subband **sub, ColorSpace color, uint32 steps, uint32 bits);
uint32 	frame_range_encode	(Frame *frame, Subband **sub, ColorSpace color, uint32 steps, uint32 bits);
uint32 	frame_range_decode	(Frame *frame, Subband **sub, ColorSpace color, uint32 steps, uint32 bits);
void 	frame_compress		(Frame *frame, Subband **sub, ColorSpace color, uint32 steps, uint32 bits, double per);
void 	frame_decompress	(Frame *frame, Subband **sub, ColorSpace color, uint32 steps, uint32 bits, uint32 st);
void 	frame_white_balance	(Frame *frame, ColorSpace color, uint32 bits, uint32 out_bits, BayerGrid bay, Gamma gamma);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
