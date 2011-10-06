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
	uint8 		cl[4];		//The left color
	uint8 		cr[4];		//The right color
} Point;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void seg_find_clusters(uint32 *i3d, uint16 *lut, int16 *r, int16 *g, int16 *b, uint32 w, uint32 h, uint32 rd,  uint32 bpp, p3d *q,uint32 *buf);

void seg_grad16(int16 *img, int16 *img1, uint32 w, uint32 h, uint32 th);
void seg_grad(uint8 *img, uint8 *img1, uint32 w, uint32 h, uint32 th);
void seg_local_max( Pixel *pix,  uint32 *npix, uint8 *img, uint32 w, uint32 h);
uint32 seg_line(Pixel *pix, Edge *edges, uint8 *img, uint32 w, uint32 h);
void seg_draw_lines(Pixel *pix, uint32 npix, uint8 *img, uint32 w, uint32 h);
void seg_draw_edges(Pixel *pix, Edge *edge, uint32 nedge, uint8 *img, uint32 w, uint32 h, uint32 px, uint32 col);
void seg_draw_edges_des(Pixel *pix, Edge *edge, uint32 nedge, uint8 *img, uint32 w, uint32 h, uint32 px, uint32 col);
void seg_compare(Pixel *pix, Edge *edge, uint32 nedge, uint8 *grad1, uint8 *grad2, uint8 *img1, uint8 *img2, uint8 *mmb, uint32 w, uint32 h, uint32 mvs);
void seg_draw_vec(Pixel *pix, uint32 npix, uint8 *img, uint32 w, uint32 h);
void seg_draw_pix(Pixel *pix, uint8 *img, uint8 *grad, uint32 w, uint32 h, uint32 col);
void seg_intersect_pix(uint8 *img1, uint8 *img2, uint32 w, uint32 h);
void seg_mvector_copy(Pixel *pix, uint8 *grad1, uint8 *img1, uint8 *img2, uint32 w, uint32 h);
void seg_reduce_line(Pixel *pix, uint8 *img, uint32 w, uint32 h);
uint32 seg_pixels(Pixel *pix, uint8 *img, uint32 w, uint32 h);
uint32 seg_region(Pixel *pix, uint8 *img, uint32 w, uint32 h);

void seg_quant(uint8 *img1, uint8 *img2, uint32 w, uint32 h, uint32 q);
void seg_fall_forest(uint8 *img, uint8 *img1, uint32 w, uint32 h);
void seg_fall_forest1(uint8 *img, uint8 *img1, uint8 *img2, uint32 w, uint32 h);
void seg_grad1(uint8 *img, uint8 *img1,uint8 *img2, uint32 w, uint32 h, uint32 th);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SEGMENTATION_H_ */
