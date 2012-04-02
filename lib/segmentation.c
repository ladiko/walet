#include <walet.h>
#include <stdio.h>
#include <math.h>

void print_around(uint8 *con, uint32 yx, uint32 w)
{
	printf("%3d ",con[yx-1-w]);
	printf("%3d ",con[yx  -w]);
	printf("%3d\n",con[yx+1-w]);
	printf("%3d ",con[yx-1  ]);
	printf("%3d ",con[yx    ]);
	printf("%3d\n",con[yx+1  ]);
	printf("%3d ",con[yx-1+w]);
	printf("%3d ",con[yx  +w]);
	printf("%3d\n\n",con[yx+1+w]);
}

void print_around32(uint32 *con, uint32 yx, uint32 w)
{
	printf("%4d ",con[yx-1-w]);
	printf("%4d ",con[yx  -w]);
	printf("%4d\n",con[yx+1-w]);
	printf("%4d ",con[yx-1  ]);
	printf("%4d ",con[yx    ]);
	printf("%4d\n",con[yx+1  ]);
	printf("%4d ",con[yx-1+w]);
	printf("%4d ",con[yx  +w]);
	printf("%4d\n\n",con[yx+1+w]);
}


void seg_grad(uint8 *img, uint8 *img1, uint8 *con, uint8 *di, uint32 w, uint32 h, int th)
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
	uint32 y, x, yx, yw, w1 = w-1, h1 = h-1, w2 = w-2, h2 = h-2;
	uint8 in, col = 253;
	uint32 g[4];
	int max;

	//Make the up border
	for(x=1; x < w1; x++) img1[w + x] = col;
	con[w+1] = 255; for(x=2; x < w2; x++) con[w + x] = 64; con[w + x] = 255;
	for(x=1; x < w2; x++) di[w + x] = 8; di[w + x] = 2;

	for(y=2; y < h2; y++){
		yw = y*w;
		img1[yw + 1] = col; con[yw + 1] = 64;  di[yw + 1] = 32;
		for(x=2; x < w2; x++){
			yx = yw + x;
			g[0] = abs(img[yx-1  ] - img[yx+1  ]);
			g[1] = abs(img[yx-1-w] - img[yx+1+w]);
			g[2] = abs(img[yx-w  ] - img[yx+w  ]);
			g[3] = abs(img[yx+1-w] - img[yx-1+w]);

			max = (g[0] + g[1] + g[2] + g[3])>>1;
			img1[yx] = (max-th) > 0 ? (max > 252 ? 252 : max) : 0;
			//printf("img = %d max = %d th = %d max-th = %d\n", img1[yx], max, th, max-th);
			//img1[yx] = max>>th;
			//max = (((g[0] + g[1] + g[2] + g[3])>>2)>>th)<<th;
			//img1[yx] = max > 252 ? 252 : max;
		}
		img1[yx + 1] = col; con[yx + 1] = 64; di[yx + 1] = 2;
		 //img1[yx + 2] = col1;
	}
	//Make the bottom border
	yw = y*w;
	for(x=1; x < w1; x++) img1[yw + x] = col;
	con[yw+1] = 255; for(x=2; x < w2; x++) con[yw + x] = 64; con[yw + x] = 255;
	di[yw+1] = 32; for(x=2; x < w1; x++) di[yw + x] = 128;
}

void seg_grad_RGB(uint8 *R, uint8 *G, uint8 *B, uint8 *grad, uint32 w, uint32 h, uint32 th)
{
	uint32 y, x, yx, yw, w1 = w-2, h1 = h-2;
	uint8 in, col = 253;
	uint32 max;
	for(y=2; y < h1; y++){
		yw = y*w;
		for(x=2; x < w1; x++){
			yx = yw + x;
			max = 	abs(R[yx-1  ] - R[yx+1  ]);
			max += 	abs(R[yx-w  ] - R[yx+w  ]);
			max += 	abs(G[yx-1  ] - G[yx+1  ]);
			max += 	abs(G[yx-w  ] - G[yx+w  ]);
			max += 	abs(B[yx-1  ] - B[yx+1  ]);
			max += 	abs(B[yx-w  ] - B[yx+w  ]);

			max = max>>3;
			grad[yx] = (max>>th) ? (max > 252 ? 252 : max) : 0;
			//img1[yx] = max>>th;
			//max = (((g[0] + g[1] + g[2] + g[3])>>2)>>th)<<th;
			//img1[yx] = max > 252 ? 252 : max;
		}
		grad[yw + 1] = col; grad[yw + w1] = col;
	}
	//The border
	for(x=1; x < w1; x++) grad[w + x] = col; grad[w + x] = col;
	yw = y*w;
	for(x=1; x < w1; x++) grad[yw + x] = col; grad[yw + x] = col;
}

void seg_grad_buf(uint8 *img, uint8 *img1, uint8 *buff, uint32 w, uint32 h, uint32 th)
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
	uint32 y, x, yx, yw, yw1, h1 = h-1, w1 = w+1;
	uint8 max;
	//uint32 g[4];
	uint8 *l0 = buff, *l1 = &buff[w+2], *l2 = &buff[(w+2)<<1], *tm;

	l0[0] = img[0]; for(x=0; x < w; x++) l0[x+1] = img[x]; l0[x+1] = img[x-1];
	l1[0] = img[0]; for(x=0; x < w; x++) l1[x+1] = img[x]; l1[x+1] = img[x-1];
	for(y=0; y < h; y++){
		yw = y*w;
		yw1 = y < h1 ? yw + w : yw;
		l2[0] = img[yw1]; for(x=0; x < w; x++) l2[x+1] = img[x+yw1]; l2[x+1] = img[x-1+yw1];

		for(x=1; x < w1; x++){
			yx = yw + x;
			//g[0] = abs(l1[x-1] - l1[x+1]);
			//g[1] = abs(l0[x-1] - l2[x+1]);
			//g[2] = abs(l0[x  ] - l2[x  ]);
			//g[3] = abs(l0[x+1] - l2[x-1]);

			max = (	abs(l1[x-1] - l1[x+1]) + abs(l0[x-1] - l2[x+1]) +
					abs(l0[x  ] - l2[x  ]) + abs(l0[x+1] - l2[x-1]))>>2;
			img1[yx-1] = (max>>th) ? (max > 252 ? 252 : max) : 0;

			//max = (((g[0] + g[1] + g[2] + g[3])>>2)>>th)<<th;
			//img1[yx] = max > 252 ? 252 : max;
		}
		tm = l0; l0 = l1; l1 = l2; l2 = tm;
	}
}


void seg_grad_sub(uint8 *img, uint8 *img1, uint32 w, uint32 h, uint32 th)
{
	/// |x| |   | |x|
	/// | |x|   |x| |
	uint32 y, x, yx, sq = w*h-w, w1 = w-1, h1 = h-1;
	uint8 max, in;
	uint32 g[4];
	for(y=0; y < sq; y+=w){
		for(x=0; x < w1; x++){
			yx = y + x;
			max = (abs(img[yx] - img[yx+1+w]) + abs(img[yx+w] - img[yx+1]))>>1;
			img1[yx] = (max>>th) ? (max > 252 ? 252 : max) : 0;

			//max = (((g[0] + g[1] + g[2] + g[3])>>2)>>th)<<th;
			//img1[yx] = max > 252 ? 252 : max;
		}
	}
}

void seg_grad_max(uint8 *img, uint8 *img1, uint32 w, uint32 h)
{
	/// |x| |   | |x|
	/// | |x|   |x| |
	uint32 y, x, yx, sq = w*h-w, w1 = w-1, h1 = h-1;
	uint8 max, in;
	uint32 g[4];
	for(y=w; y < sq; y+=w){
		for(x=1; x < w1; x++){
			yx = y + x;
			if(img[yx-1] < img[yx]){
				if(img[yx] > img[yx+1]) img1[yx] = 255;
				if(img[yx] == img[yx+1]) img1[yx] = 255; //img[yx+1]++;
			}
			if(img[yx-w] < img[yx]){
				if(img[yx] > img[yx+w]) img1[yx] = 255;
				if(img[yx] == img[yx+w]) img1[yx] = 255; //img[yx+w]++;
			}
			/*
			if(img[yx-w-1] < img[yx]){
				if(img[yx] > img[yx+w+1]) img1[yx] = 255;
				//if(img[yx] == img[yx+w]) img[yx+w]++;
			}
			if(img[yx-w+1] < img[yx]){
				if(img[yx] > img[yx+w-1]) img1[yx] = 255;
				//if(img[yx] == img[yx+w]) img[yx+w]++;
			}*/
		}
	}
}


/*	\brief	Find the maximum around yx pixel.
	\param	img		The pointer to gradient image.
	\param	w		The image width.
	\param	yx		The pixel coordinate (yx = y*w + x)
	\param  dx		The pointer to x coordinate previous and new maximum.
	\param  dy		The pointer to y coordinate previous and new maximum.
*/
static inline void direction(uint8 *img, uint32 w, uint32 yx, int *dx, int *dy)
{
	uint32 max = 0;
	int dx1 = *dx, dy1 = *dy;
	*dx = 0; *dy = 0;
	//printf("direction dx = %d dy = %d\n", *dx, *dy);
	if(dx1 == 0 && dy1 == 0  ){
		if(img[yx-1  ] > max) { max = img[yx-1  ]; *dx = -1; *dy =  0;}
		if(img[yx-1-w] > max) { max = img[yx-1-w]; *dx = -1; *dy = -1;}
		if(img[yx  -w] > max) { max = img[yx  -w]; *dx =  0; *dy = -1;}
		if(img[yx+1-w] > max) { max = img[yx+1-w]; *dx =  1; *dy = -1;}
		if(img[yx+1  ] > max) { max = img[yx+1  ]; *dx =  1; *dy =  0;}
		if(img[yx+1+w] > max) { max = img[yx+1+w]; *dx =  1; *dy =  1;}
		if(img[yx  +w] > max) { max = img[yx  +w]; *dx =  0; *dy =  1;}
		if(img[yx-1+w] > max) { max = img[yx-1+w]; *dx = -1; *dy =  1;}
	}
	else if(dx1 == -1 && dy1 == 0){
		//if(img[yx  -w] > max) { max = img[yx  -w]; *dx1 =  0; *dy1 = -1;}
		if(img[yx+1  ] > max) { max = img[yx+1  ]; *dx =  1; *dy =  0;}
		if(img[yx+1-w] > max) { max = img[yx+1-w]; *dx =  1; *dy = -1;}
		if(img[yx+1+w] > max) { max = img[yx+1+w]; *dx =  1; *dy =  1;}
		//if(img[yx  +w] > max) { max = img[yx  +w]; *dx1 =  0; *dy1 =  1;}
	}
	else if(dx1 == -1 && dy1 == -1){
		//if(img[yx+1-w] > max) { max = img[yx+1-w]; *dx1 =  1; *dy1 = -1;}
		if(img[yx+1+w] > max) { max = img[yx+1+w]; *dx =  1; *dy =  1;}
		if(img[yx+1  ] > max) { max = img[yx+1  ]; *dx =  1; *dy =  0;}
		if(img[yx  +w] > max) { max = img[yx  +w]; *dx =  0; *dy =  1;}
		//if(img[yx-1+w] > max) { max = img[yx-1+w]; *dx1 = -1; *dy1 =  1;}
	}
	else if(dx1 ==  0 && dy1 == -1){
		//if(img[yx+1  ] > max) { max = img[yx+1  ]; *dx1 =  1; *dy1 =  0;}
		if(img[yx  +w] > max) { max = img[yx  +w]; *dx =  0; *dy =  1;}
		if(img[yx+1+w] > max) { max = img[yx+1+w]; *dx =  1; *dy =  1;}
		if(img[yx-1+w] > max) { max = img[yx-1+w]; *dx = -1; *dy =  1;}
		//if(img[yx  -1] > max) { max = img[yx  -1]; *dx1 = -1; *dy1 =  0;}
	}
	else if(dx1 ==  1 && dy1 == -1){
		//if(img[yx+1+w] > max) { max = img[yx+1+w]; *dx1 =  1; *dy1 =  1;}
		if(img[yx-1+w] > max) { max = img[yx-1+w]; *dx = -1; *dy =  1;}
		if(img[yx  +w] > max) { max = img[yx  +w]; *dx =  0; *dy =  1;}
		if(img[yx  -1] > max) { max = img[yx  -1]; *dx = -1; *dy =  0;}
		//if(img[yx-1-w] > max) { max = img[yx-1-w]; *dx1 = -1; *dy1 = -1;}
	}
	else if(dx1 ==  1 && dy1 ==  0){
		//if(img[yx  +w] > max) { max = img[yx  +w]; *dx1 =  0; *dy1 =  1;}
		if(img[yx  -1] > max) { max = img[yx  -1]; *dx = -1; *dy =  0;}
		if(img[yx-1+w] > max) { max = img[yx-1+w]; *dx = -1; *dy =  1;}
		if(img[yx-1-w] > max) { max = img[yx-1-w]; *dx = -1; *dy = -1;}
		//if(img[yx  -w] > max) { max = img[yx  -w]; *dx1 =  0; *dy1 = -1;}
	}
	else if(dx1 ==  1 && dy1 ==  1){
		//if(img[yx-1+w] > max) { max = img[yx-1+w]; *dx1 = -1; *dy1 =  1;}
		if(img[yx-1-w] > max) { max = img[yx-1-w]; *dx = -1; *dy = -1;}
		if(img[yx  -1] > max) { max = img[yx  -1]; *dx = -1; *dy =  0;}
		if(img[yx  -w] > max) { max = img[yx  -w]; *dx =  0; *dy = -1;}
		//if(img[yx+1-w] > max) { max = img[yx+1-w]; *dx1 =  1; *dy1 = -1;}
	}
	else if(dx1 ==  0 && dy1 ==  1){
		//if(img[yx  -1] > max) { max = img[yx  -1]; *dx1 = -1; *dy1 =  0;}
		if(img[yx  -w] > max) { max = img[yx  -w]; *dx =  0; *dy = -1;}
		if(img[yx-1-w] > max) { max = img[yx-1-w]; *dx = -1; *dy = -1;}
		if(img[yx+1-w] > max) { max = img[yx+1-w]; *dx =  1; *dy = -1;}
		//if(img[yx+1  ] > max) { max = img[yx+1  ]; *dx1 =  1; *dy1 =  0;}
	}
	else if(dx1 == -1 && dy1 ==  1){
		//if(img[yx-1-w] > max) { max = img[yx-1-w]; *dx1 = -1; *dy1 = -1;}
		if(img[yx+1-w] > max) { max = img[yx+1-w]; *dx =  1; *dy = -1;}
		if(img[yx  -w] > max) { max = img[yx  -w]; *dx =  0; *dy = -1;}
		if(img[yx+1  ] > max) { max = img[yx+1  ]; *dx =  1; *dy =  0;}
		//if(img[yx+1+w] > max) { max = img[yx+1+w]; *dx1 =  1; *dy1 =  1;}
	}
	//printf("direction dx = %d dy = %d\n", *dx, *dy);
}

static inline void direction2(uint8 *img, uint32 w, uint32 yx, int *dx, int *dy)
{
	uint32 max = 0;
	int dx1 = *dx, dy1 = *dy;
	*dx = 0; *dy = 0;
	//printf("direction dx = %d dy = %d\n", *dx, *dy);
	if(dx1 == -1 && dy1 == 0){
		if(img[yx  -w] > max) { max = img[yx  -w]; *dx =  0; *dy = -1;}
		if(img[yx  +w] > max) { max = img[yx  +w]; *dx =  0; *dy =  1;}
	}
	else if(dx1 == -1 && dy1 == -1){
		if(img[yx+1-w] > max) { max = img[yx+1-w]; *dx =  1; *dy = -1;}
		if(img[yx-1+w] > max) { max = img[yx-1+w]; *dx = -1; *dy =  1;}
	}
	else if(dx1 ==  0 && dy1 == -1){
		if(img[yx+1  ] > max) { max = img[yx+1  ]; *dx =  1; *dy =  0;}
		if(img[yx  -1] > max) { max = img[yx  -1]; *dx = -1; *dy =  0;}
	}
	else if(dx1 ==  1 && dy1 == -1){
		if(img[yx+1+w] > max) { max = img[yx+1+w]; *dx =  1; *dy =  1;}
		if(img[yx-1-w] > max) { max = img[yx-1-w]; *dx = -1; *dy = -1;}
	}
	else if(dx1 ==  1 && dy1 ==  0){
		if(img[yx  +w] > max) { max = img[yx  +w]; *dx =  0; *dy =  1;}
		if(img[yx  -w] > max) { max = img[yx  -w]; *dx =  0; *dy = -1;}
	}
	else if(dx1 ==  1 && dy1 ==  1){
		if(img[yx-1+w] > max) { max = img[yx-1+w]; *dx = -1; *dy =  1;}
		if(img[yx+1-w] > max) { max = img[yx+1-w]; *dx =  1; *dy = -1;}
	}
	else if(dx1 ==  0 && dy1 ==  1){
		if(img[yx  -1] > max) { max = img[yx  -1]; *dx = -1; *dy =  0;}
		if(img[yx+1  ] > max) { max = img[yx+1  ]; *dx =  1; *dy =  0;}
	}
	else if(dx1 == -1 && dy1 ==  1){
		if(img[yx-1-w] > max) { max = img[yx-1-w]; *dx = -1; *dy = -1;}
		if(img[yx+1+w] > max) { max = img[yx+1+w]; *dx =  1; *dy =  1;}
	} else
	printf("direction dx = %d dy = %d\n", *dx, *dy);
}

static inline void direction1(uint8 *img, uint32 w, uint32 yx, int *dx, int *dy, int d)
{
	uint32 max = 0;
	printf("direction dx = %d dy = %d dx = %d dy = %d x = %d y = %d\n", *dx, *dy, d%w, d/w, yx%w, yx/w);
	if(img[yx-1  ] > max && d != yx-1  ) { max = img[yx-1  ]; *dx = -1; *dy =  0;}
	if(img[yx-1-w] > max && d != yx-1-w) { max = img[yx-1-w]; *dx = -1; *dy = -1;}
	if(img[yx  -w] > max && d != yx  -w) { max = img[yx  -w]; *dx =  0; *dy = -1;}
	if(img[yx+1-w] > max && d != yx+1-w) { max = img[yx+1-w]; *dx =  1; *dy = -1;}
	if(img[yx+1  ] > max && d != yx+1  ) { max = img[yx+1  ]; *dx =  1; *dy =  0;}
	if(img[yx+1+w] > max && d != yx+1+w) { max = img[yx+1+w]; *dx =  1; *dy =  1;}
	if(img[yx  +w] > max && d != yx  +w) { max = img[yx  +w]; *dx =  0; *dy =  1;}
	if(img[yx-1+w] > max && d != yx-1+w) { max = img[yx-1+w]; *dx = -1; *dy =  1;}
	print_around(img, yx, w);
	printf("max = %d dx = %d dy = %d\n", max, *dx, *dy);
}

/*	\brief	Find the maximum around the pixel.
	\param	img		The pointer to gradient image.
	\param	yx		The pixel coordinate (yx = y*w + x)
	\param	w		The image width.
	\param  in1		The previous maximum direction.
	\retval			The direction of of local max.
*/
static inline int dir(uint8 *img, uint32 yx, uint32 w, int in1)
{
	uint32 max = 0;
	int in = 0;
	if(in1 == 0   ){
		if(img[yx-1  ] > max) { max = img[yx-1  ]; in = -1  ; }
		if(img[yx-1-w] > max) { max = img[yx-1-w]; in = -1-w; }
		if(img[yx  -w] > max) { max = img[yx  -w]; in =   -w; }
		if(img[yx+1-w] > max) { max = img[yx+1-w]; in = +1-w; }
		if(img[yx+1  ] > max) { max = img[yx+1  ]; in = +1  ; }
		if(img[yx+1+w] > max) { max = img[yx+1+w]; in = +1+w; }
		if(img[yx  +w] > max) { max = img[yx  +w]; in =   +w; }
		if(img[yx-1+w] > max) { max = img[yx-1+w]; in = -1+w; }
		return in;
	}
	else if(in1 == -1  ){
		if(img[yx+1  ] > max) { max = img[yx+1  ]; in = +1  ; }
		if(img[yx+1-w] > max) { max = img[yx+1-w]; in = +1-w; }
		if(img[yx+1+w] > max) { max = img[yx+1+w]; in = +1+w; }
		//if(img[yx  +w] > max) { max = img[yx  +w]; in =   +w; }
		//if(img[yx  -w] > max) { max = img[yx  -w]; in =   -w; }
		//return cn == 2 ? 0 : in;
		return in;
	}
	else if(in1 == -1-w){
		if(img[yx+1+w] > max) { max = img[yx+1+w]; in = +1+w; }
		if(img[yx+1  ] > max) { max = img[yx+1  ]; in = +1  ; }
		if(img[yx  +w] > max) { max = img[yx  +w]; in =   +w; }
		//if(img[yx-1+w] > max) { max = img[yx-1+w]; in = -1+w; }
		//if(img[yx+1-w] > max) { max = img[yx+1-w]; in = +1-w; }
		//return cn == 2 ? 0 : in;
		return in;
	}
	else if(in1 ==   -w){
		if(img[yx  +w] > max) { max = img[yx  +w]; in =   +w; }
		if(img[yx+1+w] > max) { max = img[yx+1+w]; in = +1+w; }
		if(img[yx-1+w] > max) { max = img[yx-1+w]; in = -1+w; }
		//if(img[yx  -1] > max) { max = img[yx  -1]; in =   -1; }
		//if(img[yx+1  ] > max) { max = img[yx+1  ]; in = +1  ; }
		//return cn == 2 ? 0 : in;
		return in;
	}
	else if(in1 ==  1-w){
		if(img[yx-1+w] > max) { max = img[yx-1+w]; in = -1+w; }
		if(img[yx  +w] > max) { max = img[yx  +w]; in =   +w; }
		if(img[yx  -1] > max) { max = img[yx  -1]; in =   -1; }
		//if(img[yx-1-w] > max) { max = img[yx-1-w]; in = -1-w; }
		//if(img[yx+1+w] > max) { max = img[yx+1+w]; in = +1+w; }
		//return cn == 2 ? 0 : in;
		return in;
	}
	else if(in1 ==  1  ){
		if(img[yx  -1] > max) { max = img[yx  -1]; in =   -1; }
		if(img[yx-1+w] > max) { max = img[yx-1+w]; in = -1+w; }
		if(img[yx-1-w] > max) { max = img[yx-1-w]; in = -1-w; }
		//if(img[yx  -w] > max) { max = img[yx  -w]; in =   -w; }
		//if(img[yx  +w] > max) { max = img[yx  +w]; in =   +w; }
		//return cn == 2 ? 0 : in;
		return in;
	}
	else if(in1 ==  1+w){
		if(img[yx-1-w] > max) { max = img[yx-1-w]; in = -1-w; }
		if(img[yx  -1] > max) { max = img[yx  -1]; in =   -1; }
		if(img[yx  -w] > max) { max = img[yx  -w]; in =   -w; }
		//if(img[yx+1-w] > max) { max = img[yx+1-w]; in = +1-w; }
		//if(img[yx-1+w] > max) { max = img[yx-1+w]; in = -1+w; }
		//return cn == 2 ? 0 : in;
		return in;
	}
	else if(in1 ==    w){
		if(img[yx  -w] > max) { max = img[yx  -w]; in =   -w; }
		if(img[yx-1-w] > max) { max = img[yx-1-w]; in = -1-w; }
		if(img[yx+1-w] > max) { max = img[yx+1-w]; in = +1-w; }
		//if(img[yx+1  ] > max) { max = img[yx+1  ]; in = +1  ; }
		//if(img[yx  -1] > max) { max = img[yx  -1]; in =   -1; }
		//return cn == 2 ? 0 : in;
		return in;
	}
	else if(in1 == -1+w){
		if(img[yx+1-w] > max) { max = img[yx+1-w]; in = +1-w; }
		if(img[yx  -w] > max) { max = img[yx  -w]; in =   -w; }
		if(img[yx+1  ] > max) { max = img[yx+1  ]; in = +1  ; }
		//if(img[yx+1+w] > max) { max = img[yx+1+w]; in =  1+w; }
		//if(img[yx-1-w] > max) { max = img[yx-1-w]; in = -1-w; }
		//return cn == 2 ? 0 : in;
		return in;
	}
}

/*	\brief	Check is a pixel the local maximum.
	\param	img		The pointer to gradient image.
	\param	yx		The pixel coordinate (yx = y*w + x)
	\param  w		The image width.
	\retval			1 if local max, 0 - if not
*/
static inline uint32 loc_max(uint8 *img, uint32 yx, uint32 w)
{
	if( img[yx-1] 	<= img[yx] &&
		img[yx-w]	<= img[yx] &&
		img[yx+1] 	<= img[yx] &&
		img[yx+w] 	<= img[yx] &&
		img[yx-1-w] <= img[yx] &&
		img[yx+1-w] <= img[yx] &&
		img[yx-1+w] <= img[yx] &&
		img[yx+1+w] <= img[yx] ) return 1;
	else return 0;
}

static inline uint32 draw_line_3(uint8 *r, uint8 *g, uint8 *b, Vector *v, uint32 w, uint8 *lc)
{
	uint32 i, max , min = 0, n, x, y, dx, dy;
	int sty, stx, yx;

	//stx = v->x2 > v->x1 ? 1 : -1;
	stx = 1;
	if(v->x2 > v->x1){
		x = v->x1; y = v->y1*w;
		sty = v->y2 > v->y1 ? w : -w;
	} else {
		x = v->x2; y = v->y2*w;
		sty = v->y1 > v->y2 ? w : -w;
	}

	dx = abs(v->x2 - v->x1)+1; dy = abs(v->y2 - v->y1)+1;
	//printf("x = %d y = %d x1 = %d y1 = %d x2 = %d y2 = %d dx = %d dy = %d\n", x, y/w, v->x1, v->y1, v->x2, v->y2, dx, dy);
	if(dx >= dy){
		n = dx - 0; max = dx;
		for(i=0; i < n; i++){
			yx = y + x;
			r[yx] = lc[0]; g[yx] = lc[1]; b[yx] = lc[2];
			min += dy; x += stx;
			if(min >= max) { max += dx; y += sty; }
		}
		return dx;
	} else {
		n = dy - 0; max = dy;
		for(i=0; i < n; i++){
			yx = y + x;
			r[yx] = lc[0]; g[yx] = lc[1]; b[yx] = lc[2];
			min += dx; y += sty;
			if(min >= max) { max += dy; x += stx; }
		}
		return dy;
	}
}

static inline uint32 draw_line_1(uint8 *img, Vector *v, uint32 w, uint8 col)
{
	uint32 i, max , min = 0, n, x, y, dx, dy;
	int stx, sty, yx;
	int dma, dmi, stmi, stma;

	x = v->x1; y = v->y1*w;
	sty = v->y2 > v->y1 ? w : -w;
	stx = v->x2 > v->x1 ? 1 : -1;
	dx = abs(v->x2 - v->x1)+1; dy = abs(v->y2 - v->y1)+1;
	if(dx >= dy){ dma = dx; dmi = dy; stmi = stx; stma = sty; }
	else 		{ dma = dy; dmi = dx; stmi = sty; stma = stx; }
	min = 0; max = dma; n = dma;
	if(v->x2 <= v->x1 && (dma&1 || dmi&1)) min = dmi-1;

	for(i=0; i < n; i++){
		yx = y + x;
		img[yx] = col;
		min += dmi; x += stmi;
		if(min >= max) { max += dma; y += stma; }
	}
	return dma;
}

static inline uint32 check_next_pixel(Vector *v, uint32 w)
{
	uint32 i, max , min = 0, n, x, y, dx, dy;
	int stx, sty, yx;
	int dma, dmi, stmi, stma;

	x = v->x1; y = v->y1*w;
	sty = v->y2 > v->y1 ? w : -w;
	stx = v->x2 > v->x1 ? 1 : -1;
	dx = abs(v->x2 - v->x1)+1; dy = abs(v->y2 - v->y1)+1;
	if(dx >= dy){ dma = dx; dmi = dy; stmi = stx; stma = sty; }
	else 		{ dma = dy; dmi = dx; stmi = sty; stma = stx; }
	min = 0; max = dma; n = dma;
	if(v->x2 <= v->x1 && (dma&1 || dmi&1)) min = dmi-1;

	//for(i=0; i < n; i++){
		//yx = y + x;
		//img[yx] = col;
		min += dmi; x += stmi;
		if(min >= max) { max += dma; y += stma; }
	//}
	return y + x;
}

static inline uint32 get_next_pixel(uint32 yx1, uint32 yx2, uint32 w)
{
	if      (yx2-yx1 ==   -1) return yx1-w-1;
	else if (yx2-yx1 == -w-1) return yx1-w  ;
	else if (yx2-yx1 == -w  ) return yx1-w+1;
	else if (yx2-yx1 == -w+1) return yx1  +1;
	else if (yx2-yx1 ==  1  ) return yx1+w+1;
	else if (yx2-yx1 ==  w+1) return yx1+w  ;
	else if (yx2-yx1 ==  w  ) return yx1+w-1;
	else if (yx2-yx1 ==  w-1) return yx1  -1;
}

static inline uint32 scale(uint32  x, uint32  w, uint32 k, uint32 sh)
{
	uint32 x1;
	x1 = ((x-1)*k>>sh);
	return (x1 == w-2) ? x1 : x1 + 1;
}

static inline uint32 check_true_reg(uint8 *img, uint32 yx, uint32 yxn, uint32 w)
// Check is a pixel in thue region
// d  = 0   d = 1    d = 2    d = 3
// |x|c|*|  |*|c|x|  |*|*|*|  |*|*|*|
// |c|*|*|  |*|*|c|  |*|*|c|  |c|*|*|
// |*|*|*|  |*|*|*|  |*|c|x|  |x|c|*|
{
	if(yxn - yx == -w-1){
		if((img[yx-w] & 17) || (img[yx-1] & 17) ) return 0;
	}
	else if(yxn - yx == -w+1){
		if((img[yx-w] & 68) || (img[yx+1] & 68) ) return 0;
	}
	else if(yxn - yx == w+1){
		if((img[yx+w] & 17) || (img[yx+1] & 17)) return 0;
	}
	else if(yxn - yx == w-1){
		if((img[yx+w] & 68) || (img[yx-1] & 68)) return 0;
	}
	 return 1;
}


static inline uint32 get_first_pixel(uint8 *img, Vertex *v,  Vertex *v1,  Vertex *v2, uint32 w, uint32 h, uint32 kx, uint32 ky, uint32 sh)
{
	uint32 i, x, y, yx, yx1, yx2, dx, dy, yxn;
	int stx, sty;
	int dma, dmi, stmi, stma;

	sty = v1->y > v->y ? w : -w;
	stx = v1->x > v->x ? 1 : -1;
	//dx = abs(v1->x - v->x)+1; dy = abs(v1->y - v->y)+1;
	dx = abs(scale(v1->x, w, kx, sh) - scale(v->x, w, kx, sh))+1;
	dy = abs(scale(v1->y, h, ky, sh) - scale(v->y, h, ky, sh))+1;
	if(dx >= dy){ dma = dx; dmi = dy; stmi = stx; stma = sty; }
	else 		{ dma = dy; dmi = dx; stmi = sty; stma = stx; }
	if(v1->x <= v->x && (dma&1 || dmi&1)) dmi = (dmi<<1)-1;

	//x = v->x; y = v->y*w;
	x = scale(v->x, w, kx, sh);
	y = scale(v->y, h, ky, sh)*w;
	yx = y + x;

	//min += dmi; x += stmi;
	//if(min >= max) { max += dma; y += stma; }
	//yx1 = y + x;
	yx1 = yx + stmi;
	if(dmi >= dma) yx1 += stma;

	sty = v2->y > v->y ? w : -w;
	stx = v2->x > v->x ? 1 : -1;
	//dx = abs(v2->x - v->x)+1; dy = abs(v2->y - v->y)+1;
	dx = abs(scale(v2->x, w, kx, sh) - scale(v->x, w, kx, sh))+1;
	dy = abs(scale(v2->y, h, ky, sh) - scale(v->y, h, ky, sh))+1;
	if(dx >= dy){ dma = dx; dmi = dy; stmi = stx; stma = sty; }
	else 		{ dma = dy; dmi = dx; stmi = sty; stma = stx; }
	if(v2->x <= v->x && (dma&1 || dmi&1)) dmi = (dmi<<1)-1;

	yx2 = yx + stmi;
	if(dmi >= dma) yx2 += stma;

	if(yx1 == yx2) {
		//printf("it's a problem yx = %d %d yx1 = %d %d yx2 = %d %d\n", yx%w, yx/w, yx1%w, yx1/w, yx2%w, yx2/w);
		//img[yx] = 255; img[yx1] = 255; img[yx2] = 255;
		return 0;
	}

	yxn = yx1;
	do{
		yxn = get_next_pixel(yx, yxn, w);
		//if(yxn != yx2 && !img[yxn]){
		if(yxn != yx2 && !img[yxn] && check_true_reg(img, yx, yxn, w)){
			return yxn;
		}
	} while(yxn != yx2);

	return 0;
}

static inline void add_dir2(uint8 *img, uint32 yx1, uint32 yx2, uint32 w)
{
	//vx->n++;
	if      (yx2-yx1 == -1  ) img[yx1] |= 128;
	else if (yx2-yx1 == -w-1) img[yx1] |= 64;
	else if (yx2-yx1 == -w  ) img[yx1] |= 32;
	else if (yx2-yx1 == -w+1) img[yx1] |= 16;
	else if (yx2-yx1 ==  1  ) img[yx1] |= 8;
	else if (yx2-yx1 ==  w+1) img[yx1] |= 4;
	else if (yx2-yx1 ==  w  ) img[yx1] |= 2;
	else if (yx2-yx1 ==  w-1) img[yx1] |= 1;
    //printf("finish d = %d dir = %o\n", d, dir);
}

static inline uint32 draw_line_dir(uint8 *img, Vector *v, uint32 w)
{
	uint32 i, max , min = 0, n, x, y, dx, dy;
	int stx, sty, yx, yx1;
	int dma, dmi, stmi, stma;

	x = v->x1; y = v->y1*w;
	sty = v->y2 > v->y1 ? w : -w;
	stx = v->x2 > v->x1 ? 1 : -1;
	dx = abs(v->x2 - v->x1)+1; dy = abs(v->y2 - v->y1)+1;
	if(dx >= dy){ dma = dx; dmi = dy; stmi = stx; stma = sty; }
	else 		{ dma = dy; dmi = dx; stmi = sty; stma = stx; }
	min = 0; max = dma; n = dma;
	if(v->x2 <= v->x1 && (dma&1 || dmi&1)) min = dmi-1;

	for(i=0; i < n; i++){
		yx = y + x;
		if(i > 0) add_dir2(img, yx1, yx, w);
		min += dmi; x += stmi;
		if(min >= max) { max += dma; y += stma; }
		yx1 = yx;
	}
}

