#include <walet.h>

#include <stdio.h>
#include <string.h>

void frames_init(Frame *frame, Vector *sz, ColorSpace color, uint32 bits, uint32 steps, imgtype *buf)
{
	frame->buf = buf;
	image_init(&frame->img[0], sz->x, sz->y, bits, color, steps);
	frame->size = sz->x*sz->y;
	if(color == CS444 || color == RGB) {
		image_init(&frame->img[1], sz->x, sz->y, bits, color, steps);
		image_init(&frame->img[2], sz->x, sz->y, bits, color, steps);
		frame->size = frame->size*3;
	}
	if(color == CS422){
		image_init(&frame->img[1], sz->x>>1 , sz->y, bits, color, steps);
		image_init(&frame->img[2], sz->x>>1 , sz->y, bits, color, steps);
		frame->size = frame->size*2;
	}
	if(color == CS420){
		image_init(&frame->img[1], sz->x>>1 , sz->y>>1, bits, color, steps);
		image_init(&frame->img[2], sz->x>>1 , sz->y>>1, bits, color, steps);
		frame->size = (frame->size*3)>>1;
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

void frame_dwt_53(Frame *frame, Subband **sub, ColorSpace color, uint32 steps)
///	\fn	void frame_dwt_53(Frame *frame, Subband **sub, ColorSpace color, uint32 steps)
///	\brief	Discrete wavelets frame transform.
///	\param	frame		The pointer to the frame.
///	\param	sub			The pointer to subband array.
///	\param	color		Color space.
///	\param	steps		DWT steps.
{
	image_dwt_53(&frame->img[0], frame->buf, sub, color, steps);
	if(color != GREY  && color != BAYER) {
		image_dwt_53(&frame->img[1], frame->buf, sub, color, steps);
		image_dwt_53(&frame->img[2], frame->buf, sub, color, steps);
	}
}

void frame_idwt_53(Frame *frame, Subband **sub, ColorSpace color, uint32 steps, uint32 st)
///	\fn	void frame_idwt_53(Frame *frame, Subband **sub, ColorSpace color, uint32 steps, uint32 st)
///	\brief	Invert discrete wavelets frame transform.
///	\param	frame		The pointer to the frame.
///	\param	sub			The pointer to subband array.
///	\param	color		Color space.
///	\param 	steps		DWT steps.
///	\param	st			IDWT steps.
{
	image_idwt_53(&frame->img[0], frame->buf, sub, color, steps, st);
	if(color != GREY  && color != BAYER) {
		image_idwt_53(&frame->img[1], frame->buf, sub, color, steps, st);
		image_idwt_53(&frame->img[2], frame->buf, sub, color, steps, st);
	}
}

void frame_fill_subb(Frame *frame, Subband **sub, ColorSpace color, uint32 steps, uint32 bits)
///	\fn	frame_fill_subb(Frame *frame, Subband **sub, uint32 bits, ColorSpace color, uint32 steps)
///	\brief	Fill distribution probability array for each subband in the frame.
///	\param	frame		The pointer to the frame.
///	\param	sub			the pointer to subband array.
///	\param	color		Color space.
///	\param	steps		DWT steps.
///	\param	bits		The bits per pixel input frame.
{
	image_fill_subb(&frame->img[0], sub, bits, color, steps);
	if(color != GREY  && color != BAYER) {
		image_fill_subb(&frame->img[1], sub, bits, color, steps);
		image_fill_subb(&frame->img[2], sub, bits, color, steps);
	}
}

void frame_white_balance(Frame *frame, ColorSpace color, uint32 bits, uint32 out_bits, BayerGrid bay, Gamma gamma)
///	\fn	void frame_white_balance(Frame *frame, ColorSpace color, uint32 bits, uint32 out_bits, BayerGrid bay, Gamma gamma)
///	\brief	Make white balance and gamma correction of the frame.
///	\param	frame		The pointer to the frame.
///	\param	color		Color space.
///	\param	bits		The bits per pixel input frame.
///	\param	out_bits	The bits per pixel output frame.
///	\param	bay			The bayer grid pattern.
///	\param	gamma		The type of gamma correction.
{
	Image *im = &frame->img[0];
	if(color == BAYER){
		image_fill_hist(im, bits, color, bay);
		utils_white_balance(im->img, im->img, im->hist, im->look, im->size.y, im->size.x, bay, bits, out_bits, gamma);
	}
}

void frame_bits_alloc(Frame *frame, Subband **sub, ColorSpace color, uint32 steps, uint32 bits, double per)
{
	image_bits_alloc(&frame->img[0], sub, bits, color, steps, per);
	if(color != GREY  && color != BAYER) {
		image_bits_alloc(&frame->img[1], sub, bits, color, steps, per);
		image_bits_alloc(&frame->img[2], sub, bits, color, steps, per);
	}
}

void frame_quantization(Frame *frame, Subband **sub, ColorSpace color, uint32 steps, uint32 bits)
{
	image_quantization(&frame->img[0], sub, bits, color, steps);
	if(color != GREY  && color != BAYER) {
		image_quantization(&frame->img[1], sub, bits, color, steps);
		image_quantization(&frame->img[2], sub, bits, color, steps);
	}
}

uint32 frame_range_encode(Frame *frame, Subband **sub, ColorSpace color, uint32 steps, uint32 bits)
{
	uint32 size = 0;
	size += image_range_encode(&frame->img[0], sub, bits, color, steps, (uchar*)frame->buf);
	if(color != GREY  && color != BAYER) {
		size += image_range_encode(&frame->img[1], sub, bits, color, steps, (uchar*)&frame->buf[size]);
		size += image_range_encode(&frame->img[2], sub, bits, color, steps, (uchar*)&frame->buf[size]);
	}
	return size;
}

uint32 frame_range_decode(Frame *frame, Subband **sub, ColorSpace color, uint32 steps, uint32 bits)
{
	uint32 size = 0;
	size += image_range_decode(&frame->img[0], sub, bits, color, steps, (uchar*)frame->buf);
	if(color != GREY  && color != BAYER) {
		size += image_range_decode(&frame->img[1], sub, bits, color, steps, (uchar*)&frame->buf[size]);
		size += image_range_decode(&frame->img[2], sub, bits, color, steps, (uchar*)&frame->buf[size]);
	}
	return size;
}

void frame_compress(Frame *frame, Subband **sub,  ColorSpace color, uint32 steps, uint32 bits, double per)
{
	frame_dwt_53		(frame, sub, color, steps);
	frame_fill_subb		(frame, sub, color, steps, bits);
	frame_bits_alloc	(frame, sub, color, steps, bits, per);
	frame_quantization	(frame, sub, color, steps, bits);
	frame_range_encode	(frame, sub, color, steps, bits);
	//frame_write
}

void frame_decompress(Frame *frame, Subband **sub,  ColorSpace color, uint32 steps, uint32 bits, uint32 st)
{
	//frame_read
	frame_range_decode	(frame, sub, color, steps, bits);
	frame_idwt_53		(frame, sub, color, steps, st);
}
