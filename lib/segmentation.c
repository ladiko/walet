#include <walet.h>
#include <stdio.h>

static inline uint32 check1(imgtype *img, Region *reg, uint32 yx, uint32 w, uint32 theresh, uint32 *diff)
{
	uint32  c0 = abs((reg->c[0]-reg->c[1]) - (img[yx]-img[yx+1])),
			c1 = abs((reg->c[0]-reg->c[2]) - (img[yx]-img[yx+w])),
			c2 = abs((reg->c[0]-reg->c[3]) - (img[yx]-img[yx+w+1]));
	*diff = c0 + c1 + c2;
	return  c0 < theresh && c1 < theresh && c2 < theresh ;
}

static inline uint32 check(imgtype *img, Region *reg, uint32 yx, uint32 w, uint32 theresh, uint32 *diff)
{
	uint32  c0 = abs(reg->c[0]	- img[yx]    ),
			c1 = abs(reg->c[1] 	- img[yx+1]  ),
			c2 = abs(reg->c[2]	- img[yx+w]  ),
			c3 = abs(reg->c[3]	- img[yx+w+1]);
	*diff = c0 + c1 + c2 + c3;
	return  c0 < theresh && c1 < theresh && c2 < theresh && c3 < theresh;
}

static inline uint32 check_left(imgtype *img, uint32 yx, uint32 w, uint32 theresh, uint32 *diff)
{
	uint32  c0 = abs(img[yx-2] 		- img[yx]    ),
			c1 = abs(img[yx-1]		- img[yx+1]  ),
			c2 = abs(img[yx+w-2] 	- img[yx+w]  ),
			c3 = abs(img[yx+w-1]	- img[yx+w+1]);
	*diff = c0 + c1 + c2 + c3;
	return  c0 < theresh && c1 < theresh && c2 < theresh && c3 < theresh;
}

static inline uint32 check_top(imgtype *img, uint32 yx, uint32 w, uint32 theresh, uint32 *diff)
{
	uint32 w1 = w<<1;
	uint32  c0 = abs(img[yx-w1] 	- img[yx]    ),
			c1 = abs(img[yx+1-w1]	- img[yx+1]  ),
			c2 = abs(img[yx+w-w1] 	- img[yx+w]  ),
			c3 = abs(img[yx+w+1-w1]	- img[yx+w+1]);
	*diff = c0 + c1 + c2 + c3;
	return  c0 < theresh && c1 < theresh && c2 < theresh && c3 < theresh;
}

static inline new_row(Row *row, Region *reg, uint32 yx)
{
	row->yx = yx;  row->length = 0; row->reg = reg; reg->nrows++; //reg->row[reg->nrows] = row; reg->nrows++;
	//row->ac[0] = 0; row->ac[1] = 0; row->ac[2] = 0; row->ac[3] = 0;
}

static inline new_region(Region *reg, Row *row, imgtype *img, uint32 yx, uint32 w)
{
	reg->c[0] = img[yx]; reg->c[1] = img[yx+1]; reg->c[2] = img[yx+w]; reg->c[3] = img[yx+w+1];
	reg->ac[0] = 0; reg->ac[1] = 0; reg->ac[2] = 0; reg->ac[3] = 0;
	reg->nrows = 0; reg->rowc = 0; reg->npixs = 0; reg->neic = 0; reg->nneis = 0; reg->obj = NULL;	//reg->row[reg->nrows] = row;
}

static inline add_pixel(Region *reg, Row *row, imgtype *img, uint32 yx, uint32 w)
{
	row->length += 2;
	reg->ac[0] += img[yx]; reg->ac[1] += img[yx+1]; reg->ac[2] += img[yx+w]; reg->ac[3] += img[yx+w+1];
	reg->npixs++;
}

static inline left_neighborhood(Row *row, Row **prow, Region **preg, uint32 *pregc, uint32 x)
{
	preg[(*pregc)++] = prow[x-2]->reg;  prow[x-2]->reg->neic++;
	preg[(*pregc)++] = row->reg;  row->reg->neic++;

}

