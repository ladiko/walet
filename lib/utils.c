#include <walet.h>

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <math.h>

#define hsh(w,x) ((x == -2) ? -w-w :(x == 2))

// Red and blue pattern for bayer median filter
int rb[9][2] = {
		{-2,-2,},{ 0,-2,},{ 2,-2,},    	// R   R   R
		{-2, 0,},{ 0, 0,},{ 2, 0,},		//
		{-2, 2,},{ 0, 2,},{ 2, 2,},		// R   R   R
};										//
										// R   R   R
// Green pattern for bayer median filter
int g[9][2] = {
		{ 0,-2,},{ 1,-1,},{ 2, 0,},		//     G
		{-1,-1,},{ 0, 0,},{ 1, 1,},		//   G   G
		{-2, 0,},{-1, 1,},{ 0,-2,},		// G   G   G
};										//   G   G
										//     G


#define ll(step, x, y) img[x*step + y*step*width];
#define hl(step, x, y) img[x*step + (step>>1)  + y*step*width];
#define lh(step, x, y) img[x*step + (step>>1)*width  + y*step*width];
#define hh(step, x, y) img[x*step + (step>>1) + (step>>1)*width  + y*step*width];

//#define clip(x)		x < 0 ? (x < -255 ? 255 : -x) : ( x > 255 ? 255 : x);
#define clip(x)		abs(x);



static inline void drawrect(uchar *rgb, imgtype *im, uint32 w0, uint32 h0, uint32 w, uint32 h, uint32 size, uint32 shift)
{
	uint32 x, y, tmp;
	for(y=0; y < h; y++ ){
		for(x=0; x < w; x++){
			//tmp = rnd(im[y*w+x] < 0 ? -im[y*w+x]<<1 : im[y*w+x]<<1);
			tmp = rnd(shift+im[y*w+x]);
			rgb[3*((y+h0)*size+w0+x)]   = tmp; //rnd(shift+im[y*w+x]); //im[y*w+x] ? 255 : 0; //
			rgb[3*((y+h0)*size+w0+x)+1] = tmp; //rnd(shift+im[y*w+x]); //im[y*w+x] ? 255 : 0; //
			rgb[3*((y+h0)*size+w0+x)+2] = tmp; //rnd(shift+im[y*w+x]); //im[y*w+x] ? 255 : 0; //
		}
	}
}

uchar* utils_subband_draw(Image *img, uchar *rgb, ColorSpace color, uint32 steps)
//For vizualisation only
{
	uint32 k, h, w, h0, w0, h1, w1, st;
	int i;
	imgtype* im;
	Subband *sub = img->sub;
	//for(i=0; i<=steps*3; i++) printf("sub %d h %d w %d p %p\n",sub[0][i]->subb, sub[0][i]->size.y, sub[0][i]->size.x,  sub[0][i ]);
	if(color == BAYER ){
		if(steps > 1){
			for(k=0; k<4; k++) {
				st = ((steps-1)*3+1);
				switch(k){
				case 0 : {h1 = 0; w1 = 0; break;}
				case 1 : {h1 = 0; w1 = sub[3*(steps-1)-1].size.x + sub[3*(steps-1)-2].size.x; break;}
				case 2 : {h1 = sub[3*(steps-1)-2].size.y + sub[3*(steps-1)-1].size.y; w1 = 0; break;}
				case 3 : {h1 = sub[3*(steps-1)-2].size.y + sub[3*(steps-1)-1].size.y;
						  w1 = sub[3*(steps-1)-1].size.x + sub[3*(steps-1)-2].size.x; break;}
				}
				for(i=(steps-1); i>0; i--){
					h0 = sub[k*st + 3*i-2].size.y; w0 = sub[k*st + 3*i-1].size.x;
					h  = sub[k*st + 3*i-2].size.y;  w = sub[k*st + 3*i-2].size.x;
					im = &img->img[sub[k*st + 3*i-2].loc];
					drawrect(rgb, im, w1+w0, h1, w, h, img->width, 128);

					h = sub[k*st + 3*i-1].size.y; w = sub[k*st + 3*i-1].size.x;
					im = &img->img[sub[k*st + 3*i-1].loc];
					drawrect(rgb, im, w1, h1+h0, w, h, img->width, 128);

					h = sub[k*st + 3*i].size.y; w = sub[k*st + 3*i].size.x;
					im = &img->img[sub[k*st + 3*i].loc];
					drawrect(rgb, im, w1+w0, h1+h0, w, h, img->width, 128);
				}
				h = sub[k*st ].size.y; w = sub[k*st].size.x;
				im = &img->img[sub[k*st].loc];
				drawrect(rgb, im, w1, h1, w, h, img->width, 128);
			}
		} else {
			h0 = sub[0 ].size.y; w0 = sub[0 ].size.x;
			h  = sub[1].size.y; w  = sub[1].size.x;
			im = &img->img[sub[1].loc];
			drawrect(rgb, im, w0, 0, w, h, img->width, 128);

			h = sub[2].size.y; w = sub[2].size.x;
			im = &img->img[sub[2].loc];
			drawrect(rgb, im, 0, h0, w, h, img->width, 128);

			h = sub[3].size.y; w = sub[3].size.x;
			im = &img->img[sub[3].loc];
			drawrect(rgb, im, w0, h0, w, h, img->width, 128);

			h = sub[0].size.y; w = sub[0].size.x;
			im = &img->img[sub[0].loc];
			drawrect(rgb, im, 0, 0, w, h, img->width, 0);
		}
	} else {
		for(i=steps; i>0; i--){
			h0 = sub[3*i-2].size.y; w0 = sub[3*i-1].size.x;
			h  = sub[3*i-2].size.y; w  = sub[3*i-2].size.x;
			im = &img->img[sub[3*i-2].loc];
			drawrect(rgb, im, w0, 0, w, h, img->width, 128);

			h = sub[3*i-1].size.y; w = sub[3*i-1].size.x;
			im = &img->img[sub[3*i-1].loc];
			drawrect(rgb, im, 0, h0, w, h, img->width, 128);

			h = sub[3*i].size.y; w = sub[3*i].size.x;
			im = &img->img[sub[3*i].loc];
			drawrect(rgb, im, w0, h0, w, h, img->width, 128);
		}
		h = sub[0].size.y; w = sub[0].size.x;
		im = &img->img[sub[0].loc];
		drawrect(rgb, im, 0, 0, w, h, img->width, 0);
	}
	return rgb;
}

#define oe(a,x)	(a ? x&1 : (x+1)&1)
//#define lb(x) (x&0xFF)
#define lb(x) (((x) < 0) ? 0 : (((x) > 255) ? 255 : (x)))