/*	\brief	Finf pixels with lines intersection.
	\param	grad	The pointer to input gradient image.
	\param	con		The pointer to output contour image.
	\param  w		The image width.
	\param  h		The image height.
*/
void seg_find_intersect(uint8 *grad, uint8 *con, uint32 w, uint32 h)
{
	uint32 y, y1, x, yx, yw, yx1, yx2, i, h1 = h-1, w1 = w-1;
	int d1, d2, npix = 0;
	//Make border
	con[w+1] = 255; con[(w<<1)-2] = 255;
	con[w*(h-2)+1] = 255; con[w*(h-1)-2] = 255;
	yw = w*(h-2);
	for(x=2; x < w1-1; x++) con[yw + x] = 64;

	for(y=1; y < h1; y++){
		yw = y*w;
		for(x=1; x < w1; x++){
			yx = yw + x;
			if(grad[yx] && !con[yx]){
				if(loc_max(grad, yx, w)){
					//printf("x = %d y = %d\n", x, y);
					yx1 = yx; yx2 = yx;
					con[yx1] = grad[yx1]; //con[yx1] = 64;
					d1 = dir(grad, yx1, w, 0);
					d2 = dir(grad, yx1, w, d1);
					while(1){
						yx1 = yx1 + d1;
						if(con[yx1]) {
							con[yx1] = 255; grad[yx1] = 255;
							npix++;
							break;
						}
						//if(!d1){ con[yx1] = 128; break; }
						con[yx1] = grad[yx1]; grad[yx1] = 254; //con[yx1] = 64;
						d1 = dir(grad, yx1, w, -d1);
					}
					while(1){
						yx2 = yx2 + d2;
						if(con[yx2]) {
							con[yx2] = 255; grad[yx2] = 255;
							npix++;
							break;
						}
						//if(!d2){ con[yx2] = 128; break; }
						con[yx2] = grad[yx2];  grad[yx2] = 254; //con[yx2] = 64;
						d2 = dir(grad, yx2, w, -d2);

					}
				}
				//break;
			}
		}
	}
	printf("Numbers of intersection  = %d\n", npix);
}

static inline uint32 check_nei2(uint8 *img, uint32 yx, uint32 w)
{
	uint32 cn = 0;

	if(img[yx-1]) cn++;
	if(img[yx-w]) cn++;
	if(img[yx+1]) cn++;
	if(img[yx+w]) cn++;
	if(img[yx-1-w]) cn++;
	if(img[yx+1-w]) cn++;
	if(img[yx-1+w]) cn++;
	if(img[yx+1+w]) cn++;

	if(cn < 2) return 1;
	else return 0;
}

uint32 seg_remove_line1(uint8 *con, uint32 w, uint32 h)
{
	uint32 y, y1, x, yx, yw, yx1, yx2, i, h1 = h-1, w1 = w-1, is = 0;
	int d, npix = 0;

	for(y=1; y < h1; y++){
		yw = y*w;
		for(x=1; x < w1; x++){
			yx = yw + x;
			if(con[yx] == 128){
				//printf("x = %d y = %d\n", x, y);
				yx1 = yx;
				con[yx1] = 0;
				d = dir(con, yx1, w, 0);
				while(1){
					//printf("yx = %d d = %d con = %d\n", yx1, d, con[yx1]);
					yx1 = yx1 + d;
					if(con[yx1] == 128) { con[yx1] = 0; break; }
					else if(con[yx1] == 255) {
						if(check_nei2(con, yx1, w)) d = 0;
						else break;
					}
					con[yx1] = 0;
					d = dir(con, yx1, w, -d);
					if(!d)  break;
				}
				//break;
			}
		}
	}
	//printf("Numbers of intersection  = %d\n", npix);
}

uint32 seg_fill_reg(uint32 *reg, uint32 *buff, uint32 w, uint32 h)
{
	uint32 i, j, y, x, yx, yw, yxw, h1 = h-2, w1 = w-2;
	uint32 num, rgc = 1<<8;
	uint32 *l1 = buff, *l2 = &buff[w*h>>2], *tm;

	for(y=2; y < h1; y++){
		yw = y*w;
		for(x=2; x < w1; x++){
			yx = yw + x;
			if(!reg[yx]){
				//printf("Start x = %d y = %d con = %d\n", yx%w, yx/w,  con[yx]);
				reg[yx] = rgc; num = 1; l1[0] = yx; i = 0;
				while(num){
					for(j=0; j < num; j++){
						/*
						printf("x = %d y = %d con = %d num = %d \n", l1[j]%w, l1[j]/w, con[l1[j]], num);
						printf("%3d %3d %3d\n%3d %3d %3d\n%3d %3d %3d\n\n",
								con[l1[j]-w-1], con[l1[j]-w], con[l1[j]-w+1],
								con[l1[j]-1], con[l1[j]], con[l1[j]+1],
								con[l1[j]+w-1], con[l1[j]+w], con[l1[j]+w+1]);

						printf("%5d %5d %5d\n%5d %5d %5d\n%5d %5d %5d\n\n",
								reg[l1[j]-w-1], reg[l1[j]-w], reg[l1[j]-w+1],
								reg[l1[j]-1], reg[l1[j]], reg[l1[j]+1],
								reg[l1[j]+w-1], reg[l1[j]+w], reg[l1[j]+w+1]);*/

						yxw = l1[j] - 1;
						if(!reg[yxw]) { reg[yxw] = rgc; l2[i++] = yxw; }
						yxw = l1[j] - w;
						if(!reg[yxw]) { reg[yxw] = rgc; l2[i++] = yxw; }
						yxw = l1[j] + 1;
						if(!reg[yxw]) { reg[yxw] = rgc; l2[i++] = yxw; }
						yxw = l1[j] + w;
						if(!reg[yxw]) { reg[yxw] = rgc; l2[i++] = yxw; }

					}
					num = i; i = 0;
					tm = l1; l1 = l2; l2 = tm;
				}
				rgc+=256;
			}
		}
	}
	rgc = (rgc>>8) - 1;

	printf("Numbers of regions  = %d\n", rgc);
	//printf("Numbers of intersection  = %d\n", npix);
}

static inline void remove_pixel(uint32 *reg, uint32 yx, uint32 w)
{
	uint32 cn = 0;
	if(reg[yx-1]>>8  ) if(!cn) cn = reg[yx-1];
	//printf("cn = %d i = %d\n", cn, i);
	if(reg[yx-1-w]>>8) {
		if(!cn) cn = reg[yx-1-w];
		else if (cn != reg[yx-1-w])  goto m1;
	}
	//printf("cn = %d i = %d\n", cn, i);
	if(reg[yx-w]>>8) {
		if(!cn) cn = reg[yx-w];
		else if (cn != reg[yx-w])  goto m1;
	}
	//printf("cn = %d i = %d\n", cn, i);
	if(reg[yx-w+1]>>8) {
		if(!cn) cn = reg[yx-w+1];
		else if (cn != reg[yx-w+1])  goto m1;
	}
	//printf("cn = %d i = %d\n", cn, i);
	if(reg[yx+1]>>8) {
		if(!cn) cn = reg[yx+1];
		else if (cn != reg[yx+1])  goto m1;
	}
	//printf("cn = %d i = %d\n", cn, i);
	if(reg[yx+1+w]>>8) {
		if(!cn) cn = reg[yx+1+w];
		else if (cn != reg[yx+1+w])  goto m1;
	}
	//printf("cn = %d i = %d\n", cn, i);
	if(reg[yx+w]>>8) {
		if(!cn) cn = reg[yx+w];
		else if (cn != reg[yx+w])  goto m1;
	}
	//printf("cn = %d i = %d\n", cn, i);
	if(reg[yx+w-1]>>8) {
		if(!cn) cn = reg[yx+w-1];
		else if (cn != reg[yx+w-1])  goto m1;
	}
	//printf("cn = %d i = %d\n", cn, i);
	//if(cn != 1) {
	reg[yx] = cn;
	return;
m1:
	return;
}

void seg_remove_contour(uint32 *reg, uint32 w, uint32 h)
{
	uint32 i, j, y, x, yx, yw, yxw, h1 = h-2, w1 = w-2, rg = 0;

	for(y=2; y < h1; y++){
		yw = y*w;
		for(x=2; x < w1; x++){
			yx = yw + x;
			//if(reg[yx]&0xFF){
			if(reg[yx] == 64){
				remove_pixel(reg, yx, w);
			}
		}
	}
	//printf("Numbers of regions  = %d\n", rgc);
}

static inline uint32 true_vertex(uint8 *con, uint32 *reg, uint32 yx, uint32 w, uint32 h)
{
	uint32 cn = 0;
	if(reg[yx-1]  ) if(!cn) cn = reg[yx-1];
	//printf("cn = %d i = %d\n", cn, i);
	if(reg[yx-1-w]) {
		if(!cn) cn = reg[yx-1-w];
		else if (cn != reg[yx-1-w])  goto m1;
	}
	//printf("cn = %d i = %d\n", cn, i);
	if(reg[yx-w]) {
		if(!cn) cn = reg[yx-w];
		else if (cn != reg[yx-w])  goto m1;
	}
	//printf("cn = %d i = %d\n", cn, i);
	if(reg[yx-w+1]) {
		if(!cn) cn = reg[yx-w+1];
		else if (cn != reg[yx-w+1])  goto m1;
	}
	//printf("cn = %d i = %d\n", cn, i);
	if(reg[yx+1]) {
		if(!cn) cn = reg[yx+1];
		else if (cn != reg[yx+1])  goto m1;
	}
	//printf("cn = %d i = %d\n", cn, i);
	if(reg[yx+1+w]) {
		if(!cn) cn = reg[yx+1+w];
		else if (cn != reg[yx+1+w])  goto m1;
	}
	//printf("cn = %d i = %d\n", cn, i);
	if(reg[yx+w]) {
		if(!cn) cn = reg[yx+w];
		else if (cn != reg[yx+w])  goto m1;
	}
	//printf("cn = %d i = %d\n", cn, i);
	if(reg[yx+w-1]) {
		if(!cn) cn = reg[yx+w-1];
		else if (cn != reg[yx+w-1])  goto m1;
	}
	//printf("cn = %d i = %d\n", cn, i);
	//if(cn != 1) {
	con[yx] = 0;
	return 0;
	m1:
	return 1;
}


static inline uint32 loc_max1(uint8 *img, uint32 yx, uint32 w, uint32 th)
{
	uint8 im = img[yx]>>th;
	if( (img[yx-1]>>th) <= im &&
		(img[yx-w]>>th)	<= im &&
		(img[yx+1]>>th) <= im &&
		(img[yx+w]>>th) <= im &&
		(img[yx-1-w]>>th) <= im &&
		(img[yx+1-w]>>th) <= im &&
		(img[yx-1+w]>>th) <= im &&
		(img[yx+1+w]>>th) <= im ) return 1;
	else return 0;
}


/*	\brief	Check neighborhoods around the pixel.
	\param	img		The pointer to gradient image.
	\param	yx		The pixel coordinate (yx = y*w + x)
	\param  w		The image width.
	\param  di		The pointer to direction storage.
	\retval			The number of neighborhoods.
*/
static inline uint8 check_neighbor(uint8 *img, uint32 yx, uint32 w, uint8 *di)
///Directions around the pixel
/// |1|2|3| uint8 01234567
/// |0|x|4|
/// |7|6|5|
{
	uint32 c = 0;
	*di = 0;
	if(img[yx-1  ]) { *di |= 128; 	c++; }
	if(img[yx-w-1]) { *di |= 64; 	c++; }
	if(img[yx-w  ]) { *di |= 32; 	c++; }
	if(img[yx-w+1]) { *di |= 16;	c++; }
	if(img[yx+1  ]) { *di |= 8; 	c++; }
	if(img[yx+w+1]) { *di |= 4; 	c++; }
	if(img[yx+w  ]) { *di |= 2; 	c++; }
	if(img[yx+w-1]) { *di |= 1; 	c++; }
	return c;
}

static inline uint32 new_vertex(uint8 *con, Vertex *vx, Vertex **vp, Vertex **vp1, uint32 x, uint32 y, uint32 yx, uint32 w)
{
	vx->x = x; vx->y = y;
	vx->n = check_neighbor(con, yx, w, &vx->di);
	vx->cn = 0;
	vx->vp = vp1;
	*vp = vx;
	return vx->n;
	//printf("x = %d y = %d n = %d vp = %p\n", (*vp)->x, (*vp)->y, (*vp)->n, *vp);

}

static inline void new_vertex1(uint8 dir, Vertex *vx, Vertex **vp, Vertex **vp1, uint32 x, uint32 y, uint32 w)
{
	vx->x = x; vx->y = y;
	vx->di = dir;
	vx->n = 0;
	vx->cn = 0;
	vx->vp = vp1;
	*vp = vx;
	//return vx->n;
	//printf("x = %d y = %d n = %d vp = %p\n", (*vp)->x, (*vp)->y, (*vp)->n, *vp);

}

static inline uint32 new_in_line_vertex(Vertex *vx, Vertex **vp, Vertex **vp1, uint32 x, uint32 y, uint32 w)
{
	vx->x = x; vx->y = y;
	vx->n = 0;
	//add_dir(vx, d1, w);
	//add_dir(vx, d2, w);
	vx->cn = 0;
	vx->vp = vp1;
	*vp = vx;
	//return vx->n;
	return vx->n;
	//printf("x = %d y = %d n = %d vp = %p\n", (*vp)->x, (*vp)->y, (*vp)->n, *vp);

}


static inline void new_line(Vertex *vx1, Vertex *vx2, uint32 nd1, uint32 nd2, uint8 pow)
{
	vx1->vp[nd1] = vx2;
	vx2->vp[nd2] = vx1;
}

/*      \brief  Set finished direction bit.
        \param  vx		The pointer to vertex.
        \param  d       The direction.
        \param  w       The image width.
        \retval			The number of derection.
*/
static inline uint32 finish_dir(Vertex *vx, int d, uint32 w)
{
	if      (d == -1  ) { vx->cn |= 128; 	return 0; }
	else if (d == -w-1) { vx->cn |= 64;	 	return 1; }
	else if (d == -w  ) { vx->cn |= 32; 	return 2; }
	else if (d == -w+1) { vx->cn |= 16; 	return 3; }
	else if (d ==  1  ) { vx->cn |= 8; 		return 4; }
	else if (d ==  w+1) { vx->cn |= 4; 		return 5; }
	else if (d ==  w  ) { vx->cn |= 2; 		return 6; }
	else if (d ==  w-1) { vx->cn |= 1; 		return 7; }
        //printf("finish d = %d %o %o\n", d, vx->di, vx->cn);
}

/*      \brief  Set finished direction bit.
        \param  vx		The pointer to vertex.
        \param  d       The direction.
        \param  w       The image width.
        \retval			The direction.
*/
static inline uint32 add_finish_dir(Vertex *vx, int d, uint32 w)
{
	vx->n++;
	if      (d == -1  ) { vx->di |= 128;    vx->cn |= 128;	return  0; }
	else if (d == -w-1) { vx->di |= 64; 	vx->cn |= 64;	return  1; }
	else if (d == -w  ) { vx->di |= 32;     vx->cn |= 32;	return  2; }
	else if (d == -w+1) { vx->di |= 16;     vx->cn |= 16;	return  3; }
	else if (d ==  1  ) { vx->di |= 8;      vx->cn |= 8; 	return  4; }
	else if (d ==  w+1) { vx->di |= 4;      vx->cn |= 4;	return  5; }
	else if (d ==  w  ) { vx->di |= 2;      vx->cn |= 2; 	return  6; }
	else if (d ==  w-1) { vx->di |= 1;      vx->cn |= 1; 	return  7; }
        //printf("finish d = %d %o %o\n", d, vx->di, vx->cn);
}

/*      \brief  Add new direction.
        \param  vx		The pointer to vertex.
        \param  d       The direction.
        \param  w       The image width.
        \retval			The direction.
*/
static inline uint32 add_dir(Vertex *vx, int d, uint32 w)
{
	vx->n++;
	if      (d == -1  ) { vx->di |= 128;	return  0; }
	else if (d == -w-1) { vx->di |= 64; 	return  1; }
	else if (d == -w  ) { vx->di |= 32; 	return  2; }
	else if (d == -w+1) { vx->di |= 16;		return  3; }
	else if (d ==  1  ) { vx->di |= 8;		return  4; }
	else if (d ==  w+1) { vx->di |= 4;		return  5; }
	else if (d ==  w  ) { vx->di |= 2;		return  6; }
	else if (d ==  w-1) { vx->di |= 1;		return  7; }
        //printf("finish d = %d %o %o\n", d, vx->di, vx->cn);
}

static inline void add_dir1(uint8 *dir, int d, uint32 w)
{
	//vx->n++;
	if      (d == -1  ) *dir |= 128;
	else if (d == -w-1) *dir |= 64;
	else if (d == -w  ) *dir |= 32;
	else if (d == -w+1) *dir |= 16;
	else if (d ==  1  ) *dir |= 8;
	else if (d ==  w+1) *dir |= 4;
	else if (d ==  w  ) *dir |= 2;
	else if (d ==  w-1) *dir |= 1;
    //printf("finish d = %d dir = %o\n", d, dir);
}

/*      \brief  Remove direction bit.
        \param  vx              The pointer to vertex.
        \param  d               The direction.
        \param  w               The image width.
*/
static inline void remove_dir(Vertex *vx, int d, uint32 w)
{
	vx->n--;
	if      (d == -1  ) { vx->di ^= 128;    vx->cn ^= 128; }
	else if (d == -w-1) { vx->di ^= 64; 	vx->cn ^= 64;  }
	else if (d == -w  ) { vx->di ^= 32;     vx->cn ^= 32;  }
	else if (d == -w+1) { vx->di ^= 16;     vx->cn ^= 16;  }
	else if (d ==  1  ) { vx->di ^= 8;      vx->cn ^= 8;   }
	else if (d ==  w+1) { vx->di ^= 4;      vx->cn ^= 4;   }
	else if (d ==  w  ) { vx->di ^= 2;      vx->cn ^= 2;   }
	else if (d ==  w-1) { vx->di ^= 1;      vx->cn ^= 1;   }
        //printf("finish d = %d %o %o\n", d, vx->di, vx->cn);
}

/*	\brief	Get next not finished direction.
	\param	vx		The pointer to vertex.
	\param	dx		The direction in X axis.
	\param	dy		The direction in Y axis.
	\param	nd		The direction.
	\retval			1 if exist not finished direction, 0 - if not
*/
static inline uint32 get_next_dir(Vertex *vx, int *dx, int *dy, uint32 *nd)
{
	//printf("dx = %d dy = %d di = %o cn = %o\n", *dx, *dy, vx->di, vx->cn);
	if		((vx->di&128) && !(vx->cn&128)) { *dx =-1; *dy = 0; vx->cn |= 128;	*nd = 0; 	goto m1; }
	else if	((vx->di&64 ) && !(vx->cn&64 )) { *dx =-1; *dy =-1; vx->cn |= 64;	*nd = 1;  	goto m1; }
	else if ((vx->di&32 ) && !(vx->cn&32 )) { *dx = 0; *dy =-1; vx->cn |= 32;	*nd = 2;  	goto m1; }
	else if ((vx->di&16 ) && !(vx->cn&16 )) { *dx = 1; *dy =-1; vx->cn |= 16;	*nd = 3;  	goto m1; }
	else if ((vx->di&8  ) && !(vx->cn&8  )) { *dx = 1; *dy = 0; vx->cn |= 8;	*nd = 4;  	goto m1; }
	else if ((vx->di&4  ) && !(vx->cn&4  )) { *dx = 1; *dy = 1; vx->cn |= 4;	*nd = 5;  	goto m1; }
	else if ((vx->di&2  ) && !(vx->cn&2  )) { *dx = 0; *dy = 1; vx->cn |= 2;	*nd = 6;  	goto m1; }
	else if ((vx->di&1  ) && !(vx->cn&1  )) { *dx =-1; *dy = 1; vx->cn |= 1;	*nd = 7;  	goto m1; }
	return 0;
m1: vx->n++;
	return 1;
}

static inline void get_next_dir1(uint8 dir, int *dx, int *dy)
{
	*dx = 0; *dy = 0;
	if		(dir&128) { *dx =-1; *dy = 0;}
	else if	(dir&64 ) { *dx =-1; *dy =-1;}
	else if (dir&32 ) { *dx = 0; *dy =-1;}
	else if (dir&16 ) { *dx = 1; *dy =-1;}
	else if (dir&8  ) { *dx = 1; *dy = 0;}
	else if (dir&4  ) { *dx = 1; *dy = 1;}
	else if (dir&2  ) { *dx = 0; *dy = 1;}
	else if (dir&1  ) { *dx =-1; *dy = 1;}
}

static inline void get_per_dir(int d, uint32 w, int *ld, int *rd)
{
	if      (d == -1  ) { *ld = w;  	*rd = -w; 	}
	else if (d == -w-1) { *ld = w-1;  	*rd = -w+1; }
	else if (d == -w  ) { *ld = -1;  	*rd = 1; 	}
	else if (d == -w+1) { *ld = -w-1;  	*rd = w+1; 	}
	else if (d ==  1  ) { *ld = -w;  	*rd = w; 	}
	else if (d ==  w+1) { *ld = -w+1;  	*rd = w-1; 	}
	else if (d ==  w  ) { *ld = 1;  	*rd = -1;	}
	else if (d ==  w-1) { *ld = w+1;  	*rd = -w-1; }
        //printf("finish d = %d %o %o\n", d, vx->di, vx->cn);
}
/*
static inline uint32 get_next_vertex(uint8 *img, uint8 *dir, uint32 yx, uint8 nd, uint32 *yx1, uint8 *nd1)
{
	uint32 n = 1;
	*yx1 = yx;
	if		(nd == 0) { *yx1 += -1  ;}
	else if	(nd == 1) { *yx1 += -1-w;}
	else if (nd == 2) { *yx1 +=   -w;}
	else if (nd == 3) { *yx1 +=  1-w;}
	else if (nd == 4) { *yx1 +=  1  ;}
	else if (nd == 5) { *yx1 +=  1+w;}
	else if (nd == 6) { *yx1 +=    w;}
	else if (nd == 7) { *yx1 += -1+w;}

	while(img[yx] < 254){
		n++;
		if		(dir[yx]&128) { *yx1 += -1  ;}
		else if	(dir[yx]&64 ) { *yx1 += -1-w;}
		else if (dir[yx]&32 ) { *yx1 +=   -w;}
		else if (dir[yx]&16 ) { *yx1 +=  1-w;}
		else if (dir[yx]&8  ) { *yx1 +=  1  ;}
		else if (dir[yx]&4  ) { *yx1 +=  1+w;}
		else if (dir[yx]&2  ) { *yx1 +=    w;}
		else if (dir[yx]&1  ) { *yx1 += -1+w;}
	}
	return n;
}
*/

static inline uint32 is_new_line3(int d, int *fs, int *sc, int *cfs, int *csc, int *ll)
{
	if(!*fs) { *fs = d; (*cfs)++; return 0; }
	if(!*sc && *fs != d ) { *sc = d; (*csc)++; *ll = *cfs; *cfs = 0; return 0; }
	if(*fs == d) {
		(*cfs)++; *csc = 0;
		if(*ll) if(*cfs >* ll) return 1;
		else return 0;
	}
	if(*sc == d){
		if(!*csc){
			if(*ll == *cfs) { (*csc)++; *cfs = 0; return 0; }
			else return 1;
		} else {
			return 1;
		}
	}
	if(*fs != d && *sc != d) { return 1; }
	//(*cn)++;
	//return 0;
}

static inline uint32 is_new_line1(int d, uint32 *cn, int *fs, int *sc)
{
	if(!*fs) *fs = d;
	else if(!*sc && *fs != d ) *sc = d;
	else if(*fs != d && *sc != d) { (*cn)++; return 1;}
	(*cn)++;
	//printf("fs = %d sc = %d cn =%d\n", *fs, *sc, *cn);
	return 0;
}

static inline uint32 is_new_line2(int d, uint32 *cn, int *fs, int *sc)
{
	if(!*fs) *fs = d;
	else if(!*sc && *fs != d ) {
		if(*cn > 5) { (*cn)++; return 1;}
		else *sc = d;
	}
	else if(*fs != d && *sc != d) { (*cn)++; return 1;}
	(*cn)++;
	//printf("fs = %d sc = %d cn =%d\n", *fs, *sc, *cn);
	return 0;
}

void seg_find_intersect2(uint8 *grad, uint8 *con, uint8 *di, uint32 w, uint32 h)
{
	uint32 y, y1, x, yx, yw, yx1, yx2, i, h1 = h-1, w1 = w-1;
	int d1, d2, npix = 0;
	//Make border
	con[w+1] = 255; con[(w<<1)-2] = 255;
	con[w*(h-2)+1] = 255; con[w*(h-1)-2] = 255;
	yw = w*(h-2);
	for(x=2; x < w1-1; x++) con[yw + x] = 64;
	for(x=2; x < w1; x++) di[yw + x] = 128;

	for(y=1; y < h1; y++){
		yw = y*w;
		for(x=1; x < w1; x++){
			yx = yw + x;
			if(grad[yx] && !con[yx]){
				if(loc_max(grad, yx, w)){
					//printf("x = %d y = %d\n", x, y);
					yx1 = yx; yx2 = yx;
					con[yx1] = grad[yx1]; grad[yx1] = 254; con[yx1] = 64;
					d1 = dir(grad, yx1, w, 0);
					d2 = dir(grad, yx1, w, d1);
					while(1){
						if(!d1){ con[yx1] = 128;  grad[yx1] = 255; break; }//}
						add_dir1(&di[yx1], d1, w);
						//printf("d1 = %d di = %d \n", d1, di[yx1]);
						yx1 = yx1 + d1;
						if(con[yx1]) {
							con[yx1] = 255; grad[yx1] = 255;
							npix++;
							break;
						}
						con[yx1] = grad[yx1]; grad[yx1] = 254; con[yx1] = 64;
						d1 = dir(grad, yx1, w, -d1);
					}
					while(1){
						if(!d2){ con[yx2] = 128;  grad[yx2] = 254; break;}//
						yx2 = yx2 + d2;
						add_dir1(&di[yx2], -d2, w);
						if(con[yx2]) {
							con[yx2] = 255; grad[yx2] = 255;
							npix++;
							break;
						}
						con[yx2] = grad[yx2];  grad[yx2] = 254; con[yx2] = 64;
						d2 = dir(grad, yx2, w, -d2);

					}
				}
				//break;
			}
		}
	}
	printf("Numbers of intersection  = %d\n", npix);
}

void seg_find_intersect3(uint8 *grad, uint32 *con, uint8 *di, uint32 w, uint32 h)
{
	uint32 y, y1, x, yx, yw, yx1, yx2, i, h1 = h-1, w1 = w-1;
	int d1, d2, npix = 0;
	//Make border
	//The right direction for last row
	yw = w*(h-2);
	for(x=2; x < w1-1; x++) con[yw + x] = 64;
	for(x=2; x < w1; x++) di[yw + x] = 128;

	for(y=1; y < h1; y++){
		yw = y*w;
		for(x=1; x < w1; x++){
			yx = yw + x;
			if(grad[yx] && !con[yx]){
				if(loc_max(grad, yx, w)){
					//printf("x = %d y = %d\n", x, y);
					yx1 = yx; yx2 = yx;
					con[yx1] = grad[yx1]; grad[yx1] = 254; con[yx1] = 64;
					d1 = dir(grad, yx1, w, 0);
					d2 = dir(grad, yx1, w, d1);
					while(1){
						//if(!d1){ con[yx1] = 128;  grad[yx1] = 255; break; }//}
						add_dir1(&di[yx1], d1, w);
						//printf("d1 = %d di = %d \n", d1, di[yx1]);
						yx1 = yx1 + d1;
						if(con[yx1]) {
							con[yx1] = 255; grad[yx1] = 255;
							npix++;
							break;
						}
						con[yx1] = grad[yx1]; grad[yx1] = 254; con[yx1] = 64;
						d1 = dir(grad, yx1, w, -d1);
					}
					while(1){
						//if(!d2){ con[yx2] = 128;  grad[yx2] = 254; break;}//
						yx2 = yx2 + d2;
						add_dir1(&di[yx2], -d2, w);
						if(con[yx2]) {
							con[yx2] = 255; grad[yx2] = 255;
							npix++;
							break;
						}
						con[yx2] = grad[yx2];  grad[yx2] = 254; con[yx2] = 64;
						d2 = dir(grad, yx2, w, -d2);
					}
				}
				//break;
			}
		}
	}
	printf("Numbers of intersection  = %d\n", npix);
}

void seg_find_intersect4(uint8 *grad, uint8 *con, uint8 *di, uint32 w, uint32 h)
{
	uint32 y, y1, x, yx, yw, yx1, yx2, i, h1 = h-2, w1 = w-2;
	int d1, d2, npix = 0, cr;
	//Make border
	//The right direction for last row
	//yw = w*(h-2);
	//for(x=2; x < w1-1; x++) con[yw + x] = 64;
	//for(x=2; x < w1; x++) di[yw + x] = 128;

	for(y=2; y < h1; y++){
		yw = y*w;
		for(x=2; x < w1; x++){
			yx = yw + x;
			if(grad[yx] && !con[yx]){
				if(loc_max(grad, yx, w)){
					//printf("x = %d y = %d\n", x, y);
					yx1 = yx; yx2 = yx; cr = 0;
					con[yx1] = grad[yx1]; grad[yx1] = 254; con[yx1] = 64;
					d1 = dir(grad, yx1, w, 0);
					d2 = dir(grad, yx1, w, d1);
					while(1){
						//if(!d1){ con[yx1] = 128;  grad[yx1] = 255; break; }//}
						add_dir1(&di[yx1], d1, w);
						//printf("d1 = %d di = %d \n", d1, di[yx1]);
						yx1 = yx1 + d1;
						if(con[yx1]) {
							con[yx1] = 255; grad[yx1] = 255;
							npix++;
							//Check for circle
							if(yx == yx1) cr = 1;
							break;
						}
						con[yx1] = grad[yx1]; grad[yx1] = 254; con[yx1] = 64;
						d1 = dir(grad, yx1, w, -d1);
					}
					if(!cr){
						while(1){
							//if(!d2){ con[yx2] = 128;  grad[yx2] = 254; break;}//
							yx2 = yx2 + d2;
							add_dir1(&di[yx2], -d2, w);
							if(con[yx2]) {
								con[yx2] = 255; grad[yx2] = 255;
								npix++;
								break;
							}
							con[yx2] = grad[yx2];  grad[yx2] = 254; con[yx2] = 64;
							d2 = dir(grad, yx2, w, -d2);

						}
					}
				}
				//break;
			}
		}
	}
	printf("Numbers of intersection  = %d\n", npix);
}

