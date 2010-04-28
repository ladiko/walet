#include <walet.h>

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

void frames_init(GOP *gop, uint32 fr)
///	\fn	void frames_init(GOP *gop, uint32 fr)
///	\brief	Frame initialization
///	\param	gop			The pointer to the GOP structure.
///	\param	fr			The frame number.
{
	Frame *frame = &gop->frames[fr];
	uint32 x = gop->sd->size.x, y = gop->sd->size.y;

	image_init(&frame->img[0], gop->sd, x, y);
	frame->size = x*y;
	if(gop->sd->color == CS444 || gop->sd->color == RGB) {
		image_init(&frame->img[1], gop->sd, x, y);
		image_init(&frame->img[2], gop->sd, x, y);
		frame->size = frame->size*3;
	}
	if(gop->sd->color == CS422){
		image_init(&frame->img[1], gop->sd, x>>1 , y);
		image_init(&frame->img[2], gop->sd, x>>1 , y);
		frame->size = frame->size*2;
	}
	if(gop->sd->color == CS420){
		image_init(&frame->img[1], gop->sd, x>>1 , y>>1);
		image_init(&frame->img[2], gop->sd, x>>1 , y>>1);
		frame->size = (frame->size*3)>>1;
	}
}

void frame_copy(GOP *gop, uint32 fr, uchar *y, uchar *u, uchar *v)
///	\fn	void frame_copy(GOP *gop, uint32 fr, uchar *y, uchar *u, uchar *v)
///	\brief	Write images to frame
///	\param	gop			The pointer to the GOP structure.
///	\param	fr			The frame number.
///	\param	y			The pointer to Bayer, gray, red or Y  image data
///	\param	u			The pointer to green or U  image data
///	\param	v			The pointer to blue or V  image data
{
	Frame *frame = &gop->frames[fr];

	printf("Start frame copy \n");
	image_copy(&frame->img[0], gop->sd, y);
	if(gop->sd->color != GREY  && gop->sd->color != BAYER) {
		image_copy(&frame->img[1], gop->sd, u);
		image_copy(&frame->img[2], gop->sd, v);
	}
}

void frame_dwt_53(GOP *gop, uint32 fr)
///	\fn	void frame_dwt_53(GOP *gop, uint32 fr)
///	\brief	Discrete wavelets frame transform.
///	\param	gop			The pointer to the GOP structure.
///	\param	fr			The frame number.
{
	Frame *frame = &gop->frames[fr];

	frame->img[0].sub = gop->sub[0]; //Set current image for DWT transform
	image_dwt_53(&frame->img[0], gop->sd, gop->buf);
	if(gop->sd->color != GREY  && gop->sd->color != BAYER) {
		frame->img[1].sub = gop->sub[1]; //Set current image for DWT transform
		frame->img[2].sub = gop->sub[2]; //Set current image for DWT transform
		image_dwt_53(&frame->img[1], gop->sd, gop->buf);
		image_dwt_53(&frame->img[2], gop->sd, gop->buf);
	}
}

void frame_idwt_53(GOP *gop, uint32 fr, uint32 steps)
///	\fn	void frame_idwt_53(GOP *gop, uint32 fr, uint32 step)
///	\brief	Invert discrete wavelets frame transform.
///	\param	gop			The pointer to the GOP structure.
///	\param	fr			The frame number.
/// \param	steps		The steps of IDWT should be lees or equal DWT steps
{
	Frame *frame = &gop->frames[fr];

	frame->img[0].sub = gop->sub[0]; //Set current image for IDWT transform
	image_idwt_53(&frame->img[0], gop->sd, gop->buf, steps);
	if(gop->sd->color != GREY  && gop->sd->color != BAYER) {
		frame->img[1].sub = gop->sub[1]; //Set current image for IDWT transform
		frame->img[2].sub = gop->sub[2]; //Set current image for IDWT transform
		image_idwt_53(&frame->img[1], gop->sd, gop->buf, steps);
		image_idwt_53(&frame->img[2], gop->sd, gop->buf, steps);
	}
}

void frame_fill_subb(GOP *gop, uint32 fr)
///	\fn	void frame_fill_subb(GOP *gop, uint32 fr)
///	\brief	Fill distribution probability array.
///	\param	gop			The pointer to the GOP structure.
///	\param	fr			The frame number.
{

	image_fill_subb(&gop->frames[fr].img[0], gop->sd);
	if(gop->sd->color != GREY  && gop->sd->color != BAYER) {
		image_fill_subb(&gop->frames[fr].img[1], gop->sd);
		image_fill_subb(&gop->frames[fr].img[2], gop->sd);
	}
}

