#include <walet.h>
#include <stdio.h>
#include <math.h>

void seg_grad(imgtype *img, imgtype *img1, uint32 w, uint32 h, uint32 th)
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
	uint32 y, x, yx, sq = w*h-w, w1 = w-1, h1 = h-1;
	uchar max;
	uint32 g[4];
	for(y=w; y < sq; y+=w){
		for(x=1; x < w1; x++){
			yx = y + x;
			g[0] = abs(img[yx-1  ] - img[yx+1  ]);
			g[1] = abs(img[yx-1-w] - img[yx+1+w]);
			g[2] = abs(img[yx-w  ] - img[yx+w  ]);
			g[3] = abs(img[yx+1-w] - img[yx-1+w]);
			//if(y == w)
			//printf("yx-1 = %3d yx+1 = %3d yx-w = %3d yx+w = %3d yx-1-w = %3d yx+1+w = %3d yx+1-w = %3d yx-1+w = %3d\n",
			//		yx-1, yx+1, yx-w, yx+w, yx-1-w, yx+1+w, yx+1-w, yx-1+w);
			//max = g[0]; in = 2;
			//if(max < g[1]) { max = g[1]; in = 3; }
			//if(max < g[2]) { max = g[2]; in = 0; }
			//if(max < g[3]) { max = g[3]; in = 1; }
			max = (g[0] + g[1] + g[2] + g[3])>>2;
			max = max > 252 ? 252 : max;
			//img1[yx] = max;
			//img1[yx] = max>>th ? (max >= 255 ? 254 : (max>>th)<<th): 0;
			//img1[yx] = (max>>th) ? (max > 252 ? 252 : max) : 0;
			img1[yx] = max>>th ? max : 0;
			//printf("yx = %d max = %d\n", yx, max);
		}
	}
}

void seg_grad1(imgtype *img, imgtype *img1, uint32 w, uint32 h, uint32 th)
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
			img1[yx] = max>>th ? (max >253 ? 253 : max) : 0;
			//img1[yx] = max>>th ? (max>>th)<<th : 0; img2[yx] = in;
		}
	}
}

void seg_grad2(imgtype *img, imgtype *img1, uint32 w, uint32 h, uint32 th)
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
			img1[yx] = max>>th ? (max >253 ? 253 : max) : 0;
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
                //goto end;
        }
        else if(dx == -1 && dy == 0){
                //if(img[yx  -w] > max) { max = img[yx  -w]; *dx1 =  0; *dy1 = -1;}
                if(img[yx+1-w] > max) { max = img[yx+1-w]; *dx1 =  1; *dy1 = -1;}
                if(img[yx+1  ] >= max){ max = img[yx+1  ]; *dx1 =  1; *dy1 =  0;}
                if(img[yx+1+w] > max) { max = img[yx+1+w]; *dx1 =  1; *dy1 =  1;}
                //if(img[yx  +w] > max) { max = img[yx  +w]; *dx1 =  0; *dy1 =  1;}
                //goto end;
        }
        else if(dx == -1 && dy == -1){
                //if(img[yx+1-w] > max) { max = img[yx+1-w]; *dx1 =  1; *dy1 = -1;}
                if(img[yx+1  ] > max) { max = img[yx+1  ]; *dx1 =  1; *dy1 =  0;}
                if(img[yx+1+w] >= max){ max = img[yx+1+w]; *dx1 =  1; *dy1 =  1;}
                if(img[yx  +w] > max) { max = img[yx  +w]; *dx1 =  0; *dy1 =  1;}
                //if(img[yx-1+w] > max) { max = img[yx-1+w]; *dx1 = -1; *dy1 =  1;}
                //goto end;
        }
        else if(dx ==  0 && dy == -1){
                //if(img[yx+1  ] > max) { max = img[yx+1  ]; *dx1 =  1; *dy1 =  0;}
                if(img[yx+1+w] > max) { max = img[yx+1+w]; *dx1 =  1; *dy1 =  1;}
                if(img[yx  +w] >= max){ max = img[yx  +w]; *dx1 =  0; *dy1 =  1;}
                if(img[yx-1+w] > max) { max = img[yx-1+w]; *dx1 = -1; *dy1 =  1;}
                //if(img[yx  -1] > max) { max = img[yx  -1]; *dx1 = -1; *dy1 =  0;}
                //goto end;
        }
        else if(dx ==  1 && dy == -1){
                //if(img[yx+1+w] > max) { max = img[yx+1+w]; *dx1 =  1; *dy1 =  1;}
                if(img[yx  +w] > max) { max = img[yx  +w]; *dx1 =  0; *dy1 =  1;}
                if(img[yx-1+w] >= max){ max = img[yx-1+w]; *dx1 = -1; *dy1 =  1;}
                if(img[yx  -1] > max) { max = img[yx  -1]; *dx1 = -1; *dy1 =  0;}
                //if(img[yx-1-w] > max) { max = img[yx-1-w]; *dx1 = -1; *dy1 = -1;}
                //goto end;
        }
        else if(dx ==  1 && dy ==  0){
                //if(img[yx  +w] > max) { max = img[yx  +w]; *dx1 =  0; *dy1 =  1;}
                if(img[yx-1+w] > max) { max = img[yx-1+w]; *dx1 = -1; *dy1 =  1;}
                if(img[yx  -1] >= max){ max = img[yx  -1]; *dx1 = -1; *dy1 =  0;}
                if(img[yx-1-w] > max) { max = img[yx-1-w]; *dx1 = -1; *dy1 = -1;}
                //if(img[yx  -w] > max) { max = img[yx  -w]; *dx1 =  0; *dy1 = -1;}
                //goto end;
        }
        else if(dx ==  1 && dy ==  1){
                //if(img[yx-1+w] > max) { max = img[yx-1+w]; *dx1 = -1; *dy1 =  1;}
                if(img[yx  -1] > max) { max = img[yx  -1]; *dx1 = -1; *dy1 =  0;}
                if(img[yx-1-w] >= max){ max = img[yx-1-w]; *dx1 = -1; *dy1 = -1;}
                if(img[yx  -w] > max) { max = img[yx  -w]; *dx1 =  0; *dy1 = -1;}
                //if(img[yx+1-w] > max) { max = img[yx+1-w]; *dx1 =  1; *dy1 = -1;}
                //goto end;
        }
        else if(dx ==  0 && dy ==  1){
                //if(img[yx  -1] > max) { max = img[yx  -1]; *dx1 = -1; *dy1 =  0;}
                if(img[yx-1-w] > max) { max = img[yx-1-w]; *dx1 = -1; *dy1 = -1;}
                if(img[yx  -w] >= max){ max = img[yx  -w]; *dx1 =  0; *dy1 = -1;}
                if(img[yx+1-w] > max) { max = img[yx+1-w]; *dx1 =  1; *dy1 = -1;}
                //if(img[yx+1  ] > max) { max = img[yx+1  ]; *dx1 =  1; *dy1 =  0;}
                //goto end;
        }
        else if(dx == -1 && dy ==  1){
                //if(img[yx-1-w] > max) { max = img[yx-1-w]; *dx1 = -1; *dy1 = -1;}
                if(img[yx  -w] > max) { max = img[yx  -w]; *dx1 =  0; *dy1 = -1;}
                if(img[yx+1-w] >= max){ max = img[yx+1-w]; *dx1 =  1; *dy1 = -1;}
                if(img[yx+1  ] > max) { max = img[yx+1  ]; *dx1 =  1; *dy1 =  0;}
                //if(img[yx+1+w] > max) { max = img[yx+1+w]; *dx1 =  1; *dy1 =  1;}
               // goto end;
        }

}

