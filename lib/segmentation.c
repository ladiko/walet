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
	reg->nrows = 0; reg->rowc = 0; reg->npix = 0; reg->neic = 0; reg->obj = NULL;	//reg->row[reg->nrows] = row;
}

static inline add_pixel(Region *reg, Row *row, imgtype *img, uint32 yx, uint32 w)
{
	row->length += 2;
	reg->ac[0] += img[yx]; reg->ac[1] += img[yx+1]; reg->ac[2] += img[yx+w]; reg->ac[3] += img[yx+w+1];
	reg->npix++;
}

static inline left_neighborhood(Row *row, Row **ptrc, Region **preg, uint32 *pregc, uint32 x)
{
	preg[(*pregc)++] = ptrc[x-2]->reg;  ptrc[x-2]->reg->neic++;
	preg[(*pregc)++] = row->reg;  row->reg->neic++;

}

static inline top_neighborhood(Row *row, Row **ptrc, Region **preg, uint32 *pregc, uint32 x)
{
	if(row->reg != ptrc[x]->reg) {
		preg[(*pregc)++] = ptrc[x]->reg;  ptrc[x]->reg->neic++;
		preg[(*pregc)++] = row->reg;  row->reg->neic++;
	}
}

void utils_2d_reg_seg(imgtype *img, Region *reg, Row *rows, Row **ptrc, Region **preg, uint32 w, uint32 h, uint32 theresh, uint32 *nrows, uint32 *nregs, uint32 *npreg)
{
	uint32 y, h1 = ((h>>1)<<1)*w, x, w1 = w<<1, yx;
	uint32 df1[4], df2[4], left=0, regc =0, rowc = 0, pregc = 0, dfl, dft;
	y=0; x=0;
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

			left_neighborhood(&rows[rowc], ptrc, preg, &pregc, x);
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

			top_neighborhood(&rows[rowc], ptrc, preg, &pregc, x);
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

					left_neighborhood(&rows[rowc], ptrc, preg, &pregc, x);
					top_neighborhood(&rows[rowc], ptrc, preg, &pregc, x);
					ptrc[x] = &rows[rowc];
					break;
				}
				case 1 : {
					add_pixel(ptrc[x-2]->reg, ptrc[x-2], img, yx, w);

					top_neighborhood(&rows[rowc], ptrc, preg, &pregc, x);
					ptrc[x] = ptrc[x-2];
					img[yx] = img[yx-2]; img[yx+1] = img[yx-1]; img[yx+w] = img[yx+w-2]; img[yx+w+1] = img[yx+w-1];
					break;
				}
				case 2 : {
					rowc++;
					new_row(&rows[rowc], ptrc[x]->reg, x, y);
					add_pixel(ptrc[x]->reg, &rows[rowc], img, yx, w);

					left_neighborhood(&rows[rowc], ptrc, preg, &pregc, x);
					top_neighborhood(&rows[rowc], ptrc, preg, &pregc, x);
					ptrc[x] = &rows[rowc];
					img[yx] = img[yx-w1]; img[yx+1] = img[yx+1-w1]; img[yx+w] = img[yx+w-w1]; img[yx+w+1] = img[yx+w+1-w1];
					break;
				}
				case 3 : {
					if(dfl < dft){
						add_pixel(ptrc[x-2]->reg, ptrc[x-2], img, yx, w);

						top_neighborhood(&rows[rowc], ptrc, preg, &pregc, x);
						ptrc[x] = ptrc[x-2];
						img[yx] = img[yx-2]; img[yx+1] = img[yx-1]; img[yx+w] = img[yx+w-2]; img[yx+w+1] = img[yx+w-1];
					} else {
						if(ptrc[x-2]->reg != ptrc[x]->reg){
							rowc++;
							new_row(&rows[rowc], ptrc[x]->reg, x, y);
							add_pixel(ptrc[x]->reg, &rows[rowc], img, yx, w);

							left_neighborhood(&rows[rowc], ptrc, preg, &pregc, x);
							top_neighborhood(&rows[rowc], ptrc, preg, &pregc, x);
							ptrc[x] = &rows[rowc];
						} else {
							add_pixel(ptrc[x-2]->reg, ptrc[x-2], img, yx, w);

							top_neighborhood(&rows[rowc], ptrc, preg, &pregc, x);
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
	*npreg = pregc;
	printf("rowc = %d regc = %d pregc = %d\n", rowc, regc, pregc);
}

void utils_rows_in_reg(Region *reg, Row *rows, Row **prow, uint32 nrows)
{
	uint32 rc, tmp=0;
	for(rc=0; rc < nrows; rc++) {
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

static inline void region_draw(imgtype *img, Region *reg, uint32 w)
{
	uint32 rowc, yx, x;
	for(rowc=0; rowc < reg->nrows; rowc++){
		for(x=reg->row[rowc]->x; x < (reg->row[rowc]->x + reg->row[rowc]->length); x+=2){
			yx = reg->row[rowc]->y + x;
			img[yx] 	= reg->ac[0];
			img[yx+1] 	= reg->ac[1];
			img[yx+w] 	= reg->ac[2];
			img[yx+w+1] = reg->ac[3];
		}
	}
}

void utils_region_draw(imgtype *img, Region *reg, uint32 nregs, uint32 w)
{
	uint32 rowc, x, w1 = w<<1, yx, rc;
	for(rc=0; rc < nregs; rc++) region_draw(img, &reg[rc], w);
}

static inline uint32 check_reg(Region *reg1, Region *reg2)
{
	int j;
	for(j= reg1->neic; j; j--) {
		if(reg1->reg[j] == reg2) return 0;
	}
	return 1;
}

void reg_neighborhood(Region *reg, Region **pnei, Region **preg, uint32 nreg, uint32 npreg)
{
	uint32 i, j, tmp = 0;
	for(i=0; i < nreg; i++){
		reg[i].reg = &pnei[tmp];
		tmp += reg[i].neic;
		reg[i].neic = 0;
		reg[i].obj = NULL;
	}
	for(i=0; i < npreg; i+=2){
		if(check_reg(preg[i], preg[i+1])){
			preg[i]->reg[preg[i]->neic] = preg[i+1];
			preg[i]->neic++;
		}
		if(check_reg(preg[i+1], preg[i])){
			preg[i+1]->reg[preg[i+1]->neic] = preg[i];
			preg[i+1]->neic++;
		}
	}
	tmp=0;
	for(i=0; i< nreg; i++) tmp += reg[i].neic;
	printf("Total neighborhood = %d\n", tmp);
}

static inline new_obj(Object *obj, Region *reg)
{
	obj->c[0] = reg->c[0];
	obj->c[1] = reg->c[1];
	obj->c[2] = reg->c[2];
	obj->c[3] = reg->c[3];
	obj->npix = reg->npix;
	reg->obj = obj;
	obj->nreg = 1;
	obj->regc = 0;
}

static inline add_reg(Object *obj, Region *reg)
{
	obj->c[0] = (obj->c[0]*obj->npix + reg->c[0]*reg->npix)/(obj->npix + reg->npix);
	obj->c[1] = (obj->c[1]*obj->npix + reg->c[1]*reg->npix)/(obj->npix + reg->npix);
	obj->c[2] = (obj->c[2]*obj->npix + reg->c[2]*reg->npix)/(obj->npix + reg->npix);
	obj->c[3] = (obj->c[3]*obj->npix + reg->c[3]*reg->npix)/(obj->npix + reg->npix);
	obj->npix += reg->npix;
	reg->obj = obj;
	obj->nreg++;
}

static inline uint32 check_neighbor(Region *reg1, Region *reg2, uint32 theresh)
{
	return	abs(reg1->c[0]	- reg2->c[0]) < theresh ||
			abs(reg1->c[1]	- reg2->c[1]) < theresh ||
			abs(reg1->c[2]	- reg2->c[2]) < theresh ||
			abs(reg1->c[3]	- reg2->c[3]) < theresh;
}

void utils_object_fill(Object *obj, Region *reg,  Region **preg, uint32 nregs, uint32 *nobjs, uint32 theresh)
{
	uint32 j, i, oc=0, tmp = 0;

	for(j=0; j < nregs; j++){
		if(reg[j].obj == NULL) {
			new_obj(&obj[oc], &reg[j]);
			oc++;
		}
		for(i=0; i < reg[j].neic; i++){
			if(check_neighbor(&reg[j], reg[j].reg[i], theresh)){
				add_reg(reg[j].obj, reg[j].reg[i]);
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
			tmp += reg[j].obj->nreg;
			reg[j].obj->reg[reg[j].obj->regc] = &reg[j];
			reg[j].obj->regc++;
		}
	}
}

void utils_object_draw(imgtype *img, Object *obj, uint32 nobjs, uint32 w)
{
	uint32 i, j;
	//rc = 97817; { //97848; {
	//rc = 1000; {
	for(i=0; i < nobjs; i++){
		for(j=0; j < obj[i].nreg; j++){
			region_draw(img, obj[i].reg[j], w);
		}
	}
	//printf("count = %d\n", count);
}

void utils_object_draw1(imgtype *img, Region *reg, uint32 nregs, uint32 w)
{
	uint32 rowc, x, w1 = w<<1, yx, rc, count = 0 ;
	//rc = 97817; { //97848; {
	//rc = 1000; {
	for(rc=0; rc < nregs; rc++){
	//for(rc=0; rc < 266; rc++){
		//printf("%6d nrows = %5d npix = %5d x = %5d y = %5d c0 = %3d c1 = %3d c2 = %3d c3 = %3d\n",
		//		rc, reg[rc].nrows, reg[rc].npix ,reg[rc].row[0]->x, reg[rc].row[0]->y, reg[rc].ac[0], reg[rc].ac[1], reg[rc].ac[2], reg[rc].ac[3] );
		//if(reg[rc].npix < 4){ count++;
			region_draw(img, &reg[rc], w);
			//for(x=0; x < reg[rc].neic; x++) region_draw(img, reg[rc].reg[x], w);
		//}
	}
	printf("count = %d\n", count);
}
