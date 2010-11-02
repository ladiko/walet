#include <walet.h>
#include <stdio.h>
uint32 q = 4;
/*
static inline uint32 check2(imgtype *img, Region *reg, uint32 yx, uint32 w, uint32 theresh, uint32 *diff)
{
	uint32 	c0 = ((img[yx]>>q)<<q) - reg->c[0],
			c1 = ((img[yx+1]>>q)<<q) - reg->c[1],
			c2 = ((img[yx+w]>>q)<<q) - reg->c[2],
			c3 = ((img[yx+w+1]>>q)<<q) - reg->c[3];
	*diff = c0 + c1 + c2 + c3;
	return  !c0 && !c1  && !c2  && !c3;
}

static inline uint32 check1(imgtype *img, Region *reg, uint32 yx, uint32 w, uint32 theresh, uint32 *diff)
{
	uint32  c0 = abs(reg->c[0]	- img[yx]    ),
			c1 = abs(reg->c[1] 	- img[yx+1]  ),
			c2 = abs(reg->c[2]	- img[yx+w]  ),
			c3 = abs(reg->c[3]	- img[yx+w+1]);
	*diff = c0 + c1 + c2 + c3;
	return  c0 < theresh && c1 < theresh && c2 < theresh && c3 < theresh;
}

static inline uint32 check(imgtype *img, Region *reg, uint32 yx, uint32 w, uint32 theresh, uint32 *diff)
{
	uint32  c0 = abs(reg->c[0]	- img[yx]    ),
			c1 = abs(reg->c[1] 	- img[yx+1]  ),
			c2 = abs(reg->c[2]	- img[yx+w]  ),
			c3 = abs(reg->c[3]	- img[yx+w+1]);
	*diff = c0 + c1 + c2 + c3;
	return  !(c0 > theresh && c1 > theresh && c2 > theresh && c3 > theresh);
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

static inline uint32 check_quant(imgtype *img, uint32 yx, uint32 yx1, uint32 w, uint32 q, uint32 *diff)
{

	uint32  c0 = (img[yx1]>>q)		- (img[yx]>>q),
			c1 = (img[yx1+1]>>q)	- (img[yx+1]>>q),
			c2 = (img[yx1+w]>>q) 	- (img[yx+w]>>q),
			c3 = (img[yx1+w+1]>>q)	- (img[yx+w+1]>>q);
	*diff = c0 + c1 + c2 + c3;
	return  !c0 && !c1 && !c2 && !c3;
}

static inline new_row(Row *row, Region *reg, uint32 yx)
{
	row->yx = yx;  row->length = 0; row->reg = reg; reg->nrows++; //reg->row[reg->nrows] = row; reg->nrows++;
	//row->ac[0] = 0; row->ac[1] = 0; row->ac[2] = 0; row->ac[3] = 0;
}

static inline new_region(Region *reg, Row *row, imgtype *img, uint32 yx, uint32 w)
{
	reg->c[0] = img[yx]; reg->c[1] = img[yx+1]; reg->c[2] = img[yx+w]; reg->c[3] = img[yx+w+1];
	//reg->c[0] = ((img[yx]>>q)<<q);
	//reg->c[1] = ((img[yx+1]>>q)<<q);
	//reg->c[2] = ((img[yx+w]>>q)<<q);
	//reg->c[3] = ((img[yx+w+1]>>q)<<q);
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

static inline corner_detect1(imgtype *img, Corner *cor, uint32 *corc, Region *reg0, Region *reg1, Region *reg2, Region *reg3,
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

}

static inline corner_detect(imgtype *img, Corner *cor, uint32 *corc, Row **prow0, Row **prow1, Row **prow2, uint32 yx, uint32 w, uint32 thresh)
{
	uint32 c = 0;
	if(prow1[2]->reg == prow0[0]->reg) c++;
	if(prow1[2]->reg == prow0[2]->reg) c++;
	if(prow1[2]->reg == prow0[4]->reg) c++;
	if(prow1[2]->reg == prow1[0]->reg) c++;
	if(prow1[2]->reg == prow1[4]->reg) c++;
	if(prow1[2]->reg == prow2[0]->reg) c++;
	if(prow1[2]->reg == prow2[2]->reg) c++;
	if(prow1[2]->reg == prow2[4]->reg) c++;
	if(c < 1) new_corner(img, &cor[(*corc)++], prow1[2]->reg, yx, w);
}

void seg_regions(imgtype *img, Region *reg, Row *row, Corner *cor, Row **pro, Region **preg, uint32 w, uint32 h, uint32 theresh, uint32 corth,
		uint32 *nrows, uint32 *nregs, uint32 *npreg, uint32 *ncors)
{
	uint32 i=0, j=1, k=2, y, h1 = ((h>>1)<<1)*w, x, w1 = w<<1, yx;
	uint32 df1[4], df2[4], left=0, regc =0, rowc = 0, corc = 0, pregc = 0, rc, tmp = 0, dfl, dft;
	uint32 q = 5;
	Row **pr[3], **prow, **prow1;
	Row *prowt;
	pr[0] = pro; pr[1] = &pro[w]; pr[2] = &pro[w<<1];

	prow = pr[i];
	y=0; x=0;
	yx = y+x;
	new_region(&reg[regc], &row[rowc], img, yx, w);
	new_row(&row[rowc], &reg[regc], yx);
	add_pixel(&reg[regc], &row[rowc], img, yx, w);
	prow[x] = &row[rowc];
	rowc++; regc++;
	for(x=2; x < w; x+=2){
		yx = y+x;
		//if(check_quant(img, yx, yx-2, w, q, &dfl))
		if(check(img, prow[x-2]->reg, yx, w, theresh, &dfl))
		{
			add_pixel(prow[x-2]->reg, prow[x-2], img, yx, w);
			prow[x] = prow[x-2];
			//printf("%5d %p yx = %d l = %d\n", rowc-1, &row[rowc-1], row[rowc-1].yx, row[rowc-1].length);
			//img[yx] = img[yx-2]; img[yx+1] = img[yx-1]; img[yx+w] = img[yx+w-2]; img[yx+w+1] = img[yx+w-1];
		} else {
			//rowc++; regc++;
			new_region(&reg[regc], &row[rowc], img, yx, w);
			new_row(&row[rowc], &reg[regc], yx);
			add_pixel(&reg[regc], &row[rowc], img, yx, w);

			left_neighborhood(&row[rowc], prow, preg, &pregc, x);
			prow[x] = &row[rowc];
			//printf("%5d %p yx = %d l = %d\n", rowc, &row[rowc], row[rowc].yx, row[rowc].length);
			rowc++; regc++;
		}
	}
	//printf("%5d %p yx = %d  l = %d\n", rowc, &row[rowc-1], row[rowc-1].yx, row[rowc-1].length);
	for(y=w1; y < h1; y+=w1) {
		x=0;
		yx = y+x;
		i = (i == 2) ? 0 : i+1;
		j = (j == 2) ? 0 : j+1;
		k = (k == 2) ? 0 : k+1;
		prow = pr[i];
		prow1 = pr[k];
		//printf("i = %d j = %d k = %d\n", i, j, k);
		//prowt = prow[x];
		//if(check_top(img, yx, w, theresh, &dft))
		//if(check_quant(img, yx, yx-w1, w, q, &dfl))
		if(check(img, prow1[x]->reg, yx, w, theresh, &dft))
		{
			//rowc++;
			new_row(&row[rowc], prow1[x]->reg, yx);
			add_pixel(prow1[x]->reg, &row[rowc], img, yx, w);
			prow[x] = &row[rowc];
			rowc++;
			//img[yx] = img[yx-w1]; img[yx+1] = img[yx+1-w1]; img[yx+w] = img[yx+w-w1]; img[yx+w+1] = img[yx+w+1-w1];
		} else {
			//rowc++; regc++;
			new_region(&reg[regc], &row[rowc], img, yx, w);
			new_row(&row[rowc], &reg[regc], yx);
			add_pixel(&reg[regc], &row[rowc], img, yx, w);

			top_neighborhood(&row[rowc], prow1, preg, &pregc, x);
			prow[x] = &row[rowc];
			rowc++; regc++;
		}
		for(x=2; x < w; x+=2){
			yx = y+x;
			//printf("prow = %p prow1 = %p yx = %d\n", prow[x-2], prow1[x], yx);
			//left =  check_quant(img, yx, yx-2, w, q, &dfl);
			//left += check_quant(img, yx, yx-w1, w, q, &dfl)<<1;
			left =  check(img, prow[x-2]->reg, yx, w, theresh, &dfl);
			left += check(img, prow1[x]->reg, yx, w, theresh, &dft)<<1;
			//printf("%5d %p yx = %d  l = %d left = %d, dfl = %d dft = %d\n", rowc, &row[rowc-1], row[rowc-1].yx, row[rowc-1].length, left, dfl, dft);
			switch(left){
				case 0 : {
					//rowc++; regc++;
					new_region(&reg[regc], &row[rowc], img, yx, w);
					new_row(&row[rowc], &reg[regc], yx);
					add_pixel(&reg[regc], &row[rowc], img, yx, w);
					//corner_detect(img, cor, &corc, prowt->reg, prow[x]->reg, prow[x-2]->reg, &reg[regc], yx, w, corth);

					left_neighborhood(&row[rowc], prow, preg, &pregc, x);
					top_neighborhood(&row[rowc], prow1, preg, &pregc, x);
					prowt = prow[x];
					prow[x] = &row[rowc];
					rowc++; regc++;
					//printf("%5d %p yx = %d  l = %d\n", rowc, &row[rowc-1], row[rowc-1].yx, row[rowc-1].length);
					break;
				}
				case 1 : {
					add_pixel(prow[x-2]->reg, prow[x-2], img, yx, w);

					top_neighborhood(&row[rowc-1], prow1, preg, &pregc, x);
					prow[x] = prow[x-2];
					//img[yx] = img[yx-2]; img[yx+1] = img[yx-1]; img[yx+w] = img[yx+w-2]; img[yx+w+1] = img[yx+w-1];
					break;
				}
				case 2 : {
					//rowc++;
					new_row(&row[rowc], prow1[x]->reg, yx);
					add_pixel(prow1[x]->reg, &row[rowc], img, yx, w);
					//corner_detect(img, cor, &corc, prowt->reg, prow[x]->reg, prow[x-2]->reg, prow[x]->reg, yx, w, corth);

					left_neighborhood(&row[rowc], prow, preg, &pregc, x);
					top_neighborhood(&row[rowc], prow1, preg, &pregc, x);
					prowt = prow[x];
					prow[x] = &row[rowc];
					rowc++;
					//img[yx] = img[yx-w1]; img[yx+1] = img[yx+1-w1]; img[yx+w] = img[yx+w-w1]; img[yx+w+1] = img[yx+w+1-w1];
					break;
				}
				case 3 : {
					//printf("prow[x-2]->reg = %p prow1[x]->reg = %p\n", prow[x-2]->reg, prow1[x]->reg);
					if(dfl < dft){
						add_pixel(prow[x-2]->reg, prow[x-2], img, yx, w);

						top_neighborhood(&row[rowc-1], prow1, preg, &pregc, x);
						prow[x] = prow[x-2];
						//img[yx] = img[yx-2]; img[yx+1] = img[yx-1]; img[yx+w] = img[yx+w-2]; img[yx+w+1] = img[yx+w-1];
					} else {
						if(prow[x-2]->reg != prow1[x]->reg){
							//rowc++;
							new_row(&row[rowc], prow1[x]->reg, yx);
							add_pixel(prow1[x]->reg, &row[rowc], img, yx, w);
							//corner_detect(img, cor, &corc, prowt->reg, prow[x]->reg, prow[x-2]->reg, prow[x]->reg, yx, w, corth);

							left_neighborhood(&row[rowc], prow, preg, &pregc, x);
							top_neighborhood(&row[rowc], prow1, preg, &pregc, x);
							prowt = prow[x];
							prow[x] = &row[rowc];
							rowc++;
						} else {
							add_pixel(prow[x-2]->reg, prow[x-2], img, yx, w);

							top_neighborhood(&row[rowc-1], prow1, preg, &pregc, x);
							prow[x] = prow[x-2];
						}
						//img[yx] = img[yx-w1]; img[yx+1] = img[yx+1-w1]; img[yx+w] = img[yx+w-w1]; img[yx+w+1] = img[yx+w+1-w1];
					}
					break;
				}
			}
		}
		//Coner detection
		if(y > w1) {
			for(x=2; x < w-2; x+=2){
				yx = y-w1+x;
				//printf("corc = %d prj = %p prk = %p pri = %p\n", corc, &pr[j][x-2], &pr[k][x-2], &pr[i][x-2]);
				corner_detect(img, cor, &corc, &pr[j][x-2], &pr[k][x-2], &pr[i][x-2], yx, w, corth);
			}
		}
	}
	*nrows = rowc; *nregs = regc; *npreg = pregc; *ncors = corc;
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
			//row[rc].reg->c[0] = row[rc].reg->ac[0] / row[rc].reg->npixs;
			//row[rc].reg->c[1] = row[rc].reg->ac[1] / row[rc].reg->npixs;
			//row[rc].reg->c[2] = row[rc].reg->ac[2] / row[rc].reg->npixs;
			//row[rc].reg->c[3] = row[rc].reg->ac[3] / row[rc].reg->npixs;
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
*/

