#include <walet.h>
#include <stdio.h>
#include <math.h>

void seg_grad(imgtype *img, imgtype *img1, imgtype *img2, uint32 w, uint32 h, uint32 th)
{
	/// | |x| |      | | | |      |x| | |      | | |x|
	/// | |x| |      |x|x|x|      | |x| |      | |x| |
	/// | |x| |      | | | |      | | |x|      |x| | |
	///  g[2]         g[0]         g[1]         g[3]
	/// Direction
	///   n=0          n=2         n=3          n=1
	/// | | | |      | ||| |      | | |/|      |\| | |
	/// |-|-|-|      | ||| |      | |/| |      | |\| |
	/// | | | |      | ||| |      |/| | |      | | |\|
	uint32 y, x, yx, sq = w*h-w, w1 = w-1, h1 = h-1, max, in;
	uint32 g[4];
	for(y=w; y < sq; y+=w){
		for(x=1; x < w1; x++){
			yx = y + x;
			g[0] = abs(img[yx-1  ] - img[yx+1  ]);
			g[1] = abs(img[yx-1-w] - img[yx+1+w]);
			g[2] = abs(img[yx-w  ] - img[yx+w  ]);
			g[3] = abs(img[yx+1-w] - img[yx-1+w]);
			max = g[0]; in = 2;
			if(max < g[1]) { max = g[1]; in = 3; }
			if(max < g[2]) { max = g[2]; in = 0; }
			if(max < g[3]) { max = g[3]; in = 1; }
			max = (g[0] + g[1] + g[2] + g[3])>>2;
			//img1[yx] = max>>th ? (max >= 255 ? 254 : (max>>th)<<th): 0; img2[yx] = in;
			img1[yx] = max>>th ? (max >252 ? 252 : max) : 0; img2[yx] = in;
			//img1[yx] = max>>th ? (max>>th)<<th : 0; img2[yx] = in;
		}
	}
}

void seg_grad1(imgtype *img, imgtype *img1, imgtype *img2, uint32 w, uint32 h, uint32 th)
{
	/// | |x| |      | | | |      |x| | |      | | |x|
	/// | |x| |      |x|x|x|      | |x| |      | |x| |
	/// | |x| |      | | | |      | | |x|      |x| | |
	///  g[2]         g[0]         g[1]         g[3]
	/// Direction
	///   n=0          n=2         n=3          n=1
	/// | | | |      | ||| |      | | |/|      |\| | |
	/// |-|-|-|      | ||| |      | |/| |      | |\| |
	/// | | | |      | ||| |      |/| | |      | | |\|
	uint32 y, x, yx, sq = w*h-w, w1 = w-1, h1 = h-1, max, in;
	uint32 g[4];
	for(y=w; y < sq; y+=w){
		for(x=1; x < w1; x++){
			yx = y + x;
			max = (	abs(img[yx-1-w] - img[yx-w  ]) +
					abs(img[yx-w  ] - img[yx-w+1]) +
					abs(img[yx-w+1] - img[yx+1  ]) +
					abs(img[yx+1  ] - img[yx+1+w]) +
					abs(img[yx+1+w] - img[yx+w  ]) +
					abs(img[yx+w  ] - img[yx+w-1]) +
					abs(img[yx+w-1] - img[yx-1  ]) +
					abs(img[yx-1  ] - img[yx-1-w])
					//abs(img[yx    ] - img[yx-1  ]) +
					//abs(img[yx    ] - img[yx-w  ]) +
					//abs(img[yx    ] - img[yx+1  ]) +
					//abs(img[yx    ] - img[yx+w  ])
					) >> 2;
			img1[yx] = max>>th ? (max >253 ? 253 : max) : 0; img2[yx] = in;
			//img1[yx] = max>>th ? (max>>th)<<th : 0; img2[yx] = in;
		}
	}
}

void seg_grad2(imgtype *img, imgtype *img1, imgtype *img2, uint32 w, uint32 h, uint32 th)
{
	/// | |x| |      | | | |      |x| | |      | | |x|
	/// | |x| |      |x|x|x|      | |x| |      | |x| |
	/// | |x| |      | | | |      | | |x|      |x| | |
	///  g[2]         g[0]         g[1]         g[3]
	/// Direction
	///   n=0          n=2         n=3          n=1
	/// | | | |      | ||| |      | | |/|      |\| | |
	/// |-|-|-|      | ||| |      | |/| |      | |\| |
	/// | | | |      | ||| |      |/| | |      | | |\|
	uint32 y, x, yx, sq = w*h-w, w1 = w-1, h1 = h-1, gv, gh, max, in;
	uint32 g[4];
	for(y=w; y < sq; y+=w){
		for(x=1; x < w1; x++){
			yx = y + x;
			//Vertical sobel gradient
			gv = abs(img[yx-1-w] + (img[yx-1  ]<<1) + img[yx-1+w] - img[yx+1-w] - (img[yx+1  ]<<1) - img[yx+1+w])>>2;
			//Horizontal sobel gradient
			gh = abs(img[yx-1-w] + (img[yx-w  ]<<1) + img[yx+1-w] - img[yx-1+w] - (img[yx+w  ]<<1) - img[yx+1+w])>>2;
			max = gv + gh;
			img1[yx] = max>>th ? (max >253 ? 253 : max) : 0; img2[yx] = in;
			//img1[yx] = max>>th ? (max>>th)<<th : 0; img2[yx] = in;
		}
	}
}