/*	\brief	Create vertexes and lines arrays
	\param	con		The pointer to contour image.
	\param	vx		The pointer to Vertex array.
	\param	ln		The pointer to Line array.
	\param  w		The image width.
	\param  h		The image height.
	\retval			The number of vertex.
*/
uint32 seg_vertex1(uint8 *con, uint8 *di, Vertex *vx, Vertex **vp, uint32 w, uint32 h)
{
	uint32 j, y, x, x1, y1, x2, y2, yx, yx1, yx2, yx3, yw, nd1, nd2, yxd, h1 = h-1, w1 = w-1;
	int vxc = 0, lnc = 0, pow, cc, lc;
	int d, d1, d2, dx, dy, fs, sc, cfs, csc, ll, ld, rd;
	Vertex **vp1 = &vp[w*h>>4];
	//int dx1, dy1;

	for(y=1; y < h1; y++){
		yw = y*w;
		for(x=1; x < w1; x++){
			yx = yw + x;
			if(con[yx] == 255 || con[yx] == 254) { //New vertex
				x1 = x; y1 = y;
				if(con[yx] == 255) {
					new_vertex1(di[yx], &vx[yx], &vp[vxc++], &vp1[lnc+=8], x1, y1, w);
					//new_in_line_vertex(&vx[yx1], &vp[vxc++], &vp1[lnc+=8], x1, y1, w);
					con[yx] = 254;
					//printf("New vertex dir = %d yx = %d vx[yx].lp = %p\n", di[yx], yx, vx[yx].lp);
					/*
					if(new_vertex1(di[yx], &vx[yx], &vp[vxc++], &lp[lnc+=8], x1, y1, w)){
						printf("New vertex dir = %d\n", di[yx]);
						con[yx] = 254;
					} else {	// Remove  not connected vertex
						vxc--; con[yx] = 0;
					}*/
				}
				yx1 = yx;

				while(get_next_dir(&vx[yx1], &dx, &dy, &nd2)){
					//printf("get_next_dir x = %d y = %d dx = %d dy = %d nd2 = %d di = %o cn = %o\n", vx[yx1].x, vx[yx1].y, dx, dy, nd2, vx[yx1].di, vx[yx1].cn);
					yx2 = yx1; yx3 = yx1; d2 = dx + w*dy;  x2 = x1; y2 = y1;
					lc = 0;
					//print_around(con, yx1, w);
					//print_around(di, yx1, w);
					//printf("y = %d x = %d con = %d d = %d w = %d di = %o\n", (yx1)/w, (yx1)%w, con[yx1], dx + w*dy, w, vx[yx1].di);
					// Variable for line construction.
					fs = 0; sc = 0; cfs = 0; csc = 0; ll = 0;
					//Find two perpendicular directions

					//rc[0] = r[yx1]; rc[1] = g[yx1]; rc[2] = b[yx1]; cc = 1;
					pow = 0; cc = 0;
					while(1){
						x1 += dx; y1 += dy;
						d = dx + w*dy;
						yx1 = yx1 + d;
						cc++;

						//print_around(con, yx1, w);
						//print_around(di, yx1, w);
						//printf("y = %d x = %d con = %d d = %d w = %d yx1 = %d\n", (yx1)/w, (yx1)%w, con[yx1], -d, w, yx1);
						if(con[yx1] == 128){ //Not connected virtex
							remove_dir(&vx[yx3], d2, w);
							//linc =  linc - lc;
							vxc = vxc - lc;
							/*
							printf("Remove dir d2 = %d di = %o cn = %o\n", d2, vx[yx3].di, vx[yx3].cn);
							printf("x = %d y = %d d = %o lc = %d linc = %d vxc = %d \n",vx[yx3].x, vx[yx3].y, di[yx3], lc, linc, vxc);
							printf("x = %d y = %d \n",yx1%w, yx1/w);
							print_around(con, yx1, w);
							print_around(di, yx1, w);
							print_around(con, yx3, w);
							print_around(di, yx3, w);
							*/
							yx1 = yx; x1 = x; y1 = y;
							//return lc;
							break;
							//return 0;
						}
						if(con[yx1] > 253) {
							if(con[yx1] == 255)  {
								new_vertex1(di[yx1], &vx[yx1], &vp[vxc++], &vp1[lnc+=8], x1, y1, w);
								nd1 = add_finish_dir(&vx[yx1], -d, w); //vx[yx1].n++;
								con[yx1] = 254;
								//printf("New vertex \n");
							} else {
								nd1 = add_finish_dir(&vx[yx1], -d, w); //vx[yx1].n++;
								//printf("New direction \n");
								//print_around(di, yx1, w);
							}
							//rc[0] = rc[0]/cc; rc[1] = rc[1]/cc; rc[2] = rc[2]/cc;
							pow = pow/cc;
							//printf("yx1 = %d yx2 = %d  nd2 = %d nd1 = %d pow = %d\n", yx1, yx2, nd2, nd1, pow);
							//printf(" vx[yx2].lp = %p vx[yx1].lp = %p \n", vx[yx2].lp, vx[yx1].lp);
							new_line(&vx[yx2], &vx[yx1], nd2, nd1, pow);
							//printf("New line \n");
							yx1 = yx; x1 = x; y1 = y;
							break;
						}
						//if(is_new_line2(d, &cn, &fs, &sc)){
						if(is_new_line3(d, &fs, &sc, &cfs, &csc, &ll)){
							lc++;
							yx1 -= d; x1 -= dx; y1 -= dy;
							new_in_line_vertex(&vx[yx1], &vp[vxc++], &vp1[lnc+=8], x1, y1, w);
							nd1 = add_finish_dir(&vx[yx1], -d1, w);
							//rc[0] = rc[0]/cc; rc[1] = rc[1]/cc; rc[2] = rc[2]/cc;
							pow = pow/cc;
							new_line(&vx[yx2], &vx[yx1], nd2, nd1, pow);

							nd2 = add_finish_dir(&vx[yx1], d, w);
							con[yx1] = 254; yx2 = yx1;
							fs = 0; sc = 0; cfs = 0; csc = 0; ll = 0;
							pow = 0; cc = 0;
							//printf("New line \n");
							//break;
						}
						pow += con[yx1];
						d1 = d; //dx1 = -dx; dy1 = -dy;
						dx = -dx; dy = -dy;
						get_next_dir1(di[yx1], &dx, &dy);
						//direction(con, w, yx1, &dx, &dy);
						//if(!dx && !dy) {
						//	printf("dx = %d dy = %d\n", dx1, dy1);
						//	direction1(con, w, yx1, &dx, &dy, -d1);
						//	return 0;
						//}
						//con[yx1] = 0;
					}
				}
			}
		}
	}

	printf("Numbers of vertexs  = %d\n", vxc);
	//printf("Numbers of lines    = %d\n", linc);

	for(j=0; j < vxc; j++) {
		//printf("%d %p \n", j, vp[j]);
		if(vp[j]->di != vp[j]->cn ) {
			printf("%d  x = %d y = %d %o %o %o n = %d\n", j, vp[j]->x,  vp[j]->y, vp[j]->di, vp[j]->cn, vp[j]->cn^vp[j]->di, vp[j]->n);
			//printf("%d n = %d %o %o %o\n", j, vp[j]->n, vp[j]->di, vp[j]->cn, vp[j]->cn^vp[j]->di);
			con[vp[j]->y*w + vp[j]->x-w] = 255;
			con[vp[j]->y*w + vp[j]->x-1] = 255;
			con[vp[j]->y*w + vp[j]->x+w] = 255;
			con[vp[j]->y*w + vp[j]->x+1] = 255;
		}
	}
	return vxc;
}

uint32 seg_vertex2(uint32 *con, uint8 *di, Vertex *vx, Vertex **vp, uint32 w, uint32 h)
{
	uint32 j, y, x, x1, y1, x2, y2, yx, yx1, yx2, yx3, yw, nd1, nd2, yxd, h1 = h-1, w1 = w-1;
	int vxc = 0, lnc = 0, pow, cc, lc;
	int d, d1, d2, dx, dy, fs, sc, cfs, csc, ll, ld, rd;
	Vertex **vp1 = &vp[w*h>>3];
	//int dx1, dy1;

	//printf("w = %d h = %d w*h>>4 = %d\n", w, h, w*h>>4 );
	for(y=1; y < h1; y++){
		yw = y*w;
		for(x=1; x < w1; x++){
			yx = yw + x;
			if(con[yx] == 255 || con[yx] == 254) { //New vertex
				x1 = x; y1 = y;
				if(con[yx] == 255) {
					new_vertex1(di[yx], &vx[yx], &vp[vxc++], &vp1[lnc+=8], x1, y1, w);
					con[yx] = 254;
				}
				yx1 = yx;

				while(get_next_dir(&vx[yx1], &dx, &dy, &nd2)){
					d2 = dx + w*dy;
					//printf("get_next_dir x = %d y = %d dx = %d dy = %d nd2 = %d di = %o cn = %o\n", vx[yx1].x, vx[yx1].y, dx, dy, nd2, vx[yx1].di, vx[yx1].cn);
					yx2 = yx1; yx3 = yx1;  x2 = x1; y2 = y1;
					lc = 0;
					//print_around(con, yx1, w);
					//print_around(di, yx1, w);
					//printf("y = %d x = %d con = %d d = %d w = %d di = %o\n", (yx1)/w, (yx1)%w, con[yx1], dx + w*dy, w, vx[yx1].di);
					// Variable for line construction.
					fs = 0; sc = 0; cfs = 0; csc = 0; ll = 0;
					//Find two perpendicular directions

					//rc[0] = r[yx1]; rc[1] = g[yx1]; rc[2] = b[yx1]; cc = 1;
					pow = 0; cc = 0;
					while(1){
						x1 += dx; y1 += dy;
						d = dx + w*dy;
						yx1 = yx1 + d;
						cc++;

						if(con[yx1]>>8) { // Remove dir
							yx1 = yx1 - d;
							if(con[yx1] == 254){
								remove_dir(&vx[yx1], d, w);
							} else {
								x1 -= dx; y1 -= dy;
								new_vertex1(0, &vx[yx1], &vp[vxc++], &vp1[lnc+=8], x1, y1, w);
								nd1 = add_finish_dir(&vx[yx1], d, w);
								con[yx1] = 254;
								pow = pow/cc;
								new_line(&vx[yx2], &vx[yx1], nd2, nd1, pow);
							}
							//printf("New line \n");
							yx1 = yx; x1 = x; y1 = y;
							break;
						}

						//print_around32(con, yx1, w);
						//print_around(di, yx1, w);
						//printf("y = %d x = %d con = %d d = %d w = %d yx1 = %d\n", (yx1)/w, (yx1)%w, con[yx1], -d, w, yx1);
						if(con[yx1] > 253) {
							if(con[yx1] == 255)  {
								new_vertex1(di[yx1], &vx[yx1], &vp[vxc++], &vp1[lnc+=8], x1, y1, w);
								nd1 = add_finish_dir(&vx[yx1], -d, w); //vx[yx1].n++;
								con[yx1] = 254;
								//printf("New vertex \n");
							} else {
								nd1 = add_finish_dir(&vx[yx1], -d, w); //vx[yx1].n++;
								//printf("New direction \n");
								//print_around(di, yx1, w);
							}
							//rc[0] = rc[0]/cc; rc[1] = rc[1]/cc; rc[2] = rc[2]/cc;
							pow = pow/cc;
							//printf("yx1 = %d yx2 = %d  nd2 = %d nd1 = %d pow = %d\n", yx1, yx2, nd2, nd1, pow);
							//printf(" vx[yx2].lp = %p vx[yx1].lp = %p \n", vx[yx2].lp, vx[yx1].lp);
							new_line(&vx[yx2], &vx[yx1], nd2, nd1, pow);
							//printf("New line \n");
							yx1 = yx; x1 = x; y1 = y;
							break;
						}
						//if(is_new_line2(d, &cn, &fs, &sc)){
						if(is_new_line3(d, &fs, &sc, &cfs, &csc, &ll)){
							lc++;
							yx1 -= d; x1 -= dx; y1 -= dy;
							new_in_line_vertex(&vx[yx1], &vp[vxc++], &vp1[lnc+=8], x1, y1, w);
							nd1 = add_finish_dir(&vx[yx1], -d1, w);
							//rc[0] = rc[0]/cc; rc[1] = rc[1]/cc; rc[2] = rc[2]/cc;
							pow = pow/cc;
							new_line(&vx[yx2], &vx[yx1], nd2, nd1, pow);

							nd2 = add_finish_dir(&vx[yx1], d, w);
							con[yx1] = 254; yx2 = yx1;
							fs = 0; sc = 0; cfs = 0; csc = 0; ll = 0;
							pow = 0; cc = 0;
							//printf("New line \n");
							//break;
						}
						pow += con[yx1];
						d1 = d; //dx1 = -dx; dy1 = -dy;
						dx = -dx; dy = -dy;
						get_next_dir1(di[yx1], &dx, &dy);
					}
				}
			}
		}
	}

	printf("Numbers of vertexs  = %d\n", vxc);
	//printf("Numbers of lines    = %d\n", linc);

	for(j=0; j < vxc; j++) {
		//printf("%d %p \n", j, vp[j]);
		if(vp[j]->di != vp[j]->cn ) {
			printf("%d  x = %d y = %d %o %o %o n = %d\n", j, vp[j]->x,  vp[j]->y, vp[j]->di, vp[j]->cn, vp[j]->cn^vp[j]->di, vp[j]->n);
			//printf("%d n = %d %o %o %o\n", j, vp[j]->n, vp[j]->di, vp[j]->cn, vp[j]->cn^vp[j]->di);
			con[vp[j]->y*w + vp[j]->x-w] = 255;
			con[vp[j]->y*w + vp[j]->x-1] = 255;
			con[vp[j]->y*w + vp[j]->x+w] = 255;
			con[vp[j]->y*w + vp[j]->x+1] = 255;
		}
	}

	return vxc;
}

static inline uint8 find_vertex(uint8 *con, uint8 *di, int8 *buf, int *dx, int *dy, int *d, int *d1, uint32 *x, uint32 *y, uint32 *cc, uint32 *pow, uint32 w)
{
	Vector v;
	uint32 x1 = *x, y1 = *y, yx = *x + (*y)*w, yx1 = *x + *dx + (*y + *dy)*w;
	int fs = 0, sc = 0, cfs = 0, csc = 0, ll = 0;
	*cc = 0, *pow = 0;

	while(1){
		*(buf++) = -(*dx); *(buf++) = -(*dy);
		//printf("x = %d y = %d\n", -(*dx), -(*dy));
		*x += *dx; *y += *dy;
		*d = *dx + w*(*dy);
		yx = yx + *d;
		(*cc)++;
		//printf("in  buf = %d\n", *buf);
		v.x1 = x1; v.y1 = y1;
		v.x2 = *x; v.y2 = *y;
		//printf("dx = %d dy = %d\n", *dx, *dy);
		if(yx1 != check_next_pixel(&v, w)) return 0;
		if(con[yx] == 255) return 255;
		if(con[yx] == 254) return 254;
		if(is_new_line3(*d, &fs, &sc, &cfs, &csc, &ll)) return 0;


		*pow += con[yx];
		*d1 = *d; //dx1 = -dx; dy1 = -dy;
		//dx = -dx; dy = -dy;
		//*(buf++) = di[yx - *d];


		//*(buf++) = (di[yx - *d]<<4) + (di[yx - *d]>>4);
		//printf("buf = %d %d\n", di[yx - *d], *buf);
		get_next_dir1(di[yx], dx, dy);
	}
}

static inline uint8 find_vertex_back(uint8 *con, int8 *buf, int *dx, int *dy, int *d, int *d1, uint32 *x, uint32 *y, uint32 *cc, uint32 *pow, uint32 w)
{
	Vector v;
	uint32 x1 = *x, y1 = *y, yx = *x + (*y)*w, yx1 = *x + *dx + (*y + *dy)*w, i = 0;
	*cc = 0, *pow = 0;

	while(1){
		//printf("x = %d y = %d\n", *dx, *dy);
		*x += *dx; *y += *dy;
		*d = *dx + w*(*dy);
		yx = yx + *d;
		(*cc)++;
		//printf("dx = %d dy = %d\n", *dx, *dy);
		//printf("out buf = %d\n", *buf);
		v.x1 = x1; v.y1 = y1;
		v.x2 = *x; v.y2 = *y;
		if(yx1 != check_next_pixel(&v, w)) return 0;
		if(con[yx] == 255) return 255;
		if(con[yx] == 254) return 254;

		*pow += con[yx];
		*d1 = *d; //dx1 = -dx; dy1 = -dy;
		//dx = -dx; dy = -dy;

		//get_next_dir1(*(buf--), dx, dy);
		*dy = *(buf--); *dx = *(buf--);

	}
}

uint32 seg_vertex3(uint8 *con, uint8 *di, Vertex *vx, Vertex **vp, int8 *buf, uint32 w, uint32 h)
{
	uint32 j, y, x, x1, y1, x2, y2, yx, yx1, yx2, yx3, yxn, yw, nd, nd1, nd2, yxd, h1 = h-1, w1 = w-1;
	int vxc = 0, lnc = 0, pow, cc, lc, bl;//, lcn;
	int d, d1, d2, dx, dy, fs, sc, cfs, csc, ll, ld, rd, tmp, tmp1, chk;
	Vertex **vp1 = &vp[w*h>>3];
	Vector v;

	//printf("w = %d h = %d w*h>>4 = %d\n", w, h, w*h>>4 );
	for(y=1; y < h1; y++){
		yw = y*w;
		for(x=1; x < w1; x++){
			yx = yw + x;
			if(con[yx] == 255 || con[yx] == 254) { //New vertex
				x1 = x; y1 = y;
				if(con[yx] == 255) {
					new_vertex1(di[yx], &vx[yx], &vp[vxc++], &vp1[lnc+=8], x1, y1, w);
					con[yx] = 254;
				}
				yx1 = yx;

				while(get_next_dir(&vx[yx1], &dx, &dy, &nd2)){
					d2 = dx + w*dy;
					//printf("get_next_dir x = %d y = %d dx = %d dy = %d nd2 = %d di = %o cn = %o\n", vx[yx1].x, vx[yx1].y, dx, dy, nd2, vx[yx1].di, vx[yx1].cn);
					yx2 = yx1;  x2 = x1; y2 = y1; yx3 = yx1 + d2;
					lc = 0; fs = 0; sc = 0; cfs = 0; csc = 0; ll = 0;
					pow = 0; cc = 0; bl = 0;
					//lcn = get_next_vertex(con, di, yx1, nd2);

					while (1){
						//printf("start chk = %d x = %d y = %d\n", chk, x1, y1);
						chk = find_vertex(con, di, &buf[bl], &dx, &dy, &d, &d1, &x1, &y1, &cc, &pow, w);
						bl += (cc<<1);
						//printf("end chk = %d x = %d y = %d cc = %d\n", chk, x1, y1, cc);
						yx1 = x1 + y1*w;
						if(chk > 253){
							if(chk == 255)  {
								new_vertex1(di[yx1], &vx[yx1], &vp[vxc++], &vp1[lnc+=8], x1, y1, w);
								nd1 = add_finish_dir(&vx[yx1], -d, w); //vx[yx1].n++;
								con[yx1] = 254;
								//printf("New vertex \n");
							} else if (chk == 254){
								nd1 = add_finish_dir(&vx[yx1], -d, w); //vx[yx1].n++;
								//printf("New direction \n");
								//print_around(di, yx1, w);
							}
							/*
							v.x1 = x2; v.y1 = y2;
							v.x2 = x1; v.y2 = y1;
							tmp = check_next_pixel(&v, w);
							if(yx3 != tmp){
								printf("fist tmp x = %d y = %d x1 = %d y1 = %d x2 = %d y2 = %d lc = %d nd2 = %d d2 = %d w = %d\n",
										tmp%w, tmp/w, x2, y2, x1, y1, lc, nd2, d2, w);
							}*/

							v.x1 = x1; v.y1 = y1;
							v.x2 = x2; v.y2 = y2;
							tmp = check_next_pixel(&v, w);

							if(yx1-d != tmp){
								//printf("lc = %d\n", lc);
								yx3 = yx1;
								//printf("second  tmp x = %d y = %d x1 = %d y1 = %d x2 = %d y2 = %d lc = %d nd2 = %d d2 = %d w = %d\n",
								//		tmp%w, tmp/w, x2, y2, x1, y1, lc, nd2, d2, w);
								//} else {
									pow = 0; cc = 0;
									dx = -dx; dy = -dy;

									tmp1 = x1 + dx + (y1 + dy)*w;
									//printf("back start chk = %d x = %d y = %d\n", chk, x1, y1);
									v.x1 = x1; v.y1 = y1;
									chk = find_vertex_back(con, &buf[bl-3], &dx, &dy, &d, &d1, &x1, &y1, &cc, &pow, w);

									//printf("back end chk = %d x = %d y = %d\n", chk, x1, y1);
									yx1 = x1 + y1*w;

									if(chk > 253){
										printf("Back to vertecs\n");
										printf("yx1 = %d yx2 = %d yx3 = %d cc = %d\n", yx1, yx2, yx3, cc);

											//printf("yx1 = %d yx2 = %d yx3 = %d cc = %d\n", yx1, yx2, yx3, cc);
											con[yx1-w] = 255;
											con[yx1-1] = 255;
											con[yx1+w] = 255;
											con[yx1+1] = 255;
											con[yx2-w] = 128;
											con[yx2-1] = 128;
											con[yx2+w] = 128;
											con[yx2+1] = 128;
											con[yx3-w] = 200;
											con[yx3-1] = 200;
											con[yx3+w] = 200;
											con[yx3+1] = 200;

										return;
										break;
									} else {
										yx1 -= d; x1 -= dx; y1 -= dy;
										//---------------Test-------------------
										v.x2 = x1; v.y2 = y1;
										tmp = check_next_pixel(&v, w);
										if(tmp != tmp1){
											printf("Not right line x1 = %d y1 = %d x2 = %d y2 = %d \n", tmp%w, tmp/w, tmp1%w, tmp1/w);

										}


										new_vertex1(di[yx1], &vx[yx1], &vp[vxc++], &vp1[lnc+=8], x1, y1, w);
										nd = add_finish_dir(&vx[yx1], -d1, w);
										//rc[0] = rc[0]/cc; rc[1] = rc[1]/cc; rc[2] = rc[2]/cc;
										pow = pow/cc;
										new_line(&vx[yx3], &vx[yx1], nd1, nd, pow);

										nd = add_finish_dir(&vx[yx1], d, w);
										new_line(&vx[yx2], &vx[yx1], nd2, nd, pow);

										con[yx1] = 254;
										//bl+=cc;
										yx1 = yx; x1 = x; y1 = y;
										//return 0;
										break;
										//yx2 = yx1; x2 = x1; y2 = y1;
										//pow = 0; cc = 0;
									}
								//}
							} else {
								pow = pow/cc;
								//printf("yx1 = %d yx2 = %d  nd2 = %d nd1 = %d pow = %d\n", yx1, yx2, nd2, nd1, pow);
								//printf(" vx[yx2].lp = %p vx[yx1].lp = %p \n", vx[yx2].lp, vx[yx1].lp);
								new_line(&vx[yx2], &vx[yx1], nd2, nd1, pow);
								//printf("New line \n");
								yx1 = yx; x1 = x; y1 = y;
								break;
							}
						} else {
							//yx3 = yx1;
							//bl+=cc;
							lc++;
							yx1 -= d; x1 -= dx; y1 -= dy;
							//printf("dx = %d dy = %d x = %d y = %d\n", dx, dy, x1, y1);
							//yx1 = yx1 - d; x1 = x1 - dx; y1 = y1 - dy;
							//new_in_line_vertex(&vx[yx1], &vp[vxc++], &vp1[lnc+=8], x1, y1, w);
							new_vertex1(di[yx1], &vx[yx1], &vp[vxc++], &vp1[lnc+=8], x1, y1, w);
							nd1 = add_finish_dir(&vx[yx1], -d1, w);
							//rc[0] = rc[0]/cc; rc[1] = rc[1]/cc; rc[2] = rc[2]/cc;
							pow = pow/cc;
							new_line(&vx[yx2], &vx[yx1], nd2, nd1, pow);

							nd2 = add_finish_dir(&vx[yx1], d, w);
							con[yx1] = 254;

							yx2 = yx1; x2 = x1; y2 = y1;
							pow = 0; cc = 0;
						}
					}
					/*
					while(1){
						x1 += dx; y1 += dy;
						d = dx + w*dy;
						yx1 = yx1 + d;
						cc++;

						//print_around(con, yx1, w);
						//printf("x = %d y = %d con = %d d = %d w = %d yx1 = %d\n", (yx1)%w, (yx1)/w, con[yx1], -d, w, yx1);
						if(con[yx1] > 253) {

							if(con[yx1] == 255)  {
								new_vertex1(di[yx1], &vx[yx1], &vp[vxc++], &vp1[lnc+=8], x1, y1, w);
								nd1 = add_finish_dir(&vx[yx1], -d, w); //vx[yx1].n++;
								con[yx1] = 254;
								//printf("New vertex \n");
							} else {
								nd1 = add_finish_dir(&vx[yx1], -d, w); //vx[yx1].n++;
								//printf("New direction \n");
								//print_around(di, yx1, w);
							}
							//rc[0] = rc[0]/cc; rc[1] = rc[1]/cc; rc[2] = rc[2]/cc;
							//Check if any lines in boder
							v.x1 = x1; v.y1 = y1;
							v.x2 = x2; v.y2 = y2;
							tmp = check_next_pixel(&v, w);
							if(yx1-d != tmp){
								if(lc){
									printf("lc = %d\n", lc);
								} else {

								}
							} else {
								pow = pow/cc;
								//printf("yx1 = %d yx2 = %d  nd2 = %d nd1 = %d pow = %d\n", yx1, yx2, nd2, nd1, pow);
								//printf(" vx[yx2].lp = %p vx[yx1].lp = %p \n", vx[yx2].lp, vx[yx1].lp);
								new_line(&vx[yx2], &vx[yx1], nd2, nd1, pow);
								//printf("New line \n");
								yx1 = yx; x1 = x; y1 = y;
								break;
							}
						}
						//if(is_new_line2(d, &cn, &fs, &sc)){
						v.x1 = x2; v.y1 = y2;
						v.x2 = x1; v.y2 = y1;
						tmp = check_next_pixel(&v, w);
						//printf("yx3 x = %d y = %d tmp x = %d y = %d\n", yx3%w, yx3/w, tmp%w, tmp/w);

						//if(yx3 != check_next_pixel(&v, w)){
						if(yx3 != tmp){
							//return;
							lc++;
							yx3 = yx1;
							yx1 -= d; x1 -= dx; y1 -= dy;
							//new_in_line_vertex(&vx[yx1], &vp[vxc++], &vp1[lnc+=8], x1, y1, w);
							new_vertex1(di[yx1], &vx[yx1], &vp[vxc++], &vp1[lnc+=8], x1, y1, w);
							nd1 = add_finish_dir(&vx[yx1], -d1, w);
							//rc[0] = rc[0]/cc; rc[1] = rc[1]/cc; rc[2] = rc[2]/cc;
							pow = pow/cc;
							new_line(&vx[yx2], &vx[yx1], nd2, nd1, pow);

							nd2 = add_finish_dir(&vx[yx1], d, w);
							con[yx1] = 254;
							bl+=cc;
							yx2 = yx1; x2 = x1; y2 = y1;
							fs = 0; sc = 0; cfs = 0; csc = 0; ll = 0;
							pow = 0; cc = 0;

						}
						pow += con[yx1];
						d1 = d; //dx1 = -dx; dy1 = -dy;
						dx = -dx; dy = -dy;
						get_next_dir1(di[yx1], &dx, &dy);
					}*/

				}
			}
		}
	}

	printf("Numbers of vertexs  = %d\n", vxc);
	//printf("Numbers of lines    = %d\n", linc);

	for(j=0; j < vxc; j++) {
		//printf("%d %p \n", j, vp[j]);
		if(vp[j]->di != vp[j]->cn ) {
			printf("%d  x = %d y = %d %o %o %o n = %d\n", j, vp[j]->x,  vp[j]->y, vp[j]->di, vp[j]->cn, vp[j]->cn^vp[j]->di, vp[j]->n);
			//printf("%d n = %d %o %o %o\n", j, vp[j]->n, vp[j]->di, vp[j]->cn, vp[j]->cn^vp[j]->di);
			con[vp[j]->y*w + vp[j]->x-w] = 255;
			con[vp[j]->y*w + vp[j]->x-1] = 255;
			con[vp[j]->y*w + vp[j]->x+w] = 255;
			con[vp[j]->y*w + vp[j]->x+1] = 255;
		}
	}

	return vxc;
}

static inline uint32 check_in_line(Vertex *vx, uint8 *d1, uint8 *d2)
{
	if      (!(vx->di^0x88)) { *d1 = 0; *d2 = 4; return 1; }
	else if (!(vx->di^0x44)) { *d1 = 1; *d2 = 5; return 1; }
	else if (!(vx->di^0x22)) { *d1 = 2; *d2 = 6; return 1; }
	else if (!(vx->di^0x11)) { *d1 = 3; *d2 = 7; return 1; }
	return 0;
}

static inline Vertex**  find_pointer(Vertex *vx1, Vertex *vx2)
{
	if		(vx1->vp[0] == vx2) return &vx1->vp[0];
	else if	(vx1->vp[1] == vx2) return &vx1->vp[1];
	else if	(vx1->vp[2] == vx2) return &vx1->vp[2];
	else if	(vx1->vp[3] == vx2) return &vx1->vp[3];
	else if	(vx1->vp[4] == vx2) return &vx1->vp[4];
	else if	(vx1->vp[5] == vx2) return &vx1->vp[5];
	else if	(vx1->vp[6] == vx2) return &vx1->vp[6];
	else if	(vx1->vp[7] == vx2) return &vx1->vp[7];
}

static inline uint32 get_dir2(Vertex *vx, uint8 *nd)
{
	if(!(vx->di - vx->cn)) return 0;
	if		((vx->di&128) && !(vx->cn&128)) { vx->cn |= 128;	*nd = 0; 	return 1; }
	else if	((vx->di&64 ) && !(vx->cn&64 )) { vx->cn |= 64;		*nd = 1;  	return 1; }
	else if	((vx->di&32 ) && !(vx->cn&32 )) { vx->cn |= 32;		*nd = 2;  	return 1; }
	else if ((vx->di&16 ) && !(vx->cn&16 )) { vx->cn |= 16;		*nd = 3;  	return 1; }
	else if ((vx->di&8  ) && !(vx->cn&8  )) { vx->cn |= 8;		*nd = 4;  	return 1; }
	else if ((vx->di&4  ) && !(vx->cn&4  )) { vx->cn |= 4;		*nd = 5;  	return 1; }
	else if ((vx->di&2  ) && !(vx->cn&2  )) { vx->cn |= 2;		*nd = 6;  	return 1; }
	else if ((vx->di&1  ) && !(vx->cn&1  )) { vx->cn |= 1;		*nd = 7;  	return 1; }
	return 0;
}

static inline uint32 is_dir(Vertex *vx)
{
	if(!(vx->di - vx->cn)) return 0;
	if		((vx->di&128) && !(vx->cn&128)) return 1;
	else if	((vx->di&64 ) && !(vx->cn&64 )) return 1;
	else if	((vx->di&32 ) && !(vx->cn&32 )) return 1;
	else if ((vx->di&16 ) && !(vx->cn&16 )) return 1;
	else if ((vx->di&8  ) && !(vx->cn&8  )) return 1;
	else if ((vx->di&4  ) && !(vx->cn&4  )) return 1;
	else if ((vx->di&2  ) && !(vx->cn&2  )) return 1;
	else if ((vx->di&1  ) && !(vx->cn&1  )) return 1;
	return 0;
}

static inline uint32 get_dir_clock(Vertex *vx, uint8 *nd)
{
	if(!(vx->di - vx->cn)) return 0;
	if		((vx->di&128) && !(vx->cn&128)) { vx->cn |= 128;	*nd = 0; 	return 1; }
	else if ((vx->di&1  ) && !(vx->cn&1  )) { vx->cn |= 1;		*nd = 7;  	return 1; }
	else if ((vx->di&2  ) && !(vx->cn&2  )) { vx->cn |= 2;		*nd = 6;  	return 1; }
	else if ((vx->di&4  ) && !(vx->cn&4  )) { vx->cn |= 4;		*nd = 5;  	return 1; }
	else if ((vx->di&8  ) && !(vx->cn&8  )) { vx->cn |= 8;		*nd = 4;  	return 1; }
	else if ((vx->di&16 ) && !(vx->cn&16 )) { vx->cn |= 16;		*nd = 3;  	return 1; }
	else if	((vx->di&32 ) && !(vx->cn&32 )) { vx->cn |= 32;		*nd = 2;  	return 1; }
	else if	((vx->di&64 ) && !(vx->cn&64 )) { vx->cn |= 64;		*nd = 1;  	return 1; }
	return 0;
}

static inline uint32 get_dir3(Vertex *vx, uint8 *nd)
{
	if(!(vx->di - vx->cn)) return 0;
	if		((vx->di&128) && !(vx->cn&128)) { *nd = 0; return 1; }
	else if	((vx->di&64 ) && !(vx->cn&64 )) { *nd = 1; return 1; }
	else if	((vx->di&32 ) && !(vx->cn&32 )) { *nd = 2; return 1; }
	else if ((vx->di&16 ) && !(vx->cn&16 )) { *nd = 3; return 1; }
	else if ((vx->di&8  ) && !(vx->cn&8  )) { *nd = 4; return 1; }
	else if ((vx->di&4  ) && !(vx->cn&4  )) { *nd = 5; return 1; }
	else if ((vx->di&2  ) && !(vx->cn&2  )) { *nd = 6; return 1; }
	else if ((vx->di&1  ) && !(vx->cn&1  )) { *nd = 7; return 1; }
	return 0;
}

static inline uint8 get_same_dir(Vertex *vx, uint8 nd)
{
	if (nd == 0) {
		if(vx->di&128) { vx->cn |= 128; return nd; }
		else { vx->cn |= 2; return 6;}
	}
	if (nd == 2) {
		if(vx->di&32) { vx->cn |= 32; return nd; }
		else { vx->cn |= 128; return 0; }
	}
	if (nd == 4) {
		if(vx->di&8) { vx->cn |= 8;  return nd; }
		else { vx->cn |= 32; return 2; }
	}
	if (nd == 6) {
		if(vx->di&2) { vx->cn |= 2; return nd; }
		else { vx->cn |= 8; return 4; }
	}
}

static inline void remove_dir1(Vertex *vx, uint8 nd)
{
	vx->n--;
	if      (nd == 0) { vx->di ^= 128;    vx->cn ^= 128; }
	else if (nd == 1) { vx->di ^= 64;	  vx->cn ^= 64;  }
	else if (nd == 2) { vx->di ^= 32;     vx->cn ^= 32;  }
	else if (nd == 3) { vx->di ^= 16;     vx->cn ^= 16;  }
	else if (nd == 4) { vx->di ^= 8;      vx->cn ^= 8; 	 }
	else if (nd == 5) { vx->di ^= 4;      vx->cn ^= 4; 	 }
	else if (nd == 6) { vx->di ^= 2;      vx->cn ^= 2;   }
	else if (nd == 7) { vx->di ^= 1;      vx->cn ^= 1;   }
        //printf("finish d = %d %o %o\n", d, vx->di, vx->cn);
}

static inline void remove_dir2(Vertex *vx, uint8 nd)
{
	Vertex *vx1 = vx->vp[nd];
    //printf("1finish nd = %d %o %o n = %d %p\n", nd, vx->di, vx->cn, vx->n, vx);
	vx->n--; vx1->n--;
	if      (nd == 0) { vx->di ^= 128;    vx->cn ^= 128; }
	else if (nd == 1) { vx->di ^= 64;	  vx->cn ^= 64;  }
	else if (nd == 2) { vx->di ^= 32;     vx->cn ^= 32;  }
	else if (nd == 3) { vx->di ^= 16;     vx->cn ^= 16;  }
	else if (nd == 4) { vx->di ^= 8;      vx->cn ^= 8; 	 }
	else if (nd == 5) { vx->di ^= 4;      vx->cn ^= 4; 	 }
	else if (nd == 6) { vx->di ^= 2;      vx->cn ^= 2; 	 }
	else if (nd == 7) { vx->di ^= 1;      vx->cn ^= 1; 	 }

	if		(vx1->vp[0] == vx) { vx1->di ^= 128;   vx1->cn ^= 128;	}// 	vx1->vp[0] = NULL; }
	else if	(vx1->vp[1] == vx) { vx1->di ^= 64;	   vx1->cn ^= 64; 	}//	vx1->vp[1] = NULL; }
	else if	(vx1->vp[2] == vx) { vx1->di ^= 32;    vx1->cn ^= 32; 	}//	vx1->vp[2] = NULL; }
	else if	(vx1->vp[3] == vx) { vx1->di ^= 16;    vx1->cn ^= 16; 	}//	vx1->vp[3] = NULL; }
	else if	(vx1->vp[4] == vx) { vx1->di ^= 8;     vx1->cn ^= 8; 	}//	vx1->vp[4] = NULL; }
	else if	(vx1->vp[5] == vx) { vx1->di ^= 4;     vx1->cn ^= 4; 	}// 	vx1->vp[5] = NULL; }
	else if	(vx1->vp[6] == vx) { vx1->di ^= 2;     vx1->cn ^= 2; 	}//	vx1->vp[6] = NULL; }
	else if	(vx1->vp[7] == vx) { vx1->di ^= 1;     vx1->cn ^= 1; 	}//	vx1->vp[7] = NULL; }

	//vx->vp[nd] = vx->vp[2];

    //printf("2finish nd = %d %o %o\n", nd, vx->di, vx->cn);
}

