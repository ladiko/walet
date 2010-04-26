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


void image_init(Image *img, StreamData *sd, uint32 x, uint32 y)
///	\fn void image_init(Image *img, StreamData *sd)
///	\brief Create image
///	\param im	 		The pointer to image.
///	\param sd 			Pointer to StreamData.
{
	int i, num;
	img->img = (imgtype *)calloc(x*y, sizeof(imgtype));
	img->hist = (sd->color == BAYER) ? (uint32 *)calloc((1<<sd->bits)*3, sizeof(uint32)) : (uint32 *)calloc(1<<sd->bits, sizeof(uint32));
	img->look = (sd->color == BAYER) ? (uint16 *)calloc((1<<sd->bits)*3, sizeof(uint16)) : (uint16 *)calloc(1<<sd->bits, sizeof(uint16));
	//img->qfl[steps] = 1; for(i=steps-1; i; i--) img->qfl[i] += img->qfl[i+1]+3; img->qfl[0] = img->qfl[1]+2;
	num = (sd->color == BAYER) ? sd->steps : sd->steps+1;
	img->qfl  = (uint32 *)calloc(num, sizeof(uint32));
	img->qfl[0] = 1; for(i=1; i< num-1; i++) img->qfl[i] += img->qfl[i-1]+3; img->qfl[num-1] = img->qfl[num-2]+2;
	for(i=0; i<sd->steps; i++) printf("fl[%d] = %d \n", i, img->qfl[i]);

	img->size.x = x; img->size.y = y;
	printf("Create frame x = %d y = %d p = %p\n", img->size.x, img->size.y, img->img);
}

void image_copy(Image *img, StreamData *sd, uchar *v)
///	\fn void image_copy(Image *img, StreamData *sd, uchar *v)
///	\brief Copy image.
///	\param im	 		The pointer to image.
///	\param sd 			Pointer to StreamData.
///	\param v 			Input image buffer.
{
	uint32 i, size = img->size.x*img->size.y;
	printf("Start copy  x = %d y = %d p = %p \n", img->size.x, img->size.y, img->img);
	if(sd->bits > 8) for(i=0; i<size; i++) img->img[i] = (v[i<<1]<<8) | v[(i<<1)+1];
	else 		 for(i=0; i<size; i++) img->img[i] = v[i];
}

void image_dwt_53(Image *im, StreamData *sd, imgtype *buf)
///	\fn void image_dwt_53(Image *im, StreamData *sd, imgtype *buf)
///	\brief Discrete wavelets transform of the image.
///	\param im	 		The pointer to image.
///	\param sd 			Pointer to StreamData.
///	\param buf 			Pointer to temporary buffer.
{
	imgtype *s1 = buf, *img = im->img;
	uint32 j, k, h, w, st;
	int i, nsub;
	h =  im->size.y; w = im->size.x;
	Subband *sub = im->sub;

	if(sd->color == BAYER){
		for(j=0; j < h; j++) dwt53_1d_1h(&img[j*w], &s1[j*w], w);
		dwt53_2d_v(s1, img, w, h);
		//dwt53_2d_v_8bit(s1, img, w, h);
		if(sd->steps > 1){
			for(k=0; k<4; k++) {
				st = ((sd->steps-1)*3+1)*k;
				for(i=(sd->steps-1); i>0; i--){
					w = sub[3*i-1+st].size.x + sub[3*i-2+st].size.x;
					h = sub[3*i-1+st].size.y + sub[3*i-2+st].size.y;
					for(j=0; j < h; j++) dwt53_1d_1h(&img[sub[st].loc+j*w], &s1[j*w], w);
				 	dwt53_2d_v(s1, &img[sub[st].loc], w, h);
				}
			}
		}
	} else {
		for(i=sd->steps; i>0; i--){
			for(j=0; j < h; j++) dwt53_1d_1h(&img[j*w], &s1[j*w], w);
			dwt53_2d_v(s1, img, w, h);
			w = sub[3*i-1].size.x;
			h = sub[3*i-2].size.y;
		}
	}
}

