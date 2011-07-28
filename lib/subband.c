#include <walet.h>
//#include <range-coder.h>

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <utils.h>


void subband_init(Subband *sub, int16 *img, uint32 w, uint32 h, uint32 bpp)
{
	sub->d_bits = bpp+2;
	sub->dist = (uint32 *)calloc((1<<sub->d_bits)*3, sizeof(uint32));
	sub->q = (int *)calloc(1<<sub->d_bits, sizeof(int));;
	sub->pic = img;
	sub->w = w;
	sub->h = h;
}

void subband_fill_prob(Subband *sub)
///	\fn void subband_fill_prob(uint8 *img, Subband *sub)
///	\brief Fill distribution probability arrays.
///	\param img	 		The pointer to image.
///	\param sub 			Pointer to filling subband.
{
	int i, ds = 1<<sub->d_bits, half = ds>>1;
	int min, max, tmp, size = sub->w*sub->h;

	memset(sub->dist, 0, sizeof(uint32)*ds);

	for(i=0; i < size; i++) {
		sub->dist[sub->pic[i] + half]++;
	}
	for(i=0   ; ; i++) if(sub->dist[i] != 0) {min = i - half; break; }
	for(i=ds-1; ; i--) if(sub->dist[i] != 0) {max = i - half; break; }
	//for(i=0; i< (1<<bits); i++) if(dist[i]) printf("dist[%d] = %d\n", i, dist[i]);
	sub->max = (max + min) > 0 ? max : -min;
	tmp = sub->max;
	for(i=0; tmp; i++, tmp>>=1);
	printf("min = %d max = %d  tot = %d bits = %i\n", min, max, max-min, i+1);
	sub->a_bits = i+1;
	sub->q_bits = sub->a_bits;
}
/*
void q_i_uniform(Subband *sub)
{
	int i, del = sub->a_bits - sub->q_bits, step = 1<<del, range = 1<<(sub->q_bits-1);
	int *in = &sub->q[1<<sub->d_bits];
	in[0] = step;
	for(i=1; i < range; i++) {
		in[i] = in[i-1] + step;
	}
}

void q_i_nonuniform(Subband *sub)
{
	int i, del = sub->a_bits - sub->q_bits, step = 1<<del, range = 1<<(sub->q_bits-1);
	int *in = &sub->q[1<<sub->d_bits];
	step = 1 + del;
	in[0] = (1<<sub->a_bits-1) - step*(range-1);
	//printf("in[%d] = %d ", 0, in[0]);
	for(i=1; i < range; i++) {
		in[i] = in[i-1] + step;
		//printf("in[%d] = %d ", i, in[i]);
	}
}

void q_i_nonuniform1(Subband *sub)
{
	int i, del, step, range;
	int *in = &sub->q[1<<sub->d_bits];
	int tmp = (1<<sub->a_bits-1) - sub->a_bits +1;
	if(tmp > sub->max && sub->a_bits != sub->q_bits){
		range = 1<<(sub->q_bits-2);
		del = sub->a_bits -1 - sub->q_bits;
		step = (1<<del) + del;
		in[0] = (1<<sub->a_bits-2) - step*(range-1);
		for(i = (1<<sub->a_bits-2); i < (1<<sub->a_bits-1); i++) in[i] = (1<<sub->a_bits-2)-1;
	} else {
		range = 1<<(sub->q_bits-1);
		del = sub->a_bits - sub->q_bits;
		step = (1<<del) + del;
		in[0] = (1<<sub->a_bits-1) - step*(range-1);
	}
	//printf("a_bits = %d q_bits = %d del = %d step = %d range = %d\n", sub->a_bits, sub->q_bits, del, step, range);
	//printf("in[%d] = %d ", 0, in[0]);
	for(i=1; i < range; i++) {
		in[i] = in[i-1] + step;
		//printf("in[%d] = %d ", i, in[i]);
	}
	//printf("\n");
}
*/

