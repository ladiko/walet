#ifndef UTILS_H_
#define UTILS_H_


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

void utils_image_copy_n(uint8 *buff, int16 *img, uint32 w, uint32 h, uint32 bpp);
uint8* utils_grey_draw8_n(uint8 *img, uint8 *rgb, uint32 w, uint32 h);
uint8* utils_gray16_rgb8(int16 *in, uint8 *out, uint32 w, uint32 h, uint32 bpp, uint32 par);
uint8* utils_rgb16_rgb8(int16 *in, uint8 *out, uint32 w, uint32 h, uint32 bpp, uint32 par);
void utils_wb(int16 *in, int *rm, int *bm, uint32 *buff, int sh, uint32 bpp, uint32 w, uint32 h);
void utils_wb_rgb(int16 *in, int16 *out, int16 *buff, uint32 bpp, uint32 w, uint32 h);
void utils_wb_bayer(int16 *in, int16 *out, int16 *buff, uint32 bpp, uint32 bg, uint32 w, uint32 h);
void utils_transorm_to_8bits(const int16 *in, int16 *out, uint8 *buff, const uint32 bits, const uint32 b, const uint32 w, const uint32 h);

void utils_bits12to8(int16 *img, uint8 *img1, uint32 *look, uint32 w, uint32 h);

void utils_bayer_to_RGB_DWGI(int16 *img, int16 *R, int16 *G, int16 *B, int16 *buff, uint32 w, uint32 h, BayerGrid bay);

void utils_ACE(int16 *in, int16 *out, int16 *buff, uint32 bits, uint32 w, uint32 h);
void utils_ACE_fast(int16 *in, int16 *out, int16 *buff, uint32 bits, uint32 w, uint32 h);
void utils_ACE_fast_y(int16 *in, int16 *out, int16 *buff, uint32 bg, uint32 bpp, uint32 w, uint32 h);
void utils_ACE_fast_local(int16 *in, int16 *out, int *buff, uint32 bpp, uint32 bits, uint32 w, uint32 h);
void utils_HDR_multy(int16 *in, int16 *out, uint32 bits, uint32 w, uint32 h);
void utils_HDR_multy_local(int16 *in, int16 *out, uint32 bits, uint32 w, uint32 h);
void utils_HDR_avr(int16 *in, int16 *out, int16 *dif, uint32 *buff, uint32 bpp, uint32 w, uint32 h);

void utils_BM_denoise(int16 *in, int16 *out, uint32 *buff, uint32 bg, uint32 bpp, uint32 w, uint32 h);
void utils_BM_denoise_local(int16 *in, int16 *out, uint32 *buff, uint32 bg,  uint32 bpp, uint32 sg, uint32 w, uint32 h);
void utils_NLM_denoise(int16 *in, int16 *out, int16 *buff, uint32 bg,  uint32 bpp, uint32 sg, uint32 w, uint32 h);
int utils_noise_detection(int16 *in, int16 *med, int *buff, uint32 bpp, uint32 w, uint32 h);

uint32 utils_read_pgm_whb(FILE **wl, const char *filename, uint32 *w, uint32 *h, uint32 *bpp);
uint32 utils_read_pgm_img(FILE **wl,uint32 *w, uint32 *h, uint32 *bpp, uint8 *img);

void utils_resize_down_2x(uint8 *in, uint8 *out, uint8 *buff, uint32 w, uint32 h);
void utils_resize_down_2x_(uint8 *in, uint8 *out, uint8 *buff, uint32 w, uint32 h);
void utils_zoom_out_gray16(uint16 *in, uint16 *out, uint32 *buff, uint32 zoom, uint32 w, uint32 h);
void utils_zoom_out_rgb16_to_rgb16(uint16 *in, uint16 *out, uint32 *buff, uint32 zoom, uint32 shift, uint32 w, uint32 h);
void utils_zoom_out_bayer16_to_rgb16(uint16 *in, uint16 *out, uint32 *buff, uint32 zoom, BayerGrid bay, uint32 w, uint32 h);


uint8* utils_dwt_bayer_draw(GOP *g, uint32 fn, WaletConfig *wc, uint8 *rgb, uint8 steps);
uint8* utils_bayer_to_rgb_grad	(int16 *img, uint8 *rgb, uint32 w, uint32 h, BayerGrid bay, int shift);
uint8* utils_dwt_image_draw(Image *img, uint8 *rgb, uint32 steps);

uint8* utils_resize_draw(Pic8u *p, uint8 *rgb, uint32 steps, uint32 w);
uint8* utils_resize_draw_rgb(Pic8u *r, Pic8u *g, Pic8u *b, uint8 *rgb, uint32 steps, uint32 w);
uint8* utils_contour(Pic8u *p, uint8 *rgb, uint32 n);
uint8* utils_contour32(Pic32u *p, uint8 *rgb, uint32 n);
uint8* utils_contour_rgb(Pic8u *r, Pic8u *g, Pic8u *b, uint8 *rgb, uint32 n);

void utils_rgb2bayer(uint8 *rgb, int16 *bay, uint32 w, uint32 h);
void fill_bayer_hist(int16 *img, uint32 *r, uint32 *g, uint32 *b, uint32 w, uint32 h,  BayerGrid bay, uint32 bits);

uint8* utils_grey_draw(int16 *img, uint8 *rgb, uint32 w, uint32 h, uint32 sh);
uint8* utils_grey_draw8(uint8 *img, uint8 *rgb, uint32 w, uint32 h, uint32 sh);
uint8* utils_bayer_draw(int16 *img, uint8 *rgb, uint32 height, uint32 width,  BayerGrid bay);
double utils_ape_16(int16 *before, int16 *after, uint32 dim, uint32 d);
double utils_psnr_16(int16 *before, int16 *after, uint32 dim, uint32 d);
double utils_ssim_16(int16 *im1, int16 *im2, uint32 w, uint32 h, uint32 bbp, int bd, uint32 d);

