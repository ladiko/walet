#include <walet.h>
//#include <range-coder.h>

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <utils.h>

void subband_init_bayer(Subband **sub, uint32 x, uint32 y, uint32 steps, uint32 bits)
{
	uint32  i, k;
	uint32  s[4], sh[4], h[2], w[2], h0, w0, h1, w1, s0;
	w[0] = x; h[0] = y;

	if(steps == 1){
		h[1] = (h[0]>>1), h[0] = (h[0]>>1) + h[0]%2, w[1] = (w[0]>>1), w[0] = (w[0]>>1) + w[0]%2;
		s[0] = 0; s[1] = w[0]*h[0]; s[2] = s[1] + w[1]*h[0]; s[3] = s[2] + w[0]*h[1];

		for(k=0; k<4; k++) {
			sub[k] = (Subband *)calloc(1, sizeof(Subband));
			sub[k][0].distrib = (uint32 *)calloc(1<<(bits+3), sizeof(uint32));
			sub[k][0].q = (int *)calloc(1<<(bits+2), sizeof(int));
		}

		sub[0][0].size.x = w[0]; sub[0][0].size.y = h[0]; sub[0][0].loc = s[0];
		sub[1][0].size.x = w[1]; sub[1][0].size.y = h[0]; sub[1][0].loc = s[1];
		sub[2][0].size.x = w[0]; sub[2][0].size.y = h[1]; sub[2][0].loc = s[2];
		sub[3][0].size.x = w[1]; sub[3][0].size.y = h[1]; sub[3][0].loc = s[3];
	} else {
		h1 = (h[0]>>1), h0 = (h[0]>>1) + h[0]%2, w1 = (w[0]>>1), w0 = (w[0]>>1) + w[0]%2;
		sh[0] = 0; sh[1] = w0*h0; sh[2] = sh[1] + w1*h0; sh[3] = sh[2] + w0*h1;
		for(k=0; k<4; k++) {
			sub[k] = (Subband *)calloc((steps-1)*3+1, sizeof(Subband));
			switch(k){
				case 0 : {h[0] = h0; w[0] = w0; break;}
				case 1 : {h[0] = h0; w[0] = w1; break;}
				case 2 : {h[0] = h1; w[0] = w0; break;}
				case 3 : {h[0] = h1; w[0] = w1; break;}
			}
			for(i=(steps-1); i>0; i--){
				h[1] = (h[0]>>1), h[0] = (h[0]>>1) + h[0]%2, w[1] = (w[0]>>1), w[0] = (w[0]>>1) + w[0]%2;
				s[0] = 0; s[1] = w[0]*h[0]; s[2] = s[1] + w[1]*h[0]; s[3] = s[2] + w[0]*h[1];

				sub[k][3*i  ].size.x = w[1]; sub[k][3*i  ].size.y = h[1]; sub[k][3*i  ].loc = sh[k]+s[3];
				sub[k][3*i-1].size.x = w[0]; sub[k][3*i-1].size.y = h[1]; sub[k][3*i-1].loc = sh[k]+s[2];
				sub[k][3*i-2].size.x = w[1]; sub[k][3*i-2].size.y = h[0]; sub[k][3*i-2].loc = sh[k]+s[1];
				sub[k][3*i  ].distrib = (uint32 *)calloc(1<<(bits+3), sizeof(uint32));
				sub[k][3*i-1].distrib = (uint32 *)calloc(1<<(bits+3), sizeof(uint32));
				sub[k][3*i-2].distrib = (uint32 *)calloc(1<<(bits+3), sizeof(uint32));
				sub[k][3*i  ].q = (int *)calloc(1<<(bits+2), sizeof(int));
				sub[k][3*i-1].q = (int *)calloc(1<<(bits+2), sizeof(int));
				sub[k][3*i-2].q = (int *)calloc(1<<(bits+2), sizeof(int));
							}
			sub[k][0].size.x = w[0]; sub[k][0].size.y = h[0]; sub[k][0].loc = sh[k];
			sub[k][0].distrib = (uint32 *)calloc(1<<(bits+3), sizeof(uint32));
			sub[k][0].q = (int *)calloc(1<<(bits+2), sizeof(int));
		}
	}
	//int j;
	//for(i=0; i < 4; i++) for(j=0; j < (steps-1)*3+1; j++)
	//	printf("i = %d j = %d loc = %d size.x = %d  size.y = %d distrib = %p\n", i, j, sub[i][j].loc, sub[i][j].size.x, sub[i][j].size.y, sub[i][j].distrib);

}


