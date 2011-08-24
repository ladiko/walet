#ifndef _RANGECODER_H_
#define _RANGECODER_H_

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

uint32  range_encoder_ad	(int16 *img, uint32 size, uint32 a_bits , uint32 q_bits, uint8 *buff, int *q, uint32 *d);
uint32  range_decoder_ad	(int16 *img, uint32 size, uint32 a_bits , uint32 q_bits, uint8 *buff, int *q, uint32 *buff1);
uint32  range_encoder		(int16 *img, uint32 size, uint32 a_bits , uint32 q_bits, uint8 *buff, int *q, uint32 *d, uint32 *buff1);
uint32  range_decoder		(int16 *img, uint32 size, uint32 a_bits , uint32 q_bits, uint8 *buff, int *q, uint32 *buff1);
uint32  range_encoder_fast	(int16 *img, uint32 size, uint32 a_bits , uint32 q_bits, uint8 *buff, int *q, uint32 *d, uint32 *buff1);
uint32  range_decoder_fast	(int16 *img, uint32 size, uint32 a_bits , uint32 q_bits, uint8 *buff, int *q, uint32 *buff1);
uint32  write_array			(int16 *img, uint32 size, uint32 bits, uint8 *buff, int *q);
uint32  read_array			(int16 *img, uint32 size, uint32 bits, uint8 *buff, int *q);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RANGECODER */