static inline uint32 loc_max(imgtype *img, uint32 yx, uint32 w)
{
	uint32 in = 0;
	if(img[yx]){
		if( img[yx-1] 	<= img[yx] &&
			img[yx-w]	<= img[yx] &&
			img[yx+1] 	<= img[yx] &&
			img[yx+w] 	<= img[yx] &&
			img[yx-1-w] <= img[yx] &&
			img[yx+1-w] <= img[yx] &&
			img[yx-1+w] <= img[yx] &&
			img[yx+1+w] <= img[yx] ) return 1;
		else return 0;

	} else return 0;
	/*
	if(	img[yx-1] 	<= img[yx]) {
		if(img1[yx-1] == 255) goto no;
		else if(!img[yx-1]) in++;
	} else goto no;
	if(	img[yx-w]	<= img[yx]){
		if(img1[yx-w] == 255) goto no;
		else if(!img[yx-w]) in++;
	} else goto no;
	if(	img[yx+1] 	<= img[yx]){
		if(img1[yx+1] == 255) goto no;
		else if(!img[yx+1]) in++;
	} else goto no;
	if(	img[yx+w] 	<= img[yx]){
		if(img1[yx+w] == 255) goto no;
		else if(!img[yx+w]) in++;
	} else goto no;
	if(	img[yx-1-w] <= img[yx]){
		if(img1[yx-1-w] == 255) goto no;
		else if(!img[yx-1-w]) in++;
	} else goto no;
	if(	img[yx+1-w] <= img[yx]){
		if(img1[yx+1-w] == 255) goto no;
		else if(!img[yx+1-w]) in++;
	} else goto no;
	if(	img[yx-1+w] <= img[yx]){
		if(img1[yx-1+w] == 255) goto no;
		else if(!img[yx-1+w]) in++;
	} else goto no;
	if(	img[yx+1+w] <= img[yx]){
		if(img1[yx+1+w] == 255) goto no;
		else if(!img[yx+1+w]) in++;
	} else goto no;
	return in >= 7 ? 0 :1;
	//return 1;
no:
	return 0; */
}

static inline int dir(imgtype *img, uint32 yx, uint32 w, int in1)
//Check for pixel
{
	uint32 max = 0;
	int in = 0;
	/*
	if(in1 != -1  ) if(img[yx-1  ] > max) { max = img[yx-1  ]; in = -1  ; }
	if(in1 != -1-w) if(img[yx-1-w] > max) { max = img[yx-1-w]; in = -1-w; }
	if(in1 !=   -w) if(img[yx  -w] > max) { max = img[yx  -w]; in =   -w; }
	if(in1 !=  1-w) if(img[yx+1-w] > max) { max = img[yx+1-w]; in = +1-w; }
	if(in1 !=  1  ) if(img[yx+1  ] > max) { max = img[yx+1  ]; in = +1  ; }
	if(in1 !=  1+w) if(img[yx+1+w] > max) { max = img[yx+1+w]; in = +1+w; }
	if(in1 !=    w) if(img[yx  +w] > max) { max = img[yx  +w]; in =   +w; }
	if(in1 != -1+w) if(img[yx-1+w] > max) { max = img[yx-1+w]; in = -1+w; }
	return in;*/
		//goto end;
/*Fix it optimization*/

	if(in1 == 0   ){
		if(img[yx-1  ] > max) { max = img[yx-1  ]; in = -1  ; }
		if(img[yx-1-w] > max) { max = img[yx-1-w]; in = -1-w; }
		if(img[yx  -w] > max) { max = img[yx  -w]; in =   -w; }
		if(img[yx+1-w] > max) { max = img[yx+1-w]; in = +1-w; }
		if(img[yx+1  ] > max) { max = img[yx+1  ]; in = +1  ; }
		if(img[yx+1+w] > max) { max = img[yx+1+w]; in = +1+w; }
		if(img[yx  +w] > max) { max = img[yx  +w]; in =   +w; }
		if(img[yx-1+w] > max) { max = img[yx-1+w]; in = -1+w; }
		goto end;
	}
	if(in1 == -1  ){
		//if(img[yx  -w] > max) { max = img[yx  -w]; in =   -w; }
		if(img[yx+1-w] > max) { max = img[yx+1-w]; in = +1-w; }
		if(img[yx+1  ] >= max) { max = img[yx+1  ]; in = +1  ; }
		if(img[yx+1+w] > max) { max = img[yx+1+w]; in = +1+w; }
		//if(img[yx  +w] > max) { max = img[yx  +w]; in =   +w; }
		goto end;
	}
	if(in1 == -1-w){
		//if(img[yx+1-w] > max) { max = img[yx+1-w]; in = +1-w; }
		if(img[yx+1  ] > max) { max = img[yx+1  ]; in = +1  ; }
		if(img[yx+1+w] >= max) { max = img[yx+1+w]; in = +1+w; }
		if(img[yx  +w] > max) { max = img[yx  +w]; in =   +w; }
		//if(img[yx-1+w] > max) { max = img[yx-1+w]; in = -1+w; }
		goto end;
	}
	if(in1 ==   -w){
		//if(img[yx+1  ] > max) { max = img[yx+1  ]; in = +1  ; }
		if(img[yx+1+w] > max) { max = img[yx+1+w]; in = +1+w; }
		if(img[yx  +w] >= max) { max = img[yx  +w]; in =   +w; }
		if(img[yx-1+w] > max) { max = img[yx-1+w]; in = -1+w; }
		//if(img[yx  -1] > max) { max = img[yx  -1]; in =   -1; }
		goto end;
	}
	if(in1 ==  1-w){
		//if(img[yx+1+w] > max) { max = img[yx+1+w]; in = +1+w; }
		if(img[yx  +w] > max) { max = img[yx  +w]; in =   +w; }
		if(img[yx-1+w] >= max) { max = img[yx-1+w]; in = -1+w; }
		if(img[yx  -1] > max) { max = img[yx  -1]; in =   -1; }
		//if(img[yx-1-w] > max) { max = img[yx-1-w]; in = -1-w; }
		goto end;
	}
	if(in1 ==  1  ){
		//if(img[yx  +w] > max) { max = img[yx  +w]; in =   +w; }
		if(img[yx-1+w] > max) { max = img[yx-1+w]; in = -1+w; }
		if(img[yx  -1] >= max) { max = img[yx  -1]; in =   -1; }
		if(img[yx-1-w] > max) { max = img[yx-1-w]; in = -1-w; }
		//if(img[yx  -w] > max) { max = img[yx  -w]; in =   -w; }
		goto end;
	}
	if(in1 ==  1+w){
		//if(img[yx-1+w] > max) { max = img[yx-1+w]; in = -1+w; }
		if(img[yx  -1] > max) { max = img[yx  -1]; in =   -1; }
		if(img[yx-1-w] >= max) { max = img[yx-1-w]; in = -1-w; }
		if(img[yx  -w] > max) { max = img[yx  -w]; in =   -w; }
		//if(img[yx+1-w] > max) { max = img[yx+1-w]; in = +1-w; }
		goto end;
	}
	if(in1 ==    w){
		//if(img[yx  -1] > max) { max = img[yx  -1]; in =   -1; }
		if(img[yx-1-w] > max) { max = img[yx-1-w]; in = -1-w; }
		if(img[yx  -w] >= max) { max = img[yx  -w]; in =   -w; }
		if(img[yx+1-w] > max) { max = img[yx+1-w]; in = +1-w; }
		//if(img[yx+1  ] > max) { max = img[yx+1  ]; in = +1  ; }
		goto end;
	}
	if(in1 == -1+w){
		//if(img[yx-1-w] > max) { max = img[yx-1-w]; in = -1-w; }
		if(img[yx  -w] > max) { max = img[yx  -w]; in =   -w; }
		if(img[yx+1-w] >= max) { max = img[yx+1-w]; in = +1-w; }
		if(img[yx+1  ] > max) { max = img[yx+1  ]; in = +1  ; }
		//if(img[yx+1+w] > max) { max = img[yx+1+w]; in =  1+w; }
		goto end;
	}
end:
	return in;
}

