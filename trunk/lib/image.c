#include <walet.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define lim(max,min, x)  	((x)>max ? max :((x)<min ? min : (x)))
#define max(x, m) 			((x>m) ? (m) : (x))


static inline void dwt53_1d_1h(imgtype *in, imgtype *out, const uint32 w)
///	\fn static inline void dwt53_1d_1h(imgtype *in, imgtype *out, const uint32 w)
///	\brief 1D 5.3 wavelet transform.
///	\param in	 		The input line.
///	\param out 			The output line.
///	\param w 			The line width.
{
	int wt, i, j, sj, shift = (w>>1) + (w&1);
	wt = w-2;
	out[shift] = in[1] - ((in[0] + in[2])>>1);
	out[0] = in[0] + (out[shift]>>1);
	for(i=2,j=1; i < wt; i+=2,j++){
		sj = shift+j;
		out[sj] = in[i+1] - ((in[i] + in[i+2])>>1);
		out[j]  = in[i] + ((out[sj-1] + out[sj])>>2);
	}
	if(w&1){
		out[j] = in[i] + (out[shift+j-1]>>1);
	} else{
		sj = shift+j;
		out[sj] = in[i+1] - in[i];
		out[j] = in[i] + ((out[sj-1] + out[sj])>>2);
	}
}

static inline void idwt53_1d_1h(imgtype *in, imgtype *out, const uint32 w)
///	\fn static inline void idwt53_1d_1h(imgtype *in, imgtype *out, const uint32 w)
///	\brief 1D 5.3 invert wavelet transform.
///	\param in	 		The input line.
///	\param out 			The output line.
///	\param w 			The line width.
{
	int wt, i, j, sj, shift = (w>>1) + (w&1);
	wt = w-2;
	out[0] = in[0] - (in[shift]>>1);
	for(i=2,j=0; i <= wt; i+=2,j++){
		sj = shift+j;
		out[i] = in[j+1] - ((in[sj] + in[sj+1])>>2);
		out[i-1] = in[sj] + ((out[i-2] + out[i])>>1);
	}
	if(w&1){
		sj = shift+j;
		out[i] = in[j+1] - (in[sj]>>1);
		out[i-1] = in[sj] + ((out[i-2] + out[i])>>1);
	} else{
		sj = shift+j;
		out[i-1] = in[sj] + out[i-2];
	}
}

