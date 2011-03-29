/*
 * segmentation.h
 *
 *  Created on: 21.08.2010
 *      Author: vadim
 */

#ifndef SEGMENTATION_H_
#define SEGMENTATION_H_

typedef struct {
	uint16		x;			//The point location
	uchar 		cl[4];		//The left color
	uchar 		cr[4];		//The right color
} Point;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void seg_grad(imgtype *img, imgtype *img1, uint32 w, uint32 h, uint32 th);
void seg_local_max( Pixel *pix,  uint32 *npix, imgtype *img, uint32 w, uint32 h);
uint32 seg_line(Pixel *pix, Edge *edges, imgtype *img, uint32 w, uint32 h);
void seg_draw_lines(Pixel *pix, uint32 npix, imgtype *img, uint32 w, uint32 h);
void seg_draw_edges(Pixel *pix, Edge *edge, uint32 nedge, imgtype *img, uint32 w, uint32 h);
void seg_compare(Pixel *pix, Pixel *pix1, imgtype *grad1, imgtype *grad2, imgtype *img1, imgtype *img2, uint32 w, uint32 h);
void seg_draw_vec(Pixel *pix, uint32 npix, imgtype *img, uint32 w, uint32 h);
void seg_draw_pix(Pixel *pix, uint32 npix, imgtype *img, uint32 w, uint32 h);
void seg_intersect_pix(imgtype *img1, imgtype *img2, uint32 w, uint32 h);
void seg_mvector_copy(Pixel *pix, imgtype *grad1, imgtype *img1, imgtype *img2, uint32 w, uint32 h);
void seg_reduce_line(Pixel *pix, imgtype *img, uint32 w, uint32 h);

void seg_quant(imgtype *img1, imgtype *img2, uint32 w, uint32 h, uint32 q);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SEGMENTATION_H_ */