static inline void dir1(imgtype *img, uint32 w, uint yx, int dx, int dy, int *dx1, int *dy1)
//Check for pixel
{
        uint32 max = 0;
        if(dx == 0 && dy == 0  ){
                if(img[yx-1  ] > max) { max = img[yx-1  ]; *dx1 = -1; *dy1 =  0;}
                if(img[yx-1-w] > max) { max = img[yx-1-w]; *dx1 = -1; *dy1 = -1;}
                if(img[yx  -w] > max) { max = img[yx  -w]; *dx1 =  0; *dy1 = -1;}
                if(img[yx+1-w] > max) { max = img[yx+1-w]; *dx1 =  1; *dy1 = -1;}
                if(img[yx+1  ] > max) { max = img[yx+1  ]; *dx1 =  1; *dy1 =  0;}
                if(img[yx+1+w] > max) { max = img[yx+1+w]; *dx1 =  1; *dy1 =  1;}
                if(img[yx  +w] > max) { max = img[yx  +w]; *dx1 =  0; *dy1 =  1;}
                if(img[yx-1+w] > max) { max = img[yx-1+w]; *dx1 = -1; *dy1 =  1;}
                goto end;
        }
        if(dx == -1 && dy == 0){
                //if(img[yx  -w] > max) { max = img[yx  -w]; *dx1 =  0; *dy1 = -1;}
                if(img[yx+1-w] > max) { max = img[yx+1-w]; *dx1 =  1; *dy1 = -1;}
                if(img[yx+1  ] >= max){ max = img[yx+1  ]; *dx1 =  1; *dy1 =  0;}
                if(img[yx+1+w] > max) { max = img[yx+1+w]; *dx1 =  1; *dy1 =  1;}
                //if(img[yx  +w] > max) { max = img[yx  +w]; *dx1 =  0; *dy1 =  1;}
                goto end;
        }
        if(dx == -1 && dy == -1){
                //if(img[yx+1-w] > max) { max = img[yx+1-w]; *dx1 =  1; *dy1 = -1;}
                if(img[yx+1  ] > max) { max = img[yx+1  ]; *dx1 =  1; *dy1 =  0;}
                if(img[yx+1+w] >= max){ max = img[yx+1+w]; *dx1 =  1; *dy1 =  1;}
                if(img[yx  +w] > max) { max = img[yx  +w]; *dx1 =  0; *dy1 =  1;}
                //if(img[yx-1+w] > max) { max = img[yx-1+w]; *dx1 = -1; *dy1 =  1;}
                goto end;
        }
        if(dx ==  0 && dy == -1){
                //if(img[yx+1  ] > max) { max = img[yx+1  ]; *dx1 =  1; *dy1 =  0;}
                if(img[yx+1+w] > max) { max = img[yx+1+w]; *dx1 =  1; *dy1 =  1;}
                if(img[yx  +w] >= max){ max = img[yx  +w]; *dx1 =  0; *dy1 =  1;}
                if(img[yx-1+w] > max) { max = img[yx-1+w]; *dx1 = -1; *dy1 =  1;}
                //if(img[yx  -1] > max) { max = img[yx  -1]; *dx1 = -1; *dy1 =  0;}
                goto end;
        }
        if(dx ==  1 && dy == -1){
                //if(img[yx+1+w] > max) { max = img[yx+1+w]; *dx1 =  1; *dy1 =  1;}
                if(img[yx  +w] > max) { max = img[yx  +w]; *dx1 =  0; *dy1 =  1;}
                if(img[yx-1+w] >= max){ max = img[yx-1+w]; *dx1 = -1; *dy1 =  1;}
                if(img[yx  -1] > max) { max = img[yx  -1]; *dx1 = -1; *dy1 =  0;}
                //if(img[yx-1-w] > max) { max = img[yx-1-w]; *dx1 = -1; *dy1 = -1;}
                goto end;
        }
        if(dx ==  1 && dy ==  0){
                //if(img[yx  +w] > max) { max = img[yx  +w]; *dx1 =  0; *dy1 =  1;}
                if(img[yx-1+w] > max) { max = img[yx-1+w]; *dx1 = -1; *dy1 =  1;}
                if(img[yx  -1] >= max){ max = img[yx  -1]; *dx1 = -1; *dy1 =  0;}
                if(img[yx-1-w] > max) { max = img[yx-1-w]; *dx1 = -1; *dy1 = -1;}
                //if(img[yx  -w] > max) { max = img[yx  -w]; *dx1 =  0; *dy1 = -1;}
                goto end;
        }
        if(dx ==  1 && dy ==  1){
                //if(img[yx-1+w] > max) { max = img[yx-1+w]; *dx1 = -1; *dy1 =  1;}
                if(img[yx  -1] > max) { max = img[yx  -1]; *dx1 = -1; *dy1 =  0;}
                if(img[yx-1-w] >= max){ max = img[yx-1-w]; *dx1 = -1; *dy1 = -1;}
                if(img[yx  -w] > max) { max = img[yx  -w]; *dx1 =  0; *dy1 = -1;}
                //if(img[yx+1-w] > max) { max = img[yx+1-w]; *dx1 =  1; *dy1 = -1;}
                goto end;
        }
        if(dx ==  0 && dy ==  1){
                //if(img[yx  -1] > max) { max = img[yx  -1]; *dx1 = -1; *dy1 =  0;}
                if(img[yx-1-w] > max) { max = img[yx-1-w]; *dx1 = -1; *dy1 = -1;}
                if(img[yx  -w] >= max){ max = img[yx  -w]; *dx1 =  0; *dy1 = -1;}
                if(img[yx+1-w] > max) { max = img[yx+1-w]; *dx1 =  1; *dy1 = -1;}
                //if(img[yx+1  ] > max) { max = img[yx+1  ]; *dx1 =  1; *dy1 =  0;}
                goto end;
        }
        if(dx == -1 && dy ==  1){
                //if(img[yx-1-w] > max) { max = img[yx-1-w]; *dx1 = -1; *dy1 = -1;}
                if(img[yx  -w] > max) { max = img[yx  -w]; *dx1 =  0; *dy1 = -1;}
                if(img[yx+1-w] >= max){ max = img[yx+1-w]; *dx1 =  1; *dy1 = -1;}
                if(img[yx+1  ] > max) { max = img[yx+1  ]; *dx1 =  1; *dy1 =  0;}
                //if(img[yx+1+w] > max) { max = img[yx+1+w]; *dx1 =  1; *dy1 =  1;}
                goto end;
        }

end:
        return;
}