static inline int dir2(imgtype *img, uint32 w, uint32 yx, int dyx, uint32 max)
//Check for pixel
{
	//printf("dyx = %d\n", dyx);
	if(dyx == -1){
			if(img[yx+1-w] > max) return 1-w;
			if(img[yx+1  ] > max) return 1;
			if(img[yx+1+w] > max) return 1+w;
	 }
	else if(dyx == -1-w){
			if(img[yx+1  ] > max) return 1;
			if(img[yx+1+w] > max) return 1+w;
			if(img[yx  +w] > max) return w;
	}
	else if(dyx == -w){
			if(img[yx+1+w] > max) return 1+w;
			if(img[yx  +w] > max) return w;
			if(img[yx-1+w] > max) return -1+w;
	 }
	else if(dyx == 1-w){
			if(img[yx  +w] > max) return w;
			if(img[yx-1+w] > max) return -1-w;
			if(img[yx  -1] > max) return -1;
	 }
	else if(dyx == 1){
			if(img[yx-1+w] > max) return -1+w;
			if(img[yx  -1] > max) return -1;
			if(img[yx-1-w] > max) return -1-w;
	 }
	else if(dyx == 1+w){
			if(img[yx  -1] > max) return -1;
			if(img[yx-1-w] > max) return -1-w;
			if(img[yx  -w] > max) return -w;
	 }
	else if(dyx == w){
			if(img[yx-1-w] > max) return -1-w;
			if(img[yx  -w] > max) return -w;
			if(img[yx+1-w] > max) return 1-w;
	 }
	else if(dyx == -1+w){
			if(img[yx  -w] > max) return -w;
			if(img[yx+1-w] > max) return 1-w;
			if(img[yx+1  ] > max) return 1;
	 }
	printf("not find\n");
}

static inline void new_pix(Pixel *pix, imgtype img, uint32 x, uint32 y)
{
	pix->nin = 0; pix->nout = 0; pix->x = x; pix->y = y; //pix->draw = 1; //pix->end = 0;
}

static inline void new_pix1(Pixel *pix, imgtype img, uint32 x, uint32 y, uint32 nei)
{
	pix->nin = 0; pix->nout = 0; pix->x = x; pix->y = y; pix->nnei = nei;//pix->draw = 1; //pix->end = 0;
}

static inline void new_line(Pixel *pix, Pixel *pix1, uchar pow, uint16 npix)
{
	pix->out = pix1;   pix->nout++; pix->pow = pow; pix1->pow = pow; //pix->pow[pix->nout] = pow; pix->pow[pix->nout] = pow;
	pix1->nin++; pix->npix = npix;
}