static inline uint32 finish_dir1(Vertex *vx, uint8 nd)
{
	if (nd == 0) {
		if(vx->cn&128) return 1;
		else { vx->cn |= 128; return 0;}
	}
	if (nd == 1) {
		if(vx->cn&64) return 1;
		else { vx->cn |= 64; return 0;}
	}
	if (nd == 2) {
		if(vx->cn&32) return 1;
		else { vx->cn |= 32; return 0;}
	}
	if (nd == 3) {
		if(vx->cn&16) return 1;
		else { vx->cn |= 16; return 0;}
	}
	if (nd == 4) {
		if(vx->cn&8) return 1;
		else { vx->cn |= 8; return 0;}
	}
	if (nd == 5) {
		if(vx->cn&4) return 1;
		else { vx->cn |= 4; return 0;}
	}
	if (nd == 6) {
		if(vx->cn&2) return 1;
		else { vx->cn |= 2; return 0;}
	}
	if (nd == 7) {
		if(vx->cn&1) return 1;
		else { vx->cn |= 1; return 0;}
	}
        //printf("finish d = %d %o %o\n", d, vx->di, vx->cn);
}

static inline void finish_dir2(Vertex *vx, uint8 nd)
{
	if 		(nd == 0) { vx->cn |= 128; 	if(!(vx->di&128)) 	{ vx->di |= 128; 	vx->n++; } }
	else if (nd == 1) { vx->cn |= 64; 	if(!(vx->di&64)) 	{ vx->di |= 64; 	vx->n++; } }
	else if (nd == 2) { vx->cn |= 32; 	if(!(vx->di&32)) 	{ vx->di |= 32; 	vx->n++; } }
	else if (nd == 3) { vx->cn |= 16; 	if(!(vx->di&16)) 	{ vx->di |= 16; 	vx->n++; } }
	else if (nd == 4) { vx->cn |= 8; 	if(!(vx->di&8)) 	{ vx->di |= 8; 		vx->n++; } }
	else if (nd == 5) { vx->cn |= 4; 	if(!(vx->di&4)) 	{ vx->di |= 4; 		vx->n++; } }
	else if (nd == 6) { vx->cn |= 2; 	if(!(vx->di&2)) 	{ vx->di |= 2; 		vx->n++; } }
	else if (nd == 7) { vx->cn |= 1; 	if(!(vx->di&1)) 	{ vx->di |= 1; 		vx->n++; } }
        //printf("finish d = %d %o %o\n", d, vx->di, vx->cn);
}

static inline uint8  find_pointer1(Vertex *vx1, Vertex *vx2)
{
	/*
	if		(vx1->di&128 && vx1->vp[0] == vx2) return 0;
	else if	(vx1->di&64  && vx1->vp[1] == vx2) return 1;
	else if	(vx1->di&32  && vx1->vp[2] == vx2) return 2;
	else if	(vx1->di&16  && vx1->vp[3] == vx2) return 3;
	else if	(vx1->di&8   && vx1->vp[4] == vx2) return 4;
	else if	(vx1->di&4   && vx1->vp[5] == vx2) return 5;
	else if	(vx1->di&2   && vx1->vp[6] == vx2) return 6;
	else if	(vx1->di&1   && vx1->vp[7] == vx2) return 7;
	*/
	if		(vx1->vp[0] == vx2) return 0;
	else if	(vx1->vp[1] == vx2) return 1;
	else if	(vx1->vp[2] == vx2) return 2;
	else if	(vx1->vp[3] == vx2) return 3;
	else if	(vx1->vp[4] == vx2) return 4;
	else if	(vx1->vp[5] == vx2) return 5;
	else if	(vx1->vp[6] == vx2) return 6;
	else if	(vx1->vp[7] == vx2) return 7;
}

static inline uint8 get_clockwise_dir(Vertex *vx, uint8 nd, uint8 *nd1)
{
	uint8 tm, i = 0;
	if(!(vx->di - vx->cn)) return 0;

	if      (nd == 0) { vx->cn |= 128; goto m1; }
	else if (nd == 1) { vx->cn |= 64;  goto m8; }
	else if (nd == 2) { vx->cn |= 32;  goto m7; }
	else if (nd == 3) { vx->cn |= 16;  goto m6; }
	else if (nd == 4) { vx->cn |= 8;   goto m5; }
	else if (nd == 5) { vx->cn |= 4;   goto m4; }
	else if (nd == 6) { vx->cn |= 2;   goto m3; }
	else if (nd == 7) { vx->cn |= 1;   goto m2; }
	//return 0;

	m1:	if ((vx->di&1  ) && !(vx->cn&1  )) { *nd1 = 7; vx->cn |= 1;		return 1; }
	m2:	if ((vx->di&2  ) && !(vx->cn&2  )) { *nd1 = 6; vx->cn |= 2;		return 1; }
	m3:	if ((vx->di&4  ) && !(vx->cn&4  )) { *nd1 = 5; vx->cn |= 4;		return 1; }
	m4:	if ((vx->di&8  ) && !(vx->cn&8  )) { *nd1 = 4; vx->cn |= 8;		return 1; }
	m5:	if ((vx->di&16 ) && !(vx->cn&16 )) { *nd1 = 3; vx->cn |= 16;	return 1; }
	m6:	if ((vx->di&32 ) && !(vx->cn&32 )) { *nd1 = 2; vx->cn |= 32;	return 1; }
	m7:	if ((vx->di&64 ) && !(vx->cn&64 )) { *nd1 = 1; vx->cn |= 64;	return 1; }
	m8:	if ((vx->di&128) && !(vx->cn&128)) { *nd1 = 0; vx->cn |= 128; 	return 1; }
		if ((vx->di&1  ) && !(vx->cn&1  )) { *nd1 = 7; vx->cn |= 1;		return 1; }
		if ((vx->di&2  ) && !(vx->cn&2  )) { *nd1 = 6; vx->cn |= 2;		return 1; }
		if ((vx->di&4  ) && !(vx->cn&4  )) { *nd1 = 5; vx->cn |= 4;		return 1; }
		if ((vx->di&8  ) && !(vx->cn&8  )) { *nd1 = 4; vx->cn |= 8;		return 1; }
		if ((vx->di&16 ) && !(vx->cn&16 )) { *nd1 = 3; vx->cn |= 16;	return 1; }
		if ((vx->di&32 ) && !(vx->cn&32 )) { *nd1 = 2; vx->cn |= 32;	return 1; }
		if ((vx->di&64 ) && !(vx->cn&64 )) { *nd1 = 1; vx->cn |= 64;	return 1; }
	return 0;
        //printf("finish d = %d %o %o\n", d, vx->di, vx->cn);
}

static inline uint8 get_clockwise_dir1(Vertex *vx, uint8 nd)
{
	if      (nd == 0) goto m1;
	else if (nd == 1) goto m8;
	else if (nd == 2) goto m7;
	else if (nd == 3) goto m6;
	else if (nd == 4) goto m5;
	else if (nd == 5) goto m4;
	else if (nd == 6) goto m3;
	else if (nd == 7) goto m2;
	//return 0;

	m1:	if (vx->di&1  ) return 7;
	m2:	if (vx->di&2  ) return 6;
	m3:	if (vx->di&4  ) return 5;
	m4:	if (vx->di&8  ) return 4;
	m5:	if (vx->di&16 ) return 3;
	m6:	if (vx->di&32 ) return 2;
	m7:	if (vx->di&64 ) return 1;
	m8:	if (vx->di&128) return 0;
		if (vx->di&1  ) return 7;
		if (vx->di&2  ) return 6;
		if (vx->di&4  ) return 5;
		if (vx->di&8  ) return 4;
		if (vx->di&16 ) return 3;
		if (vx->di&32 ) return 2;
		if (vx->di&64 ) return 1;
        //printf("finish d = %d %o %o\n", d, vx->di, vx->cn);
}

static inline uint8 get_counterclockwise_dir(Vertex *vx, uint8 nd)
{
	if      (nd == 0) goto m1;
	else if (nd == 1) goto m2;
	else if (nd == 2) goto m3;
	else if (nd == 3) goto m4;
	else if (nd == 4) goto m5;
	else if (nd == 5) goto m6;
	else if (nd == 6) goto m7;
	else if (nd == 7) goto m8;
	//return 0;

	m1:	if (vx->di&64 ) return 1;
	m2:	if (vx->di&32 ) return 2;
	m3:	if (vx->di&16 ) return 3;
	m4:	if (vx->di&8  ) return 4;
	m5:	if (vx->di&4  ) return 5;
	m6:	if (vx->di&2  ) return 6;
	m7:	if (vx->di&1  ) return 7;
	m8:	if (vx->di&128) return 0;
		if (vx->di&64 ) return 1;
		if (vx->di&32 ) return 2;
		if (vx->di&16 ) return 3;
		if (vx->di&8  ) return 4;
		if (vx->di&4  ) return 5;
		if (vx->di&2  ) return 6;
		if (vx->di&1  ) return 7;
}

static inline uint32 check_is_in_line(Vertex *vx1, Vertex *vx2, Vertex *vx3)
{
	int dx1 = vx2->x - vx1->x, dx2 = vx3->x - vx2->x;
	int dy1 = vx2->y - vx1->y, dy2 = vx3->y - vx2->y;
	if		(dx1 == 0 && dx2 == 0) return 1;
	else if	(dy1 == 0 && dy2 == 0) return 1;
	return 0;
}

uint32 seg_remove_virtex(Vertex **vp, uint32 vxc, uint32 w, uint32 h)
{
	uint32 i, j, k, vc = 0, tmp;
	uint8 nd, nd1;
	Vertex *vx, *vx1, *vx2;

	for(i=0; i < vxc; i++) vp[i]->cn = 0;

	for(i=0; i < vxc; i++) {
		//Remove all vertexes with one neighborhood
		if(vp[i]->n == 1){
			vx = vp[i];
			//printf("%4d vx->n = %d vx->di = %o vx->cn = %o p = %p\n", i, vx->n, vx->di, vx->cn, vx);
			while(get_dir2(vx, &nd)){
				//printf("nd = %d n = %d\n", nd, vx->vp[nd]->n);
				//remove_dir1(vx, nd);
				//remove_dir1(vx->vp[nd], find_pointer1(vx->vp[nd], vx));
				remove_dir2(vx, nd);
				vc++;
				if(vx->vp[nd]->n == 1){
					vx = vx->vp[nd];
					//printf("Next di = %o cn = %o n = %d p = %p\n", vx->di, vx->cn, vx->n, vx);
				} else break;
			}
		}
	}
	//Remove inline vertexes
	for(i=0; i < vxc; i++) {
		if(vp[i]->n == 2){
			vx = vp[i];
			get_dir2(vx, &nd);
			get_dir2(vx, &nd1);
			vx1 = vp[i]->vp[nd];
			vx2 = vp[i]->vp[nd1];
			if(check_is_in_line(vx1, vp[i], vx2)){
				//printf("INLINE \n", nd, vp[i]->n);
				vx->n = 0; vx->di = 0; vx->cn = 0;
				nd = find_pointer1(vx1, vp[i]);
				nd1 = find_pointer1(vx2, vp[i]);
				//finish_dir1(vx2, nd2);
				vx1->vp[nd] = vx2;
				vx2->vp[nd1] = vx1;
				//vx1 = vx2;
				vc++;
			}
		}
	}
	//Remove closed loop
	/*
	for(i=0; i < vxc; i++) {
		if(vp[i]->n > 1){
			tmp = 0;
			for(j=0; j < 8; j++){
				if(vp[i]->di&(1<<(7-j)) && vp[i]->vp[j]) {
					if(tmp){
						if (vp[i]->vp[j] == vp[i]->vp[tmp]){
							remove_dir2(vp[i], j);
							vc++;
						}
					}
					tmp = j;
				}
			}
			for(j=0; j < 8; j++){
				if(vp[i]->vp[j]) {
					if (vp[i]->di&(1<<(7-j)) && vp[i]->vp[j] == vp[i]->vp[tmp]){
						remove_dir2(vp[i], j);
						vc++;
					}
					break;
				}
			}
		}
	}*/
	printf("Numbers of removed vertexs  = %d\n", vc);
}

void seg_vertex_draw(uint8 *r, uint8 *g, uint8 *b, Vertex **vp, uint32 vxc, uint32 w)
{
	uint32 i, yx, vc = 0;
	Vector v;
	uint8 c[3], nd, nd1, nd2;
	Vertex *vx, *vx1, *vx2;

	for(i=0; i < vxc; i++) vp[i]->cn = 0;

	for(i=0; i < vxc; i++){
		if(vp[i]->n){
			vc++;
			vx = vp[i];
			//printf("%4d x = %4d y = %4d di = %d cn = %d nd = %d n = %d\n", i, vx->x, vx->y, vx->di, vx->cn, nd, vx->n);
			while(get_dir2(vx, &nd)){
				//printf("x = %4d y = %4d di = %d cn = %d nd = %d n = %d\n", vx->x, vx->y, vx->di, vx->cn, nd, vx->n);
				vx1 = vx->vp[nd];
				finish_dir1(vx, nd);
				finish_dir1(vx1, find_pointer1(vx1, vx));

				v.x1 =  vx->x; v.y1 =  vx->y;
				v.x2 =  vx1->x; v.y2 =  vx1->y;
				//printf("x1 = %4d x2 = %4d  y1 = %d y2 = %d \n", vx->x, vx1->y,vx->y, vx1->y);

				c[0] = 128; c[1] = 128; c[2] = 128;
				draw_line_3(r, g, b, &v, w, c);
				yx = v.y2*w + v.x2;
				r[yx] = 255; g[yx] = 255; b[yx] = 255;
			}

			yx = vp[i]->y*w + vp[i]->x;
			r[yx] = 255; g[yx] = 255; b[yx] = 255;
		}
	}

	for(i=0; i < vxc; i++) {
		//printf("%d %p \n", j, vp[j]);
		//if(vp[i]->di != vp[i]->cn ) {
		if(vp[i]->n == 1 || vp[i]->di != vp[i]->cn) {
			printf("%d  x = %d y = %d %o %o %o n = %d\n", i, vp[i]->x,  vp[i]->y, vp[i]->di, vp[i]->cn, vp[i]->cn^vp[i]->di, vp[i]->n);
			//printf("%d n = %d %o %o %o\n", i, vp[i]->n, vp[i]->di, vp[i]->cn, vp[i]->cn^vp[i]->di);
			r[vp[i]->y*w + vp[i]->x-w] = 255;
			r[vp[i]->y*w + vp[i]->x-1] = 255;
			r[vp[i]->y*w + vp[i]->x+w] = 255;
			r[vp[i]->y*w + vp[i]->x+1] = 255;
		}
	}

	printf("Numbers of drawing vertexs  = %d\n", vc);
}

void seg_vertex_draw1(uint8 *img, Vertex **vp, uint32 vxc, uint32 w, uint32 h, uint32 k)
{
	uint32 i, yx, vc = 0, w2 = (w-2)>>k, h2 = (h-2)>>k;
	Vector v;
	uint8  nd, nd1, nd2;
	Vertex *vx, *vx1, *vx2, *vp1;

	for(i=0; i < vxc; i++) vp[i]->cn = 0;

	for(i=0; i < vxc; i++){
		//If any problem may change if to while
		while(vp[i]->n > 1 && get_dir2(vp[i], &nd)){
			vx = vp[i];
			//vp1 = vp[i];

			//printf("%4d x = %4d y = %4d di = %d cn = %d nd = %d n = %d\n", i, vx->x, vx->y, vx->di, vx->cn, nd, vx->n);
			do{
				vc++;
				//printf("x = %4d y = %4d di = %d cn = %d nd = %d n = %d\n", vx->x, vx->y, vx->di, vx->cn, nd, vx->n);
				vx1 = vx->vp[nd];
				finish_dir1(vx, nd);
				nd1 =  find_pointer1(vx1, vx);
				finish_dir1(vx1, nd1);

				v.x1 = ((vx->x-1)<<k); 	v.y1 = ((vx->y-1)<<k);
				v.x2 = ((vx1->x-1)<<k); v.y2 = ((vx1->y-1)<<k);

				if(!k){
					v.x1 = v.x1+1; 	v.y1 = v.y1+1;
					v.x2 = v.x2+1; 	v.y2 = v.y2+1;
				} else {
					v.x1 = (vx->x) == w2 ? (w&1 ? v.x1+3 : v.x1+2): v.x1+1;
					v.y1 = (vx->y) == h2 ? (h&1 ? v.y1+3 : v.y1+2): v.y1+1;
					v.x2 = (vx1->x) == w2 ? (w&1 ? v.x2+3 : v.x2+2): v.x2+1;
					v.y2 = (vx1->y) == h2 ? (h&1 ? v.y2+3 : v.y2+2): v.y2+1;
				}
				//printf("x1 = %4d x2 = %4d  y1 = %d y2 = %d \n", vx->x, vx1->y,vx->y, vx1->y);

				//c[0] = 128; c[1] = 128; c[2] = 128;
				draw_line_1(img, &v, w, 128);
				yx = v.y2*w + v.x2;
				img[yx] = 255;
				yx = v.y1*w + v.x1;
				img[yx] = 255;
				if(!get_clockwise_dir(vx1, nd1, &nd)) break;
				vx = vx1;
			}while(vx != vp[i]);

			//yx = vp[i]->y*w + vp[i]->x;
			//img[yx] = 255;
		}
	}
	//img[(h-2)*w + w-2] = 255;
	//img[h2*w + w2] = 255;
	//printf("w2 = %d h2 = %d\n", w2, h2);
	//printf("w = %d h = %d\n", w, h);

	for(i=0; i < vxc; i++) {
		//printf("%d %p \n", j, vp[j]);
		//if(vp[i]->di != vp[i]->cn ) {
		if(vp[i]->n == 1 || vp[i]->di != vp[i]->cn) {
			printf("%d  x = %d y = %d %o %o %o n = %d\n", i, vp[i]->x,  vp[i]->y, vp[i]->di, vp[i]->cn, vp[i]->cn^vp[i]->di, vp[i]->n);
			//printf("%d n = %d %o %o %o\n", i, vp[i]->n, vp[i]->di, vp[i]->cn, vp[i]->cn^vp[i]->di);
			img[vp[i]->y*w + vp[i]->x-w] = 255;
			img[vp[i]->y*w + vp[i]->x-1] = 255;
			img[vp[i]->y*w + vp[i]->x+w] = 255;
			img[vp[i]->y*w + vp[i]->x+1] = 255;
		}
	}

	printf("Numbers of drawing vertexs  = %d\n", vc);
}

void seg_vertex_draw2(uint8 *img, Vertex **vp, uint32 vxc, uint32 w, uint32 h, uint32 w1, uint32 h1)
{
	uint32 i, yx, vc = 0;
	Vector v;
	uint8  nd, nd1, nd2, sh = 15;
	Vertex *vx, *vx1, *vx2, *vp1;
	uint32 kx = ((w-2)<<sh)/(w1-3);
	uint32 ky = ((h-2)<<sh)/(h1-3);
	//printf("w = %d w1 = %d h = %d h1 = %d kx = %d ky = %d\n", w-2, w1-2, h-2, h1-2, kx, ky);

	for(i=0; i < vxc; i++) vp[i]->cn = 0;

	for(i=0; i < vxc; i++){
		//If any problem may change if to while
		while(vp[i]->n > 1 && get_dir2(vp[i], &nd)){
			vx = vp[i];
			//vp1 = vp[i];

			//printf("%4d x = %4d y = %4d di = %d cn = %d nd = %d n = %d\n", i, vx->x, vx->y, vx->di, vx->cn, nd, vx->n);
			do{
				vc++;
				//printf("x = %4d y = %4d di = %d cn = %d nd = %d n = %d\n", vx->x, vx->y, vx->di, vx->cn, nd, vx->n);
				vx1 = vx->vp[nd];
				finish_dir1(vx, nd);
				nd1 =  find_pointer1(vx1, vx);
				finish_dir1(vx1, nd1);

				v.x1 = ((vx->x-1)*kx>>sh); 	v.x1 = (v.x1 == w-2) ? v.x1 : v.x1 + 1;
				v.y1 = ((vx->y-1)*ky>>sh);	v.y1 = (v.y1 == h-2) ? v.y1 : v.y1 + 1;
				v.x2 = ((vx1->x-1)*kx>>sh); v.x2 = (v.x2 == w-2) ? v.x2 : v.x2 + 1;
				v.y2 = ((vx1->y-1)*ky>>sh);	v.y2 = (v.y2 == h-2) ? v.y2 : v.y2 + 1;

				//if(vx->y >= h1-2 ) printf("x = %4d y = %d \n", vx->x-1, vx->y-1);
				//if(v.y1 >= h-2 ) {
				//	printf("x = %4d y = %d \n", vx->x-1, vx->y-1);
				//	printf("x = %4d y = %d \n\n", v.x1-1, v.y1-1);
				//}

				//c[0] = 128; c[1] = 128; c[2] = 128;
				draw_line_1(img, &v, w, 128);
				yx = v.y2*w + v.x2;
				img[yx] = 255;
				yx = v.y1*w + v.x1;
				img[yx] = 255;
				if(!get_clockwise_dir(vx1, nd1, &nd)) break;
				vx = vx1;
			}while(vx != vp[i]);

			//yx = vp[i]->y*w + vp[i]->x;
			//img[yx] = 255;
		}
	}
	//img[(h-2)*w + w-2] = 255;
	//img[h2*w + w2] = 255;
	//printf("w2 = %d h2 = %d\n", w2, h2);
	//printf("w = %d h = %d\n", w, h);

	for(i=0; i < vxc; i++) {
		//printf("%d %p \n", j, vp[j]);
		//if(vp[i]->di != vp[i]->cn ) {
		if(vp[i]->n == 1 || vp[i]->di != vp[i]->cn) {
			printf("%d  x = %d y = %d %o %o %o n = %d\n", i, vp[i]->x,  vp[i]->y, vp[i]->di, vp[i]->cn, vp[i]->cn^vp[i]->di, vp[i]->n);
			//printf("%d n = %d %o %o %o\n", i, vp[i]->n, vp[i]->di, vp[i]->cn, vp[i]->cn^vp[i]->di);
			img[vp[i]->y*w + vp[i]->x-w] = 255;
			img[vp[i]->y*w + vp[i]->x-1] = 255;
			img[vp[i]->y*w + vp[i]->x+w] = 255;
			img[vp[i]->y*w + vp[i]->x+1] = 255;
		}
	}
	printf("Numbers of drawing vertexs  = %d\n", vc);
}

static inline uint32 get_pix(uint8 *img, uint32 yx, uint32 w, uint8 nd1)
{
	uint8 nd = nd1+1 > 7 ? 0 : nd1+1;
	//if(nd != nd2 ){
		if		(nd == 0) { return yx-1  ; }
		else if	(nd == 1) { return yx-1-w; }
		else if	(nd == 2) { return yx  -w; }
		else if	(nd == 3) { return yx+1-w; }
		else if	(nd == 4) { return yx+1  ; }
		else if	(nd == 5) { return yx+1+w; }
		else if	(nd == 6) { return yx+  w; }
		else if	(nd == 7) { return yx-1+w; }
	//}
	return 0;
}

uint32  seg_vertex_draw3(uint8 *img, Vertex **vp, uint32 *inp, uint32 vxc, uint32 w, uint32 h, uint32 w1, uint32 h1)
{
	uint32 i, yx, vc = 0, lc = 0, rc = 0;
	Vector v;
	uint8  nd, nd1, nd2, sh = 15;
	Vertex *vx, *vx1, *vx2, *vp1;
	uint32 kx = ((w-2)<<sh)/(w1-3);
	uint32 ky = ((h-2)<<sh)/(h1-3);

	for(i=0; i < vxc; i++) vp[i]->cn = 0;

	for(i=0; i < vxc; i++){
		if(vp[i]->n > 1) vc++;
		//If any problem may change if to while
		while(vp[i]->n > 1 && get_dir2(vp[i], &nd)){
			vx = vp[i]; nd2 = nd; //vc = 0;
			//vp1 = vp[i];
			//printf("%4d x = %4d y = %4d di = %d cn = %d nd = %d n = %d\n", i, vx->x, vx->y, vx->di, vx->cn, nd, vx->n);
			while(1){
				//printf("x = %4d y = %4d di = %d cn = %d nd = %d n = %d\n", vx->x, vx->y, vx->di, vx->cn, nd, vx->n);
				vx1 = vx->vp[nd];
				finish_dir1(vx, nd);
				//finish_dir2(vx, nd);
				nd1 =  find_pointer1(vx1, vx);
				finish_dir1(vx1, nd1);
				//finish_dir2(vx1, nd1);

				v.x1 = scale(vx->x, w, kx, sh);
				v.y1 = scale(vx->y, h, ky, sh);
				v.x2 = scale(vx1->x, w, kx, sh);
				v.y2 = scale(vx1->y, h, ky, sh);
				/*
				v.x1 = ((vx->x-1)*kx>>sh); 	v.x1 = (v.x1 == w-2) ? v.x1 : v.x1 + 1;
				v.y1 = ((vx->y-1)*ky>>sh);	v.y1 = (v.y1 == h-2) ? v.y1 : v.y1 + 1;
				v.x2 = ((vx1->x-1)*kx>>sh); v.x2 = (v.x2 == w-2) ? v.x2 : v.x2 + 1;
				v.y2 = ((vx1->y-1)*ky>>sh);	v.y2 = (v.y2 == h-2) ? v.y2 : v.y2 + 1;
				*/

				draw_line_1(img, &v, w, 64);
				//draw_line_dir(img, &v, w);
				lc++;

				yx = v.y2*w + v.x2;
				img[yx] = 128;
				yx = v.y1*w + v.x1;
				img[yx] = 128;


				if(!get_clockwise_dir(vx1, nd1, &nd)) break;
				vx = vx1;
			} //;while(vx != vp[i]);

			rc++;
		}
	}

	for(i=0; i < vxc; i++) {
		//printf("%d %p \n", j, vp[j]);
		//if(vp[i]->di != vp[i]->cn ) {
		if(vp[i]->n == 1 || vp[i]->di != vp[i]->cn) {
			printf("%d  x = %d y = %d %o %o %o n = %d\n", i, vp[i]->x,  vp[i]->y, vp[i]->di, vp[i]->cn, vp[i]->cn^vp[i]->di, vp[i]->n);
			//printf("%d n = %d %o %o %o\n", i, vp[i]->n, vp[i]->di, vp[i]->cn, vp[i]->cn^vp[i]->di);
			img[vp[i]->y*w + vp[i]->x] = 255;
			//img[vp[i]->y*w + vp[i]->x-w] = 255;
			//img[vp[i]->y*w + vp[i]->x-1] = 255;
			//img[vp[i]->y*w + vp[i]->x+w] = 255;
			//img[vp[i]->y*w + vp[i]->x+1] = 255;
		}
	}
	printf("Numbers of drawing lines  = %d vertexes = %d regions  = %d\n", lc, vc, rc);
	return rc;
}

static inline uint8 get_region_color(uint8 *img, uint8 *con, uint32 *l1, uint32 num, uint32 w)
{
	uint32 cl = 128, c = 0, cn = 0, k = 0, yx, j;
	uint32 *l2 = &l1[num];

	while(num){
		for(j=0; j < num; j++){
			yx = l1[j] - 1;  if(!con[yx]) { con[yx] = cl; l2[k++] = yx; }
			yx = l1[j] - w;  if(!con[yx]) { con[yx] = cl; l2[k++] = yx; }
			yx = l1[j] + 1;  if(!con[yx]) { con[yx] = cl; l2[k++] = yx; }
			yx = l1[j] + w;  if(!con[yx]) { con[yx] = cl; l2[k++] = yx; }
			c += img[l1[j]]; cn++;
		}
		num = k; k = 0;
		l1 = l2; l2 = &l1[num];
	}
	return  c/cn;
}

static inline void fill_region_color(uint8 *con, uint32 *l1, uint32 num, uint8 col, uint32 w)
{
	uint32 k = 0, yx, j;
	uint32 *l2 = &l1[num];

	while(num){
		for(j=0; j < num; j++){
			yx = l1[j] - 1;  if(!con[yx]) { con[yx] = col; l2[k++] = yx; }
			yx = l1[j] - w;  if(!con[yx]) { con[yx] = col; l2[k++] = yx; }
			yx = l1[j] + 1;  if(!con[yx]) { con[yx] = col; l2[k++] = yx; }
			yx = l1[j] + w;  if(!con[yx]) { con[yx] = col; l2[k++] = yx; }
		}
		num = k; k = 0;
		l1 = l2; l2 = &l1[num];
	}
}

static inline void new_line1(Line *ln, Vertex *vx1, Vertex *vx2, uint8 nd1, uint8 nd2)
{
	ln->vx[0] = vx1; ln->vx[1] = vx2;
	ln->nd[0] = nd1; ln->nd[1] = nd2;
	ln->lc = 1;
	//vx1->lp[nd1] = ln; vx2->lp[nd2] = ln;
}

uint32  seg_remove_loops(uint8 *img, Vertex **vp, Vertex **vp1, Line *ln, Line **lp, uint32 vxc, uint32 w, uint32 h)
{
	uint32 i, j, k, yx, yxw, vc = 0, rc = 0, vc1, pn=1, pn1, fd, num;
	Vector v;
	uint8  nd, nd1, nd2, cn, lc = 0;
	Vertex *vx, *vx1, *vx2;
	//uint32 *l1 = buff, *l2 ;

	Vertex *vpx;
	int sq;
	uint32 regc = 2, cloop = 0, dloop = 0;

	for(i=0; i < vxc; i++) { vp[i]->cn = 0; vp[i]->reg = 0; }

	//Remove the outer contour
	vx = vp[0];
	get_dir_clock(vx, &nd);
	vx->reg = regc;
	do{
		vx = vx->vp[nd];
		nd = get_same_dir(vx, nd);
		vx->reg = regc;
	} while(vx != vp[0]);

	//Start main cycle
	for(i=0; i < vxc; i++){
		if(vp[i]->n > 1 && is_dir(vp[i])){
			//printf("New closed region\n");
			cloop++;
			pn = 1; pn1 = 0;
			vp1[0] = vp[i];
			while(pn1 < pn){
				vpx = vp1[pn1++];
				//printf("pn = %d\n", pn1);
				while(get_dir2(vpx, &nd)){
					regc++;
					vx = vpx; vc = 0; vc1 = 0; sq = 0; k = 0, lc = 0;

					vx->lp = &lp[lc+=8];
					for(j=0; j < 8; j++) vx->lp[j] = NULL;
					vx->reg = regc;

					do{
						vx1 = vx->vp[nd];
						nd2 = nd;
						//Calculate square of region
						//sq += (vx1->y + vx->y)*(vx1->x - vx->x)>>1;
						nd1 =  find_pointer1(vx1, vx);
						nd = get_clockwise_dir1(vx1, nd1);
						fd = finish_dir1(vx1, nd);

						if(vx1->reg != regc) vx1->reg = regc;
						else {
							new_line1(&ln[k++], vx1, vx1->vp[nd], nd, find_pointer1(vx1->vp[nd], vx1));
							//remove_dir2(vx1, nd)
						}


						//printf("vx->lp[nd2] = %p\n", vx1->lp[nd1]);

						//fd = finish_dir1(vx1, get_clockwise_dir1(vx1, nd1));
						/*
						if(vx1->reg == regc){
							if(!vx1->rc) vp2[j++] = vx1;
							vx1->rc++;

							//if(vx1->n == 2) { vx1->n = 0; } //vx1->di = 0; }
							//img[vx1->y*w + vx1->x-w] = 255;
							//img[vx1->y*w + vx1->x+w] = 255;
							//img[vx1->y*w + vx1->x-1] = 255;
							//img[vx1->y*w + vx1->x+1] = 255;
						} else vx1->reg = regc;
						*/

						/*
						if(vx1->reg != 1){
							if(vx1->reg == regc){
								vp2[j++] = vx1;
								vx1->reg = 1;
								//if(vx1->n == 2) { vx1->n = 0; } //vx1->di = 0; }
								//img[vx1->y*w + vx1->x-w] = 255;
								//img[vx1->y*w + vx1->x+w] = 255;
								//img[vx1->y*w + vx1->x-1] = 255;
								//img[vx1->y*w + vx1->x+1] = 255;
							} else vx1->reg = regc;
						}
						*/
						//if(vx1->reg == 1 && vx->reg == 1){
							//printf("vx = %p vx[nd2] = %p vx1 = %p vx1[nd1] = %p\n", vx, vx->vp[nd2], vx1, vx1->vp[nd1]);
						//	remove_dir2(vx, nd2);
						//}
						//printf("vx = %p vx1 = %p\n", vx, vx1);

						//Store in the buffer
						/*
						if(!(vx1 == vpx && fd)){
							vx1->lp = &lp[lc+=8];
							for(j=0; j < 8; j++) vx1->lp[j] = NULL;
						} else {
							//printf("Break regc = %d\n", regc);
							break;
						}
						if(vx1->lp[nd1] == NULL) {
							new_line1(&ln[k++], vx, vx1, nd2, nd1);
							//printf("New line %p %p\n", vx, vx1);
							//printf("new lc[%d] = %d\n", k, vx1->lp[nd1]->lc);
						} else {
							vx1->lp[nd1]->lc++;
							printf("old lc = %d\n", vx1->lp[nd1]->lc);
						}*/

						img[vx1->y*w + vx1->x] = 255;
						//img[vx1->y*w + vx1->x+w] = 255;
						//img[vx1->y*w + vx1->x-1] = 255;
						//img[vx1->y*w + vx1->x+1] = 255;

						if(vx1->di != vx1->cn && vx1->n > 2) vp1[pn++] = vx1;
						vc1++;
						vx = vx1;
						if(vx == vpx && fd) break;
					} while(1);

					for(j=0; j < k; j++){
						//Remove all lines with count more than one
						//if(ln[j].lc > 1){

							remove_dir1(ln[j].vx[0], ln[j].nd[0]);
							remove_dir1(ln[j].vx[1], ln[j].nd[1]);
							//printf("k = %d  j = %d x = %d y = %d\n", k, j, ln[j].vx[0]->x, ln[j].vx[0]->y);
							/*
							img[ln[j].vx[0]->y*w + ln[j].vx[0]->x-w] = 255;
							img[ln[j].vx[0]->y*w + ln[j].vx[0]->x+w] = 255;
							img[ln[j].vx[0]->y*w + ln[j].vx[0]->x-1] = 255;
							img[ln[j].vx[0]->y*w + ln[j].vx[0]->x+1] = 255;
							img[ln[j].vx[1]->y*w + ln[j].vx[1]->x-w] = 255;
							img[ln[j].vx[1]->y*w + ln[j].vx[1]->x+w] = 255;
							img[ln[j].vx[1]->y*w + ln[j].vx[1]->x-1] = 255;
							img[ln[j].vx[1]->y*w + ln[j].vx[1]->x+1] = 255;*/

							//return 0;
						//}
					}
					//if(k) return 0;
					/*
					num = j;
					//Remove direction
					for(j=0; j < num; j++){
						if(vp2[j]->n == vp2[j]->rc+1){
							vx = vp2[j];
							if ((vx->di&128) && (vx->cn&128)) { vx->di ^= 128; 	vx->cn ^= 128; 	vx->n--; }
							if ((vx->di&64 ) && (vx->cn&64 )) { vx->di ^= 64; 	vx->cn ^= 64; 	vx->n--; }
							if ((vx->di&32 ) && (vx->cn&32 )) { vx->di ^= 32; 	vx->cn ^= 32; 	vx->n--; }
							if ((vx->di&16 ) && (vx->cn&16 )) { vx->di ^= 16; 	vx->cn ^= 16; 	vx->n--; }
							if ((vx->di&8  ) && (vx->cn&8  )) { vx->di ^= 8; 	vx->cn ^= 8; 	vx->n--; }
							if ((vx->di&4  ) && (vx->cn&4  )) { vx->di ^= 4; 	vx->cn ^= 4; 	vx->n--; }
							if ((vx->di&2  ) && (vx->cn&2  )) { vx->di ^= 2; 	vx->cn ^= 2; 	vx->n--; }
							if ((vx->di&1  ) && (vx->cn&1  )) { vx->di ^= 1; 	vx->cn ^= 1; 	vx->n--; }
						}
					}
					for(j=0; j < num; j++){
						if(vp2[j]->n > 2){
							vx = vp2[j];
							if ((vx->di&128) && (vx->cn&128) && vx->vp[0]->n == 0) { vx->di ^= 128; vx->cn ^= 128; 	vx->n--; }
							if ((vx->di&64 ) && (vx->cn&64 ) && vx->vp[1]->n == 0) { vx->di ^= 64; 	vx->cn ^= 64; 	vx->n--; }
							if ((vx->di&32 ) && (vx->cn&32 ) && vx->vp[2]->n == 0) { vx->di ^= 32; 	vx->cn ^= 32; 	vx->n--; }
							if ((vx->di&16 ) && (vx->cn&16 ) && vx->vp[3]->n == 0) { vx->di ^= 16; 	vx->cn ^= 16; 	vx->n--; }
							if ((vx->di&8  ) && (vx->cn&8  ) && vx->vp[4]->n == 0) { vx->di ^= 8; 	vx->cn ^= 8; 	vx->n--; }
							if ((vx->di&4  ) && (vx->cn&4  ) && vx->vp[5]->n == 0) { vx->di ^= 4; 	vx->cn ^= 4; 	vx->n--; }
							if ((vx->di&2  ) && (vx->cn&2  ) && vx->vp[6]->n == 0) { vx->di ^= 2; 	vx->cn ^= 2; 	vx->n--; }
							if ((vx->di&1  ) && (vx->cn&1  ) && vx->vp[7]->n == 0) { vx->di ^= 1; 	vx->cn ^= 1; 	vx->n--; }
						}
					}
					for(j=0; j < num; j++){
						if(vp2[j]->n > 2){
							vx = vp2[j];
							if ((vx->di&128) &&  vx->vp[0]->n > 2) remove_dir2(vx, 0);
							if ((vx->di&64 ) &&  vx->vp[1]->n > 2) remove_dir2(vx, 1);
							if ((vx->di&32 ) &&  vx->vp[2]->n > 2) remove_dir2(vx, 2);
							if ((vx->di&16 ) &&  vx->vp[3]->n > 2) remove_dir2(vx, 3);
							if ((vx->di&8  ) &&  vx->vp[4]->n > 2) remove_dir2(vx, 4);
							if ((vx->di&4  ) &&  vx->vp[5]->n > 2) remove_dir2(vx, 5);
							if ((vx->di&2  ) &&  vx->vp[6]->n > 2) remove_dir2(vx, 6);
							if ((vx->di&1  ) &&  vx->vp[7]->n > 2) remove_dir2(vx, 7);
						}
					}
					*/
				}
			}
		}
	}
/*
	for(i=0; i < vxc; i++) {
		if(vp[i]->n > 1 && vp[i]->di != vp[i]->cn) {
			printf("%d  x = %d y = %d %o %o %o n = %d\n", i, vp[i]->x,  vp[i]->y, vp[i]->di, vp[i]->cn, vp[i]->cn^vp[i]->di, vp[i]->n);
			//printf("%d n = %d %o %o %o\n", i, vp[i]->n, vp[i]->di, vp[i]->cn, vp[i]->cn^vp[i]->di);
			//img[vp[i]->y*w + vp[i]->x-w] = 255;
			//img[vp[i]->y*w + vp[i]->x-1] = 255;
			//img[vp[i]->y*w + vp[i]->x+w] = 255;
			//img[vp[i]->y*w + vp[i]->x+1] = 255;
		}
	}
	*/
	printf("Numbers of drawing regions  = %d closed loops = %d deg loop = %d\n", regc, cloop, dloop - cloop);
	return rc;
}

