#include <walet.h>
#include <stdio.h>


static inline void assign_1(uchar *f, uchar *s)
{
	*f = *s;
}

static inline void assign_3(uchar *f, uchar *s)
{
	f[0] = s[0]; f[1] = s[1]; f[2] = s[2];
}

static inline void assign_4(uchar *f, uchar *s)
{
	f[0] = s[0]; f[1] = s[1]; f[2] = s[2]; f[3] = s[3];
}

static inline uint32  check_1(uchar *f, uchar *s, uchar theresh)
{
	return (abs(f[0] - s[0]) > theresh);
}

static inline uint32  check_3(uchar *f, uchar *s, uchar theresh)
{
	return (abs(f[0] - s[0]) > theresh || abs(f[1] - s[1]) > theresh || abs(f[2] - s[2]) > theresh);
}

static inline uint32  check_4(uchar *f, uchar *s, uchar theresh)
{
	return (abs(f[0] - s[0]) > theresh || abs(f[1] - s[1]) > theresh || abs(f[2] - s[2]) > theresh || abs(f[3] - s[3]) > theresh);
}


void utils_row_seg(imgtype *img, Row *rows, uint32 *col, uint32 w, uint32 h, uint32 theresh)
{
	uint32 y, h1 = h>>1, x, w1 = w<<1, yx, y1, tot=0, ar=0, ag1=0, ag2=0, ab=0, old;

	int rc =-1;
	col[0] = 0;
	//rows[0].length = 1;
	for(y=0, y1=0; y1 < h1; y+=w1, y1++) {
		for(x=0; x < w; x+=2){
			yx = y+x;
			if(x){
				//if( abs(((rows[rc].c[0]+rows[rc].c[1]+rows[rc].c[2]+rows[rc].c[3])>>2) -
				//		((img[yx]+img[yx+1]+img[yx+w]+img[yx+w+1])>>2) ) > theresh )
				if( abs(rows[rc].c[0] - img[yx]    ) > theresh ||
					abs(rows[rc].c[1] - img[yx+1]  ) > theresh ||
					abs(rows[rc].c[2] - img[yx+w]  ) > theresh ||
					abs(rows[rc].c[3] - img[yx+w+1]) > theresh)
					{
					rows[rc].c[0] = ar/(rows[rc].length>>1);
					rows[rc].c[1] = ag1/(rows[rc].length>>1);
					rows[rc].c[2] = ag2/(rows[rc].length>>1);
					rows[rc].c[3] = ab/(rows[rc].length>>1);
					++rc;
					rows[rc].length = 2; rows[rc].x = x; rows[rc].y = y1;
					rows[rc].c[0] = img[yx]; rows[rc].c[1] = img[yx+1]; rows[rc].c[2] = img[yx+w]; rows[rc].c[3] = img[yx+w+1];
					ar = img[yx]; ag1 = img[yx+1]; ag2 = img[yx+w]; ab = img[yx+w+1];
				} else {
					rows[rc].length+=2;
					ar += img[yx]; ag1 += img[yx+1]; ag2 += img[yx+w]; ab += img[yx+w+1];

				}
			} else {
				if(rc > -1){
					rows[rc].c[0] = ar/(rows[rc].length>>1);
					rows[rc].c[1] = ag1/(rows[rc].length>>1);
					rows[rc].c[2] = ag2/(rows[rc].length>>1);
					rows[rc].c[3] = ab/(rows[rc].length>>1);
				}
				++rc;
				rows[rc].length = 2; rows[rc].x = 0; rows[rc].y = y1;
				rows[rc].c[0] = img[yx]; rows[rc].c[1] = img[yx+1]; rows[rc].c[2] = img[yx+w]; rows[rc].c[3] = img[yx+w+1];
				ar = img[yx]; ag1 = img[yx+1]; ag2 = img[yx+w]; ab = img[yx+w+1];
			}
		}
		col[y1] = rc+1;
	}
	printf("Rows  = %d \n", rc);
}

