#include <walet.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define lim(max,min, x)  	((x)>max ? max :((x)<min ? min : (x)))
#define max(x, m) 			((x>m) ? (m) : (x))

static inline void dwt_haar_1d(imgtype *in, imgtype *out, const uint32 w)
///	\fn static inline void dwt_haar_1d(imgtype *in, imgtype *out, const uint32 w)
///	\brief 1D Haar DWT.
///	\param in	 		The input line.
///	\param out 			The output line.
///	\param w 			The line width.
{
	int i, j, sh = (w>>1) + (w&1), sh1 = (w>>1);
	for(i=0, j=0; j < sh1; i+=2, j+=1){
		out[j]		= (in[i] + in[i+1]);//>>1;
		out[sh+j]	= (in[i] - in[i+1]);//>>1;
	}
	if(w&1) out[j] 	= in[i];
}

static inline void idwt_haar_1d(imgtype *in, imgtype *out, const uint32 w)
///	\fn static inline void idwt_haar_1d(imgtype *in, imgtype *out, const uint32 w)
///	\brief 1D Haar IDWT.
///	\param in	 		The input line.
///	\param out 			The output line.
///	\param w 			The line width.
{
	int i, j, sh = (w>>1) + (w&1), sh1 = (w>>1);
	for(i=0, j=0; j < sh1; i+=2, j+=1){
		out[i]		= (in[j] + in[sh+j])>>1;
		out[i+1]	= (in[j] - in[sh+j])>>1;
	}
	if(w&1) out[i] 	= in[j];
}

static inline void dwt_haar_2d(imgtype *in, imgtype *out, const uint32 w, const uint32 h)
///	\fn static inline void dwt53_2d_v(imgtype *in, imgtype *out, const uint32 w, const uint32 h)
///	\brief 2D Haar vertical DWT.
///	\param in	 		The input image data.
///	\param out 			The output image data.
///	\param w 			The image width.
///	\param h 			The image height.
{
	uint32 h2 = (h>>1), h1 = (h>>1) + (h&1), w2 = (w>>1), w1 = (w>>1) + (w&1);
	uint32 x, y, y1, y2, s[4], sz = ((h>>1)<<1)*w;

	s[0] = 0; s[1] = w1*h1; s[2] = s[1] + w2*h1; s[3] = s[2] + w1*h2;

	for(y=0, y1=0, y2=0; y < sz; y+=(w<<1), y1+=w1, y2+=w2){
		for(x=0; x<w1; x++){
			out[y1+x     ] = (in[y+x] + in[y+x+w]);//>>1;
			out[s[2]+y1+x] = (in[y+x] - in[y+x+w]);//>>1;
		}
		for(   ; x<w; x++){
			out[s[1]+y2+x-w1] = (in[y+x] + in[y+x+w]);//>>1;
			out[s[3]+y2+x-w1] = (in[y+x] - in[y+x+w]);//>>1;
		}
		//for(x=0; x<w2; x++){
		//	out[s[1]+y2+x] = (in[y+x+w1] + in[y+x+w1+w])>>1;
		//	out[s[3]+y2+x] = (in[y+x+w1] - in[y+x+w1+w])>>1;
		//}
	}
	if(h&1){
		for(x=0; x<w1; x++){
			out[y1+x] = in[y+x];
		}
		for(x=0; x<w2; x++){
			out[s[1]+y2+x] = in[y+x+w1];
		}
	}
}