void frame_white_balance(GOP *gop, uint32 fr,  uint32 out_bits, Gamma gamma)
///	\fn	void frame_white_balance(GOP *gop, uint32 fr,  uint32 out_bits, Gamma gamma)
///	\brief	Make white balance and gamma correction of the frame (for bayer frames only).
///	\param	gop			The pointer to the GOP structure.
///	\param	fr			The frame number.
///	\param	out_bits	The bits per pixel for output frame.
///	\param	gamma		The type of gamma correction.
{
	Image *im = &gop->frames[fr].img[0];
	if(gop->sd->color == BAYER){
		image_fill_bayer_hist(im, gop->sd);
		utils_white_balance(im->img, im->img, im->hist, im->look, im->size.y, im->size.x, gop->sd->bg, gop->sd->bits, out_bits, gamma);
	}
}

void frame_bits_alloc(GOP *gop, uint32 fr, uint32 times)
///	\fn	void frame_bits_alloc(GOP *gop, uint32 fr, uint32 times)
///	\brief	Bits allocation for frame.
///	\param	gop			The pointer to the GOP structure.
///	\param	fr			The frame number.
///	\param	times		Compression in times in relation to the original image.
{
	image_bits_alloc(&gop->frames[fr].img[0], gop->sd, times);
	if(gop->sd->color != GREY  && gop->sd->color != BAYER) {
		image_bits_alloc(&gop->frames[fr].img[1], gop->sd, times);
		image_bits_alloc(&gop->frames[fr].img[2], gop->sd, times);
	}
}

void frame_quantization(GOP *gop, uint32 fr)
///	\fn	void frame_quantization(GOP *gop, uint32 fr)
///	\brief	Frame quantization.
///	\param	gop			The pointer to the GOP structure.
///	\param	fr			The frame number.
{
	image_quantization(&gop->frames[fr].img[0], gop->sd);
	if(gop->sd->color != GREY  && gop->sd->color != BAYER) {
		image_quantization(&gop->frames[fr].img[1], gop->sd);
		image_quantization(&gop->frames[fr].img[2], gop->sd);
	}
}

uint32 frame_range_encode(GOP *gop, uint32 fr)
{
	uint32 size = 0;
	size += image_range_encode(&gop->frames[fr].img[0], gop->sd, (uchar*)gop->buf);
	if(gop->sd->color != GREY  && gop->sd->color != BAYER) {
		size += image_range_encode(&gop->frames[fr].img[1], gop->sd, (uchar*)gop->buf);
		size += image_range_encode(&gop->frames[fr].img[2], gop->sd, (uchar*)gop->buf);
	}
	return size;
}

uint32 frame_range_decode(GOP *gop, uint32 fr)
{
	uint32 size = 0;
	size += image_range_decode(&gop->frames[fr].img[0], gop->sd, (uchar*)gop->buf);
	if(gop->sd->color != GREY  && gop->sd->color != BAYER) {
		size += image_range_decode(&gop->frames[fr].img[1], gop->sd, (uchar*)gop->buf);
		size += image_range_decode(&gop->frames[fr].img[2], gop->sd, (uchar*)gop->buf);
	}
	return size;
}

void frame_compress(GOP *gop, uint32 fr, uint32 times)
{
	Frame *frame = &gop->frames[fr];
	uint32 bits = gop->sd->bits, color =  gop->sd->color, steps = gop->sd->steps;
	Subband **sub;
	clock_t start, end;
	struct timeval tv;

	gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
	frame_dwt_53		(gop, fr);
	gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
	printf("DWT time             = %f\n", (double)(end-start)/1000000.);

	gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
	frame_fill_subb		(gop, fr);
	gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
	printf("Fill subband time    = %f\n", (double)(end-start)/1000000.);

	gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
	frame_bits_alloc	(gop, fr, times);
	gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
	printf("Bits allocation time = %f\n", (double)(end-start)/1000000.);

	gettimeofday(&tv, NULL); start = tv.tv_usec + tv.tv_sec*1000000;
	frame_range_encode	(gop, fr);
	gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
	printf("Range coder time     = %f\n", (double)(end-start)/1000000.);
	//printf("Frame size  = %d\n", size);
	//frame_write
}

void frame_decompress(Frame *frame, Subband **sub,  ColorSpace color, uint32 steps, uint32 bits, uint32 st)
{
	//frame_read
	//frame_range_decode	(frame, sub, color, steps, bits);
	//frame_idwt_53		(frame, sub, color, steps, st);
}