void subband_init(Subband *sub, uint32 x, uint32 y, uint32 steps, uint32 bits)
{
	uint32  i;
	uint32  s[4], sh[4], h[2], w[2];
	w[0] = x; h[0] = y;
	printf("x = %d y = %d steps = %d\n", x, y, steps);
	//sub = (Subband **)calloc(1, sizeof(Subband*));
	sub = (Subband *)calloc((steps-1)*3+1, sizeof(Subband));
	//sub[0] = new Subband*[steps*3+1];
	printf("x = %d y = %d steps = %d\n", x, y, steps);
	for(i=steps; i>0; i--){
		h[1] = (h[0]>>1), h[0] = (h[0]>>1) + h[0]%2, w[1] = (w[0]>>1), w[0] = (w[0]>>1) + w[0]%2;
		s[0] = 0; s[1] = w[0]*h[0]; s[2] = s[1] + w[1]*h[0]; s[3] = s[2] + w[0]*h[1];

		sub[3*i  ].size.x = w[1]; sub[3*i  ].size.y = h[1]; sub[3*i  ].loc = s[3];
		sub[3*i-1].size.x = w[0]; sub[3*i-1].size.y = h[1]; sub[3*i-1].loc = s[2];
		sub[3*i-2].size.x = w[1]; sub[3*i-2].size.y = h[0]; sub[3*i-2].loc = s[1];
		sub[3*i  ].distrib = (uint32 *)calloc(1<<(bits+3), sizeof(uint32));
		sub[3*i-1].distrib = (uint32 *)calloc(1<<(bits+3), sizeof(uint32));
		sub[3*i-2].distrib = (uint32 *)calloc(1<<(bits+3), sizeof(uint32));
		//sub[0][3*i  ] = new Subband(&img[s[3]], h[1], w[1], 3*i  , (uchar *)dwt_buf);
		//sub[0][3*i-1] = new Subband(&img[s[2]], h[1], w[0], 3*i-1, (uchar *)dwt_buf);
		//sub[0][3*i-2] = new Subband(&img[s[1]], h[0], w[1], 3*i-2, (uchar *)dwt_buf);
		//printf("sub %d h %d w %d s %d p %p\n",sub[0][3*i  ]->subb, sub[0][3*i  ]->size.y, sub[0][3*i  ]->size.x, s[1], sub[0][3*i  ]);
		//printf("sub %d h %d w %d s %d p %p\n",sub[0][3*i-1]->subb, sub[0][3*i-1]->size.y, sub[0][3*i-1]->size.x, s[2], sub[0][3*i-1]);
		//printf("sub %d h %d w %d s %d p %p\n",sub[0][3*i-2]->subb, sub[0][3*i-2]->size.y, sub[0][3*i-2]->size.x, s[3], sub[0][3*i-2]);
	}
	sub[0].size.x = w[0]; sub[0].size.y = h[0]; sub[0].loc = 0;
	sub[0].distrib = (uint32 *)calloc(1<<(bits+3), sizeof(uint32));
	sub[0].q = (int *)calloc(1<<(bits+2), sizeof(int));
	//sub[0][0] = new Subband(img, h[0], w[0], 0, (uchar *)dwt_buf);
	//printf("sub %d h %d w %d s %d p %p\n",sub[0][0]->subb, sub[0][0]->size.y, sub[0][0]->size.x, s[0], sub[0][0]);
}