void seg_corners_draw(imgtype *img, Corner *cor, uint32 ncors, uint32 w)
{
	uint32 i;
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
	//obj->ac[3] = reg->c[3]*reg->npixs;
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
	//obj->ac[3] += reg->c[3]*reg->npixs;
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
			abs(reg1->c[2]	- reg2->c[2]) < theresh ;
			//abs(reg1->c[3]	- reg2->c[3]) < theresh;
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
			c2 = abs(reg1->c[2]	- reg2->c[2]);
			//c3 = abs(reg1->c[3]	- reg2->c[3]);
	*diff = c0 + c1 + c2;
	return  c0 < theresh && c1 < theresh && c2 < theresh;
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
			printf("neic = %d ", reg[j].neic);
			for(i=0; i < reg[j].neic; i++){
				if(reg[j].reg[i]->obj != NULL){
					if(check_neighbor2(&reg[j], reg[j].reg[i], theresh, &diff)) {
						if(ck){
							if(diff < min) { min = diff; in = i;}
						} else {
							in = i; min = diff;
						}
						ck++;
						printf("ck = %d ", ck);
					}
				}
			}
			if(ck) { add_reg(reg[j].reg[in]->obj, &reg[j]); }
			else { new_obj(&obj[oc], &reg[j]); oc++; }
			//if(!ck)  { new_obj(&obj[oc], &reg[j]); oc++;}
			//else{
			//	if(reg[j].reg[in]->obj == NULL){
			//		new_obj(&obj[oc], &reg[j]);
			//		add_reg(&obj[oc], reg[j].reg[in]); oc++;
			//	} else {
			//		add_reg(reg[j].reg[in]->obj, &reg[j]);
			//	}
			//}
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
			//reg[j].obj->c[3] = reg[j].obj->ac[3] / reg[j].obj->npixs;
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
			//region_draw(img, obj[i].reg[j], obj[i].c, w);
		}
		//}
	}
	//printf("objpix = %d regpix = %d\n", tmp, tmp1);
}