static inline top_neighborhood(Row *row, Row **prow, Region **preg, uint32 *pregc, uint32 x)
{
	if(row->reg != prow[x]->reg) {
		preg[(*pregc)++] = prow[x]->reg;  prow[x]->reg->neic++;
		preg[(*pregc)++] = row->reg;  row->reg->neic++;
	}
}

static inline new_corner(imgtype *img, Corner *cor, Region *reg, uint32 yx, uint32 w)
{

	cor->yx = yx; cor->reg = reg;
	cor->c[0] = img[yx]; cor->c[1] = img[yx+1]; cor->c[2] = img[yx+w]; cor->c[3] = img[yx+w+1];
	//printf("yx = %d c0 = %d, c1 = %d c2 = %d c3 = %d\n", cor->yx, cor->c[0], cor->c[1], cor->c[2], cor->c[3]);
}

static inline corner_detect(imgtype *img, Corner *cor, uint32 *corc, Region *reg0, Region *reg1, Region *reg2, Region *reg3,
		uint32 yx, uint32 w, uint32 thresh)
{
	//  sdif[4]			 dif[6]  0
	//|-----|-----|		|-----------|
	//|  0  |  1  |		|			|
	//|-----|-----|   3 |  			| 1
	//|  2  |  3  |		|			|
	//|-----|-----|		|-----------|
	//						2

	uint32 dif[6], sdif[4], w2 = w<<1, max = 0, in = 4, x;
	Region *reg;

	dif[0] 	= abs(img[yx-2-w2] 	- img[yx-w2])
			+ abs(img[yx-1-w2] 	- img[yx+1-w2])
			+ abs(img[yx-2-w] 	- img[yx-w])
			+ abs(img[yx-1-w] 	- img[yx-w+1]);

	dif[1] 	= abs(img[yx] 		- img[yx-w2])
			+ abs(img[yx+1] 	- img[yx+1-w2])
			+ abs(img[yx+w] 	- img[yx-w])
			+ abs(img[yx+w+1] 	- img[yx-w+1]);

	dif[2]	= abs(img[yx] 		- img[yx-2])
			+ abs(img[yx+1] 	- img[yx-1])
			+ abs(img[yx+w] 	- img[yx+w-2])
			+ abs(img[yx+w+1] 	- img[yx+w-1]);

	dif[3] 	= abs(img[yx-2-w2] 	- img[yx-2])
			+ abs(img[yx-1-w2] 	- img[yx-1])
			+ abs(img[yx-2-w] 	- img[yx+w-2])
			+ abs(img[yx-1-w]  	- img[yx+w-1]);

	dif[4] 	= abs(img[yx-2-w2] 	- img[yx])
			+ abs(img[yx-1-w2] 	- img[yx+1])
			+ abs(img[yx-2-w] 	- img[yx+w])
			+ abs(img[yx-1-w]  	- img[yx+w+1]);

	dif[5] 	= abs(img[yx-w2]	- img[yx-2])
			+ abs(img[yx+1-w2]	- img[yx-1])
			+ abs(img[yx-w] 	- img[yx+w-2])
			+ abs(img[yx-w+1]  	- img[yx+w-1]);

	sdif[0] = dif[0] + dif[3] + dif[4];
	sdif[1] = dif[0] + dif[1] + dif[5];
	sdif[2] = dif[2] + dif[3] + dif[5];
	sdif[3] = dif[1] + dif[2] + dif[4];

	//printf("sdif0 = %d sdif1 = %d sdif2 = %d sdif3 = %d &cor[*corc] = %p \n",sdif[0], sdif[1], sdif[2], sdif[3], &cor[*corc]);
	//if(sdif[0] > thresh && reg0 != reg1 && reg0 != reg2) new_corner(img, &cor[(*corc)++], reg0, yx-w2-2, w);
	//if(sdif[1] > thresh && reg1 != reg0 && reg1 != reg3) new_corner(img, &cor[(*corc)++], reg1, yx-w2  , w);
	//if(sdif[2] > thresh && reg2 != reg0 && reg2 != reg3) new_corner(img, &cor[(*corc)++], reg2, yx     , w);
	//if(sdif[3] > thresh && reg3 != reg1 && reg3 != reg2) new_corner(img, &cor[(*corc)++], reg3, yx-2   , w);
	if(sdif[0] > thresh && reg0 != reg1 && reg0 != reg2) { max = sdif[0]; in = 0;}
	if(sdif[1] > thresh && reg1 != reg0 && reg1 != reg3) if(sdif[1] > max) { max = sdif[1]; in = 1;}
	if(sdif[2] > thresh && reg2 != reg0 && reg2 != reg3) if(sdif[2] > max) { max = sdif[2]; in = 2;}
	if(sdif[3] > thresh && reg3 != reg1 && reg3 != reg2) if(sdif[3] > max) { in = 3;}

	//printf("ind = %d ", in);
	if(in == 0) { reg = reg0; x = yx-w2-2; }
	if(in == 1) { reg = reg1; x = yx-w2  ; }
	if(in == 2) { reg = reg2; x = yx-2   ; }
	if(in == 3) { reg = reg3; x = yx     ; }

	if(in != 4) new_corner(img, &cor[(*corc)++], reg, x, w);

	//if(in == 0) new_corner(img, &cor[(*corc)++], reg0, yx-w2-2, w);
	//if(in == 1) new_corner(img, &cor[(*corc)++], reg1, yx-w2  , w);
	//if(in == 2) new_corner(img, &cor[(*corc)++], reg2, yx-2   , w);
	//if(in == 3) new_corner(img, &cor[(*corc)++], reg3, yx     , w);

	/*
	switch(in) {
		case 0 : { new_corner(img, &cor[(*corc)++], reg0, yx-w2-2, w); break;}
		case 1 : { new_corner(img, &cor[(*corc)++], reg1, yx-w2  , w); break;}
		case 2 : { new_corner(img, &cor[(*corc)++], reg2, yx-2   , w); break;}
		case 3 : { new_corner(img, &cor[(*corc)++], reg3, yx     , w); break;}
	}*/
}