static inline void change_line(Pixel *pix, Pixel *pix1, uchar pow, uint16 npix)
{
	pix->out = pix1;  pix->nout = 1; pix->pow = pow;  //pix->pow[pix->nout] = pow; pix->pow[pix->nout] = pow;
	pix1->nin = 1; pix1->pow = pow; pix->npix = npix;
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

static inline void set_blocks(Pixel *pix, uint32 yx, int dx, int dy, uint32 w)
{
	if(dx) { pix->yx = w; return; }  //pix->yx2 = yx + w; }
	else   { pix->yx = 1; }//pix->yx2 = yx + 1; }
}

static inline uint32 check_dir(int dx1, int dy1, int dx2, int dy2)
{
	if(dx1 != 0 && dy1 != 0 ) if(dx2 == -dx1 || dy2 == -dy1) return 1;
	else{
		if(dx1 == 0 ) if(dx2 != 0 && dy2 == 0) return 1;
		else  if(dx2 == 0 && dy2 != 0) return 1;
	}
	return 0;
}

static inline uint32 check_corner(int dx1, int dy1, int dx2, int dy2)
{
	if(!dx1) if(dx2 && !dy2) return 1;
	if(!dy1) if(dy2 && !dx2) return 1;
	if(dx1 && dy1) if((dx1 == -dx2) || (dy1 == -dy2)) return 1;
	return 0;
}

static inline uint32 find_pixels(Pixel *pix, imgtype *img, uint32 x, uint32 y, uint32 dx, uint32 dy, uint32 w)
{
	uint32 npix = 0, len = 0, yx = y*w + x, x1 = x, y1 = y, yx1 = yx, x2, y2, yx2, yxt,  xt, yt, c = 0, c2 = 0; //yx2 = yx1,
	uchar min = img[yx];
	int dx1, dy1, dx2, dy2;
	dx1 = dx; dy1 = dy;
    while(1){
    	yxt = yx; xt = x; yt = y; // Save previous pixel
      	yx = yx + dy*w + dx; x = x + dx; y = y + dy; c++;
       	if(img[yx]) min = img[yx] < min ? img[yx] : min;
		if(!img[yx]) { //End point with 0
			//new_pix1(&pix[yx], img[yx], x, y, 1);
			//img[yx] = 255; npix++;
			new_pix1(&pix[yxt], img[yxt], xt, yt, 1);
			img[yxt] = 255; npix++;
			return npix;
		}
		if(img[yx] == 254) { //End point with 0
			new_pix1(&pix[yx], img[yx], x, y, 3);
			img[yx] = 255; npix++;
			return npix;
		}
		if(img[yx] == 255) { //End point with 0
			pix[yx].nnei++;
			return npix;
		}
		img[yx] = 254;
		dx2 = dx; dy2 = dy;
		//dx1 = dx; dy1 = dy;
		dir1(img, w, yx, -dx, -dy, &dx, &dy);
		if(dx1 == dx && dy1 == dy) { x2 = x; y2 = y; yx2 = yx; c2 = c; }
		if((c > 2 && check_corner(dx1, dy1, dx, dy))){ //New point
			//printf("dx1 = %d dx = %d dy1 = %d dy = %d c = %d test = %d\n", dx1, dx, dy1, dy, c, (dx1 == -dx) || (dy1 == -dy));
			if(!c2) {
				//new_pix1(&pix[yx], img[yx], x, y, );
				//img[yx] = 255; npix++;
			} else {
				new_pix1(&pix[yx], img[yx], x, y, 2);
				new_pix1(&pix[yx2], img[yx2], x2, y2, 2);
				img[yx] = 255; img[yx2] = 255; npix += 2;
			}
			dx1 = dx; dy1 = dy;
			x1 = x; y1 = y; yx1 = yx; c = 0; c2 = 0;
		}
    }
	return npix;
}

static inline uint32 find_lines(Pixel *pix, imgtype *img, uint32 x, uint32 y, uint32 dx, uint32 dy, uint16 *npix, uint16 *nline, uint32 w, uchar dir)
{
	uint32 len = 0, yx = y*w + x, x1 = x, y1 = y, yx1 = yx, yxt,  xt, yt, c = 0; //yx2 = yx1,
	uchar min = img[yx];
	//imgtype im;
	int dx1, dy1, dx2, dy2;
	if(dir){
		new_pix(&pix[yx1], img[yx1], x1, y1); img[yx1] = 255;
		set_blocks(&pix[yx1], yx1, dx, dy, w);
	}
    while(1){
     	//len += length(dx, dy);
    	yxt = yx; xt = x; yt = y; // Save previous pixel
     	yx = yx + dy*w + dx; x = x + dx; y = y + dy; c++;
      	if(img[yx]) min = img[yx] < min ? img[yx] : min;
		if(img[yx] == 255 || img[yx] == 254 || !img[yx]) { //End point with 0
			if(c > 1){
				new_pix(&pix[yxt], img[yxt], xt, yt); img[yxt] = 255;
				set_blocks(&pix[yxt], yxt, dx, dy, w);
				if(dir)	new_line(&pix[yx1], &pix[yxt], min, c);
				else	new_line(&pix[yxt], &pix[yx1], min, c);
				(*nline)++; *npix += c;
				return yxt;
			} else return yxt;
		}
		img[yx] = 254;
		dx2 = dx1; dy2 = dy1;
		dx1 = dx; dy1 = dy;
		dir1(img, w, yx, -dx, -dy, &dx, &dy);
		if(c > 2 && is_in_line(dx, dy, dx2, dy2)){ //New point
			//printf("dx1 = %d dx = %d dy1 = %d dy = %d c = %d\n", dx1, dx, dy1, dy, c);
			new_pix(&pix[yx], img[yx], x, y);
			img[yx] = 255; //im = img[yx], Save previous value
			set_blocks(&pix[yx], yx, dx && dx1, dy && dy1, w);
			if(dir)	new_line(&pix[yx1], &pix[yx ], min, c);
			else 	new_line(&pix[yx ], &pix[yx1], min, c);
			(*nline)++; *npix += c;
			x1 = x; y1 = y; yx1 = yx; c = 0; //len = 0; yx2 = yx1;
		}
    }
}

uint32 seg_pixels(Pixel *pix, imgtype *img, uint32 w, uint32 h)
{
	uint32 y, x, yx, w1 = w-1, h1 = h-1;
	uint32 npix = 0;
	int dx, dy, dx1, dy1;
	for(y=1; y < h1; y++){
		for(x=1; x < w1; x++){
			yx = y*w + x;
			if(img[yx] && img[yx] < 253){
				if(loc_max(img, yx, w)){
					dir1(img, w,  yx,  0,  0,  &dx,  &dy);
					dir1(img, w,  yx, dx, dy, &dx1, &dy1);
					npix += find_pixels(pix, img, x, y, dx,  dy , w);
					//printf("nedge = %d 1 pixs = %3d lines = %3d ", nedge, edges[nedge].pixs, edges[nedge].lines);
					npix += find_pixels(pix, img, x, y, dx1, dy1, w);
					//printf("0 pixs = %3d lines = %3d ", edges[nedge].pixs, edges[nedge].lines);
					//printf("yxs = %7d yxe = %7d pixs = %3d lines = %3d\n", edges[nedge].yxs, edges[nedge].yxe, edges[nedge].pixs, edges[nedge].lines);
				}
			}
		}
	}
	printf("Numbers of pixels  = %6d\n", npix);
	return npix;

}

static inline uint32 set_dir(imgtype *img, uint32 yx, uint32 w, int dyx)
{
	uchar dir;
	if     (img[yx-1  ] == dyx) dir += 1;
	else if(img[yx-1-w] == dyx) dir += 2;
	else if(img[yx  -w] == dyx) dir += 4;
	else if(img[yx+1-w] == dyx) dir += 8;
	else if(img[yx+1  ] == dyx) dir += 16;
	else if(img[yx+1+w] == dyx) dir += 32;
	else if(img[yx  +w] == dyx) dir += 64;
	else if(img[yx-1+w] == dyx) dir += 128;
	return dir;
}

static inline uint32 get_dir(imgtype *img, uint32 yx, uint32 w, uchar dir, int *dm)
{
	uchar i;
	if(img[yx-1  ] > 253 && !(dir&1  )) { dm[i] = -1;   i++; }
	if(img[yx-1-w] > 253 && !(dir&2  )) { dm[i] = -1-w; i++; }
	if(img[yx  -w] > 253 && !(dir&4  )) { dm[i] =   -w; i++; }
	if(img[yx+1-w] > 253 && !(dir&8  )) { dm[i] =  1-w; i++; }
	if(img[yx+1  ] > 253 && !(dir&16 )) { dm[i] =  1;   i++; }
	if(img[yx+1+w] > 253 && !(dir&32 )) { dm[i] =  1+w; i++; }
	if(img[yx  +w] > 253 && !(dir&64 )) { dm[i] =  w;   i++; }
	if(img[yx-1+w] > 253 && !(dir&128)) { dm[i] = -1+w; i++; }
	return i;
}

static inline void test(imgtype *img, uint32 yx, uint32 w)
{
	printf("%3d ",img[yx-1-w]);
	printf("%3d ",img[yx-w]);
	printf("%3d\n",img[yx+1-w]);
	printf("%3d ",img[yx-1]);
	printf("%3d ",img[yx]);
	printf("%3d\n",img[yx+1]);
	printf("%3d ",img[yx-1+w]);
	printf("%3d ",img[yx+w]);
	printf("%3d\n",img[yx+1+w]);
}

uint32 seg_region(Pixel *pix, imgtype *img, uint32 w, uint32 h)
{
	uint32 i, y, x, yx, yx1, w1 = w-1, h1 = h-1;
	uint32 npix = 0, n;
	int dm[5], dyx;
	for(y=1; y < h1; y++){
		for(x=1; x < w1; x++){
			yx = y*w + x;
			if(img[yx] == 255){
				npix++;
				n = get_dir(img, yx, w, pix[yx].dir, dm);
				test(img, yx, w);
				printf("dir = %d n = %d \n", pix[yx].dir, n);

				for(i=0; i < n; i++){
					yx1 = yx;
					printf("dm[%d] = %d \n", i, dm[i]);
					dyx = dm[i];
					while(1){
						test(img, yx1, w);
						yx1 = yx1 + dyx;
						printf("img = %d  dyx = %d\n", img[yx1], dyx);
						if(img[yx1] == 255){
							pix[yx1].dir = set_dir(img, yx1, w, -dyx);
							break;
						}
						dyx = dir2(img, w, yx1, -dyx, 253);
					}
				}
			}
		}
	}
	printf("Numbers of pixels = %6d\n", npix);
	return npix;

}

uint32 seg_line(Pixel *pix, Edge *edges, imgtype *img, uint32 w, uint32 h)
{
	uint32 y, x, yx, w1 = w-1, h1 = h-1;
	uint32 npix = 0, nline = 0, nedge = 0, px, ln;
	int dx, dy, dx1, dy1;
	for(y=1; y < h1; y++){
		for(x=1; x < w1; x++){
			yx = y*w + x;
			if(img[yx] && img[yx] < 253){
				if(loc_max(img, yx, w)){
					dir1(img, w,  yx,  0,  0,  &dx,  &dy);
					dir1(img, w,  yx, dx, dy, &dx1, &dy1);
					edges[nedge].yxe = find_lines(pix, img, x, y, dx,  dy,  &edges[nedge].pixs, &edges[nedge].lines, w, 1);
					//printf("nedge = %d 1 pixs = %3d lines = %3d ", nedge, edges[nedge].pixs, edges[nedge].lines);
					edges[nedge].yxs = find_lines(pix, img, x, y, dx1, dy1, &edges[nedge].pixs, &edges[nedge].lines, w, 0);
					//printf("0 pixs = %3d lines = %3d ", edges[nedge].pixs, edges[nedge].lines);
					//printf("yxs = %7d yxe = %7d pixs = %3d lines = %3d\n", edges[nedge].yxs, edges[nedge].yxe, edges[nedge].pixs, edges[nedge].lines);
					npix += edges[nedge].pixs;
					nline += edges[nedge].lines;
					if(edges[nedge].yxs != edges[nedge].yxe) nedge++;
					//find_lines(pix, img, x, y, w);
				}
			}
		}
	}
	printf("Numbers of pixels  = %6d\n", npix);
	printf("Numbers of lines   = %6d\n", nline);
	printf("Numbers of edges   = %6d\n", nedge);
	return nedge;

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

static inline uint32 draw_line(imgtype *img, Vector *v, uint32 w, uint32 col, uchar end)
//Draw line, return number of pixels in line
//If end 1 not draw last pixel, if 0 draw

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
	max = end ? max - 1 : max;
	//if(max == 1) printf("max = %d dx = %d dy = %d end = %d\n", max, dx, dy, end);
	for(i=0; i < max; i++){
		img[xy(mi, ma, c)] = col;
		if(mit >= mat) { mat += max; mi += mist;}
		mit += min;
		ma += mast;
	}
	return max;
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
				//printf("dx = %d dy = %d pow = %d\n", pix[i].x - pix[i].out->x, pix[i].y - pix[i].out->y, pix[i].pow);
				xy.x1 = pix[i].x; xy.y1 = pix[i].y;
				xy.x2 = pix[i].out->x; xy.y2 = pix[i].out->y;
				draw_line(img, &xy, w, pix[i].pow, pix[i].out->nout);
				nline++;
			}
	}

	for(i=0; i < w*h; i++){
		if(pix[i].nout || pix[i].nin){
		//if(pix[i].nout){
			img[i] = pix[i].pow<<1;
			img[i + pix[i].yx] = pix[i].pow<<1;
			img[i - pix[i].yx] = pix[i].pow<<1;
			pixs++;
		}
	}

	printf("Pixels  = %d\n", pixs);
	printf("Lines   = %d\n", nline);
}

