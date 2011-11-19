#include <walet.h>
#include <stdio.h>
#include <math.h>

void seg_grad(uint8 *img, uint8 *img1, uint32 w, uint32 h, uint32 th)
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
			//max = g[0];
			img1[yx] = (max>>th) ? (max > 252 ? 252 : max) : 0;
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
		//if(img[yx  -w] > max) { max = img[yx  -w]; in =   -w; }
		if(img[yx+1  ] > max) { max = img[yx+1  ]; in = +1  ; }
		if(img[yx+1-w] > max) { max = img[yx+1-w]; in = +1-w; }
		if(img[yx+1+w] > max) { max = img[yx+1+w]; in = +1+w; }
		//if(img[yx  +w] > max) { max = img[yx  +w]; in =   +w; }
		return in;
	}
	else if(in1 == -1-w){
		//if(img[yx+1-w] > max) { max = img[yx+1-w]; in = +1-w; }
		if(img[yx+1+w] > max) { max = img[yx+1+w]; in = +1+w; }
		if(img[yx+1  ] > max) { max = img[yx+1  ]; in = +1  ; }
		if(img[yx  +w] > max) { max = img[yx  +w]; in =   +w; }
		//if(img[yx-1+w] > max) { max = img[yx-1+w]; in = -1+w; }
		return in;
	}
	else if(in1 ==   -w){
		//if(img[yx+1  ] > max) { max = img[yx+1  ]; in = +1  ; }
		if(img[yx  +w] > max) { max = img[yx  +w]; in =   +w; }
		if(img[yx+1+w] > max) { max = img[yx+1+w]; in = +1+w; }
		if(img[yx-1+w] > max) { max = img[yx-1+w]; in = -1+w; }
		//if(img[yx  -1] > max) { max = img[yx  -1]; in =   -1; }
		return in;
	}
	else if(in1 ==  1-w){
		//if(img[yx+1+w] > max) { max = img[yx+1+w]; in = +1+w; }
		if(img[yx-1+w] > max) { max = img[yx-1+w]; in = -1+w; }
		if(img[yx  +w] > max) { max = img[yx  +w]; in =   +w; }
		if(img[yx  -1] > max) { max = img[yx  -1]; in =   -1; }
		//if(img[yx-1-w] > max) { max = img[yx-1-w]; in = -1-w; }
		return in;
	}
	else if(in1 ==  1  ){
		//if(img[yx  +w] > max) { max = img[yx  +w]; in =   +w; }
		if(img[yx  -1] > max) { max = img[yx  -1]; in =   -1; }
		if(img[yx-1+w] > max) { max = img[yx-1+w]; in = -1+w; }
		if(img[yx-1-w] > max) { max = img[yx-1-w]; in = -1-w; }
		//if(img[yx  -w] > max) { max = img[yx  -w]; in =   -w; }
		return in;
	}
	else if(in1 ==  1+w){
		//if(img[yx-1+w] > max) { max = img[yx-1+w]; in = -1+w; }
		if(img[yx-1-w] > max) { max = img[yx-1-w]; in = -1-w; }
		if(img[yx  -1] > max) { max = img[yx  -1]; in =   -1; }
		if(img[yx  -w] > max) { max = img[yx  -w]; in =   -w; }
		//if(img[yx+1-w] > max) { max = img[yx+1-w]; in = +1-w; }
		return in;
	}
	else if(in1 ==    w){
		//if(img[yx  -1] > max) { max = img[yx  -1]; in =   -1; }
		if(img[yx  -w] > max) { max = img[yx  -w]; in =   -w; }
		if(img[yx-1-w] > max) { max = img[yx-1-w]; in = -1-w; }
		if(img[yx+1-w] > max) { max = img[yx+1-w]; in = +1-w; }
		//if(img[yx+1  ] > max) { max = img[yx+1  ]; in = +1  ; }
		return in;
	}
	else if(in1 == -1+w){
		//if(img[yx-1-w] > max) { max = img[yx-1-w]; in = -1-w; }
		if(img[yx+1-w] > max) { max = img[yx+1-w]; in = +1-w; }
		if(img[yx  -w] > max) { max = img[yx  -w]; in =   -w; }
		if(img[yx+1  ] > max) { max = img[yx+1  ]; in = +1  ; }
		//if(img[yx+1+w] > max) { max = img[yx+1+w]; in =  1+w; }
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

/*	\brief	Finf pixels with lines intersection.
	\param	grad	The pointer to input gradient image.
	\param	con		The pointer to output contour image.
	\param  w		The image width.
	\param  h		The image height.
*/
void seg_find_intersect(uint8 *grad, uint8 *con, uint32 w, uint32 h)
{
	uint32 y, y1, x, yx, yw, yx1, yx2, i, h1 = h-1, w1 = w-1, is = 0;
	int d1, d2, npix = 0;

	for(y=1; y < h1; y++){
		yw = y*w;
		for(x=1; x < w1; x++){
			yx = yw + x;
			if(grad[yx] && !con[yx]){
				if(loc_max(grad, yx, w)){
					//printf("x = %d y = %d\n", x, y);
					yx1 = yx; yx2 = yx;
					con[yx1] = grad[yx1];
					d1 = dir(grad, yx1, w, 0);
					d2 = dir(grad, yx1, w, d1);
					while(1){
						yx1 = yx1 + d1;
						if(con[yx1]) {
							con[yx1] = 255; grad[yx1] = 255;
							npix++;
							break;
						}
						con[yx1] = grad[yx1]; grad[yx1] = 254;
						d1 = dir(grad, yx1, w, -d1);
					}
					while(1){
						yx2 = yx2 + d2;
						if(con[yx2]) {
							con[yx2] = 255; grad[yx2] = 255;
							npix++;
							break;
						}
						con[yx2] = grad[yx2]; grad[yx2] = 254;
						d2 = dir(grad, yx2, w, -d2);
					}
				}
				//break;
			}
		}
	}
	printf("Numbers of intersection  = %d\n", npix);
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

static inline uint32 new_vertex(uint8 *con, Vertex *vx, Vertex **vp, Line **lp, uint32 x, uint32 y, uint32 yx, uint32 w)
{
	vx->x = x; vx->y = y;
	vx->n = check_neighbor(con, yx, w, &vx->di);
	vx->cn = 0;
	vx->lp = lp;
	*vp = vx;
	return vx->n;
	//printf("x = %d y = %d n = %d vp = %p\n", (*vp)->x, (*vp)->y, (*vp)->n, *vp);

}

static inline void new_line(Line *ln, Vertex *vx1, Vertex *vx2, uint32 nd1, uint32 nd2, uint32 *l, uint32 *r)
{
	ln->vx[0] = vx1; ln->vx[1] = vx2;
	ln->l[0] = l[0]; ln->l[1] = l[1]; ln->l[2] = l[1];
	//ln->r[0] = r[0]; ln->r[1] = r[1]; ln->r[2] = r[2];
	vx1->lp[nd1] = ln; vx2->lp[nd2] = ln;
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
	//vx->n++;
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
	//vx->n++;
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

/*      \brief  Remove direction bit.
        \param  vx              The pointer to vertex.
        \param  d               The direction.
        \param  w               The image width.
*/
static inline void remove_dir(Vertex *vx, int d, uint32 w)
{
	//vx->n--;
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
	if		((vx->di&128) && !(vx->cn&128)) { *dx =-1; *dy = 0; vx->cn |= 128;	*nd = 0; 	return 1; }
	else if	((vx->di&64 ) && !(vx->cn&64 )) { *dx =-1; *dy =-1; vx->cn |= 64;	*nd = 1;  	return 1; }
	else if ((vx->di&32 ) && !(vx->cn&32 )) { *dx = 0; *dy =-1; vx->cn |= 32;	*nd = 2;  	return 1; }
	else if ((vx->di&16 ) && !(vx->cn&16 )) { *dx = 1; *dy =-1; vx->cn |= 16;	*nd = 3;  	return 1; }
	else if ((vx->di&8  ) && !(vx->cn&8  )) { *dx = 1; *dy = 0; vx->cn |= 8;	*nd = 4;  	return 1; }
	else if ((vx->di&4  ) && !(vx->cn&4  )) { *dx = 1; *dy = 1; vx->cn |= 4;	*nd = 5;  	return 1; }
	else if ((vx->di&2  ) && !(vx->cn&2  )) { *dx = 0; *dy = 1; vx->cn |= 2;	*nd = 6;  	return 1; }
	else if ((vx->di&1  ) && !(vx->cn&1  )) { *dx =-1; *dy = 1; vx->cn |= 1;	*nd = 7;  	return 1; }
	return 0;
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


static inline uint32 new_in_line_vertex(Vertex *vx, Vertex **vp, Line **lp, uint32 x, uint32 y, uint32 w, int d1, int d2)
{
	vx->x = x; vx->y = y;
	vx->n = 2;
	add_dir(vx, d1, w);
	add_dir(vx, d2, w);
	vx->cn = 0;
	vx->lp = lp;
	*vp = vx;
	//return vx->n;
	return vx->n;
	//printf("x = %d y = %d n = %d vp = %p\n", (*vp)->x, (*vp)->y, (*vp)->n, *vp);

}


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

/*	\brief	Create vertexes and lines arrays
	\param	con		The pointer to contour image.
	\param	vx		The pointer to Vertex array.
	\param	ln		The pointer to Line array.
	\param  w		The image width.
	\param  h		The image height.
	\retval			The number of vertex.
*/
uint32 seg_vertex(uint8 *con, uint8 *r, uint8 *g, uint8 *b, Vertex *vx, Vertex **vp, Line *ln, Line **lp, uint32 w, uint32 h)
{
	uint32 j, y, x, x1, y1, x2, y2, yx, yx1, yx2, yw, w1 = w-1, h1 = h-1, nd1, nd2, yxd;
	int vxc = 0, lnc = 0, linc = 0, lc[3], rc[3], cc;
	int d, d1, d2, dx, dy, fs, sc, cfs, csc, ll, ld, rd;


	for(y=1; y < h1; y++){
		yw = y*w;
		for(x=1; x < w1; x++){
			yx = yw + x;
			if(con[yx] == 255 || con[yx] == 254) { //New vertex
				x1 = x; y1 = y;
				if(con[yx] == 255) {
					if(new_vertex(con, &vx[yx], &vp[vxc++], &lp[lnc+=8], x1, y1, yx, w)){
						con[yx] = 254;
					} else {	// Remove  not connected vertex
						vxc--; con[yx] = 0;
					}
				}
				yx1 = yx;

				while(get_next_dir( &vx[yx1], &dx, &dy, &nd2)){
					//printf("d = %d dx = %d dy = %d %o %o\n", d, dx, dy, vx[yx1].di,  vx[yx1].cn);
					yx2 = yx1; d2 = dx + w*dy; x2 = x1; y2 = y1;
					// Variable for line construction.
					fs = 0; sc = 0; cfs = 0; csc = 0; ll = 0;
					//Find two perpendicular directions
					/*
					get_per_dir(dx + w*dy, w, &ld, &rd);
					yxd = yx1 + ld;
					lc[0] = r[yxd]; lc[1] = g[yxd]; lc[2] = b[yxd];
					yxd = yx1 + rd;
					rc[0] = r[yxd]; rc[1] = g[yxd]; rc[2] = b[yxd]; cc = 1;
					*/

					rc[0] = r[yx1]; rc[1] = g[yx1]; rc[2] = b[yx1]; cc = 1;
					while(1){
						x1 += dx; y1 += dy;
						d = dx + w*dy;
						yx1 = yx1 + d;
						//if(yx1 == 33973) printf("      d = %d dx = %d dy = %d %o %o n = %d yx = %d\n",
						//		d, dx, dy, vx[yx1].di,  vx[yx1].cn, vx[yx1].n, vx[yx1].y*w + vx[yx1].x);
						if(!con[yx1]){ //Remove not connected direction
							remove_dir(&vx[yx2], d2, w); vx[yx2].n--;
							yx1 = yx; x1 = x; y1 = y;
							//yx1 = yx2; x1 = x2; y1 = y2;
							break;
						}
						/*
						get_per_dir(d, w, &ld, &rd);
						yxd = yx1 + ld;
						lc[0] += r[yxd]; lc[1] += g[yxd]; lc[2] += b[yxd];
						yxd = yx1 + rd;
						rc[0] += r[yxd]; rc[1] += g[yxd]; rc[2] += b[yxd]; cc++;
						*/
						rc[0] += r[yx1]; rc[1] += g[yx1]; rc[2] += b[yx1]; cc++;
						//print_around(con, yx1, w);
						//printf("y = %d x = %d d = %d w = %d\n", (yx1-d)/w, (yx1-d)%w, d, w);
						if(con[yx1] == 255 || con[yx1] == 254) {
							if(con[yx1] == 255)  {
								new_vertex(con, &vx[yx1], &vp[vxc++], &lp[lnc+=8], x1, y1, yx1, w);
								nd1 = add_finish_dir(&vx[yx1], -d, w); vx[yx1].n++;
								con[yx1] = 254;
							} else {
								nd1 = add_finish_dir(&vx[yx1], -d, w); vx[yx1].n++;
							}
							//lc[0] = lc[0]/cc; lc[1] = lc[1]/cc; lc[2] = lc[2]/cc;
							rc[0] = rc[0]/cc; rc[1] = rc[1]/cc; rc[2] = rc[2]/cc;
							//rc = rc/cc; gc = gc/cc; bc = bc/cc;
							new_line(&ln[linc++], &vx[yx2], &vx[yx1], nd2, nd1, rc, rc);
							//new_line1(&vx[yx1].ln[nd1], &vx[yx2], rc, lc);
							//cl = 0; cc = 0;
							//linc++;
							yx1 = yx; x1 = x; y1 = y;
							break;
						}
						//if(is_new_line2(d, &cn, &fs, &sc)){
						if(is_new_line3(d, &fs, &sc, &cfs, &csc, &ll)){
							yx1 -= d; x1 -= dx; y1 -= dy;
							new_in_line_vertex(&vx[yx1], &vp[vxc++], &lp[lnc+=8], x1, y1, w, d, -d1);
							nd1 = finish_dir(&vx[yx1], -d1, w);

							//if(yx1 == 33973) printf("new line d = %d dx = %d dy = %d %o %o n = %d yx = %d\n",
							//		d, dx, dy, vx[yx1].di,  vx[yx1].cn, vx[yx1].n, vx[yx1].y*w + vx[yx1].x);
							//new_vertex(con, &vx[yx1], &vp[vxc++], &ln[lnc+=8], x1, y1, yx1, w);
							//nd1 = add_finish_dir(&vx[yx1], -d1, w);
							//lc[0] = lc[0]/cc; lc[1] = lc[1]/cc; lc[2] = lc[2]/cc;
							rc[0] = rc[0]/cc; rc[1] = rc[1]/cc; rc[2] = rc[2]/cc;
							//rc = rc/cc; gc = gc/cc; bc = bc/cc;
							new_line(&ln[linc++], &vx[yx2], &vx[yx1], nd2, nd1, rc, rc);
							//new_line1(&vx[yx1].ln[nd1], &vx[yx2], rc, lc);
							//linc++;
							con[yx1] = 254;
							break;
						}

						d1 = d;
						dx = -dx; dy = -dy;
						direction(con, w, yx1, &dx, &dy);
						con[yx1] = 0;
					}
				}
			}
		}
	}
	printf("Numbers of vertexs  = %d\n", vxc);
	printf("Numbers of lines    = %d\n", linc);

	for(j=0; j < vxc; j++) {
		//printf("%d %p \n", j, vp[j]);
		if(vp[j]->di != vp[j]->cn) {
			printf("%d  %o %o %o\n", j,  vp[j]->di, vp[j]->cn, vp[j]->cn^vp[j]->di);
			//printf("%d n = %d %o %o %o\n", j, vp[j]->n, vp[j]->di, vp[j]->cn, vp[j]->cn^vp[j]->di);
			con[vp[j]->y*w + vp[j]->x-w] = 255;
			con[vp[j]->y*w + vp[j]->x-1] = 255;
			con[vp[j]->y*w + vp[j]->x+w] = 255;
			con[vp[j]->y*w + vp[j]->x+1] = 255;
		}
	}
	return linc;
}

uint32 seg_select_reg(uint8 *con, uint8 *r, uint8 *g, uint8 *b, Vertex *vx, Vertex **vp, Line *ln, Line **lp, Reg *rg, uint32 w, uint32 h)
{
	uint32 i, y, x, yx, yw, w1 = w-1, h1 = h-1, nd1, nd2, yxd;
	uint32 regc;
	/*
	for(i=0; i < vxc; i++){
		yx = vp[i]->y*w + vp[i]->x;

		if(vp[i]->n > 1){

		} else {

		}
		while(get_next_dir( vp[i], &dx, &dy, &nd)){
			//if(tmp < 3) {
			v.x1 =  vp[i]->lp[nd]->vx[0]->x; v.y1 =  vp[i]->lp[nd]->vx[0]->y;
			v.x2 =  vp[i]->lp[nd]->vx[1]->x; v.y2 =  vp[i]->lp[nd]->vx[1]->y;
			draw_three_lines(r, g, b, &v, w, vp[i]->lp[nd]->l, vp[i]->lp[nd]->r);
		}
		//}
		r[yx] = 255; g[yx] = 255; b[yx] = 255;
	}
*/
}

static inline uint32 draw_line(uint8 *r, uint8 *g, uint8 *b, Vector *v, uint32 w, uint8 *lc)
{
	uint32 i, max , min = 0, n, x, y, dx, dy;
	int sty, stx, yx;

	x = v->x1; y = v->y1*w;
	stx = v->x2 > v->x1 ? 1 : -1;
	sty = v->y2 > v->y1 ? w : -w;

	dx = abs(v->x2 - v->x1)+1; dy = abs(v->y2 - v->y1)+1;
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

static inline uint32 draw_three_lines(uint8 *r, uint8 *g, uint8 *b, Vector *v, uint32 w, uint8 *lc, uint8 *rc)
{
	uint32 i, max , min = 0, n, x, y, dx, dy;
	int sty, stx, yx, yxs, lfx, lfy;

	x = v->x1; y = v->y1*w;
	stx = v->x2 > v->x1 ? 1 : -1;
	sty = v->y2 > v->y1 ? w : -w;
	lfx = v->x2 > v->x1 ? w : -w;
	lfy = v->y2 > v->y1 ? 1 : -1;

	dx = abs(v->x2 - v->x1)+1; dy = abs(v->y2 - v->y1)+1;
	if(dx >= dy){
		n = dx - 0; max = dx;
		for(i=0; i < n; i++){
			yx = y + x;
			yxs = yx - lfx;
			if(!r[yxs])  r[yxs] = lc[0]; g[yxs] = lc[1]; b[yxs] = lc[2];
			r[yx] = (lc[0] + rc[0])>>1;
			g[yx] = (lc[1] + rc[1])>>1;
			b[yx] = (lc[2] + rc[2])>>1;
			yxs = yx + lfx;
			if(!r[yxs])  r[yxs] = rc[0]; g[yxs] = rc[1]; b[yxs] = rc[2];
			min += dy; x += stx;
			if(min >= max) { max += dx; y += sty; }
		}
		return dx;
	} else {
		n = dy - 0; max = dy;
		for(i=0; i < n; i++){
			yx = y + x;
			yxs = yx + lfy;
			if(!r[yxs])  r[yxs] = lc[0]; g[yxs] = lc[1]; b[yxs] = lc[2];
			r[yx] = (lc[0] + rc[0])>>1;
			g[yx] = (lc[1] + rc[1])>>1;
			b[yx] = (lc[2] + rc[2])>>1;
			yxs = yx - lfy;
			if(!r[yxs])  r[yxs] = rc[0]; g[yxs] = rc[1]; b[yxs] = rc[2];
			min += dx; y += sty;
			if(min >= max) { max += dy; x += stx; }
		}
		return dy;
	}
}

void seg_vertex_draw(uint8 *r, uint8 *g, uint8 *b, Vertex **vp, Line *ln, uint32 vxc, uint32 w)
{
	uint32 i, yx, nd;//, tmp;
	int dx , dy;
	Vector v;

	for(i=0; i < vxc; i++) vp[i]->cn = 0;
	//printf("Number of vertex = %d", vxc);
	for(i=0; i < vxc; i++){
		yx = vp[i]->y*w + vp[i]->x;
		while(get_next_dir( vp[i], &dx, &dy, &nd)){
			//if(tmp < 3) {
			v.x1 =  vp[i]->lp[nd]->vx[0]->x; v.y1 =  vp[i]->lp[nd]->vx[0]->y;
			v.x2 =  vp[i]->lp[nd]->vx[1]->x; v.y2 =  vp[i]->lp[nd]->vx[1]->y;
			draw_three_lines(r, g, b, &v, w, vp[i]->lp[nd]->l, vp[i]->lp[nd]->r);
		}
		//}
		r[yx] = 255; g[yx] = 255; b[yx] = 255;
		//r[yx] = vp[i]->ln[0].l[0]; g[yx] = vp[i]->ln[0].l[1]; b[yx] = vp[i]->ln[0].l[2];
		//r[yx] = (vp[i]->lp[nd]->l[0] + vp[i]->lp[nd]->r[0])>>1;
		//g[yx] = (vp[i]->lp[nd]->l[1] + vp[i]->lp[nd]->r[1])>>1;
		//b[yx] = (vp[i]->lp[nd]->l[2] + vp[i]->lp[nd]->r[2])>>1;
	}
}

void seg_draw_line(uint8 *r, uint8 *g, uint8 *b, Line *ln, uint32 lc, uint32 w)
{
	uint32 i, yx, nd;
	int dx , dy;
	Vector v;

	for(i=0; i < lc; i++){
		v.x1 = ln[i].vx[0]->x; v.y1 = ln[i].vx[0]->y;
		v.x2 = ln[i].vx[1]->x; v.y2 = ln[i].vx[1]->y;
		//printf("%d x1 = %d y1 = %d x2 = %d y2 = %d\n", i, v.x1, v.y1, v.x2, v.y2);
		//draw_line1(r, g, b, &v, w, vp[i]->ln[nd].l[0], vp[i]->ln[nd].l[1], vp[i]->ln[nd].l[2]);
		//if(abs(v.x1-v.x2) > 2 || abs(v.y1-v.y2) > 2){
			//if(ln[i].vx[0]->n > 1 || ln[i].vx[1]->n > 1) {
				//draw_three_lines(r, g, b, &v, w, ln[i].l, ln[i].r);

				draw_line(r, g, b, &v, w, ln[i].l);
				/*
				yx = ln[i].vx[0]->y*w + ln[i].vx[0]->x;
				r[yx] = 255; g[yx] = 255; b[yx] = 255;
				yx = ln[i].vx[1]->y*w + ln[i].vx[1]->x;
				r[yx] = 255; g[yx] = 255; b[yx] = 255;
				*/
			//}
		//}
		//draw_line(r, g, b, &v, w, ln[i].l);


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
	uint8 c[3];
	c[0] = 255; c[1] = 255; c[2] = 255;
	v.x1 = 0; v.y1 = 0; v.x2 = w-1; v.y2 = 0;
	draw_line(r, g, b, &v, w, c);
	v.x1 = w-1; v.y1 = 0; v.x2 = w-1; v.y2 = h-1;
	draw_line(r, g, b, &v, w, c);
	v.x1 = w-1; v.y1 = h-1; v.x2 = 0; v.y2 = h-1;
	draw_line(r, g, b, &v, w, c);
	v.x1 = 0; v.y1 = 0; v.x2 = 0; v.y2 = h-1;
	draw_line(r, g, b, &v, w, c);
	/*
	v.x1 = 1; v.y1 = 1; v.x2 = w-2; v.y2 = 1;
	draw_line(r, g, b, &v, w, c);
	v.x1 = w-2; v.y1 = 1; v.x2 = w-2; v.y2 = h-2;
	draw_line(r, g, b, &v, w, c);
	v.x1 = w-2; v.y1 = h-2; v.x2 = 1; v.y2 = h-2;
	draw_line(r, g, b, &v, w, c);
	v.x1 = 1; v.y1 = 1; v.x2 = 1; v.y2 = h-2;
	draw_line(r, g, b, &v, w, c);
	*/
	//Draw rectangle

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

	cl[0] = 255; cl[1] = 255; cl[2] = 255;
	v.x1 = 0; v.y1 = 0; v.x2 = w-1; v.y2 = 0;
	draw_line(r1, g1, b1, &v, w, cl);
	v.x1 = w-1; v.y1 = 0; v.x2 = w-1; v.y2 = h-1;
	draw_line(r1, g1, b1, &v, w, cl);
	v.x1 = w-1; v.y1 = h-1; v.x2 = 0; v.y2 = h-1;
	draw_line(r1, g1, b1, &v, w, cl);
	v.x1 = 0; v.y1 = 0; v.x2 = 0; v.y2 = h-1;
	draw_line(r1, g1, b1, &v, w, cl);
	//Draw rectangle

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

static inline uint32 reg_number(uint8 *img, uint32 *line1, uint32 *line2, uint32 *num, uint32 yx, uint32 x, uint32 w)
{
	if(img[yx-w]) return line1[x  ];
	if(img[yx-1]) return line2[x-1];
	if(!img[yx-w+1] && !img[yx+1]) return line1[x+1];
	return (*num)++;
}


uint32  seg_get_color1(uint8 *r, uint8 *g, uint8 *b, uint8 *r1, uint8 *g1, uint8 *b1, uint32 *col, uint32 *buf, uint32 w, uint32 h)
{
	uint32 y, x, yx, yx1, yw, fst = 0, yxf, rgc = 0, cn, rc3, w1 = w-1, h1 = h-1;
	uint32 *l[2], ind, in3;
	int d;
	l[0] = buf; l[1] = &buf[w];

	for(y=1; y < h1; y++){
		yw = y*w;
		for(x=1; x < w1; x++){
			yx = yw + x;
			ind = reg_number(r1, l[0], l[1], &rgc, yx, x, w);
			in3 = ind<<2;
			col[in3] += r[yx]; col[in3+1] += g[yx]; col[in3+2] += b[yx]; col[3]++;
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
	uint32 c[3];
	uint8 cl[3];
	printf("w = %d h = %d\n", w, h);
	cl[0] = 255; cl[1] = 255; cl[2] = 255;
	v.x1 = 0; v.y1 = 0; v.x2 = w-1; v.y2 = 0;
	draw_line(r1, g1, b1, &v, w, cl);
	v.x1 = w-1; v.y1 = 0; v.x2 = w-1; v.y2 = h-1;
	draw_line(r1, g1, b1, &v, w, cl);
	v.x1 = w-1; v.y1 = h-1; v.x2 = 0; v.y2 = h-1;
	draw_line(r1, g1, b1, &v, w, cl);
	v.x1 = 0; v.y1 = 0; v.x2 = 0; v.y2 = h-1;
	draw_line(r1, g1, b1, &v, w, cl);
	//Draw rectangle

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

static inline uint32 get_dir(uint8 *img, uint32 yx, uint32 w, uint8 dir, int *dm)
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
				n = get_dir(img, yx, w, pix[yx].dir, dm);
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

static inline void find_local_min(uint8 *mb, uint16 *min, uint16 *xm, uint16 *ym, uint16 wm)
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

void seg_fall_forest(uint8 *img, uint8 *img1, uint32 w, uint32 h)
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
 			}
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

