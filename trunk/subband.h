#ifndef _SUBBAND_H_
#define _SUBBAND_H_

typedef struct {
	Vector size;		//Subband size
	uint32 loc;			//Subband location in the buffer
	uint32 *dist;	//Distribution probabilities array
	uint32 bits;		//Real bits per pixels
	uint32 q_bits;		//Quantization bits
	int *q;				//Quatntization value array
} Subband;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void 	subband_init_bayer		(Subband **sub, uint32 x, uint32 y, uint32 steps, uint32 bits, int *q);
void 	subband_init			(Subband **sub, uint32 num, uint32 x, uint32 y, uint32 steps, uint32 bits, int *q);
uint32 	subband_range_encoder	(imgtype *img, uint32 *d, uint32 size, uint32 a_bits, uint32 q_bits, uchar *buff, int *q);
uint32  subband_range_decoder	(imgtype *img, uint32 *d, uint32 size, uint32 a_bits, uint32 q_bits, uchar *buff, int *q);
//uint32 	subband_range_encoder	(imgtype *img, uint32 *d, uint32 size, uint32 bits , uchar *buff);
//uint32	subband_range_decoder	(imgtype *img, uint32 *d, uint32 size, uint32 bits , uchar *buff);
uint32 	subband_fill_prob		(imgtype *img, uint32 size, uint32 *distrib, uint32 bits);
//void 	subband_dist_entr		(uint32 *distrib, uint32 dist_size, uint32 step, uint32 size, double *dis, double *e);
double 	subband_entropy			(uint32 *d,    uint32 size, uint32 d_bits, uint32 a_bits, uint32 q_bits);
void  	subband_quantization	(imgtype *img, uint32 size, uint32 d_bits, uint32 a_bits, uint32 q_bits, int *q);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //_SUBBAND_HH_