static inline void idwt_haar_2d(imgtype *in, imgtype *out, const uint32 w, const uint32 h, uint32 *loc)
///	\fn static inline void dwt53_2d_v(imgtype *in, imgtype *out, const uint32 w, const uint32 h)
///	\brief 2D Haar vertical DWT.
///	\param in	 		The input image data.
///	\param out 			The output image data.
///	\param w 			The image width.
///	\param h 			The image height.
///	\param loc 			The subband location.
{
	uint32 h2 = (h>>1), h1 = (h>>1) + (h&1), w2 = (w>>1), w1 = (w>>1) + (w&1);
	uint32 x, y, y1, y2, s[4], sz = ((h>>1)<<1)*w;

	if(loc){ s[0] = loc[0]; s[1] = loc[1]; s[2] = loc[2]; s[3] = loc[3]; }
	else 	{ s[0] = 0;	s[1] = w1*h1;  s[2] = s[1] + w2*h1; s[3] = s[2] + w1*h2; }

	for(y=0, y1=0, y2=0; y < sz; y+=(w<<1), y1+=w1, y2+=w2){
		for(x=0; x<w1; x++){
			in[y+x] 	= (out[y1+x] + out[s[2]+y1+x])>>1;
			in[y+x+w] 	= (out[y1+x] - out[s[2]+y1+x])>>1;
		}
		for(x=0; x<w2; x++){
			in[y+x+w1] 		= (out[s[1]+y2+x] + out[s[3]+y2+x])>>1;
			in[y+x+w+w1] 	= (out[s[1]+y2+x] - out[s[3]+y2+x])>>1;
		}
	}
	if(h&1){
		for(x=0; x<w1; x++){
			in[y+x] = out[y1+x];
		}
		for(x=0; x<w2; x++){
			in[y+x+w1] = out[s[1]+y2+x];
		}
	}
}

static inline void dwt_haar_2d_one(imgtype *in, imgtype *out, const uint32 w, const uint32 h)
///	\fn static inline void dwt53_2d_v(imgtype *in, imgtype *out, const uint32 w, const uint32 h)
///	\brief One step 2D Haar DWT.
///	\param in	 		The input image data.
///	\param out 			The output image data.
///	\param w 			The image width.
///	\param h 			The image height.
{
	uint32 j, sz = w*h;
	for(j=0; j < sz; j+=w) dwt_haar_1d(&in[j], &out[j], w);
	dwt_haar_2d(out, in, w, h);

}

static inline void idwt_haar_2d_one(imgtype *in, imgtype *out, const uint32 w, const uint32 h, uint32 *loc)
///	\fn static inline void dwt53_2d_v(imgtype *in, imgtype *out, const uint32 w, const uint32 h)
///	\brief One step 2D Haar IDWT.
///	\param in	 		The input image data.
///	\param out 			The output image data.
///	\param w 			The image width.
///	\param h 			The image height.
{
	uint32 j, sz = w*h;
	idwt_haar_2d(in, out, w, h, loc);
	for(j=0; j < sz; j+=w) idwt_haar_1d(&out[j], &in[j], w);

}

static inline void dwt_53_1d(imgtype *in, imgtype *out, const uint32 w)
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

static inline void idwt_53_1d(imgtype *in, imgtype *out, const uint32 w)
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

static inline void dwt_53_2d(imgtype *in, imgtype *out, const uint32 w, const uint32 h)
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