uchar* utils_bayer_draw(imgtype *img, uchar *rgb, uint32 w, uint32 h,  BayerGrid bay)
/*! \fn void bayer_to_rgb(uchar *rgb)
	\brief DWT picture transform.
  	\param	rgb 	The pointer to rgb array.
*/
{
/*
   All RGB cameras use one of these Bayer grids:

	BGGR  0         GRBG 1          GBRG  2         RGGB 3
	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5
	0 B G B G B G	0 G R G R G R	0 G B G B G B	0 R G R G R G
	1 G R G R G R	1 B G B G B G	1 R G R G R G	1 G B G B G B
	2 B G B G B G	2 G R G R G R	2 G B G B G B	2 R G R G R G
	3 G R G R G R	3 B G B G B G	3 R G R G R G	3 G B G B G B
 */
	uint32 x, y, wy, xwy, xwy3, y2, x2, a, b, h1 = h-1, w1 = w-1, yw, yw1;

	switch(bay){
		case(BGGR):{ a = 1; b = 1; break;}
		case(GRBG):{ a = 0; b = 1; break;}
		case(GBRG):{ a = 1; b = 0; break;}
		case(RGGB):{ a = 0; b = 0; break;}
	}

	for(y=0, yw=0, yw1=0 ; y < h1; y++, yw+=w, yw1+=w1){
		for(x=0; x < w1; x++){
			y2 = oe(a,y);
			x2 = oe(b,x);
			xwy = x + yw;
			wy 	= x + yw1;
			xwy3 = wy + wy + wy;
			rgb[xwy3    ] = y2 ? (x2 ?  lb(img[xwy    ]) : lb(img[xwy+1])) : (x2 ? lb(img[xwy+w]) : lb(img[xwy+w+1]));
			rgb[xwy3 + 1] = y2 ? (x2 ? (lb(img[xwy+w  ]) + lb(img[xwy+1]))>>1 :   (lb(img[xwy  ]) + lb(img[xwy+w+1]))>>1) :
								 (x2 ? (lb(img[xwy+w+1]) + lb(img[xwy  ]))>>1 :   (lb(img[xwy+1]) + lb(img[xwy+w  ]))>>1);
			rgb[xwy3 + 2] = y2 ? (x2 ?  lb(img[xwy+w+1]) : lb(img[xwy+w])) : (x2 ? lb(img[xwy+1]) : lb(img[xwy    ]));
		}
	}
	return rgb;
}

uchar* utils_grey_draw(imgtype *img, uchar *rgb, uint32 w, uint32 h)
{
	int i, j, dim = h*w*3;
	for(i = 0,  j= 0; j < dim; j+=3, i++){
		rgb[j]     = lb(img[i]);
		rgb[j + 1] = lb(img[i]);
		rgb[j + 2] = lb(img[i]);
		//printf("y_w[%d] = %4d\n",i,mod(yuv_buffer->y_w[i]));
	}
	return rgb;
}

uchar* utils_reg_draw(uint32 *img, uchar *rgb, uint32 w, uint32 h)
{
	int i, j, dim = h*w*3;
	for(i = 0,  j= 0; j < dim; j+=3, i++){
		rgb[j]     = img[i]&0xFF;
		rgb[j + 1] = img[i]&0xFF;
		rgb[j + 2] = img[i]&0xFF;
		//printf("y_w[%d] = %4d\n",i,mod(yuv_buffer->y_w[i]));
	}
	return rgb;
}

imgtype* utils_cat(imgtype *img, imgtype *img1, uint32 w, uint32 h, uint32 bits)
{
	int i, dim = h*w, sh = bits-8;
	for(i = 0; i < dim; i++) img1[i] = img[i]<0 ? 0 : rnd(img[i]>>sh);
	return img1;
}

imgtype* utils_bayer_to_Y(imgtype *img, imgtype *img1, uint32 w, uint32 h)
{
	uint32 x, y, wy, xwy, y2, x2, a, b, h1 = h-1, w1 = w-1, yw, yw1;

	for(y=0, yw=0, yw1=0 ; y < h1; y++, yw+=w, yw1+=w1){
		for(x=0; x < w1; x++){
			y2 = oe(a,y);
			x2 = oe(b,x);
			xwy = x + yw;
			wy = (x + yw1);
			//xwy3 = wy + wy + wy;
			img1[wy] = 	(img[xwy    ] + img[xwy+1] + img[xwy+w] + img[xwy+w+1])>>2;
		}
	}
	return img1;
}

imgtype* utils_bayer_to_gradient(imgtype *img, imgtype *img1, uint32 w, uint32 h, BayerGrid bay, uint32 thresh)
{
	uint32 x, y, wy, xwy, y2, x2, a, b, h1 = h-1, w1 = w-1, yw, yw1;

    switch(bay){
            case(BGGR) : { a = 1; b = 1; break; }
            case(GRBG) : { a = 0; b = 1; break; }
            case(GBRG) : { a = 1; b = 0; break; }
            case(RGGB) : { a = 0; b = 0; break; }
    }

//	for(y=0, yw=0, yw1=0 ; y < h1; y++, yw+=w, yw1+=w1){
//		for(x=0; x < w1; x++){
	for(y=1, yw=w, yw1=w1 ; y < h1-1; y++, yw+=w, yw1+=w1){
		for(x=1; x < w1-1; x++){
		//for(y=1, yw=w, yw1=w1 ; y < 2; y++, yw+=w, yw1+=w1){
		//	for(x=1; x < 2; x++){
			y2 = oe(a,y);
			x2 = oe(b,x);
			xwy = x + yw;
			wy = (x + yw1);
			//printf("y2 = %d %d x2 = %d %d\n", y2,!y2, x2, !x2);
			//printf("xwy+w+1 = %d xwy-(w<<1) = %d xwy = %d\n", xwy+w+1, xwy-(w<<1), xwy);

			//Green
            img1[wy] = y2 ? (x2 ? abs(img[xwy+w  ] - img[xwy+1  ]) : abs(img[xwy    ] - img[xwy+w+1])) :
                            (x2 ? abs(img[xwy+w+1] - img[xwy    ]) : abs(img[xwy+1  ] - img[xwy+w  ])) ;
            //Red
			img1[wy] += y2 ? (x2 ? abs(img[xwy  ]   - ((img[xwy]     + img[xwy+2]   + img[xwy+(w<<1)]   + img[xwy+(w<<1)+2])>>2)) :
								   abs(img[xwy+1]   - ((img[xwy+1]   + img[xwy-1]   + img[xwy+(w<<1)+1] + img[xwy+(w<<1)-1])>>2))):
							 (x2 ? abs(img[xwy+w]   - ((img[xwy+w]   + img[xwy+2+w] + img[xwy-w]        + img[xwy-w+2])>>2)) :
								   abs(img[xwy+w+1] - ((img[xwy+w+1] + img[xwy-1+w] + img[xwy-w+1]      + img[xwy-w-1])>>2)));
			//Blue
			img1[wy] += !y2 ? (!x2 ? abs(img[xwy  ]   - ((img[xwy]     + img[xwy+2]   + img[xwy+(w<<1)]   + img[xwy+(w<<1)+2])>>2)) :
								     abs(img[xwy+1]   - ((img[xwy+1]   + img[xwy-1]   + img[xwy+(w<<1)+1] + img[xwy+(w<<1)-1])>>2))):
							  (!x2 ? abs(img[xwy+w]   - ((img[xwy+w]   + img[xwy+2+w] + img[xwy-w]        + img[xwy-w+2])>>2)) :
								     abs(img[xwy+w+1] - ((img[xwy+w+1] + img[xwy-1+w] + img[xwy-w+1]      + img[xwy-w-1])>>2)));
			img1[wy] = img1[wy] > thresh ? img1[wy] : 0;
 		}
	}
	return img1;
}

imgtype* utils_bayer_gradient(imgtype *img, imgtype *img1, uint32 w, uint32 h, BayerGrid bay, uint32 thresh)
{
	uint32 x, y, yx, h1 = (h-1)*w, w1 = w-1, yw;

	for(y=w ; y < h1; y+=w){
		for(x=1; x < w1; x++){
			yx = y + x;

			img1[yx] =   abs(img[yx-1] + img[yx-w] + img[yx-w-1] - img[yx+1] - img[yx+w] - img[yx+w+1])>>2;
			img1[yx] +=  abs(img[yx+1] + img[yx-w] + img[yx-w+1] - img[yx-1] - img[yx+w] - img[yx+w-1])>>2;
			//img1[yx] +=  abs(img[yx-1-w] + img[yx-w] + img[yx-w+1] - img[yx-1+w] - img[yx+w] - img[yx+w+1])>>2;
			//img1[yx] +=  abs(img[yx-1-w] + img[yx-1] + img[yx+w-1] - img[yx+1-w] - img[yx+1] - img[yx+w+1])>>2;

			img1[yx] = img1[yx] > thresh ? img1[yx] : 0;
 		}
	}
	return img1;
}