static inline void new_pix(Pixel *pix, imgtype img, uint32 x, uint32 y)
{
	pix->nin = 0; pix->nout = 0; pix->x = x; pix->y = y; //pix->draw = 1; //pix->end = 0;
}

static inline void new_line(Pixel *pix, Pixel *pix1, uchar pow)
{
	pix->out = pix1;   pix->nout++; pix->pow = pow; //pix->pow[pix->nout] = pow; pix->pow[pix->nout] = pow;
	pix1->nin++;//pix1->in[pix->nin] = pix;
}

void seg_local_max( Pixel *pix, uint32 *npix, imgtype *img, uint32 w, uint32 h)
{
	uint32 y, y1, x, yx, yx1, yx2, i, sq = w*h - w, w1 = w-1, is = 0;
	int d1, d2;
	(*npix) = 0;
	for(y=1, y1=w; y1 < sq; y++, y1+=w){
		for(x=1; x < w1; x++){
			yx = y1 + x;
			//if(img[yx] && img[yx]!= 255){
			if(img[yx] && img[yx] != 255){
				if(loc_max(img, yx, w)){
					yx1 = yx; yx2 = yx;
					img[yx1] = 255;
					d1 = dir(img, yx1, w, 0);
					d2 = dir(img, yx1, w, d1);
					while(1){
						yx1 = yx1 + d1;
						if(img[yx1] == 255 || !img[yx1]) break;
						(*npix)++;
						img[yx1] = 255;
						d1 = dir(img, yx1, w, -d1);
					}
					while(1){
						yx2 = yx2 + d2; //yx2 = yx2 + dy2*w + dx2; //x = x + dx; y = y + dy;
						if(img[yx2] == 255 || !img[yx2]) break;
						(*npix)++;
						img[yx2] = 255;
						d2 = dir(img, yx2, w, -d2);
					}
				}
			}
		}
	}
	printf("Numbers of pixels  = %d\n", *npix);
}

