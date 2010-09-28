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

void seg_regions(imgtype *img, Region *reg, Row *row, Corner *cor, Row **prow, Region **preg, uint32 w, uint32 h, uint32 theresh, uint32 corth,
		uint32 *nrows, uint32 *nregs, uint32 *npreg, uint32 *ncors);
void seg_regions_draw(imgtype *img, Region *reg, uint32 nregs, uint32 w);
void seg_objects_draw(imgtype *img, Object *obj, uint32 nobjs, uint32 w);
void seg_regions_neighbor(Region *reg, Region **pnei, Region **preg, uint32 nreg, uint32 npreg);
void seg_objects(Object *obj, Region *reg,  Region **preg, uint32 nregs, uint32 *nobjs, uint32 theresh);
void seg_corners_draw(imgtype *img, Corner *cor, uint32 ncors, uint32 w);

void seg_color_quant(imgtype *img, imgtype *img1, uint32 w, uint32 h, uint32 quant);
void seg_coners_bayer(uchar *img, Corner *cor, uint32 w, uint32 h, uint32 theresh, uint32 *ncors);
void seg_coners_rgb(uchar *img, Corner *cor, uint32 w, uint32 h, uint32 theresh, uint32 *ncors);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SEGMENTATION_H_ */