void inline local_max(imgtype *img, imgtype *img1, uint32 w)
{
	uint32 x, w1 = w-1;
	for(x=1; x < w1; x++){
		if(img[x-1])
		img1[x] = (img[x-1] <= img[x] && img[x] >= img[x+1]) ? img[x] : 0;
	}
}


static inline void check_min(imgtype *img, uint32 x, int w , uint32 *min)
{
	uint32 y = x+w;
	if(img[x] > img[y]) if(img[*min] > img[y]) *min = y;
}

imgtype* utils_watershed(imgtype *img, imgtype *img1, uint32 w, uint32 h)
{
	uint32 y=0, sq = w*(h-1), x, w1 = w-1, min, yx;
	//img[0] = 255;
	y=0;
	for(y=w; y < sq; y+=w) {
		x = 0;
		for(x=1; x < w1; x++){
			yx = y+x;
			if(img[yx]){
				min = yx;
				check_min(img, yx, -1 , &min);
				check_min(img, yx, -w , &min);
				check_min(img, yx,  1 , &min);
				check_min(img, yx,  w , &min);
				//check_min(img, yx, -1-w , &min);
				//check_min(img, yx,  1-w , &min);
				//check_min(img, yx,  w+1 , &min);
				//check_min(img, yx,  w-1 , &min);
				img1[min] = 0;
				//if(min != yx ) img1[min] = 0;

				//if(min) { img1[min] = 255; img1[yx] = img[yx]; }
				//else if (img[yx] > 0 ) img1[yx] = 255;
			}
		}
	}
	return img1;
}


void utils_min_region(imgtype *img, uint32 *ind, uint32 *arg, uint32 w, uint32 h)
{
	uint32 y=0, sq = w*(h-1), x, w1 = w-1, min, yx, zc = 0, mc = sq>>1 ;

	ind[0] = img[0] ? mc++ : zc++;
	for(x=1; x < w1; x++){
		yx = y+x;
		ind[yx] = img[yx] ? mc++ : (img[yx-1] ? zc++ : ind[yx-1]);
	}
	//printf("ind[%d] = %d\n", yx, ind[yx]);
	for(y=w; y < sq; y+=w) {
		for(x=1; x < w1; x++){
			yx = y+x;
			if(img[yx] == 0) {
				if(img[yx-1] == 0 ) {
					ind[yx] = ind[yx-1];
				} else ind[yx] = zc++;

				if (img[yx-w] == 0 && img[yx-1] == 0 ) {
					//if(img[yx-w-1]) ind[yx] = zc++;
					if(ind[yx] != ind[yx-w]) {
						arg[ind[yx-w]] = ind[yx];
						//if( ind[yx] > ind[yx-w]) arg[ind[yx]] = ind[yx-w];
						//else arg[ind[yx-w]] = ind[yx];
					}
					//ind[yx] = ind[yx-w];
				}
			} else {
				//ind[yx] = mc++;
				ind[yx] = zc++;
			}
		}
	}
}

void utils_steep_descent(imgtype *img, uint32 *ind, uint32 *arg, uint32 w, uint32 h)
{
	uint32 y, sq = w*(h-1), x, w1 = w-1, min, yx, tmp;
	//y=w;{
	for(y=w; y < sq; y+=w) {
		for(x=1; x < w1; x++){
			yx = y+x;
			min = yx;
			if(img[yx] != 0) {
				check_min(img, yx, -1 , &min);
				check_min(img, yx, -w , &min);
				check_min(img, yx,  1 , &min);
				check_min(img, yx,  w , &min);
				arg[ind[yx]] = (min == yx) ? ind[yx] : ind[min];
			}
		}
	}
}

void utils_connect_region(imgtype *img, uint32 *ind, uint32 *arg, uint32 w, uint32 h)
{
	uint32 y, sq = w*(h-1), x, w1 = w-1, min, yx, tmp;
	//y=w;{
	for(y=w; y < sq; y+=w) {
		for(x=1; x < w1; x++){
			yx = y+x;
			//if(img[yx] == 0) {
			for(tmp = arg[ind[yx]]; arg[tmp] != tmp; tmp = arg[tmp]);
				//printf("yx = %d ind[yx] = %d arg[ind] = %d arg[arg[ind]] = %d  \n", yx, ind[yx], arg[ind[yx]], arg[tmp]);
			ind[yx] = tmp;
			//}
		}
	}
}

void utils_row_seg(imgtype *img, Row *rows, uint32 *col, uint32 w, uint32 h, uint32 theresh)
{
	uint32 y, h1 = h>>1, x, w1 = w<<1, yx, y1, tot=0, ar=0, ag1=0, ag2=0, ab=0, old;

	int rc =-1;
	col[0] = 0;
	//rows[0].length = 1;
	for(y=0, y1=0; y1 < h1; y+=w1, y1++) {
		for(x=0; x < w; x+=2){
			yx = y+x;
			if(x){
				//if( abs(((rows[rc].c[0]+rows[rc].c[1]+rows[rc].c[2]+rows[rc].c[3])>>2) -
				//		((img[yx]+img[yx+1]+img[yx+w]+img[yx+w+1])>>2) ) > theresh )
				if( abs(rows[rc].c[0] - img[yx]    ) > theresh ||
					abs(rows[rc].c[1] - img[yx+1]  ) > theresh ||
					abs(rows[rc].c[2] - img[yx+w]  ) > theresh ||
					abs(rows[rc].c[3] - img[yx+w+1]) > theresh)
					{
					rows[rc].c[0] = ar/(rows[rc].length>>1);
					rows[rc].c[1] = ag1/(rows[rc].length>>1);
					rows[rc].c[2] = ag2/(rows[rc].length>>1);
					rows[rc].c[3] = ab/(rows[rc].length>>1);
					++rc;
					rows[rc].length = 2; rows[rc].x = x; rows[rc].y = y1;
					rows[rc].c[0] = img[yx]; rows[rc].c[1] = img[yx+1]; rows[rc].c[2] = img[yx+w]; rows[rc].c[3] = img[yx+w+1];
					ar = img[yx]; ag1 = img[yx+1]; ag2 = img[yx+w]; ab = img[yx+w+1];
				} else {
					rows[rc].length+=2;
					ar += img[yx]; ag1 += img[yx+1]; ag2 += img[yx+w]; ab += img[yx+w+1];

				}
			} else {
				if(rc > -1){
					rows[rc].c[0] = ar/(rows[rc].length>>1);
					rows[rc].c[1] = ag1/(rows[rc].length>>1);
					rows[rc].c[2] = ag2/(rows[rc].length>>1);
					rows[rc].c[3] = ab/(rows[rc].length>>1);
				}
				++rc;
				rows[rc].length = 2; rows[rc].x = 0; rows[rc].y = y1;
				rows[rc].c[0] = img[yx]; rows[rc].c[1] = img[yx+1]; rows[rc].c[2] = img[yx+w]; rows[rc].c[3] = img[yx+w+1];
				ar = img[yx]; ag1 = img[yx+1]; ag2 = img[yx+w]; ab = img[yx+w+1];
			}
		}
		col[y1] = rc+1;
	}
	printf("Rows  = %d \n", rc);
}

