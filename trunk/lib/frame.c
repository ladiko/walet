#include <walet.h>

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

void frames_init(GOP *gop, uint32 fr)
///	\fn	void frames_init(GOP *gop, uint32 fr)
///	\brief	Frame initialization
///	\param	gop			The GOP structure.
///	\param	fr			The frame number.
{
	Frame *frame = &gop->frames[fr];
	uint32 w = gop->width, h = gop->height ;
	//(Image *im, uint32 width, uint32 height, ColorSpace color, uint32 bpp, uint32 steps)
	image_init(&frame->img[0], w, h, gop->color, gop->bpp, gop->steps);
	frame->size = w*h;
	if(gop->color == CS444 || gop->color == RGB) {
		image_init(&frame->img[1], w, h, gop->color, gop->bpp, gop->steps);
		image_init(&frame->img[2], w, h, gop->color, gop->bpp, gop->steps);
		frame->size = frame->size*3;
	}
	if(gop->color == CS422){
		image_init(&frame->img[1], w>>1 , h, gop->color, gop->bpp, gop->steps);
		image_init(&frame->img[2], w>>1 , h, gop->color, gop->bpp, gop->steps);
		frame->size = frame->size*2;
	}
	if(gop->color == CS420){
		image_init(&frame->img[1], w>>1 , h>>1, gop->color, gop->bpp, gop->steps);
		image_init(&frame->img[2], w>>1 , h>>1, gop->color, gop->bpp, gop->steps);
		frame->size = (frame->size*3)>>1;
	}
}

void frame_copy(GOP *gop, uint32 fr, uchar *y, uchar *u, uchar *v)
///	\fn	void frame_copy(GOP *gop, uint32 fr, uchar *y, uchar *u, uchar *v)
///	\brief	Fill frame from the stream.
///	\param	gop			The GOP structure.
///	\param	fr			The frame number.
///	\param	y			The pointer to Bayer, gray, red or Y  image data
///	\param	u			The pointer to green or U  image data
///	\param	v			The pointer to blue or V  image data
{

	Frame *frame = &gop->frames[fr];

	printf("Start frame copy \n");
	image_copy(&frame->img[0], gop->bpp, y);
	if(gop->color != GREY  && gop->color != BAYER) {
		image_copy(&frame->img[1], gop->bpp, u);
		image_copy(&frame->img[2], gop->bpp, v);
	}
}

void frame_dwt_53(GOP *gop, uint32 fr)
///	\fn	void frame_dwt_53(GOP *gop, uint32 fr)
///	\brief	Discrete wavelets frame transform.
///	\param	gop			The GOP structure.
///	\param	fr			The frame number.
{
	Frame *frame = &gop->frames[fr];

	frame->img[0].sub = gop->sub[0]; //Set current image for DWT transform
	image_dwt_53(&frame->img[0], gop->color, gop->steps, gop->buf);
	if(gop->color != GREY  && gop->color != BAYER) {
		frame->img[1].sub = gop->sub[1]; //Set current image for DWT transform
		frame->img[2].sub = gop->sub[2]; //Set current image for DWT transform
		image_dwt_53(&frame->img[1], gop->color, gop->steps, gop->buf);
		image_dwt_53(&frame->img[2], gop->color, gop->steps, gop->buf);
	}
}

void frame_idwt_53(GOP *gop, uint32 fr, uint32 isteps)
///	\fn	void frame_idwt_53(GOP *gop, uint32 fr, uint32 step)
///	\brief	Invert discrete wavelets frame transform.
///	\param	gop			The GOP structure.
///	\param	fr			The frame number.
/// \param	isteps		The steps of IDWT should be lees or equal DWT steps
{
	Frame *frame = &gop->frames[fr];

	frame->img[0].sub = gop->sub[0]; //Set current image for IDWT transform
	image_idwt_53(&frame->img[0], gop->color, gop->steps, gop->buf, isteps);
	if(gop->color != GREY  && gop->color != BAYER) {
		frame->img[1].sub = gop->sub[1]; //Set current image for IDWT transform
		frame->img[2].sub = gop->sub[2]; //Set current image for IDWT transform
		image_idwt_53(&frame->img[1], gop->color, gop->steps, gop->buf, isteps);
		image_idwt_53(&frame->img[2], gop->color, gop->steps, gop->buf, isteps);
	}
}

void frame_fill_subb(GOP *gop, uint32 fr)
///	\fn	void frame_fill_subb(GOP *gop, uint32 fr)
///	\brief	Fill distribution probability array.
///	\param	gop			The GOP structure.
///	\param	fr			The frame number.
{

	image_fill_subb(&gop->frames[fr].img[0], gop->color, gop->steps);
	if(gop->color != GREY  && gop->color != BAYER) {
		image_fill_subb(&gop->frames[fr].img[1], gop->color, gop->steps);
		image_fill_subb(&gop->frames[fr].img[2], gop->color, gop->steps);
	}
}