void image_idwt_53(Image *im, StreamData *sd, imgtype *buf, uint32 steps)
///	\fn void image_idwt_53(Image *im, StreamData *sd, imgtype *buf, uint32 steps)
///	\brief Invert discrete wavelets transform of the image.
///	\param im	 		The pointer to image.
///	\param sd 			Pointer to StreamData.
///	\param buf 			Pointer to temporary buffer.
///	\param steps		IDWT steps.
{
	imgtype *s1 = buf, *img = im->img;
	uint32 k, i, j, h, w, st;
	Subband *sub = im->sub;

	if(sd->color == BAYER){
		if(sd->steps == 1){
			h = im->size.y;
			w = im->size.x;
			idwt53_2d_v(img, s1, w, h);
			for(j=0; j < h; j++) idwt53_1d_1h(&s1[j*w], &img[j*w], w);
			im->idwts.x = w; im->idwts.y = h;
		} else {
			for(k=0; k<4; k++) {
				for(i=0; i<(steps-1); i++){
					st = ((sd->steps-1)*3+1)*k;
					h = sub[3*i+1+st].size.y + sub[3*i+2+st].size.y;
					w = sub[3*i+1+st].size.x + sub[3*i+2+st].size.x;
					idwt53_2d_v(&img[sub[st].loc], s1, w, h);
					for(j=0; j < h; j++) idwt53_1d_1h(&s1[j*w], &img[sub[st].loc+j*w], w);
				}
			}
			if(sd->steps == steps) { h = im->size.y; w = im->size.x;}
			else {
				h = sub[3*i+1].size.y + sub[3*i+1 + ((sd->steps-1)*3+1)*2].size.y;
				w = sub[3*i+2].size.x + sub[3*i+2 + ((sd->steps-1)*3+1)  ].size.x;
			}

			idwt53_2d_v(img, s1, w, h);
			for(j=0; j < h; j++) idwt53_1d_1h(&s1[j*w], &img[j*w], w);
			im->idwts.x = w; im->idwts.y = h;
		}
	} else {
		for(i=0; i<steps; i++){
			h = sub[3*i+1].size.y + sub[3*i+2].size.y;
			w = sub[3*i+1].size.x + sub[3*i+2].size.x;
			//printf("1.y = %d 2.y = %d 2.x = %d 1.x = %d\n",sub[0][3*i+1]->size.y, sub[0][3*i+2]->size.y, sub[0][3*i+2]->size.x, sub[0][3*i+1]->size.x);
			idwt53_2d_v(img, s1, w, h);
			for(j=0; j < h; j++) idwt53_1d_1h(&s1[j*w], &img[j*w], w);
		}
		im->idwts.x = w; im->idwts.y = h;
	}
}

void image_fill_subb(Image *im, StreamData *sd)
///	\fn void image_fill_subb(Image *im, StreamData *sd)
///	\brief Fill distribution probability arrays.
///	\param im	 		The pointer to image.
///	\param sd 			Pointer to StreamData.
{
	uint32 i, sz, st = ((sd->steps-1)*3+1);
	imgtype *img = im->img;
	im->qst = 0;
	Subband *sub = im->sub;


	sz = (sd->color == BAYER) ? ((sd->steps-1)*3+1)<<2 : sd->steps*3 + 1;
	printf("sz = %d\n  ", sz);
	for(i=0; i < sz; i++) {
		printf("%2d  ", i);
		subband_fill_prob(&img[sub[i].loc], &sub[i]);
		im->qst += sub[i].a_bits-1;
	}
	sz = (sd->color == BAYER) ? 4 : 1;
	for(i=0; i <sz; i++) im->qst -= sub[i*st].a_bits-1; //Remove all ll subbands from quantization
}