void utils_row_seg_ver(imgtype *img, uint32 w, uint32 h, uint32 theresh)
{
	uint32 y, h1 = h>>1, x, w1 = w<<1, yx, y1, tot=0, ar=0, ag1=0, ag2=0, ab=0, old;

	for(x=0; x < w; x+=2){
		for(y=0, y1=0; y1 < h1; y+=w1, y1++) {
			yx = y+x;
			if(y){
				if( abs(ar	- img[yx]    ) > theresh ||
					abs(ag1	- img[yx+1]  ) > theresh ||
					abs(ag2	- img[yx+w]  ) > theresh ||
					abs(ab	- img[yx+w+1]) > theresh)
				{
					ar = img[yx]; ag1 = img[yx+1]; ag2 = img[yx+w]; ab = img[yx+w+1];
				} else{
					img[yx] = img[yx-w1];
					img[yx+1] = img[yx+1-w1];
					img[yx+w] = img[yx+w-w1];
					img[yx+w+1] = img[yx+w+1-w1];

				}
			} else {
				ar = img[yx]; ag1 = img[yx+1]; ag2 = img[yx+w]; ab = img[yx+w+1];
			}
		}
	}
}

void utils_2d_seg(imgtype *img, uint32 w, uint32 h, uint32 theresh)
{
	uint32 y, h1 = h>>1, x, w1 = w<<1, yx, y1, tot=0, ar=0, ag1=0, ag2=0, ab=0, old;
	uint32 df1[4], df2[4], left=0;
	y=0; y1=0;
	for(x=2; x < w; x+=2){
		yx = y+x;
		if( abs(img[yx-2]	- img[yx]    ) < theresh ||
			abs(img[yx-1] 	- img[yx+1]  ) < theresh ||
			abs(img[yx+w-2]	- img[yx+w]  ) < theresh ||
			abs(img[yx+w-1]	- img[yx+w+1]) < theresh)
		{
			img[yx] = img[yx-2];
			img[yx+1] = img[yx-1];
			img[yx+w] = img[yx+w-2];
			img[yx+w+1] = img[yx+w-1];
		}
	}
	for(y=w1, y1=1; y1 < h1; y+=w1, y1++) {
		x=0;
		yx = y+x;
		if( abs(img[yx-w1]		- img[yx]    ) < theresh ||
			abs(img[yx+1-w1] 	- img[yx+1]  ) < theresh ||
			abs(img[yx+w-w1]	- img[yx+w]  ) < theresh ||
			abs(img[yx+w+1-w1]	- img[yx+w+1]) < theresh)
		{
			img[yx] = img[yx-w1];
			img[yx+1] = img[yx+1-w1];
			img[yx+w] = img[yx+w-w1];
			img[yx+w+1] = img[yx+w+1-w1];
		}
		for(x=2; x < w; x+=2){
			yx = y+x;
			df1[0] = abs(img[yx-2]	- img[yx]    );
			df1[1] = abs(img[yx-1] 	- img[yx+1]  );
			df1[2] = abs(img[yx+w-2]- img[yx+w]  );
			df1[3] = abs(img[yx+w-1]- img[yx+w+1]);
			df2[0] = abs(img[yx-w1]		- img[yx]    );
			df2[1] = abs(img[yx+1-w1] 	- img[yx+1]  );
			df2[2] = abs(img[yx+w-w1]	- img[yx+w]  );
			df2[3] = abs(img[yx+w+1-w1]	- img[yx+w+1]);
			left = 0;
			left =  (df1[0] < theresh || df1[1] < theresh || df1[2] < theresh || df1[3] < theresh);
			left += (df2[0] < theresh || df2[1] < theresh || df2[2] < theresh || df2[3] < theresh)<<1;

			switch(left){
				case 0 : break;
				case 1 : {
					img[yx] = img[yx-2];
					img[yx+1] = img[yx-1];
					img[yx+w] = img[yx+w-2];
					img[yx+w+1] = img[yx+w-1];
					break;
				}
				case 2 : {
					img[yx] = img[yx-w1];
					img[yx+1] = img[yx+1-w1];
					img[yx+w] = img[yx+w-w1];
					img[yx+w+1] = img[yx+w+1-w1];
					break;
				}
				case 3 : {
					if(df1[0]+df1[1]+df1[2]+df1[3] > df2[0]+df2[1]+df2[2]+df2[3]){
						img[yx] = img[yx-w1];
						img[yx+1] = img[yx+1-w1];
						img[yx+w] = img[yx+w-w1];
						img[yx+w+1] = img[yx+w+1-w1];
					} else {
						img[yx] = img[yx-2];
						img[yx+1] = img[yx-1];
						img[yx+w] = img[yx+w-2];
						img[yx+w+1] = img[yx+w-1];

					}
					break;
				}
			}
		}
	}
}