uint32 utils_region_seg(Region *reg, Row *rows, uint32 *col, uint32 w, uint32 h, uint32 theresh)
{
	uint32 i, y, h1 = h>>1, x, rc, rcu, rcd;
	for(rc=0; rc < col[0]; rc++){
		rows[rc].reg = &reg[rc];
		reg[rc].nrows = 1;
		reg[rc].x = rows[rcd].x; reg[rc].y = 0;
		reg[rc].ac[0] = rows[rc].c[0];
		reg[rc].ac[1] = rows[rc].c[1];
		reg[rc].ac[2] = rows[rc].c[2];
		reg[rc].ac[3] = rows[rc].c[3];
		reg[rc].c[0] = rows[rc].c[0];
		reg[rc].c[1] = rows[rc].c[1];
		reg[rc].c[2] = rows[rc].c[2];
		reg[rc].c[3] = rows[rc].c[3];
	}
	for(y=1; y < h1; y++) {
		rcd = col[y-1]; rcu = y==1 ? 0 : col[y-2];
		//printf("     rc = %d rcd = %d rcu = %d \n", rc, col[y], col[y-1]);
		//printf("      x = %d l = %d\n", rows[col[y]-1].x, rows[col[y]].length);
		while(rcd < col[y])
		{

			if( abs(rows[rcu].reg->c[0] - rows[rcd].c[0]) > theresh ||
				abs(rows[rcu].reg->c[1] - rows[rcd].c[1]) > theresh ||
				abs(rows[rcu].reg->c[2] - rows[rcd].c[2]) > theresh ||
				abs(rows[rcu].reg->c[3] - rows[rcd].c[3]) > theresh)

			{
				//printf("> %4d rc = %d rcu = %d rcd = %d rows[rcu+1].x = %d rows[rcd+1].x = %d \n"
				//		, y, rc, rcu, rcd, rows[rcu].x + rows[rcu].length, rows[rcd].x + rows[rcd].length);
				if(rows[rcu].x + rows[rcu].length >= rows[rcd].x + rows[rcd].length) {
					rows[rcd].reg = &reg[rc];
					reg[rc].nrows = 1;
					reg[rc].x = rows[rcd].x; reg[rc].y = y;
					reg[rc].ac[0] = rows[rcd].c[0];
					reg[rc].ac[1] = rows[rcd].c[1];
					reg[rc].ac[2] = rows[rcd].c[2];
					reg[rc].ac[3] = rows[rcd].c[3];
					reg[rc].c[0] = rows[rcd].c[0];
					reg[rc].c[1] = rows[rcd].c[1];
					reg[rc].c[2] = rows[rcd].c[2];
					reg[rc].c[3] = rows[rcd].c[3];
					if(rows[rcu].x + rows[rcu].length == rows[rcd].x + rows[rcd].length) rcu++;
					rc++; rcd++;
				} else rcu++;
			} else {
				//printf("< %4d rc = %d rcu = %d rcd = %d rows[rcu+1].x = %d rows[rcd+1].x = %d \n"
				//		, y, rc, rcu, rcd, rows[rcu].x + rows[rcu].length, rows[rcd].x + rows[rcd].length);
				rows[rcd].reg = rows[rcu].reg;
				//printf("pointer = %p\n", rows[rcu].reg);
				rows[rcd].reg->nrows++;
				rows[rcd].reg->ac[0] += rows[rcd].c[0];
				rows[rcd].reg->ac[1] += rows[rcd].c[1];
				rows[rcd].reg->ac[2] += rows[rcd].c[2];
				rows[rcd].reg->ac[3] += rows[rcd].c[3];
				++rcd;
				if(rcd == col[y]) break;
				if(rcu != col[y-1]-1) while(rows[rcu].x + rows[rcu].length <= rows[rcd].x ) {
					//printf(" rcu - rcd = %d\n", rows[rcu].x + rows[rcu].length - rows[rcd].x);
					rcu++;
				}
			}
		}
	}
	for(i=0; i < rc; i++) {
		//printf("c0 = %d c1 = %d c2 = %d c3 = %d n = %d\n", reg[i].ac[0], reg[i].ac[1], reg[i].ac[2], reg[i].ac[3], reg[i].nrows);
		reg[i].c[0] = reg[i].ac[0] / reg[i].nrows;
		reg[i].c[1] = reg[i].ac[1] / reg[i].nrows;
		reg[i].c[2] = reg[i].ac[2] / reg[i].nrows;
		reg[i].c[3] = reg[i].ac[3] / reg[i].nrows;
		reg[i].rowc = 0;
	}
	printf("Regionts = %d\n", rc);
	return rc;
}

void utils_region_fill(Region *reg, Row *rows, Row **pr, uint32 *col, uint32 w, uint32 h)
{
	uint32 i, y, h1 = h>>1, x, rc=0, rcu, rcd, tmp=0;
	//printf("col[h1] = %d ", col[h1-1]);
	for(rc=0; rc < col[h1-1]; rc++) {
		//printf("rowc = %d ", rows[rc].reg->rowc);
		if(rows[rc].reg->rowc){
			rows[rc].reg->row[rows[rc].reg->rowc] = &rows[rc];
			rows[rc].reg->rowc++;
		}
		else  {
			rows[rc].reg->row = &pr[tmp];
			//printf("prow = %p ", rows[rc].reg->row);
			tmp+=rows[rc].reg->nrows;
			rows[rc].reg->row[rows[rc].reg->rowc] = &rows[rc];
			rows[rc].reg->rowc++;
			//printf(reg[rc].row[y])
		}
	}
}

void utils_chain_construct(Region *reg, Row *rows, Row **pr, uint32 *col, uint32 w, uint32 h)
{
	uint32 i, y, h1 = h>>1, x, rc=0, rcu, rcd, tmp=0;
	//printf("col[h1] = %d ", col[h1-1]);
	for(rc=0; rc < col[h1-1]; rc++) {
		//printf("rowc = %d ", rows[rc].reg->rowc);
		if(rows[rc].reg->rowc){
			rows[rc].reg->row[rows[rc].reg->rowc] = &rows[rc];
			rows[rc].reg->rowc++;
		}
		else  {
			rows[rc].reg->row = &pr[tmp];
			//printf("prow = %p ", rows[rc].reg->row);
			tmp+=rows[rc].reg->nrows;
			rows[rc].reg->row[rows[rc].reg->rowc] = &rows[rc];
			rows[rc].reg->rowc++;
			//printf(reg[rc].row[y])
		}
	}
}

void utils_row_draw(imgtype *img, Row *rows, uint32 *col, uint32 w, uint32 h)
{
	uint32 y, h1 = h>>1, x, w1 = w<<1, yx, rc = 0, y1;
	for(y=0, y1=0; y1 < h1; y+=w1, y1++) {
		for(x=0; x < w; x+=2){
			yx = y+x;
			img[yx] 	= rows[rc].c[0];
			img[yx+1] 	= rows[rc].c[1];
			img[yx+w] 	= rows[rc].c[2];
			img[yx+w+1] = rows[rc].c[3];
			if(x == (rows[rc].x + rows[rc].length-2)) {
				//printf("x = %d rows[%d].x = %d rows[%d].length = %d ", x, rc, rows[rc].x, rc, rows[rc].length);
				++rc;
				//printf("x = %d rows[%d].x = %d rows[%d].length = %d ", x, rc, rows[rc].x, rc, rows[rc].length);
			}
		}
	}
}

void utils_region_draw(imgtype *img, Row *rows, uint32 *col, uint32 w, uint32 h)
{
	uint32 y, h1 = h>>1, x, w1 = w<<1, yx, rc = 0, y1;
	for(y=0, y1=0; y1 < h1; y+=w1, y1++) {
		for(x=0; x < w; x+=2){
			yx = y+x;
			img[yx] 	= rows[rc].reg->c[0];
			img[yx+1] 	= rows[rc].reg->c[1];
			img[yx+w] 	= rows[rc].reg->c[2];
			img[yx+w+1] = rows[rc].reg->c[3];
			if(x == (rows[rc].x + rows[rc].length-2)){
				//printf("x = %d rows[%d].x = %d rows[%d].length = %d ", x, rc, rows[rc].x, rc, rows[rc].length);
				++rc;
				//printf("x = %d rows[%d].x = %d rows[%d].length = %d ", x, rc, rows[rc].x, rc, rows[rc].length);
			}
		}
	}
}

