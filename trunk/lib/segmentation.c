#include <walet.h>
#include <stdio.h>

static inline uint32 check(imgtype *img, Region *reg, uint32 yx, uint32 w, uint32 theresh, uint32 *diff)
{
	uint32  c0 = abs(reg->c[0]	- img[yx]    ),
			c1 = abs(reg->c[1] 	- img[yx+1]  ),
			c2 = abs(reg->c[2]	- img[yx+w]  ),
			c3 = abs(reg->c[3]	- img[yx+w+1]);
	*diff = c0 + c1 + c2 + c3;
	return  c0 < theresh || c1 < theresh || c2 < theresh || c3 < theresh;
}

static inline new_row(Row *row, Region *reg, uint16 x, uint32 y)
{
	row->x = x; row->y = y; row->length = 0; row->reg = reg; reg->nrows++; //reg->row[reg->nrows] = row; reg->nrows++;
	//row->ac[0] = 0; row->ac[1] = 0; row->ac[2] = 0; row->ac[3] = 0;
}

static inline new_region(Region *reg, Row *row, imgtype *img, uint32 yx, uint32 w)
{
	reg->c[0] = img[yx]; reg->c[1] = img[yx+1]; reg->c[2] = img[yx+w]; reg->c[3] = img[yx+w+1];
	reg->ac[0] = 0; reg->ac[1] = 0; reg->ac[2] = 0; reg->ac[3] = 0;
	reg->nrows = 0; reg->rowc = 0; reg->npix = 0;//reg->row[reg->nrows] = row;
}

static inline add_pixel(Region *reg, Row *row, imgtype *img, uint32 yx, uint32 w)
{
	row->length += 2;
	reg->ac[0] += img[yx]; reg->ac[1] += img[yx+1]; reg->ac[2] += img[yx+w]; reg->ac[3] += img[yx+w+1];
	reg->npix++;
}

void utils_2d_reg_seg(imgtype *img, Region *reg, Row *rows, Row **ptrc, uint32 w, uint32 h, uint32 theresh, uint32 *nrows, uint32 *nregs )
{
	uint32 y, h1 = ((h>>1)<<1)*w, x, w1 = w<<1, yx;
	uint32 df1[4], df2[4], left=0, regc =0, rowc = 0, dfl, dft;
	y=0;  x=0;
	yx = y+x;
	new_region(&reg[regc], &rows[rowc], img, yx, w);
	new_row(&rows[rowc], &reg[regc], x, y);
	add_pixel(&reg[regc], &rows[rowc], img, yx, w);
	ptrc[x] = &rows[rowc];
	for(x=2; x < w; x+=2){
		yx = y+x;
		if(check(img, ptrc[x-2]->reg, yx, w, theresh, &dfl))
		{
			add_pixel(ptrc[x-2]->reg, ptrc[x-2], img, yx, w);
			ptrc[x] = ptrc[x-2];
			img[yx] = img[yx-2]; img[yx+1] = img[yx-1]; img[yx+w] = img[yx+w-2]; img[yx+w+1] = img[yx+w-1];
		} else {
			//printf("%5d %p x = %d y = %d l = %d\n", rowc, &rows[rowc], rows[rowc].x, rows[rowc].y, rows[rowc].length);
			rowc++; regc++;
			new_region(&reg[regc], &rows[rowc], img, yx, w);
			new_row(&rows[rowc], &reg[regc], x, y);
			add_pixel(&reg[regc], &rows[rowc], img, yx, w);
			ptrc[x] = &rows[rowc];
		}
	}
	//printf("%5d %p x = %d y = %d l = %d\n", rowc, &rows[rowc], rows[rowc].x, rows[rowc].y, rows[rowc].length);
	for(y=w1; y < h1; y+=w1) {
		x=0;
		yx = y+x;
		if(check(img, ptrc[x]->reg, yx, w, theresh, &dft))
		{
			rowc++;
			new_row(&rows[rowc], ptrc[x]->reg, x, y);
			add_pixel(ptrc[x]->reg, &rows[rowc], img, yx, w);
			ptrc[x] = &rows[rowc];
			img[yx] = img[yx-w1]; img[yx+1] = img[yx+1-w1]; img[yx+w] = img[yx+w-w1]; img[yx+w+1] = img[yx+w+1-w1];
		} else {
			rowc++; regc++;
			new_region(&reg[regc], &rows[rowc], img, yx, w);
			new_row(&rows[rowc], &reg[regc], x, y);
			add_pixel(&reg[regc], &rows[rowc], img, yx, w);
			ptrc[x] = &rows[rowc];
		}
		for(x=2; x < w; x+=2){
			yx = y+x;
			left =  check(img, ptrc[x-2]->reg, yx, w, theresh, &dfl);
			left += check(img, ptrc[x]->reg, yx, w, theresh, &dft)<<1;

			switch(left){
				case 0 : {
					rowc++; regc++;
					new_region(&reg[regc], &rows[rowc], img, yx, w);
					new_row(&rows[rowc], &reg[regc], x, y);
					add_pixel(&reg[regc], &rows[rowc], img, yx, w);
					ptrc[x] = &rows[rowc];
					break;
				}
				case 1 : {
					add_pixel(ptrc[x-2]->reg, ptrc[x-2], img, yx, w);
					ptrc[x] = ptrc[x-2];
					img[yx] = img[yx-2]; img[yx+1] = img[yx-1]; img[yx+w] = img[yx+w-2]; img[yx+w+1] = img[yx+w-1];
					break;
				}
				case 2 : {
					rowc++;
					new_row(&rows[rowc], ptrc[x]->reg, x, y);
					add_pixel(ptrc[x]->reg, &rows[rowc], img, yx, w);
					ptrc[x] = &rows[rowc];
					img[yx] = img[yx-w1]; img[yx+1] = img[yx+1-w1]; img[yx+w] = img[yx+w-w1]; img[yx+w+1] = img[yx+w+1-w1];
					break;
				}
				case 3 : {
					if(dfl < dft){
						add_pixel(ptrc[x-2]->reg, ptrc[x-2], img, yx, w);
						ptrc[x] = ptrc[x-2];
						img[yx] = img[yx-2]; img[yx+1] = img[yx-1]; img[yx+w] = img[yx+w-2]; img[yx+w+1] = img[yx+w-1];
					} else {
						if(ptrc[x-2]->reg != ptrc[x]->reg){
							rowc++;
							new_row(&rows[rowc], ptrc[x]->reg, x, y);
							add_pixel(ptrc[x]->reg, &rows[rowc], img, yx, w);
							ptrc[x] = &rows[rowc];
						} else {
							add_pixel(ptrc[x-2]->reg, ptrc[x-2], img, yx, w);
							ptrc[x] = ptrc[x-2];
						}
						img[yx] = img[yx-w1]; img[yx+1] = img[yx+1-w1]; img[yx+w] = img[yx+w-w1]; img[yx+w+1] = img[yx+w+1-w1];
					}
					break;
				}
			}
		}
	}
	*nrows = rowc+1;
	*nregs = regc+1;
	printf("rowc = %d regc = %d\n", rowc, regc);
	//return rowc;
}