void utils_row_seg_hor_left(imgtype *img, uint32 w, uint32 h, uint32 theresh)
{
	uint32 y, h1 = h>>1, x, w1 = w<<1, yx, y1, tot=0, ar=0, ag1=0, ag2=0, ab=0, old;

	for(y=0, y1=0; y1 < h1; y+=w1, y1++) {
		for(x=0; x < w; x+=2){
			yx = y+x;
			if(x){
				if( abs(ar	- img[yx]    ) > theresh ||
					abs(ag1	- img[yx+1]  ) > theresh ||
					abs(ag2	- img[yx+w]  ) > theresh ||
					abs(ab	- img[yx+w+1]) > theresh)
				{
					ar = img[yx]; ag1 = img[yx+1]; ag2 = img[yx+w]; ab = img[yx+w+1];
				} else{
					img[yx] = img[yx-2];
					img[yx+1] = img[yx+1-2];
					img[yx+w] = img[yx+w-2];
					img[yx+w+1] = img[yx+w+1-2];

				}
			} else {
				ar = img[yx]; ag1 = img[yx+1]; ag2 = img[yx+w]; ab = img[yx+w+1];
			}
		}
	}
}

void utils_row_seg_hor_right(imgtype *img, uint32 w, uint32 h, uint32 theresh)
{
	uint32 y, h1 = h>>1,  w1 = w<<1, yx, y1, tot=0, ar=0, ag1=0, ag2=0, ab=0, old;
	int x;

	for(y=0, y1=0; y1 < h1; y+=w1, y1++) {
		for(x=w-1; x > -1; x-=2){
			yx = y+x;
			if(x != w-1){
				if( abs(ar	- img[yx]    ) > theresh ||
					abs(ag1	- img[yx+1]  ) > theresh ||
					abs(ag2	- img[yx+w]  ) > theresh ||
					abs(ab	- img[yx+w+1]) > theresh)
				{
					ar = img[yx]; ag1 = img[yx+1]; ag2 = img[yx+w]; ab = img[yx+w+1];
				} else{
					img[yx] = img[yx+2];
					img[yx+1] = img[yx+3];
					img[yx+w] = img[yx+w+2];
					img[yx+w+1] = img[yx+w+3];
				}
			} else {
				ar = img[yx]; ag1 = img[yx+1]; ag2 = img[yx+w]; ab = img[yx+w+1];
			}
		}
	}
}

void utils_row_seg1(imgtype *img, Row *rows, uint32 *col, uint32 w, uint32 h, uint32 theresh)
{
	uint32 y, h1 = h>>1, x, w1 = w<<1, yx, y1, tot=0, ar=0, ag1=0, ag2=0, ab=0, old;

	int rc =-1;
	col[0] = 0;
	//rows[0].length = 1;
	for(y=0, y1=0; y1 < h1; y+=w1, y1++) {
		for(x=0; x < w; x+=2){
			yx = y+x;
			if(x){
				//if( abs(((rows[rc].c[0]+rows[rc].c[1]+rows[rc].c[2]+rows[rc].c[3])>>2) -
				//		((img[yx]+img[yx+1]+img[yx+w]+img[yx+w+1])>>2) ) > theresh )
				if( abs(rows[rc].c[0] - img[yx]    ) > theresh ||
					abs(rows[rc].c[1] - img[yx+1]  ) > theresh ||
					abs(rows[rc].c[2] - img[yx+w]  ) > theresh ||
					abs(rows[rc].c[3] - img[yx+w+1]) > theresh)
					{
					++rc;
					rows[rc].length = 2; rows[rc].x = x; rows[rc].y = y1;
					rows[rc].c[0] = img[yx]; rows[rc].c[1] = img[yx+1]; rows[rc].c[2] = img[yx+w]; rows[rc].c[3] = img[yx+w+1];
				} else {
					rows[rc].length+=2;
					img[yx] = img[yx-2];
					img[yx+1] = img[yx+1-2];
					img[yx+w] = img[yx+w-2];
					img[yx+w+1] = img[yx+w+1-2];

				}
			} else {
				++rc;
				rows[rc].length = 2; rows[rc].x = 0; rows[rc].y = y1;
				rows[rc].c[0] = img[yx]; rows[rc].c[1] = img[yx+1]; rows[rc].c[2] = img[yx+w]; rows[rc].c[3] = img[yx+w+1];
			}
		}
		col[y1] = rc+1;
	}
	printf("Rows  = %d \n", rc);
}

