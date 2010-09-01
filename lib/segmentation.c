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
	reg->nrows = 0; reg->rowc = 0; reg->npixs = 0; reg->neic = 0; reg->nneis = 0; reg->obj = NULL;	//reg->row[reg->nrows] = row;
}

static inline add_pixel(Region *reg, Row *row, imgtype *img, uint32 yx, uint32 w)
{
	row->length += 2;
	reg->ac[0] += img[yx]; reg->ac[1] += img[yx+1]; reg->ac[2] += img[yx+w]; reg->ac[3] += img[yx+w+1];
	reg->npixs++;
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

void seg_regions(imgtype *img, Region *reg, Row *row, Row **prow, Region **preg, uint32 w, uint32 h, uint32 theresh, uint32 *nrows, uint32 *nregs, uint32 *npreg)
{
	uint32 y, h1 = ((h>>1)<<1)*w, x, w1 = w<<1, yx;
	uint32 df1[4], df2[4], left=0, regc =0, rowc = 0, pregc = 0, rc, tmp = 0, dfl, dft;
	y=0; x=0;
	yx = y+x;
	new_region(&reg[regc], &row[rowc], img, yx, w);
	new_row(&row[rowc], &reg[regc], x, y);
	add_pixel(&reg[regc], &row[rowc], img, yx, w);
	prow[x] = &row[rowc];
	for(x=2; x < w; x+=2){
		yx = y+x;
		if(check(img, prow[x-2]->reg, yx, w, theresh, &dfl))
		{
			add_pixel(prow[x-2]->reg, prow[x-2], img, yx, w);
			prow[x] = prow[x-2];
			img[yx] = img[yx-2]; img[yx+1] = img[yx-1]; img[yx+w] = img[yx+w-2]; img[yx+w+1] = img[yx+w-1];
		} else {
			//printf("%5d %p x = %d y = %d l = %d\n", rowc, &row[rowc], row[rowc].x, row[rowc].y, row[rowc].length);
			rowc++; regc++;
			new_region(&reg[regc], &row[rowc], img, yx, w);
			new_row(&row[rowc], &reg[regc], x, y);
			add_pixel(&reg[regc], &row[rowc], img, yx, w);

			left_neighborhood(&row[rowc], prow, preg, &pregc, x);
			prow[x] = &row[rowc];
		}
	}
	//printf("%5d %p x = %d y = %d l = %d\n", rowc, &row[rowc], row[rowc].x, row[rowc].y, row[rowc].length);
	for(y=w1; y < h1; y+=w1) {
		x=0;
		yx = y+x;
		if(check(img, prow[x]->reg, yx, w, theresh, &dft))
		{
			rowc++;
			new_row(&row[rowc], prow[x]->reg, x, y);
			add_pixel(prow[x]->reg, &row[rowc], img, yx, w);
			prow[x] = &row[rowc];
			img[yx] = img[yx-w1]; img[yx+1] = img[yx+1-w1]; img[yx+w] = img[yx+w-w1]; img[yx+w+1] = img[yx+w+1-w1];
		} else {
			rowc++; regc++;
			new_region(&reg[regc], &row[rowc], img, yx, w);
			new_row(&row[rowc], &reg[regc], x, y);
			add_pixel(&reg[regc], &row[rowc], img, yx, w);

			top_neighborhood(&row[rowc], prow, preg, &pregc, x);
			prow[x] = &row[rowc];

		}
		for(x=2; x < w; x+=2){
			yx = y+x;
			left =  check(img, prow[x-2]->reg, yx, w, theresh, &dfl);
			left += check(img, prow[x]->reg, yx, w, theresh, &dft)<<1;

			switch(left){
				case 0 : {
					rowc++; regc++;
					new_region(&reg[regc], &row[rowc], img, yx, w);
					new_row(&row[rowc], &reg[regc], x, y);
					add_pixel(&reg[regc], &row[rowc], img, yx, w);

					left_neighborhood(&row[rowc], prow, preg, &pregc, x);
					top_neighborhood(&row[rowc], prow, preg, &pregc, x);
					prow[x] = &row[rowc];
					break;
				}
				case 1 : {
					add_pixel(prow[x-2]->reg, prow[x-2], img, yx, w);

					top_neighborhood(&row[rowc], prow, preg, &pregc, x);
					prow[x] = prow[x-2];
					img[yx] = img[yx-2]; img[yx+1] = img[yx-1]; img[yx+w] = img[yx+w-2]; img[yx+w+1] = img[yx+w-1];
					break;
				}
				case 2 : {
					rowc++;
					new_row(&row[rowc], prow[x]->reg, x, y);
					add_pixel(prow[x]->reg, &row[rowc], img, yx, w);

					left_neighborhood(&row[rowc], prow, preg, &pregc, x);
					top_neighborhood(&row[rowc], prow, preg, &pregc, x);
					prow[x] = &row[rowc];
					img[yx] = img[yx-w1]; img[yx+1] = img[yx+1-w1]; img[yx+w] = img[yx+w-w1]; img[yx+w+1] = img[yx+w+1-w1];
					break;
				}
				case 3 : {
					if(dfl < dft){
						add_pixel(prow[x-2]->reg, prow[x-2], img, yx, w);

						top_neighborhood(&row[rowc], prow, preg, &pregc, x);
						prow[x] = prow[x-2];
						img[yx] = img[yx-2]; img[yx+1] = img[yx-1]; img[yx+w] = img[yx+w-2]; img[yx+w+1] = img[yx+w-1];
					} else {
						if(prow[x-2]->reg != prow[x]->reg){
							rowc++;
							new_row(&row[rowc], prow[x]->reg, x, y);
							add_pixel(prow[x]->reg, &row[rowc], img, yx, w);

							left_neighborhood(&row[rowc], prow, preg, &pregc, x);
							top_neighborhood(&row[rowc], prow, preg, &pregc, x);
							prow[x] = &row[rowc];
						} else {
							add_pixel(prow[x-2]->reg, prow[x-2], img, yx, w);

							top_neighborhood(&row[rowc], prow, preg, &pregc, x);
							prow[x] = prow[x-2];
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
	uint32 rowc, yx, x;
	for(rowc=0; rowc < reg->nrows; rowc++){
		for(x=reg->row[rowc]->x; x < (reg->row[rowc]->x + reg->row[rowc]->length); x+=2){
			yx = reg->row[rowc]->y + x;
			//img[yx] 	= reg->ac[0];
			//img[yx+1] 	= reg->ac[1];
			//img[yx+w] 	= reg->ac[2];
			//img[yx+w+1] = reg->ac[3];
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
	obj->c[0] = reg->c[0];
	obj->c[1] = reg->c[1];
	obj->c[2] = reg->c[2];
	obj->c[3] = reg->c[3];
	obj->npixs = reg->npixs;
	reg->obj = obj;
	obj->nregs = 1;
	obj->regc = 0;
}

static inline add_reg(Object *obj, Region *reg)
{
	obj->c[0] = (obj->c[0]*obj->npixs + reg->c[0]*reg->npixs)/(obj->npixs + reg->npixs);
	obj->c[1] = (obj->c[1]*obj->npixs + reg->c[1]*reg->npixs)/(obj->npixs + reg->npixs);
	obj->c[2] = (obj->c[2]*obj->npixs + reg->c[2]*reg->npixs)/(obj->npixs + reg->npixs);
	obj->c[3] = (obj->c[3]*obj->npixs + reg->c[3]*reg->npixs)/(obj->npixs + reg->npixs);
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

void seg_objects(Object *obj, Region *reg,  Region **preg, uint32 nregs, uint32 *nobjs, uint32 theresh)
{
	uint32 j=0, i, oc=0, tmp = 0, ck;
	new_obj(&obj[oc], &reg[j]); oc++;

	for(j=1; j < nregs; j++){
		for(i=0; i < reg[j].nneis; i++){
			ck = 0;
			if(reg[j].reg[i]->obj != NULL && check_neighbor(&reg[j], reg[j].reg[i], theresh)){
				add_reg(reg[j].reg[i]->obj, &reg[j]); ck++;
				break;
			}
		}
		if(!ck)  new_obj(&obj[oc], &reg[j]); oc++;
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
		}
	}
}

void seg_objects_draw(imgtype *img, Object *obj, uint32 nobjs, uint32 w)
{
	uint32 i, j;
	//rc = 97817; { //97848; {
	//i = 1; {
	for(i=0; i < nobjs; i++){
		//printf("obj = %p nregs = %d\n", &obj[i], obj[i].nregs);
		for(j=0; j < obj[i].nregs; j++){
			//printf("reg = %p \n", &obj[i].reg[j]);
			region_draw(img, obj[i].reg[j], obj[i].c, w);
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
			region_draw(img, &reg[rc], reg[rc].c, w);
			//for(x=0; x < reg[rc].neic; x++) region_draw(img, reg[rc].reg[x], w);
		//}
	}
	printf("count = %d\n", count);
}