void utils_region_draw1(imgtype *img, Region *reg, uint32 nreg, uint32 w, uint32 h)
{
	uint32 rowc, h1 = h>>1, x, w1 = w<<1, yx, rc = 0, y1;
	//rc = 97817; { //97848; {
	//rc = 97848; {
	//for(rc=0; rc < 97818; rc++){
	for(rc=0; rc < nreg; rc++){
		//printf("%6d nrows = %d x = %d y = %d\n", rc, reg[rc].nrows, reg[rc].x, reg[rc].y);
		if(reg[rc].nrows < 2){
		for(rowc=0; rowc < reg[rc].nrows; rowc++){
			//printf("p = %p\n", reg[rc].row);
			for(x=reg[rc].row[rowc]->x; x < (reg[rc].row[rowc]->x + reg[rc].row[rowc]->length); x+=2){
				yx = ((reg[rc].row[rowc]->y)*w<<1) + x;
				img[yx] 	= reg[rc].c[0];
				img[yx+1] 	= reg[rc].c[1];
				img[yx+w] 	= reg[rc].c[2];
				img[yx+w+1] = reg[rc].c[3];
			}
		}
		}
	}
}

void utils_print_img(imgtype* img, uint32* ind, uint32 w, uint32 h,  uint32 bx, uint32 by,  uint32 lx, uint32 ly)
{
	uint32 x, y;
	for(y=by; y < ly+by; y++) {
		printf("%5d ", y);
		for(x=bx; x < lx+bx; x++){
			printf("%5d ", img[y*w+x]);
		}
		printf("\n");
	//}for(y=by; y < ly+by; y++) {
		printf("%5d ", y);
		for(x=bx; x < lx+bx; x++){
			printf("%5d ", ind[y*w+x]);
		}
		printf("\n");
	}
	printf("\n");
}

void utils_print_ind(imgtype* img, uint32 w, uint32 h,  uint32 bx, uint32 by,  uint32 lx, uint32 ly)
{
	uint32 x, y;
	for(y=by; y < ly; y++) {
		for(x=bx; x < lx; x++){
			printf("%3d ", img[y*w+x]);
		}
		printf("\n");
	}
}


double utils_dist(imgtype *before, imgtype *after, uint32 dim, uint32 d)
/// \fn double dist(imgtype *before, imgtype *after, uint32 dim, uint32 d)
/// \brief Calculate distortion of two image.
/// \param before	Pointer to first image.
/// \param after	Pointer to second image.
/// \param dim 		Size of image height*width.
/// \param d 		d = 1 if gray image, d = 3 if color image.
/// \retval 		The distortion.
{
	uint32 i;
	double ape;
	unsigned long long r = 0, g = 0, b = 0;
	if(d==3){
		for(i = 0; i < dim; i = i + 3){
			//if(before[i]     - after[i]) printf("x = %4d y = %4d r = %4d\n",i/3,i/3,before[i]     - after[i]);
			r += (before[i]     - after[i])*(before[i]     - after[i]);
			g += (before[i+1] - after[i+1])*(before[i+1] - after[i+1]);
			b += (before[i+2] - after[i+2])*(before[i+2] - after[i+2]);
		}
		ape = (double)(r + g + b)/((double)dim*3.);
		return ape;
	}
	if(d==1){
		for(i = 0; i < dim; i++){
			//if(before[i]- after[i]) printf("i = %9d b = %4d a = %4d r = %4d\n",i,before[i], after[i], before[i] - after[i] );
			r += (before[i] - after[i])*(before[i] - after[i]);
			//printf("a = %3d  b = %3d ", before[i], after[i]);
		}
		//printf("r = %d  dim = %d ", r, dim);
		ape = (double)r/(double)dim;
		//printf(" dist = %f\n",ape);
		return ape;
	}
	else{printf("ape: ERROR\n");
		 return 0.;
	}
}

double utils_ape(imgtype *before, imgtype *after, uint32 dim, uint32 d)
/// \fn double ape(imgtype *before, imgtype *after, uint32 dim, uint32 d)
/// \brief Calculate APE of two image.
/// \param before	Pointer to first image.
/// \param after	Pointer to second image.
/// \param dim 		Size of image height*width.
/// \param d 		d = 1 if gray image, d = 3 if color image.
/// \retval 		The APE.
{
	uint32 i;
	double ape;
	unsigned long long r = 0, g = 0, b = 0;
	if(d==3){
		for(i = 0; i < dim; i = i + 3){
			//if(before[i]     - after[i]) printf("x = %4d y = %4d r = %4d\n",i/3,i/3,before[i]     - after[i]);
			r += abs((int)(before[i]     - after[i]));
			g += abs((int)(before[i+1] - after[i+1]));
			b += abs((int)(before[i+2] - after[i+2]));
		}
		ape = (double)(r + g + b)/((double)dim*3.);
		return ape;
	}
	if(d==1){
		for(i = 0; i < dim; i++){
			//if(before[i] - after[i] && i < 1920*2) printf("i = %4d b = %4d a = %4d\n", i, before[i], after[i]);
			r += abs((int)(before[i] - after[i] ));
		}
		ape = (double)r/(double)dim;
		return ape;
	}
	else{printf("ape: ERROR\n");
		 return 0.;
	}
}

double utils_psnr(imgtype *before, imgtype *after, uint32 dim, uint32 d)
/// \fn double psnr(imgtype *before, imgtype *after, uint32 dim, uint32 d)
/// \brief Calculate PSNR of two image.
/// \param before	Pointer to first image.
/// \param after	Pointer to second image.
/// \param dim 		Size of image height*width.
/// \param d 		d = 1 if gray image, d = 3 if color image.
/// \retval 		The PSNR.
{
	uint32 i;
	double psnr;
	unsigned long long r = 0, g = 0, b = 0;
	if(d==3){
		for(i = 0; i< dim; i = i + 3){
			r += (before[i]   - after[i]  )*(before[i]   - after[i]  );
			g += (before[i+1] - after[i+1])*(before[i+1] - after[i+1]);
			b += (before[i+2] - after[i+2])*(before[i+2] - after[i+2]);
		}

		if((r + g + b) == 0){
			return 0.;
		}

		psnr = 10.*log10((255.*255.)/((double)(r + g + b)/((double)dim*3.)));
		return psnr;
	}
	if(d==1){
		for(i = 0; i< dim; i++){
			r += (before[i]   - after[i] )*(before[i]   - after[i]);
		}

		if(r  == 0){
			return 0.;
		}

		psnr = 10.*log10((255.*255.)/((double)r/(double)dim));
		return psnr;
	}
	else{
		printf("psnr: ERROR\n");
		return 0.;
	}
}