void utils_row_seg2(imgtype *img, Row *rows, uint32 *col, uint32 w, uint32 h, uint32 theresh)
{
	uint32 y, h1 = h>>1, x, w1 = w<<1, yx, y1, tot=0, ar=0, ag1=0, ag2=0, ab=0, old;

	int rc =-1;
	col[0] = 0;
	//rows[0].length = 1;
	for(y=0, y1=0; y1 < h1; y+=w1, y1++) {
		for(x=0; x < w; x+=2){
			yx = y+x;
			if(x){
				//if( abs(((rows[rc].c[0]+rows[rc].c[1]+rows[rc].c[2]+rows[rc].c[3])>>2) -
				//		((img[yx]+img[yx+1]+img[yx+w]+img[yx+w+1])>>2) ) > theresh )
				if( abs(rows[rc].c[0] - img[yx]    ) > theresh ||
					abs(rows[rc].c[1] - img[yx+1]  ) > theresh ||
					abs(rows[rc].c[2] - img[yx+w]  ) > theresh ||
					abs(rows[rc].c[3] - img[yx+w+1]) > theresh)
					{
					++rc;
					rows[rc].length = 2; rows[rc].x = x; rows[rc].y = y1;
					rows[rc].c[0] = img[yx]; rows[rc].c[1] = img[yx+1]; rows[rc].c[2] = img[yx+w]; rows[rc].c[3] = img[yx+w+1];
				} else {
					rows[rc].length+=2;
					img[yx] = img[yx-2];
					img[yx+1] = img[yx+1-2];
					img[yx+w] = img[yx+w-2];
					img[yx+w+1] = img[yx+w+1-2];
				}
			} else {
				++rc;
				rows[rc].length = 2; rows[rc].x = 0; rows[rc].y = y1;
				rows[rc].c[0] = img[yx]; rows[rc].c[1] = img[yx+1]; rows[rc].c[2] = img[yx+w]; rows[rc].c[3] = img[yx+w+1];
			}
		}
		col[y1] = rc+1;
	}
	printf("Rows  = %d \n", rc);
}