void seg_color_quant(imgtype *img, imgtype *img1, uint32 w, uint32 h, uint32 quant)
{
	uint32 y, x, yx, w1 = w<<1, h1 = ((h>>1)<<1)*w, half = ((1<<quant)>>1);
	uint32 mask = (0xFFFFFFFF>>quant)<<quant;
	for(y=0; y < h1; y+=w1) {
		for(x=0; x < w; x+=2){
			yx = y+x;
			/*
			img1[yx]		= ((img[yx]		>>quant)<<quant) + half;
			img1[yx+1]		= ((img[yx+1]	>>quant)<<quant) + half;
			img1[yx+w]		= ((img[yx+w]	>>quant)<<quant) + half;
			img1[yx+w+1]	= ((img[yx+w+1]	>>quant)<<quant) + half;*/
			img1[yx]		= (img[yx]		& mask) + half;
			img1[yx+1]		= (img[yx+1]	& mask) + half;
			img1[yx+w]		= (img[yx+w]	& mask) + half;
			img1[yx+w+1]	= (img[yx+w+1]	& mask) + half;

		}
	}
}


void seg_coners(uchar *img, Corner *cor, uint32 w, uint32 h, uint32 theresh, uint32 *ncors, uint32 color)
{
	uint32 c, x, y, yx, h1 = h*w-w, w1 = w-1, i=0, d, diff=0;
	//*ncors = 0;
	for(y=w; y < h1; y+=w){
		for(x=1; x < w1; x++){
			yx = y + x;
			c = 0; diff=0;
			d = abs(img[yx] - img[yx-w-1]); if(d > theresh) { c++; diff+=d; }
			d = abs(img[yx] - img[yx-w]	 ); if(d > theresh) { c++; diff+=d; }
			d = abs(img[yx] - img[yx-w+1]); if(d > theresh) { c++; diff+=d; }
			d = abs(img[yx] - img[yx-1]	 ); if(d > theresh) { c++; diff+=d; }
			d = abs(img[yx] - img[yx+1]	 ); if(d > theresh) { c++; diff+=d; }
			d = abs(img[yx] - img[yx+w-1]); if(d > theresh) { c++; diff+=d; }
			d = abs(img[yx] - img[yx+w]	 ); if(d > theresh) { c++; diff+=d; }
			d = abs(img[yx] - img[yx+w+1]); if(d > theresh) { c++; diff+=d; }
			if(c >= 4 && c <= 7) {
				cor[*ncors].yx = yx;
				cor[*ncors].diff = diff;
				cor[*ncors].c[color] = img[yx];
				(*ncors)++;
				//img[yx] = 255;
			}
		}
	}
}

static inline uint32 check_color_3x3_1(uchar *img, uint32 yx, uint32 w, uint32 theresh, uint32 *diff)
{
	uint32 c=0, d;
	*diff = 0;
	d = abs(img[yx] - img[yx-w-1]); diff+=d; if(d > theresh) c++;
	d = abs(img[yx] - img[yx-w]	 ); diff+=d; if(d > theresh) c++;
	d = abs(img[yx] - img[yx-w+1]); diff+=d; if(d > theresh) c++;
	d = abs(img[yx] - img[yx-1]	 ); diff+=d; if(d > theresh) c++;
	d = abs(img[yx] - img[yx+1]	 ); diff+=d; if(d > theresh) c++;
	d = abs(img[yx] - img[yx+w-1]); diff+=d; if(d > theresh) c++;
	d = abs(img[yx] - img[yx+w]	 ); diff+=d; if(d > theresh) c++;
	d = abs(img[yx] - img[yx+w+1]); diff+=d; if(d > theresh) c++;
	//*diff = (*diff)>>3;
	return c;
}

static inline uint32 check_color_3x3(uchar *img, uint32 yx, uint32 yx1, uint32 w, uint32 theresh, uint32 *diff)
{
	uint32 c=0, d;
	//*diff = 0;
	d = abs(img[yx] 	- img[yx1]); 		*diff+=d; if(d > theresh) c++;
	d = abs(img[yx+1] 	- img[yx1+1]); 		*diff+=d; if(d > theresh) c++;
	d = abs(img[yx+w] 	- img[yx1+w]); 		*diff+=d; if(d > theresh) c++;
	d = abs(img[yx+1+w] - img[yx1+1+w]);	*diff+=d; if(d > theresh) c++;
	//printf("diff = %d\n", *diff);
	//*diff = (*diff)>>3;
	return c > 2 ? 1 : 0;
}

void seg_coners_bayer(uchar *img, Corner *cor, uint32 w, uint32 h, uint32 theresh, uint32 *ncors)
{
	uint32 c, x, y, yx, w2 = w<<1, h1 = h*w-w2, w1 = w-2, i=0, d, diff, tmp;
	tmp = *ncors;
	for(y=w2; y < h1; y+=w2){
		for(x=2; x < w1; x+=2){
			diff = 0;
			yx = y + x;
			c  = check_color_3x3(img, yx, yx-w2-2, 	w, theresh, &diff);
			c += check_color_3x3(img, yx, yx-w2,   	w, theresh, &diff);
			c += check_color_3x3(img, yx, yx-w2+2, 	w, theresh, &diff);
			c += check_color_3x3(img, yx, yx-2,		w, theresh, &diff);
			c += check_color_3x3(img, yx, yx+2,		w, theresh, &diff);
			c += check_color_3x3(img, yx, yx+w2-2,	w, theresh, &diff);
			c += check_color_3x3(img, yx, yx+w2,	w, theresh, &diff);
			c += check_color_3x3(img, yx, yx+w2+2,	w, theresh, &diff);
			diff = diff>>5;
			//printf("diff = %d ", diff);
			if(c == 5) {
			//if((c == 5 || c == 4)) {
			//if(diff > 15) {
				cor[*ncors].yx = yx;
				cor[*ncors].diff = diff;
				cor[*ncors].c[0] = img[yx];
				cor[*ncors].c[1] = img[yx+1];
				cor[*ncors].c[2] = img[yx+w];
				cor[*ncors].c[3] = img[yx+1+w];
				(*ncors)++;
				//img[yx] = 255;
			}
		}
	}
	seg_corners_draw(img, &cor[tmp], *ncors - tmp, w);
}


static inline uint32 check_color(uchar *img, uint32 yx, uint32 yx1, uint32 w, uint32 theresh, uint32 *diff)
{
	uint32 c=0, d;
	//*diff = 0;
	d = abs(img[yx] 	- img[yx1]); 		*diff+=d; if(d > theresh) c++;
	d = abs(img[yx+1] 	- img[yx1+1]); 		*diff+=d; if(d > theresh) c++;
	d = abs(img[yx+2] 	- img[yx1+2]); 		*diff+=d; if(d > theresh) c++;
	//printf("diff = %d\n", *diff);
	//*diff = (*diff)>>3;
	return c > 1 ? 1 : 0;
}

void corners_draw(imgtype *img, Corner *cor, uint32 ncors, uint32 w)
{
	uint32 i;
	for(i=0; i < ncors; i++) {
		img[cor[i].yx] 		= 255;
		img[cor[i].yx+1] 	= 255;
		img[cor[i].yx+2] 	= 255;
	}
}

