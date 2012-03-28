#ifndef _FILTERS_H_
#define _FILTERS_H_

typedef int (*FP)(uint8* , uint32 );

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void filters_white_balance			(int16 *in, int16 *out, uint32 *hist, uint16 *look, uint32 w, uint32 h,  BayerGrid bay,  uint32 in_bits, uint32 out_bits, Gamma gamma);
void filters_wb(int16 *R, int16 *G, int16 *B, uint32 *hist, uint16 *look, uint32 w, uint32 h,  BayerGrid bay,  uint32 in_bits, uint32 out_bits, Gamma gamma);
void filters_bayer_median_filter_3x3(int16 *img, int16 *img1, uint32 w, uint32 h, BayerGrid bay);
void filters_median_3x3				(int16 *img, int16 *img1, uint32 w, uint32 h);
void filters_bayer_to_Y				(int16 *img, int16 *img1, uint32 w, uint32 h, BayerGrid bay);
void filters_bayer_gradient			(int16 *img, int16 *img1, uint32 w, uint32 h, BayerGrid bay, uint32 thresh);
void filters_gradient				(int16 *img, int16 *img1, uint32 w, uint32 h, uint32 thresh);
void filters_edge_detector			(int16 *img, int16 *img1, uint32 w, uint32 h);

void filter_median(uint8 *img, uint8 *img1, uint32 w, uint32 h);
void filter_contrast(uint8 *img, uint8 *img1, uint32 w, uint32 h);
void filter_median_bayer(int16 *img, int16 *img1, uint32 w, uint32 h);
void filter_average(int16 *img, int16 *img1, uint32 w, uint32 h, uint32 thresh);
void filter_noise(uint8 *img, uint8 *img1, uint32 w, uint32 h, uint32 th);

void filter_median_buf(uint8 *img, uint8 *img1, uint8 *buff, uint32 w, uint32 h);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
