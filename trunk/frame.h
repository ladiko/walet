#ifndef _FRAME_H_
#define _FRAME_H_

typedef struct{
	Image img[3];
}	Frame;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void 	frames_init			(Frame *frame, Vector *sz, ColorSpace color, uint32 bits);
void 	frame_copy			(Frame *frame, ColorSpace color, uint32 bits, uchar *y, uchar *u, uchar *v);
void 	frame_dwt_53 		(Frame *frame, imgtype *buf, Subband **sub, ColorSpace color, uint32 steps);
void 	frame_idwt_53		(Frame *frame, imgtype *buf, Subband **sub, ColorSpace color, uint32 steps);
void 	frame_fill_prob		(Frame *frame, Subband **sub, uint32 bits, ColorSpace color, uint32 steps);
void 	frame_quantization	(Frame *frame, Subband **sub, uint32 bits, ColorSpace color, uint32 steps, int st);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