void seg_regions(imgtype *img, Region *reg, Row *row, Corner *cor, Row **prow, Region **preg, uint32 w, uint32 h, uint32 theresh, uint32 corth,
		uint32 *nrows, uint32 *nregs, uint32 *npreg, uint32 *ncors)
{
	uint32 y, h1 = ((h>>1)<<1)*w, x, w1 = w<<1, yx;
	uint32 df1[4], df2[4], left=0, regc =0, rowc = 0, corc = 0, pregc = 0, rc, tmp = 0, dfl, dft;
	Row *prowt;
	y=0; x=0;
	yx = y+x;
	new_region(&reg[regc], &row[rowc], img, yx, w);
	new_row(&row[rowc], &reg[regc], yx);
	add_pixel(&reg[regc], &row[rowc], img, yx, w);
	prow[x] = &row[rowc];
	rowc++; regc++;
	for(x=2; x < w; x+=2){
		yx = y+x;
		//if(check_left(img, yx, w, theresh, &dfl))
		if(check(img, prow[x-2]->reg, yx, w, theresh, &dfl))
		{
			add_pixel(prow[x-2]->reg, prow[x-2], img, yx, w);
			prow[x] = prow[x-2];
			//img[yx] = img[yx-2]; img[yx+1] = img[yx-1]; img[yx+w] = img[yx+w-2]; img[yx+w+1] = img[yx+w-1];
		} else {
			//printf("%5d %p x = %d y = %d l = %d\n", rowc, &row[rowc], row[rowc].x, row[rowc].y, row[rowc].length);
			new_region(&reg[regc], &row[rowc], img, yx, w);
			new_row(&row[rowc], &reg[regc], yx);
			add_pixel(&reg[regc], &row[rowc], img, yx, w);

			left_neighborhood(&row[rowc], prow, preg, &pregc, x);
			prow[x] = &row[rowc];
			rowc++; regc++;
		}
	}
	//printf("%5d %p yx = %d  l = %d\n", rowc, &row[rowc-1], row[rowc-1].yx, row[rowc-1].length);
	for(y=w1; y < h1; y+=w1) {
		x=0;
		yx = y+x;
		//prowt = prow[x];
		//if(check_top(img, yx, w, theresh, &dft))
		if(check(img, prow[x]->reg, yx, w, theresh, &dft))
		{
			new_row(&row[rowc], prow[x]->reg, yx);
			add_pixel(prow[x]->reg, &row[rowc], img, yx, w);
			prow[x] = &row[rowc];
			rowc++;
			//img[yx] = img[yx-w1]; img[yx+1] = img[yx+1-w1]; img[yx+w] = img[yx+w-w1]; img[yx+w+1] = img[yx+w+1-w1];
		} else {
			new_region(&reg[regc], &row[rowc], img, yx, w);
			new_row(&row[rowc], &reg[regc], yx);
			add_pixel(&reg[regc], &row[rowc], img, yx, w);

			top_neighborhood(&row[rowc], prow, preg, &pregc, x);
			prow[x] = &row[rowc];
			rowc++; regc++;
		}
		for(x=2; x < w; x+=2){
			yx = y+x;
			//left =  check_left(img, yx, w, theresh, &dfl);
			//left += check_top(img, yx, w, theresh, &dft)<<1;
			left =  check(img, prow[x-2]->reg, yx, w, theresh, &dfl);
			left += check(img, prow[x]->reg, yx, w, theresh, &dft)<<1;
			switch(left){
				case 0 : {
					new_region(&reg[regc], &row[rowc], img, yx, w);
					new_row(&row[rowc], &reg[regc], yx);
					add_pixel(&reg[regc], &row[rowc], img, yx, w);
					corner_detect(img, cor, &corc, prowt->reg, prow[x]->reg, prow[x-2]->reg, &reg[regc], yx, w, corth);

					left_neighborhood(&row[rowc], prow, preg, &pregc, x);
					top_neighborhood(&row[rowc], prow, preg, &pregc, x);
					prowt = prow[x];
					prow[x] = &row[rowc];
					rowc++; regc++;
					break;
				}
				case 1 : {
					add_pixel(prow[x-2]->reg, prow[x-2], img, yx, w);

					top_neighborhood(&row[rowc-1], prow, preg, &pregc, x);
					prow[x] = prow[x-2];
					//img[yx] = img[yx-2]; img[yx+1] = img[yx-1]; img[yx+w] = img[yx+w-2]; img[yx+w+1] = img[yx+w-1];
					break;
				}
				case 2 : {
					new_row(&row[rowc], prow[x]->reg, yx);
					add_pixel(prow[x]->reg, &row[rowc], img, yx, w);
					corner_detect(img, cor, &corc, prowt->reg, prow[x]->reg, prow[x-2]->reg, prow[x]->reg, yx, w, corth);

					left_neighborhood(&row[rowc], prow, preg, &pregc, x);
					top_neighborhood(&row[rowc], prow, preg, &pregc, x);
					prowt = prow[x];
					prow[x] = &row[rowc];
					rowc++;
					//img[yx] = img[yx-w1]; img[yx+1] = img[yx+1-w1]; img[yx+w] = img[yx+w-w1]; img[yx+w+1] = img[yx+w+1-w1];
					break;
				}
				case 3 : {
					if(dfl < dft){
						add_pixel(prow[x-2]->reg, prow[x-2], img, yx, w);

						top_neighborhood(&row[rowc-1], prow, preg, &pregc, x);
						prow[x] = prow[x-2];
						//img[yx] = img[yx-2]; img[yx+1] = img[yx-1]; img[yx+w] = img[yx+w-2]; img[yx+w+1] = img[yx+w-1];
					} else {
						if(prow[x-2]->reg != prow[x]->reg){
							new_row(&row[rowc], prow[x]->reg, yx);
							add_pixel(prow[x]->reg, &row[rowc], img, yx, w);
							corner_detect(img, cor, &corc, prowt->reg, prow[x]->reg, prow[x-2]->reg, prow[x]->reg, yx, w, corth);

							left_neighborhood(&row[rowc], prow, preg, &pregc, x);
							top_neighborhood(&row[rowc], prow, preg, &pregc, x);
							prowt = prow[x];
							prow[x] = &row[rowc];
							rowc++;
						} else {
							add_pixel(prow[x-2]->reg, prow[x-2], img, yx, w);

							top_neighborhood(&row[rowc-1], prow, preg, &pregc, x);
							prow[x] = prow[x-2];
						}
						//img[yx] = img[yx-w1]; img[yx+1] = img[yx+1-w1]; img[yx+w] = img[yx+w-w1]; img[yx+w+1] = img[yx+w+1-w1];
					}
					break;
				}
			}
		}
	}
	*nrows = rowc;
	*nregs = regc;
	*npreg = pregc;
	*ncors = corc;
	printf("rowc = %d regc = %d corc = %d pregc = %d\n", rowc, regc, corc, pregc);

	for(rc=0; rc < *nrows; rc++) {
		if(row[rc].reg->rowc){
			row[rc].reg->row[row[rc].reg->rowc] = &row[rc];
			row[rc].reg->rowc++;
		} else {
			row[rc].reg->row = &prow[tmp];
			tmp+=row[rc].reg->nrows;
			row[rc].reg->row[row[rc].reg->rowc] = &row[rc];
			row[rc].reg->rowc++;
			row[rc].reg->c[0] = row[rc].reg->ac[0] / row[rc].reg->npixs;
			row[rc].reg->c[1] = row[rc].reg->ac[1] / row[rc].reg->npixs;
			row[rc].reg->c[2] = row[rc].reg->ac[2] / row[rc].reg->npixs;
			row[rc].reg->c[3] = row[rc].reg->ac[3] / row[rc].reg->npixs;
			//printf(reg[rc].row[y])
		}
	}
}


