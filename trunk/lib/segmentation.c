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
			img1[yx] = max>>th ? (max >253 ? 253 : max) : 0; img2[yx] = in;
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
	/*
	if(in1 == 0   ){
		if(img[yx-1  ] > max) { max = img[yx-1  ]; in = -1  ; }
		if(img[yx  -w] > max) { max = img[yx  -w]; in =   -w; }
		if(img[yx+1  ] > max) { max = img[yx+1  ]; in = +1  ; }
		if(img[yx  +w] > max) { max = img[yx  +w]; in =   +w; }
		goto end;
	}
	if(in1 == -1  ){
		if(img[yx  -w] > max) { max = img[yx  -w]; in =   -w; }
		if(img[yx+1  ] >= max) { max = img[yx+1  ]; in = +1  ; }
		if(img[yx  +w] > max) { max = img[yx  +w]; in =   +w; }
		goto end;
	}
	if(in1 ==   -w){
		if(img[yx+1  ] > max) { max = img[yx+1  ]; in = +1  ; }
		if(img[yx  +w] >= max) { max = img[yx  +w]; in =   +w; }
		if(img[yx  -1] > max) { max = img[yx  -1]; in =   -1; }
		goto end;
	}
	if(in1 ==  1  ){
		if(img[yx  +w] > max) { max = img[yx  +w]; in =   +w; }
		if(img[yx  -1] >= max) { max = img[yx  -1]; in =   -1; }
		if(img[yx  -w] > max) { max = img[yx  -w]; in =   -w; }
		goto end;
	}
	if(in1 ==    w){
		if(img[yx  -1] > max) { max = img[yx  -1]; in =   -1; }
		if(img[yx  -w] >= max) { max = img[yx  -w]; in =   -w; }
		if(img[yx+1  ] > max) { max = img[yx+1  ]; in = +1  ; }
		goto end;
	}*/

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
        /*
        if(dx == 0 && dy == 0  ){
                if(img[yx-1  ] > max) { max = img[yx-1  ]; *dx1 = -1; *dy1 =  0;}
                if(img[yx  -w] > max) { max = img[yx  -w]; *dx1 =  0; *dy1 = -1;}
                if(img[yx+1  ] > max) { max = img[yx+1  ]; *dx1 =  1; *dy1 =  0;}
                if(img[yx  +w] > max) { max = img[yx  +w]; *dx1 =  0; *dy1 =  1;}
                goto end;
        }
        if(dx == -1 && dy == 0){
                if(img[yx  -w] > max) { max = img[yx  -w]; *dx1 =  0; *dy1 = -1;}
                if(img[yx+1  ] >= max){ max = img[yx+1  ]; *dx1 =  1; *dy1 =  0;}
                if(img[yx  +w] > max) { max = img[yx  +w]; *dx1 =  0; *dy1 =  1;}
                goto end;
        }
        if(dx ==  0 && dy == -1){
                if(img[yx+1  ] > max) { max = img[yx+1  ]; *dx1 =  1; *dy1 =  0;}
                if(img[yx  +w] >= max){ max = img[yx  +w]; *dx1 =  0; *dy1 =  1;}
                if(img[yx  -1] > max) { max = img[yx  -1]; *dx1 = -1; *dy1 =  0;}
                goto end;
        }
        if(dx ==  1 && dy ==  0){
                if(img[yx  +w] > max) { max = img[yx  +w]; *dx1 =  0; *dy1 =  1;}
                if(img[yx  -1] >= max){ max = img[yx  -1]; *dx1 = -1; *dy1 =  0;}
                if(img[yx  -w] > max) { max = img[yx  -w]; *dx1 =  0; *dy1 = -1;}
                goto end;
        }
       if(dx ==  0 && dy ==  1){
                if(img[yx  -1] > max) { max = img[yx  -1]; *dx1 = -1; *dy1 =  0;}
                if(img[yx  -w] >= max){ max = img[yx  -w]; *dx1 =  0; *dy1 = -1;}
                if(img[yx+1  ] > max) { max = img[yx+1  ]; *dx1 =  1; *dy1 =  0;}
                goto end;
        }*/

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
	if(!dx && !dx1) return 1;
	if(!dy && !dy1) return 1;
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