uint32  seg_get_or_fill_color(uint8 *img, uint8 *con, uint8 *col, uint32 *buff, Vertex **vp, Vertex **vp1, uint32 vxc, uint32 w, uint32 h, uint32 w1, uint32 h1, uint32 get)
{
	uint32 i, j, k, yx, yxw, vc = 0, rc = 0, vc1, pn=1, pn1, fd;
	Vector v;
	uint8  nd, nd1, nd2, sh = 15, cl = 100;
	Vertex *vx, *vx1, *vx2;
	uint32 kx = ((w-2)<<sh)/(w1-3);
	uint32 ky = ((h-2)<<sh)/(h1-3);
	//uint32 num, c, cn;
	//uint32 *l1 = buff, *l2 = &buff[w*h>>2], *tm;
	uint32 *l1 = buff, *l2 ;

	Vertex *vpx;
	int sq;
	uint32 cloop = 0, dloop = 0;

	for(i=0; i < vxc; i++) vp[i]->cn = 0;

	//Remove the outer contour
	vx = vp[0];
	get_dir_clock(vx, &nd);
	do{
		vx = vx->vp[nd];
		nd = get_same_dir(vx, nd);
	} while(vx != vp[0]);

	//Start main cycle
	for(i=0; i < vxc; i++){
		if(vp[i]->n > 1 && is_dir(vp[i])){
			//printf("New closed region\n");
			cloop++;
			pn = 1; pn1 = 0;
			vp1[0] = vp[i];
			while(pn1 < pn){
				vpx = vp1[pn1++];
				//printf("pn = %d\n", pn1);
				while(get_dir2(vpx, &nd)){
					vx = vpx; vc = 0; vc1 = 0; sq = 0;//nd2 = nd;
					if(!get) cl = col[rc];
					do{
						vx1 = vx->vp[nd];
						//Calculate square of region
						sq += (vx1->y + vx->y)*(vx1->x - vx->x)>>1;

						nd1 =  find_pointer1(vx1, vx);
						nd = get_clockwise_dir1(vx1, nd1);
						fd = finish_dir1(vx1, nd);
						//Store in the buffer
						if(vx1->di != vx1->cn && vx1->n > 2) vp1[pn++] = vx1;

						yxw  = get_first_pixel(con, vx1, vx1->vp[nd], vx1->vp[nd1], w, h, kx, ky, sh);
						if(yxw) { l1[vc++] = yxw; con[yxw] = cl; }
						//last = yxw ? 1 : 0;
						vx = vx1;
						vc1++;
						if(vx == vpx && fd) break;
					} while(1);
					//} while(vx != vpx && df);

					if(!vc){
						//printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!       x = %d y = %d vc = %d di = %d cn = %d pn1 = %d sq = %d\n", vpx->x, vpx->y, vc1, vpx->di, vpx->cn, pn1, sq);
						//con[vpx->x + w*vpx->y] = 255;
						dloop++;
					}

					if(vc){
						if(get) col[rc++] = get_region_color(img, con, l1, vc, w);
						else {
							if(col[rc]) fill_region_color(con, l1, vc, col[rc++], w);
							else rc++;
						}
					} else {
						if(get) col[rc++] = 0;
						else rc++;
					}
				}
			}
		}
	}

	for(i=0; i < vxc; i++) {
		if(vp[i]->n == 1 || vp[i]->di != vp[i]->cn) {
			printf("%d  x = %d y = %d %o %o %o n = %d\n", i, vp[i]->x,  vp[i]->y, vp[i]->di, vp[i]->cn, vp[i]->cn^vp[i]->di, vp[i]->n);
			//printf("%d n = %d %o %o %o\n", i, vp[i]->n, vp[i]->di, vp[i]->cn, vp[i]->cn^vp[i]->di);
			img[vp[i]->y*w + vp[i]->x-w] = 255;
			img[vp[i]->y*w + vp[i]->x-1] = 255;
			img[vp[i]->y*w + vp[i]->x+w] = 255;
			img[vp[i]->y*w + vp[i]->x+1] = 255;
		}
	}
	printf("Numbers of drawing regions  = %d closed loops = %d deg loop = %d\n", rc, cloop, dloop - cloop);
	return rc;
}


uint32 seg_get_one_color(uint8 *img,  uint8 *con, uint8 *col, uint32 *buff, uint32 w, uint32 h)
{
	uint32 i, j, y, x, yx, yxw, yw,  h1 = h-2, w1 = w-2;
	uint32 rgc = 0;
	uint32 num, c, cn, cl = 254;
	uint32 *l1 = buff, *l2 = &buff[w*h>>2], *tm;

	for(y=2; y < h1; y++){
		yw = y*w;
		for(x=2; x < w1; x++){
			yx = yw + x;
			if(!con[yx]){
				//printf("%d x = %d  %d y = %d con = %d\n", w, yx%w, h, yx/w, con[yx]);
				if(yx == (712 + 1298*w) || yx == (2 + 1302*w)) break;
				c = 0;  cn = 0;
				con[yx] = cl; num = 1; l1[0] = yx; i = 0;
				while(num){
					for(j=0; j < num; j++){
						yxw = l1[j] - 1;
						if(!con[yxw]) { con[yxw] = cl; l2[i++] = yxw; }
						yxw = l1[j] - w;
						if(!con[yxw]) { con[yxw] = cl; l2[i++] = yxw; }
						yxw = l1[j] + 1;
						if(!con[yxw]) { con[yxw] = cl; l2[i++] = yxw; }
						yxw = l1[j] + w;
						if(!con[yxw]) { con[yxw] = cl; l2[i++] = yxw; }

						c += img[l1[j]]; cn++;
					}
					num = i; i = 0;
					tm = l1; l1 = l2; l2 = tm;
				}
				col[rgc] = c/cn;
				rgc++;
				//if(rgc == 1) return 0;
			}
		}
	}
	printf("Numbers of geted regions  = %d\n", rgc);
	//for(i=0; i < rgc; i++)  printf("%5d  %3d %3d %3d\n", i, col[i*3], col[i*3+1], col[i*3+1]);
	return rgc;
}

static inline uint32 check_reg_pix(uint8 *dir, uint32 yx, uint32 w, uint32 d)
// Check is a pixel in region
// d  = 0   d = 1    d = 2    d = 3
// |x|c|*|  |*|c|x|  |*|*|*|  |*|*|*|
// |c|*|*|  |*|*|c|  |*|*|c|  |c|*|*|
// |*|*|*|  |*|*|*|  |*|c|x|  |x|c|*|
{
	if(d == 0){
		if(dir[yx+w] == 255 || dir[yx+1] == 255 ) return 0;
		if(!(dir[yx+w] & 17)&& !(dir[yx+1] & 17) ) return 1;
		return 0;
	}
	else if(d == 1){
		if(dir[yx+w] == 255 || dir[yx-1] == 255 ) return 0;
		if(!(dir[yx+w] & 68)&& !(dir[yx-1] & 68) ) return 1;
		return 0;
	}
	else if(d == 2){
		if(dir[yx-w] == 255 || dir[yx-1] == 255 ) return 0;
		if(!(dir[yx-w] & 17) && !(dir[yx-1] & 17)) return 1;
		return 0;
	}
	else if(d == 3){
		if(dir[yx-w] == 255 || dir[yx+1] == 255 ) return 0;
		if(!(dir[yx-w] & 68) && !(dir[yx+1] & 68)) return 1;
		return 0;
	}
}

uint32 seg_get_one_color1(uint8 *img, uint8 *con, uint8 *col, uint32 *buff, uint32 w, uint32 h)
{
	uint32 i, j, y, x, yx, yxw, yw,  h1 = h-2, w1 = w-2;
	uint32 rgc = 0, cl = 255;
	uint32 num, c, cn, sh = 1<<31;
	uint32 *l1 = buff, *l2 = &buff[w*h>>2], *tm;

	for(y=2; y < h1; y++){
		yw = y*w;
		for(x=2; x < w1; x++){
			yx = yw + x;
			if(!con[yx]){
				c = 0;  cn = 0;
				con[yx] = cl; num = 1; l1[0] = yx; i = 0;
				//printf("%d x = %d  %d y = %d con = %d\n", w, yx%w, h, yx/w, con[yx]);
				while(num){
					for(j=0; j < num; j++){
						yxw = l1[j] - 1; //printf("%d x = %d  %d y = %d con = %d\n", w, yxw%w, h, yxw/w, con[yxw]);
						if(!con[yxw]) { con[yxw] = cl; l2[i++] = yxw; }
						yxw = l1[j] - 1 - w; //printf("%d x = %d  %d y = %d con = %d\n", w, yxw%w, h, yxw/w, con[yxw]);
						if(!con[yxw] && check_reg_pix(con, yxw, w, 0)) { con[yxw] = cl; l2[i++] = yxw; }
						yxw = l1[j] - w; //printf("%d x = %d  %d y = %d con = %d\n", w, yxw%w, h, yxw/w, con[yxw]);
						if(!con[yxw]) { con[yxw] = cl; l2[i++] = yxw; }
						yxw = l1[j] + 1 - w; //printf("%d x = %d  %d y = %d con = %d\n", w, yxw%w, h, yxw/w, con[yxw]);
						if(!con[yxw] && check_reg_pix(con, yxw, w, 1)) { con[yxw] = cl; l2[i++] = yxw; }
						yxw = l1[j] + 1; //printf("%d x = %d  %d y = %d con = %d\n", w, yxw%w, h, yxw/w, con[yxw]);
						if(!con[yxw]) { con[yxw] = cl; l2[i++] = yxw; }
						yxw = l1[j] + 1 + w; //printf("%d x = %d  %d y = %d con = %d\n", w, yxw%w, h, yxw/w, con[yxw]);
						if(!con[yxw] && check_reg_pix(con, yxw, w, 2)) { con[yxw] = cl; l2[i++] = yxw; }
						yxw = l1[j] + w; //printf("%d x = %d  %d y = %d con = %d\n", w, yxw%w, h, yxw/w, con[yxw]);
						if(!con[yxw]) { con[yxw] = cl; l2[i++] = yxw; }
						yxw = l1[j] - 1 + w; //printf("%d x = %d  %d y = %d con = %d\n", w, yxw%w, h, yxw/w, con[yxw]);
						if(!con[yxw] && check_reg_pix(con, yxw, w, 3)) { con[yxw] = cl; l2[i++] = yxw; }

						c += img[l1[j]]; cn++;
					}
					num = i; i = 0;
					tm = l1; l1 = l2; l2 = tm;
				}
				col[rgc++] = c/cn;
				//rgc++;
				//if(rgc == 1) return 0;
			}
		}
	}
	printf("Numbers of geted regions  = %d\n", rgc);
	//for(i=0; i < rgc; i++)  printf("%5d  %3d %3d %3d\n", i, col[i*3], col[i*3+1], col[i*3+1]);
	return rgc;
}

uint32 seg_get_one_color2(uint8 *img, uint8 *con, uint8 *col, uint32 *inp, uint32 *buff, uint32 rgc, uint32 w, uint32 h)
{
	uint32 i, j, y, x, yx, yxw, yw,  h1 = h-2, w1 = w-2;
	uint32 rc, cl = 255;
	uint32 num, c, cn, sh = 1<<31;
	uint32 *l1 = buff, *l2 = &buff[w*h>>2], *tm;

	for(rc=0; rc < rgc; rc++){
		c = 0;  cn = 0;
		yx = inp[rc];
		con[yx] = cl; num = 1; l1[0] = yx; i = 0;
		//printf("%d w = %d x = %d  %d y = %d con = %d\n", rc, w, yx%w, h, yx/w, con[yx]);

		while(num){
			for(j=0; j < num; j++){
				yxw = l1[j] - 1; //printf("%d x = %d  %d y = %d con = %d\n", w, yxw%w, h, yxw/w, con[yxw]);
				if(!con[yxw]) { con[yxw] = cl; l2[i++] = yxw; }
				yxw = l1[j] - 1 - w; //printf("%d x = %d  %d y = %d con = %d\n", w, yxw%w, h, yxw/w, con[yxw]);
				if(!con[yxw] && check_reg_pix(con, yxw, w, 0)) { con[yxw] = cl; l2[i++] = yxw; }
				yxw = l1[j] - w; //printf("%d x = %d  %d y = %d con = %d\n", w, yxw%w, h, yxw/w, con[yxw]);
				if(!con[yxw]) { con[yxw] = cl; l2[i++] = yxw; }
				yxw = l1[j] + 1 - w; //printf("%d x = %d  %d y = %d con = %d\n", w, yxw%w, h, yxw/w, con[yxw]);
				if(!con[yxw] && check_reg_pix(con, yxw, w, 1)) { con[yxw] = cl; l2[i++] = yxw; }
				yxw = l1[j] + 1; //printf("%d x = %d  %d y = %d con = %d\n", w, yxw%w, h, yxw/w, con[yxw]);
				if(!con[yxw]) { con[yxw] = cl; l2[i++] = yxw; }
				yxw = l1[j] + 1 + w; //printf("%d x = %d  %d y = %d con = %d\n", w, yxw%w, h, yxw/w, con[yxw]);
				if(!con[yxw] && check_reg_pix(con, yxw, w, 2)) { con[yxw] = cl; l2[i++] = yxw; }
				yxw = l1[j] + w; //printf("%d x = %d  %d y = %d con = %d\n", w, yxw%w, h, yxw/w, con[yxw]);
				if(!con[yxw]) { con[yxw] = cl; l2[i++] = yxw; }
				yxw = l1[j] - 1 + w; //printf("%d x = %d  %d y = %d con = %d\n", w, yxw%w, h, yxw/w, con[yxw]);
				if(!con[yxw] && check_reg_pix(con, yxw, w, 3)) { con[yxw] = cl; l2[i++] = yxw; }

				c += img[l1[j]]; cn++;
			}
			num = i; i = 0;
			tm = l1; l1 = l2; l2 = tm;
		}
		col[rc] = c/cn;

		//if(rgc == 1) return 0;
	}
	printf("Numbers of geted regions  = %d\n", rgc);
	//for(i=0; i < rgc; i++)  printf("%5d  %3d %3d %3d\n", i, col[i*3], col[i*3+1], col[i*3+1]);
	return rgc;
}

uint32 seg_draw_color_one(uint8 *img, uint8 *col, uint32 *buff, uint32 w, uint32 h)
{
	uint32 i, j, y, x, yx, yxw, yw,  h1 = h-2, w1 = w-2;
	uint32 rgc = 0;
	uint32 num, c, cn, cl;
	uint32 *l1 = buff, *l2 = &buff[w*h>>2], *tm;

	for(y=2; y < h1; y++){
		yw = y*w;
		for(x=2; x < w1; x++){
			yx = yw + x;
			if(!img[yx]){
				//printf("yx = %d img = %d\n", yx, img[yx]);
				c = 0;  cn = 0;
				cl = col[rgc]; img[yx] = cl; num = 1; l1[0] = yx; i = 0;
				while(num){
					for(j=0; j < num; j++){
						yxw = l1[j] - 1;
						if(!img[yxw]) { img[yxw] = cl; l2[i++] = yxw; }
						yxw = l1[j] - w;
						if(!img[yxw]) { img[yxw] = cl; l2[i++] = yxw; }
						yxw = l1[j] + 1;
						if(!img[yxw]) { img[yxw] = cl; l2[i++] = yxw; }
						yxw = l1[j] + w;
						if(!img[yxw]) { img[yxw] = cl; l2[i++] = yxw; }

						c += img[l1[j]]; cn++;
					}
					num = i; i = 0;
					tm = l1; l1 = l2; l2 = tm;
				}
				//col[rgc] = c/cn;
				rgc++;
				//if(rgc == 1) return 0;
			}
		}
	}
	printf("Numbers of drawing regions  = %d\n", rgc);
	//for(i=0; i < rgc; i++)  printf("%5d  %3d %3d %3d\n", i, col[i*3], col[i*3+1], col[i*3+1]);
	return rgc;
}

void seg_draw_line_one(uint8 *img, uint32 w, uint32 h)
{
	uint32 y, x, yx, yxw, yw,  h1 = h-1, w1 = w-1, sum, cn;

	for(y=1; y < h1; y++){
		yw = y*w;
		for(x=1; x < w1; x++){
			yx = yw + x;
			if(img[yx] < 2){
				sum = 0; cn = 0;
				if(img[yx-1  ] > 1) { sum += img[yx-1  ]; cn++; }
				if(img[yx-1-w] > 1) { sum += img[yx-1-w]; cn++; }
				if(img[yx  -w] > 1) { sum += img[yx  -w]; cn++; }
				if(img[yx+1-w] > 1) { sum += img[yx+1-w]; cn++; }
				if(img[yx+1  ] > 1) { sum += img[yx+1  ]; cn++; }
				if(img[yx+1+w] > 1) { sum += img[yx+1+w]; cn++; }
				if(img[yx  +w] > 1) { sum += img[yx  +w]; cn++; }
				if(img[yx-1+w] > 1) { sum += img[yx-1+w]; cn++; }
				img[yx] = sum/cn;
			}
		}
	}
}


void seg_draw_line_color(uint8 *r, uint8 *g, uint8 *b, Line *ln, uint32 lc, uint32 w, uint32 h)
{
	uint32 i, yx, nd;
	int dx , dy;
	Vector v;
	uint8 c[3];

	for(i=0; i < lc; i++){
		v.x1 = ln[i].vx[0]->x; v.y1 = ln[i].vx[0]->y;
		v.x2 = ln[i].vx[1]->x; v.y2 = ln[i].vx[1]->y;
		//printf("%d x1 = %d y1 = %d x2 = %d y2 = %d\n", i, v.x1, v.y1, v.x2, v.y2);
		//draw_line_31(r, g, b, &v, w, vp[i]->ln[nd].l[0], vp[i]->ln[nd].l[1], vp[i]->ln[nd].l[2]);
		//if(abs(v.x1-v.x2) > 2 || abs(v.y1-v.y2) > 2){
			//if(ln[i].vx[0]->n > 1 || ln[i].vx[1]->n > 1) {
				//draw_three_lines(r, g, b, &v, w, ln[i].l, ln[i].r);
				c[0] = ln[i].pow; c[1] = ln[i].pow; c[2] = ln[i].pow;
				draw_line_3(r, g, b, &v, w, c);

				yx = ln[i].vx[0]->y*w + ln[i].vx[0]->x;
				r[yx] = 255; g[yx] = 255; b[yx] = 255;
				yx = ln[i].vx[1]->y*w + ln[i].vx[1]->x;
				r[yx] = 255; g[yx] = 255; b[yx] = 255;

			//}
		//}
		//draw_line_3(r, g, b, &v, w, ln[i].l);


	}
}

static inline int get_next_pix(uint8 *r, uint32 yx, uint32 w, int d)
{
	if      (d == -1  ) {
		//if 		(!r[yx + w-1]) return  w-1;
		if 		(!r[yx + w  ]) return  w  ;
		//else if (!r[yx + w+1]) return  w+1;
		else if (!r[yx +   1]) return    1;
		//else if (!r[yx - w+1]) return -w+1;
		else if (!r[yx - w  ]) return -w  ;
		//else if (!r[yx - w-1]) return -w-1;
	}
	/*else if (d == -w-1) {
		if      (!r[yx -   1]) return   -1;
		else if (!r[yx + w-1]) return  w-1;
		else if (!r[yx + w  ]) return  w  ;
		else if (!r[yx + w+1]) return  w+1;
		else if (!r[yx +   1]) return    1;
		else if (!r[yx - w+1]) return -w+1;
		else if (!r[yx - w  ]) return -w  ;
	}*/
	else if (d == -w  ) {
		//if 		(!r[yx - w-1]) return -w-1;
		if 		(!r[yx -   1]) return   -1;
		//else if (!r[yx + w-1]) return  w-1;
		else if (!r[yx + w  ]) return  w  ;
		//else if (!r[yx + w+1]) return  w+1;
		else if (!r[yx +   1]) return    1;
		//else if (!r[yx - w+1]) return -w+1;
	}
	/*else if (d == -w+1) {
		if 		(!r[yx - w  ]) return -w  ;
		else if (!r[yx - w-1]) return -w-1;
		else if (!r[yx -   1]) return   -1;
		else if (!r[yx + w-1]) return  w-1;
		else if (!r[yx + w  ]) return  w  ;
		else if (!r[yx + w+1]) return  w+1;
		else if (!r[yx +   1]) return    1;
	}*/
	else if (d ==  1  ) {
		//if 		(!r[yx - w+1]) return -w+1;
		if 		(!r[yx - w  ]) return -w  ;
		//else if (!r[yx - w-1]) return -w-1;
		else if (!r[yx -   1]) return   -1;
		//else if (!r[yx + w-1]) return  w-1;
		else if (!r[yx + w  ]) return  w  ;
		//else if (!r[yx + w+1]) return  w+1;
	}
	/*else if (d ==  w+1) {
		if 		(!r[yx +   1]) return    1;
		else if (!r[yx - w+1]) return -w+1;
		else if (!r[yx - w  ]) return -w  ;
		else if (!r[yx - w-1]) return -w-1;
		else if (!r[yx -   1]) return   -1;
		else if (!r[yx + w-1]) return  w-1;
		else if (!r[yx + w  ]) return  w  ;
	}*/
	else if (d ==  w  ) {
		//if 		(!r[yx + w+1]) return  w+1;
		if 		(!r[yx +   1]) return    1;
		//else if (!r[yx - w+1]) return -w+1;
		else if (!r[yx - w  ]) return -w  ;
		//else if (!r[yx - w-1]) return -w-1;
		else if (!r[yx -   1]) return   -1;
		//else if (!r[yx + w-1]) return  w-1;
	}
	/*else if (d ==  w-1) {
		if 		(!r[yx + w  ]) return  w  ;
		else if (!r[yx + w+1]) return  w+1;
		else if (!r[yx +   1]) return    1;
		else if (!r[yx - w+1]) return -w+1;
		else if (!r[yx - w  ]) return -w  ;
		else if (!r[yx - w-1]) return -w-1;
		else if (!r[yx -   1]) return   -1;
	}*/
	return 0;

}

static inline int fill_pixel(uint8 *r, uint8 *g, uint8 *b, uint32 yx, uint32 w, int d)
{
	uint32 cn = 0, c[3];
	c[0] = 0; c[1] = 0; c[2] = 0;

	if(r[yx-1  ]) { c[0]+=r[yx-1]; c[1]+=g[yx-1]; c[2]+=b[yx-1]; cn++; }
	if(r[yx-w  ]) { c[0]+=r[yx-w]; c[1]+=g[yx-w]; c[2]+=b[yx-w]; cn++; }
	if(r[yx+1  ]) { c[0]+=r[yx+1]; c[1]+=g[yx+1]; c[2]+=b[yx+1]; cn++; }
	if(r[yx+w  ]) { c[0]+=r[yx+w]; c[1]+=g[yx+w]; c[2]+=b[yx+w]; cn++; }

	if(cn == 1) 	 { r[yx] = c[0];    g[yx] = c[1];    b[yx] = c[2];   }
	else if(cn == 2) { r[yx] = c[0]>>1; g[yx] = c[1]>>1; b[yx] = c[2]>>1;}
	else if(cn == 3) { r[yx] = c[0]/3;  g[yx] = c[1]/3;  b[yx] = c[2]/3; }
	else if(cn == 4) { r[yx] = c[0]>>2; g[yx] = c[1]>>2; b[yx] = c[2]>>2;}

	return get_next_pix(r, yx, w, d);
}


uint32  seg_fill_region(uint8 *r, uint8 *g, uint8 *b, uint32 w, uint32 h)
{
	uint32 y, x, yx, yx1, yw, fst = 0, yxf, rgc = 0, pixc;
	int d;
	Vector v;

	for(y=0; y < h; y++){
		yw = y*w;
		for(x=0; x < w; x++){
			yx = yw + x;
			if(!r[yx]){
				pixc = 1;
				yx1 = yx;
				if(!fst) { fst = 1; yxf = yx; }
				d = fill_pixel(r, g, b, yx1, w, -1);
				while(d){
					yx1 += d;
					d = fill_pixel(r, g, b, yx1, w, d);
					pixc++;
				}
				rgc++;
				//printf("%d npix = %d\n", rgc, pixc);
			}
		}
	}
	printf("Numbers of regions  = %d\n", rgc);

	return rgc;
}

uint32  seg_get_color(uint8 *r, uint8 *g, uint8 *b, uint8 *r1, uint8 *g1, uint8 *b1, uint8 *col, uint32 w, uint32 h)
{
	uint32 y, x, yx, yx1, yw, fst = 0, yxf, rgc = 0, cn, rc3;
	int d;
	Vector v;
	uint32 c[3];
	uint8 cl[3];

	for(y=0; y < h; y++){
		yw = y*w;
		for(x=0; x < w; x++){
			yx = yw + x;
			if(!r1[yx]){
				yx1 = yx;
				if(!fst) { fst = 1; yxf = yx; }
				d = get_next_pix(r1, yx1, w, -1);
				c[0] = r[yx1]; c[1] = g[yx1]; c[2] = b[yx1]; cn = 1;
				r1[yx1] = r[yx1]; g1[yx1] = g[yx1]; b1[yx1] = b[yx1];
				while(d){
					yx1 += d;
					c[0] += r[yx1]; c[1] += g[yx1]; c[2] += b[yx1]; cn++;
					r1[yx1] = r[yx1]; g1[yx1] = g[yx1]; b1[yx1] = b[yx1];
					//printf("yx = %d d = %d r = %d g = %d b = %d\n", yx1, d, r1[yx1], g1[yx1], b1[yx1]);
					d = get_next_pix(r1, yx1, w, d);
				}
				rc3 = rgc*3;
				col[rc3] = c[0]/cn; col[rc3+1] = c[1]/cn; col[rc3+2] = c[2]/cn;
				rgc++;
			}
		}
	}
	printf("Numbers of regions  = %d\n", rgc);
	return rgc;
}

uint32  seg_draw_color(uint8 *r1, uint8 *g1, uint8 *b1, uint8 *col, uint32 w, uint32 h)
{
	uint32 y, x, yx, yx1, yw, fst = 0, yxf, rgc = 0, cn, rc3;
	int d;
	Vector v;
	uint8 cl[3];

	for(y=0; y < h; y++){
		yw = y*w;
		for(x=0; x < w; x++){
			yx = yw + x;
			if(!r1[yx]){
				yx1 = yx;
				rc3 = rgc*3;
				d = 1;
				do{
					r1[yx1] = col[rc3]; g1[yx1] = col[rc3+1]; b1[yx1] = col[rc3+2];
					printf("yx = %d d = %d r = %d g = %d b = %d\n", yx1, d, r1[yx1], g1[yx1], b1[yx1]);
					d = get_next_pix(r1, yx1, w, -d);
					yx1 += d;
				} while(d);
				rgc++;
				if(rgc > 3) return 0;
			}
		}
	}
	printf("Numbers of regions  = %d\n", rgc);
	return rgc;
}


uint32 seg_get_color1(uint8 *r, uint8 *g, uint8 *b, uint8 *r1, uint32 *col, uint32 *colp, uint8 *pon, uint32 *buf, uint32 w, uint32 h)
{
	uint32 y, x, yx, yx1, yw, fst = 0, yxf, cn, rc3, w1 = w-1, h1 = h-1;
	uint32 *l1, *l2, ind, in2, in3, *tm;
	uint32 nrg = 0, cp;
	int d,  rgc = -1;
	l1 = buf; l2 = &buf[w];
	//for(x=0; x < w; x++) l[0][x] = 0;

	for(y=1; y < h1; y++){
		yw = y*w;
		//for(x=0; x < w; x++) l[1][x] = 0;
		for(x=1; x < w1; x++){
			yx = yw + x;
			if(!r1[yx]){
				if(!r1[yx-w]) {
					l2[x]  = l1[x];
					if(x>1 && !r1[yx-1] && l2[x] != l2[x-1] && !colp[l2[x]]) {//
						if(colp[l2[x-1]] != l2[x]){
							if(!colp[l2[x-1]]){
								colp[l2[x]] = l2[x-1];
							}
							else if(!colp[colp[l2[x-1]]]){
								colp[l2[x]] = colp[l2[x-1]];
							}
						}
						//pon[l2[x-1]] = 1;
						//colp[l2[x]] = l2[x-1];
					}
					ind =  l2[x];
				}
				else if(!r1[yx-1]) { l2[x]  = l2[x-1]; ind = l2[x]; }
				//else if(!r1[yx-w+1] && !r1[yx+1]) { l2[x] = l1[x+1]; ind = l2[x]; }
				else{
					rgc++;
					colp[rgc] = 0; pon[rgc] = 0;
					in3 = rgc<<2;
					col[in3] = 0; col[in3+1] = 0; col[in3+2] = 0; col[in3+3] = 0;
					l2[x] = rgc;
					ind = rgc;
				}

				//ind = reg_number(r1, l[0], l[1], col, colp, pon, rgc, yx, x, w);
				in3 = ind<<2;
				col[in3] += r[yx]; col[in3+1] += g[yx]; col[in3+2] += b[yx]; col[in3+3]++;
			}
		}
		tm = l1; l1 = l2; l2 = tm;
	}
	printf("Numbers of regions  = %d\n", rgc);
	for(x=0; x < rgc; x++)  printf("colp[%d] = %d\n", x, colp[x]); //if(colp[x]> 7000)
	// Group the same regions
	for(x=0; x < rgc; x++){
		if(colp[x]){
			cp = x;
			while(1){
				printf("cp = %d ", cp);
				if(!colp[cp]) break;
				//if(cp == colp[colp[cp]]) break;
				cp = colp[cp];
			}
			printf("\n");
			colp[x] = cp;
			in3 = colp[x]<<2;
			in2 = x<<2;
			col[in3] 	+= col[in2];
			col[in3+1] 	+= col[in2+1];
			col[in3+2]  += col[in2+2];
			col[in3+3]  += col[in2+3];
		}
	}
	// Calculate average colors
	for(x=0; x < rgc; x++){
		if(!colp[x]){
			in3 = x<<2;
			col[in3] 	= col[in3]/col[in3+3];
			col[in3+1] 	= col[in3+1]/col[in3+3];
			col[in3+2]  = col[in3+2]/col[in3+3];
			nrg++;
		}
	}
	// Set colors to rest regions
	for(x=0; x < rgc; x++){
		if(colp[x]){
			in3 = colp[x]<<2;
			in2 = x<<2;
			col[in2] 	= col[in3];
			col[in2+1] 	= col[in3+1];
			col[in2+2]  = col[in3+2];
			col[in2+3]  = col[in3+3];
		}
		//printf("%d r = %d g = %d b = %d n = %d\n", x, col[in3], col[in3+1], col[in3+2], col[in3+3]);
	}
	printf("Numbers of real regions  = %d\n", nrg);
	return rgc;
}


