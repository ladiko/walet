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
		sub[i].dist = (uint32 *)calloc(1<<(bits+3), sizeof(uint32));
		sub[i].q = q;
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

uint32 subband_range_encoder(imgtype *img, uint32 *d, uint32 size, uint32 a_bits, uint32 q_bits, uchar *buff, int *q)
{
	return range_encoder(img, d, size, a_bits , q_bits, buff, q);
}

uint32  subband_range_decoder(imgtype *img, uint32 *d, uint32 size, uint32 a_bits, uint32 q_bits, uchar *buff, int *q)
{
	return range_decoder(img, d, size, a_bits , q_bits, buff, q);
}

uint32 subband_fill_prob(imgtype *img, uint32 size, uint32 *dist, uint32 d_bits)
/*! \fn void fill_dist()
	\brief 		Fill subband distution array.
*/
{
	int i,  ds = 1<<d_bits, half = ds>>1;
	int min, max, diff;
	memset(dist, 0, sizeof(uint32)*ds);
	for(i=0; i < size; i++) {
		dist[img[i] + half]++;
	}
	for(i=0   ; ; i++) if(dist[i] != 0) {min = i - half; break; }
	for(i=ds-1; ; i--) if(dist[i] != 0) {max = i - half; break; }
	//for(i=0; i< (1<<bits); i++) if(dist[i]) printf("dist[%d] = %d\n", i, dist[i]);
	diff = (max+min) > 0 ? max : -min;
	for(i=0; diff; i++) diff>>=1;
	printf("min = %d max = %d  tot = %d bits = %i\n", min, max, max-min, i+1);
	return i+1;
	//for(int i = 0; i<DIM; i++) if(dist[i]) printf("dist[%4d] = %8d\n",i - HALF, dist[i]);
}

double subband_entropy(uint32 *d, uint32 size, uint32 d_bits, uint32 a_bits, uint32 q_bits)
/// \fn double subband_entropy(uint32 *d, uint32 d_bits, uint32 a_bits, uint32 q_bits, uint32 size, uint32 *q)
/// \brief Calculate subband entropy.
/// \param d	 		The pointer to distution probabilities arrey.
/// \param size			The subband size.
/// \param d_bits 		The 1<<d_bits size of distution probabilities array.
/// \param a_bits		The 1<<a_bits actual size of distution probabilities.
/// \param q_bits		Bits for quantization.
/// \retval 			The subband entropy.

{
	// |--------|--------0--------|--------|
	// |           1<< dist_bits           |
	//          |   1<<act_bits   |
	//          |  |  |  |  |  |  |
	//     step  =  1<<(a_bits-q_bits)

	double e =0, tmp, e1;
	uint32  en=0;
	int i, j;
	int step = 1<<(a_bits-q_bits), rest = 1<<(a_bits-1), half = (1<<(d_bits-1));

	if(q_bits == 0){
		printf("q_bits should be more than 0\n");
		return 0;
	}

	for(j=(1-step); j< step; j++) en += d[half+j];
	if(en) {
		tmp = (double)en/(double)size;
		e -= tmp*log2(tmp);
		//e -= ((double)en/(double)size)*log2((double)en/(double)size);
	}

	for(i=step; i < rest; i+=step){
		en = 0;
		for(j= 0; j< step; j++) en += d[half+i+j];
		if(en) {
			tmp = (double)en/(double)size;
			e -= tmp*log2(tmp);
			//e -= ((double)en/(double)size)*log2((double)en/(double)size);
		}
		//printf("tot = %d i = %d rest = %d en = %d st = %d e = %f num = %d\n", tot, i, rest, en, st, ((double)en/(double)size)*log2((double)en/(double)size), num-i-j);
	}
	for(i=step; i < rest; i+=step){
		en=0;
		for(j= 0; j< step; j++) en += d[half-i-j];
		//if(i == rest-step) en += d[half-i-j];
		if(en) {
			tmp = (double)en/(double)size;
			e -= tmp*log2(tmp);
			//e -= ((double)en/(double)size)*log2((double)en/(double)size);
		}
		//printf("tot = %d i = %d rest = %d en = %d st = %d e = %f num = %d\n", tot, i, rest, en, st, ((double)en/(double)size)*log2((double)en/(double)size), num-i-j);
	}
	return e;
}

void  subband_encode_table(uint32 d_bits, uint32 a_bits, uint32 q_bits, uint32 *q)
/// \fn void  subband_quantization(imgtype *img,  uint32 size, uint32 *q, uint32 d_bits)
///	\brief Calculate distortion for the given uniform quantizer.
///	\param img			The pointer to subband
///	\param size			The number of pixels in subband
/// \param q	 		The pointer to quantization array.
/// \param d_bits 		The 1<<d_bits size of quantization array.
///
{
	int i, j;
	int del = a_bits-q_bits, step = 1<<del, rest = 1<<(a_bits-1), half = 1<<(d_bits-1);

	for(j=(1-step); j< step; j++) q[half+j] = 0;
	for(i=step; i < rest; i+=step) for(j= 0; j< step; j++) q[half+i+j] =  i>>del;
	for(i=step; i < rest; i+=step) for(j= 0; j< step; j++) q[half-i-j] = -(i>>del);
	q[half-i-j] = -(i>>del);

}

void  subband_decode_table(uint32 d_bits, uint32 a_bits, uint32 q_bits, uint32 *q)
/// \fn void  subband_quantization(imgtype *img,  uint32 size, uint32 *q, uint32 d_bits)
///	\brief Calculate distortion for the given uniform quantizer.
///	\param img			The pointer to subband
///	\param size			The number of pixels in subband
/// \param q	 		The pointer to quantization array.
/// \param d_bits 		The 1<<d_bits size of quantization array.
///
{
	int i, j;
	int del = a_bits-q_bits, step = 1<<del, rest = 1<<(a_bits-1), half = 1<<(d_bits-1), val = step>>1;;

	for(j=(1-step); j< step; j++) q[half+j] = 0;
	for(i=step; i < rest; i+=step) for(j= 0; j< step; j++) q[half+i+j] =  i+val;
	for(i=step; i < rest; i+=step) for(j= 0; j< step; j++) q[half-i-j] = -i-val;
	q[half-i-j] = -i-val;

}

void  subband_quantization(imgtype *img,  uint32 size, uint32 d_bits, uint32 a_bits, uint32 q_bits, int *q)
/// \fn void  subband_quantization(imgtype *img,  uint32 size, uint32 *q, uint32 d_bits)
///	\brief Calculate distortion for the given uniform quantizer.
///	\param img			The pointer to subband
///	\param size			The number of pixels in subband
/// \param q	 		The pointer to quantization array.
/// \param d_bits 		The 1<<d_bits size of quantization array.
///
{
	int i, half = 1<<(d_bits-1);
	//subband_decode_table(d_bits, a_bits, q_bits, q);
	for(i=0; i < size; i++ ) img[i] = q[img[i] + half];
	//printf("min = %d max = %d  tot = %d bits = %d step = %d range = %d\n", min, max, max-min, i+1, step, range);
	//return i+1;
}