void frame_white_balance(GOP *gop, uint32 fr,  uint32 out_bits, Gamma gamma)
///	\fn	void frame_white_balance(GOP *gop, uint32 fr,  uint32 out_bits, Gamma gamma)
///	\brief	Make white balance and gamma correction of the frame (now for bayer frames only).
///	\param	gop			The GOP structure.
///	\param	fr			The frame number.
///	\param	out_bits	The bits per pixel for output frame.
///	\param	gamma		The type of gamma correction.
{
	Image *im = &gop->frames[fr].img[0];
	if(gop->color == BAYER){
		image_fill_hist(im, gop->color, gop->bg, gop->bpp);
		utils_white_balance(im->img, im->img, im->hist, im->look, im->width, im->height, gop->bg, gop->bpp, out_bits, gamma);
	}
}

void frame_bits_alloc(GOP *gop, uint32 fr, uint32 times)
///	\fn	void frame_bits_alloc(GOP *gop, uint32 fr, uint32 times)
///	\brief	Bits allocation for frame.
///	\param	gop			The GOP structure.
///	\param	fr			The frame number.
///	\param	times		Compression times.
{
	image_bits_alloc(&gop->frames[fr].img[0], gop->color, gop->steps, gop->bpp, times);
	if(gop->color != GREY  && gop->color != BAYER) {
		image_bits_alloc(&gop->frames[fr].img[1], gop->color, gop->steps, gop->bpp, times);
		image_bits_alloc(&gop->frames[fr].img[2], gop->color, gop->steps, gop->bpp, times);
	}
}

void frame_quantization(GOP *gop, uint32 fr)
///	\fn	void frame_quantization(GOP *gop, uint32 fr)
///	\brief	Frame quantization.
///	\param	gop			The GOP structure.
///	\param	fr			The frame number.
{
	image_quantization(&gop->frames[fr].img[0], gop->color, gop->steps);
	if(gop->color != GREY  && gop->color != BAYER) {
		image_quantization(&gop->frames[fr].img[1], gop->color, gop->steps);
		image_quantization(&gop->frames[fr].img[2], gop->color, gop->steps);
	}
}

uint32 frame_range_encode(GOP *gop, uint32 fr)
///	\fn	uint32 frame_range_encode(GOP *gop, uint32 fr)
///	\brief	Frame range encoder.
///	\param	gop			The GOP structure.
///	\param	fr			The frame number.
///	\retval				The size of encoded frame in bytes.
{
	uint32 size = 0;
	gop->frames[fr].img[0].c_size = image_range_encode(&gop->frames[fr].img[0], gop->color, gop->steps, gop->bpp, (uchar*)gop->buf);
	size += gop->frames[fr].img[0].c_size;
	if(gop->color != GREY  && gop->color != BAYER) {
		gop->frames[fr].img[1].c_size = image_range_encode(&gop->frames[fr].img[1], gop->color, gop->steps, gop->bpp, (uchar*)gop->buf);
		gop->frames[fr].img[1].c_size = image_range_encode(&gop->frames[fr].img[2], gop->color, gop->steps, gop->bpp, (uchar*)gop->buf);
		size += gop->frames[fr].img[1].c_size + gop->frames[fr].img[2].c_size;
	}
	return size;
}

uint32 frame_range_decode(GOP *gop, uint32 fr)
///	\fn	uint32 frame_range_decode(GOP *gop, uint32 fr)
///	\brief	Frame range decoder.
///	\param	gop			The GOP structure.
///	\param	fr			The frame number.
///	\retval				The size of decoded frame in bytes.
{
	uint32 size = 0;
	size += image_range_decode(&gop->frames[fr].img[0], gop->color, gop->steps, gop->bpp, (uchar*)gop->buf);
	if(gop->color != GREY  && gop->color != BAYER) {
		size += image_range_decode(&gop->frames[fr].img[1], gop->color, gop->steps, gop->bpp, (uchar*)gop->buf);
		size += image_range_decode(&gop->frames[fr].img[2], gop->color, gop->steps, gop->bpp, (uchar*)gop->buf);
	}
	return size;
}

