#include <walet.h>

#include <stdio.h>
#include <stdlib.h>

#define lim(max,min, x)  ((x)>max ? max :((x)<min ? min : (x)))

static inline void dwt53_1d_1h(imgtype *in, imgtype *out, const uint32 w)
{
	int wt, i, j, sj, shift = (w>>1) + w%2;
	wt = w-2;
	out[shift] = in[1] - ((in[0] + in[2])>>1);
	out[0] = in[0] + (out[shift]>>1);
	for(i=2,j=1; i < wt; i+=2,j++){
		sj = shift+j;
		out[sj] = in[i+1] - ((in[i] + in[i+2])>>1);
		out[j]  = in[i] + ((out[sj-1] + out[sj])>>2);
	}
	if(w%2){
		out[j] = in[i] + (out[shift+j-1]>>1);
	} else{
		sj = shift+j;
		out[sj] = in[i+1] - in[i];
		out[j] = in[i] + ((out[sj-1] + out[sj])>>2);
	}
}

static inline void idwt53_1d_1h(imgtype *in, imgtype *out, const uint32 w)
{
	int wt, i, j, sj, shift = (w>>1) + w%2;
	wt = w-2;
	out[0] = in[0] - (in[shift]>>1);
	for(i=2,j=0; i <= wt; i+=2,j++){
		sj = shift+j;
		out[i] = in[j+1] - ((in[sj] + in[sj+1])>>2);
		out[i-1] = in[sj] + ((out[i-2] + out[i])>>1);
	}
	if(w%2){
		sj = shift+j;
		out[i] = in[j+1] - (in[sj]>>1);
		out[i-1] = in[sj] + ((out[i-2] + out[i])>>1);
	} else{
		sj = shift+j;
		out[i-1] = in[sj] + out[i-2];
	}
}


static inline void dwt53_2d_v(imgtype *in, imgtype *out, const uint32 w, const uint32 h)
/*
 * 	The order of subband  LL HL LH HH
 *
 */
{
	uint32 i, k=1;
	uint32 h2 = (h>>1), h1 = (h>>1) + h%2, w2 = (w>>1), w1 = (w>>1) + w%2;
	uint32 s[4];
	int kw1, kwi, kw, k1;

	s[0] = 0; s[1] = w1*h1; s[2] = s[1] + w2*h1; s[3] = s[2] + w1*h2;

	for(i=0; i<w1; i++){
		out[s[2]+i] = in[w+i] - ((in[i] + in[i+(w<<1)])>>1);
		out[s[0]+i] = in[i]  + (out[s[2]+i]>>1);
		//printf("out[%d] = %d out[%d] = %d \n", s[2]+kw1, out[s[2]+kw1], s[0]+kw1, out[s[0]+kw1]);
	}
	for(i=0; i<w2; i++){
		out[s[3]+i] = in[w+w1+i] - ((in[w1+i] + in[i+(w<<1)+w1])>>1);
		out[s[1]+i] = in[i+w1]  + (out[s[3]+i]>>1);
	}
	for(k=3; k < h-1; k+=2){
		k1 = (k>>1)*w1; kw = k*w;
		for(i=0; i<w1; i++){
			kw1 = k1+i; kwi = kw+i;
			out[s[2]+kw1] = in[kwi] - ((in[kwi-w] + in[kwi+w])>>1);
			out[s[0]+kw1] = in[kwi-w] + ((out[s[2]+kw1 -w1] + out[s[2]+kw1])>>2);
		}
		k1 = (k>>1)*w2; kw = k*w+w1;
		for(i=0; i<w2; i++){
			kw1 = k1+i; kwi = kw+i;
			out[s[3]+kw1] = in[kwi] - ((in[kwi-w] + in[kwi+w])>>1);
			out[s[1]+kw1] = in[kwi-w]  + ((out[s[3]+kw1-w2] + out[s[3]+kw1])>>2);
		}
	}
	if(h%2){
		k1 = (k>>1)*w1; kw = k*w;
		for(i=0; i<w1; i++){
			kw1 = k1+i; kwi = kw+i;
			out[s[0]+kw1] = in[kwi-w] + (out[s[2]+kw1 -w1]>>1);
		}
		k1 = (k>>1)*w2; kw = k*w+w1;
		for(i=0; i<w2; i++){
			kw1 = k1+i; kwi = kw+i;
			out[s[1]+kw1] = in[kwi-w] + (out[s[3]+kw1-w2]>>1);
		}
	} else{
		k1 = (k>>1)*w1; kw = k*w;
		for(i=0; i<w1; i++){
			kw1 = k1+i; kwi = kw+i;
			out[s[2]+kw1] = in[kwi] - in[kwi-w];
			out[s[0]+kw1] = in[kwi-w] + ((out[s[2]+kw1 -w1] + out[s[2]+kw1])>>2);
		}
		k1 = (k>>1)*w2; kw = k*w+w1;
		for(i=0; i<w2; i++){
			kw1 = k1+i; kwi = kw+i;
			out[s[3]+kw1] = in[kwi] - in[kwi-w];
			out[s[1]+kw1] = in[kwi-w]  + ((out[s[3]+kw1-w2] + out[s[3]+kw1])>>2);
		}
	}
}

