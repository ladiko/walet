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

void seg_regions(imgtype *img, Region *reg, Row *rows, Row **ptrc, Region	**preg, uint32 w, uint32 h, uint32 theresh, uint32 *nrows, uint32 *nregs, uint32 *npreg);
//void utils_reg_color(Region *reg, Row *rows, Row **prow, uint32 nregs);
void seg_regions_draw(imgtype *img, Region *reg, uint32 nregs, uint32 w);
void seg_objects_draw(imgtype *img, Object *obj, uint32 nobjs, uint32 w);
void seg_regions_neighbor(Region *reg, Region **pnei, Region **preg, uint32 nreg, uint32 npreg);
void seg_objects(Object *obj, Region *reg,  Region **preg, uint32 nregs, uint32 *nobjs, uint32 theresh);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* SEGMENTATION_H_ */