static inline uint32 check_cont(uchar *c)
{
	uchar i, sum=0;
	for(i=0; i < 8; i++) sum += c[i];
	if(sum == 5) {
		if(c[0]){
			for(i=0;;i++) if(!c[i])  break;
			if(!c[i+1] && !c[i+2]) return 1;
		} else{
			for(i=0;;i++) if(c[i])  break;
			if(c[i+1] && c[i+2] && c[i+3] && c[i+4]) return 1;
		}
	}
	return 0;
}
void seg_coners_rgb(uchar *img, Corner *cor, uint32 w, uint32 h, uint32 theresh, uint32 *ncors)
{
	uint32  x, y, yx, diff, w3 = w*3, sq = h*w3-w3;
	uchar c[8], c1;
	for(y=w3; y < sq; y+=w3){
		for(x=3; x < w3; x+=3){
			//if(yx > sq) printf("yx = %d ", yx);
			diff = 0;
			yx = y + x;
			c1  = check_color(img, yx, yx-w3-3, w, theresh, &diff);
			c1 += check_color(img, yx, yx-w3,   w, theresh, &diff);
			c1 += check_color(img, yx, yx-w3+3, w, theresh, &diff);
			c1 += check_color(img, yx, yx-3,	w, theresh, &diff);
			c1 += check_color(img, yx, yx+3,	w, theresh, &diff);
			c1 += check_color(img, yx, yx+w3-3,	w, theresh, &diff);
			c1 += check_color(img, yx, yx+w3,	w, theresh, &diff);
			c1 += check_color(img, yx, yx+w3+3,	w, theresh, &diff);
			c1 += check_color(img, yx, yx-w3-3, w, theresh, &diff);
			/*
			c[1] = check_color(img, yx, yx-w3,   	w, theresh, &diff);
			c[2] = check_color(img, yx, yx-w3+3, 	w, theresh, &diff);
			c[3] = check_color(img, yx, yx-3,		w, theresh, &diff);
			c[4] = check_color(img, yx, yx+3,		w, theresh, &diff);
			c[5] = check_color(img, yx, yx+w3-3,	w, theresh, &diff);
			c[6] = check_color(img, yx, yx+w3,		w, theresh, &diff);
			c[7] = check_color(img, yx, yx+w3+3,	w, theresh, &diff);
			*/
			diff = diff>>5;
			//printf("diff = %d ", diff);
			if(c1 <= 6 && c1 >= 4 ) {
			//if(check_cont(c)) {
			//if((c == 5 || c == 4)) {
			//if(diff > 15) {
				cor[*ncors].yx = yx;
				cor[*ncors].diff = diff;
				cor[*ncors].c[0] = img[yx];
				cor[*ncors].c[1] = img[yx+1];
				cor[*ncors].c[2] = img[yx+2];
				(*ncors)++;
				//img[yx] = 255;
			}
		}
	}
	//corners_draw(img, cor, *ncors, w);
}