static inline void idwt_53_2d(imgtype *in, imgtype *out, const uint32 w, const uint32 h, uint32 *loc)
///	\fn static inline void idwt53_2d_v(imgtype *in, imgtype *out, const uint32 w, const uint32 h)
///	\brief 2D 5.3 vertical invert wavelet transform.
///	\param in	 		The input image data.
///	\param out 			The output image data.
///	\param w 			The image width.
///	\param h 			The image height.
///	\param loc 			The subband location.
{
	uint32 i, k=0;
	uint32 h2 = (h>>1), h1 = (h>>1) + (h&1), w2 = (w>>1), w1 = (w>>1) + (w&1);
	uint32 s[4];
	int kw1, kwi, kw, k1;

	if(loc){ s[0] = loc[0]; s[1] = loc[1]; s[2] = loc[2]; s[3] = loc[3]; }
	else 	{ s[0] = 0;	s[1] = w1*h1;  s[2] = s[1] + w2*h1; s[3] = s[2] + w1*h2; }

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

static inline void dwt_53_2d_one(imgtype *in, imgtype *out, const uint32 w, const uint32 h)
///	\fn static inline void dwt53_2d_v(imgtype *in, imgtype *out, const uint32 w, const uint32 h)
///	\brief One step 2D 53 DWT.
///	\param in	 		The input image data.
///	\param out 			The output image data.
///	\param w 			The image width.
///	\param h 			The image height.
{
	uint32 j, sz = w*h;
	for(j=0; j < sz; j+=w) dwt_53_1d(&in[j], &out[j], w);
	dwt_53_2d(out, in, w, h);

}

static inline void idwt_53_2d_one(imgtype *in, imgtype *out, const uint32 w, const uint32 h, uint32 *loc)
///	\fn static inline void dwt53_2d_v(imgtype *in, imgtype *out, const uint32 w, const uint32 h)
///	\brief One step 53 Haar IDWT.
///	\param in	 		The input image data.
///	\param out 			The output image data.
///	\param w 			The image width.
///	\param h 			The image height.
{
	uint32 j, sz = w*h;
	idwt_53_2d(in, out, w, h, loc);
	for(j=0; j < sz; j+=w) idwt_53_1d(&out[j], &in[j], w);
}

static void dwt(Image *im, imgtype *buf, uint32 steps, funwt dwt_one)
///	\fn static void image_dwt1(Image *im, imgtype *buf, uint32 steps, funwt dwt_one)
///	\brief Discrete wavelets transform.
///	\param im	 		The image structure.
///	\param buf 			The temporary buffer.
///	\param steps 		The steps of DWT transform.
/// \param funwt		The function for one step 2d DWT.
{
	imgtype *img = im->img;
	uint32 j, h, w;
	int i;
	w = im->width; h = im->height;
	Subband *sub = im->sub;

	for(i=steps; i > 0; i--){
		(*dwt_one)(im->img, buf, w, h);
		w = sub[3*i-1].size.x;
		h = sub[3*i-2].size.y;
	}
}

static void dwt_mallet(Image *im, imgtype *buf, uint32 steps, funwt dwt_one)
//	\fn static void image_mallet_dwt(Image *im, imgtype *buf, uint32 steps, funwt dwt_one)
///	\brief Discrete wavelets transform.
///	\param im	 		The image structure.
///	\param buf 			The temporary buffer.
///	\param steps 		The steps of DWT transform.
/// \param funwt		The function for one step 2d DWT.
{
	imgtype *img = im->img;
	uint32 j, k, h, w, st;
	int i;
	w = im->width; h = im->height;
	Subband *sub = im->sub;

	(*dwt_one)(im->img, buf, w, h);
	if(steps > 1){
		for(k=0; k<4; k++) {
			st = ((steps-1)*3+1)*k;
			for(i=(steps-1); i>0; i--){
				w = sub[3*i-1+st].size.x + sub[3*i-2+st].size.x;
				h = sub[3*i-1+st].size.y + sub[3*i-2+st].size.y;
				(*dwt_one)(&img[sub[st].loc], &buf[sub[st].loc], w, h);
			}
		}
	}
}

static void idwt(Image *im, imgtype *buf, uint32 steps,  uint32 isteps, funidwt idwt_one)
///	\fn static void image_idwt1(Image *im, imgtype *buf, uint32 steps,  uint32 isteps, funwt idwt_one)
///	\brief Discrete invert wavelets transform of the image.
///	\param im	 		The image structure.
///	\param buf 			The temporary buffer.
///	\param steps 		The steps of DWT transform.
///	\param isteps		The steps of IDWT transform.
/// \param funwt		The function for one step 2d IDWT.
{
	imgtype *img = im->img;
	uint32 i, h, w;
	Subband *sub = im->sub;

	for(i=0; i<isteps; i++){
			w = sub[3*i+1].size.x + sub[3*i+2].size.x;
			h = sub[3*i+1].size.y + sub[3*i+2].size.y;
			(*idwt_one)(im->img, buf, w, h, NULL);
		}
		im->idwts.x = w; im->idwts.y = h;
}

static void idwt_mallet(Image *im, imgtype *buf, uint32 steps,  uint32 isteps, funidwt idwt_one)
///	\fn static void image_mallet_idwt(Image *im, imgtype *buf, uint32 steps,  uint32 isteps, funwt idwt_one)
///	\brief Discrete invert wavelets transform of the image.
///	\param im	 		The image structure.
///	\param buf 			The temporary buffer.
///	\param steps 		The steps of DWT transform.
///	\param isteps		The steps of IDWT transform.
/// \param funwt		The function for one step 2d IDWT.
{
	imgtype *img = im->img;
	uint32 j, k, h, w, st, sz = (steps-1)*3+1;
	uint32 s[4];
	int i;
	w = im->width; h = im->height;
	Subband *sub = im->sub;

	if(steps == 1){
		(*idwt_one)(im->img, buf, w, h, NULL);
		im->idwts.x = w; im->idwts.y = h;
	} else {
		for(k=0; k<4; k++) {
			for(i=0; i<(isteps-1); i++){
				st = sz*k;
				w = sub[3*i+1+st].size.x + sub[3*i+2+st].size.x;
				h = sub[3*i+1+st].size.y + sub[3*i+2+st].size.y;
				(*idwt_one)(&img[sub[st].loc], &buf[sub[st].loc], w, h, NULL);
			}
		}
		if(steps == isteps) {
			w = im->width; h = im->height;
			(*idwt_one)(img, buf, w, h, NULL);
			im->idwts.x = w; im->idwts.y = h;
		}
		else {
			w = sub[3*i+2].size.x + sub[3*i+2+sz  ].size.x;
			h = sub[3*i+1].size.y + sub[3*i+1+sz*2].size.y;
			s[0] = 0; s[1] = sub[sz].loc; s[2] = sub[sz*2].loc; s[3] = sub[sz*3].loc;
			(*idwt_one)(img, buf, w, h, s);
			im->idwts.x = w; im->idwts.y = h;
			//w = sub[3*i+2].size.x + sub[3*i+2 + ((steps-1)*3+1)  ].size.x;
			//h = sub[3*i+1].size.y + sub[3*i+1 + ((steps-1)*3+1)*2].size.y;
		}
	}
}
/*
static void image_dwt(Image *im, ColorSpace color, uint32 steps, imgtype *buf, fun1d dwt_1d, fun2d dwt_2d)
///	\fn static void image_dwt(Image *im, ColorSpace color, uint32 steps, imgtype *buf, fun_1d dwt_1d, fun_2d dwt_2d)
///	\brief Discrete wavelets transform of the image.
///	\param im	 		The image structure.
///	\param color		The color space of the stream.
///	\param steps 		The steps of DWT transform.
///	\param buf 			The temporary buffer.
/// \param dwt_1d		The function for 1d horizontal DWT.
/// \param dwt_2d		The function for 2d vertical DWT.
{
	imgtype *s1 = buf, *img = im->img;
	uint32 j, k, h, w, st;
	int i;
	w = im->width; h = im->height;
	Subband *sub = im->sub;

	if(color == BAYER){
		for(j=0; j < h; j++) (*dwt_1d)(&img[j*w], &s1[j*w], w);
		(*dwt_2d)(s1, img, w, h);
		if(steps > 1){
			for(k=0; k<4; k++) {
				st = ((steps-1)*3+1)*k;
				for(i=(steps-1); i>0; i--){
					w = sub[3*i-1+st].size.x + sub[3*i-2+st].size.x;
					h = sub[3*i-1+st].size.y + sub[3*i-2+st].size.y;
					for(j=0; j < h; j++) (*dwt_1d)(&img[sub[st].loc+j*w], &s1[j*w], w);
					(*dwt_2d)(s1, &img[sub[st].loc], w, h);
				}
			}
		}
	} else {
		for(i=steps; i>0; i--){
			for(j=0; j < h; j++) (*dwt_1d)(&img[j*w], &s1[j*w], w);
			(*dwt_2d)(s1, img, w, h);
			w = sub[3*i-1].size.x;
			h = sub[3*i-2].size.y;
		}
	}
}

static void image_idwt(Image *im, ColorSpace color, uint32 steps, imgtype *buf, uint32 isteps, fun1d idwt_1d, fun2d idwt_2d)
///	\fn static void image_idwt(Image *im, ColorSpace color, uint32 steps, imgtype *buf, uint32 isteps, fun_1d idwt_1d, fun_2d idwt_2d)
///	\brief Discrete invert wavelets transform of the image.
///	\param im	 		The image structure.
///	\param color		The color space of the stream.
///	\param steps 		The steps of DWT transform.
///	\param buf 			The temporary buffer.
///	\param isteps		The steps of IDWT transform.
/// \param idwt_1d		The function for 1d horizontal IDWT.
/// \param idwt_2d		The function for 2d vertical IDWT.
{
	imgtype *s1 = buf, *img = im->img;
	uint32 k, i, j, h, w, st;
	Subband *sub = im->sub;

	if(color == BAYER){
		if(steps == 1){
			w = im->width; h = im->height;
			(*idwt_2d)(img, s1, w, h);
			for(j=0; j < h; j++) (*idwt_1d)(&s1[j*w], &img[j*w], w);
			im->idwts.x = w; im->idwts.y = h;
		} else {
			for(k=0; k<4; k++) {
				for(i=0; i<(isteps-1); i++){
					st = ((steps-1)*3+1)*k;
					w = sub[3*i+1+st].size.x + sub[3*i+2+st].size.x;
					h = sub[3*i+1+st].size.y + sub[3*i+2+st].size.y;
					(*idwt_2d)(&img[sub[st].loc], s1, w, h);
					for(j=0; j < h; j++) (*idwt_1d)(&s1[j*w], &img[sub[st].loc+j*w], w);
				}
			}
			if(steps == isteps) {
				w = im->width; h = im->height;
				(*idwt_2d)(img, s1, w, h);
				for(j=0; j < h; j++) (*idwt_1d)(&s1[j*w], &img[j*w], w);
				im->idwts.x = w; im->idwts.y = h;
			} else {
				w = sub[3*i+2].size.x + sub[3*i+2 + ((steps-1)*3+1)  ].size.x;
				h = sub[3*i+1].size.y + sub[3*i+1 + ((steps-1)*3+1)*2].size.y;
			}
		}
	} else {
		for(i=0; i<isteps; i++){
			w = sub[3*i+1].size.x + sub[3*i+2].size.x;
			h = sub[3*i+1].size.y + sub[3*i+2].size.y;
			//printf("1.y = %d 2.y = %d 2.x = %d 1.x = %d\n",sub[0][3*i+1]->size.y, sub[0][3*i+2]->size.y, sub[0][3*i+2]->size.x, sub[0][3*i+1]->size.x);
			(*idwt_2d)(img, s1, w, h);
			for(j=0; j < h; j++) (*idwt_1d)(&s1[j*w], &img[j*w], w);
		}
		im->idwts.x = w; im->idwts.y = h;
	}
}
*/

void image_dwt(Image *im, ColorSpace color, uint32 steps, imgtype *buf, TransformType tt, FilterBank fb)
///	\fn void image_dwt_53(Image *im, ColorSpace color, uint32 steps, imgtype *buf)
///	\brief Discrete 5/3 filter wavelets transform of the image.
///	\param im	 		The image structure.
///	\param color		The color space of the stream.
///	\param steps 		The steps of DWT transform.
///	\param buf 			The temporary buffer.
{
	if(tt == CLASSIC){
		if(fb == FR_HAAR)	dwt(im, buf, steps, &dwt_haar_2d_one);
		else				dwt(im, buf, steps, &dwt_53_2d_one);
	} else {
		if(fb == FR_HAAR)	dwt_mallet(im, buf, steps, &dwt_haar_2d_one);
		else				dwt_mallet(im, buf, steps, &dwt_53_2d_one);
	}
}

void image_idwt(Image *im, ColorSpace color, uint32 steps, imgtype *buf, uint32 isteps, TransformType tt, FilterBank fb)
///	\fn void image_idwt_53(Image *im, ColorSpace color, uint32 steps, imgtype *buf, uint32 isteps)
///	\brief Discrete 5/3 filter invert wavelets transform of the image.
///	\param im	 		The image structure.
///	\param color		The color space of the stream.
///	\param steps 		The steps of DWT transform.
///	\param buf 			The temporary buffer.
///	\param isteps		The steps of IDWT transform.
{
	if(tt == CLASSIC){
		if(fb == FR_HAAR)	idwt(im, buf, steps, isteps, &idwt_haar_2d_one);
		else				idwt(im, buf, steps, isteps, &idwt_53_2d_one);
	} else {
		if(fb == FR_HAAR)	idwt_mallet(im, buf, steps, isteps, &idwt_haar_2d_one);
		else				idwt_mallet(im, buf, steps, isteps, &idwt_53_2d_one);
	}
}

static void fill_bayer_hist(imgtype *img, uint32 *r, uint32 *g, uint32 *b, uint32 w, uint32 h,  BayerGrid bay, uint32 bits)
{
//
//   All RGB cameras use one of these Bayer grids:
//
//	BGGR  0         GRBG 1          GBRG  2         RGGB 3
//	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5
//	0 B G B G B G	0 G R G R G R	0 G B G B G B	0 R G R G R G
//	1 G R G R G R	1 B G B G B G	1 R G R G R G	1 G B G B G B
//	2 B G B G B G	2 G R G R G R	2 G B G B G B	2 R G R G R G
//	3 G R G R G R	3 B G B G B G	3 R G R G R G	3 G B G B G B
//
	uint32 x, y, i, size = h*w;
	uint32 *c[4];
	memset(r, 0, sizeof r);
	memset(g, 0, sizeof g);
	memset(b, 0, sizeof b);

	switch(bay){
		case(BGGR):{ c[0] = r; c[1] = g; c[2] = g; c[3] = b; break;}
		case(GRBG):{ c[0] = g; c[1] = b; c[2] = r; c[3] = g; break;}
		case(GBRG):{ c[0] = g; c[1] = r; c[2] = b; c[3] = g; break;}
		case(RGGB):{ c[0] = b; c[1] = g; c[2] = g; c[3] = r; break;}
	}

	for(i=0, x=0, y=0; i < size; i++, x++){
		if(x == w) { x=0; y++;}
		if(y&1)
			if(x&1) c[0][img[i]]++;
			else 	c[1][img[i]]++;
		else
			if(x&1)	c[2][img[i]]++;
			else 	c[3][img[i]]++;
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
		fill_bayer_hist(im->img, im->hist, &im->hist[sz], &im->hist[sz*2], im->width, im->height, bg, bpp);
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

void image_median_filter(Image *im, ColorSpace color, BayerGrid bg, imgtype *buf)
///	\fn void image_median_filter(Image *im, ColorSpace color, BayerGrid bg, imgtype *buf)
///	\brief Image median filter.
///	\param im	 		The image structure.
///	\param color 		The color space of the stream.
///	\param steps 		The steps of DWT transform.
{
	if(color == BAYER) filters_bayer_median_3x3(im->img, buf, im->width, im->height, bg);
	else filters_median_3x3(im->img, buf, im->width, im->height);
}

void image_subband_median_filter(Image *im, ColorSpace color, uint32 steps, imgtype *buf)
///	\fn uint32 image_range_decode(Image *im, ColorSpace color, uint32 steps, uint32 bpp, uchar *buf)
///	\brief Image range decoder.
///	\param im	 		The image structure.
///	\param color 		The color space of the stream.
///	\param steps 		The steps of DWT transform.
///	\param bpp 			The bits per pixel.
///	\param buf 			The buffer for encoded data.
///	\retval				The size of decoded image in bytes.
{
	uint32 i, sz;
	imgtype *img = im->img;
	Subband *sub = im->sub;

	sz = (color == BAYER) ? ((steps-1)*3+1)<<2 : steps*3 + 1;
	for(i=0; i < sz; i++) filters_median_3x3(&img[sub[i].loc], &buf[sub[i].loc], sub[i].size.x, sub[i].size.y);
}