uint8* 	utils_bayer_to_RGB24(int16 *img, uint8 *rgb, int16 *buff, uint32 w, uint32 h, BayerGrid bay, uint32 bpp);
void 	utils_bayer_to_RGB	(int16 *img, int16 *R, int16 *G, int16 *B, int16 *buff, uint32 w, uint32 h, BayerGrid bay);
void 	utils_RGB24_to_RGB	(uint8 *img, int16 *r, int16 *g, int16 *b, uint32 w, uint32 h, uint32 bpp, uint32 pad);
void 	utils_RGB_to_RGB24	(uint8 *img, int16 *r, int16 *g, int16 *b, uint32 w, uint32 h, uint32 bpp);
uint8*	utils_RGB_to_RGB24_8(uint8 *img, uint8 *r, uint8 *g, uint8 *b, uint32 w, uint32 h, uint32 bpp);
void 	utils_bayer_to_YUV444(int16 *img, int16 *Y, int16 *U, int16 *V, int16 *buff, uint32 w, uint32 h, BayerGrid bay);
void 	utils_RGB24_to_YUV444(uint8 *rgb, int16 *Y, int16 *U, int16 *V, uint32 w, uint32 h, uint32 bpp, uint32 pad);
void 	utils_RGB24_to_YUV420(uint8 *rgb, int16 *Y, int16 *U, int16 *V, uint32 w, uint32 h, uint32 bpp, uint32 pad);
uint8* 	utils_YUV444_to_RGB24(uint8 *rgb, int16 *Y, int16 *U, int16 *V, uint32 w, uint32 h, uint32 bpp);
uint8* 	utils_YUV420_to_RGB24(uint8 *rgb, int16 *Y, int16 *U, int16 *V, uint32 w, uint32 h, uint32 bpp);
//void utils_bayer_to_RGB_DWGI(int16 *img, int16 *R, int16 *G, int16 *B, int16 *buff, uint32 w, uint32 h, BayerGrid bay);

void 	utils_bayer_to_RGB_fast	(int16 *img, uint8 *r, uint8 *g, uint8 *b, uint32 w, uint32 h, BayerGrid bay, uint32 sh);
void 	utils_bayer_to_RGB_fast_(int16 *img, uint8 *r, uint8 *g, uint8 *b, uint32 w, uint32 h, BayerGrid bay, uint32 sh);
uint8* 	utils_bayer_to_Y_fast	(int16 *img, uint8 *Y, uint32 w, uint32 h, uint32 sh);
uint8* 	utils_bayer_to_Y_fast_	(int16 *img, uint8 *Y, uint32 w, uint32 h, uint32 sh);
void 	utils_bayer_to_YUV420	(int16 *img, uint8 *Y, uint8 *U, uint8 *V, int16 *buff, uint32 w, uint32 h, BayerGrid bay);

int16* 	utils_specular_border(int16 *img, int16 *img1, uint32 w, uint32 h, uint32 bor);
void 	utils_image_copy(uint8 *buff, int16 *img, uint32 w, uint32 h, uint32 bpp);
void 	utils_turn_on_180(int16 *img, uint16 *buff, uint32 w, uint32 h);
void 	fill_hist(int16 *img, uint32 *h, uint32 size, uint32 bits);

void shift_b_to_w(uint8 *in,  int8 *out, int shift, uint32 size);
void shift_w_to_b( int8 *in, uint8 *out, int shift, uint32 size);
uint8* utils_shift(int16 *img, uint8 *rgb, uint32 w, uint32 h, int sh);
int16* utils_shift16(int16 *img, int16 *rgb, uint32 w, uint32 h, int sh);

double entropy8(uint8 *img, uint32 *buf, uint32 w, uint32 h, uint32 bpp);
double entropy16(int16 *img, uint32 *buf, uint32 w, uint32 h, uint32 bpp);
uint8* utils_grey_draw_rgb(int16 *R, int16 *G, int16 *B, uint8 *rgb, uint32 w, uint32 h, uint32 sh);

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

void make_lookup(int16 *img, uint32 *h, uint32 *look, uint32 size, uint32 ibit, uint32 hbit);
void make_lookup1(int16 *img, uint32 *hist, uint32 *look, uint32 w, uint32 h, uint32 ibit, uint32 hbit);
void bits12to8(int16 *img, int16 *img1, uint32 *look, uint32 w, uint32 h, uint32 ibit, uint32 hbit);
void utils_bayer_to_RGB24_white_balance(int16 *img, int16 *img1, uint32 w, uint32 h, BayerGrid bay, uint32 bpp);
void utils_bayer_local_hdr(int16 *img, int16 *img1, uint32 w, uint32 h, BayerGrid bay, uint32 bpp, int low, int top);
void make_hist(int16 *img, uint32 *h, uint32 size, uint32 ibit, int *low, int *top);
void utils_bayer_local_hdr1(int16 *img, int16 *img1, uint32 w, uint32 h, BayerGrid bay, uint32 bpp);
void utils_bayer_local_hdr2(int16 *img, int16 *img1, int16 *buff, uint32 w, uint32 h, BayerGrid bay, uint32 bpp);

void utils_subtract1(uint8 *img1, uint8 *img2, int16 *img3, uint32 w, uint32 h);
void utils_remove_border(uint8 *img1, uint8 *img2, uint32 w, uint32 h);
void util_make_hdr(int16 *in, int16 *inm, int16 *im8, int16 *out, int16 *df, uint32 w, uint32 h);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*UTILS_HH_*/