uint32 frame_write(GOP *gop, uint32 fr, const char *filename)
///	\fn	uint32 frame_write(GOP *gop, uint32 fr, const char *filename)
///	\brief	Write frame in file.
///	\param	gop			The pointer to the GOP structure.
///	\param	fr			The frame number.
//	\param	filename	The file name.
///	\retval				The size of file.
{
    FILE *wl;
    WaletHeader wh;
    uchar *bits[3];
    uint32 i, sz, sz1, size=0;
    Frame *frm = &gop->frames[fr];

    //Fill header
    wh.marker	= 0x776C;
    wh.width	= gop->width;
    wh.height	= gop->height;
    wh.color	= gop->color;
    wh.bg		= gop->bg;
    wh.bpp		= gop->bpp;
    wh.steps	= gop->steps;

    sz = (gop->color == BAYER) ? ((gop->steps-1)*3+1)<<2 : gop->steps*3 + 1;
    bits[0] = (uchar *)calloc(sz, sizeof(uchar));
    for(i=0; i<sz; i++)  bits[0][i] = (frm->img[0].sub[i].a_bits<<4) | frm->img[0].sub[i].q_bits;

    if(gop->color != GREY  && gop->color != BAYER) {
    	sz1 = gop->steps*3 + 1;
    	bits[1] = (uchar *)calloc(sz, sizeof(uchar));
    	bits[2] = (uchar *)calloc(sz, sizeof(uchar));
    	for(i=0; i<sz; i++)  bits[1][i] = (frm->img[1].sub[i].a_bits<<4) | frm->img[1].sub[i].q_bits;
    	for(i=0; i<sz; i++)  bits[2][i] = (frm->img[2].sub[i].a_bits<<4) | frm->img[2].sub[i].q_bits;
    }

	wl = fopen(filename, "wb");
    if(wl == NULL) {
    	printf("Can't write to file %s\n", filename);
    	return;
    }
    //Write header
    fwrite (&wh, sizeof(wh), 1, wl); size += sizeof(wh);
    //Write bits array
    fwrite (&bits[0], 1, sz, wl); size += sz;
    //Write data
    fwrite (gop->buf, 1, frm->img[0].c_size, wl); size += frm->img[0].c_size;

    if(gop->color != GREY  && gop->color != BAYER) {
    	fwrite (&bits[1], 1, sz1, wl);  size += sz1;
    	fwrite (&gop->buf[frm->img[0].c_size], 1, frm->img[1].c_size, wl); size += frm->img[1].c_size;
    	fwrite (&bits[2], 1, sz1, wl);	size += sz1;
    	fwrite (&gop->buf[frm->img[0].c_size+frm->img[1].c_size], 1, frm->img[2].c_size, wl); size += frm->img[2].c_size;
    }
    fclose(wl);
    return size;
}

uint32 frame_read(GOP *gop, uint32 fr, const char *filename)
///	\fn	uint32 frame_write(GOP *gop, uint32 fr, const char *filename)
///	\brief	Read frame from file.
///	\param	gop			The pointer to the GOP structure.
///	\param	fr			The frame number.
//	\param	filename	The file name.
///	\retval				The size of file.
{
    FILE *wl;
    WaletHeader wh;
    uchar *bits[3];
    uint32 i, sz, sz1, size=0;
    Frame *frm = &gop->frames[fr];

	wl = fopen(filename, "rb");
    if(wl == NULL) {
    	printf("Can't open file %s\n", filename);
    	return;
    }

    //Read header
    if(fread(&wh, sizeof(wh), 1, wl) != 1) {
    	printf("Header read error\n", filename);
    	return;
    }
    size += sizeof(wh);
    if(wh.marker != 0x776C ){
    	printf("It's not walet format file\n", filename);
    	return;
    }
    // Fill Stream data stucture
    gop->width = 	wh.width;
    gop->height =	wh.height;
    gop->color = 	wh.color;
    gop->bg = 		wh.bg;
    gop->bpp = 	wh.bpp;
    gop->steps = 	wh.steps;

    //if(gop->sd->color)
 }

void frame_compress(GOP *gop, uint32 fr, uint32 times)
{
	Frame *frame = &gop->frames[fr];
	uint32 bits = gop->bpp, color =  gop->color, steps = gop->steps;
	uint32 size;
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
	size = frame_range_encode	(gop, fr);
	gettimeofday(&tv, NULL); end  = tv.tv_usec + tv.tv_sec*1000000;
	printf("Range coder time     = %f\n", (double)(end-start)/1000000.);
	printf("Frame size  = %d\n", size);
	//frame_write
}

void frame_decompress(Frame *frame, Subband **sub,  ColorSpace color, uint32 steps, uint32 bits, uint32 st)
{
	//frame_read
	//frame_range_decode	(frame, sub, color, steps, bits);
	//frame_idwt_53		(frame, sub, color, steps, st);
}