void utils_row_seg_double(imgtype *img, Row *rows, uint32 *col, imgtype *buf, uint32 w, uint32 h, uint32 theresh)
{
	uint32 y, h1 = h>>1, w1 = w<<1, yx, y1, tot=0, ar=0, ag1=0, ag2=0, ab=0, old, pc1, pc2;
	int x, i, j;
	//imgtype* line1 = buf;
	//imgtype* line2 = &buf[w1];
	Point *poi1 = (Point*) buf, *poi2;
	int rc = 0;
	col[0] = 0;
	//rows[0].length = 1;

	for(y=0, y1=0; y1 < h1; y+=w1, y1++) {
		//Left to right scan
		pc1 = 0; pc2 = 0;
		for(x=0; x < w; x+=2){
			yx = y+x;
			if(x){
				if( abs(ar	- img[yx]    ) > theresh ||
					abs(ag1	- img[yx+1]  ) > theresh ||
					abs(ag2	- img[yx+w]  ) > theresh ||
					abs(ab	- img[yx+w+1]) > theresh)
				{
					ar = img[yx]; ag1 = img[yx+1]; ag2 = img[yx+w]; ab = img[yx+w+1];
					poi1[pc1].cl[0] = img[yx-2];
					poi1[pc1].cl[1] = img[yx-1];
					poi1[pc1].cl[2] = img[yx+w-2];
					poi1[pc1].cl[3] = img[yx+w-1];
					poi1[pc1].x = x;
					pc1++;
					//line1[x] = img[yx]; line1[x+1] = img[yx+1]; line1[x+w] = img[yx+w]; line1[x+w+1] = img[yx+w+1];
				} //else{
					//line1[x] = img[yx-2]; line1[x+1] = img[yx-1]; line1[x+w] = img[yx+w-2]; line1[x+w+1] = img[yx+w-1];

				//}
			} else {
				ar = img[yx]; ag1 = img[yx+1]; ag2 = img[yx+w]; ab = img[yx+w+1];
				//line1[x] = img[yx]; line1[x+1] = img[yx+1]; line1[x+w] = img[yx+w]; line1[x+w+1] = img[yx+w+1];
			}
			poi1[pc1].cl[0] = ar;
			poi1[pc1].cl[1] = ag1;
			poi1[pc1].cl[2] = ag2;
			poi1[pc1].cl[3] = ab;
			poi1[pc1].x = w-1;
			pc1++;
		}
		//Right to left scan
		poi2 = &poi1[pc1];
		for(x=w-1; x > -1; x-=2){
			yx = y+x;
			if(x != w-1){
				if( abs(ar	- img[yx]    ) > theresh ||
					abs(ag1	- img[yx+1]  ) > theresh ||
					abs(ag2	- img[yx+w]  ) > theresh ||
					abs(ab	- img[yx+w+1]) > theresh)
				{
					ar = img[yx]; ag1 = img[yx+1]; ag2 = img[yx+w]; ab = img[yx+w+1];
					poi2[pc2].cl[0] = img[yx+2];
					poi2[pc2].cl[1] = img[yx+3];
					poi2[pc2].cl[2] = img[yx+w+2];
					poi2[pc2].cl[3] = img[yx+w+3];
					poi2[pc2].x = x+2;
					pc2++;
					//line2[x] = img[yx]; line2[x-1] = img[yx-1]; line2[x+w] = img[yx+w]; line2[x+w-1] = img[yx+w-1];
				} //else{
					//line2[x] = img[yx+2]; line2[x-1] = img[yx+1]; line2[x+w] = img[yx+w+2]; line2[x+w-1] = img[yx+w+1];
				//}
			} else {
				ar = img[yx]; ag1 = img[yx+1]; ag2 = img[yx+w]; ab = img[yx+w+1];
				//line2[x] = img[yx]; line2[x-1] = img[yx-1]; line2[x+w] = img[yx+w]; line2[x+w-1] = img[yx+w-1];
			}
			poi2[pc2].cl[0] = ar;
			poi2[pc2].cl[1] = ag1;
			poi2[pc2].cl[2] = ag2;
			poi2[pc2].cl[3] = ab;
			poi2[pc2].x = 0;
			pc2++;
		}
		//Rows segmentation
		x=0; j = pc2-1;
		for(i=0; i < pc1; i++){
			 while(poi2[j].x < poi1[i].x-1)  j--;
			 if(poi1[i].x - poi2[j].x < 2) {
					rows[rc].x = x; x = poi1[i].x; rows[rc].length = poi1[i].x - rows[rc].x; rows[rc].y = y1;
					//rows[rc].c[0] = poi1[i].cl[0];
					//rows[rc].c[1] = poi1[i].cl[1];
					//rows[rc].c[2] = poi1[i].cl[2];
					//rows[rc].c[3] = poi1[i].cl[3];
					rows[rc].c[0] = poi2[j].cl[0];
					rows[rc].c[1] = poi2[j].cl[1];
					rows[rc].c[2] = poi2[j].cl[2];
					rows[rc].c[3] = poi2[j].cl[3];
					//rows[rc].c[0] = (poi1[i].cl[0] + poi2[j].cl[0])>>1;
					//rows[rc].c[1] = (poi1[i].cl[1] + poi2[j].cl[1])>>1;
					//rows[rc].c[2] = (poi1[i].cl[2] + poi2[j].cl[2])>>1;
					//rows[rc].c[3] = (poi1[i].cl[3] + poi2[j].cl[3])>>1;
					rc++;
			 }
		}
		col[y1] = rc;
		printf("y = %4d pl = %d pr = %d %5d x = %d length = %d c0 = %d c1 = %d c2 = %d c3 = %d\n",
				y1, pc1-1, pc2-1, rc-1, rows[rc-1].x, rows[rc-1].length, rows[rc-1].c[0], rows[rc-1].c[1], rows[rc-1].c[2], rows[rc-1].c[3]);
	}
	printf("Rows  = %d \n", rc);
}