static inline void idwt53_2d_v(imgtype *in, imgtype *out, const uint32 w, const uint32 h)
/*
 * 	The order of subband  LL HL LH HH
 *
 */
{
	uint32 i, k=0;
	uint32 h2 = (h>>1), h1 = (h>>1) + h%2, w2 = (w>>1), w1 = (w>>1) + w%2;
	uint32 s[4];
	int kw1, kwi, kw, k1;

	s[0] = 0; s[1] = w1*h1; s[2] = s[1] + w2*h1; s[3] = s[2] + w1*h2;

	for(i=0; i<w1; i++){
		out[i] = in[s[0]+i] - (in[s[2]+i]>>1);
	}
	for(i=0; i<w2; i++){
		out[w1+i] = in[s[1]+i] - (in[s[3]+i]>>1);
	}
	for(k=2; k < h-1; k+=2){
		k1 = (k>>1)*w1; kw = k*w;
		for(i=0; i<w1; i++){
			kw1 = k1+i; kwi = kw+i;
			out[kwi  ] = in[s[0]+kw1] - ((in[s[2]+kw1-w1] + in[s[2]+kw1])>>2);
			out[kwi-w] = in[s[2]+kw1-w1] + ((out[kwi-(w<<1)] + out[kwi])>>1);
		}
		k1 = (k>>1)*w2; kw = k*w+w1;
		for(i=0; i<w2; i++){
			kw1 = k1+i; kwi = kw+i;
			out[kwi  ] = in[s[1]+kw1] - ((in[s[3]+kw1-w2] + in[s[3]+kw1])>>2);
			out[kwi-w] = in[s[3]+kw1-w2] + ((out[kwi-(w<<1)] + out[kwi])>>1);
		}
	}
	if(h%2){
		k1 = (k>>1)*w1; kw = k*w;
		for(i=0; i<w1; i++){
			kw1 = k1+i; kwi = kw+i;
			out[kwi  ] = in[s[0]+kw1] - (in[s[2]+kw1-w1]>>1);
			out[kwi-w] = in[s[2]+kw1-w1] + ((out[kwi-(w<<1)] + out[kwi])>>1);

		}
		k1 = (k>>1)*w2; kw = k*w+w1;
		for(i=0; i<w2; i++){
			kw1 = k1+i; kwi = kw+i;
			out[kwi  ] = in[s[1]+kw1] - (in[s[3]+kw1-w2]>>1);
			out[kwi-w] = in[s[3]+kw1-w2] + ((out[kwi-(w<<1)] + out[kwi])>>1);
		}
	} else{
		k1 = (k>>1)*w1; kw = k*w;
		for(i=0; i<w1; i++){
			kw1 = k1+i; kwi = kw+i;
			out[kwi-w] = in[s[2]+kw1-w1] + out[kwi-(w<<1)];
			//if(i==0) printf("k = %d out[%d] = %d in[%d] = %d out[%d] = %d\n", k, kwi-w, out[kwi-w], kw1-w1, in[s[0]+kw1-w1], kwi-w*2, out[kwi-w*2]);
		}
		k1 = (k>>1)*w2; kw = k*w+w1;
		for(i=0; i<w2; i++){
			kw1 = k1+i; kwi = kw+i;
			out[kwi-w] = in[s[3]+kw1-w2] + out[kwi-(w<<1)];
		}
	}
}


