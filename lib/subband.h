#ifndef _SUBBAND_H_
#define _SUBBAND_H_
/*
typedef struct {
	Dim size;		//Subband size
	uint32 loc;			//Subband location in the buffer
	uint32 *dist;		//Distribution probabilities array
	uint32 d_bits;		//The max bit per pixel after DWT
	uint32 a_bits;		//Real bits per pixels
	uint32 q_bits;		//Quantization bits per pixel
	int *q;				//Quantization value array
	uint32 max;
} Subband;
*/
typedef struct {	// Signed picture 16bit
	uint16 	w;
	uint16 	h;
	short int 	*pic;
	//int *q;				//Quantization value array.
	uint32 	*dist;		//Distribution probabilities array.
	uint32 	d_bits;		//The max bit per pixel after DWT.
	uint32 	a_bits;		//Real bits per pixels.
	uint32 	q_bits;		//Quantization bits per pixel.
	uint32 	max;		//The absolute value of the maximum.
	uint32	ssz;		//The subband size after range coder compression.
	uint32 	ssq;		//The subband size for given q_step
	uint32 	ssd;		//The subband size difference for given q_step and q_step-1
	double	ssim;
} Subband;

typedef struct {	//One DWT decomposition level
	Subband s[4];	//Subband array s[0] - LL, s[1] - HL, s[2] - LH, s[3] - HH
} Level;

typedef void (*QI)(Subband *);

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void 	subband_init		(Subband *sub, int16 *img, uint32 w, uint32 h, uint32 bpp);
void 	subband_fill_prob	(Subband *sub);
uint32 	subband_size		(Subband *sub);
void  	subband_encode_table(Subband *sub, int *q);
void  	subband_decode_table(Subband *sub, int *q);
void  	subband_quantization(Subband *sub, int *q);
void  	subb_copy			(Subband *in, Subband *out);

//Old interface
//void 	subband_init			(Subband **sub, uint32 num, ColorSpace color, uint32 x, uint32 y, uint32 steps, uint32 bits, int *q);

//void 	subband_fill_prob		(uint8 *img, Subband *sub);

//void  	subband_encode_table	(Subband *sub, QI q_i);
//void  	subband_decode_table	(Subband *sub, QI q_i);
//void  	subband_quantization	(uint8 *img, Subband *sub, QI q_i);

//void 	q_i_uniform(Subband *sub);
//void 	q_i_nonuniform(Subband *sub);
//void 	q_i_nonuniform1(Subband *sub);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //_SUBBAND_HH_