uint32 seg_get_color2(uint8 *r, uint8 *g, uint8 *b, uint8 *r1, uint8 *col, uint32 *l1, uint32 *l2, uint32 w, uint32 h)
{
	uint32 i, j, y, x, yx, yxw, yw;
	uint32 in, rgc = 0;
	uint32 num, c[4], *tm;

	Vector v;
	uint8 cs[3];

	cs[0] = 255; cs[1] = 255; cs[2] = 255;
	v.x1 = 0; v.y1 = 0; v.x2 = w-1; v.y2 = 0;
	draw_line_3(r1, g, b, &v, w, cs);
	v.x1 = w-1; v.y1 = 0; v.x2 = w-1; v.y2 = h-1;
	draw_line_3(r1, g, b, &v, w, cs);
	v.x1 = w-1; v.y1 = h-1; v.x2 = 0; v.y2 = h-1;
	draw_line_3(r1, g, b, &v, w, cs);
	v.x1 = 0; v.y1 = 0; v.x2 = 0; v.y2 = h-1;
	draw_line_3(r1, g, b, &v, w, cs);

	for(y=1; y < h-1; y++){
		yw = y*w;
		for(x=1; x < w-1; x++){
			yx = yw + x;
			if(!r1[yx]){
				//printf("yx = %d r = %d\n", yx, r1[yx]);
				c[0] = 0; c[1] = 0; c[2] = 0; c[3] = 0;
				r1[yx] = 254; num = 1; l1[0] = yx; i = 0;
				while(num){
					for(j=0; j < num; j++){
						yxw = l1[j] - 1;
						if(!r1[yxw]) { r1[yxw] = 254; l2[i++] = yxw; }
						yxw = l1[j] - w;
						if(!r1[yxw]) { r1[yxw] = 254; l2[i++] = yxw; }
						yxw = l1[j] + 1;
						if(!r1[yxw]) { r1[yxw] = 254; l2[i++] = yxw; }
						yxw = l1[j] + w;
						if(!r1[yxw]) { r1[yxw] = 254; l2[i++] = yxw; }

						c[0] += r[l1[j]]; c[1] += g[l1[j]]; c[2] += b[l1[j]]; c[3]++;
					}
					num = i; i = 0;
					tm = l1; l1 = l2; l2 = tm;
				}
				in = rgc*3;
				col[in] = c[0]/c[3]; col[in+1] = c[1]/c[3]; col[in+2] = c[2]/c[3];
				rgc++;
				//if(rgc == 1) return 0;
			}
		}
	}
	printf("Numbers of regions  = %d\n", rgc);
	//for(i=0; i < rgc; i++)  printf("%5d  %3d %3d %3d\n", i, col[i*3], col[i*3+1], col[i*3+1]);
	return rgc;
}

uint32 seg_draw_color2(uint8 *r, uint8 *g, uint8 *b, uint8 *col, uint32 *l1, uint32 *l2, uint32 w, uint32 h)
{
	uint32 i, j, y, x, yx, yxw, yw;
	uint32 in, rgc = 0;
	uint32 num, c[4], *tm;

	for(y=0; y < h; y++){
		yw = y*w;
		for(x=0; x < w; x++){
			yx = yw + x;
			if(!r[yx]){
				c[0] = 0; c[1] = 0; c[2] = 0; c[3] = 0;
				num = 1; l1[0] = yx; i = 0;
				in = rgc*3;
				r[yx] = col[in]; g[yx] = col[in+1]; b[yx] = col[in+2];
				while(num){
					for(j=0; j < num; j++){
						yxw = l1[j] - 1;
						if(!r[yxw]) { r[yxw] = col[in]; g[yxw] = col[in+1]; b[yxw] = col[in+2];  l2[i++] = yxw; }
						yxw = l1[j] - w;
						if(!r[yxw]) { r[yxw] = col[in]; g[yxw] = col[in+1]; b[yxw] = col[in+2];  l2[i++] = yxw; }
						yxw = l1[j] + 1;
						if(!r[yxw]) { r[yxw] = col[in]; g[yxw] = col[in+1]; b[yxw] = col[in+2];  l2[i++] = yxw; }
						yxw = l1[j] + w;
						if(!r[yxw]) { r[yxw] = col[in]; g[yxw] = col[in+1]; b[yxw] = col[in+2];  l2[i++] = yxw; }

					}
					num = i; i = 0;
					tm = l1; l1 = l2; l2 = tm;
				}
				//in = rgc*3;
				//col[in] = c[0]/c[3]; col[in+1] = c[1]/c[3]; col[in+2] = c[2]/c[3];
				rgc++;
				//if(rgc == 8) return 0;
			}
		}
	}
	printf("Numbers of regions  = %d\n", rgc);
	//for(i=0; i < rgc; i++)  printf("%5d  %3d %3d %3d\n", i, col[i*3], col[i*3+1], col[i*3+1]);
	return rgc;
}

uint32 seg_draw_color1(uint8 *r, uint8 *g, uint8 *b, uint8 *r1, uint32 *col, uint32 *buf, uint32 w, uint32 h)
{
	uint32 y, x, yx, yx1, yw, fst = 0, yxf, cn, rc3, w1 = w-1, h1 = h-1;
	uint32 *l1, *l2, ind, in2, in3, *tm;
	uint32 nrg = 0, cp;
	int d,  rgc = -1;
	l1 = buf; l2 = &buf[w];
	//for(x=0; x < w; x++) l[0][x] = 0;

	for(y=1; y < h1; y++){
		yw = y*w;
		//for(x=0; x < w; x++) l[1][x] = 0;
		for(x=1; x < w1; x++){
			yx = yw + x;
			if(!r1[yx]){
				if(!r1[yx-w]) { l2[x]  = l1[x]; ind =  l2[x]; }
				else if(!r1[yx-1]) { l2[x]  = l2[x-1]; ind = l2[x]; }
				//else if(!r1[yx-w+1] && !r1[yx+1]) { l2[x] = l1[x+1]; ind = l2[x]; }
				else{
					rgc++;
					l2[x] = rgc;
					ind = rgc;
				}

				//ind = reg_number(r1, l[0], l[1], col, colp, pon, rgc, yx, x, w);
				in3 = ind<<2;
				r[yx] = col[in3]; g[yx] = col[in3+1]; b[yx] = col[in3+2];
			}
		}
		tm = l1; l1 = l2; l2 = tm;
	}
	printf("Numbers of regions  = %d\n", rgc);
	return rgc;
}

uint32 seg_init_regs(uint8 *r, uint8 *g, uint8 *b, uint8 *r1, uint16 *rg, uint32 *col, uint32 *l1, uint32 *l2, uint32 w, uint32 h)
{
	uint32 i, j, y, x, yx, yxw, yw;
	uint32 in, rgc = 1;
	uint32 num, *tm;//, c[4], ;

	Vector v;
	uint8 cs[3];

	cs[0] = 255; cs[1] = 255; cs[2] = 255;
	v.x1 = 0; v.y1 = 0; v.x2 = w-1; v.y2 = 0;
	draw_line_3(r1, g, b, &v, w, cs);
	v.x1 = w-1; v.y1 = 0; v.x2 = w-1; v.y2 = h-1;
	draw_line_3(r1, g, b, &v, w, cs);
	v.x1 = w-1; v.y1 = h-1; v.x2 = 0; v.y2 = h-1;
	draw_line_3(r1, g, b, &v, w, cs);
	v.x1 = 0; v.y1 = 0; v.x2 = 0; v.y2 = h-1;
	draw_line_3(r1, g, b, &v, w, cs);

	for(y=1; y < h-1; y++){
		yw = y*w;
		for(x=1; x < w-1; x++){
			yx = yw + x;
			if(!r1[yx]){
				//printf("yx = %d r = %d\n", yx, r1[yx]);
				in = rgc<<2;
				col[in] = 0; col[in+1] = 0; col[in+2] = 0; col[in+3] = 0;
				r1[yx] = 1; num = 1; l1[0] = yx; i = 0;
				while(num){
					for(j=0; j < num; j++){
						yxw = l1[j] - 1;
						if(!r1[yxw]) { r1[yxw] = 1; l2[i++] = yxw; }
						yxw = l1[j] - w;
						if(!r1[yxw]) { r1[yxw] = 1; l2[i++] = yxw; }
						yxw = l1[j] + 1;
						if(!r1[yxw]) { r1[yxw] = 1; l2[i++] = yxw; }
						yxw = l1[j] + w;
						if(!r1[yxw]) { r1[yxw] = 1; l2[i++] = yxw; }
						rg[l1[j]] = rgc;
						col[in] += r[l1[j]]; col[in+1] += g[l1[j]]; col[in+2] += b[l1[j]]; col[in+3]++;
					}
					num = i; i = 0;
					tm = l1; l1 = l2; l2 = tm;
				}
				//in = rgc*3;
				//col[in] = c[0]/c[3]; col[in+1] = c[1]/c[3]; col[in+2] = c[2]/c[3];
				rgc++;
				//if(rgc == 1) return 0;
			}
		}
	}
	printf("Numbers of regions  = %d\n", rgc);
	//for(i=0; i < rgc; i++)  printf("%5d  %3d %3d %3d\n", i, col[i*3], col[i*3+1], col[i*3+1]);
	return rgc;
}

uint32 seg_fill_regs(uint8 *r, uint8 *g, uint8 *b, uint8 *r1, uint16 *rg, uint32 *col, uint32 *l1, uint32 rgc, uint32 w, uint32 h)
{
	uint32 i, j, y, x, yx, yxw, yw;
	uint32 in;
	uint32 num, min;//, c[4], *tm;
	int d;

	for(y=1; y < h-1; y++){
		yw = y*w;
		for(x=1; x < w-1; x++){
			yx = yw + x;
			if(r1[yx] != 1){
				i = 0;
				while(!rg[yx]){
					l1[i++] = yx;
					min = r1[yx]; d = 0;
					if(r1[yx-1  ] < min) { min = r1[yx-1  ]; d = -1;}
					if(r1[yx  -w] < min) { min = r1[yx  -w]; d = -w;}
					if(r1[yx+1  ] < min) { min = r1[yx+1  ]; d = 1;}
					if(r1[yx  +w] < min) { min = r1[yx  +w]; d = w;}
					yx = yx + d;
					printf("yx = %d rg = %d gr = %d \n", yx, rg[yx] ,r1[yx]);
				}
				printf("i = %d\n",i );
				for(j=0; j < i; j++) {
					rg[l1[j]] = rg[yx];
					col[rg[yx]] += r[l1[j]]; col[rg[yx]+1] += g[l1[j]]; col[rg[yx]+2] += b[l1[j]]; col[rg[yx]+3]++;
				}
			}
				//if(rgc == 1) return 0;
		}
	}
	printf("Numbers of regions  = %d\n", rgc);
	for(i=0; i < rgc; i++)  {
		in = rgc<<2;
		col[in] = col[in]/col[in+3]; col[in+1] = col[in+1]/col[in+3]; col[in+2] = col[in+2]/col[in+3];
		//printf("%5d  %3d %3d %3d\n", i, col[i*3], col[i*3+1], col[i*3+1]);
	}
	return rgc;
}

static void inline set_dir(uint8 *img, uint32 yx, uint32 w,  int d)
{
	if		(d ==  0) img[yx] = 0;
	else if	(d == -1) img[yx] = 1;
	else if	(d == -w) img[yx] = 2;
	else if	(d ==  1) img[yx] = 3;
	else if	(d ==  w) img[yx] = 4;
}

static inline uint32 get_dir(uint8 *img, uint32 yx, uint32 yx1, uint32 w)
{
	if(img[yx] > 4) return yx1 + 1;
	if(img[yx] == 0) return yx1;
	if(img[yx] == 1) return yx - 1;
	if(img[yx] == 2) return yx - w;
	if(img[yx] == 3) return yx + 1;
	if(img[yx] == 4) return yx + w;
	/*
	if(img[yx] == 5) return yx-1-w;
	if(img[yx] == 6) return yx+1-w;
	if(img[yx] == 7) return yx+1+w;
	if(img[yx] == 8) return yx-1+w;*/

}

static inline uint32 chk_dir(uint32 *img, uint32 yx, uint32 yx1, uint32 w)
{
	if(img[yx] > 4) return yx1 + 1;
	if(img[yx] == 0) return yx1;
	if(img[yx] == 1) return yx - 1;
	if(img[yx] == 2) return yx - w;
	if(img[yx] == 3) return yx + 1;
	if(img[yx] == 4) return yx + w;
	//if(img[yx] == 5) { img[yx] = 6; return yx1+1; }
	//if(img[yx] == 5) { img[yx] = 6; (*i)++; return yx1+1; }
	/*
	if(img[yx] == 5) return yx-1-w;
	if(img[yx] == 6) return yx+1-w;
	if(img[yx] == 7) return yx+1+w;
	if(img[yx] == 8) return yx-1+w;*/

}

void seg_fall_forest(uint8 *img, uint32 *img1, uint32 w, uint32 h)
{
	uint32 y, x, yx, yw, sq = w*h, dir, w1 = w-2, h1 = h-2, min;
	//The one pixel border
	for(x=0; x < w; x++) img1[x] = 5;
	img1[w] = 5; for(x=1; x < w-1; x++) img1[w+x] = 4; img1[w+x] = 5;

	for(y=2; y < h1; y++){
		yw = y*w;
		img1[yw] = 5; img1[yw+1] = 3;
		for(x=2; x < w1; x++){
			yx = yw + x;
			if(img[yx]) {
				min = img[yx]; img1[yx] = 0;
                if(img[yx-1  ] < min) { min = img[yx-1  ]; img1[yx] = 1; }
                if(img[yx  -w] < min) { min = img[yx  -w]; img1[yx] = 2; }
                if(img[yx+1  ] < min) { min = img[yx+1  ]; img1[yx] = 3; }
                if(img[yx  +w] < min) { min = img[yx  +w]; img1[yx] = 4; }
 			} else img1[yx] = 0;
		}
		img1[yx+1] = 1; img1[yx+2] = 5;
	}
	yw = y*w;
	img1[yw] = 5; for(x=1; x < w-1; x++) img1[yw+x] = 2; img1[yw+x] = 5;
	yw = yw+w;
	for(x=0; x < w; x++) img1[yw+x] = 5;
}

static inline void check_max(uint8 *img, uint8 *con, uint32 yx, uint32 w, uint32 reg)
{
	uint32 yx1, cn = 0;
	if(!con[yx]){
		if(img[yx-1] != reg) { cn++; yx1 = yx-1; }
		if(img[yx-w] != reg) { cn++; yx1 = yx-w; }
		if(img[yx+1] != reg) { cn++; yx1 = yx+1; }
		if(img[yx+w] != reg) { cn++; yx1 = yx+w; }

		if(cn == 1) { con[yx1] = 255;
		//if(img[yx] > img[yx1]) con[yx] = 255;
		//else con[yx1] = 255;
		}
		else {//if(cn == 2 || cn == 3){
			con[yx] = 255;
		}
	}
}

static inline uint32 get_next(uint8 *img, uint32 yx, uint32 *yxp, uint32 w, uint32 reg)
{
	uint32 yx1;
	//printf("yx = %d yx1 = %d\n", yx, *yxp);
	//One pixel
	if(	img[yx-1] != reg &&
		img[yx-w] != reg &&
		img[yx+1] != reg &&
		img[yx+w] != reg )  return yx;

	if		(*yxp == yx-1  ) { *yxp = yx; goto m1;}
	else if	(*yxp == yx-1-w) { *yxp = yx; goto m2;}
	else if	(*yxp == yx-w  ) { *yxp = yx; goto m3;}
	else if	(*yxp == yx-w+1) { *yxp = yx; goto m4;}
	else if	(*yxp == yx+1  ) { *yxp = yx; goto m5;}
	else if	(*yxp == yx+1+w) { *yxp = yx; goto m6;}
	else if	(*yxp == yx+w  ) { *yxp = yx; goto m7;}
	else if	(*yxp == yx+w-1) { *yxp = yx; goto m8;}

	while(1){
		m1:
		yx1 = yx-1-w;
		if(img[yx1+w] != reg && img[yx1] == reg) return yx1;
		m2:
		yx1 = yx-w;
		if(img[yx1-1] != reg && img[yx1] == reg) return yx1;
		m3:
		yx1 = yx-w+1;
		if(img[yx1-1] != reg && img[yx1] == reg) return yx1;
		m4:
		yx1 = yx+1;
		if(img[yx1-w] != reg && img[yx1] == reg) return yx1;
		m5:
		yx1 = yx+1+w;
		if(img[yx1-w] != reg && img[yx1] == reg) return yx1;
		m6:
		yx1 = yx+w;
		if(img[yx1+1] != reg && img[yx1] == reg) return yx1;
		m7:
		yx1 = yx+w-1;
		if(img[yx1+1] != reg && img[yx1] == reg) return yx1;
		m8:
		yx1 = yx-1;
		if(img[yx1+w] != reg && img[yx1] == reg) return yx1;
	}
}

uint32 seg_group_reg(uint32 *reg, uint32 *buff, uint32 w, uint32 h)
{
	uint32 i, j, y, x, yx, yxw, yxw1, yw, h1 = h-1, w1 = w-1;
	uint32 in, rgc = 6, tmp,  num;
	uint32 *l1 = buff, *l2 = &buff[w*h>>2], *tm;

	for(y=1; y < h1; y++){
		yw = y*w;
		for(x=1; x < w1; x++){
			yx = yw + x;
			if(!reg[yx]){
				//printf("x = %d y = %d img = %d\n", x, y, img[yx]);
				reg[yx] = rgc; num = 1; l1[0] = yx; i = 0;
				//printf("reg = %d\n", rgc);
				while(num){
					for(j=0; j < num; j++){
						/*
						printf("num = %d reg = %d x = %d y = %d\n", num, reg[yx], l1[j]%w, l1[j]/w);
						printf("%4d %4d %4d\n%4d %4d %4d\n%4d %4d %4d\n\n",
								reg[l1[j]-w-1], reg[l1[j]-w], reg[l1[j]-w+1],
								reg[l1[j]-1], reg[l1[j]], reg[l1[j]+1],
								reg[l1[j]+w-1], reg[l1[j]+w], reg[l1[j]+w+1]);
						*/
						yxw = l1[j] - 1;
						//tmp = chk_dir(img, yxw, l1[j], w); //printf("%3d %3d\n", yxw, tmp);
						if(l1[j] == chk_dir(reg, yxw, l1[j], w)) { reg[yxw] = rgc; l2[i++] = yxw; }
						yxw = l1[j] - w;
						//tmp = chk_dir(img, yxw, l1[j], w); //printf("%3d %3d\n", yxw, tmp);
						if(l1[j] == chk_dir(reg, yxw, l1[j], w)) { reg[yxw] = rgc; l2[i++] = yxw; }
						yxw = l1[j] + 1;
						//tmp = chk_dir(img, yxw, l1[j], w); //printf("%3d %3d\n", yxw, tmp);
						if(l1[j] == chk_dir(reg, yxw, l1[j], w)) { reg[yxw] = rgc; l2[i++] = yxw; }
						yxw = l1[j] + w;
						//tmp = chk_dir(img, yxw, l1[j], w); //printf("%3d %3d\n", yxw, tmp);
						if(l1[j] == chk_dir(reg, yxw, l1[j], w)) { reg[yxw] = rgc; l2[i++] = yxw; }
						//if(i > fs || c ) grad[l1[j]] = 0;
						//reg[l1[j]] = rgc;
						//printf("i = %d\n", i);
					}
					num = i; i = 0;
					tm = l1; l1 = l2; l2 = tm;
				}
				rgc++;
				//Find border of region
				/*
				yxw = l2[0]; yxw1 = l2[0];
				do{

					//printf("yx = %d yx1 = %d\n", yxw, yxw1);
					printf("%3d %3d %3d\n%3d %3d %3d\n%3d %3d %3d\n\n",
							img[yxw-w-1], img[yxw-w], img[yxw-w+1],
							img[yxw-1], img[yxw], img[yxw+1],
							img[yxw+w-1], img[yxw+w], img[yxw+w+1]);
					printf("%3d %3d %3d\n%3d %3d %3d\n%3d %3d %3d\n\n",
							con[yxw-w-1], con[yxw-w], con[yxw-w+1],
							con[yxw-1], con[yxw], con[yxw+1],
							con[yxw+w-1], con[yxw+w], con[yxw+w+1]);

					check_max(img, con, yxw, w, rgc);
					//yxw1 = yxw;
					yxw = get_next(img, yxw, &yxw1, w, rgc);
				}while(yxw != l2[0]);
				*/
				//if(rgc == 1) return 0;
			}
		}
	}
	rgc-=6;
	printf("Numbers of regions  = %d\n", rgc);
	//for(i=0; i < rgc; i++)  printf("%5d  %3d %3d %3d\n", i, col[i*3], col[i*3+1], col[i*3+1]);
	return rgc;
}

uint32 seg_group_pixels(uint8 *r, uint8 *g, uint8 *b, uint8 *r1, uint8 *grad, uint32 *rg, uint8 *col, uint32 *l1, uint32 *l2, uint32 w, uint32 h)
{
	uint32 i, j, y, x, yx, yxw, yw, w1 = w-1, h1 = h-1;
	uint32 in, rgc = 1, tmp, fs, max;
	uint32 num, c[4], *tm;
	int d;

	for(y=1; y < h1; y++){
		yw = y*w;
		for(x=1; x < w1; x++){
			yx = yw + x;
			if(!r1[yx]){
				//printf("yx = %d r = %d\n", yx, r1[yx]);
				c[0] = 0; c[1] = 0; c[2] = 0; c[3] = 0;
				r1[yx] = 9; num = 1; l1[0] = yx; i = 0;
				//printf("reg = %d\n", rgc);
				while(num){
					for(j=0; j < num; j++){
						//printf("j = %d\n", j);
						//printf("%3d %3d %3d\n%3d %3d %3d\n%3d %3d %3d\n",
						//		r1[yx-w-1], r1[yx-w], r1[yx-w+1],
						//		r1[yx-1], r1[yx], r1[yx+1],
						//		r1[yx+w-1], r1[yx+w], r1[yx+w+1]);
						yxw = l1[j] - 1; fs = i;
						tmp = get_dir(r1, yxw, l1[j], w); //printf("%3d %3d\n", yxw, tmp);
						if(l1[j] == tmp) { r1[yxw] = 9; l2[i++] = yxw; }
						yxw = l1[j] - w;
						tmp = get_dir(r1, yxw, l1[j], w); //printf("%3d %3d\n", yxw, tmp);
						if(l1[j] == tmp) { r1[yxw] = 9; l2[i++] = yxw; }
						yxw = l1[j] + 1;
						tmp = get_dir(r1, yxw, l1[j], w); //printf("%3d %3d\n", yxw, tmp);
						if(l1[j] == tmp) { r1[yxw] = 9; l2[i++] = yxw; }
						yxw = l1[j] + w;
						tmp = get_dir(r1, yxw, l1[j], w); //printf("%3d %3d\n", yxw, tmp);
						if(l1[j] == tmp) { r1[yxw] = 9; l2[i++] = yxw; }
						/*
						if(fs == i) {
							max = grad[l1[j]]; d = l1[j];
							yxw = l1[j] - 1;
							if(grad[yxw] > max) { max = grad[yxw]; d = yxw;}
							yxw = l1[j] - w;
							if(grad[yxw] > max) { max = grad[yxw]; d = yxw;}
							yxw = l1[j] + 1;
							if(grad[yxw] > max) { max = grad[yxw]; d = yxw;}
							yxw = l1[j] + w;
							if(grad[yxw] > max) { max = grad[yxw]; d = yxw;}
							grad[d] = 255;
						}*/
						/*
						yxw = l1[j] - 1-w;
						tmp = get_dir(r1, yxw, l1[j], w); //printf("%3d %3d\n", yxw, tmp);
						if(l1[j] == tmp) { r1[yxw] = 9; l2[i++] = yxw; }
						yxw = l1[j] - w+1;
						tmp = get_dir(r1, yxw, l1[j], w); //printf("%3d %3d\n", yxw, tmp);
						if(l1[j] == tmp) { r1[yxw] = 9; l2[i++] = yxw; }
						yxw = l1[j] + 1+w;
						tmp = get_dir(r1, yxw, l1[j], w); //printf("%3d %3d\n", yxw, tmp);
						if(l1[j] == tmp) { r1[yxw] = 9; l2[i++] = yxw; }
						yxw = l1[j] + w-1;
						tmp = get_dir(r1, yxw, l1[j], w); //printf("%3d %3d\n", yxw, tmp);
						if(l1[j] == tmp) { r1[yxw] = 9; l2[i++] = yxw; }*/

						c[0] += r[l1[j]]; c[1] += g[l1[j]]; c[2] += b[l1[j]]; c[3]++;
						rg[l1[j]] = rgc;
						//printf("i = %d\n", i);
					}
					num = i; i = 0;
					tm = l1; l1 = l2; l2 = tm;
				}
				in = rgc*3;
				col[in] = c[0]/c[3]; col[in+1] = c[1]/c[3]; col[in+2] = c[2]/c[3];
				rgc++;
				//if(rgc == 1) return 0;
			}
		}
	}
	printf("Numbers of regions  = %d\n", rgc);
	//for(i=0; i < rgc; i++)  printf("%5d  %3d %3d %3d\n", i, col[i*3], col[i*3+1], col[i*3+1]);
	return rgc;
}

void seg_draw_reg(uint8 *r, uint8 *g, uint8 *b, uint32 *rg, uint8 *col, uint32 w, uint32 h)
{
	uint32 y, x, yx, yw, sq = w*h, dir, w1 = w-1, h1 = h-1, min, in, cn, yxl;
	int d;

	for(y=1; y < h1; y++){
		yw = y*w;
		for(x=1; x < w1; x++){
			yx = yw + x;
			in = rg[yx]*3;
			r[yx] = col[in]; g[yx] = col[in+1]; b[yx] = col[in+2];
			/*
			cn = 0;
            if(rg[yx-1] != rg[yx]) { cn++; yxl = yx-1; }
            if(rg[yx-w] != rg[yx]) { cn++; yxl = yx-w; }
            if(rg[yx+1] != rg[yx]) { cn++; yxl = yx+1; }
            if(rg[yx+w] != rg[yx]) { cn++; yxl = yx+w; }

            if(cn == 3) { in = rg[yxl]*3; r[yx] = col[in]; g[yx] = col[in+1]; b[yx] = col[in+2];}
            else { r[yx] = col[in]; g[yx] = col[in+1]; b[yx] = col[in+2]; }
            */
		}
	}
}

static inline uint32 loc_min(uint8 *img, uint32 yx, uint32 w)
{
	if( img[yx-1] 	>= img[yx] &&
		img[yx-w]	>= img[yx] &&
		img[yx+1] 	>= img[yx] &&
		img[yx+w] 	>= img[yx] &&
		img[yx-1-w] >= img[yx] &&
		img[yx+1-w] >= img[yx] &&
		img[yx-1+w] >= img[yx] &&
		img[yx+1+w] >= img[yx] ) return 1;
	else return 0;
}

uint32 seg_new_contur(uint8 *grad, uint8 *con, uint32 *l1, uint32 *l2, uint32 w, uint32 h)
{
	uint32 i, j, y, x, yx, yxw, yw, fs, c;
	uint32 in, rgc = 0;
	uint32 num, *tm;
	uint32 tmp = 0;

	Vector v;
	uint8 cs[3];

	cs[0] = 255; cs[1] = 255; cs[2] = 255;
	v.x1 = 1; v.y1 = 1; v.x2 = w-2; v.y2 = 1;
	draw_line_3(grad, grad, grad, &v, w, cs);
	v.x1 = w-2; v.y1 = 1; v.x2 = w-2; v.y2 = h-2;
	draw_line_3(grad, grad, grad, &v, w, cs);
	v.x1 = w-2; v.y1 = h-2; v.x2 = 1; v.y2 = h-2;
	draw_line_3(grad, grad, grad, &v, w, cs);
	v.x1 = 1; v.y1 = 1; v.x2 = 1; v.y2 = h-2;
	draw_line_3(grad, grad, grad, &v, w, cs);

	cs[0] = 1; cs[1] = 1; cs[2] = 1;
	v.x1 = 0; v.y1 = 0; v.x2 = w-1; v.y2 = 0;
	draw_line_3(con, con, con, &v, w, cs);
	v.x1 = w-1; v.y1 = 0; v.x2 = w-1; v.y2 = h-1;
	draw_line_3(con, con, con, &v, w, cs);
	v.x1 = w-1; v.y1 = h-1; v.x2 = 0; v.y2 = h-1;
	draw_line_3(con, con, con, &v, w, cs);
	v.x1 = 0; v.y1 = 0; v.x2 = 0; v.y2 = h-1;
	draw_line_3(con, con, con, &v, w, cs);

	for(y=1; y < 40; y++){
		yw = y*w;
		for(x=1; x < 40; x++){
			yx = yw + x;
			printf("%3d ", grad[yx]);
		}
		printf("\n");
	}

	for(y=1; y < h-1; y++){
		yw = y*w;
		for(x=1; x < w-1; x++){
			yx = yw + x;
			if(!con[yx] && (!grad[yx] || loc_min(grad, yx, w))){
				printf("rgc = %d grad = %d x = %d y = %d\n", rgc, grad[yx], x, y);
				con[yx] = 50; num = 1; l1[0] = yx; i = 0;
				while(num){
					printf("num = %d\n", tmp);
					for(j=0; j < num; j++){
						//printf("grad = %d x = %d y = %d\n", grad[l1[j]], l1[j]%w,  l1[j]/w);

						printf("%3d %3d %3d\n%3d %3d %3d\n%3d %3d %3d\n\n",
								grad[l1[j]-w-1], grad[l1[j]-w], grad[l1[j]-w+1],
								grad[l1[j]-1], grad[l1[j]], grad[l1[j]+1],
								grad[l1[j]+w-1], grad[l1[j]+w], grad[l1[j]+w+1]);
						printf("%3d %3d %3d\n%3d %3d %3d\n%3d %3d %3d\n\n",
								con[l1[j]-w-1], con[l1[j]-w], con[l1[j]-w+1],
								con[l1[j]-1], con[l1[j]], con[l1[j]+1],
								con[l1[j]+w-1], con[l1[j]+w], con[l1[j]+w+1]);
						//Check for 0
						yxw = l1[j] - 1; fs = i; c = 0;
						if(!con[yxw]){
							c++;
							if(!grad[yxw] || grad[yxw] >= grad[l1[j]]) {
								con[yxw] = 50; l2[i++] = yxw;
							}
						}
						else if (con[yxw] == 255 ){

						}
						yxw = l1[j] - w;
						if(!con[yxw]){
							c++;
							if(!grad[yxw] || grad[yxw] >= grad[l1[j]]) {
								con[yxw] = 50; l2[i++] = yxw;
							}
						}
						else if (con[yxw] == 255 ){

						}
						yxw = l1[j] + 1;
						if(!con[yxw]){
							c++;
							if(!grad[yxw] || grad[yxw] >= grad[l1[j]]) {
								con[yxw] = 50; l2[i++] = yxw;
							}
						}
						else if (con[yxw] == 255 ){

						}
						yxw = l1[j] + w;
						if(!con[yxw]){
							c++;
							if(!grad[yxw] || grad[yxw] >= grad[l1[j]]) {
								con[yxw] = 50; l2[i++] = yxw;
							}
						}
						else if (con[yxw] == 255 ){

						}
						fs = i-fs;
						if(c > fs) con[l1[j]] = 255;
					}
					num = i; i = 0;
					tm = l1; l1 = l2; l2 = tm; tmp++;
					if(tmp == 5)return 0;
				}
				return 0;
				rgc++;
				//if(rgc == 1) return 0;
			}
		}
	}
	printf("Numbers of regions  = %d\n", rgc);
	//for(i=0; i < rgc; i++)  printf("%5d  %3d %3d %3d\n", i, col[i*3], col[i*3+1], col[i*3+1]);
	return rgc;
}


void seg_draw_grad(uint8 *grad, uint8 *out, uint32 *rg, uint32 w, uint32 h)
{
	uint32 y, x, yx, yxw, yw, w1 = w-1, h1 = h-1;

	for(y=1; y < h1; y++){
		yw = y*w;
		for(x=1; x < w1; x++){
			yx = yw + x;
            if		(rg[yx-1] != rg[yx]) out[yx] = grad[yx];
            else if	(rg[yx-w] != rg[yx]) out[yx] = grad[yx];
            else if	(rg[yx+1] != rg[yx]) out[yx] = grad[yx];
            else if	(rg[yx+w] != rg[yx]) out[yx] = grad[yx];
            else { out[yx] = 0;}
		}
 	}
}

void seg_max_rise(uint8 *img, uint8 *img1, uint32 w, uint32 h)
{
	uint32 y, x, yx, yw, sq = w*h, dir, w1 = w-1, h1 = h-1, max;
	int d;

	for(y=1; y < h1; y++){
		yw = y*w;
		for(x=1; x < w1; x++){
			yx = yw + x;
			if(img[yx]) {
				max = img[yx]; d = 0; img1[yx] = 0;
                if(img[yx-1  ] > max) { max = img[yx-1  ]; img1[yx] = 1; }
                if(img[yx  -w] > max) { max = img[yx  -w]; img1[yx] = 2; }
                if(img[yx+1  ] > max) { max = img[yx+1  ]; img1[yx] = 3; }
                if(img[yx  +w] > max) { max = img[yx  +w]; img1[yx] = 4; }
 			} else img1[yx] = 0;
		}
	}
}

void seg_max_con(uint8 *img, uint8 *img1, uint32 w, uint32 h)
{
	uint32 y, x, yx, yw, sq = w*h, w1 = w-1, h1 = h-1, max, cn;
	//int d;

	for(y=1; y < h1; y++){
		yw = y*w;
		for(x=1; x < w1; x++){
			yx = yw + x;
			if(img[yx]) {
				cn = 0;
                if(img[yx-1] == 3) cn++;
                if(img[yx-w] == 4) cn++;
                if(img[yx+1] == 1) cn++;
                if(img[yx+w] == 2) cn++;
                if(cn < 1) img1[yx] = 255;
 			}
		}
	}
}

