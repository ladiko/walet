#ifndef IMAGE_H_
#define IMAGE_H_

typedef struct {
	Vector size;
	imgtype *img;
}	Image;

static uint16 eng[5][16] = {
	{   4,  2,  2,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, },
	{   8,  4,  4,  2,  2,  2,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0, },
	{  16,  8,  8,  4,  4,  4,  2,  2,  2,  1,  0,  0,  0,  0,  0,  0, },
	{  32, 16, 16,  8,  8,  8,  4,  4,  4,  2,  2,  2,  1,  0,  0,  0, },
	{  64, 32, 32, 16, 16, 16,  8,  8,  8,  4,  4,  4,  2,  2,  2,  1, },
};

static uint16 del[5][16] = {
	{   0,  1,  1,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, },
	{   0,  1,  1,  2,  2,  2,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0, },
	{   0,  1,  1,  2,  2,  2,  3,  3,  3,  4,  0,  0,  0,  0,  0,  0, },
	{   0,  1,  1,  2,  2,  2,  3,  3,  3,  4,  4,  4,  5,  0,  0,  0, },
	{   0,  1,  1,  2,  2,  2,  3,  3,  3,  4,  4,  4,  5,  5,  5,  6, },
};

static uint16 sb[4] = {0, 1, 1, 2};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void	image_init			(Image *img, uint32 x, uint32 y);
void 	image_copy			(Image *img, uchar *v);
void 	image_dwt_53 		(Image *im, imgtype *buf, Subband **sub, ColorSpace color, uint32 steps);
void 	image_idwt_53		(Image *im, imgtype *buf, Subband **sub, ColorSpace color, uint32 steps);
void 	image_fill_prob		(Image *im, Subband **sub, uint32 bits, uint32 color, uint32 steps);
double 	image_entropy		(Image *im, Subband **sub, uint32 bits, ColorSpace color, uint32 steps, int st);
void 	image_quantization	(Image *im, Subband **sub, uint32 bits, ColorSpace color, uint32 steps, int st);
uint32 	image_compress		(Image *im, Subband **sub, uint32 bits, ColorSpace color, uint32 steps, uchar *buf);
uint32 	image_decompress	(Image *im, Subband **sub, uint32 bits, ColorSpace color, uint32 steps, uchar *buf);

void image_idwt_531(Image *im, imgtype *buf, Subband **sub, ColorSpace color, uint32 steps, uint32 st, Vector *size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* IMAGE_HH_ */