void image_fill_bayer_hist(Image *im, StreamData *sd)
///	\fn void image_fill_bayer_hist(Image *im, StreamData *sd)
///	\brief Fill color histogram for white balancing.
///	\param im	 		The pointer to image.
///	\param sd 			Pointer to StreamData.
{
	uint32 i, j, size = im->size.y*im->size.x, sz = 1<<sd->bits, sum;
	uint32	tmp = size;
	if(sd->color == BAYER) {
		utils_fill_bayer_hist(im->img, im->hist, &im->hist[sz], &im->hist[sz*2], im->size.y, im->size.x, sd->bg, sd->bits);
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

static void bits_per_subband1(Image *im, Subband **sub, uint32 num, ColorSpace color, uint32 steps, uint32 qst, uint32 st)
///	\fn static void bits_allocation(Image *im, Subband **sub, ColorSpace color, uint32 steps, uint32 st)
///	\brief Bits allocation for quantization algorithm.
///	\param im	 		The pointer to image.
///	\param sub 			Pointer to subband arrey.
///	\param color		Color space.
///	\param steps		DWT steps.
///	\param st			Quantization steps 0-no quantization.
{
	uint32 i, j, k, sz, df;
	//qst = 0;
	if(color == BAYER) {
		sz = (steps-1)*3;
		for(k=0; k<4; k++) {
			for(i=0; i < sz+1; i++) sub[k][i].q_bits = sub[k][i].a_bits;
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
					if(sub[3-k][sz-j].q_bits > 1) { sub[3-k][sz-j].q_bits--; st--;}
					if(!st) break;
				}
				if(!st) break;
			}
			if(!st) break;
		}
	} else {
		sz = steps*3;
		//for(i=0; i < sz+1; i++) sub[num][i].q_bits = sub[num][i].bits;

		//printf("stmax = %d\n", stmax);
		st = max(st, qst);
		for(i=0; ; i++){
			for(j=0; j < im->qfl[max(i,steps)]; j++){
				if(sub[num][sz-j].q_bits > 1) { sub[num][sz-j].q_bits--; st--;}
				if(!st) break;
			}
			if(!st) break;
		}
		//for(i=0; i < sz+1; i++) printf("%2d ", sub[0][i].q_bits);
		//printf("\n");
	}
}

static void bits_per_subband(Image *im, StreamData *sd, uint32 qstep)
///	\fn static void bits_per_subband(Image *im, StreamData *sd, uint32 qstep)
///	\brief Bits allocation for quantization algorithm.
///	\param im	 		The pointer to image.
///	\param sd 			Pointer to StreamData.
///	\param qstep		Quantization steps (if 0-no quantization).
{
	uint32 i, j, k, sz, df;
	//im->qst = 0;
	Subband *sub = im->sub;
	//qst = 0;
	if(sd->color == BAYER) {
		sz = (sd->steps-1)*3;

		for(i=0; i < ((sz+1)<<2); i++) sub[i].q_bits = sub[i].a_bits;
		//printf("qstep = %d\n", qstep);
		qstep = max(qstep, im->qst);
		for(i=0; i<50; i++){
			for(k=0; k<4; k++){
				switch(k){
					case(0):{ df = im->qfl[max(i,sd->steps-1)]; break;}
					case(1):{ df = i>0 ? im->qfl[max(i-1,sd->steps-1)] : 0; break;}
					case(2):{ df = i>0 ? im->qfl[max(i-1,sd->steps-1)] : 0; break;}
					case(3):{ df = i>1 ? im->qfl[max(i-2,sd->steps-1)] : 0; break;}
				}
				for(j=0; j < df; j++){
					//if(sub[3-k][sz-j].q_bits > 1) { sub[3-k][sz-j].q_bits--; qstep--;}
					if(sub[(3-k)*(sz+1)+sz-j].q_bits > 1) { sub[(3-k)*(sz+1)+sz-j].q_bits--; qstep--;}
					if(!qstep) break;
				}
				//printf("qstep = %d df = %d q_bits[%d] = %d\n", qstep, df, (3-k)*(sz+1)+sz-j, sub[(3-k)*(sz+1)+sz-j].q_bits);
				if(!qstep) break;
			}
			if(!qstep) break;
		}
		//for(i=0; i < (sz+1)*4; i++) printf("a_bits = %d q_bits = %2d\n", sub[i].a_bits, sub[i].q_bits);
	} else {
		sz = sd->steps*3;
		for(i=0; i < (sz+1); i++) sub[i].q_bits = sub[i].a_bits;
		//printf("stmax = %d\n", stmax);
		qstep = max(qstep, im->qst);
		for(i=0; ; i++){
			for(j=0; j < im->qfl[max(i,sd->steps)]; j++){
				if(sub[sz-j].q_bits > 1) { sub[sz-j].q_bits--; qstep--;}
				if(!qstep) break;
			}
			if(!qstep) break;
		}
		//for(i=0; i < sz+1; i++) printf("%2d ", sub[0][i].q_bits);
		//printf("\n");
	}
}
/*
double image_entropy(Image *im, Subband **sub, uint32 num, uint32 bits, ColorSpace color, uint32 steps, uint32 st)
/// \fn double image_entropy(Image *im, Subband **sub, uint32 bits, ColorSpace color, uint32 steps, int st)
///     \brief Calculate image entropy.
/// \param im                   The pointer to image.
/// \param sub                  The pointer to subband array.
///     \param bits                     The bits per pixel.
///     \param color            Image color space.
///     \param steps            The DWT steps.
///     \param st                       The step of quantization.
///     \retval                         The image entropy.
{
        uint32 i, j, k, sq, sz;
        uint32 size = 0;
        Subband *s;
        imgtype *img = im->img;
        double en=0.;

        //if(st) bits_per_subband(im, sub, num, color, steps, im->qst, st);

        sz = (color == BAYER) ? ((steps-1)*3+1)<<2 : steps*3 + 1;
        for(i=0; i < sz; i++) {
                s = (color == BAYER) ? sub[i & 0x3] : sub[num];
                j = (color == BAYER) ? i>>2 : i;
                sq = s[j].size.x*s[j].size.y;
                if(s[j].q_bits > 1) en += sq*subband_entropy(s[j].dist, sq, bits+2, s[j].a_bits, s[j].q_bits);
        }
        return en /= (im->size.x*im->size.y);
}
*/
uint32 image_size(Image *im, StreamData *sd, uint32 qstep)
///	\fn uint32 image_size(Image *im, StreamData *sd, uint32 qstep)
///	\brief Calculate the size off subband after quantization.
///	\param im	 		The pointer to image.
///	\param sd 			Pointer to StreamData.
///	\param qstep		Quantization steps (if 0-no quantization).
{
	uint32 i, sz, s=0;
	Subband *sub = im->sub;

	if(qstep) bits_per_subband(im, sd, qstep);

	sz = (sd->color == BAYER) ? ((sd->steps-1)*3+1)<<2 : sd->steps*3 + 1;
	for(i=0; i < sz; i++) {
		if(sub[i].q_bits > 1) s += subband_size(&sub[i]);
	}
	return s;
}