uint32 utils_region_seg(Region *reg, Row *rows, uint32 *col, uint32 w, uint32 h, uint32 theresh)
{
	uint32 i, y, h1 = h>>1, x, rc, rcu, rcd;
	for(rc=0; rc < col[0]; rc++){
		rows[rc].reg = &reg[rc];
		reg[rc].nrows = 1;
		reg[rc].x = rows[rcd].x; reg[rc].y = 0;
		reg[rc].ac[0] = rows[rc].c[0];
		reg[rc].ac[1] = rows[rc].c[1];
		reg[rc].ac[2] = rows[rc].c[2];
		reg[rc].ac[3] = rows[rc].c[3];
		reg[rc].c[0] = rows[rc].c[0];
		reg[rc].c[1] = rows[rc].c[1];
		reg[rc].c[2] = rows[rc].c[2];
		reg[rc].c[3] = rows[rc].c[3];
	}
	for(y=1; y < h1; y++) {
		rcd = col[y-1]; rcu = y==1 ? 0 : col[y-2];
		//printf("     rc = %d rcd = %d rcu = %d \n", rc, col[y], col[y-1]);
		//printf("      x = %d l = %d\n", rows[col[y]-1].x, rows[col[y]].length);
		while(rcd < col[y])
		{
			if( abs(rows[rcu].reg->c[0] - rows[rcd].c[0]) > theresh ||
				abs(rows[rcu].reg->c[1] - rows[rcd].c[1]) > theresh ||
				abs(rows[rcu].reg->c[2] - rows[rcd].c[2]) > theresh ||
				abs(rows[rcu].reg->c[3] - rows[rcd].c[3]) > theresh)
			{
				//printf("> %4d rc = %d rcu = %d rcd = %d rows[rcu+1].x = %d rows[rcd+1].x = %d \n"
				//		, y, rc, rcu, rcd, rows[rcu].x + rows[rcu].length, rows[rcd].x + rows[rcd].length);
				if(rows[rcu].x + rows[rcu].length >= rows[rcd].x + rows[rcd].length) {
					rows[rcd].reg = &reg[rc];
					reg[rc].nrows = 1;
					reg[rc].x = rows[rcd].x; reg[rc].y = y;
					reg[rc].ac[0] = rows[rcd].c[0];
					reg[rc].ac[1] = rows[rcd].c[1];
					reg[rc].ac[2] = rows[rcd].c[2];
					reg[rc].ac[3] = rows[rcd].c[3];
					reg[rc].c[0] = rows[rcd].c[0];
					reg[rc].c[1] = rows[rcd].c[1];
					reg[rc].c[2] = rows[rcd].c[2];
					reg[rc].c[3] = rows[rcd].c[3];
					if(rows[rcu].x + rows[rcu].length == rows[rcd].x + rows[rcd].length) rcu++;
					rc++; rcd++;
				} else rcu++;
			} else {
				//printf("< %4d rc = %d rcu = %d rcd = %d rows[rcu+1].x = %d rows[rcd+1].x = %d \n"
				//		, y, rc, rcu, rcd, rows[rcu].x + rows[rcu].length, rows[rcd].x + rows[rcd].length);
				rows[rcd].reg = rows[rcu].reg;
				//printf("pointer = %p\n", rows[rcu].reg);
				rows[rcd].reg->nrows++;
				rows[rcd].reg->ac[0] += rows[rcd].c[0];
				rows[rcd].reg->ac[1] += rows[rcd].c[1];
				rows[rcd].reg->ac[2] += rows[rcd].c[2];
				rows[rcd].reg->ac[3] += rows[rcd].c[3];
				++rcd;
				if(rcd == col[y]) break;
				if(rcu != col[y-1]-1) while(rows[rcu].x + rows[rcu].length <= rows[rcd].x ) {
					//printf(" rcu - rcd = %d\n", rows[rcu].x + rows[rcu].length - rows[rcd].x);
					rcu++;
				}
			}
		}
	}
	for(i=0; i < rc; i++) {
		//printf("c0 = %d c1 = %d c2 = %d c3 = %d n = %d\n", reg[i].ac[0], reg[i].ac[1], reg[i].ac[2], reg[i].ac[3], reg[i].nrows);
		reg[i].c[0] = reg[i].ac[0] / reg[i].nrows;
		reg[i].c[1] = reg[i].ac[1] / reg[i].nrows;
		reg[i].c[2] = reg[i].ac[2] / reg[i].nrows;
		reg[i].c[3] = reg[i].ac[3] / reg[i].nrows;
		reg[i].rowc = 0;
		reg[i].nreg = 0;
	}
	printf("Regionts = %d\n", rc);
	return rc;
}