static inline uint32 find_line(Pixel *pix, imgtype *img, uint32 x, uint32 y, uint32 yx, uint32 *xo, uint32 *yo, uint32 *yxo, uint32 w, int dx, int dy, uint32 dir)
{
	uint32 len = 0, x1 = x, y1 = y, yx1 = yx, min, c = 0;
	min = img[yx];
    while(1){
     	len += length(dx, dy);
     	yx = yx + dy*w + dx; x = x + dx; y = y + dy;
     	min = img[yx] < min ? img[yx] : min;
		if(!img[yx]) {
			if(c > 0){
				if(!pix[yx1].x && !pix[yx1].y)  { new_pix(&pix[yx1], img[yx1], x1, y1); img[yx1] = 255;}
				new_pix(&pix[yx], img[yx], x, y); img[yx] = 255;
				new_line(&pix[yx1], &pix[yx], min);
				//if(dir) new_line(&pix[yx1], &pix[yx], min);
				//else	new_line(&pix[yx], &pix[yx1], min);
				*xo = x; *yo = y; *yxo = yx;
			}
			img[yx] = 254;
			return 0;
		}
		if(img[yx] == 255 || img[yx] == 254) {
			if(yx != yx1 && c > 0){
				if(!pix[yx].x && !pix[yx].y) { new_pix(&pix[yx], img[yx], x, y); img[yx] = 255; }
				if(!pix[yx1].x && !pix[yx1].y) { new_pix(&pix[yx1], img[yx1], x1, y1); img[yx1] = 255; }
				new_line(&pix[yx1], &pix[yx], min);
				//if(dir) new_line(&pix[yx1], &pix[yx], min);
				//else	new_line(&pix[yx], &pix[yx1], min);
				*xo = x; *yo = y; *yxo = yx;
			}
			img[yx] = 254;
			return 0;
		}
		if(is_new_line(x1, y1, x, y, len, 10) && c > 1){
			new_pix(&pix[yx], img[yx], x, y); img[yx] = 255;
			if(!pix[yx1].x && !pix[yx1].y) { new_pix(&pix[yx1], img[yx1], x1, y1); img[yx1] = 255;}
			new_line(&pix[yx1], &pix[yx], min);
			//if(dir) new_line(&pix[yx1], &pix[yx], min);
			//else	new_line(&pix[yx], &pix[yx1], min);
			*xo = x; *yo = y; *yxo = yx;
			//img[yx] = 255;
			return 1;
		}
		img[yx] = 254;
		dir1(img, w, yx, -dx, -dy, &dx, &dy);
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
	uint32 y, y1, y2, y3, y11, y22, yp, x, x1, x2, x3, x11, x22, xp, yx, yx1, yx2, yx3, yx11, yx22, i, w1 = w-3, h1 = h-3,  nline = 0, min, npix = 0, pc;
	int d = 0, d1, dif, dif1, dif2, dx, dy, dx1, dy1, dx2, dy2, min1, min2, len, len1, len2, st, s;
	for(y=3; y < h1; y++){
	//for(y=1; y < 2; y++){
		for(x=3; x < w1; x++){
		//for(x=1; x < 4; x++){
			yx = y*w + x;
			if(img[yx] && img[yx] != 255){
				if(loc_max(img, yx, w)){
					yx1 = yx; x1 = x; y1 = y;
					yx2 = yx; x2 = x; y2 = y;
					//new_pix(&pix[yx], img[yx], x, y); npix++;
					dir1(img, w,  yx,   0,   0, &dx1, &dy1);
					//dir1(img, w,  yx, dx1, dy1, &dx2, &dy2);
					while (find_line(pix, img, x1, y1, yx1, &x1, &y1, &yx1, w, dx1, dy1, 1)) { npix++; nline++; }
					//while (find_line(pix, img, x2, y2, yx2, &x1, &y1, &yx1, w, dx2, dy2, 0)) { npix++; nline++; }
				}
			}
		}
	}
	printf("Numbers of pixels  = %d\n", npix);
	printf("Numbers of lines   = %d\n", nline);
}

static inline void draw_line(imgtype *img, uint32 x1, uint32 y1, uint32 x2, uint32 y2, uint32 w, uchar col)
{
	int dx = x2 - x1, dy = y2 - y1, xs, ys;
	uint32 i, j, st, stx, sty, dxa = abs(dx)+1, dya = abs(dy)+1;
	stx = dx < 0 ? -1 : (dx > 0 ? 1 : 0);
	sty = dy < 0 ? -1 : (dy > 0 ? 1 : 0);

	if(dxa >= dya){
		st = dxa;
		for(i=0,j=0; i < dxa; i++, j+=dya){
			if(j >= st){
				st += dxa;
				y1 += sty;
			}
			img[y1*w + x1] = col;
			x1 += stx;
		}
	} else {
		st = dya;
		for(i=0,j=0; i < dya; i++, j+=dxa){
			if(j >= st){
				st += dya;
				x1 += stx;
			}
			img[y1*w + x1] = col;
			y1 += sty;
		}
	}
}

void seg_draw_lines(Pixel *pix, uint32 npix, imgtype *img, uint32 w, uint32 h)
{
	uint32 i, j, k, pixs = 0, nline = 0;
	for(i=0; i < w*h; i++){
		//if(pix[i].nnei > 4){
			//if(pix[i].nout) {
			//if(pix[i].pow == 255) {
			if(pix[i].nout) {
				if(pix[i].nout > 1) printf("nout = %d\n",pix[i].nout);
				//printf("x = %d y = %d x1 = %d y1 = %d pow = %d\n", pix[i].x, pix[i].y, pix[i].out->x, pix[i].out->y, pix[i].pow);
				draw_line(img, pix[i].x, pix[i].y, pix[i].out->x, pix[i].out->y, w, pix[i].pow);
				//if(pix[i].pow == 255) printf("pow = %d\n", pix[i].pow);
				//draw_line(img, pix[i].x, pix[i].y, pix[i].out->x, pix[i].out->y, w, 255);
				nline++;
				//for(k=0; k < pix[i].nnei; k++) printf("pix[%d].pix[%d] x = %d y = %d\n", i, k, pix[i].pix[k]->x, pix[i].pix[k]->y);
				/*
				for(j=0; j < pix[i].nout; j++) {
					//printf("%6d x = %4d y = %4d nout = %d nin = %d x = %d y = %d\n", i, pix[i].x, pix[i].y, pix[i].nout, pix[i].nin, pix[i].out[j]->x, pix[i].out[j]->y);
					draw_line(img, pix[i].x, pix[i].y, pix[i].out[j]->x, pix[i].out[j]->y, w, pix[i].pow[j]);
					//draw_line(img, pix[i].x, pix[i].y, pix[i].out[j]->x, pix[i].out[j]->y, w, 100);
					nline++;
				}*/
				//printf("%6d x = %4d y = %4d nnei = %d pow = %d\n", i, pix[i].x, pix[i].y, pix[i].nnei, pix[i].pow);
			}
	}
	for(i=0; i < w*h; i++){
		if(pix[i].nout || pix[i].nin){
			img[i] = 255;
			pixs++;
		}
	}

	printf("Numbers of pixels  = %d\n", pixs);
	printf("Numbers of lines   = %d\n", nline);
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


static inline uint16 block_match( imgtype *grad, imgtype *img1, imgtype *img2, uint32 x1, uint32 y1,  uint32 *xo, uint32 *yo, uint32 w, uint32 h, uint32 st)
{
	int x, y, yx, yx1 = y1*w + x1,  sad, npix = 0;
	uint16 min = 0xFFFF;
	int ax = x1 - st, ay = y1 - st, bx = x1 + st, by = y1 + st;
	//int ax = (x1 - st) < 0 ? 0 : x1 - st;
	//int ay = (y1 - st) < 0 ? 0 : y1 - st;
	//int bx = (x1 + st) > w ? w : x1 + st;
	//int by = (y1 + st) > h ? h : y1 + st;
	if(ax <= 0) ax = 1;
	if(ay <= 0) ay = 1;
	if(bx >= w) bx = w-1;
	if(by >= h) by = h-1;
	//printf("ax = %d bx = %d ay = %d by = %d x = %d y = %d\n",ax, bx, ay, by, x1, y1);
	for(y=ay; y < by; y++){
		for(x=ax; x < bx; x++){
			yx = y*w + x;
			//printf("x1 = %d y1 = %d  \n", x, y);
			//if(grad[yx] > 253){
			if(grad[yx] == 255){
				sad =  (abs(img1[yx1    ] - img2[yx    ]) +
						abs(img1[yx1-1  ] - img2[yx-1  ]) +
						abs(img1[yx1-1-w] - img2[yx-1-w]) +
						abs(img1[yx1  -w] - img2[yx  -w]) +
						abs(img1[yx1+1-w] - img2[yx+1-w]) +
						abs(img1[yx1+1  ] - img2[yx+1  ]) +
						abs(img1[yx1+1+w] - img2[yx+1+w]) +
						abs(img1[yx1  +w] - img2[yx  +w]) +
						abs(img1[yx1-1+w] - img2[yx-1+w]));
				if(sad < min) {min = sad; *xo = x; *yo = y; }
				npix++;
			}
		}
	}
	if(!npix) { *xo = x1; *yo = y1;}
	//if(abs(*xo - x1) > 8 || abs(*yo - y1) > 8 )
	//printf("npix = %d min = %d x = %d y = %d x1 = %d y1 = %d vx = %d vy = %d\n", npix, min, x1, y1, *xo, *yo, *xo - x1, *yo - y1);
	return min;
}

void seg_compare(Pixel *pix, Pixel *pix1, imgtype *grad1, imgtype *grad2, imgtype *img1, imgtype *img2, uint32 w, uint32 h)
{
	uint32 yx, y, y1,  x, x1, w1 = w-1, h1 = h-1, xo, yo, npix = 0;
	for(y=1; y < h1; y++){
		for(x=1; x < w1; x++){
			yx = y*w + x;
			if(grad1[yx] > 253){
				pix[yx].mach = block_match(grad2, img1, img2, x, y,  &xo, &yo, w, h, 8);
				pix[yx].vx = xo - x;
				pix[yx].vy = yo - y;
				npix++;
			}
		}
	}
	printf("seg_compare npix = %d\n", npix);
}

void seg_draw_vec(Pixel *pix, uint32 npix, imgtype *img, uint32 w, uint32 h)
{
	uint32 i, j, k, sq = w*h, pixs = 0, nline = 0;
	for(i=0; i < sq; i++){
		if(pix[i].nout || pix[i].nin) {
			draw_line(img, pix[i].x, pix[i].y, pix[i].x + pix[i].vx, pix[i].y + pix[i].vy, w, 100);
			//printf("x = %d %d y = %d %d\n", pix[i].x, pix[i].vx, pix[i].y,  pix[i].vy);
			nline++;
		}
	}

	for(i=0; i < w*h; i++){
		if(pix[i].nout || pix[i].nin){
			img[i] = 255;
			pixs++;
		}
	}

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
	img2[yx2    ] = img1[yx1   ];
	img2[yx2-1  ] = img1[yx1-1 ];
	img2[yx2-1-w] = img1[yx1-1-w];
	img2[yx2  -w] = img1[yx1  -w];
	img2[yx2+1-w] = img1[yx1+1-w];
	img2[yx2+1  ] = img1[yx1+1  ];
	img2[yx2+1+w] = img1[yx1+1+w];
	img2[yx2  +w] = img1[yx1  +w];
	img2[yx2-1+w] = img1[yx1-1+w];
}

void seg_mvector_copy(Pixel *pix, imgtype *grad1, imgtype *img1, imgtype *img2, uint32 w, uint32 h)
{
	uint32 yx, y, y1, x, x1, w1 = w-1, h1 = h-1, npix = 0;
	for(y=1; y < h1; y++){
		for(x=1; x < w1; x++){
			yx = y*w + x;
			if(grad1[yx] == 255){
				copy_block(img1, yx, img2, (pix[yx].x + pix[yx].vx) + (pix[yx].y + pix[yx].vy)*w, w);
			}
		}
	}
	printf("seg_intersect_pix npix = %d\n", npix);
}