static inline void set_dir(imgtype *img, uchar *dir, uint32 x, uint32 y, uint32 w, uint32 h)
{
	uint32 yx, min, w1 = w-1, h1 = h1-1;
	yx = y*w + x;
	if(!img[yx]) { *dir = 0; return; }
	min = img[yx]; *dir = 1;
	if(x) 		if(img[yx-1] < min) { min = img[yx-1]; *dir = 1; }
	if(y) 		if(img[yx-w] < min) { min = img[yx-w]; *dir = 2; }
	if(x != w1) if(img[yx+1] < min) { min = img[yx+1]; *dir = 3; }
	if(y != h1) if(img[yx+w] < min) { min = img[yx+w]; *dir = 4; }
}
/*
static inline uint32 check_left(uchar dir1, uchar dir2)
{
	if(!dir2 && !dir1) return 1;
	if((dir1 == 1 && dir2 == 3) || (dir1 == 2 && dir2 == 4) || (dir1 == 4 && dir2 == 2)) return 0;
	else return 1;
}

static inline uint32 check_top(uchar dir1, uchar dir2)
{
	if(!dir2 && !dir1) return 1;
	if((dir1 == 2 && dir2 == 4) || (dir1 == 1 && dir2 == 3) || (dir1 == 3 && dir2 == 1)) return 0;
	else return 1;
}


static inline uint32 check(imgtype *img, Region *reg, uint32 yx, uint32 theresh, uint32 *diff)
{
	uint32  c0 = abs(reg->c[0]	- img[yx]    ),
			c1 = abs(reg->c[1] 	- img[yx+1]  ),
			c2 = abs(reg->c[2]	- img[yx+2]  );
	*diff = c0 + c1 + c2;
	return  !(c0 > theresh && c1 > theresh && c2 > theresh);
}

static inline uint32 check1(imgtype *img, uint32 yx, uint32 yx1, uint32 theresh, uint32 *diff)
{
	uint32  c0 = abs(img[yx1]	- img[yx]    ),
			c1 = abs(img[yx1+1] - img[yx+1]  ),
			c2 = abs(img[yx1+2]	- img[yx+2]  );
	*diff = c0 + c1 + c2;
	return  !(c0 > theresh && c1 > theresh && c2 > theresh);
}

static inline uint32 check2(imgtype *img, Region *reg, uint32 yx, uint32 q, uint32 *diff)
{
	uint32 	c0 = ((img[yx  ]>>q)<<q) - reg->c[0],
			c1 = ((img[yx+1]>>q)<<q) - reg->c[1],
			c2 = ((img[yx+2]>>q)<<q) - reg->c[2];
	*diff = c0 + c1 + c2;
	return  !c0 && !c1  && !c2;
}

static inline new_row(Row *row, Region *reg, uint32 yx)
{
	row->yx = yx;  row->length = 0; row->reg = reg; reg->nrows++; //reg->row[reg->nrows] = row; reg->nrows++;
	//row->ac[0] = 0; row->ac[1] = 0; row->ac[2] = 0; row->ac[3] = 0;
}

static inline new_region(Region *reg, Row *row, imgtype *img, uint32 yx, uint32 w)
{
	reg->c[0] = img[yx]; reg->c[1] = img[yx+1]; reg->c[2] = img[yx+2];
	//reg->c[0] = ((img[yx]>>q)<<q);
	//reg->c[1] = ((img[yx+1]>>q)<<q);
	//reg->c[2] = ((img[yx+w]>>q)<<q);
	//reg->c[3] = ((img[yx+w+1]>>q)<<q);
	//reg->ac[0] = 0; reg->ac[1] = 0; reg->ac[2] = 0; reg->ac[3] = 0;
	reg->nrows = 0; reg->rowc = 0; reg->npixs = 0; reg->neic = 0; reg->nneis = 0; reg->obj = NULL;	//reg->row[reg->nrows] = row;
}

static inline add_pixel(Region *reg, Row *row, imgtype *img, uint32 yx, uint32 w)
{
	row->length += 3;
	//reg->ac[0] += img[yx]; reg->ac[1] += img[yx+1]; reg->ac[2] += img[yx+w]; reg->ac[3] += img[yx+w+1];
	reg->npixs++;
}

static inline left_neighborhood(Row *row, Row **prow, Region **preg, uint32 *pregc, uint32 x)
{
	preg[(*pregc)++] = prow[x-1]->reg; prow[x-1]->reg->neic++;
	preg[(*pregc)++] = row->reg; row->reg->neic++;

}

static inline top_neighborhood(Row *row, Row **prow, Row **prow1, Region **preg, uint32 *pregc, uint32 x)
{
	if(prow[x-1]->reg != prow[x]->reg)
		if(prow[x-1]->reg != prow1[x]->reg) {
			preg[(*pregc)++] = prow1[x]->reg; prow1[x]->reg->neic++;
			preg[(*pregc)++] = row->reg; row->reg->neic++;
		}
	if(prow1[x]->reg != row->reg)
		if(prow1[x-1]->reg != prow1[x]->reg){
			preg[(*pregc)++] = prow1[x]->reg; prow1[x]->reg->neic++;
			preg[(*pregc)++] = row->reg; row->reg->neic++;
		}
}

void seg_regions(imgtype *img, imgtype *grad, Region *reg, Row *row, Corner *cor, Row **pro, Region **preg, uchar *dir, uint32 w, uint32 h, uint32 theresh, uint32 corth,
		uint32 *nrows, uint32 *nregs, uint32 *npreg, uint32 *ncors)
{
	uint32 i=0, j=1, k=2, y, h1 = h*w, x, w1 = w-1, yx;
	uint32 df1[4], df2[4], left=0, regc =0, rowc = 0, corc = 0, pregc = 0, rc, tmp = 0, dfl, dft;
	uint32 q = 5;
	Row **pr[3], **prow, **prow1;
	Row *prowt;
	pr[0] = pro; pr[1] = &pro[w]; pr[2] = &pro[w<<1];

	prow = pr[i];
	y=0; x=0;
	yx = y+x;
	new_region(&reg[regc], &row[rowc], img, yx, w);
	new_row(&row[rowc], &reg[regc], yx);
	add_pixel(&reg[regc], &row[rowc], img, yx, w);
	set_dir(grad, &dir[x], x, y, w, h);
	prow[x] = &row[rowc];
	rowc++; regc++;
	for(x=1; x < w; x++){
		yx = x;
		set_dir(grad, &dir[yx], x, y, w, h);
		if(check_left(dir[yx-1], dir[yx]))
		{
			add_pixel(prow[x-1]->reg, prow[x-1], img, yx, w);
			prow[x] = prow[x-1];
			//printf("%5d %p yx = %d l = %d\n", rowc-1, &row[rowc-1], row[rowc-1].yx, row[rowc-1].length);
		} else {
			new_region(&reg[regc], &row[rowc], img, yx, w);
			new_row(&row[rowc], &reg[regc], yx);
			add_pixel(&reg[regc], &row[rowc], img, yx, w);

			left_neighborhood(&row[rowc], prow, preg, &pregc, x);
			prow[x] = &row[rowc];
			//printf("%5d %p yx = %d l = %d\n", rowc, &row[rowc], row[rowc].yx, row[rowc].length);
			rowc++; regc++;
		}
	}
	//printf("%5d %p yx = %d  l = %d\n", rowc, &row[rowc-1], row[rowc-1].yx, row[rowc-1].length);
	for(y=1; y < h; y++) {
		x=0;
		yx = y*w+x;
		i = (i == 2) ? 0 : i+1;
		j = (j == 2) ? 0 : j+1;
		k = (k == 2) ? 0 : k+1;
		prow = pr[i];
		prow1 = pr[k];
		set_dir(grad, &dir[yx], x, y, w, h);
		if(check_top(dir[yx-w], dir[yx]))
		{
			new_row(&row[rowc], prow1[x]->reg, yx);
			add_pixel(prow1[x]->reg, &row[rowc], img, yx, w);
			prow[x] = &row[rowc];
			rowc++;
		} else {
			new_region(&reg[regc], &row[rowc], img, yx, w);
			new_row(&row[rowc], &reg[regc], yx);
			add_pixel(&reg[regc], &row[rowc], img, yx, w);

			top_neighborhood(&row[rowc], prow, prow1, preg, &pregc, x);
			prow[x] = &row[rowc];
			rowc++; regc++;
		}
		for(x=1; x < w; x++){
			yx = y*w + x;
			set_dir(grad, &dir[yx], x, y, w, h);
			//printf("prow = %p prow1 = %p yx = %d\n", prow[x-2], prow1[x], yx);
			left =  check_left(dir[yx-1], dir[yx]);
			left += check_top(dir[yx-w], dir[yx])<<1;
			//if(left != 3) printf("%5d %p yx = %d  l = %d left = %d \n", rowc, &row[rowc-1], row[rowc-1].yx, row[rowc-1].length, left);
			//printf("%5d %p yx = %d  l = %d left = %d \n", rowc, &row[rowc-1], row[rowc-1].yx, row[rowc-1].length, left);
			switch(left){
				case 0 : {
					new_region(&reg[regc], &row[rowc], img, yx, w);
					new_row(&row[rowc], &reg[regc], yx);
					add_pixel(&reg[regc], &row[rowc], img, yx, w);
					//corner_detect(img, cor, &corc, prowt->reg, prow[x]->reg, prow[x-2]->reg, &reg[regc], yx, w, corth);

					left_neighborhood(&row[rowc], prow, preg, &pregc, x);
					top_neighborhood(&row[rowc], prow, prow1, preg, &pregc, x);
					prowt = prow[x];
					prow[x] = &row[rowc];
					rowc++; regc++;
					//printf("%5d %p yx = %d  l = %d\n", rowc, &row[rowc-1], row[rowc-1].yx, row[rowc-1].length);
					break;
				}
				case 1 : {
					add_pixel(prow[x-1]->reg, prow[x-1], img, yx, w);

					top_neighborhood(&row[rowc-1], prow, prow1, preg, &pregc, x);
					prow[x] = prow[x-1];
					break;
				}
				case 2 : {
					//rowc++;
					new_row(&row[rowc], prow1[x]->reg, yx);
					add_pixel(prow1[x]->reg, &row[rowc], img, yx, w);
					//corner_detect(img, cor, &corc, prowt->reg, prow[x]->reg, prow[x-2]->reg, prow[x]->reg, yx, w, corth);

					left_neighborhood(&row[rowc], prow, preg, &pregc, x);
					top_neighborhood(&row[rowc], prow, prow1, preg, &pregc, x);
					prowt = prow[x];
					prow[x] = &row[rowc];
					rowc++;
					break;
				}
				case 3 : {
					//printf("prow[x-2]->reg = %p prow1[x]->reg = %p\n", prow[x-2]->reg, prow1[x]->reg);
					add_pixel(prow[x-1]->reg, prow[x-1], img, yx, w);

					top_neighborhood(&row[rowc-1], prow, prow1, preg, &pregc, x);
					prow[x] = prow[x-1];
					break;
				}
			}
		}
	}
	*nrows = rowc; *nregs = regc; *npreg = pregc; *ncors = corc;
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
			//row[rc].reg->c[0] = row[rc].reg->ac[0] / row[rc].reg->npixs;
			//row[rc].reg->c[1] = row[rc].reg->ac[1] / row[rc].reg->npixs;
			//row[rc].reg->c[2] = row[rc].reg->ac[2] / row[rc].reg->npixs;
			//row[rc].reg->c[3] = row[rc].reg->ac[3] / row[rc].reg->npixs;
			//printf(reg[rc].row[y])
		}
	}
}

void seg_regions_rgb(imgtype *img, Region *reg, Row *row, Corner *cor, Row **pro, Region **preg, uint32 w, uint32 h, uint32 theresh, uint32 corth,
		uint32 *nrows, uint32 *nregs, uint32 *npreg, uint32 *ncors)
{
	uint32 i=0, j=1, k=2, y, h3 = h*w*3, x, x1, w3 = w*3, yx, w6 = w3<<1;
	uint32 df1[4], df2[4], left=0, regc =0, rowc = 0, corc = 0, pregc = 0, rc, tmp = 0, dfl, dft;
	uint32 q = 5;
	Row **pr[3], **prow, **prow1;
	Row *prowt;
	pr[0] = pro; pr[1] = &pro[w]; pr[2] = &pro[w<<1];

	prow = pr[i];
	y=0; x=0;
	yx = y+x;
	new_region(&reg[regc], &row[rowc], img, yx, w);
	new_row(&row[rowc], &reg[regc], yx);
	add_pixel(&reg[regc], &row[rowc], img, yx, w);
	prow[x] = &row[rowc];
	rowc++; regc++;
	for(x=3, x1=1; x < w3; x+=3, x1++){
		yx = y+x;
		//if(check_quant(img, yx, yx-2, w, q, &dfl))
		if(check(img, prow[x1-1]->reg, yx, theresh, &dfl))
		//if(check(img, yx, yx-3, theresh, &dfl))
		{
			add_pixel(prow[x1-1]->reg, prow[x1-1], img, yx, w);
			prow[x1] = prow[x1-1];
			//printf("%5d %p yx = %d l = %d\n", rowc-1, &row[rowc-1], row[rowc-1].yx, row[rowc-1].length);
			//img[yx] = img[yx-2]; img[yx+1] = img[yx-1]; img[yx+w] = img[yx+w-2]; img[yx+w+1] = img[yx+w-1];
		} else {
			//rowc++; regc++;
			new_region(&reg[regc], &row[rowc], img, yx, w);
			new_row(&row[rowc], &reg[regc], yx);
			add_pixel(&reg[regc], &row[rowc], img, yx, w);

			left_neighborhood(&row[rowc], prow, preg, &pregc, x1);
			prow[x1] = &row[rowc];
			//printf("%5d %p yx = %d l = %d\n", rowc, &row[rowc], row[rowc].yx, row[rowc].length);
			rowc++; regc++;
		}
	}
	//printf("%5d %p yx = %d  l = %d\n", rowc, &row[rowc-1], row[rowc-1].yx, row[rowc-1].length);
	for(y=w3; y < h3; y+=w3) {
		x=0; x1 = 0;
		yx = y+x;
		i = (i == 2) ? 0 : i+1;
		j = (j == 2) ? 0 : j+1;
		k = (k == 2) ? 0 : k+1;
		prow = pr[i];
		prow1 = pr[k];
		//printf("i = %d j = %d k = %d\n", i, j, k);
		//prowt = prow[x];
		//if(check_top(img, yx, w, theresh, &dft))
		//if(check_quant(img, yx, yx-w1, w, q, &dfl))
		if(check(img, prow1[x1]->reg, yx, theresh, &dft))
		//if(check(img, yx, yx-w3, theresh, &dft))
		{
			//rowc++;
			new_row(&row[rowc], prow1[x1]->reg, yx);
			add_pixel(prow1[x1]->reg, &row[rowc], img, yx, w);
			prow[x1] = &row[rowc];
			rowc++;
			//img[yx] = img[yx-w1]; img[yx+1] = img[yx+1-w1]; img[yx+w] = img[yx+w-w1]; img[yx+w+1] = img[yx+w+1-w1];
		} else {
			//rowc++; regc++;
			new_region(&reg[regc], &row[rowc], img, yx, w);
			new_row(&row[rowc], &reg[regc], yx);
			add_pixel(&reg[regc], &row[rowc], img, yx, w);

			top_neighborhood(&row[rowc], prow, prow1, preg, &pregc, x1);
			prow[x1] = &row[rowc];
			rowc++; regc++;
		}
		for(x=3, x1=1; x < w3; x+=3, x1++){
			yx = y+x;
			//printf("prow = %p prow1 = %p yx = %d\n", prow[x-2], prow1[x], yx);
			//left =  check_quant(img, yx, yx-2, w, q, &dfl);
			//left += check_quant(img, yx, yx-w1, w, q, &dfl)<<1;
			left =  check(img, prow[x1-1]->reg, yx, theresh, &dfl);
			left += check(img, prow1[x1]->reg, yx, theresh, &dft)<<1;
			//left =  check(img, yx, yx-3, theresh, &dfl);
			//left += check(img, yx, yx-w3, theresh, &dft)<<1;
			//printf("%5d %p yx = %d  l = %d left = %d, dfl = %d dft = %d\n", rowc, &row[rowc-1], row[rowc-1].yx, row[rowc-1].length, left, dfl, dft);
			switch(left){
				case 0 : {
					//rowc++; regc++;
					new_region(&reg[regc], &row[rowc], img, yx, w);
					new_row(&row[rowc], &reg[regc], yx);
					add_pixel(&reg[regc], &row[rowc], img, yx, w);
					//corner_detect(img, cor, &corc, prowt->reg, prow[x]->reg, prow[x-2]->reg, &reg[regc], yx, w, corth);

					left_neighborhood(&row[rowc], prow, preg, &pregc, x1);
					top_neighborhood(&row[rowc], prow, prow1, preg, &pregc, x1);
					prowt = prow[x1];
					prow[x1] = &row[rowc];
					rowc++; regc++;
					//printf("%5d %p yx = %d  l = %d\n", rowc, &row[rowc-1], row[rowc-1].yx, row[rowc-1].length);
					break;
				}
				case 1 : {
					add_pixel(prow[x1-1]->reg, prow[x1-1], img, yx, w);

					top_neighborhood(&row[rowc-1], prow, prow1, preg, &pregc, x1);
					prow[x1] = prow[x1-1];
					//img[yx] = img[yx-2]; img[yx+1] = img[yx-1]; img[yx+w] = img[yx+w-2]; img[yx+w+1] = img[yx+w-1];
					break;
				}
				case 2 : {
					//rowc++;
					new_row(&row[rowc], prow1[x1]->reg, yx);
					add_pixel(prow1[x1]->reg, &row[rowc], img, yx, w);
					//corner_detect(img, cor, &corc, prowt->reg, prow[x]->reg, prow[x-2]->reg, prow[x]->reg, yx, w, corth);

					left_neighborhood(&row[rowc], prow, preg, &pregc, x1);
					top_neighborhood(&row[rowc], prow, prow1, preg, &pregc, x1);
					prowt = prow[x1];
					prow[x1] = &row[rowc];
					rowc++;
					//img[yx] = img[yx-w1]; img[yx+1] = img[yx+1-w1]; img[yx+w] = img[yx+w-w1]; img[yx+w+1] = img[yx+w+1-w1];
					break;
				}
				case 3 : {
					//printf("prow[x-2]->reg = %p prow1[x]->reg = %p\n", prow[x-2]->reg, prow1[x]->reg);
					if(dfl < dft){
						add_pixel(prow[x1-1]->reg, prow[x1-1], img, yx, w);

						top_neighborhood(&row[rowc-1], prow, prow1, preg, &pregc, x1);
						prow[x1] = prow[x1-1];
						//img[yx] = img[yx-2]; img[yx+1] = img[yx-1]; img[yx+w] = img[yx+w-2]; img[yx+w+1] = img[yx+w-1];
					} else {
						if(prow[x1-1]->reg != prow1[x1]->reg){
							//rowc++;
							new_row(&row[rowc], prow1[x1]->reg, yx);
							add_pixel(prow1[x1]->reg, &row[rowc], img, yx, w);
							//corner_detect(img, cor, &corc, prowt->reg, prow[x]->reg, prow[x-2]->reg, prow[x]->reg, yx, w, corth);

							left_neighborhood(&row[rowc], prow, preg, &pregc, x1);
							top_neighborhood(&row[rowc], prow, prow1, preg, &pregc, x1);
							prowt = prow[x1];
							prow[x1] = &row[rowc];
							rowc++;
						} else {
							add_pixel(prow[x1-1]->reg, prow[x1-1], img, yx, w);

							top_neighborhood(&row[rowc-1], prow, prow1, preg, &pregc, x1);
							prow[x1] = prow[x1-1];
						}
						//img[yx] = img[yx-w1]; img[yx+1] = img[yx+1-w1]; img[yx+w] = img[yx+w-w1]; img[yx+w+1] = img[yx+w+1-w1];
					}
					break;
				}
			}
		}
		//Coner detection
		if(y > w6) {
			//for(x=3, x1=1; x < w3-3; x+=3, x1++){
			//	yx = y-w3+x;
				//printf("corc = %d prj = %p prk = %p pri = %p\n", corc, &pr[j][x-2], &pr[k][x-2], &pr[i][x-2]);
				//corner_detect(img, cor, &corc, &pr[j][x1-1], &pr[k][x1-1], &pr[i][x1-1], yx, w, corth);
			//}
		}
	}
	*nrows = rowc; *nregs = regc; *npreg = pregc; *ncors = corc;
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
			//row[rc].reg->c[0] = row[rc].reg->ac[0] / row[rc].reg->npixs;
			//row[rc].reg->c[1] = row[rc].reg->ac[1] / row[rc].reg->npixs;
			//row[rc].reg->c[2] = row[rc].reg->ac[2] / row[rc].reg->npixs;
			//row[rc].reg->c[3] = row[rc].reg->ac[3] / row[rc].reg->npixs;
			//printf(reg[rc].row[y])
		}
	}
}
*/
static inline void region_draw(imgtype *img, Region *reg, uchar *c)
{
	uint32 rowc, yx;
	for(rowc=0; rowc < reg->nrows; rowc++){
		//if(reg->npixs < 10)
		for(yx=reg->row[rowc]->yx; yx < (reg->row[rowc]->yx + reg->row[rowc]->length); yx+=3){
			img[yx] 	= c[0];
			img[yx+1] 	= c[1];
			img[yx+2] 	= c[2];
		}
	}
}