uint32 utils_region_seg1(Region *reg, Row *rows, uint32 *col, uint32 w, uint32 h, uint32 theresh)
{
	uint32 i, y, h1 = h>>1, x, rc, rcu, rcd;
	for(rc=0; rc < col[0]; rc++){
		rows[rc].reg = &reg[rc];
		reg[rc].nrows = 1;
		reg[rc].rowc = 0; reg[rc].nreg = 0;
		reg[rc].x = rows[rcd].x; reg[rc].y = 0;
		//reg[rc].ac[0] = rows[rc].c[0];
		//reg[rc].ac[1] = rows[rc].c[1];
		//reg[rc].ac[2] = rows[rc].c[2];
		//reg[rc].ac[3] = rows[rc].c[3];
		reg[rc].c[0] = rows[rc].c[0];
		reg[rc].c[1] = rows[rc].c[1];
		reg[rc].c[2] = rows[rc].c[2];
		reg[rc].c[3] = rows[rc].c[3];
	}
	for(y=1; y < h1; y++) {
		rcd = col[y-1]; rcu = y==1 ? 0 : col[y-2];
		//printf("     rc = %d rcd = %d rcu = %d \n", rc, col[y], col[y-1]);
		//printf("      x = %d l = %d\n", rows[col[y]-1].x, rows[col[y]].length);
		while(rcd < col[y])
		{
			if( abs(rows[rcu].reg->c[0] - rows[rcd].c[0]) > theresh ||
				abs(rows[rcu].reg->c[1] - rows[rcd].c[1]) > theresh ||
				abs(rows[rcu].reg->c[2] - rows[rcd].c[2]) > theresh ||
				abs(rows[rcu].reg->c[3] - rows[rcd].c[3]) > theresh)
			{
				//printf("> %4d rc = %d rcu = %d rcd = %d rows[rcu+1].x = %d rows[rcd+1].x = %d \n"
				//		, y, rc, rcu, rcd, rows[rcu].x + rows[rcu].length, rows[rcd].x + rows[rcd].length);
				if(rows[rcu].x + rows[rcu].length >= rows[rcd].x + rows[rcd].length) {
					rows[rcd].reg = &reg[rc];
					reg[rc].nrows = 1;
					reg[rc].rowc = 0; reg[rc].nreg = 0;
					reg[rc].x = rows[rcd].x; reg[rc].y = y;
					//reg[rc].ac[0] = rows[rcd].c[0];
					//reg[rc].ac[1] = rows[rcd].c[1];
					//reg[rc].ac[2] = rows[rcd].c[2];
					//reg[rc].ac[3] = rows[rcd].c[3];
					reg[rc].c[0] = rows[rcd].c[0];
					reg[rc].c[1] = rows[rcd].c[1];
					reg[rc].c[2] = rows[rcd].c[2];
					reg[rc].c[3] = rows[rcd].c[3];
					if(rows[rcu].x + rows[rcu].length == rows[rcd].x + rows[rcd].length) rcu++;
					rc++; rcd++;
				} else rcu++;
			} else {
				//printf("< %4d rc = %d rcu = %d rcd = %d rows[rcu+1].x = %d rows[rcd+1].x = %d \n"
				//		, y, rc, rcu, rcd, rows[rcu].x + rows[rcu].length, rows[rcd].x + rows[rcd].length);
				rows[rcd].reg = rows[rcu].reg;
				//printf("pointer = %p\n", rows[rcu].reg);
				rows[rcd].reg->nrows++;
				//rows[rcd].reg->ac[0] += rows[rcd].c[0];
				//rows[rcd].reg->ac[1] += rows[rcd].c[1];
				//rows[rcd].reg->ac[2] += rows[rcd].c[2];
				//rows[rcd].reg->ac[3] += rows[rcd].c[3];
				++rcd;
				if(rcd == col[y]) break;
				if(rcu != col[y-1]-1) while(rows[rcu].x + rows[rcu].length <= rows[rcd].x ) {
					//printf(" rcu - rcd = %d\n", rows[rcu].x + rows[rcu].length - rows[rcd].x);
					rcu++;
				}
			}
		}
	}
	/*
	for(i=0; i < rc; i++) {
		//printf("c0 = %d c1 = %d c2 = %d c3 = %d n = %d\n", reg[i].ac[0], reg[i].ac[1], reg[i].ac[2], reg[i].ac[3], reg[i].nrows);
		reg[i].c[0] = reg[i].ac[0] / reg[i].nrows;
		reg[i].c[1] = reg[i].ac[1] / reg[i].nrows;
		reg[i].c[2] = reg[i].ac[2] / reg[i].nrows;
		reg[i].c[3] = reg[i].ac[3] / reg[i].nrows;
		reg[i].rowc = 0;
		reg[i].nreg = 0;
	}*/
	printf("Regionts = %d\n", rc);
	return rc;
}

void utils_region_fill(Region *reg, Row *rows, Row **prow, uint32 *col, uint32 w, uint32 h)
{
	uint32 i, y, h1 = h>>1, x, rc=0, rcu, rcd, tmp=0;
	//printf("col[h1] = %d ", col[h1-1]);
	for(rc=0; rc < col[h1-1]; rc++) {
		//printf("rowc = %d ", rows[rc].reg->rowc);
		if(rows[rc].reg->rowc){
			rows[rc].reg->row[rows[rc].reg->rowc] = &rows[rc];
			rows[rc].reg->rowc++;
		}
		else  {
			rows[rc].reg->row = &prow[tmp];
			//printf("prow = %p ", rows[rc].reg->row);
			tmp+=rows[rc].reg->nrows;
			rows[rc].reg->row[rows[rc].reg->rowc] = &rows[rc];
			rows[rc].reg->rowc++;
			//printf(reg[rc].row[y])
		}
	}
}

