#ifndef _SUBBAND_H_
#define _SUBBAND_H_

typedef struct {
	Vector size;		//Subband size
	uint32 loc;			//Subband location in the buffer
	uint32 *dist;		//Distribution probabilities array
	uint32 d_bits;		//The max bit per pixel after DWT
	uint32 a_bits;		//Real bits per pixels
	uint32 q_bits;		//Quantization bits per pixel
	int *q;				//Quantization value array
} Subband;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void 	subband_init			(Subband **sub, uint32 num, ColorSpace color, uint32 x, uint32 y, uint32 steps, uint32 bits, int *q);

uint32 	subband_range_encoder	(imgtype *img, uint32 *d, uint32 size, uint32 a_bits, uint32 q_bits, uchar *buff, int *q);
uint32  subband_range_decoder	(imgtype *img, uint32 *d, uint32 size, uint32 a_bits, uint32 q_bits, uchar *buff, int *q);
//uint32 	subband_range_encoder	(imgtype *img, uint32 *d, uint32 size, uint32 bits , uchar *buff);
//uint32	subband_range_decoder	(imgtype *img, uint32 *d, uint32 size, uint32 bits , uchar *buff);
void 	subband_fill_prob		(imgtype *img, Subband *sub);
uint32 	subband_size			(Subband *sub);
void  	subband_encode_table	(Subband *sub);
void  	subband_decode_table	(Subband *sub);
//void 	subband_dist_entr		(uint32 *distrib, uint32 dist_size, uint32 step, uint32 size, double *dis, double *e);
//double 	subband_entropy			(uint32 *d,    uint32 size, uint32 d_bits, uint32 a_bits, uint32 q_bits);
void  	subband_quantization	(imgtype *img, Subband *sub);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //_SUBBAND_HH_