void image_init(Image *img, uint32 x, uint32 y, uint32 bits, ColorSpace color, uint32 steps)
{
	int i, num;
	img->img = (imgtype *)calloc(x*y, sizeof(imgtype));
	img->hist = (color == BAYER) ? (uint32 *)calloc((1<<bits)*3, sizeof(uint32)) : (uint32 *)calloc(1<<bits, sizeof(uint32));
	img->look = (color == BAYER) ? (uint16 *)calloc((1<<bits)*3, sizeof(uint16)) : (uint16 *)calloc(1<<bits, sizeof(uint16));
	//img->qfl[steps] = 1; for(i=steps-1; i; i--) img->qfl[i] += img->qfl[i+1]+3; img->qfl[0] = img->qfl[1]+2;
	if(color == BAYER) num = steps;
	else num = steps+1;
	img->qfl[0]  = (uint32 *)calloc(num, sizeof(uint32));
	img->qfl[1]  = (uint32 *)calloc(num, sizeof(uint32));
	img->qfl[0][0] = 1; for(i=1; i< num-1; i++) img->qfl[0][i] += img->qfl[0][i-1]+3; img->qfl[0][num-1] = img->qfl[0][num-2]+2;
	img->qfl[1][0] = 1; for(i=1; i< num-1; i++) img->qfl[1][i] = 3; img->qfl[1][num-1] = 2;
	for(i=0; i<steps; i++) printf("fl[0][%d] = %d fl[0][%d] = %d\n", i, img->qfl[0][i], i, img->qfl[1][i]);

	img->size.x = x; img->size.y = y;
	printf("Create frame x = %d y = %d p = %p\n", img->size.x, img->size.y, img->img);
}

void image_copy(Image *img, uint32 bits, uchar *v)
{
	uint32 i, size = img->size.x*img->size.y, sz = 1<<bits;
	printf("Start copy  x = %d y = %d p = %p size = %d\n", img->size.x, img->size.y, img->img, sz);
	if(bits > 8) for(i=0; i<size; i++) img->img[i] = (v[i<<1]<<8) | v[(i<<1)+1];
	else 		 for(i=0; i<size; i++) img->img[i] = v[i];
}

void image_dwt_53(Image *im, imgtype *buf, Subband **sub, ColorSpace color, uint32 steps)
{
	imgtype *s1 = buf, *img = im->img;
	uint32 j, k, h, w;
	int i, nsub;
	h =  im->size.y; w = im->size.x;

	if(color == BAYER){
		for(j=0; j < h; j++) dwt53_1d_1h(&img[j*w], &s1[j*w], w);
		dwt53_2d_v(s1, img, w, h);
		//dwt53_2d_v_8bit(s1, img, w, h);
		if(steps > 1){
			for(i=(steps-1); i>0; i--){
				for(k=0; k<4; k++) {
					w = sub[k][3*i-1].size.x + sub[k][3*i-2].size.x;
					h = sub[k][3*i-1].size.y + sub[k][3*i-2].size.y;
					for(j=0; j < h; j++) dwt53_1d_1h(&img[sub[k][0].loc+j*w], &s1[j*w], w);
				 	dwt53_2d_v(s1, &img[sub[k][0].loc], w, h);
				 	//dwt53_2d_v_8bit(s1, sub[k][0]->img, w, h);
				}
			}
		}
	} else {
		for(i=steps; i>0; i--){
			for(j=0; j < h; j++) dwt53_1d_1h(&img[j*w], &s1[j*w], w);
			dwt53_2d_v(s1, img, w, h);
			//dwt53_2d_v_8bit(s1, img, w, h);
			w = sub[0][3*i-1].size.x;
			h = sub[0][3*i-2].size.y;
		}
	}
}