uint32 seg_group_pixels1(uint8 *r, uint8 *g, uint8 *b, uint8 *r1, uint32 *rg, uint8 *col, uint32 *l1, uint32 *l2, uint32 w, uint32 h)
{
	uint32 i, j, y, x, yx, yxw, yw, w1 = w-1, h1 = h-1;
	uint32 in, rgc = 1, tmp;
	uint32 num, c[4], *tm;

	for(y=1; y < h1; y++){
		yw = y*w;
		for(x=1; x < w1; x++){
			yx = yw + x;
			if(!r1[yx]){
				//printf("yx = %d r = %d\n", yx, r1[yx]);
				c[0] = 0; c[1] = 0; c[2] = 0; c[3] = 0;
				r1[yx] = 9; num = 1; l1[0] = yx; i = 0;
				//printf("reg = %d\n", rgc);
				while(num){
					for(j=0; j < num; j++){
						//printf("j = %d\n", j);
						//printf("%3d %3d %3d\n%3d %3d %3d\n%3d %3d %3d\n",
						//		r1[yx-w-1], r1[yx-w], r1[yx-w+1],
						//		r1[yx-1], r1[yx], r1[yx+1],
						//		r1[yx+w-1], r1[yx+w], r1[yx+w+1]);
						yxw = l1[j] - 1;
						tmp = get_dir(r1, yxw, l1[j], w); //printf("%3d %3d\n", yxw, tmp);
						if(l1[j] == tmp) { r1[yxw] = 9; l2[i++] = yxw; }
						yxw = l1[j] - w;
						tmp = get_dir(r1, yxw, l1[j], w); //printf("%3d %3d\n", yxw, tmp);
						if(l1[j] == tmp) { r1[yxw] = 9; l2[i++] = yxw; }
						yxw = l1[j] + 1;
						tmp = get_dir(r1, yxw, l1[j], w); //printf("%3d %3d\n", yxw, tmp);
						if(l1[j] == tmp) { r1[yxw] = 9; l2[i++] = yxw; }
						yxw = l1[j] + w;
						tmp = get_dir(r1, yxw, l1[j], w); //printf("%3d %3d\n", yxw, tmp);
						if(l1[j] == tmp) { r1[yxw] = 9; l2[i++] = yxw; }
						/*
						yxw = l1[j] - 1-w;
						tmp = get_dir(r1, yxw, l1[j], w); //printf("%3d %3d\n", yxw, tmp);
						if(l1[j] == tmp) { r1[yxw] = 9; l2[i++] = yxw; }
						yxw = l1[j] - w+1;
						tmp = get_dir(r1, yxw, l1[j], w); //printf("%3d %3d\n", yxw, tmp);
						if(l1[j] == tmp) { r1[yxw] = 9; l2[i++] = yxw; }
						yxw = l1[j] + 1+w;
						tmp = get_dir(r1, yxw, l1[j], w); //printf("%3d %3d\n", yxw, tmp);
						if(l1[j] == tmp) { r1[yxw] = 9; l2[i++] = yxw; }
						yxw = l1[j] + w-1;
						tmp = get_dir(r1, yxw, l1[j], w); //printf("%3d %3d\n", yxw, tmp);
						if(l1[j] == tmp) { r1[yxw] = 9; l2[i++] = yxw; }*/

						c[0] += r[l1[j]]; c[1] += g[l1[j]]; c[2] += b[l1[j]]; c[3]++;
						rg[l1[j]] = rgc;
						//printf("i = %d\n", i);
					}
					num = i; i = 0;
					tm = l1; l1 = l2; l2 = tm;
				}
				in = rgc*3;
				col[in] = c[0]/c[3]; col[in+1] = c[1]/c[3]; col[in+2] = c[2]/c[3];
				rgc++;
				//if(rgc == 1) return 0;
			}
		}
	}
	printf("Numbers of regions  = %d\n", rgc);
	//for(i=0; i < rgc; i++)  printf("%5d  %3d %3d %3d\n", i, col[i*3], col[i*3+1], col[i*3+1]);
	return rgc;
}


/*
uint32 seg_vector(uint8 *con, Vertex *vx, Vertex **vp, Line *ln, uint32 vxc, uint32 w)
{
	uint32 y, x, yx, cn;
	int i, j, d, d1, fs, sc;
	//print_around(con, vp[802]->y*w + vp[802]->x, w);
	//printf("n = %d %o %o\n", vp[802]->n, vp[802]->di, vp[802]->cn);
	//i = 802;{
	for(i=0; i < vxc; i++){
		//printf("Virtex = %d n = %d\n", i, vp[i]->n);
		//printf("dir = %o %o\n", vp[i]->di, vp[i]->cn);
		for(j=0; j < vp[i]->n; j++){
			yx = vp[i]->y*w + vp[i]->x;
			//if(i == 802) {
			//	print_around(con, yx, w);
			//	printf("n = %d %o %o\n", vp[i]->n, vp[i]->di, vp[i]->cn);
			//}
			if(get_next_dir(vp[i], &d, w)){
				fs = 0; sc = 0; cn = 0;
				//printf("d = %d %o %o\n", d, vp[i]->di, vp[i]->cn);
				while(1){
					yx = yx + d;
					//printf("y = %d x = %d d = %d w = %d\n", (yx-d)/w, (yx-d)%w, d, w);
					if(d == 0 && con[yx-d1] == 255){
						remove_dir(&vx[yx-d1], d1, w);
						j--;
						break;
					}
					if(con[yx] == 255) {
						finish_dir(&vx[yx], -d, w);
						break;
					}
					if(d == 0){
						printf("d = %d\n", d1);
						print_around(con, yx, w);
						con[yx] = 128;
						break;
					}
					d1 = d;
					if(is_new_line1(d, &cn, &fs, &sc)){
						break;
					}
					d = dir(con, yx, w, -d);
					con[yx] = 0;
				}
			}
		}
	}
	//i = 802;  if(vp[i]->di != vp[i]->cn) {
	for(i=0; i < vxc; i++) if(vp[i]->di != vp[i]->cn) {
		printf("%d n = %d %o %o %o\n", i, vp[i]->n, vp[i]->di, vp[i]->cn, vp[i]->cn^vp[i]->di);
		con[vp[i]->y*w + vp[i]->x-w] = 255;
		con[vp[i]->y*w + vp[i]->x-1] = 255;
		con[vp[i]->y*w + vp[i]->x+w] = 255;
		con[vp[i]->y*w + vp[i]->x+1] = 255;
	}
	printf("Numbers of vertexs  = %6d\n", vxc);
	return vxc;

}
*/
//Mean-shift 3d algorithm

void mean_print(uint32 *i3d, uint16 *lut, p3d *d, p3d *p, int r)
{
	int x, y, z, zy, yx, xyz, w2 = d->x*d->y, mask = 0x7FFFFFFF;
	//int xm = 0, ym = 0, zm = 0, ms = 0;
	p3d b, e;

	b.x = (p->x - r) < 0 ? 0 : p->x - r;
	e.x = (p->x + r) > (d->x - 1) ? d->x - 1 : p->x + r;
	b.y = (p->y - r) < 0 ? 0 : p->y - r;
	e.y = (p->y + r) > (d->y - 1) ? d->y - 1 : p->y + r;
	b.z = (p->z - r) < 0 ? 0 : p->z - r;
	e.z = (p->z + r) > (d->z - 1) ? d->z - 1 : p->z + r;
	//printf("r = %d x = %d y = %d z = %d\n", r, p->x, p->y, p->z);

	for(z=b.z; z <= e.z; z++){
		zy = z*w2;
		//printf("zy = %d\n", zy);
		for(y=b.y; y <= e.y; y++){
			yx = zy + y*d->x;
			//printf("yx = %d\n", yx);
			for(x=b.x; x <= e.x; x++){
				xyz = yx + x;
				//printf("i3d[%d] = %d\n", xyz, i3d[xyz]);
				//if(i3d[xyz]){
					printf("%6d ", i3d[xyz]&mask);
				//}
			}
			printf("  ");
			for(x=b.x; x <= e.x; x++){
				xyz = yx + x;
				//printf("i3d[%d] = %d\n", xyz, i3d[xyz]);
				//if(i3d[xyz]){
					printf("%6d ", lut[xyz]);
				//}
			}
			printf("\n");
		}
		printf("\n\n");
	}
	//printf("ms = %d xm = %d ym = %d zm = %d\n", ms, xm, ym, zm);
	//p->x = xm/ms; p->y = ym/ms; p->z = zm/ms;
}

uint32 center_mass(uint32 *i3d, p3d *d, p3d *p, int r)
{
	int x, y, z, zy, yx, xyz, w2 = d->x*d->y, mask = 0x7FFFFFFF;
	int xm = 0, ym = 0, zm = 0, ms = 0;
	p3d b, e;

	b.x = (p->x - r) < 0 ? 0 : p->x - r;
	e.x = (p->x + r) > (d->x - 1) ? d->x - 1 : p->x + r;
	b.y = (p->y - r) < 0 ? 0 : p->y - r;
	e.y = (p->y + r) > (d->y - 1) ? d->y - 1 : p->y + r;
	b.z = (p->z - r) < 0 ? 0 : p->z - r;
	e.z = (p->z + r) > (d->z - 1) ? d->z - 1 : p->z + r;
	//printf("r = %d x = %d y = %d z = %d\n", r, p->x, p->y, p->z);

	for(z=b.z; z <= e.z; z++){
		zy = z*w2;
		//printf("zy = %d\n", zy);
		for(y=b.y; y <= e.y; y++){
			yx = zy + y*d->x;
			//printf("yx = %d\n", yx);
			for(x=b.x; x <= e.x; x++){
				xyz = yx + x;
				//printf("i3d[%d] = %d\n", xyz, i3d[xyz]);
				if(i3d[xyz]){
					ms += i3d[xyz]&mask;
					xm += (i3d[xyz]&mask)*x;
					ym += (i3d[xyz]&mask)*y;
					zm += (i3d[xyz]&mask)*z;
				}
			}
		}
	}
	//printf("ms = %d xm = %d ym = %d zm = %d\n", ms, xm, ym, zm);
	p->x = xm/ms; p->y = ym/ms; p->z = zm/ms;
}

void fill_3d_array(int16 *r, int16 *g, int16 *b, uint32 w, uint32 h, uint32 *i3d, uint32 bpp, p3d *q)
{
	int i, sz = w*h, d = 1<<bpp, w1 = (d>>q->x), w2 = w1*(d>>q->y), size = w2*(d>>q->z), sh = d>>1;
	memset(i3d, 0, size*sizeof(uint32));
	for(i=0; i < sz; i++)  i3d[((b[i]+sh)>>q->z)*w2 + ((g[i]+sh)>>q->y)*w1 + ((r[i]+sh)>>q->x)]++;
	/*
	int x, y, z;
	for(z=0; z < 1<<(bpp-q->z); z++){
		for(y=0; y < 1<<(bpp-q->y); y++){
			for(x=0; x < 1<<(bpp-q->x); x++){
				printf("%4d ", i3d[w2*z + w1*y + x]);
			}
			printf("\n");
		}
		printf("\n\n");
	}*/
}

void seg_find_clusters(uint32 *i3d, uint16 *lut, int16 *r, int16 *g, int16 *b, uint32 w, uint32 h, uint32 rd,  uint32 bpp, p3d *q, uint32 *buf)
{
	int i, j, x, y, z, zy, yx, xyz, val, w2, sz = 1<<bpp;
	uint32 msb = 0x80000000, mask = 0x7FFFFFFF, max;
	p3d p, d;
	int k = 0;

	d.x = sz>>q->x; d.y = sz>>q->y; d.z = sz>>q->z;
	w2 = d.x*d.y; max = w2*d.z  + 1;

	memset(lut, 0, w2*d.z*sizeof(uint16));

	fill_3d_array(r, g, b, w, h, i3d, bpp, q);
	printf("Finesh fill_3d_array\n");
	printf("d.x = %d d.y = %d d.z = %d \n", d.x, d.y, d.z);

	for(z=0; z < d.z; z++){
		zy = z*w2;
		//printf("zy = %d\n", zy);
		for(y=0; y < d.y; y++){
			yx = zy + y*d.x;
			//printf("yx = %d\n", yx);
			for(x=0; x < d.x; x++){
				xyz = yx + x;
				if(!(i3d[xyz]&msb) && (i3d[xyz]&mask)){
					//printf("new\n");
					p.x = x; p.y = y; p.z = z;
					//printf("i3d[%d] = %d x = %d y = %d z = %d \n", xyz, i3d[xyz]&mask, p.x, p.y, p.z);
					buf[0] = xyz; //buf[1] = max;
					val = 0; i = 0;
					do{
						i++;
						center_mass(i3d, &d, &p, rd);
						buf[i] = p.z*w2 + p.y*d.x + p.x;
						//printf("i3d[%d] = %d x = %d y = %d z = %d buf[%d] = %d\n", buf[i], i3d[buf[i]]&mask, p.x, p.y, p.z, i, buf[i]);
						if(i3d[buf[i]]&msb) {
							val = lut[buf[i]];
							if(val == 0) printf("val = %d %d buf = %d i3d = %d x = %d y = %d z = %d \n", val, i, buf[i], i3d[buf[i]]&mask, p.x, p.y, p.z);
							//p.x = lut[buf[i]]&0xF800;
							//p.y = lut[buf[i]]&0x7E0;
							//p.z = lut[buf[i]]&0x1F;
							break;
						}
						//i3d[buf[i]] |= msb;

						//if(i > 100) break;
					} while(buf[i-1] != buf[i]);
					if(!val) {
						val = (p.x<<11) + (p.y<<5) + p.z;
						//for(j=0; j <= i; j++) { lut[buf[j]] = val; i3d[buf[j]] |= msb; }
						//printf("val = %d lut[buf[i]] = %d\n", val, lut[buf[i]]);
						//mean_print(i3d, lut, &d, &p, rd);
					}
					for(j=0; j <= i; j++) { lut[buf[j]] = val; i3d[buf[j]] |= msb; }
					//mean_print(i3d, lut, &d, &p, rd);
					k++;
					//printf("%d val = %d color = %3d %3d %3d num = %d i3d = %d\n", k, val, (val&0xF800)>>(11-q->x), (val&0x7E0)>>(5-q->y), (val&0x1F)<<q->z, i, i3d[val]&mask);
				}
			}
		}
	}
	printf("Colors = %d\n", k-1);
	uint32 sum=0;
	//for(i=0, k=0; i < w2*d.z; i++) if(lut[i] == i) printf("%d i3d = %d\n", i, i3d[i]&mask);

	for(i=0, k=0; i < w2*d.z; i++) {
		if(i3d[i]) {
			xyz = ((lut[i]&0xF800)>>11) + d.x*((lut[i]&0x7E0)>>5) + w2*(lut[i]&0x1F);
			if(i != xyz) { i3d[xyz] += (i3d[i]&mask); i3d[i] = 0; }
			else printf("i = %d i3d = %d\n", i, i3d[i]&mask);
		}
	}
	for(i=0, k=0; i < w2*d.z; i++)  sum += i3d[i]&mask; printf("sum = %d\n", sum);
	for(i=0, k=0; i < w2*d.z; i++) if(i3d[i]) {
		printf("%d color = %3d %3d %3d num = %d lut = %d i = %d\n",
				k++, (lut[i]&0xF800)>>(11-q->x), (lut[i]&0x7E0)>>(5-q->y), (lut[i]&0x1F)<<q->z, i3d[i]&mask, lut[i], i);
	}
}

void seg_quantization(uint16 *lut, uint8 *rgb, int16 *r, int16 *g, int16 *b, uint32 w, uint32 h, uint32 bpp, p3d *q)
{
	int i, i3, val, sz = w*h, d = 1<<bpp, w1 = (d>>q->x), w2 = w1*(d>>q->y), size = w2*(d>>q->z), sh = d>>1;
	for(i=0; i < sz; i++) {
		i3 = i*3;
		val = lut[((b[i]+sh)>>q->z)*w2 + ((g[i]+sh)>>q->y)*w1 + ((r[i]+sh)>>q->x)];
		rgb[i3] 	= (val&0xF800)>>(11-q->x);
		rgb[i3+1] 	= (val&0x7E0)>>(5-q->y);
		rgb[i3+2] 	= (val&0x1F)<<q->z;
		//printf("%3d %3d %3d  %3d %3d %3d   ", r[i], g[i], b[i], rgb[i3], rgb[i3+1], rgb[i3+2]);
	}
}

void mean_print_2d(uint8 *img, int16 *out, uint32 w, uint32 h, int *ox, int *oy, int *col, int ds, int dc, uint32 bpp)
{
	int x, y, yx, xy, bx, by, bc, ex, ey, ec, z = 1<<bpp;
	int xm = 0, ym = 0, cm = 0, ms = 0, im;
	uint32 mask = 0xFF;
	//im = img[xy]&mask;

	xy = (*oy)*w + *ox;
	im = img[xy];

	//printf("xy = %d\n", xy);
	if(*ox - ds < 0){ bx = 0; ex = *ox<<1; }
	else if(*ox + ds > (w - 1)) { bx = (*ox<<1) - w +1; ex = w - 1; }
	else { bx = *ox - ds; ex = *ox + ds; }

	if(*oy - ds < 0){ by = 0; ey = *oy<<1; }
	else if(*oy + ds > (h - 1)) { by = (*oy<<1) - h +1; ey = h - 1; }
	else { by = *oy - ds; ey = *oy + ds; }

	if(im - dc < 0){ bc = 0; ec = im<<1; }
	else if(im + dc > (z - 1)) { bc = (im<<1) - z +1; ec = z - 1; }
	else { bc = im - dc; ec = im + dc; }
	/*seg_remove_line1
	bx = (*ox - ds) < 0 ? 0 : *ox - ds;
	ex = (*ox + ds) > (w - 1) ? w - 1 : *ox + ds;
	by = (*oy - ds) < 0 ? 0 : *oy - ds;
	ey = (*oy + ds) > (h - 1) ? h - 1 : *oy + ds;
	bc = (im - dc) < 0 ? 0 : im - dc;
	ec = (im + dc) > (z - 1) ? z - 1 : im + dc;
	*/
	//printf("bx = %d ex = %d by = %d ey = %d bc = %d ec = %d\n", bx, ex, by, ey, bc, ec);

	for(y=by; y <= ey; y++){
		yx = y*w;
		//printf("yx = %d\n", yx);
		for(x=bx; x <= ex; x++){
			xy = yx + x;
			//printf("i3d[%d] = %d\n", xyz, i3d[xyz]);
			printf("%3d ", img[xy]);
			//if(img[xy] >= bc && img[xy] <= ec){
			//	printf("%3d ", img[xy]);
			//}else printf("%3d ", 0);
		}
		printf("   ");
		for(x=bx; x <= ex; x++){
			xy = yx + x;
			//printf("i3d[%d] = %d\n", xyz, i3d[xyz]);
			//printf("%3d ", img[xy]);
			if(img[xy] >= bc && img[xy] <= ec){
				printf("%3d ", img[xy]);
			}else printf("%3d ", 0);
		}
		printf("   ");
		for(x=bx; x <= ex; x++){
			xy = yx + x;
			printf("%3d ", out[xy]);
		}
		printf("\n");
	}
	printf("\n\n");
	//ms = (ex - bx + 1)*(ey - by + 1);
	//printf("ms = %d xm = %d ym = %d zm = %d\n", ms, xm, ym, zm);
	//*ox = xm/ms; *oy = ym/ms; *col = cm/ms;
}

uint32 center_mass_2d(uint8 *img, uint32 w, uint32 h, int *ox, int *oy, int *col, int ds, int dc, uint32 bpp)
{
	int x, y, yx, xy, bx, by, bc, ex, ey, ec, z = 1<<bpp;
	int xm = 0, ym = 0, cm = 0, ms = 0, im;
	uint32 mask = 0xFF;
	//im = img[xy]&mask;

	xy = (*oy)*w + *ox;
	im = img[xy];

	//printf("xy = %d\n", xy);
	if(*ox - ds < 0){ bx = 0; ex = *ox<<1; }
	else if(*ox + ds > (w - 1)) { bx = (*ox<<1) - w +1; ex = w - 1; }
	else { bx = *ox - ds; ex = *ox + ds; }

	if(*oy - ds < 0){ by = 0; ey = *oy<<1; }
	else if(*oy + ds > (h - 1)) { by = (*oy<<1) - h +1; ey = h - 1; }
	else { by = *oy - ds; ey = *oy + ds; }

	if(im - dc < 0){ bc = 0; ec = im<<1; }
	else if(im + dc > (z - 1)) { bc = (im<<1) - z +1; ec = z - 1; }
	else { bc = im - dc; ec = im + dc; }
	/*
	bx = (*ox - ds) < 0 ? 0 : *ox - ds;
	ex = (*ox + ds) > (w - 1) ? w - 1 : *ox + ds;
	by = (*oy - ds) < 0 ? 0 : *oy - ds;
	ey = (*oy + ds) > (h - 1) ? h - 1 : *oy + ds;
	bc = (im - dc) < 0 ? 0 : im - dc;
	ec = (im + dc) > (z - 1) ? z - 1 : im + dc;
	*/
	//printf("bx = %d ex = %d by = %d ey = %d bc = %d ec = %d\n", bx, ex, by, ey, bc, ec);

	for(y=by; y <= ey; y++){
		yx = y*w;
		//printf("yx = %d\n", yx);
		for(x=bx; x <= ex; x++){
			xy = yx + x;
			//printf("i3d[%d] = %d\n", xyz, i3d[xyz]);
			if(img[xy] >= bc && img[xy] <= ec){
				ms++;
				cm += img[xy];
				xm += x;
				ym += y;
				//zm += i2d;
			}
		}
	}
	//ms = (ex - bx + 1)*(ey - by + 1);
	//printf("ms = %d xm = %d ym = %d zm = %d\n", ms, xm, ym, zm);
	*ox = xm/ms; *oy = ym/ms; *col = cm/ms;
}

uint32 center_mass_2d_rgb(uint8 *r, uint8 *g, uint8 *b, uint32 w, uint32 h, int *ox, int *oy, int ds, int dc, uint32 bpp)
{
	int x, y, yx, xy, bx, by, bc, ex, ey, ec, z = 1<<bpp;
	int xm = 0, ym = 0, rm = 0, gm = 0, bm = 0, ms = 0, im;
	uint32 mask = 0xFF;
	//im = img[xy]&mask;

	xy = (*oy)*w + *ox;
	//im = r[xy];

	// X coordinate
	if(*ox - ds < 0){ bx = 0; ex = *ox<<1; }
	else if(*ox + ds > (w - 1)) { bx = (*ox<<1) - w + 1; ex = w - 1; }
	else { bx = *ox - ds; ex = *ox + ds; }
	// Y coordinate
	if(*oy - ds < 0){ by = 0; ey = *oy<<1; }
	else if(*oy + ds > (h - 1)) { by = (*oy<<1) - h + 1; ey = h - 1; }
	else { by = *oy - ds; ey = *oy + ds; }
	// Red color
	if(r[xy] - dc < 0){ bc = 0; ec = r[xy]<<1; }
	else if(r[xy] + dc > (z - 1)) { bc = (r[xy]<<1) - z + 1; ec = z - 1; }
	else { bc = r[xy] - dc; ec = r[xy] + dc; }
	// Green color
	if(g[xy] - dc < 0){ bc = 0; ec = g[xy]<<1; }
	else if(g[xy] + dc > (z - 1)) { bc = (g[xy]<<1) - z + 1; ec = z - 1; }
	else { bc = g[xy] - dc; ec = g[xy] + dc; }
	// Blue color
	if(b[xy] - dc < 0){ bc = 0; ec = b[xy]<<1; }
	else if(b[xy] + dc > (z - 1)) { bc = (b[xy]<<1) - z + 1; ec = z - 1; }
	else { bc = b[xy] - dc; ec = b[xy] + dc; }
	/*
	bx = (*ox - ds) < 0 ? 0 : *ox - ds;
	ex = (*ox + ds) > (w - 1) ? w - 1 : *ox + ds;
	by = (*oy - ds) < 0 ? 0 : *oy - ds;
	ey = (*oy + ds) > (h - 1) ? h - 1 : *oy + ds;
	bc = (im - dc) < 0 ? 0 : im - dc;
	ec = (im + dc) > (z - 1) ? z - 1 : im + dc;
	*/
	//printf("bx = %d ex = %d by = %d ey = %d bc = %d ec = %d\n", bx, ex, by, ey, bc, ec);

	for(y=by; y <= ey; y++){
		yx = y*w;
		//printf("yx = %d\n", yx);
		for(x=bx; x <= ex; x++){
			xy = yx + x;
			//printf("i3d[%d] = %d\n", xyz, i3d[xyz]);
			if((r[xy] >= bc && r[xy] <= ec) && (g[xy] >= bc && g[xy] <= ec) && (b[xy] >= bc && b[xy] <= ec)){
				ms++;
				//rm += r[xy]; gm += g[xy]; bm += b[xy];
				xm += x;
				ym += y;
				//zm += i2d;
			}
		}
	}
	//ms = (ex - bx + 1)*(ey - by + 1);
	//printf("ms = %d xm = %d ym = %d zm = %d\n", ms, xm, ym, zm);
	*ox = xm/ms; *oy = ym/ms; //*col = cm/ms;
}


void seg_find_clusters_2d(uint8 *in, uint8 *out, uint32 w, uint32 h, uint32 ds, uint32 dc, uint32 bpp, uint32 *buf)
{
	int i, j, k=0, c = 0, e = 0, x, y, z, zy, yx, xy, val, w2, outx, outy, col;
	uint32 msb = 0x100, mask = 0xFF, min, sz = w*h;

	memset(out, 0, w*h*sizeof(uint8));
	//y=0;{
	for(y=0; y < h; y++){
		yx = y*w;
		//printf("yx = %d\n", yx);
		for(x=0; x < w; x++){
			xy = yx + x;
			//printf("xy = %d\n", yx);
			//printf("new\n");
			if(!out[xy]){
				//printf("new\n");
				buf[0] = xy; //buf[1] = max;
				outx = x; outy = y;
				//out[buf[0]] = in[buf[0]];
				out[buf[0]] = 1;
				val = 0; i = 0;
				do{
					i++;
					//mean_print_2d(in, out, w, h, &outx, &outy, &col, ds, dc, bpp);
					center_mass_2d(in, w, h, &outx, &outy, &col, ds, dc, bpp);
					buf[i] = outy*w + outx;
					//out[buf[i]] = col;
					//printf("%d buf[i-1] = %d buf[i] = %d x = %d y = %d img = %d col = %d \n", i, buf[i-1], buf[i], outx, outy, in[buf[i]], col);
					if(out[buf[i]] > 1) {
						out[buf[i]] = col;
						break;
					} else if (out[buf[i]] == 1) {
						out[buf[i]] = in[buf[i]];
						//Find clusters
						/*
						min = in[buf[i]];

						if(buf[i] > w && buf[i] < w*(h-1)){
							if(abs(in[buf[i]-1  ] - in[buf[i]]) < min) { val = in[buf[i]-1  ]; }
							if(abs(in[buf[i]-1-w] - in[buf[i]]) < min) { val = in[buf[i]-1-w]; }
							if(abs(in[buf[i]  -w] - in[buf[i]]) < min) { val = in[buf[i]  -w]; }
							if(abs(in[buf[i]+1-w] - in[buf[i]]) < min) { val = in[buf[i]+1-w]; }
							if(abs(in[buf[i]+1  ] - in[buf[i]]) < min) { val = in[buf[i]+1  ]; }
							if(abs(in[buf[i]+1+w] - in[buf[i]]) < min) { val = in[buf[i]+1+w]; }
							if(abs(in[buf[i]  +w] - in[buf[i]]) < min) { val = in[buf[i]  +w]; }
							if(abs(in[buf[i]-1+w] - in[buf[i]]) < min) { val = in[buf[i]-1+w]; }
						}

						out[buf[i]] = val;
						if(i > 10) for(j=0; j < i; j++) out[buf[j]] = out[buf[i]];*/
						break;
					}
					out[buf[i]] = 1;
					//i3d[buf[i]] |= msb;

					//if(i > 100) break;
				} while(1);//while(buf[i-1] != buf[i]);

				//for(j=0; j < i; j++) out[buf[j]] = out[buf[i]];
				//for(j=0; j < i; j++) out[buf[j]] = col;
				k+= i; c++;
				//printf("%d val = %d color = %3d %3d %3d num = %d i3d = %d\n", k, val, (val&0xF800)>>(11-q->x), (val&0x7E0)>>(5-q->y), (val&0x1F)<<q->z, i, i3d[val]&mask);
			}
		}
	}
	printf("Colors = %d clusters = %d one = %d\n", k, c, e);
	uint32 sum=0;
	//for(i=0, k=0; i < w2*d.z; i++) if(lut[i] == i) printf("%d i3d = %d\n", i, i3d[i]&mask);
	/*
	for(i=0; i < w2*d.z; i++) {
		if(i3d[i]) {
			xyz = ((lut[i]&0xF800)>>11) + d.x*((lut[i]&0x7E0)>>5) + w2*(lut[i]&0x1F);
			if(i != xyz) { i3d[xyz] += (i3d[i]&mask); i3d[i] = 0; }
			else printf("i = %d i3d = %d\n", i, i3d[i]&mask);
		}
	}
	for(i=0; i < sz; i++)  sum += img[i]&mask; printf("sum = %d\n", sum);
	for(i=0; i < sz; i++) if(i3d[i]) {
		printf("%d color = %3d %3d %3d num = %d lut = %d i = %d\n",
				k++, (lut[i]&0xF800)>>(11-q->x), (lut[i]&0x7E0)>>(5-q->y), (lut[i]&0x1F)<<q->z, i3d[i]&mask, lut[i], i);
	}
	*/
}

void seg_grad16(int16 *img, uint8 *img1, uint32 w, uint32 h, uint32 th)
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
	uint8 max, in;
	uint32 g[4];
	for(y=w; y < sq; y+=w){
		for(x=1; x < w1; x++){
			yx = y + x;
			g[0] = abs(img[yx-1  ] - img[yx+1  ]);
			g[1] = abs(img[yx-1-w] - img[yx+1+w]);
			g[2] = abs(img[yx-w  ] - img[yx+w  ]);
			g[3] = abs(img[yx+1-w] - img[yx-1+w]);
			max = (g[0] + g[1] + g[2] + g[3])>>2;
			//max = g[2];
			img1[yx] = (max>>th) ? (max > 252 ? 252 : max) : 0;
		}
	}
}

void seg_corner(int16 *img, uint8 *img1, uint32 w, uint32 h, uint32 th)
{
	uint32 y, x, yx, sq = w*h-w, w1 = w-1, h1 = h-1, sh = 1<<th;
	int max, in;
	for(y=w; y < sq; y+=w){
		for(x=1; x < w1; x++){
			yx = y + x;
			max = 	abs(img[yx-1  ] - img[yx-1-w]) + abs(img[yx-1-w] - img[yx-w  ]) +
					abs(img[yx-w  ] - img[yx+1-w]) + abs(img[yx+1-w] - img[yx+1  ]) +
					abs(img[yx+1  ] - img[yx+1+w]) + abs(img[yx+1+w] - img[yx+w  ]) +
					abs(img[yx+w  ] - img[yx-1+w]) + abs(img[yx-1+w] - img[yx-1  ]);
			max = (max>>2) - sh;
			img1[yx] = (max > 0) ? (max > 252 ? 252 : max) : 0;
		}
	}
}


void seg_grad1(uint8 *img, uint8 *img1,uint8 *img2, uint32 w, uint32 h, uint32 th)
{
	uint32 y, x, yx, sq = w*h-w, w1 = w-1, h1 = h-1;
	uint8 max, in;
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
			max = g[0]; in = 2;
			if(max < g[1]) { max = g[1]; in = 3; }
			if(max < g[2]) { max = g[2]; in = 0; }
			if(max < g[3]) { max = g[3]; in = 1; }
			img2[yx] = in;
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

void seg_grad2(uint8 *img, uint8 *img1, uint32 w, uint32 h, uint32 th)
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



static inline void dir1(uint8 *img, uint32 w, uint yx, int dx, int dy, int *dx1, int *dy1)
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
                if(img[yx+1+w] > max) { max = img[yx+1+w]; *dx1 =  1; *dy1 =  1;}
                if(img[yx+1  ] >= max){ max = img[yx+1  ]; *dx1 =  1; *dy1 =  0;}
                //if(img[yx  +w] > max) { max = img[yx  +w]; *dx1 =  0; *dy1 =  1;}
                //goto end;
        }
        else if(dx == -1 && dy == -1){
                //if(img[yx+1-w] > max) { max = img[yx+1-w]; *dx1 =  1; *dy1 = -1;}
                if(img[yx+1  ] > max) { max = img[yx+1  ]; *dx1 =  1; *dy1 =  0;}
                if(img[yx  +w] > max) { max = img[yx  +w]; *dx1 =  0; *dy1 =  1;}
                if(img[yx+1+w] >= max){ max = img[yx+1+w]; *dx1 =  1; *dy1 =  1;}
                //if(img[yx-1+w] > max) { max = img[yx-1+w]; *dx1 = -1; *dy1 =  1;}
                //goto end;
        }
        else if(dx ==  0 && dy == -1){
                //if(img[yx+1  ] > max) { max = img[yx+1  ]; *dx1 =  1; *dy1 =  0;}
                if(img[yx+1+w] > max) { max = img[yx+1+w]; *dx1 =  1; *dy1 =  1;}
                if(img[yx-1+w] > max) { max = img[yx-1+w]; *dx1 = -1; *dy1 =  1;}
                if(img[yx  +w] >= max){ max = img[yx  +w]; *dx1 =  0; *dy1 =  1;}
                //if(img[yx  -1] > max) { max = img[yx  -1]; *dx1 = -1; *dy1 =  0;}
                //goto end;
        }
        else if(dx ==  1 && dy == -1){
                //if(img[yx+1+w] > max) { max = img[yx+1+w]; *dx1 =  1; *dy1 =  1;}
                if(img[yx  +w] > max) { max = img[yx  +w]; *dx1 =  0; *dy1 =  1;}
                if(img[yx  -1] > max) { max = img[yx  -1]; *dx1 = -1; *dy1 =  0;}
                if(img[yx-1+w] >= max){ max = img[yx-1+w]; *dx1 = -1; *dy1 =  1;}
                //if(img[yx-1-w] > max) { max = img[yx-1-w]; *dx1 = -1; *dy1 = -1;}
                //goto end;
        }
        else if(dx ==  1 && dy ==  0){
                //if(img[yx  +w] > max) { max = img[yx  +w]; *dx1 =  0; *dy1 =  1;}
                if(img[yx-1+w] > max) { max = img[yx-1+w]; *dx1 = -1; *dy1 =  1;}
                if(img[yx-1-w] > max) { max = img[yx-1-w]; *dx1 = -1; *dy1 = -1;}
                if(img[yx  -1] >= max){ max = img[yx  -1]; *dx1 = -1; *dy1 =  0;}
                //if(img[yx  -w] > max) { max = img[yx  -w]; *dx1 =  0; *dy1 = -1;}
                //goto end;
        }
        else if(dx ==  1 && dy ==  1){
                //if(img[yx-1+w] > max) { max = img[yx-1+w]; *dx1 = -1; *dy1 =  1;}
                if(img[yx  -1] > max) { max = img[yx  -1]; *dx1 = -1; *dy1 =  0;}
                if(img[yx  -w] > max) { max = img[yx  -w]; *dx1 =  0; *dy1 = -1;}
                if(img[yx-1-w] >= max){ max = img[yx-1-w]; *dx1 = -1; *dy1 = -1;}
                //if(img[yx+1-w] > max) { max = img[yx+1-w]; *dx1 =  1; *dy1 = -1;}
                //goto end;
        }
        else if(dx ==  0 && dy ==  1){
                //if(img[yx  -1] > max) { max = img[yx  -1]; *dx1 = -1; *dy1 =  0;}
                if(img[yx-1-w] > max) { max = img[yx-1-w]; *dx1 = -1; *dy1 = -1;}
                if(img[yx+1-w] > max) { max = img[yx+1-w]; *dx1 =  1; *dy1 = -1;}
                if(img[yx  -w] >= max){ max = img[yx  -w]; *dx1 =  0; *dy1 = -1;}
                //if(img[yx+1  ] > max) { max = img[yx+1  ]; *dx1 =  1; *dy1 =  0;}
                //goto end;
        }
        else if(dx == -1 && dy ==  1){
                //if(img[yx-1-w] > max) { max = img[yx-1-w]; *dx1 = -1; *dy1 = -1;}
                if(img[yx  -w] > max) { max = img[yx  -w]; *dx1 =  0; *dy1 = -1;}
                if(img[yx+1  ] > max) { max = img[yx+1  ]; *dx1 =  1; *dy1 =  0;}
                if(img[yx+1-w] >= max){ max = img[yx+1-w]; *dx1 =  1; *dy1 = -1;}
                //if(img[yx+1+w] > max) { max = img[yx+1+w]; *dx1 =  1; *dy1 =  1;}
               // goto end;
        }
}