uint32 subband_range_encoder(imgtype *img, uint32 *d, uint32 size, uint32 a_bits, uint32 q_bits, uchar *buff)
{
	return range_encoder(img, d, size, a_bits , q_bits, buff);
}

uint32  subband_range_decoder(imgtype *img, uint32 *d, uint32 size, uint32 a_bits, uint32 q_bits, uchar *buff)
{
	return range_decoder(img, d, size, a_bits , q_bits, buff);
}

uint32 subband_fill_prob(imgtype *img, uint32 size, uint32 *distrib, uint32 d_bits)
/*! \fn void fill_distrib()
	\brief 		Fill subband distribution array.
*/
{
	uint32 i, half = (1<<(d_bits-1));
	int min = img[0], max = img[0], diff;
	memset(distrib, 0, sizeof(uint32)*(1<<d_bits));
	for(i=0; i < size; i++) {
		distrib[img[i] + half]++;
		if(img[i] > max) max = img[i];
		if(img[i] < min) min = img[i];
	}
	//for(i=0; i< (1<<bits); i++) if(distrib[i]) printf("distrib[%d] = %d\n", i, distrib[i]);
	diff = (max+min) > 0 ? max : -min;
	for(i=0; diff; i++) diff>>=1;
	printf("min = %d max = %d  tot = %d bits = %i\n", min, max, max-min, i+1);
	return i+1;
	//for(int i = 0; i<DIM; i++) if(distrib[i]) printf("dist[%4d] = %8d\n",i - HALF, distrib[i]);
}
/*
void subband_dist_entr(uint32 *distrib, uint32 dist_size, uint32 step, uint32 size, double *dis, double *e)
///! \fn static inline int dist_unifom_8(uint32 *distrib, const uint32 bit)
//	\brief Calculate distortion for the given uniform quantizer.
//    \param distrib	 	The pointer to array of distribution probabilities.
//    \param dist_size 	The size of distribution probabilities array.
//	\param step			The step of quantization.
//	\param size			The number of pixels in subband
//	\param q			The quntization array
//	\param dis			The subband distortion
//	\param en			The subband entropy
//
{
	uint32 sum = 0, en=0, tot=0;
	int i, j, k, rest, last, lasth, last1, lasth1;
	uint32  hstep = step>>1, num = (dist_size>>1), st;  // The half size of interval
	*e=0.;
	for(j= (1-step); j< (int)step; j++) {
		sum += distrib[num+j]*j*j;
		en += distrib[num+j];
		//q[num+i+j] = 0;
		//printf("sum = %d en = %d\n", sum, en);
	}
	if(en) *e -= ((double)en/(double)size)*log2((double)en/(double)size);
	tot += en;

	rest = num-step;
	for(i=step; rest > 0; i+=step){
		en = 0;
		st = (rest<step) ? rest : step;
		for(j= 0; j< st; j++) {
			//printf("distrib[%d] = %d\n", num+i+j, distrib[num+i+j]);
			sum += distrib[num+i+j]*(j-hstep)*(j-hstep);
			en += distrib[num+i+j];
			//q[num +i+j] = i+(st>>1);
		}
		if(en) *e -= ((double)en/(double)size)*log2((double)en/(double)size);
		tot += en;
		//printf("tot = %d i = %d rest = %d en = %d st = %d e = %f num = %d\n", tot, i, rest, en, st, ((double)en/(double)size)*log2((double)en/(double)size), num-i-j);
		rest -= step;
	}
	rest = num-step+1;
	for(i=step; rest > 0; i+=step){
		en=0;
		st = (rest<step) ? rest : step;
		for(j= 0; j< st; j++) {
			//printf("distrib[%d] = %d\n", num-i-j, distrib[num-i-j]);
			sum += distrib[num-i-j]*(hstep-j)*(hstep-j);
			en += distrib[num-i-j];
			//q[num -i-j] = -i-(st>>1);
		}
		if(en) *e -= ((double)en/(double)size)*log2((double)en/(double)size);
		tot += en;
		//printf("tot = %d i = %d rest = %d en = %d st = %d e = %f num = %d\n", tot, i, rest, en, st, ((double)en/(double)size)*log2((double)en/(double)size), num-i-j);
		rest -= step;
	}

	*dis = (double)sum/(double)size;
	//for(i=0; i<256;i++) printf("q[%d]=%d\n",i,q[i]);
	//printf("distr = %f entrop = %f tot = %d size = %d\n",dis, e, tot, size);
	//return (double)sum/(double)size;
}

*/
double subband_entropy(uint32 *d, uint32 d_bits, uint32 a_bits, uint32 q_bits, uint32 size, uint32 *q)
/*! \fn static inline int dist_unifom_8(uint32 *distrib, const uint32 bit)
	\brief Calculate distortion for the given uniform quantizer.
    \param d	 		The pointer to distribution probabilities arrey.
    \param d_bits 		The 1<<d_bits size of distribution probabilities array.
	\param a_bits		The 1<<a_bits actual size of distribution probabilities.
	\param q_bits		Bits for quantization.
	\param size			The subband size.
	\param e			The subband entropy.
 */
{
	// |--------|--------0--------|--------|
	// |           1<< dist_bits           |
	//          |   1<<act_bits   |
	//          |  |  |  |  |  |  |
	//     step  =  1<<(a_bits-q_bits)
	double e =0;
	uint32  en=0, tot=0;
	int i, j, step = 1<<(a_bits-q_bits), rest = 1<<(a_bits-1);
	int half = (1<<(d_bits-1)), val = step>>1;

	if(q_bits == 0){
		printf("q_bits should be more than 0\n");
		return 0;
	}

	for(j=(1-step); j< (int)step; j++) {
		en += d[half+j];
		q[half+j] = 0;
	}
	if(q_bits == 1) en += d[half-j];
	if(en) e -= ((double)en/(double)size)*log2((double)en/(double)size);
	tot += en;

	for(i=step; i < rest; i+=step){
		en = 0;
		for(j= 0; j< step; j++) {
			en += d[half+i+j];
			q[half +i+j] = i+val;
		}
		if(en) e -= ((double)en/(double)size)*log2((double)en/(double)size);
		tot += en;
		//printf("tot = %d i = %d rest = %d en = %d st = %d e = %f num = %d\n", tot, i, rest, en, st, ((double)en/(double)size)*log2((double)en/(double)size), num-i-j);
	}
	for(i=step; i < rest; i+=step){
		en=0;
		for(j= 0; j< step; j++) {
			en += d[half-i-j];
			q[half-i-j] = -i-val;
		}
		if(i == rest-step) en += d[half-i-j];
		if(en) e -= ((double)en/(double)size)*log2((double)en/(double)size);
		tot += en;
		//printf("tot = %d i = %d rest = %d en = %d st = %d e = %f num = %d\n", tot, i, rest, en, st, ((double)en/(double)size)*log2((double)en/(double)size), num-i-j);
		//rest -= step;
	}
	printf("tot = %d ", tot);
	return e;
}

void  subband_quantization(imgtype *img,  uint32 size, uint32 *q, uint32 d_bits)
/*! \fn static inline int dist_unifom_8(uint32 *distrib, const uint32 bit)
	\brief Calculate distortion for the given uniform quantizer.
	\param img			The pointer to subband
	\param size			The number of pixels in subband
    \param q	 		The pointer to quantization array.
    \param d_bits 		The 1<<d_bits size of quantization array.
 */
{
	int i, half = (1<<(d_bits-1));
	//Quantization and finding range of distribution
	for(i=0; i < size; i++ ) {
		img[i] = q[img[i] + half];
	}
	//printf("min = %d max = %d  tot = %d bits = %d step = %d range = %d\n", min, max, max-min, i+1, step, range);
	//return i+1;
}