void image_idwt_53(Image *im, imgtype *buf, Subband **sub, ColorSpace color, uint32 steps)
{
	imgtype *s1 = buf, *img = im->img;
	uint32 k, i, j, h, w;
	if(color == BAYER){
		if(steps == 1){
			h = im->size.y;
			w = im->size.x;
			idwt53_2d_v(img, s1, w, h);
			for(j=0; j < h; j++) idwt53_1d_1h(&s1[j*w], &img[j*w], w);
			//for(j=0; j < h; j++) idwt53_1d_1h_8bit(&s1[j*w], &img[j*w], w);
		} else {
			for(i=0; i<(steps-1); i++){
				for(k=0; k<4; k++) {
					h = sub[k][3*i+1].size.y + sub[k][3*i+2].size.y;
					w = sub[k][3*i+1].size.x + sub[k][3*i+2].size.x;
					idwt53_2d_v(&img[sub[k][0].loc], s1, w, h);
					for(j=0; j < h; j++) idwt53_1d_1h(&s1[j*w], &img[sub[k][0].loc+j*w], w);
					//for(j=0; j < h; j++) idwt53_1d_1h_8bit(&s1[j*w], &sub[k][0]->img[j*w], w);
				}
			}
			h = im->size.y;
			w = im->size.x;
			idwt53_2d_v(img, s1, w, h);
			for(j=0; j < h; j++) idwt53_1d_1h(&s1[j*w], &img[j*w], w);
			//for(j=0; j < h; j++) idwt53_1d_1h_8bit(&s1[j*w], &img[j*w], w);
		}
	} else {
		for(i=0; i<steps; i++){
			h = sub[0][3*i+1].size.y + sub[0][3*i+2].size.y;
			w = sub[0][3*i+1].size.x + sub[0][3*i+2].size.x;
			//printf("1.y = %d 2.y = %d 2.x = %d 1.x = %d\n",sub[0][3*i+1]->size.y, sub[0][3*i+2]->size.y, sub[0][3*i+2]->size.x, sub[0][3*i+1]->size.x);
			idwt53_2d_v(img, s1, w, h);
			for(j=0; j < h; j++) idwt53_1d_1h(&s1[j*w], &img[j*w], w);
			//for(j=0; j < h; j++) idwt53_1d_1h_8bit(&s1[j*w], &img[j*w], w);
		}
	}

}

void image_idwt_531(Image *im, imgtype *buf, Subband **sub, ColorSpace color, uint32 steps, uint32 st, Vector *size)
{
	imgtype *s1 = buf, *img = im->img;
	uint32 k, i, j, h, w;
	if(color == BAYER){
		if(steps == 1){
			h = im->size.y;
			w = im->size.x;
			idwt53_2d_v(img, s1, w, h);
			for(j=0; j < h; j++) idwt53_1d_1h(&s1[j*w], &img[j*w], w);
			size->x = w; size->y = h;
			//for(j=0; j < h; j++) idwt53_1d_1h_8bit(&s1[j*w], &img[j*w], w);
		} else {
			for(i=0; i<(st-1); i++){
				for(k=0; k<4; k++) {
					h = sub[k][3*i+1].size.y + sub[k][3*i+2].size.y;
					w = sub[k][3*i+1].size.x + sub[k][3*i+2].size.x;
					idwt53_2d_v(&img[sub[k][0].loc], s1, w, h);
					for(j=0; j < h; j++) idwt53_1d_1h(&s1[j*w], &img[sub[k][0].loc+j*w], w);
					//for(j=0; j < h; j++) idwt53_1d_1h_8bit(&s1[j*w], &sub[k][0]->img[j*w], w);
				}
			}
			if(steps == st) { h = im->size.y; w = im->size.x;}
			else {
				h = sub[0][3*i+1].size.y + sub[2][3*i+1].size.y;
				w = sub[0][3*i+2].size.x + sub[1][3*i+2].size.x;
			}

			idwt53_2d_v(img, s1, w, h);
			for(j=0; j < h; j++) idwt53_1d_1h(&s1[j*w], &img[j*w], w);
			size->x = w; size->y = h;
			//for(j=0; j < h; j++) idwt53_1d_1h_8bit(&s1[j*w], &img[j*w], w);
		}
	} else {
		for(i=0; i<st; i++){
			h = sub[0][3*i+1].size.y + sub[0][3*i+2].size.y;
			w = sub[0][3*i+1].size.x + sub[0][3*i+2].size.x;
			//printf("1.y = %d 2.y = %d 2.x = %d 1.x = %d\n",sub[0][3*i+1]->size.y, sub[0][3*i+2]->size.y, sub[0][3*i+2]->size.x, sub[0][3*i+1]->size.x);
			idwt53_2d_v(img, s1, w, h);
			for(j=0; j < h; j++) idwt53_1d_1h(&s1[j*w], &img[j*w], w);
			//for(j=0; j < h; j++) idwt53_1d_1h_8bit(&s1[j*w], &img[j*w], w);
		}
		size->x = w; size->y = h;
	}
}