static inline uint32 is_in_line(int dx, int dy, int dx1, int dy1)
{
	if(dx == dx1 && dy == dy1) return 0;
	//if(!dx && !dx1) return 1;
	//if(!dy && !dy1) return 1;
	//if(dx1*dy == dy1*dx) return 1;
	return 1;
}

static inline uint32 is_in_line1(int dx, int dy, int dx1, int dy1)
{
	//if(dx == dx1 && dy == dy1) return 0;
	//if(!dx && !dx1) return 1;
	//if(!dy && !dy1) return 1;
	if(dx1*dy == dy1*dx) return 1;
	return 0;
}

static inline uint32 line_diff(int x1, int y1, int x2, int y2, uint32 len)
{
	int dx = abs(x2 - x1), dy = abs(y2 - y1);
	//printf("len = %d real = %f\n", len, sqrt(dx*dx + dy*dy)*32);
	return (len - sqrt(dx*dx + dy*dy)*32);
}

static inline uint32 is_new_line(int x1, int y1, int x2, int y2, uint32 len, uint32 th)
{
	if(line_diff(x1, y1, x2, y2, len) > th) return 1;
	return 0;
}

static inline uint32 length(int dx, int dy)
{
	if(dx && dy) return 45;
	return 32;
}

static inline uint32 find_lines(Pixel *pix, imgtype *img, uint32 x, uint32 y, uint32 yx, uint32 w)
{
	uint32 len = 0, x1 = x, y1 = y, yx1 = yx, min, c = 0;
	int dx, dy, dx1, dy1, dx2, dy2;
	dir1(img, w,  yx,   0,   0, &dx, &dy);
	min = img[yx];
    while(1){
     	//len += length(dx, dy);
     	yx = yx + dy*w + dx; x = x + dx; y = y + dy;
     	min = img[yx] < min ? img[yx] : min;
		if(!img[yx]) {
			if(c > 0){
				if(!pix[yx1].x && !pix[yx1].y)  { new_pix(&pix[yx1], img[yx1], x1, y1); img[yx1] = 255;}
				new_pix(&pix[yx], img[yx], x, y); img[yx] = 255;
				new_line(&pix[yx1], &pix[yx], min);
			}
			img[yx] = 254;
			return 0;
		}
		if(img[yx] == 255 || img[yx] == 254) {
			if(yx != yx1 && c > 0){
				if(!pix[yx].x && !pix[yx].y) { new_pix(&pix[yx], img[yx], x, y); img[yx] = 255; }
				if(!pix[yx1].x && !pix[yx1].y) { new_pix(&pix[yx1], img[yx1], x1, y1); img[yx1] = 255; }
				new_line(&pix[yx1], &pix[yx], min);
			}
			img[yx] = 254;
			return 0;
		}
		img[yx] = 254;
		dx2 = dx1; dy2 = dy1;
		dx1 = dx; dy1 = dy;
		dir1(img, w, yx, -dx, -dy, &dx, &dy);
		//if(is_new_line(x1, y1, x, y, len, 10) && c > 1){
		if(c > 2 && is_in_line(dx, dy, dx2, dy2)){
			//printf("dx1 = %d dx = %d dy1 = %d dy = %d c = %d\n", dx1, dx, dy1, dy, c);
			new_pix(&pix[yx], img[yx], x, y); img[yx] = 255;
			if(!pix[yx1].x && !pix[yx1].y) { new_pix(&pix[yx1], img[yx1], x1, y1); img[yx1] = 255;}
			new_line(&pix[yx1], &pix[yx], min);
			x1 = x; y1 = y; yx1 = yx; c = 0; //len = 0;
		}
		c++;
    }
}

void seg_line(Pixel *pix, imgtype *img, uint32 w, uint32 h)
//
//  |-----dif1------|
//        len1
//          |-----dif2------|
//                len2
//	x11     x1      x2     x22
//	*<------*------>*------>*
//     dx1     dx      dx2
//     dy2     dy      dy2
//
{
	uint32 y, x, yx, w1 = w-1, h1 = h-1;
	for(y=1; y < h1; y++){
		for(x=1; x < w1; x++){
			yx = y*w + x;
			if(img[yx] && img[yx] < 253){
				if(loc_max(img, yx, w)){
					find_lines(pix, img, x, y, yx, w);
				}
			}
		}
	}
	//printf("Numbers of pixels  = %d\n", npix);
	//printf("Numbers of lines   = %d\n", nline);
}

void seg_reduce_line(Pixel *pix, imgtype *img, uint32 w, uint32 h)
{
	uint32 y, x, yx, w1 = w-1, h1 = h-1;
	Pixel *pix1, *pix2;
	for(y=1; y < h1; y++){
		for(x=1; x < w1; x++){
			yx = y*w + x;
			if(img[yx] == 255 && pix[yx].nout && pix[yx].out->nout){
				pix1 = pix[yx].out;
				pix2 = pix[yx].out->out;
				while(1){
					if(is_in_line1(pix[yx].x - pix1->x, pix[yx].y - pix1->y, pix1->x - pix2->x, pix1->y - pix2->y)){
						//printf("dx1 = %d dy1 = %d dx2 = %d dy2 = %d\n", pix[yx].x - pix1->x, pix[yx].y - pix1->y, pix1->x - pix2->x, pix1->y - pix2->y);
						pix[yx].out = pix2;
						pix1->nout = 0; pix1->nin = 0;
						img[yx] = 254;
						if(pix2->nout){ pix1 = pix2; pix2 = pix1->out; }
						else break;
					} else break;
				}
			}
		}
	}
	//printf("Numbers of pixels  = %d\n", npix);
	//printf("Numbers of lines   = %d\n", nline);
}