static inline void region_draw(imgtype *img, Region *reg, uchar *c, uint32 w)
{
	uint32 rowc, yx;
	for(rowc=0; rowc < reg->nrows; rowc++){
		for(yx=reg->row[rowc]->yx; yx < (reg->row[rowc]->yx + reg->row[rowc]->length); yx+=2){
			img[yx] 	= c[0];
			img[yx+1] 	= c[1];
			img[yx+w] 	= c[2];
			img[yx+w+1] = c[3];
		}
	}
}

void seg_regions_draw(imgtype *img, Region *reg, uint32 nregs, uint32 w)
{
	uint32 rowc, x, w1 = w<<1, yx, rc;
	for(rc=0; rc < nregs; rc++) region_draw(img, &reg[rc], reg[rc].c, w);
}

void seg_corners_draw(imgtype *img, Corner *cor, uint32 ncors, uint32 w)
{
	uint32 i, w2 = w<<1;
	for(i=0; i < ncors; i++) {
		//img[cor[i].yx] 		= cor[i].c[0];
		//img[cor[i].yx+1] 	= cor[i].c[1];
		//img[cor[i].yx+w] 	= cor[i].c[2];
		//img[cor[i].yx+w+1] 	= cor[i].c[3];
		img[cor[i].yx] 		= 255;
		img[cor[i].yx+1] 	= 255;
		img[cor[i].yx+w] 	= 255;
		img[cor[i].yx+w+1] 	= 255;

	}
}