void image_fill_subb(Image *im, Subband **sub, uint32 bits, ColorSpace color, uint32 steps)
{
	uint32 i, j;
	imgtype *img = im->img;
	if(color == BAYER){
		for(j=0; j < (steps-1)*3+1; j++)
			for(i=0; i < 4; i++) {
				printf("%2d %2d ", j, i);
				sub[i][j].bits = subband_fill_prob(&img[sub[i][j].loc], sub[i][j].size.x*sub[i][j].size.y, sub[i][j].dist, bits+2);
				//sub[i][j].q_bits = sub[i][j].bits;
				sub[i][j].q_bits = 0;
			}
	} else {
		for(j=0; j < steps*3 + 1; j++)
			sub[0][j].bits = subband_fill_prob(&img[sub[0][j].loc], sub[0][j].size.x*sub[0][j].size.y, sub[0][j].dist, bits+2);
			//sub[0][j].q_bits = sub[0][j].bits;
			sub[0][j].q_bits = 0;
	}
}

void image_fill_hist(Image *im, uint32 bits, ColorSpace color, BayerGrid bay)
{
	uint32 i, j, size = im->size.y*im->size.x, sz = 1<<bits, sum;
	uint32	tmp = size;
	if(color == BAYER) {
		utils_fill_hist(im->img, im->hist, &im->hist[sz], &im->hist[sz*2], im->size.y, im->size.x, bay, bits);
		sum = 0; for(i=0; i<sz; i++) sum +=im->hist[i]; tmp -= sum;
		printf("size = %d r = %d ", im->size.y*im->size.x, sum);
		sum = 0; for(i=0; i<sz; i++) sum +=im->hist[sz+i]; tmp -= sum;
		printf("g = %d ", sum);
		sum = 0; for(i=0; i<sz; i++) sum +=im->hist[(sz<<1)+i]; tmp -= sum;
		printf("b = %d  diff = %d\n", sum, tmp);
	}
	else  for(i=0; i < size; i++) im->hist[im->img[i]]++;

}

#define max(x, m) ((x>m) ? (m) : (x))

static void bits_allocation(Image *im, Subband **sub, ColorSpace color, uint32 steps, int st)
{
	uint32 i,j=0, num, tmp;
	if(color == BAYER) num = steps;
	else num = steps+1;

	for(i=0; i < num; i++) sub[0][i].q_bits = 0;

	for(i=0;;i++)
		if(st > 0) st -= im->qfl[0][max(i,steps-1)];
		else break;
	tmp = i;

	for(i=0; i<(tmp<num ? tmp : num ); i++)
		for(j=0; j < im->qfl[1][i]; j++)
			sub[0][(num-1)*3 - j - (i ? im->qfl[0][i-1] : 0) ].q_bits = tmp-i;
	//for(i=0; i < tmp; i++)
	//	for(j=0; j < im->qfl[0][i]; j++)
	//		sub[0][(num-1)*3-j].q_bits++;
	if(st) for(j=0; j < -st; j++) sub[0][(num-1)*3+1 - im->qfl[0][max(tmp-1,num-1)] + j].q_bits--;

	printf("%3d tmp = %3d  ", st, tmp);
	for(i=0; i < (num-1)*3+1; i++) printf("%2d ", sub[0][i].q_bits);
	printf("\n");

}

