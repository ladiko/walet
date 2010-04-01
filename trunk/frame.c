#include <walet.h>

#include <stdio.h>
#include <string.h>

void frames_init(Frame *frame, Vector *sz, ColorSpace color, uint32 bits)
{
	image_init(&frame->img[0], sz->x, sz->y, bits);
	if(color == CS444 || color == RGB) {
		image_init(&frame->img[1], sz->x, sz->y, bits);
		image_init(&frame->img[2], sz->x, sz->y, bits);
	}
	if(color == CS422){
		image_init(&frame->img[1], sz->x>>1 , sz->y, bits);
		image_init(&frame->img[2], sz->x>>1 , sz->y, bits);
	}
	if(color == CS420){
		image_init(&frame->img[1], sz->x>>1 , sz->y>>1, bits);
		image_init(&frame->img[2], sz->x>>1 , sz->y>>1, bits);
	}
}

void frame_copy(Frame *frame, ColorSpace color, uint32 bits, uchar *y, uchar *u, uchar *v)
{
	printf("Start frame copy \n");
	image_copy(&frame->img[0], bits, y);
	if(color != GREY  && color != BAYER) {
		image_copy(&frame->img[1], bits, u);
		image_copy(&frame->img[2], bits, v);
	}
}

void frame_dwt_53(Frame *frame, imgtype *buf, Subband **sub, ColorSpace color, uint32 steps)
{
	image_dwt_53(&frame->img[0], buf, sub, color, steps);
	if(color != GREY  && color != BAYER) {
		image_dwt_53(&frame->img[1], buf, sub, color, steps);
		image_dwt_53(&frame->img[2], buf, sub, color, steps);
	}
}

void frame_idwt_53(Frame *frame, imgtype *buf, Subband **sub, ColorSpace color, uint32 steps)
{
	image_idwt_53(&frame->img[0], buf, sub, color, steps);
	if(color != GREY  && color != BAYER) {
		image_idwt_53(&frame->img[1], buf, sub, color, steps);
		image_idwt_53(&frame->img[2], buf, sub, color, steps);
	}
}

void frame_fill_subb(Frame *frame, Subband **sub, uint32 bits, ColorSpace color, uint32 steps)
{
	image_fill_subb(&frame->img[0], sub, bits, color, steps);
	if(color != GREY  && color != BAYER) {
		image_fill_subb(&frame->img[1], sub, bits, color, steps);
		image_fill_subb(&frame->img[2], sub, bits, color, steps);
	}
}

void frame_fill_hist(Frame *frame, ColorSpace color, uint32 bits, BayerGrid bay)
{
	image_fill_hist(&frame->img[0], bits, color, bay);
	if(color != GREY  && color != BAYER) {
		image_fill_hist(&frame->img[1], bits, color, bay);
		image_fill_hist(&frame->img[2], bits, color, bay);
	}
}

void frame_quantization(Frame *frame, Subband **sub, uint32 bits, ColorSpace color, uint32 steps, int st)
{
	image_quantization(&frame->img[0], sub, bits, color, steps, st);
	if(color != GREY  && color != BAYER) {
		image_quantization(&frame->img[1], sub, bits, color, steps, st);
		image_quantization(&frame->img[2], sub, bits, color, steps, st);
	}
}