uint32 subband_size(Subband *sub)
/// \fn uint32 subband_size(Subband *sub)
/// \brief Calculate subband size after quantization.
///	\param sub 			Pointer to subband.
/// \retval 			The subband size after quantization.
{
	// |--------|--------0--------|--------|
	// |           1<< dist_bits           |
	//          |   1<<act_bits   |
	//          |  |  |  |  |  |  |
	//     step  =  1<<(a_bits-q_bits)

	double s = 0., s0 = log2(sub->h*sub->w);
	uint32  en=0;
	int i, j, st = 1 <<(sub->a_bits - sub->q_bits);
	int rest = 1<<(sub->q_bits-1), half = (1<<(sub->d_bits-1));;
	//int *in = &sub->q[1<<sub->d_bits];

	//q_i(sub);
	if(sub->q_bits == 0){
		printf("q_bits should be more than 0\n");
		return 0;
	}

	for(j=(1-(1<<st)); j< (1<<st); j++) en += sub->dist[half+j];
	if(en) s -= en*(log2(en) - s0);

	for(i=1; i < rest-1; i++){
		en = 0;
		for(j= (i<<st); j< ((i+1)<<st); j++) en += sub->dist[half+j];
		if(en) s -= en*(log2(en) - s0);
		en=0;
		for(j= (i<<st); j< ((i+1)<<st); j++) en += sub->dist[half-j];
		if(en) s -= en*(log2(en) - s0);
	}
	return (uint32)s;
}

void  subband_encode_table(Subband *sub)
/// \fn void  subband_encode_table(Subband *sub, int *q)
///	\brief Make quantization array for encoder.
///	\param sub 			Pointer to subband.
/// \param q			The quantization array.
{
	int i, j, st = 1 <<(sub->a_bits - sub->q_bits);
	int range = 1<<(sub->a_bits-1), half = 1<<(sub->q_bits-1);;
	//int *in = &sub->q[1<<sub->d_bits];

	//q_i(sub);

	for(j=(1-(1<<st)); j< (1<<st); j++) sub->q[range+j] = half;
	for(i=0; i < half-1; i++){
		for(j= (i<<st); j< ((i+1)<<st); j++) {
			sub->q[range+j] = half + i + 1;
			sub->q[range-j] = half - i - 1;
		}
	}
	sub->q[range-j] = sub->q[range-j+1];
}

void  subband_decode_table(Subband *sub)
/// \fn void  subband_decode_table(Subband *sub, int *q)
///	\brief Make quantization array for decoder.
///	\param sub 			Pointer to subband.
/// \param q			The quantization array.
{
	int j, st = 1 <<(sub->a_bits - sub->q_bits);
	int  half = 1<<(sub->q_bits-1);
	//int *in = &sub->q[1<<sub->d_bits];

	q_i(sub);

	sub->q[half] = 0;
	for(j=0; j < half; j++) {
		//sub->q[half+j+1] =  ( in[j] + in[j+1])>>1;
		//sub->q[half-j-1] = -((in[j] + in[j+1])>>1);
		sub->q[half+j+1] =  ((j<<st) + ((j+1)<<st))>>1;
		sub->q[half-j-1] = -(((j<<st) + ((j+1)<<st))>>1);
	}
	sub->q[half-j] = sub->q[half-j+1];
}

void  subband_quantization(Subband *sub)
/// \fn void  subband_quantization(uint8 *img, Subband *sub)
///	\brief Subband quantization.
///	\param img			The pointer to subband
///	\param sub 			Pointer to subband.

{
	int i, j, size = sub->w*sub->h, st = 1 <<(sub->a_bits - sub->q_bits);
	int range = 1<<(sub->q_bits-1), half = 1<<(sub->a_bits-1);
	//int *in = &sub->q[1<<sub->d_bits];
	//printf("d_bits = %d\n", 1<<sub->d_bits);

	if(sub->a_bits != sub->q_bits){
		if(sub->q_bits > 1) {
			//q_i(sub);

			for(j=(1-(1<<st)); j< (1<<st); j++) sub->q[half+j] = 0;
			for(i=0; i < range-1; i++){
				for(j= (i<<st); j< ((i+1)<<st); j++) {
					sub->q[half+j] =  ((j<<st) + ((j+1)<<st))>>1;
					sub->q[half-j] = -(((j<<st) + ((j+1)<<st))>>1);
					//if(half+j >=1024 || half-j < 0) printf("half = %d j = %d in[%d] = %d in[%d] = %d \n", half, j, i, in[i], i+1, in[i+1]);
				}
			}
			sub->q[half-j] = sub->q[half-j+1];
			for(i=0; i < size; i++ ) sub->pic[i] = sub->q[sub->pic[i] + half];
		} else for(i=0; i < size; i++ ) sub->pic[i] = 0;
	}
}