void unifom_8bit(uint32 *distrib, uint32 bits, uint32 step, uchar sub, uint32 size, uint32 *q, double *dis, double *e)
/*! \fn static inline int dist_unifom_8(uint32 *distrib, const uint32 bit)
	\brief Calculate distortion for the given uniform quantizer.
    \param distrib	 The pointer to array of distribution probabilities.
    \param bits 	The bits per pixel.
	\param step		The step of quantization.
	\param sub		The number of subband
	\param size		The number of pixels in subband
	\param q		The quntization array
	\param dis		The subband distortion
	\param en		The subband entropy
*/
{
	uint32 sum = 0, en, tot=0, HALF = (1<<(bits-1));
	int i, j, rest, last, lasth;
	int hstep = step>>1;  // The half size of interval
	*e=0.;

	if(!sub){
		rest = 256;
		for(i=0; rest >= step; i+=step){
			rest -= step; en = 0;
			for(j=0; j< step; j++) {
				sum += distrib[HALF+i+j]*(j-hstep)*(j-hstep);
				en += distrib[HALF+i+j];
				q[i+j] = i + hstep;
				//if(distrib[HALF+i+j]) printf("%d\n", distrib[HALF+i+j]);
			}
			if(en) *e -= ((double)en/(double)size)*log2((double)en/(double)size);
			tot += en;
			//printf("rest = %d decision[%d] = %d, value[%d] = %d step = %d e = %f \n", rest, i/step, i, i/step, i+hstep, step, ((double)en/(double)size)*log2((double)en/(double)size));
		}
		if(rest){
			last = rest; en = 0;
			lasth = (last>>1);
			for(j=0; j< last; j++) {
				sum += distrib[HALF+i+j]*(j-lasth)*(j-lasth);
				en += distrib[HALF+i+j];
				q[i+j] = i + lasth;
			}
			if(en) *e -= ((double)en/(double)size)*log2((double)en/(double)size);
			tot += en;
			//printf("rest = %d decision[%d] = %d, value[%d] = %d step = %d e = %f \n", rest, i/step, i, i/step, i+lasth, last, ((double)en/(double)size)*log2((double)en/(double)size));
		}
		//printf("distr = %f entrop = %f tot = %d size = %d\n",(double)sum/(double)size, e, tot, size);

	} else {
		rest = 128-step;  en = 0;
		for(j= (1-step); j< (int)step; j++) {
			sum += distrib[HALF+j]*j*j;
			en += distrib[HALF+j];
			q[128 +i+j] = 0;
			//printf("sum = %d en = %d\n", sum, en);
		}
		if(en) *e -= ((double)en/(double)size)*log2((double)en/(double)size);
		tot += en;
		//printf("rest = %d decision[%d] = %d, value[%d] = %d step = %d e = %f \n", rest, 0, 0, 0, 0, step, ((double)en/(double)size)*log2((double)en/(double)size));
		for(i=step; rest >= step; i+=step){
			rest -= step;  en = 0;
			for(j= 0; j< step; j++) {
				sum += distrib[HALF+i+j]*(j-hstep)*(j-hstep);
				en += distrib[HALF+i+j];
				q[128 +i+j] = i+hstep;
			}
			tot += en;
			if(en) *e -= ((double)en/(double)size)*log2((double)en/(double)size);
			//printf("rest = %d decision[%d] = %d, value[%d] = %d step = %d e = %f \n", rest, i/step, i, i/step, i+hstep, step, ((double)en/(double)size)*log2((double)en/(double)size));
			en=0;
			for(j= 0; j< step; j++) {
				sum += distrib[HALF-i-j]*(hstep-j)*(hstep-j);
				en += distrib[HALF-i-j];
				q[128 -i-j] = -i-hstep;
			}
			if(en) *e -= ((double)en/(double)size)*log2((double)en/(double)size);
			tot += en;
			//printf("rest = %d decision[%d] = %d, value[%d] = %d step = %d e = %f \n", rest, i/step, -i, i/step, -i-hstep, step, ((double)en/(double)size)*log2((double)en/(double)size));
		}
		if(rest){
			last = rest; en = 0;
			lasth = (last>>1);
			for(j= 0; j< last ; j++) {
				sum += distrib[HALF+i+j]*(j-lasth)*(j-lasth);
				en += distrib[HALF+i+j];
				q[128 +i+j] = i+lasth;
			}
			tot += en;
			if(en) *e -= ((double)en/(double)size)*log2((double)en/(double)size);
			//printf("rest = %d decision[%d] = %d, value[%d] = %d step = %d e = %f \n", rest, i/step, i, i/step, i+lasth, last, ((double)en/(double)size)*log2((double)en/(double)size));
			en=0;
			for(j= 0; j< last; j++) {
				sum += distrib[HALF-i-j]*(lasth-j)*(lasth-j);
				en += distrib[HALF-i-j];
				q[128 -i-j] = -i-lasth;
			}
			if(en) *e -= ((double)en/(double)size)*log2((double)en/(double)size);
			tot += en;
			//printf("rest = %d decision[%d] = %d, value[%d] = %d step = %d e = %f \n", rest, i/step, -i, i/step, -i-lasth, last, ((double)en/(double)size)*log2((double)en/(double)size));
		}
		printf("distr = %f entrop = %f tot = %d size = %d\n",(double)sum/(double)size, *e, tot, size);
	}
	*dis = (double)sum/(double)size;
	//for(i=0; i<256;i++) printf("q[%d]=%d\n",i,q[i]);
	//printf("distr = %f entrop = %f tot = %d size = %d\n",dis, e, tot, size);
	//return (double)sum/(double)size;
}

//For test only----------------------------------
void utils_unifom_dist_entr(uint32 *distrib, uint32 bits, uint32 step, uint32 size, uint32 *q, double *dis, double *e)
/*! \fn static inline int dist_unifom_8(uint32 *distrib, const uint32 bit)
	\brief Calculate distortion for the given uniform quantizer.
    \param distrib	 	The pointer to array of distribution probabilities.
    \param bits 		The size of distribution probabilities array.
	\param step			The step of quantization.
	\param sub			The number of subband
	\param size			The number of pixels in subband
	\param q			The quntization array
	\param dis			The subband distortion
	\param en			The subband entropy
 */
{

	uint32 sum = 0, en=0, tot=0;
	int i, j, rest;
	uint32  hstep = step>>1, num = (1<<(bits-1)), st;  // The half size of interval
	//printf("distrib = %p\n", distrib);
	//for(i=0; i< (1<<bits); i++)  printf("distrib[%d] = %d\n", i, distrib[i]); //if(distrib[i])
	*e=0.;
	for(j= (1-step); j< (int)step; j++) {
		sum += distrib[num+j]*j*j;
		en += distrib[num+j];
		q[num+i+j] = 0;
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
			q[num +i+j] = i+(st>>1);
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
			q[num -i-j] = -i-(st>>1);
		}
		if(en) *e -= ((double)en/(double)size)*log2((double)en/(double)size);
		tot += en;
		//printf("tot = %d i = %d rest = %d en = %d st = %d e = %f num = %d\n", tot, i, rest, en, st, ((double)en/(double)size)*log2((double)en/(double)size), num-i-j);
		rest -= step;
	}
	printf("total = %d size = %d\n", tot, size);
	*dis = (double)sum/(double)size;
	//for(i=0; i<256;i++) printf("q[%d]=%d\n",i,q[i]);
	//printf("distr = %f entrop = %f tot = %d size = %d\n",dis, e, tot, size);
	//return (double)sum/(double)size;
}