void seg_draw_edges(Pixel *pix, Edge *edge, uint32 nedge, imgtype *img, uint32 w, uint32 h, uint32 px, uint32 col)
{
	uint32 i, j, in, k, npix = 0, nline = 0, color, po;
	Vector xy;
	Pixel *p;
	//Draw lines
	//printf("seg_draw_edges nedge = %d\n", nedge);
	for(i=0; i < nedge; i++){
	//i = 22160;{
		//if(edge[i].pixs > px){
		p = &pix[edge[i].yxs];
		po = px ?  px : edge[i].lines;
		po = po > edge[i].lines ? edge[i].lines : po;
		//for(j=0; j < edge[i].lines; j++){
		//if(pix[edge[i].yxs].mach > 20)
		for(j=0; j < po; j++){
			xy.x1 = p->x; xy.y1 = p->y;
			xy.x2 = p->out->x; xy.y2 = p->out->y;
			//printf("%3d x1 = %d y1 = %d x2 = %d y2 = %d p = %p\n", j, xy.x1, xy.y1, xy.x2, xy.y2, p);
			//if(!abs(xy.x1- xy.x2) && !abs(xy.y1 - xy.y2)) printf("%3d x = %d y = %d\n", j, abs(xy.x1- xy.x2), abs(xy.y1 - xy.y2));
			color = col ? col : p->pow;
			npix += draw_line(img, &xy, w, color, 1)-1;
			nline++;
			p = p->out;
		}
		npix++;
		//}
	}
	//i = 22160;{
	for(i=0; i < nedge; i++){
		if(edge[i].pixs > px){
			in = edge[i].yxs;
			color = col ? col : pix[in].pow<<1;
			po = px ?  px : edge[i].lines;
			po = po > edge[i].lines ? edge[i].lines : po;

			for(j=0; j <= po; j++){
				img[in] = color;
				//img[in] = 250;
				//img[in + pix[in].yx] = color;
				//img[in - pix[in].yx] = color;
				if(edge[i].lines != j) in = pix[in].out->x + pix[in].out->y*w;
			}
		}
	}

	printf("Draw \n");
	printf("Numbers of pixels  = %6d\n", npix);
	printf("Numbers of lines   = %6d\n", nline);
	printf("Numbers of edges   = %6d\n", nedge);
}

void seg_draw_edges_des(Pixel *pix, Edge *edge, uint32 nedge, imgtype *img, uint32 w, uint32 h, uint32 px, uint32 col)
{
	uint32 i, j, in, k, npix = 0, nline = 0, color, po;
	Vector xy;
	Pixel *p;
	//Draw lines
	//printf("seg_draw_edges nedge = %d\n", nedge);
	for(i=0; i < nedge; i++){
	//i = 22160;{
		//if(edge[i].pixs > px){
		p = &pix[edge[i].yxs];
		po = px ?  px : edge[i].lines;
		po = po > edge[i].lines ? edge[i].lines : po;
		//for(j=0; j < edge[i].lines; j++){
		//if(pix[edge[i].yxs].mach > 25)
		for(j=0; j < po; j++){
			//if(p->nout){
			xy.x1 = p->x + p->vx; xy.y1 = p->y + p->vy;
			xy.x2 = p->out->x + p->out->vx; xy.y2 = p->out->y + p->out->vy;
			//if(abs(p->vx) > 12 ||  abs(p->vy) > 12 || abs(p->out->vx) > 12 || abs(p->out->vy) > 12)
			//	printf("vx1 = %d vy1 = %d vx2 = %d vy2 = %d\n", p->vx, p->vy, p->out->vx,  p->out->vy);
			//printf("%3d x1 = %d y1 = %d x2 = %d y2 = %d p = %p\n", j, xy.x1, xy.y1, xy.x2, xy.y2, p);
			color = col ? col : p->pow;
			npix += draw_line(img, &xy, w, color, p->out->nout)-1;
			nline++;
			p = p->out;
			//}
		}
		npix++;
		//}
	}
	/*
	for(i=0; i < nedge; i++){
		if(edge[i].pixs > px){
			in = edge[i].yxs;
			color = col ? col : pix[in].pow<<1;
			for(j=0; j <= edge[i].lines; j++){
				img[in] = color;
				//img[in] = 250;
				//img[in + pix[in].yx] = color;
				//img[in - pix[in].yx] = color;
				if(edge[i].lines != j) in = pix[in].out->x + pix[in].out->y*w;
			}
		}
	}*/

	printf("Draw \n");
	printf("Numbers of pixels  = %6d\n", npix);
	printf("Numbers of lines   = %6d\n", nline);
	printf("Numbers of edges   = %6d\n", nedge);
}

