#ifndef _FILTERS_H_
#define _FILTERS_H_

typedef int (*FP)(uint8* , uint32 );

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void filters_white_balance			(uint8 *img, uint8 *img1, uint32 w, uint32 h, BayerGrid bay, uint32 *hist, uint16 *look, uint32 in_bits, uint32 out_bits, Gamma gamma);
void filters_bayer_median_filter_3x3(uint8 *img, uint8 *img1, uint32 w, uint32 h, BayerGrid bay);
void filters_median_3x3				(uint8 *img, uint8 *img1, uint32 w, uint32 h);
void filters_bayer_to_Y				(uint8 *img, uint8 *img1, uint32 w, uint32 h, BayerGrid bay);
void filters_bayer_gradient			(uint8 *img, uint8 *img1, uint32 w, uint32 h, BayerGrid bay, uint32 thresh);
void filters_gradient				(uint8 *img, uint8 *img1, uint32 w, uint32 h, uint32 thresh);
void filters_edge_detector			(uint8 *img, uint8 *img1, uint32 w, uint32 h);

void filter_median(uint8 *img, uint8 *img1, uint32 w, uint32 h);
void filter_median_bayer(uint8 *img, uint8 *img1, uint32 w, uint32 h);
void filter_average(uint8 *img, uint8 *img1, uint32 w, uint32 h, uint32 thresh);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
