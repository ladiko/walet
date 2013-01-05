#ifndef _FILTERS_H_
#define _FILTERS_H_

typedef int (*FP)(uint8* , uint32 );

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
void filter_median_ad(int16 *img, int16 *img1, int16 *buff, uint32 w, uint32 h);
void filter_median_diff(int16 *img, int16 *img1, int16 *img2, int16 *buff, uint32 w, uint32 h);
void filter_median_bayer_ad(int16 *img, int16 *img1, int16 *buff, uint32 w, uint32 h);
void filter_median_bayer_diff(int16 *img, int16 *img1, int16 *img2, int16 *buff, uint32 w, uint32 h);


void filters_white_balance			(int16 *in, int16 *out, uint32 *hist, uint16 *look, uint32 w, uint32 h,  BayerGrid bay,  uint32 in_bits, uint32 out_bits, Gamma gamma);
void filters_wb(int16 *Y, int16 *R, int16 *G, int16 *B, uint8 *r, uint8 *g, uint8 *b, uint8 *buff,  uint32 *hist, uint16 *look, uint32 w, uint32 h,  BayerGrid bay,  uint32 in_bits, uint32 out_bits, Gamma gamma);
void filters_bayer_median_filter_3x3(int16 *img, int16 *img1, uint32 w, uint32 h, BayerGrid bay);
void filters_median_3x3				(int16 *img, int16 *img1, uint32 w, uint32 h);
void filters_bayer_to_Y				(int16 *img, int16 *img1, uint32 w, uint32 h, BayerGrid bay);
void filters_bayer_gradient			(int16 *img, int16 *img1, uint32 w, uint32 h, BayerGrid bay, uint32 thresh);
void filters_gradient				(int16 *img, int16 *img1, uint32 w, uint32 h, uint32 thresh);
void filters_edge_detector			(int16 *img, int16 *img1, uint32 w, uint32 h);

void filter_fast_median(uint8 *img, uint8 *img1, uint32 w, uint32 h);
void filter_median(uint8 *img, uint8 *img1, uint32 w, uint32 h);
void filter_contrast(uint8 *img, uint8 *img1, uint32 w, uint32 h);
void filter_contrast_5x5(uint8 *img, uint8 *img1, uint32 w, uint32 h);

void filter_median_bayer1(int16 *img, int16 *img1, uint32 w, uint32 h);
void filter_average(int16 *img, int16 *img1, uint32 w, uint32 h, uint32 thresh);
void filter_noise(uint8 *img, uint8 *img1, uint32 w, uint32 h, uint32 th);

void filter_median_buf(int16 *img, int16 *img1, int16 *buff, uint32 w, uint32 h);
void filter_median_16(int16 *img, int16 *img1, uint32 w, uint32 h);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
