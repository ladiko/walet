#ifndef UTILS_H_
#define UTILS_H_


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

uchar* 		utils_subband_draw		(Image *img, uchar *rgb, ColorSpace color, uint32 steps);
uchar* 		utils_bayer_draw		(imgtype *img, uchar *rgb, uint32 height, uint32 width,  BayerGrid bay);
uchar* 		utils_grey_draw			(imgtype *img, uchar *rgb, uint32 height, uint32 width);
imgtype* 	utils_cat				(imgtype *img, imgtype *img1, uint32 height, uint32 width, uint32 bits);
imgtype* 	utils_bayer_to_Y		(imgtype *img, imgtype *img1, uint32 w, uint32 h);
imgtype* 	utils_bayer_to_gradient	(imgtype *img, imgtype *img1, uint32 w, uint32 h, BayerGrid bay, uint32 thresh);
imgtype* 	utils_watershed			(imgtype *img, imgtype *img1, uint32 w, uint32 h);
imgtype* 	utils_kill_pix			(imgtype *img, imgtype *img1, uint32 w, uint32 h);
void 		utils_min_region		(imgtype *img, uint32 *ind, uint32 *arg, uint32 w, uint32 h);
void 		utils_steep_descent		(imgtype *img, uint32 *ind, uint32 *arg, uint32 w, uint32 h);

double 		utils_dist	(imgtype *before, imgtype *after, uint32 dim, uint32 d);
double 		utils_ape	(imgtype *before, imgtype *after, uint32 dim, uint32 d);
double 		utils_psnr	(imgtype *before, imgtype *after, uint32 dim, uint32 d);

void utils_unifom_dist_entr(uint32 *distrib, uint32 dist_size, uint32 step, uint32 size, uint32 *q, double *dis, double *e);
uchar* wavelet_to_rgb(imgtype *img, uchar *rgb, int height, int width, int step);
uchar* wavelet_to_rgb1(imgtype *img, uchar *rgb, int height, int width, int step);
uchar* uchar_to_rgb(uchar *img, uchar *rgb, int height, int width);
uchar* img_to_rgb128(imgtype *img, uchar *rgb, int height, int width);
uchar* malet_to_rgb(imgtype *img, uchar *rgb, int h, int w, int step);
void copy(imgtype *in, imgtype *out, int w, int h);
void compare(imgtype *in, imgtype *out, int w, int h);
void resizeonepix(uchar *in, int w, int h);
//double dist(imgtype *before, imgtype *after, uint32 dim, uint32 d);
//double ape(imgtype *before, imgtype *after, uint32 dim, uint32 d);
//double psnr(imgtype *before, imgtype *after, uint32 dim, uint32 d);
//double dist3(uchar *before, uchar *after, uint32 dim, uint32 d);
//double ape3(uchar *before, uchar *after, uint32 dim, uint32 d);
//double psnr3(uchar *before, uchar *after, uint32 dim, uint32 d);
uchar* YUV_to_RGB(uchar *rgb, imgtype *y, imgtype *u, imgtype *v, uint32 sq);
uchar* YUV420p_to_RGB(uchar *rgb, imgtype *y, imgtype *u, imgtype *v, uint32 w, uint32 h);
//extern void fill_rect_hor(imgmv *pic,  Vector size,  Vector rec_left_top,  Vector rec_right_bot,  Vector line);
//extern void fill_rect_ver(imgmv *pic,  Vector size,  Vector rec_left_top,  Vector rec_right_bot,  Vector line);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*UTILS_HH_*/