static inline uint32 check_reg(Region *reg1, Region *reg2)
{
	int j;
	for(j= reg1->nneis; j; j--) {
		if(reg1->reg[j] == reg2) return 0;
	}
	return 1;
}

void seg_regions_neighbor(Region *reg, Region **pnei, Region **preg, uint32 nregs, uint32 npregs)
{
	uint32 i, j, tmp = 0;
	for(i=0; i < nregs; i++){
		reg[i].reg = &pnei[tmp];
		tmp += reg[i].neic;
		//reg[i].neic = 0;
		//reg[i].obj = NULL;
	}
	for(i=0; i < npregs; i+=2){
		if(check_reg(preg[i], preg[i+1])){
			preg[i]->reg[preg[i]->nneis] = preg[i+1];
			preg[i]->nneis++;
		}
		if(check_reg(preg[i+1], preg[i])){
			preg[i+1]->reg[preg[i+1]->nneis] = preg[i];
			preg[i+1]->nneis++;
		}
	}
	tmp=0;
	for(i=0; i< nregs; i++) tmp += reg[i].nneis;
	printf("neighbor = %d\n", tmp);
}

static inline new_obj(Object *obj, Region *reg)
{
	//obj->c[0] = reg->c[0];
	//obj->c[1] = reg->c[1];
	//obj->c[2] = reg->c[2];
	//obj->c[3] = reg->c[3];
	obj->ac[0] = reg->c[0]*reg->npixs;
	obj->ac[1] = reg->c[1]*reg->npixs;
	obj->ac[2] = reg->c[2]*reg->npixs;
	obj->ac[3] = reg->c[3]*reg->npixs;
	obj->npixs = reg->npixs;
	reg->obj = obj;
	obj->nregs = 1;
	obj->regc = 0;
}