void seg_regions_draw(imgtype *img, Region *reg, uint32 nregs)
{
	uint32 rc;
	for(rc=0; rc < nregs; rc++) region_draw(img, &reg[rc], reg[rc].c);
}


static inline uint32 min_3x3(imgtype *img, uint32 yx, uint32 w)
{
	uint32 min = img[yx];
	if(img[yx-w-1] < min) min = img[yx-w-1];
	if(img[yx-w  ] < min) min = img[yx-w  ];
	if(img[yx-w+1] < min) min = img[yx-w+1];
	if(img[yx-1  ] < min) min = img[yx-1  ];
	if(img[yx+1  ] < min) min = img[yx+1  ];
	if(img[yx+w-1] < min) min = img[yx+w-1];
	if(img[yx+w  ] < min) min = img[yx+w  ];
	if(img[yx+w+1] < min) min = img[yx+w+1];
	return min;
}

static inline uint32 max_3x3(imgtype *img, uint32 yx, uint32 w)
{
	uint32 max = img[yx];
	if(img[yx-w-1] > max) max = img[yx-w-1];
	if(img[yx-w  ] > max) max = img[yx-w  ];
	if(img[yx-w+1] > max) max = img[yx-w+1];
	if(img[yx-1  ] > max) max = img[yx-1  ];
	if(img[yx+1  ] > max) max = img[yx+1  ];
	if(img[yx+w-1] > max) max = img[yx+w-1];
	if(img[yx+w  ] > max) max = img[yx+w  ];
	if(img[yx+w+1] > max) max = img[yx+w+1];
	return max;
}

