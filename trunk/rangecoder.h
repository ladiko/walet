/*
 * range-coder.h
 *
 *  Created on: 19.02.2010
 *      Author: vadim
 */
#ifndef _RANGECODER_H_
#define _RANGECODER_H_

//#ifndef _WALET_TYPES_
//typedef unsigned int       	uint32;
//typedef short int			imgtype;
//typedef unsigned char       uchar;
//typedef unsigned long long 	uint64;
//#endif

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

uint32  range_encoder (imgtype *img, uint32 *d, uint32 size, uint32 a_bits , uint32 q_bits, uchar *buff);
uint32  range_decoder (imgtype *img, uint32 *d, uint32 size, uint32 a_bits , uint32 q_bits, uchar *buff);
//uint32 range_encoder(imgtype *img, uint32 *d, const uint32 size, const uint32 bits , uchar *buff);
//uint32 range_decoder(imgtype *img, uint32 *d, const uint32 size, const uint32 bits , uchar *buff);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RANGECODER */