#define xy(a,b,c) ((c) ? (a)*w + (b) : (b)*w + (a))

static inline void draw_line(imgtype *img, Vector *v, uint32 w, uint32 col)
{
	uint32 i;
	int dx = v->x2 - v->x1, dy = v->y2 - v->y1;
	uint32 stx, sty, dxa = abs(dx)+1, dya = abs(dy)+1;
	uint32 mit, mat, max, min, mi, ma, mist, mast, c, yx;

	stx = dx < 0 ? -1 : (dx > 0 ? 1 : 0);
	sty = dy < 0 ? -1 : (dy > 0 ? 1 : 0);

	if(dxa >= dya) 	{ max = dxa; min = dya; mi = v->y1; ma = v->x1; mist = sty; mast = stx; c = 1; }
	else 			{ max = dya; min = dxa; mi = v->x1; ma = v->y1; mist = stx; mast = sty; c = 0; }

	mat = max; mit = min;
	for(i=0; i < max; i++){
		img[xy(mi, ma, c)] = col;
		//yx = xy(mi, ma, c);
		if(mit >= mat) { mat += max; mi += mist;}
		mit += min;
		ma += mast;
	}
}

void seg_draw_lines(Pixel *pix, uint32 npix, imgtype *img, uint32 w, uint32 h)
{
	uint32 i, j, k, pixs = 0, nline = 0;
	Vector xy;
	for(i=0; i < w*h; i++){
		//if(pix[i].nnei > 4){
			//if(pix[i].nout) {
			//if(pix[i].pow == 255) {
			if(pix[i].nout) {
				if(pix[i].nout > 1) printf("nout = %d\n",pix[i].nout);
				//printf("x = %d y = %d x1 = %d y1 = %d pow = %d\n", pix[i].x, pix[i].y, pix[i].out->x, pix[i].out->y, pix[i].pow);
				xy.x1 = pix[i].x; xy.y1 = pix[i].y;
				xy.x2 = pix[i].out->x; xy.y2 = pix[i].out->y;
				draw_line(img, &xy, w, pix[i].pow);

				//draw_line(img, pix[i].x, pix[i].y, pix[i].out->x, pix[i].out->y, w, pix[i].pow);
				//if(pix[i].pow == 255) printf("pow = %d\n", pix[i].pow);
				//draw_line(img, pix[i].x, pix[i].y, pix[i].out->x, pix[i].out->y, w, 255);
				nline++;
			}
	}

	for(i=0; i < w*h; i++){
		if(pix[i].nout || pix[i].nin){
		//if(pix[i].nout){
			img[i] = 255;
			pixs++;
		}
	}

	printf("Pixels  = %d\n", pixs);
	printf("Lines   = %d\n", nline);
}

void seg_draw_pix(Pixel *pix, uint32 npix, imgtype *img, uint32 w, uint32 h)
{
	uint32 i, j, k, pixs = 0, nline = 0;
	for(i=0; i < w*h; i++){
		if(pix[i].nout || pix[i].nin){
			img[i] = 255;
			pixs++;
		}
	}
	//printf("Numbers of pixels  = %d\n", pixs);
}

static inline uint32 diff3x3( imgtype *img1, imgtype *img2, uint32 yx1, uint32 yx2,  uint32 w)
{
	return (abs(img1[yx1    ] - img2[yx2    ]) +
			abs(img1[yx1-1  ] - img2[yx2-1  ]) +
			abs(img1[yx1-1-w] - img2[yx2-1-w]) +
			abs(img1[yx1  -w] - img2[yx2  -w]) +
			abs(img1[yx1+1-w] - img2[yx2+1-w]) +
			abs(img1[yx1+1  ] - img2[yx2+1  ]) +
			abs(img1[yx1+1+w] - img2[yx2+1+w]) +
			abs(img1[yx1  +w] - img2[yx2  +w]) +
			abs(img1[yx1-1+w] - img2[yx2-1+w]));
}

static inline uint16 block_match(imgtype *grad, imgtype *img1, imgtype *img2, uint32 x1, uint32 y1,  uint32 *xo, uint32 *yo, uint32 w, uint32 h, uint32 st)
{
	int x, y, yx, yx1 = y1*w + x1,  sad, npix = 0;
	uint16 min = 0xFFFF;
	int ax = x1 - st, ay = y1 - st, bx = x1 + st, by = y1 + st;
	if(ax <= 0) ax = 1;
	if(ay <= 0) ay = 1;
	if(bx >= w) bx = w-1;
	if(by >= h) by = h-1;
	//printf("ax = %d bx = %d ay = %d by = %d x = %d y = %d\n",ax, bx, ay, by, x1, y1);
	for(y=ay; y < by; y++){
		for(x=ax; x < bx; x++){
			yx = y*w + x;
			//printf("x1 = %d y1 = %d  \n", x, y);
			if(grad[yx] > 253){
			//if(grad[yx] == 255){
				sad = diff3x3( img1, img2, yx1, yx, w);
				if(sad < min) { min = sad; *xo = x; *yo = y; }
				npix++;
			}
		}
	}
	if(!npix) { *xo = x1; *yo = y1;}
	//if(abs(*xo - x1) > 8 || abs(*yo - y1) > 8 )
	if(min )
	printf("npix = %3d min = %4d x = %4d y = %4d x1 = %4d y1 = %4d vx = %4d vy = %4d", npix, min, x1, y1, *xo, *yo, *xo - x1, *yo - y1);
	return min;
}

