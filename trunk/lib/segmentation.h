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

void seg_regions_rgb(imgtype *img, Region *reg, Row *row, Corner *cor, Row **prow, Region **preg, uint32 w, uint32 h, uint32 theresh, uint32 corth,
		uint32 *nrows, uint32 *nregs, uint32 *npreg, uint32 *ncors);
void seg_regions(imgtype *img, imgtype *grad, Region *reg, Row *row, Corner *cor, Row **pro, Region **preg, uchar *dir, uint32 w, uint32 h, uint32 theresh, uint32 corth,
		uint32 *nrows, uint32 *nregs, uint32 *npreg, uint32 *ncors);
void seg_regions_draw(imgtype *img, Region *reg, uint32 nregs);
void seg_objects_draw(imgtype *img, Object *obj, uint32 nobjs, uint32 w);
void seg_regions_neighbor(Region *reg, Region **pnei, Region **preg, uint32 nreg, uint32 npreg);
void seg_objects(Object *obj, Region *reg,  Region **preg, uint32 nregs, uint32 *nobjs, uint32 theresh);
void seg_corners_draw(imgtype *img, Corner *cor, uint32 ncors, uint32 w);

void seg_color_quant(imgtype *img, imgtype *img1, uint32 w, uint32 h, uint32 quant);
void seg_coners_bayer(uchar *img, Corner *cor, uint32 w, uint32 h, uint32 theresh, uint32 *ncors);
void seg_coners_rgb(uchar *img, Corner *cor, uint32 w, uint32 h, uint32 theresh, uint32 *ncors);
void corners_draw(imgtype *img, Corner *cor, uint32 ncors, uint32 w);

void seg_morph_gradient(imgtype *img, imgtype *img1, uint32 w, uint32 h, uint32 thresh);
void seg_fall_forest(imgtype *img, imgtype *img1, uint32 w, uint32 h);
void seg_remove_pix(imgtype *img, imgtype *img1, uint32 w, uint32 h);
void seg_connect_pix(imgtype *img, imgtype *img1, uint32 w, uint32 h);

void seg_row_rgb	(imgtype *img, Row *row, Row **prow4, uint32 w, uint32 h, uint32 theresh, uint32 *nrows, uint32 *nprows);
void seg_row_rgb_draw	(imgtype *img, Row *row, uint32 nrows);
void seg_reg(Region *reg, Row *row, Row **prow, uint32 *nregs, uint32 nrows, uint32 theresh);
void seg_region_draw(imgtype *img, Region *reg, uint32 nregs);

void seg_corn_edge(imgtype *img, imgtype *img1, imgtype *img2,uint32 w, uint32 h, uint32 th);
void seg_grad(imgtype *img, imgtype *img1, imgtype *img2, uint32 w, uint32 h, uint32 th);
void seg_rain(imgtype *img, imgtype *img1, imgtype *img2, uint32 w, uint32 h, uint32 th);
void seg_remove(imgtype *img, imgtype *img2, uint32 w, uint32 h, uint32 th);
void seg_check_corner(imgtype *img, imgtype *img1, uint32 w, uint32 h);
void seg_canny(imgtype *img, imgtype *img1, imgtype *img2, uint32 w, uint32 h);
void seg_cluster( Pixel *pix, imgtype *img, imgtype *img1, uint32 w, uint32 h);
void seg_line(Pixel *pix, imgtype *img, imgtype *img1, uint32 w, uint32 h);


void seg_edges(Edge *edg, Pixel *pix, Edge **pedg, imgtype *img, uint32 w, uint32 h);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SEGMENTATION_H_ */