static inline add_reg(Object *obj, Region *reg)
{
	//obj->c[0] = (obj->c[0]*obj->npixs + reg->c[0]*reg->npixs)/(obj->npixs + reg->npixs);
	//obj->c[1] = (obj->c[1]*obj->npixs + reg->c[1]*reg->npixs)/(obj->npixs + reg->npixs);
	//obj->c[2] = (obj->c[2]*obj->npixs + reg->c[2]*reg->npixs)/(obj->npixs + reg->npixs);
	//obj->c[3] = (obj->c[3]*obj->npixs + reg->c[3]*reg->npixs)/(obj->npixs + reg->npixs);
	obj->ac[0] += reg->c[0]*reg->npixs;
	obj->ac[1] += reg->c[1]*reg->npixs;
	obj->ac[2] += reg->c[2]*reg->npixs;
	obj->ac[3] += reg->c[3]*reg->npixs;
	//reg->c[0] = obj->c[0];
	//reg->c[1] = obj->c[1];
	//reg->c[2] = obj->c[2];
	//reg->c[3] = obj->c[3];
	obj->npixs += reg->npixs;
	reg->obj = obj;
	obj->nregs++;
}

static inline uint32 check_neighbor(Region *reg1, Region *reg2, uint32 theresh)
{
	return	abs(reg1->c[0]	- reg2->c[0]) < theresh ||
			abs(reg1->c[1]	- reg2->c[1]) < theresh ||
			abs(reg1->c[2]	- reg2->c[2]) < theresh ||
			abs(reg1->c[3]	- reg2->c[3]) < theresh;
}

static inline uint32 check_neighbor1(Region *reg1, Region *reg2, uint32 theresh, uint32 col)
{
	return	(abs(reg1->c[0]	- reg2->c[0]) < theresh ||
			 abs(reg1->c[1]	- reg2->c[1]) < theresh ||
			 abs(reg1->c[2]	- reg2->c[2]) < theresh ||
			 abs(reg1->c[3]	- reg2->c[3]) < theresh) &&
			(abs((reg1->c[0]-reg1->c[1])-(reg2->c[0]-reg2->c[1])) < col ||
			 abs((reg1->c[0]-reg1->c[2])-(reg2->c[0]-reg2->c[2])) < col ||
			 abs((reg1->c[0]-reg1->c[3])-(reg2->c[0]-reg2->c[3])) < col);
}

static inline uint32 check_neighbor2(Region *reg1, Region *reg2, uint32 theresh, uint32 *diff)
{
	uint32  c0 = abs(reg1->c[0]	- reg2->c[0]),
			c1 = abs(reg1->c[1] - reg2->c[1]),
			c2 = abs(reg1->c[2]	- reg2->c[2]),
			c3 = abs(reg1->c[3]	- reg2->c[3]);
	*diff = c0 + c1 + c2 + c3;
	return  c0 < theresh && c1 < theresh && c2 < theresh && c3 < theresh;
}

static inline uint32 check_neighbor3(Region *reg1, Region *reg2, uint32 theresh, uint32 *diff)
{
	uint32  c0 = abs((reg1->c[0] - reg1->c[1]) - (reg2->c[0] - reg2->c[1])),
			c1 = abs((reg1->c[0] - reg1->c[2]) - (reg2->c[0] - reg2->c[2])),
			c2 = abs((reg1->c[0] - reg1->c[3]) - (reg2->c[0] - reg2->c[3]));
	*diff = c0 + c1 + c2;
	return  c0 < theresh && c1 < theresh && c2 < theresh;
}

