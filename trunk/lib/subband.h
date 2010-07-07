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
	//int min;
	uint32 max;
	//int *in;				//Quantization value array
} Subband;

typedef void (*QI)(Subband *);

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void 	subband_init			(Subband **sub, uint32 num, ColorSpace color, uint32 x, uint32 y, uint32 steps, uint32 bits, int *q);

void 	subband_fill_prob		(imgtype *img, Subband *sub);
uint32 	subband_size			(Subband *sub, QI q_i);
void  	subband_encode_table	(Subband *sub, QI q_i);
void  	subband_decode_table	(Subband *sub, QI q_i);
void  	subband_quantization	(imgtype *img, Subband *sub, QI q_i);

void 	q_i_uniform(Subband *sub);
void 	q_i_nonuniform(Subband *sub);
void 	q_i_nonuniform1(Subband *sub);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //_SUBBAND_HH_
