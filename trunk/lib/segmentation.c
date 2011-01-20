#include <walet.h>
#include <stdio.h>

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
			if(max < g[1]) { max = g[1]; in = 3;}
			if(max < g[2]) { max = g[2]; in = 0;}
			if(max < g[3]) { max = g[3]; in = 1;}
			max = (g[0] + g[1] +g[2] + g[3])>>2;
			//img1[yx] = max>>th ? (max >= 255 ? 254 : (max>>th)<<th): 0; img2[yx] = in;
			img1[yx] = max>>th ? (max >253 ? 253 : max) : 0; img2[yx] = in;
			//img1[yx] = max>>th ? (max>>th)<<th : 0; img2[yx] = in;
		}
	}
}

static inline uint32 loc_max(imgtype *img, imgtype *img1, uint32 yx, uint32 w)
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
		if(img[yx  -w] > max) { max = img[yx  -w]; in =   -w; }
		if(img[yx+1-w] > max) { max = img[yx+1-w]; in = +1-w; }
		if(img[yx+1  ] >= max) { max = img[yx+1  ]; in = +1  ; }
		if(img[yx+1+w] > max) { max = img[yx+1+w]; in = +1+w; }
		if(img[yx  +w] > max) { max = img[yx  +w]; in =   +w; }
		goto end;
	}
	if(in1 == -1-w){
		if(img[yx+1-w] > max) { max = img[yx+1-w]; in = +1-w; }
		if(img[yx+1  ] > max) { max = img[yx+1  ]; in = +1  ; }
		if(img[yx+1+w] >= max) { max = img[yx+1+w]; in = +1+w; }
		if(img[yx  +w] > max) { max = img[yx  +w]; in =   +w; }
		if(img[yx-1+w] > max) { max = img[yx-1+w]; in = -1+w; }
		goto end;
	}
	if(in1 ==   -w){
		if(img[yx+1  ] > max) { max = img[yx+1  ]; in = +1  ; }
		if(img[yx+1+w] > max) { max = img[yx+1+w]; in = +1+w; }
		if(img[yx  +w] >= max) { max = img[yx  +w]; in =   +w; }
		if(img[yx-1+w] > max) { max = img[yx-1+w]; in = -1+w; }
		if(img[yx  -1] > max) { max = img[yx  -1]; in =   -1; }
		goto end;
	}
	if(in1 ==  1-w){
		if(img[yx+1+w] > max) { max = img[yx+1+w]; in = +1+w; }
		if(img[yx  +w] > max) { max = img[yx  +w]; in =   +w; }
		if(img[yx-1+w] >= max) { max = img[yx-1+w]; in = -1+w; }
		if(img[yx  -1] > max) { max = img[yx  -1]; in =   -1; }
		if(img[yx-1-w] > max) { max = img[yx-1-w]; in = -1-w; }
		goto end;
	}
	if(in1 ==  1  ){
		if(img[yx  +w] > max) { max = img[yx  +w]; in =   +w; }
		if(img[yx-1+w] > max) { max = img[yx-1+w]; in = -1+w; }
		if(img[yx  -1] >= max) { max = img[yx  -1]; in =   -1; }
		if(img[yx-1-w] > max) { max = img[yx-1-w]; in = -1-w; }
		if(img[yx  -w] > max) { max = img[yx  -w]; in =   -w; }
		goto end;
	}
	if(in1 ==  1+w){
		if(img[yx-1+w] > max) { max = img[yx-1+w]; in = -1+w; }
		if(img[yx  -1] > max) { max = img[yx  -1]; in =   -1; }
		if(img[yx-1-w] >= max) { max = img[yx-1-w]; in = -1-w; }
		if(img[yx  -w] > max) { max = img[yx  -w]; in =   -w; }
		if(img[yx+1-w] > max) { max = img[yx+1-w]; in = +1-w; }
		goto end;
	}
	if(in1 ==    w){
		if(img[yx  -1] > max) { max = img[yx  -1]; in =   -1; }
		if(img[yx-1-w] > max) { max = img[yx-1-w]; in = -1-w; }
		if(img[yx  -w] >= max) { max = img[yx  -w]; in =   -w; }
		if(img[yx+1-w] > max) { max = img[yx+1-w]; in = +1-w; }
		if(img[yx+1  ] > max) { max = img[yx+1  ]; in = +1  ; }
		goto end;
	}
	if(in1 == -1+w){
		if(img[yx-1-w] > max) { max = img[yx-1-w]; in = -1-w; }
		if(img[yx  -w] > max) { max = img[yx  -w]; in =   -w; }
		if(img[yx+1-w] >= max) { max = img[yx+1-w]; in = +1-w; }
		if(img[yx+1  ] > max) { max = img[yx+1  ]; in = +1  ; }
		if(img[yx+1+w] > max) { max = img[yx+1+w]; in =  1+w; }
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
		if(img[yx  -w] > max) { max = img[yx  -w]; *dx1 =  0; *dy1 = -1;}
		if(img[yx+1-w] > max) { max = img[yx+1-w]; *dx1 =  1; *dy1 = -1;}
		if(img[yx+1  ] >= max){ max = img[yx+1  ]; *dx1 =  1; *dy1 =  0;}
		if(img[yx+1+w] > max) { max = img[yx+1+w]; *dx1 =  1; *dy1 =  1;}
		if(img[yx  +w] > max) { max = img[yx  +w]; *dx1 =  0; *dy1 =  1;}
		goto end;
	}
	if(dx == -1 && dy == -1){
		if(img[yx+1-w] > max) { max = img[yx+1-w]; *dx1 =  1; *dy1 = -1;}
		if(img[yx+1  ] > max) { max = img[yx+1  ]; *dx1 =  1; *dy1 =  0;}
		if(img[yx+1+w] >= max){ max = img[yx+1+w]; *dx1 =  1; *dy1 =  1;}
		if(img[yx  +w] > max) { max = img[yx  +w]; *dx1 =  0; *dy1 =  1;}
		if(img[yx-1+w] > max) { max = img[yx-1+w]; *dx1 = -1; *dy1 =  1;}
		goto end;
	}
	if(dx ==  0 && dy == -1){
		if(img[yx+1  ] > max) { max = img[yx+1  ]; *dx1 =  1; *dy1 =  0;}
		if(img[yx+1+w] > max) { max = img[yx+1+w]; *dx1 =  1; *dy1 =  1;}
		if(img[yx  +w] >= max){ max = img[yx  +w]; *dx1 =  0; *dy1 =  1;}
		if(img[yx-1+w] > max) { max = img[yx-1+w]; *dx1 = -1; *dy1 =  1;}
		if(img[yx  -1] > max) { max = img[yx  -1]; *dx1 = -1; *dy1 =  0;}
		goto end;
	}
	if(dx ==  1 && dy == -1){
		if(img[yx+1+w] > max) { max = img[yx+1+w]; *dx1 =  1; *dy1 =  1;}
		if(img[yx  +w] > max) { max = img[yx  +w]; *dx1 =  0; *dy1 =  1;}
		if(img[yx-1+w] >= max){ max = img[yx-1+w]; *dx1 = -1; *dy1 =  1;}
		if(img[yx  -1] > max) { max = img[yx  -1]; *dx1 = -1; *dy1 =  0;}
		if(img[yx-1-w] > max) { max = img[yx-1-w]; *dx1 = -1; *dy1 = -1;}
		goto end;
	}
	if(dx ==  1 && dy ==  0){
		if(img[yx  +w] > max) { max = img[yx  +w]; *dx1 =  0; *dy1 =  1;}
		if(img[yx-1+w] > max) { max = img[yx-1+w]; *dx1 = -1; *dy1 =  1;}
		if(img[yx  -1] >= max){ max = img[yx  -1]; *dx1 = -1; *dy1 =  0;}
		if(img[yx-1-w] > max) { max = img[yx-1-w]; *dx1 = -1; *dy1 = -1;}
		if(img[yx  -w] > max) { max = img[yx  -w]; *dx1 =  0; *dy1 = -1;}
		goto end;
	}
	if(dx ==  1 && dy ==  1){
		if(img[yx-1+w] > max) { max = img[yx-1+w]; *dx1 = -1; *dy1 =  1;}
		if(img[yx  -1] > max) { max = img[yx  -1]; *dx1 = -1; *dy1 =  0;}
		if(img[yx-1-w] >= max){ max = img[yx-1-w]; *dx1 = -1; *dy1 = -1;}
		if(img[yx  -w] > max) { max = img[yx  -w]; *dx1 =  0; *dy1 = -1;}
		if(img[yx+1-w] > max) { max = img[yx+1-w]; *dx1 =  1; *dy1 = -1;}
		goto end;
	}
	if(dx ==  0 && dy ==  1){
		if(img[yx  -1] > max) { max = img[yx  -1]; *dx1 = -1; *dy1 =  0;}
		if(img[yx-1-w] > max) { max = img[yx-1-w]; *dx1 = -1; *dy1 = -1;}
		if(img[yx  -w] >= max){ max = img[yx  -w]; *dx1 =  0; *dy1 = -1;}
		if(img[yx+1-w] > max) { max = img[yx+1-w]; *dx1 =  1; *dy1 = -1;}
		if(img[yx+1  ] > max) { max = img[yx+1  ]; *dx1 =  1; *dy1 =  0;}
		goto end;
	}
	if(dx == -1 && dy ==  1){
		if(img[yx-1-w] > max) { max = img[yx-1-w]; *dx1 = -1; *dy1 = -1;}
		if(img[yx  -w] > max) { max = img[yx  -w]; *dx1 =  0; *dy1 = -1;}
		if(img[yx+1-w] >= max){ max = img[yx+1-w]; *dx1 =  1; *dy1 = -1;}
		if(img[yx+1  ] > max) { max = img[yx+1  ]; *dx1 =  1; *dy1 =  0;}
		if(img[yx+1+w] > max) { max = img[yx+1+w]; *dx1 =  1; *dy1 =  1;}
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
	Pixel *pix2;
	pix->out[pix->nout] = pix1; pix->pow[pix->nout] = pow; pix->nout++;
	pix1->in[pix->nin] = pix; pix1->nin++;
	//pix->dif[0] = pix->x - pix1->x;  pix->dif[1] = pix->y - pix1->y;
	/*
	if(pix1->nout == 1 && pix->nout == 1) {
		if(	(!pix->dif[0] && !pix1->dif[0]) ||
			(!pix->dif[1] && !pix1->dif[1]) ||
			(pix->dif[0] == pix1->dif[0] && pix->dif[1] == pix1->dif[1]))
		{
			pix->out[pix->nout-1] = pix1->out[pix1->nout-1];
			pix1->out[pix1->nout-1]->in[pix1->out[pix1->nout-1]->nin-1] = pix;
			pix1->draw = 0;
			//pix1->x = 0;
		}
	}*//*
	//printf("pix  x = %d y = %d dif = %d dif1 = %d nin = %d nout = %d in = %p out = %p\n",
	//		pix->x, pix->y, pix->dif[0], pix->dif[1], pix->nin, pix->nout, pix->in[pix->nin-1], pix->out[pix->nout-1]);

	if(pix->in[0] && pix->nin == 1){
		pix2 = pix->in[pix->nin-1];
		if(pix2->nout){
			//printf("pix2 x = %d y = %d dif = %d dif1 = %d nin = %d nout = %d in = %p out = %p\n",
			//		pix2->x, pix2->y, pix2->dif[0], pix2->dif[1], pix2->nin, pix2->nout, pix2->in[pix2->nin-1], pix2->out[pix2->nout-1]);
			if(	(!pix->dif[0] && !pix2->dif[0]) ||
				(!pix->dif[1] && !pix2->dif[1]) ||
				(pix->dif[0] == pix2->dif[0] && pix->dif[1] == pix2->dif[1]))
			{
				pix2->out[pix2->nout-1] = pix->out[pix->nout-1];
				pix->draw = 0;
				//pix1->x = 0;
			}

		}
	}*/
}