uchar* wavelet_to_rgb(imgtype *img, uchar *rgb, int height, int width, int step)
{ 
	int i, j, k, t, dim = height*width;
	int w3 = width*3;
	imgtype *p;
	
	if((p = (imgtype*)malloc(dim * sizeof(imgtype))) == NULL){
		printf("Out of memory\n");
		return NULL;
	}
	t = 1 << step;
	for(j = 0; j < height/t; j++){
		for(i = 0; i < width/t; i++){
			p[width*j + i] = ll(t, i, j);
		} 
	}
	for(k=0; k < step; k++){
		for(j = 0 ; j < height/t; j++){
			for(i = 0; i < width/t; i++){
				p[width*j + i+ width/t] = hl(t, i, j);
			}
		}
		t>>=1;
	}
	t = 1 << step;
	for(k=0; k < step; k++){
		for(j = 0 ; j < height/t; j++){
			for(i = 0; i < width/t; i++){
				p[width*j + (height*width)/t + i] = lh(t, i, j);
			}
		}
		t>>=1;
	}
	t = 1 << step;
	for(k=0; k < step; k++){
		for(j = 0 ; j < height/t; j++){
			for(i = 0; i < width/t; i++){
				p[width*j  + (height*width)/t + i + width/t] = hh(t, i, j);
			}
		}
		t>>=1;
	}
	for(k = 0; k < width; k++){
		for(i = 0; i < height; i++){
			//rgb[ i*w3 + 3*k]  		   = top(low(128 + p[i*width + k]));
			//rgb[ i*w3 + 3*k + 1] 		   = top(low(128 + p[i*width + k]));
			//rgb[ i*w3 + 3*k + 2] 		   = top(low(128 + p[i*width + k]));
			rgb[ i*w3 + 3*k]  			   = p[i*width + k];//+128
			rgb[ i*w3 + 3*k + 1] 		   = p[i*width + k];//+128
			rgb[ i*w3 + 3*k + 2] 		   = p[i*width + k];//+128
		}
	}
	free(p);
	return rgb;
}

uchar* wavelet_to_rgb1(imgtype *img, uchar *rgb, int height, int width, int step)
{ 
	int i, j, k, t, dim = height*width;
	int w3 = width*3;
	imgtype *p;
	
	if((p = (imgtype*)malloc(dim*sizeof(imgtype))) == NULL){
		printf("Out of memory\n");
		return NULL;
	}
	t = 1 << step;
	for(j = 0; j < height/t; j++){
		for(i = 0; i < width/t; i++){
			p[width*j + i] = ll(t, i, j);
		} 
	}
	for(k=0; k < step; k++){
		for(j = 0 ; j < height/t; j++){
			for(i = 0; i < width/t; i++){
				p[width*j + i+ width/t] = hl(t, i, j);
			}
		}
		t>>=1;
	}
	t = 1 << step;
	for(k=0; k < step; k++){
		for(j = 0 ; j < height/t; j++){
			for(i = 0; i < width/t; i++){
				p[width*j + (height*width)/t + i] = lh(t, i, j);
			}
		}
		t>>=1;
	}
	t = 1 << step;
	for(k=0; k < step; k++){
		for(j = 0 ; j < height/t; j++){
			for(i = 0; i < width/t; i++){
				p[width*j  + (height*width)/t + i + width/t] = hh(t, i, j);
			}
		}
		t>>=1;
	}
	for(k = 0; k < width; k++){
		for(i = 0; i < height; i++){
			/*rgb[ i*w3 + 3*k]  			   = top(low(128 + p[i*width + k]));
			rgb[ i*w3 + 3*k + 1] 		   = top(low(128 + p[i*width + k]));
			rgb[ i*w3 + 3*k + 2] 		   = top(low(128 + p[i*width + k]));*/
			rgb[ i*w3 + 3*k]  			   = (uchar)clip(128 + p[i*width + k]);
			rgb[ i*w3 + 3*k + 1] 		   = (uchar)clip(128 + p[i*width + k]);
			rgb[ i*w3 + 3*k + 2] 		   = (uchar)clip(128 + p[i*width + k]);
		}
	}
	free(p);
	return rgb;
}

uchar* img_to_rgb128(imgtype *img, uchar *rgb, int height, int width)
{ 
	int i, j, dim = height*width*3;
	
	for(i = 0,  j= 0; j < dim; j+=3, i++){
		rgb[j]     = rnd(img[i]+128);
		rgb[j + 1] = rnd(img[i]+128);
		rgb[j + 2] = rnd(img[i]+128);
		//printf("img[%d] = %4d\n",i,img[i]);
	}
	return rgb;
}


uchar* uchar_to_rgb(uchar *img, uchar *rgb, int height, int width)
{ 
	int i, j, dim = height*width*3;
	
	for(i = 0,  j= 0; j < dim; j+=3, i++){
		rgb[j]     = img[i];
		rgb[j + 1] = img[i];
		rgb[j + 2] = img[i];
		//printf("y_w[%d] = %4d\n",i,mod(yuv_buffer->y_w[i]));
	}
	return rgb;
}

//Copy image
void copy(imgtype *in, imgtype *out, int w, int h)
{
	int i, dim = h*w;
	for(i = 0; i < dim; i++){
		out[i] = in[i];
	}
}

//Compare two image
void compare(imgtype *in, imgtype *out, int w, int h)
{
	int x, y, c=0;
	for(y = 0; y < h; y++){
		for(x=0; x<w; x++){
			if(in[y*w+x] - out[y*w+x]) c++;
		}
		printf("y = %4d c = %4d\n", y, c);
		c=0;
	}
}

//Make image less for one pix horizontal and for one vertical 
void resizeonepix(uchar *in, int w, int h)
{
	int x, y;
	for(y = 0; y < (h-1); y++){
		for(x=0; x < (w-1); x++){
			in[y*(w-1)+x] = in[y*w+x];
		}
	}
}

uchar* malet_to_rgb(imgtype *img, uchar *rgb, int h, int w, int step)
{ 
	int x, y, k, t;
	int ws[4], hs[4], s[4], tlx[4], tly[4];
	
	ws[0] = (w>>1) + w%2; hs[0] = (h>>1) + h%2;
	ws[1] = (w>>1); 	  hs[1] = (h>>1) + h%2;
	ws[2] = (w>>1) + w%2; hs[2] = (h>>1);
	ws[3] = (w>>1); 	  hs[3] = (h>>1);
	s[0] = 0; s[1] = ws[0]*hs[0]; s[2] = s[1] + ws[1]*hs[1]; s[3] = s[2] + ws[2]*hs[2];
	tlx[0] = 0    ; tly[0] = 0;
	tlx[1] = ws[0]; tly[1] = 0;
	tlx[2] = 0    ; tly[2] = hs[0];
	tlx[3] = ws[0]; tly[3] = hs[0];
		
	for(t=0; t<step; t++){
		ws[0] = (ws[0]>>1) + ws[0]%2; 	hs[0] = (hs[0]>>1) + hs[0]%2;
		ws[1] = (ws[0]>>1); 	  		hs[1] = (hs[0]>>1) + hs[0]%2;
		ws[2] = (ws[0]>>1) + ws[0]%2; 	hs[2] = (hs[0]>>1);
		ws[3] = (ws[0]>>1); 	  		hs[3] = (hs[0]>>1);
		s[0] = 0; s[1] = ws[0]*hs[0]; s[2] = s[1] + ws[1]*hs[1]; s[3] = s[2] + ws[2]*hs[2];
		tlx[0] = 0    ; tly[0] = 0;
		tlx[1] = ws[0]; tly[1] = 0;
		tlx[2] = 0    ; tly[2] = hs[0];
		tlx[3] = ws[0]; tly[3] = hs[0];
		
		
		for(k=1; k < 4; k++){
			for(y=0; y < hs[k]; y++ ){
				for(x=0; x < ws[k]; x++){
					//if(y==0) printf("rgb[%d] = %d img[%d] = %d \n", 3*((y+tly[k])*w + tlx[k] +x), rgb[3*((y+tly[k])*w + tlx[k] +x)], s[k] + y*ws[k] + x, img[s[k] + y*ws[k] + x]);
					rgb[3*((y+tly[k])*w + tlx[k] +x)]   = rnd(128+img[s[k] + y*ws[k] + x]);
					rgb[3*((y+tly[k])*w + tlx[k] +x)+1] = rnd(128+img[s[k] + y*ws[k] + x]);
					rgb[3*((y+tly[k])*w + tlx[k] +x)+2] = rnd(128+img[s[k] + y*ws[k] + x]);
				}
			}
		}
	}
	k=0;
	for(y=0; y < hs[k]; y++ ){
		for(x=0; x < ws[k]; x++){
			rgb[3*((y+tly[k])*w + tlx[k] +x)]   = rnd(img[s[k] + y*ws[k] + x]);
			rgb[3*((y+tly[k])*w + tlx[k] +x)+1] = rnd(img[s[k] + y*ws[k] + x]);
			rgb[3*((y+tly[k])*w + tlx[k] +x)+2] = rnd(img[s[k] + y*ws[k] + x]);
		}
	}
	
	return rgb;
}


