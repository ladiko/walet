#ifndef UTILS_H_
#define UTILS_H_


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

uint8* utils_dwt_draw(GOP *g, uint32 fn, WaletConfig *wc, uint8 *rgb, uint8 steps);

void utils_rgb2bayer(uint8 *rgb, int16 *bay, uint32 w, uint32 h);
void fill_bayer_hist(int16 *img, uint32 *r, uint32 *g, uint32 *b, uint32 w, uint32 h,  BayerGrid bay, uint32 bits);


uint8* utils_grey_draw(int16 *img, uint8 *rgb, uint32 height, uint32 width);
uint8* utils_grey_draw8(uint8 *img, uint8 *rgb, uint32 w, uint32 h);
uint8* utils_bayer_draw(int16 *img, uint8 *rgb, uint32 height, uint32 width,  BayerGrid bay);
double utils_ape_16(int16 *before, int16 *after, uint32 dim, uint32 d);
double utils_psnr_16(int16 *before, int16 *after, uint32 dim, uint32 d);


void shift_b_to_w(uint8 *in,  int8 *out, int shift, uint32 size);
void shift_w_to_b( int8 *in, uint8 *out, int shift, uint32 size);


//uint8* 		utils_subband_draw		(Image *img, uint8 *rgb, ColorSpace color, uint32 steps);
//uint8* 		utils_bayer_draw		(uint8 *img, uint8 *rgb, uint32 height, uint32 width,  BayerGrid bay, uint8 bpp);
//uint8* 		utils_bayer_draw_16		(int16 *img, uint8 *rgb, uint32 height, uint32 width,  BayerGrid bay);

uint8* 		utils_draw				(uint8 *img, uint8 *rgb, uint32 w, uint32 h);
uint8* 	utils_cat				(uint8 *img, uint8 *img1, uint32 height, uint32 width, uint32 bits);
//uint8* 	utils_bayer_to_Y		(uint8 *img, uint8 *img1, uint32 w, uint32 h);
uint8* 		utils_ppm_to_bayer		(uint8 *img, uint8 *img1, uint32 w, uint32 h);
//uint8*		utils_bayer_to_4color	(uint8 *img, uint8 *img1, uint8 **p, uint32 w, uint32 h);
void 		utils_bayer_to_4color	(uint8 *img, uint32 w, uint32 h, uint8 *p0, uint8 *p1, uint8 *p2, uint8 *p3);
uint8* 		utils_4color_draw		(uint8 *img, uint8 *rgb, uint32 w, uint32 h, uint8 *p0, uint8 *p1, uint8 *p2, uint8 *p3);
uint8* utils_4color_scale_draw		(uint8 *rgb, uint32 w, uint32 h, Pic8u *p0,  Pic8u *p1,  Pic8u *p2,  Pic8u *p3);
//uint8* 		utils_4color_draw		(uint8 *img, uint8 *rgb, uint8 **p, uint32 w, uint32 h);

uint8* utils_draw_bayer(uint8 *img, uint8 *rgb, uint32 w, uint32 h,  BayerGrid bay);
void utils_resize_bayer_2x(int16 *img, int16 *img1, uint32 w, uint32 h);
uint8* utils_bayer_scale_draw(uint8 *rgb, uint32 w, uint32 h, Pic8u *p);
uint8* utils_color_scale_draw(uint8 *rgb, uint32 w, uint32 h, Pic8u *p);
void util_bayer_to_rgb(uint8 *img, uint8 *rgb, uint32 w, uint32 h);
void utils_resize_rgb_2x(uint8 *img, uint8 *img1, uint32 w, uint32 h);
uint8* utils_rgb_scale_draw(uint8 *rgb, uint32 w, uint32 h, Pic8u *p);
void utils_bayer_to_Y(uint8 *img, uint8 *img1, uint32 w, uint32 h);

uint8* utils_scale_draw(uint8 *rgb, uint32 w, uint32 h, Pic8u *p);
uint8* utils_rgb_scale_draw(uint8 *rgb, uint32 w, uint32 h, Pic8u *p);


void utils_print_img(uint8* img, uint32* ind, uint32 w, uint32 h,  uint32 bx, uint32 by,  uint32 lx, uint32 ly);
void utils_print_ind(uint8* img, uint32 w, uint32 h,  uint32 bx, uint32 by,  uint32 lx, uint32 ly);
void utils_connect_region(uint8 *img, uint32 *ind, uint32 *arg, uint32 w, uint32 h);
void color_seg(uint8 *img, uint32 w, uint32 h, uint32 theresh);

double 		utils_dist	(uint8 *before, uint8 *after, uint32 dim, uint32 d);
double 		utils_ape	(uint8 *before, uint8 *after, uint32 dim, uint32 d);
double 		utils_psnr	(uint8 *before, uint8 *after, uint32 dim, uint32 d);

void utils_unifom_dist_entr(uint32 *distrib, uint32 dist_size, uint32 step, uint32 size, uint32 *q, double *dis, double *e);
uint8* wavelet_to_rgb(uint8 *img, uint8 *rgb, int height, int width, int step);
uint8* wavelet_to_rgb1(uint8 *img, uint8 *rgb, int height, int width, int step);
uint8* uint8_to_rgb(uint8 *img, uint8 *rgb, int height, int width);
uint8* img_to_rgb128(uint8 *img, uint8 *rgb, int height, int width);
uint8* malet_to_rgb(uint8 *img, uint8 *rgb, int h, int w, int step);
void copy(uint8 *in, uint8 *out, int w, int h);
void compare(uint8 *in, uint8 *out, int w, int h);
void resizeonepix(uint8 *in, int w, int h);
//double dist(uint8 *before, uint8 *after, uint32 dim, uint32 d);
//double ape(uint8 *before, uint8 *after, uint32 dim, uint32 d);
//double psnr(uint8 *before, uint8 *after, uint32 dim, uint32 d);
//double dist3(uint8 *before, uint8 *after, uint32 dim, uint32 d);
//double ape3(uint8 *before, uint8 *after, uint32 dim, uint32 d);
//double psnr3(uint8 *before, uint8 *after, uint32 dim, uint32 d);
uint8* YUV_to_RGB(uint8 *rgb, uint8 *y, uint8 *u, uint8 *v, uint32 sq);
uint8* YUV420p_to_RGB(uint8 *rgb, uint8 *y, uint8 *u, uint8 *v, uint32 w, uint32 h);
//extern void fill_rect_hor(imgmv *pic,  Vector size,  Vector rec_left_top,  Vector rec_right_bot,  Vector line);
//extern void fill_rect_ver(imgmv *pic,  Vector size,  Vector rec_left_top,  Vector rec_right_bot,  Vector line);


uint32 utils_read_ppm(const char *filename, uint32 *w, uint32 *h, uint32 *bpp, uint8 *img);
void utils_subtract(uint8 *img1, uint8 *img2, uint8 *sub, uint32 w, uint32 h);
uint8* utils_color_draw(uint8 *img, uint8 *rgb, uint32 w, uint32 h, uint32 col);

void util_copy_border(uint8 *img, uint8 *img1, uint32 b, uint32 w, uint32 h);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*UTILS_HH_*/