void seg_compare1(Pixel *pix, Pixel *pix1, imgtype *grad1, imgtype *grad2, imgtype *img1, imgtype *img2, uint32 w, uint32 h)
{
	uint32 yx, y, y1,  x, x1, w1 = w-1, h1 = h-1, xo, yo, npix = 0;
	for(y=1; y < h1; y++){
		for(x=1; x < w1; x++){
			yx = y*w + x;
			if(grad1[yx] == 255){
				pix[yx].mach = block_match(grad2, img1, img2, x, y,  &xo, &yo, w, h, 12);
				pix[yx].vx = xo - x;
				pix[yx].vy = yo - y;
				grad1[yx] = 253;
				npix++;
			}
		}
	}
	printf("seg_compare npix = %d\n", npix);
}

void seg_compare(Pixel *pix, Pixel *pix1, imgtype *grad1, imgtype *grad2, imgtype *img1, imgtype *img2, uint32 w, uint32 h)
{
	int yx, yx1, y, y1,  x, x1, w1 = w-1, h1 = h-1, xo, yo, npix = 0;
	Pixel *p;
	for(y=1; y < h1; y++){
		for(x=1; x < w1; x++){
			yx = y*w + x;
			if(grad1[yx] == 255){
				p = &pix[yx];
				p->mach = block_match(grad2, img1, img2, x, y,  &xo, &yo, w, h, 12);
				if(p->mach ) printf(" block = 12\n");
				p->vx = xo - x;
				p->vy = yo - y;
				yx1 = yx;
				npix++;
				while(1){
					if(p->nout && grad1[yx1] == 255) {
						grad1[yx1] = 253;
						x1 = p->out->x + p->vx;
						y1 = p->out->y + p->vy;
						if(x1 >= 0 && x1 < w && y1 >= 0 && y1 < h){
							yx1 = p->out->x + p->out->y * w;
							p = &pix[yx1];
							//printf("block = 2\n");
							p->mach = block_match(grad2, img1, img2, x1, y1,  &xo, &yo, w, h, 5);
							if(p->mach ) printf(" block = 2\n");
							p->vx = xo - p->x;
							p->vy = yo - p->y;
							//grad1[yx1] = 253;
							//p = &pix[yx1];
							npix++;
						} else break;
					} else break;
				}
			}
		}
	}
	printf("seg_compare npix = %d\n", npix);
}

void seg_draw_vec(Pixel *pix, uint32 npix, imgtype *img, uint32 w, uint32 h)
{
	uint32 i, j, k, sq = w*h, pixs = 0, nline = 0;
	Vector xy;
	for(i=0; i < sq; i++){
		if(pix[i].nout && pix[i].vx && pix[i].vy) {
			xy.x1 = pix[i].x; xy.y1 = pix[i].y;
			xy.x2 = pix[i].x + pix[i].vx; xy.y2 = pix[i].y + pix[i].vy;
			draw_line(img, &xy, w, 100);
			//draw_line(img, pix[i].x, pix[i].y, pix[i].x + pix[i].vx, pix[i].y + pix[i].vy, w, 100);
			//printf("x = %d %d y = %d %d\n", pix[i].x, pix[i].vx, pix[i].y,  pix[i].vy);
			nline++;
		}
	}
	/*
	for(i=0; i < w*h; i++){
		if(pix[i].nout || pix[i].nin){
			img[i] = 255;
			pixs++;
		}
	}*/

	printf("Numbers of pixels  = %d\n", pixs);
	printf("Numbers of lines   = %d\n", nline);
}

static inline uint32 intersect(imgtype *img, uint32 yx, uint32 w)
{
	char i, tmp, inter = 0, ni[8];
	ni[0]=0; ni[1]=0; ni[2]=0; ni[3]=0; ni[4]=0; ni[5]=0; ni[6]=0; ni[7]=0;
	if(img[yx-1  ] == 255) { inter++; ni[0] = 1;}
	if(img[yx-1-w] == 255) { inter++; ni[1] = 1;}
	if(img[yx-w  ] == 255) { inter++; ni[2] = 1;}
	if(img[yx-w+1] == 255) { inter++; ni[3] = 1;}
	if(img[yx+1  ] == 255) { inter++; ni[4] = 1;}
	if(img[yx+1+w] == 255) { inter++; ni[5] = 1;}
	if(img[yx+w  ] == 255) { inter++; ni[6] = 1;}
	if(img[yx+w-1] == 255) { inter++; ni[7] = 1;}
	if(inter == 1) return 1;
	if(inter == 2) return 0;
	if(inter > 2)
		for(i=0; i<7; i++) if(ni[i] == 1 && ni[i+1] == 1) return 0;
	if(ni[7] == 1 && ni[0] == 1) return 0;
	return 1;
}

void seg_intersect_pix(imgtype *img1, imgtype *img2, uint32 w, uint32 h)
{
	uint32 yx, y, y1, x, x1, w1 = w-1, h1 = h-1, npix = 0;
	for(y=1; y < h1; y++){
		for(x=1; x < w1; x++){
			yx = y*w + x;
			if(img1[yx] == 255) if(intersect(img1, yx, w)) { img2[yx] = 255; npix++;}
		}
	}
	printf("seg_intersect_pix npix = %d\n", npix);
}