double image_entropy(Image *im, Subband **sub, uint32 bits, ColorSpace color, uint32 steps, int st)
/// \fn double image_entropy(Image *im, Subband **sub, uint32 bits, ColorSpace color, uint32 steps, int st)
///	\brief Calculate image entropy.
/// \param im	 		The pointer to image.
/// \param sub 			The pointer to subband array.
///	\param bits			The bits per pixel.
///	\param color		Image color space.
///	\param steps		The DWT steps.
///	\param st			The step of quantization.
///	\retval 			The image entropy.
{
	uint32 i,j=0, num, tmp;
	double en=0., dis, e;
	imgtype *img = im->img;
	if(color == BAYER){
		bits_allocation(im, sub, color, steps, st);
		/*
		for(i=0; i < 4; i++){
			sub[i][0].q_bits = sub[i][0].bits;
			e = subband_entropy(sub[i][0].dist, bits+2, sub[i][0].bits, sub[i][0].q_bits,
					sub[i][0].size.x*sub[i][0].size.y, sub[i][0].q);
			en += sub[i][0].size.x*sub[i][0].size.y*e;
			//printf("size = %d %d %d e = %f q_bits = %d\n",sub[i][0].size.x*sub[i][0].size.y, j, i, e, sub[i][0].q_bits);
		}
		for(j=1; j < (steps-1)*3+1; j++) {
			for(i=0; i < 4; i++){
				sub[i][j].q_bits = lim((int)sub[i][j].bits, 1, (int)(sub[i][j].bits-sb[i]-qu[st][j+3]));

				e = subband_entropy(sub[i][j].dist, bits+2, sub[i][j].bits, sub[i][j].q_bits,
						sub[i][j].size.x*sub[i][j].size.y, sub[i][j].q);
				en += sub[i][j].size.x*sub[i][j].size.y*e;
				//printf("size = %d %d %d e = %f q_bits = %d  bits = %d\n",
				//		sub[i][j].size.x*sub[i][j].size.y, j, i, e, sub[i][j].q_bits, sub[i][j].bits-sb[i]-qu[st][j+3]);
			}
		}
		return en /= (im->size.x*im->size.y);
		*/
	} else {
		//num = steps*3;

		sub[0][0].q_bits = sub[0][0].bits;
		e = subband_entropy(sub[0][0].dist, bits+2, sub[0][0].bits, sub[0][0].q_bits,
				sub[0][0].size.x*sub[0][0].size.y, sub[0][0].q);
		en += sub[0][0].size.x*sub[0][0].size.y*e;
		for(j=1; j < steps*3 + 1; j++){
			//subband_dist_entr(sub[0][j].dist, 1<<(bits+2), 1<<(del[steps-1][j]+st), sub[0][j].size.x*sub[0][j].size.y, &dis, &e);
			sub[0][j].q_bits = lim(sub[0][j].bits, 1, (int)(sub[0][j].bits-qu[st][j]));

			e = subband_entropy(sub[0][j].dist, bits+2, sub[0][j].bits, sub[0][j].q_bits,
					sub[0][j].size.x*sub[0][j].size.y, sub[0][j].q);
			en += sub[0][j].size.x*sub[0][j].size.y*e;
		}
		return en /= (im->size.x*im->size.y);
	}
}