double dist3(uchar *before, uchar *after, uint32 dim, uint32 d)
/*! \fn double dist(imgtype *before, imgtype *after, uint32 dim, uint32 d)
    \brief Calculate distortion of two image.
    \param before	Pointer to first image.
    \param after	Pointer to second image.
    \param dim 		Size of image height*width.
    \param d 		d = 1 if gray image, d = 3 if color image.
    \retval 		The distortion.
*/
{
	uint32 i;
	double ape;
	unsigned long long r = 0, g = 0, b = 0;
	if(d==3){
		for(i = 0; i < dim; i = i + 3){
			//if(before[i]     - after[i]) printf("x = %4d y = %4d r = %4d\n",i/3,i/3,before[i]     - after[i]);
			r += (before[i]     - after[i])*(before[i]     - after[i]);
			g += (before[i+1] - after[i+1])*(before[i+1] - after[i+1]);
			b += (before[i+2] - after[i+2])*(before[i+2] - after[i+2]);
		}
		ape = (double)(r + g + b)/((double)dim*3.);
		return ape;
	}
	if(d==1){
		for(i = 0; i < dim; i++){
			//if(before[i]- after[i]) printf("i = %9d b = %4d a = %4d r = %4d\n",i,before[i], after[i], before[i] - after[i] );
			r += (before[i] - after[i])*(before[i] - after[i]);
			//printf("a = %3d  b = %3d ", before[i], after[i]);
		}
		//printf("r = %d  dim = %d ", r, dim);
		ape = (double)r/(double)dim;
		//printf(" dist = %f\n",ape);
		return ape;
	}
	else{printf("ape: ERROR\n");
		 return 0.;
	}
}

double ape(imgtype *before, imgtype *after, uint32 dim, uint32 d)
/*! \fn double ape(imgtype *before, imgtype *after, uint32 dim, uint32 d)
    \brief Calculate APE of two image.
    \param before	Pointer to first image.
    \param after	Pointer to second image.
    \param dim 		Size of image height*width.
    \param d 		d = 1 if gray image, d = 3 if color image.
    \retval 		The APE.
*/
{
	uint32 i;
	double ape;
	unsigned long long r = 0, g = 0, b = 0;
	if(d==3){
		for(i = 0; i < dim; i = i + 3){
			//if(before[i]     - after[i]) printf("x = %4d y = %4d r = %4d\n",i/3,i/3,before[i]     - after[i]);
			r += abs((int)(before[i]     - after[i]));
			g += abs((int)(before[i+1] - after[i+1]));
			b += abs((int)(before[i+2] - after[i+2]));
		}
		ape = (double)(r + g + b)/((double)dim*3.);
		return ape;
	}
	if(d==1){
		for(i = 0; i < dim; i++){
			//if(before[i] - after[i] && i < 1920*2) printf("i = %4d b = %4d a = %4d\n", i, before[i], after[i]);
			r += abs((int)(before[i] - after[i] ));
		}
		ape = (double)r/(double)dim;
		return ape;
	}
	else{printf("ape: ERROR\n");
		 return 0.;
	}
}

double ape3(uchar *before, uchar *after, uint32 dim, uint32 d)
/*! \fn double ape(imgtype *before, imgtype *after, uint32 dim, uint32 d)
    \brief Calculate APE of two image.
    \param before	Pointer to first image.
    \param after	Pointer to second image.
    \param dim 		Size of image height*width.
    \param d 		d = 1 if gray image, d = 3 if color image.
    \retval 		The APE.
*/
{
	uint32 i;
	double ape;
	unsigned long long r = 0, g = 0, b = 0;
	if(d==3){
		for(i = 0; i < dim; i = i + 3){
			//if(before[i]     - after[i]) printf("x = %4d y = %4d r = %4d\n",i/3,i/3,before[i]     - after[i]);
			r += abs((int)(before[i]     - after[i]));
			g += abs((int)(before[i+1] - after[i+1]));
			b += abs((int)(before[i+2] - after[i+2]));
		}
		ape = (double)(r + g + b)/((double)dim*3.);
		return ape;
	}
	if(d==1){
		for(i = 0; i < dim; i++){
			//if(before[i]- after[i]) printf("i = %4d r = %4d\n",i,before[i] - after[i]);
			r += abs((int)(before[i] - after[i] ));
		}
		ape = (double)r/(double)dim;
		return ape;
	}
	else{printf("ape: ERROR\n");
		 return 0.;
	}
}


double psnr3(uchar *before, uchar *after, uint32 dim, uint32 d)
/*! \fn double psnr(imgtype *before, imgtype *after, uint32 dim, uint32 d)
    \brief Calculate PSNR of two image.
    \param before	Pointer to first image.
    \param after	Pointer to second image.
    \param dim 		Size of image height*width.
    \param d 		d = 1 if gray image, d = 3 if color image.
    \retval 		The PSNR.
*/
{
	uint32 i;
	double psnr;
	unsigned long long r = 0, g = 0, b = 0;
	if(d==3){
		for(i = 0; i< dim; i = i + 3){
			r += (before[i]   - after[i]  )*(before[i]   - after[i]  );
			g += (before[i+1] - after[i+1])*(before[i+1] - after[i+1]);
			b += (before[i+2] - after[i+2])*(before[i+2] - after[i+2]);
		}
		
		if((r + g + b) == 0){
			return 0.;
		}
		
		psnr = 10.*log10((255.*255.)/((double)(r + g + b)/((double)dim*3.)));
		return psnr;
	}
	if(d==1){
		for(i = 0; i< dim; i++){
			r += (before[i]   - after[i] )*(before[i]   - after[i]);
		}
		
		if(r  == 0){
			return 0.;
		}
		
		psnr = 10.*log10((255.*255.)/((double)r/(double)dim));
		return psnr;
	}
	else{
		printf("psnr: ERROR\n");
		return 0.;
	}
}

uchar* YUV_to_RGB(uchar *rgb, imgtype *y, imgtype *u, imgtype *v, uint32 sq)
{
	int C, D, E, i;
	for(i=0; i<sq; i++){
		C = y[i] - 16;
		D = u[i] - 128;
		E = v[i] - 128;
		rgb[i*3    ] = clip(( 298*C         + 409*E + 128) >> 8);
		rgb[i*3 + 1] = clip(( 298*C - 100*D - 208*E + 128) >> 8);
		rgb[i*3 + 2] = clip(( 298*C + 516*D         + 128) >> 8);
	}
	return rgb;
}

uchar* YUV420p_to_RGB(uchar *rgb, imgtype *y, imgtype *u, imgtype *v, uint32 w, uint32 h)
{
	int C, D, E, i, j;
	for(i=0; i<h; i++){
		for(j=0; j< w; j++){
			C = y[j + w*i] - 16;
			D = u[(j>>1) + (w>>1)*(i>>1)] - 128;
			E = v[(j>>2) + (w>>1)*(i>>2)] - 128;
			rgb[(j+w*i)*3    ] = clip(( 298*C         + 409*E + 128) >> 8);
			rgb[(j+w*i)*3 + 1] = clip(( 298*C - 100*D - 208*E + 128) >> 8);
			rgb[(j+w*i)*3 + 2] = clip(( 298*C + 516*D         + 128) >> 8);
		}
	}
	return rgb;
}
