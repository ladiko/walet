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

void utils_row_seg(imgtype *img, Row *rows, uint32 *col, uint32 w, uint32 h, uint32 theresh);
uint32 utils_region_seg(Region *reg, Row *rows, uint32 *col, uint32 w, uint32 h, uint32 theresh);
void utils_row_draw(imgtype *img, Row *rows, uint32 *col, uint32 w, uint32 h);
void utils_region_fill(Region *reg, Row *rows, Row **pr, uint32 *col, uint32 w, uint32 h);
void utils_region_draw1(imgtype *img, Region *reg, uint32 nreg, uint32 w, uint32 h);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* SEGMENTATION_H_ */