void seg_draw_pix(Pixel *pix, imgtype *img, imgtype *grad, uint32 w, uint32 h, uint32 col)
{
	uint32 i, j, k, pixs = 0, nline = 0;
	for(i=0; i < w*h; i++){
		if(grad[i] == 255) {
			//img[i] = col;
			if(pix[i].nnei > 4) {
				printf("nnei = %d\n", pix[i].nnei);
				img[i] = col;
			}
		}
	}
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

static inline uint16 block_match(imgtype *grad, imgtype *img1, imgtype *img2, uint32 x1, uint32 y1, uint32 x2, uint32 y2,  uint32 *xo, uint32 *yo, uint32 w, uint32 h, uint32 st)
{
	int x, y, yx, yx1 = x1 + y1*w,  sad, npix = 0;
	uint16 min = 0xFFFF;
	int ax = x2 - st, ay = y2 - st, bx = x2 + st, by = y2 + st;
	if(ax <= 0) ax = 1;
	if(ay <= 0) ay = 1;
	if(bx >= w) bx = w-1;
	if(by >= h) by = h-1;
	//printf("ax = %d bx = %d ay = %d by = %d x = %d y = %d\n",ax, bx, ay, by, x1, y1);
	for(y=ay; y < by; y++){
		for(x=ax; x < bx; x++){
			yx = y*w + x;
			//printf("x1 = %d y1 = %d  \n", x, y);
			if(grad[yx] > 252){
			//if(grad[yx] == 255){
				sad = diff3x3( img1, img2, yx1, yx, w);
				if(sad < min) { min = sad; *xo = x; *yo = y; }
				npix++;
			}
		}
	}
	if(!npix) { *xo = x1; *yo = y1;}
	//if(abs(*xo - x1) > 8 || abs(*yo - y1) > 8 )
	//if(min)
	//printf("npix = %3d min = %4d x = %4d y = %4d x1 = %4d y1 = %4d vx = %4d vy = %4d", npix, min, x1, y1, *xo, *yo, *xo - x1, *yo - y1);
	return min;
}

static inline uint32 diff3x5( imgtype *img1, imgtype *img2, uint32 yx1, uint32 yx2, uint32 yx3, int *sad,  uint32 w)
{
	uint32 s[5];
	int yxw1, yxw2;
	uint32 w2 = w<<1;
	//printf("abs(yx1-yx3) = %4d ", abs(yx1-yx3));
	//|*|*|*|
	//|*|x|*|
	//|*|x|*|
	//|*|x|*|
	//|*|*|*|
	if(abs(yx3) > 1) {
		yxw1 = yx1-w2; yxw2 = yx2-w2;
		//printf("yxw1 = %d yxw2 = %d\n", yxw1, yxw2);
		if(yxw1 >= 0 && yxw2 >= 0) s[0] = abs(img1[yxw1-1] - img2[yxw2-1]) + abs(img1[yxw1] - img2[yxw2]) + abs(img1[yxw1+1] - img2[yxw2+1]);
		//yxw1 = yx1-w; yxw2 = yx2-w;
		//printf("yxw1 = %d yxw2 = %d\n", yxw1, yxw2);
		//if(yxw1 >= 0 && yxw2 >= 0) s[1] = abs(img1[yxw1-1]  - img2[yxw2-1])  + abs(img1[yxw1]  - img2[yxw2])  + abs(img1[yxw1+1]  - img2[yxw2+1]);
        //s[0] = abs(img1[yx1-w2-1] - img2[yx2-w2-1]) + abs(img1[yx1-w2] - img2[yx2-w2]) + abs(img1[yx1-w2+1] - img2[yx2-w2+1]);
        s[1] = abs(img1[yx1-w-1]  - img2[yx2-w-1])  + abs(img1[yx1-w]  - img2[yx2-w])  + abs(img1[yx1-w+1]  - img2[yx2-w+1]);
		s[2] = abs(img1[yx1-1]    - img2[yx2-1])    + abs(img1[yx1]    - img2[yx2])    + abs(img1[yx1+1]    - img2[yx2+1]);
		s[3] = abs(img1[yx1+w-1]  - img2[yx2+w-1])  + abs(img1[yx1+w]  - img2[yx2+w])  + abs(img1[yx1+w+1]  - img2[yx2+w+1]);
		s[4] = abs(img1[yx1+w2-1] - img2[yx2+w2-1]) + abs(img1[yx1+w2] - img2[yx2+w2]) + abs(img1[yx1+w2+1] - img2[yx2+w2+1]);
		//printf("s0 = %d s1 = %d s2 = %d s3 = %d s4 = %d\n", s[0], s[1], s[2], s[3], s[4]);

	//|*|*|*|*|*|
	//|*|x|x|x|*|
	//|*|*|*|*|*|
	} else {
		s[0] = abs(img1[yx1-2-w] - img2[yx2-2-w]) + abs(img1[yx1-2] - img2[yx2-2]) + abs(img1[yx1-2+w] - img2[yx2-2+w]);
		s[1] = abs(img1[yx1-1-w] - img2[yx2-1-w]) + abs(img1[yx1-1] - img2[yx2-1]) + abs(img1[yx1-1+w] - img2[yx2-1+w]);
		s[2] = abs(img1[yx1-w]   - img2[yx2-w])   + abs(img1[yx1]   - img2[yx2])   + abs(img1[yx1+w]   - img2[yx2+w]);
		s[3] = abs(img1[yx1+1-w] - img2[yx2+1-w]) + abs(img1[yx1+1] - img2[yx2+1]) + abs(img1[yx1+1+w] - img2[yx2+1+w]);
		s[4] = abs(img1[yx1+2-w] - img2[yx2+2-w]) + abs(img1[yx1+2] - img2[yx2+2]) + abs(img1[yx1+2+w] - img2[yx2+2+w]);
		//printf("s0 = %d s1 = %d s2 = %d s3 = %d s4 = %d\n", s[0], s[1], s[2], s[3], s[4]);
	}
	//sad[0] = (s[0] + s[1] + s[2])>>3;
	//sad[1] = (s[1] + s[2] + s[3])>>3;
	//sad[2] = (s[2] + s[3] + s[4])>>3;
	//return (s[0] + s[1] + s[2] + s[3] + s[4])>>4;
	sad[0] = (s[0] + s[1] + s[2])/9;
	sad[1] = (s[1] + s[2] + s[3])/9;
	sad[2] = (s[2] + s[3] + s[4])/9;
	return (s[0] + s[1] + s[2] + s[3] + s[4])/15;
}

static inline uint16 block_match1(imgtype *grad, imgtype *img1, imgtype *img2, uint32 x1, uint32 y1, uint32 x2, uint32 y2, uint32 yx2, uint32 *xo, uint32 *yo, uint32 w, uint32 h, uint32 st, uint32 th)
{
	int x, y, yx, yx1 = x1 + y1*w, sad[3], diff, npix = 0;
	uint16 min[4];
	min[0] = 0xFFFF;
	int ax = x2 - st, ay = y2 - st, bx = x2 + st, by = y2 + st;
	//if(ax <= 0) ax = 1;
	//if(ay <= 0) ay = 1;
	//if(bx >= w) bx = w-1;
	//if(by >= h) by = h-1;
	if(abs(yx2) > 1){
		if(ax <= 0) ax = 1;
		if(ay <= 1) ay = 2;
		if(bx >= w) bx = w-1;
		if(by >= h-1) by = h-2;
	}
	else {
		if(ax <= 1) ax = 2;
		if(ay <= 0) ay = 1;
		if(bx >= w) bx = w-1;
		if(by >= h-1) by = h-2;
	}

	//printf("yx1-yx2 = %4d ax = %d bx = %d ay = %d by = %d x = %d y = %d\n",abs(yx1-yx2), ax, bx, ay, by, x1, y1);
	for(y=ay; y < by; y++){
		for(x=ax; x < bx; x++){
			yx = y*w + x;
			//printf("x = %d y = %d  yx = %d\n", x, y, yx);
			if(grad[yx] > th){
				//printf("grad = %d yx1 = %d yx = %d yx2 = %d\n", grad[yx], yx1, yx, yx2);
			//if(grad[yx] == 255){
				diff = diff3x5( img1, img2, yx1, yx, yx2, sad, w);
				if(diff < min[0]) { min[0] = diff; *xo = x; *yo = y; min[1]=sad[0]; min[2]=sad[1]; min[3]=sad[2]; }
				npix++;
				//printf("diff = %5d sad0 = %5d sad1 = %5d sad2 = %5d\n", diff/15, sad[0]/9, sad[1]/9, sad[2]/9);
			}
		}
	}
	//printf("%3d npix = %3d diff = %5d sad0 = %5d sad1 = %5d sad2 = %5d minimum\n", th, npix, min[0]/15, min[1]/9, min[2]/9, min[3]/9);
	if(!npix) { *xo = x1; *yo = y1;}
	return min[0];
}

static inline uint16 double_block_match(imgtype *grad, imgtype *img1, imgtype *img2, uint32 x1, uint32 y1, uint32 yx1, uint32 x2, uint32 y2, uint32 yx2, uint32 *xo, uint32 *yo, uint32 w, uint32 h, uint32 st, uint32 th)
{
	int x, y, yx, yx3, sad[3], diff[2], dif, npix = 0;
	uint32 xy1 = x1 + w*y1, xy2 = x2 + w*y2;
	uint16 min[2][4];
	uint16 minim = 0xFFFF;
	int dx = x2 - x1, dy = y2 - y1;
	int ax1 = x1 - st, ay1 = y1 - st, bx1 = x1 + st, by1 = y1 + st;
	int ax2 = x2 - st, ay2 = y2 - st, bx2 = x2 + st, by2 = y2 + st;
	int ax, ay, bx, by;
	//if(ax <= 0) ax = 1;
	//if(ay <= 0) ay = 1;
	//if(bx >= w) bx = w-1;
	//if(by >= h) by = h-1;
	if(abs(yx1) > 1){
		if(ax1 <= 0) ax1 = 1;
		if(ay1 <= 1) ay1 = 2;
		if(bx1 >= w) bx1 = w-1;
		if(by1 >= h-1) by1 = h-2;
	}
	else {
		if(ax1 <= 1) ax1 = 2;
		if(ay1 <= 0) ay1 = 1;
		if(bx1 >= w) bx1 = w-1;
		if(by1 >= h-1) by1 = h-2;
	}

	if(abs(yx2) > 1){
		if(ax2 <= 0) ax2 = 1;
		if(ay2 <= 1) ay2 = 2;
		if(bx2 >= w) bx2 = w-1;
		if(by2 >= h-1) by2 = h-2;
	}
	else {
		if(ax2 <= 1) ax2 = 2;
		if(ay2 <= 0) ay2 = 1;
		if(bx2 >= w) bx2 = w-1;
		if(by2 >= h-1) by2 = h-2;
	}
	ax = ax1 >= (ax2 + x1 - x2) ? ax1 : (ax2 + x1 - x2);
	ay = ay1 >= (ay2 + y1 - y2) ? ay1 : (ay2 + y1 - y2);
	bx = bx1 <= (bx2 + x1 - x2) ? bx1 : (bx2 + x1 - x2);
	by = by1 <= (by2 + y1 - y2) ? by1 : (by2 + y1 - y2);
	/*
	ax = (ax1 < ax2) ? ax1 : x1 - x2 + ax2;
	ay = (ay1 < ay2) ? ay1 : y1 - y2 + ay2;
	bx = (bx2 < bx1) ? bx1 : x1 - x2 + bx2;
	by = (by2 < by1) ? by1 : y1 - y2 + by2;
	*/
	//printf("ay1 = %d ay2 = %d\n",ay1,  ay2);
	//printf("ax = %d bx = %d ay = %d by = %d x1 = %d y1 = %d xy1 = %d x2 = %d y2 = %d xy2 = %d\n",ax, bx, ay, by, x1, y1, yx1, x2, y2, yx2);

	for(y=ay; y < by; y++){
		for(x=ax; x < bx; x++){
			yx = y*w + x;
			yx3 = (y+dy)*w + x + dx;
			//printf("x1 = %d y1 = %d  yx1 = %d x2 = %d y2 = %d  yx2 = %d\n", x, y, yx, x+dx, y+dy, yx3);
			if(grad[yx] > th){
				diff[0] = diff3x5( img1, img2, xy1, yx , yx1, sad, w);
				diff[1] = diff3x5( img1, img2, xy2, yx3, yx2, sad, w);
				dif = diff[0] + diff[1];
				if(dif < minim) {
					minim = dif;
					min[0][0] = diff[0]; min[0][1]=sad[0]; min[0][2]=sad[1]; min[0][3]=sad[2];
					min[1][0] = diff[1]; min[1][1]=sad[0]; min[1][2]=sad[1]; min[1][3]=sad[2];
					*xo = x; *yo = y;
				}
				npix++;
				//printf("diff = %5d sad0 = %5d sad1 = %5d sad2 = %5d\n", diff/15, sad[0]/9, sad[1]/9, sad[2]/9);
			}
		}
	}
	//printf("dif  = %5d diff = %5d sad0 = %5d sad1 = %5d sad2 = %5d\n", minim/30, min[0][0]/15, min[0][1]/9, min[0][2]/9, min[0][3]/9);
	//printf("npix = %5d diff = %5d sad0 = %5d sad1 = %5d sad2 = %5d\n", npix, min[1][0]/15, min[1][1]/9, min[1][2]/9, min[1][3]/9);
	if(!npix) { *xo = x1; *yo = y1;}
	return minim;
}

static inline void block_match_new(imgtype *grad, imgtype *img1, imgtype *img2, uchar *mmb, uint32 x1, uint32 y1, uint32 x2, uint32 y2, uint32 yx2, uint32 w, uint32 h, uint32 st, uint32 th)
{
	int x, y, xm, ym, yx, yx1 = x1 + y1*w, sad[3], diff, npix = 0, w1 = (st<<1)+1;
	uint16 min[4];
	min[0] = 0xFFFF;
	memset (mmb, 255, w1*w1);
	//For test only-----------------------------------------------------------------------
	//memset (sd[0], 255, w1*w1);
	//memset (sd[1], 255, w1*w1);
	//memset (sd[2], 255, w1*w1);
	//------------------------------------------------------------------------------------
	int ax = x2 - st, ay = y2 - st, bx = x2 + st, by = y2 + st;
	//if(ax <= 0) ax = 1;
	//if(ay <= 0) ay = 1;
	//if(bx >= w) bx = w-1;
	//if(by >= h) by = h-1;
	if(abs(yx2) > 1){
		if(ax <= 0) ax = 1;
		if(ay <= 1) ay = 2;
		if(bx >= w) bx = w-1;
		if(by >= h-1) by = h-2;
	}
	else {
		if(ax <= 1) ax = 2;
		if(ay <= 0) ay = 1;
		if(bx >= w) bx = w-1;
		if(by >= h-1) by = h-2;
	}
	//printf("yx1-yx2 = %4d ax = %d bx = %d ay = %d by = %d x = %d y = %d\n",abs(yx1-yx2), ax, bx, ay, by, x1, y1);
	for(y=ay, ym = st+ay-y2; y <= by; y++, ym++){
		for(x=ax, xm = st+ax-x2; x <= bx; x++, xm++){
			yx = y*w + x;
			//printf("x = %d y = %d  yx = %d\n", x, y, yx);
			if(grad[yx] > th){
				mmb[ym*w1 + xm] = diff3x5( img1, img2, yx1, yx, yx2, sad, w);
				//sd[0][ym*w1 + xm] = sad[0];
				//sd[1][ym*w1 + xm] = sad[1];
				//sd[2][ym*w1 + xm] = sad[2];
				npix++;
			}
		}
		//for(x=ax, xm = st+ax-x2; x <= bx; x++, xm++) printf("%3d ", ym*w1 + xm);
		//printf("  ");

	}
	//printf("\n");
	//printf("%3d npix = %3d diff = %5d sad0 = %5d sad1 = %5d sad2 = %5d minimum\n", th, npix, min[0]/15, min[1]/9, min[2]/9, min[3]/9);
	//if(!npix) { *xo = x1; *yo = y1;}
	/*
	for(y=ay, ym = st+ay-y2; y <= by; y++, ym++){
		for(x=ax, xm = st+ax-x2; x <= bx; x++, xm++) printf("%3d ", mmb[ym*w1 + xm]);
		printf("\n");
	} printf("\n");
	for(y=ay, ym = st+ay-y2; y <= by; y++, ym++){
		for(x=ax, xm = st+ax-x2; x <= bx; x++, xm++) printf("%3d ", sd[0][ym*w1 + xm]);
		printf("\n");
	} printf("\n");
	for(y=ay, ym = st+ay-y2; y <= by; y++, ym++){
		for(x=ax, xm = st+ax-x2; x <= bx; x++, xm++) printf("%3d ", sd[1][ym*w1 + xm]);
		printf("\n");
	} printf("\n");
	for(y=ay, ym = st+ay-y2; y <= by; y++, ym++){
		for(x=ax, xm = st+ax-x2; x <= bx; x++, xm++) printf("%3d ", sd[2][ym*w1 + xm]);
		printf("\n");
	} printf("\n");
	*/
	return;
}


static inline void min_add(uchar *mb1, uchar *mb2, uchar *mb3, uint32 sqm)
{
	uint32 i, sum;
	for(i=0; i < sqm; i++) {
		mb3[i] = (mb1[i] + mb2[i])>>1;
		//sum = (mb1[i] + mb2[i])>>1;
		//mb3[i] = sum > 255 ? 255 : sum;
	}
}

static inline void find_local_min(uchar *mb, uint16 *min, uint16 *xm, uint16 *ym, uint16 wm)
{
	uint32 x, y, yx;
	min[0] = 0xFFFF;
	for(y=0; y < wm; y++){
		for(x=0; x < wm; x++){
			yx = y*wm + x;
			if(mb[yx] < min[0]) {
				min[0] = mb[yx];
				xm[0] = x;
				ym[0] = y;
			}
		}
	}
}

static inline uint16 find_mv(uchar *mb1, uchar *mb2, uint16 min, uint16 xm, uint16 ym, uint16 *xo1, uint16 *yo1, uint16 *xo2, uint16 *yo2, uint16 wm)
{
	int i, x1, y1, x2, y2, yx1, yx2, xb, yb, xe, ye;
	uint16 sum, m, xt1, yt1, xt2, yt2;
	m = 0xFFFF;
	x1 = xm; y1 = ym;
	xb = (x1 - 1) < 0 ? 0 : x1-1; xe = (x1 + 1) > wm ? wm : x1+1;
	yb = (y1 - 1) < 0 ? 0 : y1-1; ye = (y1 + 1) > wm ? wm : y1+1;
		//printf("xb = %4d xe = %4d yb = %4d ye = %4d\n", xb, xe, yb, ye);
	for(y1=yb; y1 <= ye; y1++){
		for(x1=xb; x1 <= xe; x1++){
			yx1 = x1 + y1*wm;
			for(y2=yb; y2 <= ye; y2++){
				for(x2=xb; x2 <= xe; x2++){
					yx2 = x2 + y2*wm;
					sum = (mb1[yx1] + mb2[yx2])>>1;
					if(sum < m) {
						m = sum;
						//printf("x1 = %4d y1 = %4d x2 = %4d y2 = %4d\n", x1, y1, x2, y2);
						*xo1 = x1; *yo1 = y1;
						*xo2 = x2; *yo2 = y2;
					}
				}
			}
		}
	}
	return m;
}

static inline void get_small(uchar *mb1, uchar *mb2, uint16 xm, uint16 ym, uint16 wm, uint16 wm1)
{
	uint32 x, y, x1, y1, yx, yx1, xb, xe, yb, ye, mvs = wm1>>1;
	memset (mb2, 255, wm1*wm1);
	xb = (xm - mvs) < 0 ? -(xm - mvs) : 0; xe = (xm + mvs) > wm ? wm - xm : wm1;
	yb = (ym - mvs) < 0 ? -(ym - mvs) : 0; ye = (ym + mvs) > wm ? wm - ym : wm1;

	for(x=xb, x1 = xm-mvs+xb; x <= xe; x++, x1++){
		for(y=xb, y1 = ym-mvs+yb; y <= ye; y++, y1++){
			yx = x + y*wm1;
			yx1 = x1 + y1*wm;
			mb2[yx] = mb1[yx1];
		}
	}
}

void seg_compare(Pixel *pix, Edge *edge, uint32 nedge, imgtype *grad1, imgtype *grad2, imgtype *img1, imgtype *img2, uchar *mmb, uint32 w, uint32 h, uint32 mvs)
{
	uint32 i, j, yx, yx1, yx2, y, y1, y2,  x, x1, x2, w1 = w-2, h1 = h-2, npix = 0, ndge = 0, px = 3, po;
	uint32 sq = w*h, wm[2], sqm[2], nm = 1;
	int xv, yv;
	wm[0] = ((mvs<<1)+1); sqm[0] = wm[0]*wm[0];
	wm[1] = ((nm<<1)+1);  sqm[1] = wm[1]*wm[1];

	uchar *mb[2], *sum;// *sad[3];
	uint16 xm, ym, min[3], xo1, yo1, xo2, yo2;
	Pixel *p, *p1, *p2;

	uint32 min10=0, min12=0, min01=0, min02=0;

	mb[0] = mmb; mb[1] = &mmb[sqm[0]]; sum = &mmb[sqm[0]<<1];
	//sad[0] = &sum[sqm[0]]; sad[1] = &sad[0][sqm[0]]; sad[2] = &sad[1][sqm[0]];
	//for(i=0; i < 10; i++){
	for(i=0; i < nedge; i++){
		p = &pix[edge[i].yxs];
		block_match_new(grad2, img1, img2, mb[0], p->x, p->y, p->x, p->y, p->yx, w, h, mvs, 253);
		/*
		//For testing only-------------------------------------------------------------------------
		find_local_min(mb[0], &min[0], &xm, &ym, wm[0]);
		printf("match 1 min = %3d x = %4d y = %4d x1 = %4d y1 = %4d\n", min[0], xm, ym, p->x, p->y);
		find_local_min(sad[0], &min[0], &xm, &ym, wm[0]);
		printf("sad   0 min = %3d x = %4d y = %4d\n", min[0], xm, ym);
		find_local_min(sad[1], &min[0], &xm, &ym, wm[0]);
		printf("sad   1 min = %3d x = %4d y = %4d\n", min[0], xm, ym);
		find_local_min(sad[2], &min[0], &xm, &ym, wm[0]);
		printf("sad   2 min = %3d x = %4d y = %4d\n", min[0], xm, ym);
		///-----------------------------------------------------------------------------------------
		*/
		p1 = p->out;
		if(p->npix < 4 && p1->nout){
			printf("< 4 %6d npix = %d nout = %d line = %d\n", i, p->npix, p->nout, edge[i].lines);
			p2 = p1->out;
			block_match_new(grad2, img1, img2, mb[1], p2->x, p2->y, p2->x, p2->y, p2->yx, w, h, mvs, 253);
			p->out = p2; p->npix = p->npix + p1->npix;
			p1 = p1->out;
			edge[i].lines--;
			//j=3;
		} else {
			printf(">=4 %6d npix = %d nout = %d line = %d\n", i, p->npix, p->nout, edge[i].lines);
			block_match_new(grad2, img1, img2, mb[1], p1->x, p1->y, p1->x, p1->y, p1->yx, w, h, mvs, 0);
			//j=2;
		}
		if(abs(p1->x - p->x) < 3 &&  abs(p1->y - p->y) < 3) min10++;
		/*
		//For testing only---------------------------------------------------------------------------
		find_local_min(mb[1], &min[1], &xm, &ym, wm[0]);
		printf("match 2 min = %3d x = %4d y = %4d x2 = %4d y2 = %4d\n", min[1], xm, ym, p1->x, p1->y);
		find_local_min(sad[0], &min[1], &xm, &ym, wm[0]);
		printf("sad   0 min = %3d x = %4d y = %4d\n", min[0], xm, ym);
		find_local_min(sad[1], &min[1], &xm, &ym, wm[0]);
		printf("sad   1 min = %3d x = %4d y = %4d\n", min[0], xm, ym);
		find_local_min(sad[2], &min[1], &xm, &ym, wm[0]);
		printf("sad   2 min = %3d x = %4d y = %4d\n", min[0], xm, ym);
		///-----------------------------------------------------------------------------------------
		*/
		min_add(mb[0], mb[1], sum, sqm[0]);
		find_local_min(sum, &min[2], &xm, &ym, wm[0]);
		//printf("sum     min = %3d x = %4d y = %4d x2 = %4d y2 = %4d\n", min[2], xm, ym, p->x-mvs+xm, p->y-mvs+ym);

		//For testing only---------------------------------------------------------------------------
		///-----------------------------------------------------------------------------------------


		p->mach = min[2];
		p->vx  = xm - mvs; p->vy  = ym - mvs;
		p1->vx = p->vx; p1->vy = p->vy;
		//p->mach = find_mv(mb[0], mb[1], min[2], xm, ym, &xo1, &yo1, &xo2, &yo2, wm[0]);
		//p->vx  = xo1 - mvs; p->vy  = yo1 - mvs;
		//p1->vx = xo2 - mvs; p1->vy = yo2 - mvs;
		//p->vx  = xo1 - mvs; p->vy  = yo1 - mvs;
		//p2->vx = xo2 - mvs; p2->vy = yo2 - mvs;
		//p1->vx = p->vx;  p1->vy = p->vy;

		//printf("x1  = %4d y1  = %4d x2  = %4d y2  = %4d\n", xo1, yo1, xo2, yo2);
		//if(abs(p->vx - p1->vx) > 2 || abs(p->vy - p1->vy) > 2) {
		printf("  0 %3d vx = %4d vy = %4d mach = %3d x = %3d y = %3d\n", p->npix, p->vx, p->vy, p->mach, abs(p->x - p1->x)+1, abs(p->y - p1->y)+1);
		printf("  1 %3d vx = %4d vy = %4d mach = %3d \n", p1->npix, p1->vx, p1->vy, p->mach);
		//}

		//block_match_new(grad2, img1, img2, mb[1], sad, p1->x, p1->y, p1->x + p1->vx, p1->y + p1->vy, p1->yx, w, h, nm, 0);

		po = px > edge[i].lines ? edge[i].lines : px;
		//if(edge[i].lines > 1)
		//for(j=2; j <= po; j++){
		for(j=2; j <= edge[i].lines; j++){
			p = p1; p1 = p1->out;
			//printf("p1->x = %d, p1->y = %d, p1->x + p->vx = %d, p1->y + p->vy = %d\n", p1->x, p1->y, p1->x + p->vx, p1->y + p->vy);
			//block_match_new(grad2, img1, img2, mb[j&1], sad, p1->x, p1->y, p1->x + p->vx, p1->y + p->vy, p1->yx, w, h, nm, 0);
			//min_add(mb[!(j&1)], mb[j&1], sum, sqm[1]);
			//find_local_min(sum, &min[2], &xm, &ym, wm[1]);
			//p1->mach = find_mv(mb[!(j&1)], mb[j&1], min[2], xm, ym, &xo1, &yo1, &xo2, &yo2, wm[1]);
			//p1->vx = p->vx + xo2 - nm; p1->vy = p->vy + yo2 - nm;
			xv = p1->x + p->vx; yv = p1->y + p->vy;
			if(xv >= 0 && xv < w && yv >= 0 && yv < h){
				block_match_new(grad2, img1, img2, mb[0], p1->x, p1->y, p1->x + p->vx, p1->y + p->vy, p1->yx, w, h, nm, 253);
				find_local_min(mb[0], &min[2], &xm, &ym, wm[1]);
				p1->mach = (p->mach + min[2])>>1;
				p1->vx = p->vx + xm - nm; p1->vy = p->vy + ym - nm;
				//if(abs(p->vx - p1->vx) > 2 || abs(p->vy - p1->vy) > 2) {
				//printf("1 vx = %4d vy = %4d \n", p->vx, p->vy);
				printf("%3d %3d vx = %4d vy = %4d mach = %3d \n", j, p1->npix, p1->vx, p1->vy, p1->mach);
				//if(abs(p1->vx) > 12 || abs(p1->vy) > 12) printf("p->vx = %d xm = %d p->vy = %d ym = %d nm = %d \n", p->vx, xm, p->vy, ym, nm);
			} else printf("xv = %d yv = %d\n", xv, yv);
			//}
			//printf("x1 = %4d y1 = %4d x2 = %4d y2 = %4d\n", xo1, yo1, xo2, yo2);
			//printf("vx2 = %4d vy2 = %4d\n", p->vx, p->vy);
			//printf("vy1 = %4d vy1 = %4d vx2 = %4d vy2 = %4d\n", xo1 - nm, yo1 - nm, xo2 - nm, yo2 - nm);
			//printf("vx1 = %4d vy1 = %4d vx2 = %4d vy2 = %4d\n", p->vx + xo1 - nm, p1->vy + yo1 - nm, p1->vx, p1->vy);
			//get_small(mb[!(i&1)], mb[i&1], xo2, yo2, wm[0], wm[1]);
			//min_add(mb[0], mb[1], sum, sqm);
		}
		printf("\n");
	}
	printf("nedge = %d len < 4 = %d %f \n", nedge, min10, (float)min10/(float)nedge);
	//printf("nedge = %d min < 10 = %d %f min12 < min0 = %d %f min01 = %d %f\n",
	//		nedge, min10, (float)min10/(float)nedge, min12, (float)min12/(float)nedge, min01, (float)min01/(float)nedge);

	/*
	for(y=2; y < h1; y++){
		for(x=2; x < w1; x++){
			yx = y*w + x;
			if(grad1[yx] == 255 && pix[yx].nout){
				x1 = pix[yx].x; y1 = pix[yx].y; yx1 = pix[yx].yx;
				x2 = pix[yx].out->x; y2 = pix[yx].out->y; yx2 = pix[yx].out->yx;

				//printf("yx = %d\n", yx);
				pix[yx].mach = double_block_match(grad2, img1, img2, x1, y1, yx1, x2, y2, yx2, &xo, &yo, w, h, mvs, 253);
				//pix[yx].mach = block_match1(grad2, img1, img2, x, y, x, y, pix[edge[i].yxe].yx, &xo, &yo, w, h, 12, 0);
				pix[yx].vx = xo - x;
				pix[yx].vy = yo - y;
				grad1[yx] = 253;
				npix++;
			}
		}
	}*/
}

void seg_draw_vec(Pixel *pix, uint32 npix, imgtype *img, uint32 w, uint32 h)
{
	uint32 i, j, k, sq = w*h, pixs = 0, nline = 0;
	Vector xy;
	for(i=0; i < sq; i++){
		if(pix[i].nout && pix[i].vx && pix[i].vy) {
			xy.x1 = pix[i].x; xy.y1 = pix[i].y;
			xy.x2 = pix[i].x + pix[i].vx; xy.y2 = pix[i].y + pix[i].vy;
			draw_line(img, &xy, w, 200, 0);
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
			if(pix[yx].nout){
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

void seg_quant(imgtype *img1, imgtype *img2, uint32 w, uint32 h, uint32 q)
{
	uint32 x, y, yx, sq = w*h;
	for(y=0; y < sq; y+=w){
		for(x=0; x < w; x++){
			yx = y + x;
			img2[yx] = (img1[yx]>>q)<<q;
		}
	}
}