void image_bits_alloc(Image *im, StreamData *sd, uint32 times)
///	\fn void image_bits_alloc(Image *im, StreamData *sd, uint32 times)
///	\brief Bits allocation for subbands.
///	\param im	 		The pointer to image.
///	\param sd 			Pointer to StreamData.
///	\param times		Compression in times in relation to the original image.
{
	uint32 i, s, qstep, size;

	size = (im->size.x*im->size.y*sd->bits)/times;
	qstep = (im->qst>>1);

	for(i=2;;i++){
		s = image_size(im, sd, qstep);
		printf("qst = %d size = %d qstep = %d s = %d\n", im->qst, size, qstep, s);
		qstep = (s < size) ? qstep - (im->qst>>i) : qstep + (im->qst>>i);
		if(!(im->qst>>i)) break;
	}
}

void image_quantization(Image *im, StreamData *sd)
{
	uint32 i, sz;
	Subband *sub = im->sub;
	imgtype *img = im->img;

	sz = (sd->color == BAYER) ? ((sd->steps-1)*3+1)<<2 : sd->steps*3 + 1;
	for(i=0; i < sz; i++) {
		printf("%2d bits = %d q_bits = %d \n", i, sub[i].a_bits, sub[i].q_bits);
		subband_quantization(&img[sub[i].loc], &sub[i]);
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
			//subband_encode_table(bits+2, s[j].a_bits, s[j].q_bits, s[j].q);
			size += subband_range_encoder(&img[s[j].loc], &s[j].dist[1<<(bits+2)],sq, s[j].a_bits, s[j].q_bits, &buf[size], s[j].q);
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
			//subband_decode_table(bits+2, s[j].a_bits, s[j].q_bits, s[j].q);
			size += subband_range_decoder(&img[s[j].loc], &s[j].dist[1<<(bits+2)],sq, s[j].a_bits, s[j].q_bits, &buf[size], s[j].q);
			} else for(k=0; k<sq; k++) img[s[j].loc+k] = 0;
	}
	return size;
}