static inline int dir2(uint8 *img, uint32 w, uint32 yx, int dyx, uint32 max)
{
	//printf("dyx = %d\n", dyx);
	if(dyx == -1){
		if(img[yx+1  ] == 255) return 1;
		if(img[yx+1-w] == 255) return 1-w;
		if(img[yx+1+w] == 255) return 1+w;

		if(img[yx+1  ] > max) return 1;
		if(img[yx+1-w] > max) return 1-w;
		if(img[yx+1+w] > max) return 1+w;
	 }
	else if(dyx == -1-w){
		if(img[yx+1+w] == 255) return 1+w;
		if(img[yx+1  ] == 255) return 1;
		if(img[yx  +w] == 255) return w;

		if(img[yx+1+w] > max) return 1+w;
		if(img[yx+1  ] > max) return 1;
		if(img[yx  +w] > max) return w;
	}
	else if(dyx == -w){
		if(img[yx  +w] == 255) return w;
		if(img[yx+1+w] == 255) return 1+w;
		if(img[yx-1+w] == 255) return -1+w;

		if(img[yx  +w] > max) return w;
		if(img[yx+1+w] > max) return 1+w;
		if(img[yx-1+w] > max) return -1+w;
	 }
	else if(dyx == 1-w){
		if(img[yx-1+w] == 255) return -1+w;
		if(img[yx  +w] == 255) return w;
		if(img[yx  -1] == 255) return -1;

		if(img[yx-1+w] > max) return -1+w;
		if(img[yx  +w] > max) return w;
		if(img[yx  -1] > max) return -1;
	 }
	else if(dyx == 1){
		if(img[yx  -1] == 255) return -1;
		if(img[yx-1+w] == 255) return -1+w;
		if(img[yx-1-w] == 255) return -1-w;

		if(img[yx  -1] > max) return -1;
		if(img[yx-1+w] > max) return -1+w;
		if(img[yx-1-w] > max) return -1-w;
	 }
	else if(dyx == 1+w){
		if(img[yx-1-w] == 255) return -1-w;
		if(img[yx  -1] == 255) return -1;
		if(img[yx  -w] == 255) return -w;

		if(img[yx-1-w] > max) return -1-w;
		if(img[yx  -1] > max) return -1;
		if(img[yx  -w] > max) return -w;
	 }
	else if(dyx == w){
		if(img[yx  -w] == 255) return -w;
		if(img[yx-1-w] == 255) return -1-w;
		if(img[yx+1-w] == 255) return 1-w;

		if(img[yx  -w] > max) return -w;
		if(img[yx-1-w] > max) return -1-w;
		if(img[yx+1-w] > max) return 1-w;
	 }
	else if(dyx == -1+w){
		if(img[yx+1-w] == 255) return 1-w;
		if(img[yx  -w] == 255) return -w;
		if(img[yx+1  ] == 255) return 1;

		if(img[yx+1-w] > max) return 1-w;
		if(img[yx  -w] > max) return -w;
		if(img[yx+1  ] > max) return 1;
	 }
	printf("////////////////////////////////not find\n");
	return 0;
}

static inline void new_pix(Pixel *pix, uint8 img, uint32 x, uint32 y)
{
	pix->nin = 0; pix->nout = 0; pix->x = x; pix->y = y; //pix->draw = 1; //pix->end = 0;
}

static inline void new_pix1(Pixel *pix, uint8 img, uint32 x, uint32 y, uint32 nei)
{
	pix->nin = 0; pix->nout = 0; pix->x = x; pix->y = y; pix->nnei = nei; pix->cp = NULL; //pix->draw = 1; //pix->end = 0;
}

static inline void new_line2(Pixel *pix, Pixel *pix1, uint8 pow, uint16 npix)
{
	pix->out = pix1;   pix->nout++; pix->pow = pow; pix1->pow = pow; //pix->pow[pix->nout] = pow; pix->pow[pix->nout] = pow;
	pix1->nin++; pix->npix = npix;
}

static inline void change_line(Pixel *pix, Pixel *pix1, uint8 pow, uint16 npix)
{
	pix->out = pix1;  pix->nout = 1; pix->pow = pow;  //pix->pow[pix->nout] = pow; pix->pow[pix->nout] = pow;
	pix1->nin = 1; pix1->pow = pow; pix->npix = npix;
}

void seg_local_max( Pixel *pix, uint32 *npix, uint8 *img, uint32 w, uint32 h)
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

void seg_point(uint8 *con, uint32 w, uint32 h)
{
	uint32 y, y1, x, yx, in, sq = w*h - w, w1 = w-1;
	int npix = 0;

	for(y=1, y1=w; y1 < sq; y++, y1+=w){
		for(x=1; x < w1; x++){
			yx = y1 + x;
			//if(img[yx] && img[yx]!= 255){
			in = 0;
			if(con[yx]){
				if(con[yx-1  ]) in++;
				if(con[yx-1-w]) in++;
				if(con[yx  -w]) in++;
				if(con[yx+1-w]) in++;
				if(con[yx+1  ]) in++;
				if(con[yx+1+w]) in++;
				if(con[yx  +w]) in++;
				if(con[yx-1+w]) in++;
			}
			if(in > 2 || in == 1 ) { con[yx] = 255; npix++; }
		}
	}
	printf("Numbers of points  = %d\n", npix);
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

static inline void set_dir_one(Pixel *pix, uint8 *img, uint32 yx, uint32 w, int dyx)
{
	uint8 dir;
	pix[yx + dyx].cp = &pix[yx];
	img[yx + dyx] = 255;
	/*
	if     (-1   == dyx) pix[yx].dir += 1;
	else if(-1-w == dyx) pix[yx].dir += 2;
	else if(  -w == dyx) pix[yx].dir += 4;
	else if(+1-w == dyx) pix[yx].dir += 8;
	else if(+1   == dyx) pix[yx].dir += 16;
	else if(+1+w == dyx) pix[yx].dir += 32;
	else if(  +w == dyx) pix[yx].dir += 64;
	else if(-1+w == dyx) pix[yx].dir += 128;
	*/
}

static inline void set_dir_all(Pixel *pix, uint8 *img, uint32 yx, uint32 w)
{
	uint8 dir;
	/*
	if(img[yx-1  ] > 253) { pix[yx].dir += 1; 	pix[yx-1  ].cp = &pix[yx]; img[yx-1  ] = 255;}
	if(img[yx-1-w] > 253) { pix[yx].dir += 2;	pix[yx-1-w].cp = &pix[yx]; img[yx-1-w] = 255;}
	if(img[yx  -w] > 253) { pix[yx].dir += 4;	pix[yx  -w].cp = &pix[yx]; img[yx  -w] = 255;}
	if(img[yx+1-w] > 253) { pix[yx].dir += 8;	pix[yx+1-w].cp = &pix[yx]; img[yx+1-w] = 255;}
	if(img[yx+1  ] > 253) { pix[yx].dir += 16;	pix[yx+1  ].cp = &pix[yx]; img[yx+1  ] = 255;}
	if(img[yx+1+w] > 253) { pix[yx].dir += 32;	pix[yx+1+w].cp = &pix[yx]; img[yx+1+w] = 255;}
	if(img[yx  +w] > 253) { pix[yx].dir += 64;	pix[yx  +w].cp = &pix[yx]; img[yx  +w] = 255;}
	if(img[yx-1+w] > 253) { pix[yx].dir += 128;	pix[yx-1+w].cp = &pix[yx]; img[yx-1+w] = 255;}
	*/
	if(img[yx-1  ] > 253) { pix[yx-1  ].cp = &pix[yx]; img[yx-1  ] = 255;}
	if(img[yx-1-w] > 253) { pix[yx-1-w].cp = &pix[yx]; img[yx-1-w] = 255;}
	if(img[yx  -w] > 253) { pix[yx  -w].cp = &pix[yx]; img[yx  -w] = 255;}
	if(img[yx+1-w] > 253) { pix[yx+1-w].cp = &pix[yx]; img[yx+1-w] = 255;}
	if(img[yx+1  ] > 253) { pix[yx+1  ].cp = &pix[yx]; img[yx+1  ] = 255;}
	if(img[yx+1+w] > 253) { pix[yx+1+w].cp = &pix[yx]; img[yx+1+w] = 255;}
	if(img[yx  +w] > 253) { pix[yx  +w].cp = &pix[yx]; img[yx  +w] = 255;}
	if(img[yx-1+w] > 253) { pix[yx-1+w].cp = &pix[yx]; img[yx-1+w] = 255;}
}


static inline uint32 find_pixels(Pixel *pix, uint8 *img, uint32 x, uint32 y, uint32 dx, uint32 dy, uint32 w)
{
	uint32 npix = 0, len = 0, yx = y*w + x, x1 = x, y1 = y, yx1 = yx, x2, y2, yx2, x3, y3, yx3, xt, yt, c = 0, c3 = 0; //yx2 = yx1,
	uint8 min = img[yx];
	int dx1, dy1, dx2, dy2, dx3, dy3;
	dx3 = dx; dy3 = dy;
	img[yx] = 254;
	//new_pix1(&pix[yx], img[yx], x, y, 1);
	//set_dir_one(pix, img, yx, w, yx1-yx2);
    while(1){
    	//printf("New pix\n");
    	yx2 = yx1; //x1 = x; y1 = y; // Save previous pixel
    	yx1 = yx; x1 = x; y1 = y; // Save previous pixel
      	yx = yx + dy*w + dx; x = x + dx; y = y + dy; c++;
       	if(img[yx]) min = img[yx] < min ? img[yx] : min;
		if(!img[yx]) { //End point with 0
			new_pix1(&pix[yx1], img[yx1], x1, y1, 1);
			if(c >2 ) set_dir_one(pix, img, yx1, w, yx1-yx2);
			img[yx1] = 255; npix++;
			return npix;
		}
		if(img[yx] == 254) { //End point with 0
			new_pix1(&pix[yx], img[yx], x, y, 3);
			set_dir_all(pix, img, yx, w);
			img[yx] = 255; npix++;
			return npix;
		}
		if(img[yx] == 255) { //End point with 0
			if(pix[yx].cp){
				pix[yx].cp->nnei++;
				set_dir_one(pix, img, pix[yx].cp->x + pix[yx].cp->y*w, w, pix[yx].cp->x + pix[yx].cp->y*w - yx);
			} else {
				pix[yx].nnei++;
				set_dir_one(pix, img, yx, w, yx-yx1);
			}
			return npix;
		}
		img[yx] = 254;
		//dx2 = dx1; dy2 = dy1;
		//dx1 = dx; dy1 = dy;
		dir1(img, w, yx, -dx, -dy, &dx, &dy);

		if(dx3 == dx && dy3 == dy) { x3 = x; y3 = y; yx3 = yx; c3 = c; }
		if((c > 2 && check_corner(dx3, dy3, dx, dy))){ //New point
			//printf("c = %d c3 = %d \n", c, c3);
			if(!c3) {
				new_pix1(&pix[yx], img[yx], x, y, 2);
				img[yx] = 255; npix++;
			} else {
				new_pix1(&pix[yx], img[yx], x, y, 2);
				new_pix1(&pix[yx3], img[yx3], x3, y3, 2);
				img[yx] = 255; img[yx3] = 255; npix += 2;
			}
			dx3 = dx; dy3 = dy;
			x3 = x; y3 = y; yx3 = yx;
			c = 0; c3 = 0;
		}
    }
	//return npix;
}

static inline uint32 find_lines(Pixel *pix, uint8 *img, uint32 x, uint32 y, uint32 dx, uint32 dy, uint16 *npix, uint16 *nline, uint32 w, uint8 dir)
{
	uint32 len = 0, yx = y*w + x, x1 = x, y1 = y, yx1 = yx, yxt,  xt, yt, c = 0; //yx2 = yx1,
	uint8 min = img[yx];
	//uint8 im;
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
				if(dir)	new_line2(&pix[yx1], &pix[yxt], min, c);
				else	new_line2(&pix[yxt], &pix[yx1], min, c);
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
			if(dir)	new_line2(&pix[yx1], &pix[yx ], min, c);
			else 	new_line2(&pix[yx ], &pix[yx1], min, c);
			(*nline)++; *npix += c;
			x1 = x; y1 = y; yx1 = yx; c = 0; //len = 0; yx2 = yx1;
		}
    }
}

uint32 seg_pixels(Pixel *pix, uint8 *img, uint32 w, uint32 h)
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

static inline uint32 get_dir1(uint8 *img, uint32 yx, uint32 w, uint8 dir, int *dm)
{
	uint8 i;
	/*
	if(img[yx-1  ] > 253 && !(dir&1  )) { dm[i] = -1;   i++; }
	if(img[yx-1-w] > 253 && !(dir&2  )) { dm[i] = -1-w; i++; }
	if(img[yx  -w] > 253 && !(dir&4  )) { dm[i] =   -w; i++; }
	if(img[yx+1-w] > 253 && !(dir&8  )) { dm[i] =  1-w; i++; }
	if(img[yx+1  ] > 253 && !(dir&16 )) { dm[i] =  1;   i++; }
	if(img[yx+1+w] > 253 && !(dir&32 )) { dm[i] =  1+w; i++; }
	if(img[yx  +w] > 253 && !(dir&64 )) { dm[i] =  w;   i++; }
	if(img[yx-1+w] > 253 && !(dir&128)) { dm[i] = -1+w; i++; }
	*/
	if(img[yx-1  ] == 255) { dm[i] = -1;   i++; }
	if(img[yx-1-w] == 255) { dm[i] = -1-w; i++; }
	if(img[yx  -w] == 255) { dm[i] =   -w; i++; }
	if(img[yx+1-w] == 255) { dm[i] =  1-w; i++; }
	if(img[yx+1  ] == 255) { dm[i] =  1;   i++; }
	if(img[yx+1+w] == 255) { dm[i] =  1+w; i++; }
	if(img[yx  +w] == 255) { dm[i] =  w;   i++; }
	if(img[yx-1+w] == 255) { dm[i] = -1+w; i++; }

	return i;
}

static inline void test(uint8 *img, uint32 yx, uint32 w)
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

uint32 seg_region(Pixel *pix, uint8 *img, uint32 w, uint32 h)
{
	uint32 i, y, x, yx, yx1, w1 = w-1, h1 = h-1;
	uint32 npix = 0, n;
	int dm[8], dyx, dyx1;
	for(y=1; y < h1; y++){
		for(x=1; x < w1; x++){
			yx = y*w + x;
			if(img[yx] == 255){
				//printf("\n cp = %p\n", pix[yx].cp);
				if(pix[yx].cp) yx = pix[yx].cp->x + pix[yx].cp->y*w;
				npix++;
				n = get_dir1(img, yx, w, pix[yx].dir, dm);
				//test(img, yx, w);
				//printf("dir =  %d n = %d \n", pix[yx].dir, n);

				for(i=0; i < n; i++){
					yx1 = yx;
					//printf("dm[%d] = %d \n", i, dm[i]);
					dyx = dm[i];
					while(1){
						//if(!dyx)
						//test(img, yx1, w);
						yx1 = yx1 + dyx;
						//printf("img = %d  dyx = %d x = %d y = %d\n", img[yx1], dyx1, yx1%w, yx1/w);
						if(!dyx) return 0;

						if(img[yx1] == 255){
							//pix[yx1].dir = set_dir(img, yx1, w, -dyx);
							if(pix[yx1].cp) yx1 = pix[yx1].cp->x + pix[yx1].cp->y*w;
							break;
						}
						dyx1 = dyx;
						dyx = dir2(img, w, yx1, -dyx, 253);
					}
				}
			}
		}
	}
	printf("Numbers of pixels = %6d\n", npix);
	return npix;

}

uint32 seg_line(Pixel *pix, Edge *edges, uint8 *img, uint32 w, uint32 h)
{
	uint32 y, x, yx, yw, w1 = w-1, h1 = h-1;
	uint32 npix = 0, nline = 0, nedge = 0, px, ln;
	int dx, dy, dx1, dy1;
	printf("w = %d h = %d\n", w, h);
	for(y=1; y < h1; y++){
		yw = y*w;
		for(x=1; x < w1; x++){
			yx = yw + x;
			if(img[yx] && img[yx] < 253){
				if(loc_max(img, yx, w)){
					dir1(img, w,  yx,  0,  0,  &dx,  &dy);
					dir1(img, w,  yx, dx, dy, &dx1, &dy1);
					edges[nedge].yxe = find_lines(pix, img, x, y, dx,  dy,  &edges[nedge].pixs, &edges[nedge].lines, w, 1);
					//printf("nedge = %d 1 pixs = %3d lines = %3d ", nedge, edges[nedge].pixs, edges[nedge].lines);
					edges[nedge].yxs = find_lines(pix, img, x, y, dx1, dy1, &edges[nedge].pixs, &edges[nedge].lines, w, 0);
					//printf(" 0 pixs = %3d lines = %3d \n", edges[nedge].pixs, edges[nedge].lines);
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

uint32 seg_points(uint8 *img, uint32 w, uint32 h)
{
	uint32 y, x, yx, yx1, yw, w1 = w-1, h1 = h-1, sz = w*h;
	uint32 npix = 0, nline = 0, nedge = 0, px, ln;
	int dx=0, dy=0, dx1, dy1;

	for(y=1; y < h1; y++){
		yw = y*w;
		for(x=1; x < w1; x++){
			yx = yw + x;
			if(img[yx] && img[yx] < 253){
				yx1 = yx;
				img[yx1] = 254;
				while(1){
					direction(img, w,  yx1, &dx, &dy);
					yx1 += dx + dy*w;
					if(loc_max(img, yx1, w)) {
						img[yx1] = 255;
						break;
					}
					img[yx1] = 254;
				}
			}
		}
	}
	for(x=0; x < sz; x++) img[x] = (img[x] == 254) ? 0 : img[x];
	return nedge;
}

void seg_reduce_line(Pixel *pix, uint8 *img, uint32 w, uint32 h)
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

static inline uint32 draw_line2(uint8 *img, Vector *v, uint32 w, uint32 col, uint8 end)
//Draw line, return number of pixels in line
//If end 1 not draw last pixel, if 0 draw

{
	uint32 i;
	int dx = v->x2 - v->x1, dy = v->y2 - v->y1;
	uint32 stx, sty, dxa = abs(dx)+1, dya = abs(dy)+1;
	uint32 mit, mat, max, min, mi, ma, mist, mast, c, yx;
	//printf("dxa = %d dya = %d\n", dxa, dya);

	stx = dx < 0 ? -1 : (dx > 0 ? 1 : 0);
	sty = dy < 0 ? -1 : (dy > 0 ? 1 : 0);

	if(dxa >= dya) 	{ max = dxa; min = dya; mi = v->y1; ma = v->x1; mist = sty; mast = stx; c = 1; }
	else 			{ max = dya; min = dxa; mi = v->x1; ma = v->y1; mist = stx; mast = sty; c = 0; }

	mat = max; mit = min;
	max = end ? max - 1 : max;
	//if(max == 1) printf("max = %d dx = %d dy = %d end = %d\n", max, dx, dy, end);
	for(i=0; i < max; i++){
		img[xy(mi, ma, c)] = col;
		//printf("%d ", xy(mi, ma, c));
		if(mit >= mat) { mat += max; mi += mist;}
		mit += min;
		ma += mast;
	}
	//printf("\n");
	return max;
}

void seg_draw_lines(Pixel *pix, uint8 *img, uint32 w, uint32 h)
{
	uint32 i, j, k, pixs = 0, nline = 0;
	Vector xy;
	for(i=0; i < w*h; i++){
		//if(pix[i].nnei > 4){
			//if(pix[i].nout) {
			//if(pix[i].pow == 255) {
			if(pix[i].nout) {
				//if(pix[i].nout > 1) printf("nout = %d\n",pix[i].nout);
				//printf("dx = %d dy = %d pow = %d\n", pix[i].x - pix[i].out->x, pix[i].y - pix[i].out->y, pix[i].pow);
				xy.x1 = pix[i].x; xy.y1 = pix[i].y;
				xy.x2 = pix[i].out->x; xy.y2 = pix[i].out->y;
				draw_line2(img, &xy, w, pix[i].pow, pix[i].out->nout);
				nline++;
			}
	}
	for(i=0; i < w*h; i++){
		if(pix[i].nout || pix[i].nin){
			img[i] = 255;
		//if(pix[i].nout){
			//img[i] = pix[i].pow<<1;
			//img[i + pix[i].yx] = pix[i].pow<<1;
			//img[i - pix[i].yx] = pix[i].pow<<1;
			pixs++;
		}
	}

	printf("Pixels  = %d\n", pixs);
	printf("Lines   = %d\n", nline);
}

void seg_draw_edges(Pixel *pix, Edge *edge, uint32 nedge, uint8 *img, uint32 w, uint32 h, uint32 px, uint32 col)
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
			npix += draw_line2(img, &xy, w, color, 1)-1;
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

void seg_draw_edges_des(Pixel *pix, Edge *edge, uint32 nedge, uint8 *img, uint32 w, uint32 h, uint32 px, uint32 col)
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
			npix += draw_line2(img, &xy, w, color, p->out->nout)-1;
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

void seg_draw_pix(Pixel *pix, uint8 *img, uint8 *grad, uint32 w, uint32 h, uint32 col)
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

static inline uint32 diff3x3( uint8 *img1, uint8 *img2, uint32 yx1, uint32 yx2,  uint32 w)
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

static inline uint16 block_match(uint8 *grad, uint8 *img1, uint8 *img2, uint32 x1, uint32 y1, uint32 x2, uint32 y2,  uint32 *xo, uint32 *yo, uint32 w, uint32 h, uint32 st)
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

static inline uint32 diff3x5( uint8 *img1, uint8 *img2, uint32 yx1, uint32 yx2, uint32 yx3, int *sad,  uint32 w)
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

static inline uint16 block_match1(uint8 *grad, uint8 *img1, uint8 *img2, uint32 x1, uint32 y1, uint32 x2, uint32 y2, uint32 yx2, uint32 *xo, uint32 *yo, uint32 w, uint32 h, uint32 st, uint32 th)
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

static inline uint16 double_block_match(uint8 *grad, uint8 *img1, uint8 *img2, uint32 x1, uint32 y1, uint32 yx1, uint32 x2, uint32 y2, uint32 yx2, uint32 *xo, uint32 *yo, uint32 w, uint32 h, uint32 st, uint32 th)
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

static inline void block_match_new(uint8 *grad, uint8 *img1, uint8 *img2, uint8 *mmb, uint32 x1, uint32 y1, uint32 x2, uint32 y2, uint32 yx2, uint32 w, uint32 h, uint32 st, uint32 th)
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


static inline void min_add(uint8 *mb1, uint8 *mb2, uint8 *mb3, uint32 sqm)
{
	uint32 i, sum;
	for(i=0; i < sqm; i++) {
		mb3[i] = (mb1[i] + mb2[i])>>1;
		//sum = (mb1[i] + mb2[i])>>1;
		//mb3[i] = sum > 255 ? 255 : sum;
	}
}

static inline void find_local_min1(uint8 *mb, uint16 *min, uint16 *xm, uint16 *ym, uint16 wm)
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

static inline uint16 find_mv(uint8 *mb1, uint8 *mb2, uint16 min, uint16 xm, uint16 ym, uint16 *xo1, uint16 *yo1, uint16 *xo2, uint16 *yo2, uint16 wm)
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

static inline void get_small(uint8 *mb1, uint8 *mb2, uint16 xm, uint16 ym, uint16 wm, uint16 wm1)
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

void seg_compare(Pixel *pix, Edge *edge, uint32 nedge, uint8 *grad1, uint8 *grad2, uint8 *img1, uint8 *img2, uint8 *mmb, uint32 w, uint32 h, uint32 mvs)
{
	uint32 i, j, yx, yx1, yx2, y, y1, y2,  x, x1, x2, w1 = w-2, h1 = h-2, npix = 0, ndge = 0, px = 3, po;
	uint32 sq = w*h, wm[2], sqm[2], nm = 1;
	int xv, yv;
	wm[0] = ((mvs<<1)+1); sqm[0] = wm[0]*wm[0];
	wm[1] = ((nm<<1)+1);  sqm[1] = wm[1]*wm[1];

	uint8 *mb[2], *sum;// *sad[3];
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
		find_local_min1(mb[0], &min[0], &xm, &ym, wm[0]);
		printf("match 1 min = %3d x = %4d y = %4d x1 = %4d y1 = %4d\n", min[0], xm, ym, p->x, p->y);
		find_local_min1(sad[0], &min[0], &xm, &ym, wm[0]);
		printf("sad   0 min = %3d x = %4d y = %4d\n", min[0], xm, ym);
		find_local_min1(sad[1], &min[0], &xm, &ym, wm[0]);
		printf("sad   1 min = %3d x = %4d y = %4d\n", min[0], xm, ym);
		find_local_min1(sad[2], &min[0], &xm, &ym, wm[0]);
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
		find_local_min1(mb[1], &min[1], &xm, &ym, wm[0]);
		printf("match 2 min = %3d x = %4d y = %4d x2 = %4d y2 = %4d\n", min[1], xm, ym, p1->x, p1->y);
		find_local_min1(sad[0], &min[1], &xm, &ym, wm[0]);
		printf("sad   0 min = %3d x = %4d y = %4d\n", min[0], xm, ym);
		find_local_min1(sad[1], &min[1], &xm, &ym, wm[0]);
		printf("sad   1 min = %3d x = %4d y = %4d\n", min[0], xm, ym);
		find_local_min1(sad[2], &min[1], &xm, &ym, wm[0]);
		printf("sad   2 min = %3d x = %4d y = %4d\n", min[0], xm, ym);
		///-----------------------------------------------------------------------------------------
		*/
		min_add(mb[0], mb[1], sum, sqm[0]);
		find_local_min1(sum, &min[2], &xm, &ym, wm[0]);
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
			//find_local_min1(sum, &min[2], &xm, &ym, wm[1]);
			//p1->mach = find_mv(mb[!(j&1)], mb[j&1], min[2], xm, ym, &xo1, &yo1, &xo2, &yo2, wm[1]);
			//p1->vx = p->vx + xo2 - nm; p1->vy = p->vy + yo2 - nm;
			xv = p1->x + p->vx; yv = p1->y + p->vy;
			if(xv >= 0 && xv < w && yv >= 0 && yv < h){
				block_match_new(grad2, img1, img2, mb[0], p1->x, p1->y, p1->x + p->vx, p1->y + p->vy, p1->yx, w, h, nm, 253);
				find_local_min1(mb[0], &min[2], &xm, &ym, wm[1]);
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

void seg_draw_vec(Pixel *pix, uint32 npix, uint8 *img, uint32 w, uint32 h)
{
	uint32 i, j, k, sq = w*h, pixs = 0, nline = 0;
	Vector xy;
	for(i=0; i < sq; i++){
		if(pix[i].nout && pix[i].vx && pix[i].vy) {
			xy.x1 = pix[i].x; xy.y1 = pix[i].y;
			xy.x2 = pix[i].x + pix[i].vx; xy.y2 = pix[i].y + pix[i].vy;
			draw_line2(img, &xy, w, 200, 0);
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

static inline uint32 intersect(uint8 *img, uint32 yx, uint32 w)
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

void seg_intersect_pix(uint8 *img1, uint8 *img2, uint32 w, uint32 h)
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

static inline void copy_block(uint8 *img1, uint32 yx1, uint8 *img2, uint32 yx2, uint32 w)
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

static inline void copy_vector(uint8 *img1, Vector *v1, uint8 *img2, Vector *v2, uint32 w)
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

void seg_mvector_copy(Pixel *pix, uint8 *grad1, uint8 *img1, uint8 *img2, uint32 w, uint32 h)
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

void seg_quant(uint8 *img1, uint8 *img2, uint32 w, uint32 h, uint32 q)
{
	uint32 x, y, yx, sq = w*h;
	for(y=0; y < sq; y+=w){
		for(x=0; x < w; x++){
			yx = y + x;
			img2[yx] = (img1[yx]>>q)<<q;
		}
	}
}

void seg_fall_forest2(uint8 *img, uint8 *img1, uint32 w, uint32 h)
{
	uint32 y, x, yx, sq = w*h, dir, w1 = w-1, h1 = h-1, min;
	int dyx;
    //for(x=0; x<sq; x++) img1[x] = img[x];
	//for(x=0; x < sq; x++) img1[x] = 255;
	for(y=0; y < h; y++){
		for(x=0; x < w; x++){
			yx = y*w + x;
			//if(!img[yx]) img1[yx] = 0;

			if(img[yx]) {
				if( img[yx-1] 	>= img[yx] &&
					img[yx-w]	>= img[yx] &&
					img[yx+1] 	>= img[yx] &&
					img[yx+w] 	>= img[yx]
					//img[yx-1-w] <= img[yx] &&
					//img[yx+1-w] <= img[yx] &&
					//img[yx-1+w] <= img[yx] &&
					//img[yx+1+w] <= img[yx]
					) img1[yx] = 0;
			}
			/*
			if(img[yx]) {
				min = img[yx];
                if(img[yx-1  ] < min) { min = img[yx-1  ]; dyx = -1;}
                //if(img[yx-1-w] < min) { min = img[yx-1-w]; dyx = -1-w;}
                if(img[yx  -w] < min) { min = img[yx  -w]; dyx = -w;}
                //if(img[yx+1-w] < min) { min = img[yx+1-w]; dyx =  1-w;}
                if(img[yx+1  ] < min) { min = img[yx+1  ]; dyx = 1;}
                //if(img[yx+1+w] < min) { min = img[yx+1+w]; dyx =  1+w;}
                if(img[yx  +w] < min) { min = img[yx  +w]; dyx = w;}
                //if(img[yx-1+w] < min) { min = img[yx-1+w]; dyx = -1+w;}
                img1[yx+dyx] =0;
 			}*/
		}
	}
}

void seg_fall_forest1(uint8 *img, uint8 *img1, uint8 *img2, uint32 w, uint32 h)
{
	uint32 y, x, yx, sq = w*h, dir, w1 = w-1, h1 = h-1, max;
	int dyx;
    //for(x=0; x<sq; x++) img1[x] = img[x];
	for(x=0; x < sq; x++) img1[x] = 0;
	for(y=1; y < h1; y++){
		for(x=1; x < w1; x++){
			yx = y*w + x;
			//g[0] = abs(img[yx-1  ] - img[yx+1  ]);
			//g[1] = abs(img[yx-1-w] - img[yx+1+w]);
			//g[2] = abs(img[yx-w  ] - img[yx+w  ]);
			//g[3] = abs(img[yx+1-w] - img[yx-1+w]);
			//if(y == w)
			//printf("yx-1 = %3d yx+1 = %3d yx-w = %3d yx+w = %3d yx-1-w = %3d yx+1+w = %3d yx+1-w = %3d yx-1+w = %3d\n",
			//		yx-1, yx+1, yx-w, yx+w, yx-1-w, yx+1+w, yx+1-w, yx-1+w);
			//max = g[0]; in = 2;
			//if(max < g[1]) { max = g[1]; in = 3; }
			//if(max < g[2]) { max = g[2]; in = 0; }
			//if(max < g[3]) { max = g[3]; in = 1; }
			if( img[yx] > img[yx-w  ] && img[yx] > img[yx+w  ]) { img2[yx] = 255; } else img2[yx] = 0;
			if( img[yx] > img[yx+1-w] && img[yx] > img[yx-1+w]) { img2[yx] = 255; } else img2[yx] = 0;
			if( img[yx] > img[yx-1  ] && img[yx] > img[yx+1  ]) { img2[yx] = 255; } else img2[yx] = 0;
			if( img[yx] > img[yx-1-w] && img[yx] > img[yx+1+w]) { img2[yx] = 255; } else img2[yx] = 0;
			/*
			if(img[yx]) {
				switch(img1[yx]){
					case 0 : { if( img[yx] > img[yx-w  ] && img[yx] > img[yx+w  ]) img2[yx] = 255; else img2[yx] = 0; break; }
					case 1 : { if( img[yx] > img[yx+1-w] && img[yx] > img[yx-1+w]) img2[yx] = 255; else img2[yx] = 0; break; }
					case 2 : { if( img[yx] > img[yx-1  ] && img[yx] > img[yx+1  ]) img2[yx] = 255; else img2[yx] = 0; break; }
					case 3 : { if( img[yx] > img[yx-1-w] && img[yx] > img[yx+1+w]) img2[yx] = 255; else img2[yx] = 0; break; }
				}
			}
			*/
		}
	}
}