static inline uint32 maxd_3x3(imgtype *img, uint32 yx, uint32 w)
{
	uint32 dif, max = abs(img[yx-w-1] - img[yx+w+1]);
	dif = abs(img[yx-w  ] - img[yx+w  ]); if(dif > max) max = dif;
	dif = abs(img[yx-w+1] - img[yx+w-1]); if(dif > max) max = dif;
	dif = abs(img[yx-1  ] - img[yx+1  ]); if(dif > max) max = dif;
	return max;
}

static inline uint32 diff(imgtype *img, uint32 yx, uint32 w)
{
	uint32 dif=0;
	dif += abs(img[yx-w-1] - img[yx]);
	dif += abs(img[yx-w  ] - img[yx]);
	dif += abs(img[yx-w+1] - img[yx]);
	dif += abs(img[yx-1  ] - img[yx]);
	dif += abs(img[yx+1  ] - img[yx]);
	dif += abs(img[yx+w-1] - img[yx]);
	dif += abs(img[yx+w  ] - img[yx]);
	dif += abs(img[yx+w+1] - img[yx]);
	return dif>>3;
}

void seg_morph_gradient(imgtype *img, imgtype *img1, uint32 w, uint32 h, uint32 thresh)
{
	uint32 y, x, yx, sq = w*h-w, w1 = w-1, dif;
	for(y=w; y < sq; y+=w){
		for(x=1; x < w1; x++){
			yx = y + x;
			//dif = img[yx] - min_3x3(img, yx, w);
			//dif = max_3x3(img, yx, w) - min_3x3(img, yx, w);
			dif = diff(img, yx, w);
			img1[yx] = dif > thresh ? dif : 0;
		}
	}
}

void seg_fall_forest(imgtype *img, imgtype *img1, uint32 w, uint32 h)
{
	uint32 y, x, yx, sq = w*h, dir, w1 = w-1, h1 = h-1, min;
	//for(x=0; x<sq; x++) img1[x] = img[x];
	for(x=0; x < sq; x++) img1[x] = 255;
	for(y=0; y < h; y++){
		for(x=0; x < w; x++){
			yx = y*w + x;
			if(!img[yx]) img1[yx] = 0;
			else {
				//img1[yx] = 255;
				min = img[yx]; dir = 0;
				if(x) 		if(img[yx-1] < min) { min = img[yx-1]; dir = 1; }
				if(y) 		if(img[yx-w] < min) { min = img[yx-w]; dir = 2; }
				if(x != w1) if(img[yx+1] < min) { min = img[yx+1]; dir = 3; }
				if(y != h1) if(img[yx+w] < min) { dir = 4; }
				switch(dir){
					case 0 : { img1[yx  ] = 0; break; }
					case 1 : { img1[yx-1] = 0; break; }
					case 2 : { img1[yx-w] = 0; break; }
					case 3 : { img1[yx+1] = 0; break; }
					case 4 : { img1[yx+w] = 0; break; }
				}
			}
		}
	}
}

void seg_remove_pix(imgtype *img, imgtype *img1, uint32 w, uint32 h)
{
	uint32 y, x, yx, sq = w*h, dir, w1 = w-1, h1 = h-1, max;
	for(x=0; x < sq; x++) img1[x] = 0;
	for(y=0; y < h; y++){
		for(x=0; x < w; x++){
			yx = y*w + x;
			if(img[yx]) {
				max = img[yx];
				if(	img[yx-w-1] < max &&
					img[yx-w  ] < max &&
					img[yx-w+1] < max &&
					img[yx-1  ] < max &&
					img[yx+1  ] < max &&
					img[yx+w-1] < max &&
					img[yx+w  ] < max &&
					img[yx+w+1] < max) img1[yx] = 255;
			}
		}
	}
}

void seg_remove_pix1(imgtype *img, imgtype *img1, uint32 w, uint32 h)
{
	uint32 y, x, yx, sq = w*h, dir, w1 = w-1, h1 = h-1, max;
	for(x=0; x < sq; x++) img1[x] = 0;
	for(y=0; y < h; y++){
		for(x=0; x < w; x++){
			yx = y*w + x;
			if(img[yx]) {
				max = 0;
				if(img[yx-w-1]) max++;
				if(img[yx-w  ]) max++;
				if(img[yx-w+1]) max++;
				if(img[yx-1  ]) max++;
				if(img[yx+1  ]) max++;
				if(img[yx+w-1]) max++;
				if(img[yx+w  ]) max++;
				if(img[yx+w+1]) max++;

				if(max > 2) img1[yx] = 255;
			}
		}
	}
}