void seg_objects(Object *obj, Region *reg,  Region **preg, uint32 nregs, uint32 *nobjs, uint32 theresh)
{
	uint32 j=0, i, oc=0, tmp = 0, ck, in, diff, min;
	/*
	new_obj(&obj[oc], &reg[j]); oc++;
	for(j=1; j < nregs; j++){
		ck = 0;
		for(i=0; i < reg[j].nneis; i++){
			//if(reg[j].reg[i]->obj != NULL && check_neighbor(&reg[j], reg[j].reg[i], theresh)){
			if(reg[j].reg[i]->obj != NULL && check_neighbor1(&reg[j], reg[j].reg[i], theresh, 2)){
				add_reg(reg[j].reg[i]->obj, &reg[j]); ck++;
				break;
			}
		}
		if(!ck)  { new_obj(&obj[oc], &reg[j]); oc++;}
	}*/

	for(j=0; j < nregs; j++){
		ck = 0;
		min = 0;
		if(reg[j].obj == NULL){
			for(i=0; i < reg[j].nneis; i++){
				if(check_neighbor3(&reg[j], reg[j].reg[i], theresh, &diff)) {
					if(ck){
						if(diff < min) { min = diff; in = i;}
					} else {
						in = i; min = diff;
					}
					ck++;
				}
			}
			if(!ck)  { new_obj(&obj[oc], &reg[j]); oc++;}
			else{
				if(reg[j].reg[in]->obj == NULL){
					new_obj(&obj[oc], &reg[j]);
					add_reg(&obj[oc], reg[j].reg[in]); oc++;
				} else {
					add_reg(reg[j].reg[in]->obj, &reg[j]);
				}
			}
		}
	}
	*nobjs = oc;
	printf("Objects = %d\n", oc);
	for(j=0; j < nregs; j++){
		if(reg[j].obj->regc){
			reg[j].obj->reg[reg[j].obj->regc] = &reg[j];
			reg[j].obj->regc++;

		} else {
			reg[j].obj->reg = &preg[tmp];
			tmp += reg[j].obj->nregs;
			reg[j].obj->reg[reg[j].obj->regc] = &reg[j];
			reg[j].obj->regc++;
			reg[j].obj->c[0] = reg[j].obj->ac[0] / reg[j].obj->npixs;
			reg[j].obj->c[1] = reg[j].obj->ac[1] / reg[j].obj->npixs;
			reg[j].obj->c[2] = reg[j].obj->ac[2] / reg[j].obj->npixs;
			reg[j].obj->c[3] = reg[j].obj->ac[3] / reg[j].obj->npixs;
		}
	}
}

void seg_objects_draw(imgtype *img, Object *obj, uint32 nobjs, uint32 w)
{
	uint32 i, j;//, tmp=0, tmp1=0;
	//rc = 97817; { //97848; {
	//i = 100; {
	//for(i=0; i < 100; i++){
	for(i=0; i < nobjs; i++){
		//tmp+= obj[i].npixs;
		//printf("obj = %p nregs = %d\n", &obj[i], obj[i].nregs);
		//if(obj[i].nregs>30){
		for(j=0; j < obj[i].nregs; j++){
			//tmp1+= obj[i].reg[j]->npixs;
			//printf("reg = %p \n", &obj[i].reg[j]);
			region_draw(img, obj[i].reg[j], obj[i].c, w);
			//region_draw(img, obj[i].reg[j], obj[i].reg[j]->c, w);
		}
		//}
	}
	//printf("objpix = %d regpix = %d\n", tmp, tmp1);
}


void seg_color_quant(imgtype *img, imgtype *img1, uint32 w, uint32 h, uint32 quant)
{
	uint32 y, x, yx, w1 = w<<1, h1 = ((h>>1)<<1)*w, half = ((1<<quant)>>1);
	for(y=0; y < h1; y+=w1) {
		for(x=0; x < w; x+=2){
			yx = y+x;
			img1[yx]		= ((img[yx]		>>quant)<<quant) + half;
			img1[yx+1]		= ((img[yx+1]	>>quant)<<quant) + half;
			img1[yx+w]		= ((img[yx+w]	>>quant)<<quant) + half;
			img1[yx+w+1]	= ((img[yx+w+1]	>>quant)<<quant) + half;

		}
	}
}