void utils_rows_in_reg(Region *reg, Row *rows, Row **prow, uint32 nrows)
{
	uint32 rc, tmp=0;
	for(rc=0; rc < nrows; rc++) {
	//for(rc=0; rc < 1200; rc++) {
		//printf("p = %d x = %d y = %d l = %d\n", rc, rows[rc].x, rows[rc].y, rows[rc].length);
		//printf("rowc = %d ", rows[rc].reg->rowc);
		if(rows[rc].reg->rowc){
			rows[rc].reg->row[rows[rc].reg->rowc] = &rows[rc];
			rows[rc].reg->rowc++;
		} else {
			rows[rc].reg->row = &prow[tmp];
			tmp+=rows[rc].reg->nrows;
			rows[rc].reg->row[rows[rc].reg->rowc] = &rows[rc];
			rows[rc].reg->rowc++;
			rows[rc].reg->ac[0] /= rows[rc].reg->npix;
			rows[rc].reg->ac[1] /= rows[rc].reg->npix;
			rows[rc].reg->ac[2] /= rows[rc].reg->npix;
			rows[rc].reg->ac[3] /= rows[rc].reg->npix;
			//printf(reg[rc].row[y])
		}
	}
}

void utils_region_draw(imgtype *img, Region *reg, uint32 nregs, uint32 w)
{
	uint32 rowc, x, w1 = w<<1, yx, rc, count = 0 ;
	//rc = 97817; { //97848; {
	//rc = 266; {
	for(rc=0; rc < nregs; rc++){
	//for(rc=0; rc < 266; rc++){
		//printf("%6d nrows = %5d npix = %5d x = %5d y = %5d c0 = %3d c1 = %3d c2 = %3d c3 = %3d\n",
		//		rc, reg[rc].nrows, reg[rc].npix ,reg[rc].row[0]->x, reg[rc].row[0]->y, reg[rc].ac[0], reg[rc].ac[1], reg[rc].ac[2], reg[rc].ac[3] );
		//if(reg[rc].npix < 4){ count++;
		//for(rowc=0; rowc < 100; rowc++){
		for(rowc=0; rowc < reg[rc].nrows; rowc++){
			//printf("p = %p x = %d y = %d l = %d\n", reg[rc].row[rowc], reg[rc].row[rowc]->x, reg[rc].row[rowc]->y, reg[rc].row[rowc]->length);
			for(x=reg[rc].row[rowc]->x; x < (reg[rc].row[rowc]->x + reg[rc].row[rowc]->length); x+=2){
				yx = reg[rc].row[rowc]->y + x;
				//if(yx+w+1 >= 2607*w) printf("x = %d", yx+w+1);
				//if(rc >= nregs) printf("rc = %d", rc);
				img[yx] 	= reg[rc].ac[0];
				img[yx+1] 	= reg[rc].ac[1];
				img[yx+w] 	= reg[rc].ac[2];
				img[yx+w+1] = reg[rc].ac[3];
			}
		//}
		}
	}
	printf("count = %d\n", count);
}