void seg_remove_pix2(imgtype *img, imgtype *img1, uint32 w, uint32 h)
{
	uint32 y, x, yx, sq = w*h, dir, w1 = w-1, h1 = h-1, max;
	for(x=0; x < sq; x++) img1[x] = 0;
	for(y=0; y < h; y++){
		for(x=0; x < w; x++){
			yx = y*w + x;
			if(img[yx]){
				if(img[yx-w] && img[yx+w]) {
					if		(img[yx-w-1] && img[yx-1] && img[yx+w-1]) img1[yx] = 255;
					else if	(img[yx-w+1] && img[yx+1] && img[yx+w+1]) img1[yx] = 255;
					else if	(img[yx-w-1] && img[yx-1] && img[yx+w+1]) img1[yx] = 255;
					else if	(img[yx-w-1] && img[yx+1] && img[yx+w+1]) img1[yx] = 255;
					else if	(img[yx-w+1] && img[yx-1] && img[yx+w-1]) img1[yx] = 255;
					else if	(img[yx-w+1] && img[yx+1] && img[yx+w-1]) img1[yx] = 255;
				}
				else if(img[yx-1] && img[yx+1]) {
					if		(img[yx-w-1] && img[yx-w] && img[yx-w+1]) img1[yx] = 255;
					else if	(img[yx+w-1] && img[yx+w] && img[yx+w+1]) img1[yx] = 255;
					else if	(img[yx-w-1] && img[yx-w] && img[yx+w+1]) img1[yx] = 255;
					else if	(img[yx-w-1] && img[yx+w] && img[yx+w+1]) img1[yx] = 255;
					else if	(img[yx+w-1] && img[yx-w] && img[yx-w+1]) img1[yx] = 255;
					else if	(img[yx+w-1] && img[yx+w] && img[yx-w+1]) img1[yx] = 255;
				}
				else if(img[yx-1] && img[yx-w] && img[yx-w-1]) img1[yx] = 255;
				else if(img[yx+1] && img[yx-w] && img[yx-w+1]) img1[yx] = 255;
				else if(img[yx+1] && img[yx+w] && img[yx+w+1]) img1[yx] = 255;
				else if(img[yx-1] && img[yx+w] && img[yx+w-1]) img1[yx] = 255;
			}
		}
	}
}

void seg_connect_pix(imgtype *img, imgtype *img1, uint32 w, uint32 h)
{
	uint32 y, x, yx, sq = w*h, dir, w1 = w-1, h1 = h-1, max, in, c, cn;
	//for(x=0; x < sq; x++) img1[x] = 0;
	for(y=1; y < h1; y++){
		for(x=1; x < w1; x++){
			yx = y*w + x;
			if(img[yx] == 255) {
				max = 0; cn = 0;
				c = yx-w-1; if(img[c] != 255) { cn++; if(img[c] > max) { max = img[c]; in = c; }}
				c = yx-w  ; if(img[c] != 255) { cn++; if(img[c] > max) { max = img[c]; in = c; }}
				c = yx-w+1; if(img[c] != 255) { cn++; if(img[c] > max) { max = img[c]; in = c; }}
				c = yx-1  ; if(img[c] != 255) { cn++; if(img[c] > max) { max = img[c]; in = c; }}
				c = yx+1  ; if(img[c] != 255) { cn++; if(img[c] > max) { max = img[c]; in = c; }}
				c = yx+w-1; if(img[c] != 255) { cn++; if(img[c] > max) { max = img[c]; in = c; }}
				c = yx+w  ; if(img[c] != 255) { cn++; if(img[c] > max) { max = img[c]; in = c; }}
				c = yx+w+1; if(img[c] != 255) { cn++; if(img[c] > max) { max = img[c]; in = c; }}
				//img[yx] == 255;
				if(cn > 6) img[in] = 255; //printf("in = %d ", in);
			}
		}
	}
}


static inline new_row(Row *row, imgtype *img, uint32 yx)
{
	row->yx = yx;  row->length = 0; row->nrown = 0;
	row->c[0] = img[yx]; row->c[0] = img[yx+1]; row->c[0] = img[yx+2];
}

static inline add_pixel(Row *row)
{
	row->length += 3;
}

static inline uint32 check(imgtype *img, Row *row, uint32 yx, uint32 theresh, uint32 *diff)
{
	uint32  c0 = abs(row->c[0]	- img[yx]    ),
			c1 = abs(row->c[1] 	- img[yx+1]  ),
			c2 = abs(row->c[2]	- img[yx+2]  );
	*diff = c0 + c1 + c2;
	return  !(c0 > theresh && c1 > theresh && c2 > theresh);
}

static inline left_neighborhood(Row *row, Row *row1, Row **prow, uint32 *nprows)
{
	prow[(*nprows)++] = row1; row1->nrown++;
	prow[(*nprows)++] = row ; row ->nrown++;

}

static inline top_neighborhood(Row **prow1, Row *row1, Row **prow, uint32 *nprows, uint32 x)
{
	if(prow1[x-1] != prow1[x]){
		prow[(*nprows)++] = row1; row1->nrown++;
		prow[(*nprows)++] = prow1[x] ; prow1[x] ->nrown++;
	}
}

void seg_row_rgb(imgtype *img, Row *row, Row **prow4, uint32 w, uint32 h, uint32 theresh, uint32 *nrows, uint32 *nprows)
{
	uint32 i=0, j=1, y, h3 = h*w*3, x, x1, w3 = w*3, yx, w6 = w3<<1;
	//uint32 df1[4], df2[4], left=0, regc =0, rowc = 0, corc = 0, pregc = 0, rc, tmp = 0, dfl, dft;
	uint32 dfl, dft, cu = 0;
	//uint32 q = 5;
	//Row **pr[2], **pr1, **pr2;
	Row **prow, **prow1, **prow2, **prow3, **tmp;
	prow = prow4; prow1 = &prow4[w]; prow2 = &prow4[w<<1];
	*nrows = 0; *nprows = 0;

	y=0; x=0;
	yx = y+x;
	new_row(&row[*nrows], img, yx);
	add_pixel(&row[*nrows]);
	prow[x] = &row[*nrows];
	*nrows++;
	for(x=3, x1=1; x < w3; x+=3, x1++){
		yx = y+x;
		if(check(img, prow[x1-1], yx, theresh, &dfl))
		{
			add_pixel(prow[x1-1]);
			prow[x1] = prow[x1-1];
		} else {
			new_row(&row[*nrows], img, yx);
			add_pixel(&row[*nrows]);

			left_neighborhood(prow[x1-1], &row[*nrows], prow2, nprows);
			prow[x1] = &row[*nrows];
			*nrows++;
		}
	}
	for(y=w3; y < h3; y+=w3) {
		x=0; x1=0;
		yx = y+x;
		tmp = prow; prow = prow1; prow1 = tmp;

		new_row(&row[*nrows], img, yx);
		add_pixel(&row[*nrows]);
		prow[x] = &row[*nrows];
		*nrows++;
		for(x=3, x1=1; x < w3; x+=3, x1++){
			yx = y+x;
			if(check(img, prow[x1-1], yx, theresh, &dfl))
			{
				add_pixel(prow[x1-1]);
				top_neighborhood(prow1, &row[*nrows], prow2, nprows, x1);

				prow[x1] = prow[x1-1];
			} else {
				new_row(&row[*nrows], img, yx);
				add_pixel(&row[*nrows]);

				left_neighborhood(prow[x1-1], &row[*nrows], prow2, nprows);
				prow[x1] = &row[*nrows];
				*nrows++;
			}
		}
	}
	printf("nrows = %d nprows = %d \n", *nrows, *nprows);
	prow3 = &prow2[*nprows];

	for(i=0; i < *nprows; i+=2){
		if(!prow2[i]->rownc){
			prow2[i]->rown = &prow3[cu];
			cu += prow2[i]->nrown;
		}
		prow2[i]->rown[prow2[i]->rownc++] = prow2[i+1];
		if(!prow2[i+1]->rownc){
			prow2[i+1]->rown = &prow3[cu];
			cu += prow2[i+1]->nrown;
		}
		prow2[i+1]->rown[prow2[i+1]->rownc++] = prow2[i];
	}

}


