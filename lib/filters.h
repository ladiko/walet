#ifndef _FILTERS_H_
#define _FILTERS_H_

typedef int (*FP)(imgtype* , uint32 );

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void filters_white_balance			(imgtype *img, imgtype *img1, uint32 w, uint32 h, BayerGrid bay, uint32 *hist, uint16 *look, uint32 in_bits, uint32 out_bits, Gamma gamma);
void filters_bayer_median_filter_3x3(imgtype *img, imgtype *img1, uint32 w, uint32 h, BayerGrid bay);
void filters_median_3x3				(imgtype *img, imgtype *img1, uint32 w, uint32 h);
void filters_bayer_to_Y				(imgtype *img, imgtype *img1, uint32 w, uint32 h, BayerGrid bay);
void filters_bayer_gradient			(imgtype *img, imgtype *img1, uint32 w, uint32 h, BayerGrid bay, uint32 thresh);
void filters_gradient				(imgtype *img, imgtype *img1, uint32 w, uint32 h, uint32 thresh);
void filters_edge_detector			(imgtype *img, imgtype *img1, uint32 w, uint32 h);

void filter_median(uchar *img, uchar *img1, uint32 w, uint32 h);
void filter_median_bayer(uchar *img, uchar *img1, uint32 w, uint32 h);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