static inline void dwt53_2d_v(imgtype *in, imgtype *out, const uint32 w, const uint32 h)
///	\fn static inline void dwt53_2d_v(imgtype *in, imgtype *out, const uint32 w, const uint32 h)
///	\brief 2D 5.3 vertical wavelet transform.
///	\param in	 		The input image data.
///	\param out 			The output image data.
///	\param w 			The image width.
///	\param h 			The image height.
{
	uint32 i, k=1;
	uint32 h2 = (h>>1), h1 = (h>>1) + (h&1), w2 = (w>>1), w1 = (w>>1) + (w&1);
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
	if(h&1){
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
///	\fn static inline void idwt53_2d_v(imgtype *in, imgtype *out, const uint32 w, const uint32 h)
///	\brief 2D 5.3 vertical invert wavelet transform.
///	\param in	 		The input image data.
///	\param out 			The output image data.
///	\param w 			The image width.
///	\param h 			The image height.
{
	uint32 i, k=0;
	uint32 h2 = (h>>1), h1 = (h>>1) + (h&1), w2 = (w>>1), w1 = (w>>1) + (w&1);
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
	if(h&1){
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

void image_init(Image *im, uint32 width, uint32 height, ColorSpace color, uint32 bpp, uint32 steps)
///	\fn void image_init(Image *im, uint32 width, uint32 height, ColorSpace color, uint32 bpp, uint32 steps)
///	\brief Init image structure.
///	\param im	 		The image structure.
///	\param w 			The image width.
///	\param h 			The image height.
///	\param color		The color space of the stream.
///	\param bpp 			The bits per pixel.
///	\param steps 		The steps of DWT transform.

{
	int i, num;
	im->width = width; im->height = height;
	im->img = (imgtype *)calloc(width*height, sizeof(imgtype));
	im->hist = (color == BAYER) ? (uint32 *)calloc((1<<bpp)*3, sizeof(uint32)) : (uint32 *)calloc(1<<bpp, sizeof(uint32));
	im->look = (color == BAYER) ? (uint16 *)calloc((1<<bpp)*3, sizeof(uint16)) : (uint16 *)calloc(1<<bpp, sizeof(uint16));
	//img->qfl[steps] = 1; for(i=steps-1; i; i--) img->qfl[i] += img->qfl[i+1]+3; img->qfl[0] = img->qfl[1]+2;
	num = (color == BAYER) ? steps : steps+1;
	im->qfl  = (uint32 *)calloc(num, sizeof(uint32));
	im->qfl[0] = 1; for(i=1; i< num-1; i++) im->qfl[i] += im->qfl[i-1]+3; im->qfl[num-1] = im->qfl[num-2]+2;
	for(i=0; i<steps; i++) printf("fl[%d] = %d \n", i, im->qfl[i]);

	printf("Create frame x = %d y = %d p = %p\n", im->width, im->height, im->img);
}

void image_copy(Image *im, uint32 bpp, uchar *v)
///	\fn void image_copy(Image *im, uint32 bpp, uchar *v)
///	\brief Copy image from stream to image structure.
///	\param im	 		The image structure.
///	\param bpp 			The bits per pixel.
///	\param v 			The input stream buffer.
{
	uint32 i, size = im->width*im->height;
	//printf("Start copy  x = %d y = %d p = %p \n", img->size.x, img->size.y, img->img);
	if(bpp > 8) for(i=0; i<size; i++) im->img[i] = (v[i<<1]<<8) | v[(i<<1)+1];
	else 		 for(i=0; i<size; i++) im->img[i] = v[i];
}

void image_dwt_53(Image *im, ColorSpace color, uint32 steps, imgtype *buf)
///	\fn void image_dwt_53(Image *im, ColorSpace color, uint32 steps, imgtype *buf)
///	\brief Discrete wavelets transform of the image.
///	\param im	 		The image structure.
///	\param color		The color space of the stream.
///	\param steps 		The steps of DWT transform.
///	\param buf 			The temporary buffer.
{
	imgtype *s1 = buf, *img = im->img;
	uint32 j, k, h, w, st;
	int i;
	w = im->width; h = im->height;
	Subband *sub = im->sub;

	if(color == BAYER){
		for(j=0; j < h; j++) dwt53_1d_1h(&img[j*w], &s1[j*w], w);
		dwt53_2d_v(s1, img, w, h);
		//dwt53_2d_v_8bit(s1, img, w, h);
		if(steps > 1){
			for(k=0; k<4; k++) {
				st = ((steps-1)*3+1)*k;
				for(i=(steps-1); i>0; i--){
					w = sub[3*i-1+st].size.x + sub[3*i-2+st].size.x;
					h = sub[3*i-1+st].size.y + sub[3*i-2+st].size.y;
					for(j=0; j < h; j++) dwt53_1d_1h(&img[sub[st].loc+j*w], &s1[j*w], w);
				 	dwt53_2d_v(s1, &img[sub[st].loc], w, h);
				}
			}
		}
	} else {
		for(i=steps; i>0; i--){
			for(j=0; j < h; j++) dwt53_1d_1h(&img[j*w], &s1[j*w], w);
			dwt53_2d_v(s1, img, w, h);
			w = sub[3*i-1].size.x;
			h = sub[3*i-2].size.y;
		}
	}
}

void image_idwt_53(Image *im, ColorSpace color, uint32 steps, imgtype *buf, uint32 isteps)
///	\fn void image_idwt_53(Image *im, ColorSpace color, uint32 steps, imgtype *buf, uint32 isteps)
///	\brief Discrete invert wavelets transform of the image.
///	\param im	 		The image structure.
///	\param color		The color space of the stream.
///	\param steps 		The steps of DWT transform.
///	\param buf 			The temporary buffer.
///	\param isteps		The steps of IDWT transform.
{
	imgtype *s1 = buf, *img = im->img;
	uint32 k, i, j, h, w, st;
	Subband *sub = im->sub;

	if(color == BAYER){
		if(steps == 1){
			w = im->width; h = im->height;
			idwt53_2d_v(img, s1, w, h);
			for(j=0; j < h; j++) idwt53_1d_1h(&s1[j*w], &img[j*w], w);
			im->idwts.x = w; im->idwts.y = h;
		} else {
			for(k=0; k<4; k++) {
				for(i=0; i<(isteps-1); i++){
					st = ((steps-1)*3+1)*k;
					w = sub[3*i+1+st].size.x + sub[3*i+2+st].size.x;
					h = sub[3*i+1+st].size.y + sub[3*i+2+st].size.y;
					idwt53_2d_v(&img[sub[st].loc], s1, w, h);
					for(j=0; j < h; j++) idwt53_1d_1h(&s1[j*w], &img[sub[st].loc+j*w], w);
				}
			}
			if(steps == isteps) { w = im->width; h = im->height; }
			else {
				w = sub[3*i+2].size.x + sub[3*i+2 + ((steps-1)*3+1)  ].size.x;
				h = sub[3*i+1].size.y + sub[3*i+1 + ((steps-1)*3+1)*2].size.y;
			}

			idwt53_2d_v(img, s1, w, h);
			for(j=0; j < h; j++) idwt53_1d_1h(&s1[j*w], &img[j*w], w);
			im->idwts.x = w; im->idwts.y = h;
		}
	} else {
		for(i=0; i<isteps; i++){
			w = sub[3*i+1].size.x + sub[3*i+2].size.x;
			h = sub[3*i+1].size.y + sub[3*i+2].size.y;
			//printf("1.y = %d 2.y = %d 2.x = %d 1.x = %d\n",sub[0][3*i+1]->size.y, sub[0][3*i+2]->size.y, sub[0][3*i+2]->size.x, sub[0][3*i+1]->size.x);
			idwt53_2d_v(img, s1, w, h);
			for(j=0; j < h; j++) idwt53_1d_1h(&s1[j*w], &img[j*w], w);
		}
		im->idwts.x = w; im->idwts.y = h;
	}
}

void image_fill_subb(Image *im, ColorSpace color, uint32 steps)
///	\fn void image_fill_subb(Image *im, ColorSpace color, uint32 steps)
///	\brief Fill distribution probability array for each subband after DWT
///			and calculate the number of quantization steps.
///	\param im	 		The image structure.
///	\param color 		The color space of the stream.
///	\param steps 		The steps of DWT transform.
{
	uint32 i, sz, st = ((steps-1)*3+1);
	imgtype *img = im->img;
	im->qst = 0;
	Subband *sub = im->sub;

	sz = (color == BAYER) ? ((steps-1)*3+1)<<2 : steps*3 + 1;
	printf("sz = %d\n  ", sz);
	for(i=0; i < sz; i++) {
		printf("%2d  ", i);
		subband_fill_prob(&img[sub[i].loc], &sub[i]);
		im->qst += sub[i].a_bits-1;
	}
	sz = (color == BAYER) ? 4 : 1;
	for(i=0; i <sz; i++) im->qst -= sub[i*st].a_bits-1; //Remove all LL subbands from quantization
}

void image_fill_hist(Image *im, ColorSpace color, BayerGrid bg, uint32 bpp)
///	\fn void image_fill_hist(Image *im, ColorSpace color, BayerGrid bg, uint32 bpp)
///	\brief Fill color histogram for white balancing.
///	\param im	 		The image structure.
///	\param color 		The color space of the stream.
///	\param bpp 			The bits per pixel.
///	\param bg			The bayer grid pattern
///	\param bpp 			The bits per pixel.
{
	uint32 i, size = im->width*im->height, sz = 1<<bpp, sum;
	uint32	tmp = size;
	if(color == BAYER) {
		utils_fill_bayer_hist(im->img, im->hist, &im->hist[sz], &im->hist[sz*2], im->width, im->height, bg, bpp);
		sum = 0; for(i=0; i<sz; i++) sum +=im->hist[i]; tmp -= sum;
		printf("size = %d r = %d ", size, sum);
		sum = 0; for(i=0; i<sz; i++) sum +=im->hist[sz+i]; tmp -= sum;
		printf("g = %d ", sum);
		sum = 0; for(i=0; i<sz; i++) sum +=im->hist[(sz<<1)+i]; tmp -= sum;
		printf("b = %d  diff = %d\n", sum, tmp);
	}
	else  for(i=0; i < size; i++) im->hist[im->img[i]]++;

}

void image_bits_per_subband(Image *im, ColorSpace color, uint32 steps, uint32 qstep)
///	\fn void image_bits_per_subband(Image *im, ColorSpace color, uint32 steps, uint32 qstep)
///	\brief Bits allocation for quantization algorithm.
///	\param im	 		The image structure.
///	\param color 		The color space of the stream.
///	\param steps 		The steps of DWT transform.
///	\param qstep 		The quantization step  (0 <= qstep < qst).
{
	uint32 i, j, k, sz, df;
	Subband *sub = im->sub;
	//qst = 0;
	if(color == BAYER) {
		sz = (steps-1)*3;

		for(i=0; i < ((sz+1)<<2); i++) sub[i].q_bits = sub[i].a_bits;
		//printf("qstep = %d\n", qstep);
		qstep = max(qstep, im->qst);
		for(i=0; i<50; i++){
			for(k=0; k<4; k++){
				switch(k){
					case(0):{ df = im->qfl[max(i,steps-1)]; break;}
					case(1):{ df = i>0 ? im->qfl[max(i-1,steps-1)] : 0; break;}
					case(2):{ df = i>0 ? im->qfl[max(i-1,steps-1)] : 0; break;}
					case(3):{ df = i>1 ? im->qfl[max(i-2,steps-1)] : 0; break;}
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
		sz = steps*3;
		for(i=0; i < (sz+1); i++) sub[i].q_bits = sub[i].a_bits;
		//printf("stmax = %d\n", stmax);
		qstep = max(qstep, im->qst);
		for(i=0; ; i++){
			for(j=0; j < im->qfl[max(i,steps)]; j++){
				if(sub[sz-j].q_bits > 1) { sub[sz-j].q_bits--; qstep--;}
				if(!qstep) break;
			}
			if(!qstep) break;
		}
		//for(i=0; i < sz+1; i++) printf("%2d ", sub[0][i].q_bits);
		//printf("\n");
	}
}

uint32 image_size(Image *im, ColorSpace color, uint32 steps, uint32 qstep)
///	\fn uint32 image_size(Image *im, ColorSpace color, uint32 steps, uint32 qstep)
///	\brief Estimate the image size after quantization and entropy encoder.
///	\param im	 		The image structure.
///	\param color 		The color space of the stream.
///	\param steps 		The steps of DWT transform.
///	\param qstep 		The quantization step  (0 <= qstep < qst).
///	\retval				The size of image in bits.
{
	uint32 i, sz, s=0;
	Subband *sub = im->sub;

	if(qstep) image_bits_per_subband(im, color, steps,  qstep);

	sz = (color == BAYER) ? ((steps-1)*3+1)<<2 : steps*3 + 1;
	for(i=0; i < sz; i++) {
		if(sub[i].q_bits > 1) s += subband_size(&sub[i]);
	}
	return s;
}

void image_bits_alloc(Image *im, ColorSpace color, uint32 steps, uint32 bpp, uint32 times)
///	\fn void image_bits_alloc(Image *im, ColorSpace color, uint32 steps, uint32 bpp, uint32 times)
///	\brief Bits allocation for subbands for given compression times.
///	\param im	 		The image structure.
///	\param color 		The color space of the stream.
///	\param steps 		The steps of DWT transform.
///	\param bpp 			The bits per pixel.
///	\param times		Compression times.
{
	uint32 i, s, qstep, size;

	size = (im->width*im->height*bpp)/times;
	qstep = (im->qst>>1);

	for(i=2;;i++){
		s = image_size(im, color, steps, qstep);
		printf("qst = %d size = %d qstep = %d s = %d\n", im->qst, size>>3, qstep, s>>3);
		qstep = (s < size) ? qstep - (im->qst>>i) : qstep + (im->qst>>i);
		if(!(im->qst>>i)) break;
	}
}

void image_quantization(Image *im, ColorSpace color, uint32 steps)
///	\fn void image_quantization(Image *im, ColorSpace color, uint32 steps)
///	\brief Image quantization.
///	\param im	 		The image structure.
///	\param color 		The color space of the stream.
///	\param steps 		The steps of DWT transform.
{
	uint32 i, sz;
	Subband *sub = im->sub;
	imgtype *img = im->img;

	sz = (color == BAYER) ? ((steps-1)*3+1)<<2 : steps*3 + 1;
	for(i=0; i < sz; i++) {
		//printf("%2d bits = %d q_bits = %d \n", i, sub[i].a_bits, sub[i].q_bits);
		subband_quantization(&img[sub[i].loc], &sub[i]);
	}
}

uint32 image_range_encode(Image *im, ColorSpace color, uint32 steps, uint32 bpp, uchar *buf)
///	\fn uint32 image_range_encode(Image *im, ColorSpace color, uint32 steps, uint32 bpp, uchar *buf)
///	\brief Image range encoder.
///	\param im	 		The image structure.
///	\param color 		The color space of the stream.
///	\param steps 		The steps of DWT transform.
///	\param bpp 			The bits per pixel.
///	\param buf 			The buffer for encoded data.
///	\retval				The size of encoded image in bytes.
{
	uint32 i, sq, sz;
	uint32 size = 0;
	imgtype *img = im->img;
	Subband *sub = im->sub;

	sz = (color == BAYER) ? ((steps-1)*3+1)<<2 : steps*3 + 1;
	for(i=0; i < sz; i++) {
		sq = sub[i].size.x*sub[i].size.y;
		//printf("%d a_bits = %d q_bits = %d bits = %d\n", i, sub[i].a_bits, sub[i].q_bits, (sub[i].a_bits<<4) | sub[i].q_bits);
		if(sub[i].q_bits >1){
			subband_encode_table(&sub[i]);
			size += range_encoder(&img[sub[i].loc], &sub[i].dist[1<<(bpp+2)],sq, sub[i].a_bits, sub[i].q_bits, &buf[size], sub[i].q);
		}
	}
	printf("Finish range_encoder\n");
	return size;
}

uint32 image_range_decode(Image *im, ColorSpace color, uint32 steps, uint32 bpp, uchar *buf)
///	\fn uint32 image_range_decode(Image *im, ColorSpace color, uint32 steps, uint32 bpp, uchar *buf)
///	\brief Image range decoder.
///	\param im	 		The image structure.
///	\param color 		The color space of the stream.
///	\param steps 		The steps of DWT transform.
///	\param bpp 			The bits per pixel.
///	\param buf 			The buffer for encoded data.
///	\retval				The size of decoded image in bytes.
{
	uint32 i, j, sq, sz;
	uint32 size = 0;
	imgtype *img = im->img;
	Subband *sub = im->sub;

	sz = (color == BAYER) ? ((steps-1)*3+1)<<2 : steps*3 + 1;
	//i=0; {
	for(i=0; i < sz; i++) {
		sq = sub[i].size.x*sub[i].size.y;
		if(sub[i].q_bits >1){
			subband_decode_table(&sub[i]);
			size += range_decoder(&img[sub[i].loc], &sub[i].dist[1<<(bpp+2)],sq, sub[i].a_bits, sub[i].q_bits, &buf[size], sub[i].q);
		} else for(j=0; j<sq; j++) img[sub[i].loc+j] = 0;
		//printf("%d a_bits = %d q_bits = %d size = %d\n", i, sub[i].a_bits, sub[i].q_bits, size);
	}
	return size;
}