static inline void copy_block(imgtype *img1, uint32 yx1, imgtype *img2, uint32 yx2, uint32 w)
{
	img2[yx2    ] = img1[yx1    ];
	img2[yx2-1  ] = img1[yx1-1  ];
	img2[yx2-1-w] = img1[yx1-1-w];
	img2[yx2  -w] = img1[yx1  -w];
	img2[yx2+1-w] = img1[yx1+1-w];
	img2[yx2+1  ] = img1[yx1+1  ];
	img2[yx2+1+w] = img1[yx1+1+w];
	img2[yx2  +w] = img1[yx1  +w];
	img2[yx2-1+w] = img1[yx1-1+w];
}

static inline void copy_vector(imgtype *img1, Vector *v1, imgtype *img2, Vector *v2, uint32 w)
{
	int x, y, yx;
	uint32 i, j, st, l1, l2;
	int dx1 = v1->x2 - v1->x1, dy1 = v1->y2 - v1->y1;
	int dx2 = v2->x2 - v2->x1, dy2 = v2->y2 - v2->y1;
	int dxa1 = abs(dx1)+1, dya1 = abs(dy1)+1;
	int dxa2 = abs(dx2)+1, dya2 = abs(dy2)+1;
	int stx1 = dx1 < 0 ? -1 : (dx1 > 0 ? 1 : 0);
	int sty1 = dy1 < 0 ? -1 : (dy1 > 0 ? 1 : 0);
	int stx2 = dx2 < 0 ? -1 : (dx2 > 0 ? 1 : 0);
	int sty2 = dy2 < 0 ? -1 : (dy2 > 0 ? 1 : 0);

	int mit1 = 0, mat1 = 0, max1, min1, mi1, ma1, mist1, mast1, c1, yx1;
	int mit2 = 0, mat2 = 0, max2, min2, mi2, ma2, mist2, mast2, c2, yx2;

	l1 = (dxa1 >= dya1) ? dxa1 : dya1;
	l2 = (dxa2 >= dya2) ? dxa2 : dya2;

	if(dxa1 >= dya1) 	{ max1 = dxa1; min1 = dya1; mi1 = v1->y1; ma1 = v1->x1; mist1 = sty1; mast1 = stx1; c1 = 1; }
	else 				{ max1 = dya1; min1 = dxa1; mi1 = v1->x1; ma1 = v1->y1; mist1 = stx1; mast1 = sty1; c1 = 0; }
	mit1 = min1; mat1 = max1;

	if(dxa2 >= dya2) 	{ max2 = dxa2; min2 = dya2; mi2 = v2->y1; ma2 = v2->x1; mist2 = sty2; mast2 = stx2; c2 = 1; }
	else 				{ max2 = dya2; min2 = dxa2; mi2 = v2->x1; ma2 = v2->y1; mist2 = stx2; mast2 = sty2; c2 = 0; }
	mit2 = min2; mat2 = max2;

	if(l1 >= l2){
		st = l1;
		for(i=0,j=l2; i < l1; i++, j+=l2){
			if(j >= st) {
				img2[yx2] = img1[yx1];
				st += l1;
				yx2 = xy(mi2, ma2, c2);
				if(mit2 >= mat2) { mat2 += max2; mi2 += mist2;}
				mit2 += min2;
				ma2 += mast2;
			}
			yx1 = xy(mi1, ma1, c1);
			if(mit1 >= mat1) { mat1 += max1; mi1 += mist1;}
			mit1 += min1;
			ma1 += mast1;
		}
	} else {
		st = l2;
		for(i=0,j=l1; i < l2; i++, j+=l1){
			img2[yx2] = img1[yx1];
			if(j >= st) {
				st += l2;
				yx1 = xy(mi1, ma1, c1);
				if(mit1 >= mat1) { mat1 += max1; mi1 += mist1;}
				mit1 += min1;
				ma1 += mast1;
			}
			yx2 = xy(mi2, ma2, c2);
			if(mit2 >= mat2) { mat2 += max2; mi2 += mist2;}
			mit2 += min2;
			ma2 += mast2;
		}
	}
}

void seg_mvector_copy(Pixel *pix, imgtype *grad1, imgtype *img1, imgtype *img2, uint32 w, uint32 h)
{
	uint32 yx, y, y1, x, x1, w1 = w-1, h1 = h-1, npix = 0;
	Vector v1, v2;
	for(y=1; y < h1; y++){
		for(x=1; x < w1; x++){
			yx = y*w + x;
			if(grad1[yx] == 253 && pix[yx].nout){
				v1.x1 = pix[yx].x; v1.y1 = pix[yx].y;
				v1.x2 = pix[yx].out->x; v1.y2 = pix[yx].out->y;

				v2.x1 = pix[yx].x + pix[yx].vx; 			v2.y1 = pix[yx].y + pix[yx].vy;
				v2.x2 = pix[yx].out->x + pix[yx].out->vx; 	v2.y2 = pix[yx].out->y + pix[yx].out->vy;

				//printf("x1  = %d y1 = %d x2 = %d y2 = %d\n", abs(v1.x2-v1.x1), abs(v1.y2-v1.y1), abs(v2.x2-v2.x1), abs(v2.y2-v2.y1));
				//copy_block(img1, yx, img2, (pix[yx].x + pix[yx].vx) + (pix[yx].y + pix[yx].vy)*w, w);
				copy_vector(img1, &v1, img2, &v2, w);
				npix++;
			}
		}
	}
	printf("seg_mvector_copy nvectors = %d\n", npix);
}
