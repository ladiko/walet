#include <walet.h>
//#include <range-coder.h>

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <utils.h>

static void subband_ini(Subband *sub, uint32 x, uint32 y, uint32 steps, uint32 bits, uint32 ofset, int *q)
{
	uint32  i, st = steps*3+1;
	uint32  s[4], sh[4], h[2], w[2];
	w[0] = x; h[0] = y;

	for(i=steps; i>0; i--){
		h[1] = (h[0]>>1), h[0] = (h[0]>>1) + (h[0]&1), w[1] = (w[0]>>1), w[0] = (w[0]>>1) + (w[0]&1);
		s[0] = ofset; s[1] = s[0] + w[0]*h[0]; s[2] = s[1] + w[1]*h[0]; s[3] = s[2] + w[0]*h[1];

		sub[3*i  ].size.x = w[1]; sub[3*i  ].size.y = h[1]; sub[3*i  ].loc = s[3];
		sub[3*i-1].size.x = w[0]; sub[3*i-1].size.y = h[1]; sub[3*i-1].loc = s[2];
		sub[3*i-2].size.x = w[1]; sub[3*i-2].size.y = h[0]; sub[3*i-2].loc = s[1];
	}
	sub[0].size.x = w[0]; sub[0].size.y = h[0]; sub[0].loc = ofset;
	for(i=0; i<st; i++){
		sub[i].dist = (uint32 *)calloc((1<<(bits+2))*3, sizeof(uint32));
		sub[i].q = q;
		sub[i].d_bits = bits+2;
	}
	//printf("sub %d h %d w %d s %d p %p\n",sub[0][0]->subb, sub[0][0]->size.y, sub[0][0]->size.x, s[0], sub[0][0]);
}

void subband_init(Subband **sub, uint32 num, ColorSpace color, uint32 x, uint32 y, uint32 steps, uint32 bits, int *q)
{
	uint32  j, k,st;
	uint32  s[4], h[2], w[2];

	if(color == BAYER){
		if(steps == 1){
			sub[num] = (Subband *)calloc(4, sizeof(Subband));
			subband_ini(sub[num], x, y, steps, bits, 0, q);
		} else {
			h[0] = (y>>1) + (y&1), h[1] = (y>>1), w[0] = (x>>1) + (x&1), w[1] = (x>>1);
			//printf("x = %d y = %d h[0] = %d h[1] = %d w[0] = %d w[1] = %d\n", x, y, h[0], h[1], w[0], w[1]);
			s[0] = 0; s[1] = s[0] + w[0]*h[0]; s[2] = s[1] + w[1]*h[0]; s[3] = s[2] + w[0]*h[1];

			st = ((steps-1)*3+1);
			sub[num] = (Subband *)calloc(st<<2, sizeof(Subband));
			subband_ini(&sub[num][0   ], w[0], h[0], steps-1, bits, s[0], q);
			subband_ini(&sub[num][st  ], w[1], h[0], steps-1, bits, s[1], q);
			subband_ini(&sub[num][st*2], w[0], h[1], steps-1, bits, s[2], q);
			subband_ini(&sub[num][st*3], w[1], h[1], steps-1, bits, s[3], q);
			//printf("sub = %p\n", sub[num]);
		}
	} else {
		sub[num] = (Subband *)calloc(steps*3+1, sizeof(Subband));
		subband_ini(sub[num], x, y, steps, bits, 0, q);
	}

	//for(k=0; k < 4; k++) for(j=0; j < (steps-1)*3+1; j++)
	//	printf("i = %2d j = %2d loc = %8d size.x = %4d  size.y = %4d dist = %p\n",
	//			k, j, sub[num][j+((steps-1)*3+1)*k].loc, sub[num][j+((steps-1)*3+1)*k].size.x, sub[num][j+((steps-1)*3+1)*k].size.y, &sub[num][j+((steps-1)*3+1)*k]);
}

void subband_fill_prob(imgtype *img, Subband *sub)
///	\fn void subband_fill_prob(imgtype *img, Subband *sub)
///	\brief Fill distribution probability arrays.
///	\param img	 		The pointer to image.
///	\param sub 			Pointer to filling subband.
{
	int i,  ds = 1<<sub->d_bits, half = ds>>1;
	int min, max, diff, size = sub->size.x*sub->size.y;

	memset(sub->dist, 0, sizeof(uint32)*ds);

	for(i=0; i < size; i++) {
		sub->dist[img[i] + half]++;
	}
	for(i=0   ; ; i++) if(sub->dist[i] != 0) {min = i - half; break; }
	for(i=ds-1; ; i--) if(sub->dist[i] != 0) {max = i - half; break; }
	//for(i=0; i< (1<<bits); i++) if(dist[i]) printf("dist[%d] = %d\n", i, dist[i]);
	diff = (max+min) > 0 ? max : -min;
	for(i=0; diff; i++) diff>>=1;
	printf("min = %d max = %d  tot = %d bits = %i\n", min, max, max-min, i+1);
	sub->a_bits = i+1;
	sub->q_bits = sub->a_bits;
	//sub->min = min;
	sub->max = max > -min ? max : -min;
	//return i+1;
	//for(int i = 0; i<DIM; i++) if(dist[i]) printf("dist[%4d] = %8d\n",i - HALF, dist[i]);
}

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