void seg_local_max( Pixel *pix, uint32 *npix, imgtype *img, imgtype *img1, uint32 w, uint32 h)
{
	uint32 y, y1, x, yx, i, sq = w*h - w, w1 = w-1, is = 0;
	(*npix) = 0;
	for(y=1, y1=w; y1 < sq; y++, y1+=w){
		for(x=1; x < w1; x++){
			yx = y1 + x;
			//if(img[yx] && img[yx]!= 255){
			if(img[yx]){
				if(loc_max(img, img1, yx, w)){
					//new_pix(&pix[yx], img[yx], x, y);
					(*npix)++;
					img[yx] = 255;
				} //else {
				//	img1[yx] = img1[yx];
				//}
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

void seg_line(Pixel *pix, imgtype *img, imgtype *img1, uint32 w, uint32 h)
{
	uint32 y, y1, y2, yp, x, x1, x2, xp, yx, yx1, yx2, yx3, i, w1 = w-1, h1 = h-1,  nline = 0, min, npix = 0, pc;
	int d, d1, difx, difx1, dify, dify1, dx, dy, dx1, dy1;
	for(y=1; y < h1; y++){
		for(x=1; x < w1; x++){
			yx = y*w + x;
			if(img[yx] == 255){
				pc = 0;
				xp = x; yp = y;
				new_pix(&pix[yx], img[yx], xp, yp); npix++;
				//printf("npix = %d x = %d y = %d yx = %d\n", npix, x, y, yx);
				dir1(img, w,  yx, 0,  0,  &dx,  &dy);
				//dir1(img, w, dx, dy, &dx1, &dy1);
				yx1 = yx; yx3 = yx;
				x1 = x; y1 = y;
				//x2 = x; y2 = y;
				min = img[yx];
				while(1){
					yx = yx + dy*w + dx; xp = xp + dx; yp = yp + dy;
					//printf("yx = %d dx = %d dx = %d\n", yx, dx, dy);
					//printf("%3d %3d %3d \n", img[yx-1-w], img[yx-w], img[yx+1-w]);
					//printf("%3d %3d %3d \n", img[yx-1  ], img[yx  ], img[yx+1  ]);
					//printf("%3d %3d %3d \n", img[yx-1+w], img[yx+w], img[yx+1+w]);
					if(yx > 2073600) break;
					min = img[yx] < min ? img[yx] : min;
					if(img[yx] == 254){
						new_line(&pix[yx1], &pix[yx], min); nline++;
						break;
					}
					if(img[yx] == 255){
						if(yx == yx1) break;
						difx = x - x1; dify = y - y1;
						yx2 = yx;
						img[yx] = 254;
						pc++; break;
					}
					dir1(img, w, yx, -dx, -dy, &dx, &dy);
				}

				if(pc){
					dir1(img, w, yx, -dx, -dy, &dx, &dy);
					while(1){
						yx = yx + dy*w + dx; xp = xp + dx; yp = yp + dy;
						//printf("yx = %d dx = %d dx = %d\n", yx, dx, dy);
						//printf("%3d %3d %3d \n", img[yx-1-w], img[yx-w], img[yx+1-w]);
						//printf("%3d %3d %3d \n", img[yx-1  ], img[yx  ], img[yx+1  ]);
						//printf("%3d %3d %3d \n", img[yx-1+w], img[yx+w], img[yx+1+w]);
						if(yx > 2073600) break;
						min = img[yx] < min ? img[yx] : min;
						if(img[yx] == 254) {
							difx1 = difx; dify1 = dify;
							difx = xp - x2; dify = yp - y2;
							if(is_in_line(difx, dify, difx1, dify1)) {
								new_line(&pix[yx1], &pix[yx], min); nline++;
							} else {
								new_pix(&pix[yx2], img[yx2], x2, y2); npix++;
								new_line(&pix[yx1], &pix[yx2], min); nline++;
								new_line(&pix[yx2], &pix[yx], min); nline++;
							}
							break;
						}
						if(img[yx] == 255){
							img[yx] = 254;
							if(yx != yx2) {
								difx1 = difx; dify1 = dify;
								difx = xp - x2; dify = yp - y2;
								if(is_in_line(difx, dify, difx1, dify1)) {
									yx2 = yx; x2 = xp; y2 = yp;
								} else {
									new_pix(&pix[yx2], img[yx2], x2, y2); npix++;
									new_line(&pix[yx1], &pix[yx2], min); nline++;
									yx1 = yx2; yx2 = yx;
									x1 = x2; y1 = y2;
									x2 = xp; y2 = yp;
								}
							} else {
								new_pix(&pix[yx], img[yx], xp, yp); npix++;
								new_line(&pix[yx1], &pix[yx], min); nline++;
								break;
							}
						}
						dir1(img, w, yx, -dx, -dy, &dx, &dy);
					}
				}
			}
		}
	}
	//printf("Numbers of pixels  = %d\n", *npix);
	printf("Numbers of lines   = %d\n", nline);
}

void seg_line1(Pixel *pix, imgtype *img, imgtype *img1, uint32 w, uint32 h)
{
	uint32 y, y1, x, yx, yx1, yx2, i, sq = w*h - w, w1 = w-1, is = 0, nline = 0, min, loop = 0;
	int d, d1;
	for(y=1, y1=w; y1 < sq; y++, y1+=w){
		for(x=1; x < w1; x++){
			yx = y1 + x;
			if(img[yx] == 255 &&  pix[yx].nout == 0 && pix[yx].nin == 0){
			//if(img[yx] == 255){
				d  = dir(img, yx, w, 0);
				d1 = dir(img, yx, w, d);
				//if(d == pix[yx].d || d1 == pix[yx].d) break;
				//d1 = -d;
				yx1 = yx; yx2 = yx;
				min = img[yx];
				while(1){
					yx = yx + d;
					min = img[yx] < min ? img[yx] : min;
					//printf("yx = %d d = %d\n", yx, d);
					//printf("%3d %3d %3d \n", img[yx-1-w], img[yx-w], img[yx+1-w]);
					//printf("%3d %3d %3d \n", img[yx-1  ], img[yx  ], img[yx+1  ]);
					//printf("%3d %3d %3d \n", img[yx-1+w], img[yx+w], img[yx+1+w]);
					if(img[yx] == 0) break;
					if(img[yx] == 255){
						if(yx1 != yx) {
							//pix[yx1].pow = min;
							new_line(&pix[yx1], &pix[yx], min); nline++;
							//pix[yx].d = -d;
						} else {
							loop++;
							//pix[yx1].end = 1;
						}
						break;
					}
					d = dir(img, yx, w, -d);
				}
				d = d1;
				yx = yx2; yx1 = yx2;
				min = img[yx];
				while(1){
					yx = yx + d;
					min = img[yx] < min ? img[yx] : min;
					//printf("yx = %d d = %d\n", yx, d);
					//printf("%3d %3d %3d \n", img[yx-1-w], img[yx-w], img[yx+1-w]);
					//printf("%3d %3d %3d \n", img[yx-1  ], img[yx  ], img[yx+1  ]);
					//printf("%3d %3d %3d \n", img[yx-1+w], img[yx+w], img[yx+1+w]);
					if(img[yx] == 0) break;
					if(img[yx] == 255){
						if(yx1 != yx) {
							//pix[yx].pow = min;
							new_line(&pix[yx], &pix[yx1], min); nline++;
							//pix[yx].d = -d;
						} else {
							loop++;
							//pix[yx].end = 1;
						}
						break;
					}
					d = dir(img, yx, w, -d);
				}
			}
		}
	}
	//printf("Numbers of pixels  = %d\n", *npix);
	printf("Numbers of lines = %d loops = %d\n", nline, loop);
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
			if(pix[i].nout) {
				//for(k=0; k < pix[i].nnei; k++) printf("pix[%d].pix[%d] x = %d y = %d\n", i, k, pix[i].pix[k]->x, pix[i].pix[k]->y);
				for(j=0; j < pix[i].nout; j++) {
					//printf("%6d x = %4d y = %4d nout = %d nin = %d x = %d y = %d\n", i, pix[i].x, pix[i].y, pix[i].nout, pix[i].nin, pix[i].out[j]->x, pix[i].out[j]->y);
					draw_line(img, pix[i].x, pix[i].y, pix[i].out[j]->x, pix[i].out[j]->y, w, pix[i].pow[j]);
					nline++;
				}
				//printf("%6d x = %4d y = %4d nnei = %d pow = %d\n", i, pix[i].x, pix[i].y, pix[i].nnei, pix[i].pow);
			}
			//for(j=0; j < pix[i].nnei; j++){
			//	draw_line(img, pix[i].x, pix[i].y, pix[i].pix[j+1]->x, pix[i].pix[j+1]->y, w, pix[i].pix[j+1]->pow);
				//draw_line(img, pix[i].x, pix[i].y, pix[i].pix[j+1]->x, pix[i].pix[j+1]->y, w, 255);
			//	printf("pix[%d].pix[%d] x = %d y = %d pow = %d\n", i, j, pix[i].pix[j+1]->x, pix[i].pix[j+1]->y, pix[i].pix[j+1]->pow);
			//}
		//}
		/*
		if(pix[i].nnei) {
			//printf("%6d x = %4d y = %4d nnei = %d\n", i, pix[i].x, pix[i].y, pix[i].nnei);
			//for(k=0; k < pix[i].nnei; k++) printf("pix[%d].pix[%d] x = %d y = %d\n", i, k, pix[i].pix[k]->x, pix[i].pix[k]->y);

			for(j=0; j < pix[i].nnei; j++){
				draw_line(img, pix[i].x, pix[i].y, pix[i].pix[j]->x, pix[i].pix[j]->y, w, pix[i].pow[j]);
				//draw_line(img, pix[i].x, pix[i].y, pix[i].pix[j]->x, pix[i].pix[j]->y, w, 255);
			}
		}*/

	}
	for(i=0; i < w*h; i++)
		//if(pix[i].draw && (pix[i].nout || pix[i].nin)) {
		if(pix[i].nout || pix[i].nin) {
		//if(pix[i].end) {
		img[i] = 255;
		pixs++;
	}
	printf("Numbers of pixels  = %d\n", pixs);
	printf("Numbers of lines   = %d\n", nline);
}
