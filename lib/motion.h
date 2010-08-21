/*
 * motion.h
 *
 *  Created on: 28.07.2010
 *      Author: vadim
 */
#ifndef MOTION_H_
#define MOTION_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void motion_contour(Image *im, ColorSpace color, uint32 steps, uchar *buf);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MOTION_H_ */