uint32 subband_size(Subband *sub, QI q_i)
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

	double s = 0., s0 = log2(sub->size.x*sub->size.y);
	uint32  en=0;
	int i, j;
	int rest = 1<<(sub->q_bits-1), half = (1<<(sub->d_bits-1));;
	int *in = &sub->q[1<<sub->d_bits];

	q_i(sub);

	if(sub->q_bits == 0){
		printf("q_bits should be more than 0\n");
		return 0;
	}

	for(j=(1-in[0]); j< in[0]; j++) en += sub->dist[half+j];
	if(en) s -= en*(log2(en) - s0);

	for(i=0; i < rest-1; i++){
		en = 0;
		for(j= in[i]; j< in[i+1]; j++) en += sub->dist[half+j];
		if(en) s -= en*(log2(en) - s0);
		en=0;
		for(j= in[i]; j< in[i+1]; j++) en += sub->dist[half-j];
		if(en) s -= en*(log2(en) - s0);
	}
	return (uint32)s;
}

void  subband_encode_table(Subband *sub, QI q_i)
/// \fn void  subband_encode_table(Subband *sub, int *q)
///	\brief Make quantization array for encoder.
///	\param sub 			Pointer to subband.
/// \param q			The quantization array.
{
	int i, j;
	int range = 1<<(sub->a_bits-1), half = 1<<(sub->q_bits-1);;
	int *in = &sub->q[1<<sub->d_bits];

	q_i(sub);

	for(j=(1-in[0]); j< in[0]; j++) sub->q[range+j] = half;
	for(i=0; i < half-1; i++){
		for(j= in[i]; j< in[i+1]; j++) {
			sub->q[range+j] = half + i + 1;
			sub->q[range-j] = half - i - 1;
		}
	}
	sub->q[range-j] = sub->q[range-j+1];
}

void  subband_decode_table(Subband *sub, QI q_i)
/// \fn void  subband_decode_table(Subband *sub, int *q)
///	\brief Make quantization array for decoder.
///	\param sub 			Pointer to subband.
/// \param q			The quantization array.
{
	int j;
	int  half = 1<<(sub->q_bits-1);
	int *in = &sub->q[1<<sub->d_bits];

	q_i(sub);

	sub->q[half] = 0;
	for(j=0; j < half; j++) {
		sub->q[half+j+1] =  ( in[j] + in[j+1])>>1;
		sub->q[half-j-1] = -((in[j] + in[j+1])>>1);
	}
	sub->q[half-j] = sub->q[half-j+1];
}

void  subband_quantization(imgtype *img, Subband *sub, QI q_i)
/// \fn void  subband_quantization(imgtype *img, Subband *sub)
///	\brief Subband quantization.
///	\param img			The pointer to subband
///	\param sub 			Pointer to subband.

{
	int i, j, size = sub->size.x*sub->size.y;
	int range = 1<<(sub->q_bits-1), half = 1<<(sub->a_bits-1);
	int *in = &sub->q[1<<sub->d_bits];
	//printf("d_bits = %d\n", 1<<sub->d_bits);

	if(sub->a_bits != sub->q_bits){
		if(sub->q_bits > 1) {
			q_i(sub);

			for(j=(1-in[0]); j < in[0]; j++) sub->q[half+j] = 0;
			for(i=0; i < range-1; i++){
				for(j= in[i]; j< in[i+1]; j++) {
					sub->q[half+j] =  ( in[i] + in[i+1])>>1;
					sub->q[half-j] = -((in[i] + in[i+1])>>1);
					//if(half+j >=1024 || half-j < 0) printf("half = %d j = %d in[%d] = %d in[%d] = %d \n", half, j, i, in[i], i+1, in[i+1]);
				}
			}
			sub->q[half-j] = sub->q[half-j+1];
			for(i=0; i < size; i++ ) img[i] = sub->q[img[i] + half];
		} else for(i=0; i < size; i++ ) img[i] = 0;
	}
}

