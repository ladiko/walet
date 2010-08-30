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

void utils_2d_reg_seg(imgtype *img, Region *reg, Row *rows, Row **ptrc, Region	**preg, uint32 w, uint32 h, uint32 theresh, uint32 *nrows, uint32 *nregs );
void utils_rows_in_reg(Region *reg, Row *rows, Row **prow, uint32 nrows);
void utils_reg_color(Region *reg, Row *rows, Row **prow, uint32 nregs);
void utils_region_draw(imgtype *img, Region *reg, uint32 nregs, uint32 w);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* SEGMENTATION_H_ */