static inline uint32 check_region(Region *reg, Region *reg1)
{
	int i, ret=0;
	for(i=0; i < reg->nreg; i++) if(reg->reg[i] == reg1) return 0;
	return 1;

}

void utils_region_neighbor(Row *rows, Region *reg, uint32 nreg, uint32 w, uint32 h)
{
	uint32 rowc, h1 = h>>1, x, w1 = w<<1, yx, rc = 0, y1;
	Row *prow;
	for(rc=0; rc < nreg; rc++){
		//printf("%6d nrows = %d x = %d y = %d\n", rc, reg[rc].nrows, reg[rc].x, reg[rc].y);
		for(rowc=0; rowc < reg[rc].nrows; rowc++){
			//printf("p = %p\n", reg[rc].row);
			//Left neighborhood check
			if(reg[rc].row[rowc]->x != 0) {
				prow = reg[rc].row[rowc] - sizeof(Row);
				if(reg[rc].nreg) {
					//Check if this pointer already exist
					if(check_region(&reg[rc], prow->reg)){
						reg[rc].reg[reg[rc].nreg] = prow->reg;
						reg[rc].nreg++;
					}
				}
				else {
					reg[rc].reg[reg[rc].nreg] = prow->reg;
					reg[rc].nreg++;
				}
			}
		}
	}
}

void utils_row_draw(imgtype *img, Row *rows, uint32 *col, uint32 w, uint32 h)
{
	uint32 y, h1 = h>>1, x, w1 = w<<1, yx, rc = 0, y1;
	for(y=0, y1=0; y1 < h1; y+=w1, y1++) {
		for(x=0; x < w; x+=2){
			yx = y+x;
			img[yx] 	= rows[rc].c[0];
			img[yx+1] 	= rows[rc].c[1];
			img[yx+w] 	= rows[rc].c[2];
			img[yx+w+1] = rows[rc].c[3];
			if(x == (rows[rc].x + rows[rc].length-2)) {
				//printf("x = %d rows[%d].x = %d rows[%d].length = %d ", x, rc, rows[rc].x, rc, rows[rc].length);
				++rc;
				//printf("x = %d rows[%d].x = %d rows[%d].length = %d ", x, rc, rows[rc].x, rc, rows[rc].length);
			}
		}
	}
}

void utils_region_draw(imgtype *img, Row *rows, uint32 *col, uint32 w, uint32 h)
{
	uint32 y, h1 = h>>1, x, w1 = w<<1, yx, rc = 0, y1;
	for(y=0, y1=0; y1 < h1; y+=w1, y1++) {
		for(x=0; x < w; x+=2){
			yx = y+x;
			img[yx] 	= rows[rc].reg->c[0];
			img[yx+1] 	= rows[rc].reg->c[1];
			img[yx+w] 	= rows[rc].reg->c[2];
			img[yx+w+1] = rows[rc].reg->c[3];
			if(x == (rows[rc].x + rows[rc].length-2)){
				//printf("x = %d rows[%d].x = %d rows[%d].length = %d ", x, rc, rows[rc].x, rc, rows[rc].length);
				++rc;
				//printf("x = %d rows[%d].x = %d rows[%d].length = %d ", x, rc, rows[rc].x, rc, rows[rc].length);
			}
		}
	}
}

void utils_region_draw1(imgtype *img, Region *reg, uint32 nreg, uint32 w, uint32 h)
{
	uint32 rowc, h1 = h>>1, x, w1 = w<<1, yx, rc = 0, y1;
	//rc = 97817; { //97848; {
	//rc = 97848; {
	//for(rc=0; rc < 97818; rc++){
	for(rc=0; rc < nreg; rc++){
		//printf("%6d nrows = %d x = %d y = %d\n", rc, reg[rc].nrows, reg[rc].x, reg[rc].y);
		//if(reg[rc].nrows < 2){
		for(rowc=0; rowc < reg[rc].nrows; rowc++){
			//printf("p = %p\n", reg[rc].row);
			for(x=reg[rc].row[rowc]->x; x < (reg[rc].row[rowc]->x + reg[rc].row[rowc]->length); x+=2){
				yx = ((reg[rc].row[rowc]->y)*w<<1) + x;
				img[yx] 	= reg[rc].c[0];
				img[yx+1] 	= reg[rc].c[1];
				img[yx+w] 	= reg[rc].c[2];
				img[yx+w+1] = reg[rc].c[3];
			}
		//}
		}
	}
}