void image_quantization(Image *im, Subband **sub, uint32 bits, ColorSpace color, uint32 steps, int st)
{
	uint32 i,j;
	imgtype *img = im->img;
	if(color == BAYER){
		//for(i=0; i < 4; i++)
			//subband_quantization(&img[sub[i][0].loc], sub[i][0].size.x*sub[i][0].size.y, sub[i][0].dist, bits+2, 1);
		for(j=1; j < (steps-1)*3+1; j++)
			for(i=0; i < 4; i++){
				printf("%2d %2d bits = %d \n", j, i, sub[i][j].q_bits);
				if(sub[i][j].q_bits != sub[i][j].bits)
					subband_quantization(&img[sub[i][j].loc], sub[i][j].size.x*sub[i][j].size.y, sub[i][j].q, bits+2);
				//subband_quantization(&img[sub[i][j].loc], sub[i][j].size.x*sub[i][j].size.y, sub[i][j].dist, 1<<(bits+2), 1<<(del[steps-2][j]+sb[i]+st));
				//sub[i][j].bits = subband_quantization(&img[sub[i][j].loc], sub[i][j].size.x*sub[i][j].size.y, sub[i][j].dist,
				//		1<<(bits+2), 1<<(del[steps-2][j]+sb[i]+st), 1<<sub[i][j].bits);
			}
	} else {
		//subband_quantization(&img[sub[0][0].loc], sub[0][0].size.x*sub[0][0].size.y, sub[0][0].dist, bits+2, 1);
		for(j=1; j < steps*3 + 1; j++)
			if(sub[0][j].q_bits != sub[0][j].bits)
				subband_quantization(&img[sub[0][j].loc],  sub[0][j].size.x*sub[0][j].size.y, sub[0][j].q, bits+2);
			//sub[0][j].bits = subband_quantization(&img[sub[0][j].loc], sub[0][j].size.x*sub[0][j].size.y, sub[0][j].dist,
			//		1<<(bits+2), 1<<(del[steps-1][j]+st), 1<<sub[0][j].bits);
	}
}

uint32 image_compress(Image *im, Subband **sub, uint32 bits, ColorSpace color, uint32 steps, uchar *buf)
{
	uint32 i,j;
	uint32 size = 0;
	imgtype *img = im->img;
	if(color == BAYER){
		for(j=0; j < (steps-1)*3+1; j++)
			for(i=0; i < 4; i++)
				//size += subband_range_encoder(&img[sub[i][j].loc], &sub[i][j].dist[1<<(bits+2)], sub[i][j].size.x*sub[i][j].size.y, bits+2, &buf[size]);
				if(sub[i][j].q_bits >1) size += subband_range_encoder(&img[sub[i][j].loc], &sub[i][j].dist[1<<(bits+2)],
						sub[i][j].size.x*sub[i][j].size.y, sub[i][j].bits, sub[i][j].q_bits, &buf[size]);
	} else {
		for(j=0; j < steps*3 + 1; j++)
			//size += subband_range_encoder(&img[sub[0][j].loc], &sub[i][j].dist[1<<(bits+2)], sub[0][j].size.x*sub[0][j].size.y, bits+2, &buf[size]);
			if(sub[0][j].q_bits >1) size += subband_range_encoder(&img[sub[0][j].loc], &sub[i][j].dist[1<<(bits+2)],
					sub[0][j].size.x*sub[0][j].size.y, sub[0][j].bits, sub[0][j].q_bits, &buf[size]);
	}
	return size;
}

uint32 image_decompress(Image *im, Subband **sub, uint32 bits, ColorSpace color, uint32 steps, uchar *buf)
{
	uint32 i,j;
	uint32 size = 0;
	imgtype *img = im->img;
	if(color == BAYER){
		for(j=0; j < (steps-1)*3+1; j++)
			for(i=0; i < 4; i++)
				//size += subband_range_decoder(&img[sub[i][j].loc], &sub[i][j].dist[1<<(bits+2)],sub[i][j].size.x*sub[i][j].size.y, bits+2, &buf[size]);
				if(sub[i][j].q_bits >1) size += subband_range_decoder(&img[sub[i][j].loc], &sub[i][j].dist[1<<(bits+2)],
						sub[i][j].size.x*sub[i][j].size.y, sub[i][j].bits, sub[i][j].q_bits, &buf[size]);
	} else {
		for(j=0; j < steps*3 + 1; j++)
			//size += subband_range_decoder(&img[sub[0][j].loc], &sub[i][j].dist[1<<(bits+2)], sub[0][j].size.x*sub[0][j].size.y, bits+2, &buf[size]);
			if(sub[0][j].q_bits >1) size += subband_range_decoder(&img[sub[0][j].loc], &sub[i][j].dist[1<<(bits+2)],
					sub[0][j].size.x*sub[0][j].size.y, sub[0][j].bits, sub[0][j].q_bits, &buf[size]);
	}
	return size;
}
