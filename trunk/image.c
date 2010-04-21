#include <walet.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
	img->qfl  = (uint32 *)calloc(num, sizeof(uint32));
	img->qfl[0] = 1; for(i=1; i< num-1; i++) img->qfl[i] += img->qfl[i-1]+3; img->qfl[num-1] = img->qfl[num-2]+2;
	for(i=0; i<steps; i++) printf("fl[%d] = %d \n", i, img->qfl[i]);

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

void image_idwt_53_(Image *im, imgtype *buf, Subband **sub, ColorSpace color, uint32 steps)
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

void image_idwt_53(Image *im, imgtype *buf, Subband **sub, ColorSpace color, uint32 steps, uint32 st)
///	\fn void image_idwt_53(Image *im, imgtype *buf, Subband **sub, ColorSpace color, uint32 steps, uint32 st, Vector *size)
///	\brief Invert discrete wavelets transform of the image.
///	\param im	 		The pointer to image.
///	\param but 			Pointer to temporary buffer.
///	\param sub 			Pointer to subband arrey.
///	\param color		Color space.
///	\param steps		DWT steps.
///	\param st			IDWT steps.
{
	imgtype *s1 = buf, *img = im->img;
	uint32 k, i, j, h, w;
	if(color == BAYER){
		if(steps == 1){
			h = im->size.y;
			w = im->size.x;
			idwt53_2d_v(img, s1, w, h);
			for(j=0; j < h; j++) idwt53_1d_1h(&s1[j*w], &img[j*w], w);
			im->idwts.x = w; im->idwts.y = h;
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
			im->idwts.x = w; im->idwts.y = h;
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
		im->idwts.x = w; im->idwts.y = h;
	}
}

void image_fill_subb(Image *im, Subband **sub, uint32 bits, ColorSpace color, uint32 steps)
{
	uint32 i, j;
	imgtype *img = im->img;
	im->qst = 0;
	if(color == BAYER){
		for(j=0; j < (steps-1)*3+1; j++)
			for(i=0; i < 4; i++) {
				printf("%2d %2d ", j, i);
				sub[i][j].bits = subband_fill_prob(&img[sub[i][j].loc], sub[i][j].size.x*sub[i][j].size.y, sub[i][j].dist, bits+2);
				sub[i][j].q_bits = sub[i][j].bits;
				im->qst += j ? sub[i][j].bits-1 : 0;
				//sub[i][j].q_bits = 0;
			}
	} else {
		for(j=0; j < steps*3 + 1; j++)
			sub[0][j].bits = subband_fill_prob(&img[sub[0][j].loc], sub[0][j].size.x*sub[0][j].size.y, sub[0][j].dist, bits+2);
			sub[0][j].q_bits = sub[0][j].bits;
			im->qst += j ? sub[0][j].bits-1 : 0;
			//sub[0][j].q_bits = 0;
	}
}

void image_fill_bayer_hist(Image *im, uint32 bits, ColorSpace color, BayerGrid bay)
{
	uint32 i, j, size = im->size.y*im->size.x, sz = 1<<bits, sum;
	uint32	tmp = size;
	if(color == BAYER) {
		utils_fill_bayer_hist(im->img, im->hist, &im->hist[sz], &im->hist[sz*2], im->size.y, im->size.x, bay, bits);
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

static void bits_per_subband(Image *im, Subband **sub, ColorSpace color, uint32 steps, uint32 qst, uint32 st)
///	\fn static void bits_allocation(Image *im, Subband **sub, ColorSpace color, uint32 steps, uint32 st)
///	\brief Bits allocation for quantization algorithm.
///	\param im	 		The pointer to image.
///	\param sub 			Pointer to subband arrey.
///	\param color		Color space.
///	\param steps		DWT steps.
///	\param st			Quantization steps 0-no quantization.
{
	uint32 i, j, k, num, df;
	//qst = 0;
	if(color == BAYER) {
		num = (steps-1)*3;
		for(k=0; k<4; k++) {
			for(i=0; i < num+1; i++) sub[k][i].q_bits = sub[k][i].bits;
			//for(i=1; i < num+1; i++) qst += sub[k][i].bits-1;
		}
		//printf("qst = %d\n", qst);
		st = max(st, qst);
		for(i=0; ; i++){
			for(k=0; k<4; k++){
				switch(k){
					case(0):{ df = im->qfl[max(i,steps-1)]; break;}
					case(1):{ df = i>0 ? im->qfl[max(i-1,steps-1)] : 0; break;}
					case(2):{ df = i>0 ? im->qfl[max(i-1,steps-1)] : 0; break;}
					case(3):{ df = i>1 ? im->qfl[max(i-2,steps-1)] : 0; break;}
				}
				for(j=0; j < df; j++){
					if(sub[3-k][num-j].q_bits > 1) { sub[3-k][num-j].q_bits--; st--;}
					if(!st) break;
				}
				if(!st) break;
			}
			if(!st) break;
		}
	} else {
		num = steps*3;
		for(i=0; i < num+1; i++) sub[0][i].q_bits = sub[0][i].bits;

		//printf("stmax = %d\n", stmax);
		st = max(st, qst);
		for(i=0; ; i++){
			for(j=0; j < im->qfl[max(i,steps)]; j++){
				if(sub[0][num-j].q_bits > 1) { sub[0][num-j].q_bits--; st--;}
				if(!st) break;
			}
			if(!st) break;
		}
		//for(i=0; i < num+1; i++) printf("%2d ", sub[0][i].q_bits);
		//printf("\n");
	}
}

double image_entropy(Image *im, Subband **sub, uint32 bits, ColorSpace color, uint32 steps, uint32 st)
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
	uint32 i,j=0, num, tmp, sq;
	double en=0., dis, e;
	imgtype *img = im->img;

	if(st) bits_per_subband(im, sub, color, steps, im->qst, st);

	if(color == BAYER){
		for(j=0; j < (steps-1)*3+1; j++) {
			for(i=0; i < 4; i++){
				sq = sub[i][j].size.x*sub[i][j].size.y;
				if(sub[i][j].q_bits > 1){
					e = subband_entropy(sub[i][j].dist, sq, bits+2, sub[i][j].bits, sub[i][j].q_bits);
					en += sq*e;
				}
			}
		}
		//printf("entr = %f\n", en / (im->size.x*im->size.y));
		return en /= (im->size.x*im->size.y);
	} else {
		for(j=0; j < steps*3 + 1; j++){
			if(sub[0][j].q_bits > 1){
				sq = sub[0][j].size.x*sub[0][j].size.y;
				e = subband_entropy(sub[0][j].dist, sq, bits+2, sub[0][j].bits, sub[0][j].q_bits);
				en += sq*e;
			}
		}
		return en /= (im->size.x*im->size.y);
	}
}
/*
double image_size(Image *im, Subband **sub, uint32 bits, ColorSpace color, uint32 steps, uint32 st)
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

	bits_per_subband(im, sub, color, steps, im->qst, st);

	if(color == BAYER){
		for(j=0; j < (steps-1)*3+1; j++) {
			for(i=0; i < 4; i++){
				if(sub[i][j].q_bits > 1){
					e = subband_entropy(sub[i][j].dist, sub[i][j].size.x*sub[i][j].size.y,
							bits+2, sub[i][j].bits, sub[i][j].q_bits);
					en += sub[i][j].size.x*sub[i][j].size.y*e;
				}
				//printf("size = %d %d %d e = %f q_bits = %d  bits = %d\n",
				//		sub[i][j].size.x*sub[i][j].size.y, j, i, e, sub[i][j].q_bits, sub[i][j].bits-sb[i]-qu[st][j+3]);
			}
		}
		//printf("entr = %f\n", en / (im->size.x*im->size.y));
		return en /= (im->size.x*im->size.y);
	} else {
		for(j=0; j < steps*3 + 1; j++){
			if(sub[0][j].q_bits > 1){
				e = subband_entropy(sub[0][j].dist, sub[0][j].size.x*sub[0][j].size.y,
						bits+2, sub[0][j].bits, sub[0][j].q_bits);
				en += sub[0][j].size.x*sub[0][j].size.y*e;
			}
		}
		return en /= (im->size.x*im->size.y);
	}
}
*/

void image_bits_alloc(Image *im, Subband **sub, uint32 bits, ColorSpace color, uint32 steps, double per)
{
	double en, e;
	uint32 i, j, st, nsz, nsz1;
	//imgtype *img = im->img;

	en = (double)bits*per;
	st = (im->qst>>1);

	for(i=2;;i++){
		e = image_entropy(im, sub, bits, color, steps, st);
		printf("qst = %d en = %f st = %d e = %f\n", im->qst, en, st, e);
		st = (e < en) ? st - (im->qst>>i) : st + (im->qst>>i);
		if(!(im->qst>>i)) break;
	}
}

void image_quantization(Image *im, Subband **sub, uint32 num, uint32 bits, ColorSpace color, uint32 steps)
{
	uint32 i, j, k, sq, sz;
	uint32 size = 0;
	Subband *s;
	imgtype *img = im->img;

	sz = (color == BAYER) ? ((steps-1)*3+1)<<2 : steps*3 + 1;
	for(i=0; i < sz; i++) {
		s = (color == BAYER) ? sub[i & 0x3] : sub[num];
		j = (color == BAYER) ? i>>2 : i;
		sq = s[j].size.x*s[j].size.y;
		if(s[j].q_bits != s[j].bits){
			subband_encode_table(bits+2, s[j].bits, s[j].q_bits, s[j].q);
			subband_quantization(&img[s[j].loc], sq, bits+2, s[j].bits, s[j].q_bits, s[j].q);
		} else for(k=0; k<sq; k++) img[s[j].loc+k] = 0;
	}
}

uint32 image_range_encode(Image *im, Subband **sub, uint32 num, uint32 bits, ColorSpace color, uint32 steps, uchar *buf)
{
	uint32 i, j, k, sq, sz;
	uint32 size = 0;
	Subband *s;
	imgtype *img = im->img;

	sz = (color == BAYER) ? ((steps-1)*3+1)<<2 : steps*3 + 1;
	for(i=0; i < sz; i++) {
		s = (color == BAYER) ? sub[i & 0x3] : sub[num];
		j = (color == BAYER) ? i>>2 : i;
		sq = s[j].size.x*s[j].size.y;
		if(s[j].q_bits >1){
			subband_encode_table(bits+2, s[j].bits, s[j].q_bits, s[j].q);
			size += subband_range_encoder(&img[s[j].loc], &s[j].dist[1<<(bits+2)],sq, s[j].bits, s[j].q_bits, &buf[size], s[j].q);
		}
	}
	return size;
}

uint32 image_range_decode(Image *im, Subband **sub, uint32 num, uint32 bits, ColorSpace color, uint32 steps, uchar *buf)
{
	uint32 i, j, k, sq, sz;
	uint32 size = 0;
	Subband *s;
	imgtype *img = im->img;

	sz = (color == BAYER) ? ((steps-1)*3+1)<<2 : steps*3 + 1;
	for(i=0; i < sz; i++) {
		s = (color == BAYER) ? sub[i & 0x3] : sub[num];
		j = (color == BAYER) ? i>>2 : i;
		sq = s[j].size.x*s[j].size.y;
		if(s[j].q_bits >1){
			subband_decode_table(bits+2, s[j].bits, s[j].q_bits, s[j].q);
			size += subband_range_decoder(&img[s[j].loc], &s[j].dist[1<<(bits+2)],sq, s[j].bits, s[j].q_bits, &buf[size], s[j].q);
			} else for(k=0; k<sq; k++) img[s[j].loc+k] = 0;
	}
	return size;
}
