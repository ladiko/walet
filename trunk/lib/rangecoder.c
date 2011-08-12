#include <stdio.h>
#include <string.h>
#include <walet.h>
#include <rangecoder.h>

uint32 div_look_up[256] = {
                1016, 1008, 1000, 992,  984,  977,  969,
                961,  954,  947,  939,  932,  925,  917,  910,
                903,  896,  889,  882,  875,  868,  861,  855,
                848,  841,  835,  828,  822,  815,  809,  802,
                796,  790,  783,  777,  771,  765,  759,  753,
                747,  741,  735,  729,  723,  717,  712,  706,
                700,  694,  689,  683,  678,  672,  667,  661,
                656,  651,  645,  640,  635,  629,  624,  619,
                614,  609,  604,  599,  594,  589,  584,  579,
                574,  569,  564,  559,  555,  550,  545,  541,
                536,  531,  527,  522,  518,  513,  509,  504,
                500,  495,  491,  486,  482,  478,  473,  469,
                465,  461,  457,  452,  448,  444,  440,  436,
                432,  428,  424,  420,  416,  412,  408,  404,
                400,  396,  392,  389,  385,  381,  377,  374,
                370,  366,  363,  359,  355,  352,  348,  344,
                341,  337,  334,  330,  327,  323,  320,  316,
                313,  310,  306,  303,  299,  296,  293,  290,
                286,  283,  280,  276,  273,  270,  267,  264,
                261,  257,  254,  251,  248,  245,  242,  239,
                236,  233,  230,  227,  224,  221,  218,  215,
                212,  209,  206,  203,  200,  198,  195,  192,
                189,  186,  184,  181,  178,  175,  173,  170,
                167,  164,  162,  159,  156,  154,  151,  148,
                146,  143,  141,  138,  135,  133,  130,  128,
                125,  123,  120,  118,  115,  113,  110,  108,
                105,  103,  101,   98,   96,   93,   91,   89,
                86,   84,   82,   79,   77,   75,   72,   70,
                68,   65,   63,   61,   59,   57,   54,   52,
                50,   48,   45,   43,   41,   39,   37,   35,
                33,   30,   28,   26,   24,   22,   20,   18,
                16,   14,   12,   10,    8,    6,    4,    2, 0,
};

static inline uint32 find_msb_bit(uint32 b)
// Find most significant bit.
{
	uint32 bit=0;
	if(b>>16) bit = 16;
	if(!(b>>(bit+=8))) bit =  bit - 8;
	if(!(b>>(bit+=4))) bit =  bit - 4;
	if(!(b>>(bit+=2))) bit =  bit - 2;
	if(!(b>>(bit+=1))) bit =  bit - 1;
	return bit;
}

static inline uint32  division(uint32 a, uint32 b, uint32 bit)
//Fast division approximation.
{
        uint32 rest = b - (1<<bit);
        return (a>>bit)*(div_look_up[bit > 8 ? rest>>(bit-8) : rest<<(8-bit)] + 1024)>>11;
}

static inline uint32 divide(uint32 n, uint32 d)
//Slow division algorithm.
{
	uint32 i, k, q=0;
	for(k=0; !(0x80000000 & d); k++) d<<=1;
	k++;
	for(i=0; i<k; i++){
		if(n >= d) { n-=d; q<<=1; q|=1;}
		else	q<<=1;
		d>>=1;
	}
	return q;
}

static inline uint32 divide1(uint32 n, uint32 d)
//Slow division algorithm.
{
	uint32 i, k, q=0;
	k = 32 - find_msb_bit(d);
	d<<=k;
	for(i=0; i<k; i++){
		if(n >= d) { n-=d; q<<=1; q|=1;}
		else	q<<=1;
		d>>=1;
	}
	return q;
}

static inline uint32 nr_divide(uint32 n, uint32 d, uint32 bit1)
//Newton-Raphson  integer division algorithm.
{
	uint32  i, bit = bit1+1, qt, q= 0xFFFFFFFF>>bit, q1 = q;
	for(i = 0;i<10;i++) {
		if(q1) q1 = (((-d*q)>>bit)*q)>>(32-bit);
		else break;
		//printf("n = %8X d = %8X bit = %d d*q = %8X -d*q = %8X (-d*q)>>bit = %8X ((-d*q)>>bit)*q = %8X q = %8X q1 = %8X\n",
		//		n, d, bit, d*q, -d*q, (-d*q)>>bit, ((-d*q)>>bit)*q, q, q1);
		q += q1;
	}
	qt = q*(n>>(bit))>>(32-bit);
	if(n-qt*d > (d<<1) ) qt+=2;
	if(n-qt*d > (d<<1) ) qt+=2;
	return qt;
}

static inline uint32 nr_divide1(uint32 n, uint32 d, uint32 bit1)
//Newton-Raphson  integer division algorithm.
{
	uint32  i, bit = bit1+1, q , qt;
	q = division(0xFFFFFFFF, d, bit1);
	uint64 q1 = q;
	for(i=0;;i++) {
		if(q1) q1 = ((0x100000000-(uint64)d*(uint64)q)*(uint64)q)>>32;
		else break;
		//printf("%d n = %8X d = %8X bit = %d d*q = %16LX -d*q = %16LX  (-(uint64)d*(uint64)q)*(uint64)q) = %16LX q = %8X q1 = %16LX\n",
		//		i, n, d, bit, (uint64)d*(uint64)q, 0x100000000-(uint64)d*(uint64)q, (0x100000000-(uint64)d*(uint64)q)*(uint64)q, q, q1);
		q += q1;
	}
	//printf("%d ", i);
	qt = (uint64)q*(uint64)n>>32;
	if(n-qt*d >= d) qt+=1;
	//if(n-qt*d > d) qt+=1;
	return qt;
}

static inline void init_prob( uint32 *d, uint32 bits, uint32 **c)
//Init probability array.
{
	uint32 i, j, val, sz = 1<<bits, sz1 = 0, ind=0;
	memset(d, 0, sizeof(uint32)*sz<<1);
	for(j=0; j < bits; j++){
		ind+=sz1;
		c[j] = &d[ind];
		sz1 = sz>>j;
		val = 1<<j;
		for(i=0; i < sz1; i++) c[j][i]=val;
	}
}

static inline uint32 get_cum(uint16 img, uint32 **c, uint32 bits)
//Get the cumulative frequency and update array
{
	uint32 i, cf=0, ind;
	for(i=0; i < bits; i++) {
		ind = img>>i;
		if(ind&1) cf += c[i][ind-1];
		c[i][ind]++;
	}
	return cf;
}

static inline uint32 get_pix(uint32 cum, uint32 **c, uint32 bits, uint32 *f, uint32 *cf)
//Get the pixel and update array
{
	uint32 i, j=0 , cu = cum;
	for(i=bits-1; i ; i--) {
		if (cu >= c[i][j]) { cu -= c[i][j]; c[i][j+1]++; j = (j<<1)+2; }
		else { c[i][j]++; j<<=1; }
	}
	if(cu >= c[0][j]) { *cf = cum - cu + c[0][j]; *f = c[0][j+1]; c[0][j+1]++; return j+1;  }
	else { *cf = cum - cu; *f = c[0][j]; c[0][j]++; return j; }
}

/*
static inline void fill_prob( uint8 *img, uint32 *d, uint32 bits, uint32 a_bits, uint32 size, int *q)
//Init probability array.
{
	uint32 j, sz = 1<<bits, *c = &d[sz], half = 1<<(a_bits-1);
	//printf("bits = %d sz = %d half = %d  size = %d\n", bits, sz, half, size);
	//memset(d, 1, sizeof(uint32)*sz);
	for(j=0; j < sz; j++) d[j] = 1;
	for(j=0; j < size; j++) d[q[img[j] + half]]++;
	c[0] = 0;
	for(j=1; j < sz; j++) c[j] = c[j-1]+d[j-1];
	//for(j=0; j < sz; j++) printf("d[%d] = %d c[%d] = %d\n", j, d[j], j, c[j]);
}
*/

uint32 range_encoder(int16 *img, uint32 size, uint32 a_bits , uint32 q_bits, uint8 *buff, int *q, uint32 *d)
/*! \fn uint32  range_encoder(uint8 *img, uint32 *distrib, const uint32 size, const uint8 bits)
	\brief Range encoder.
    \param img	 	The pointer to encoding message data.
    \param d		The pointer to array of distribution probabilities of the message.
	\param size		The size of the  message
	\param a_bits	Bits per symbols befor quantization.
	\param q_bits	Bits per symbols after quantization.
	\param buff		The encoded output  buffer
	\retval			The encoded message size in byts .
*/
{
	uint32 num = (1<<q_bits), sz = num, sh = 8, size1 = size-1;
	uint32 top = 0xFFFFFFFF, bot = (top>>sh), low=0, low1=0, range;
	uint32 i, j, k=0 , cu, bits, tmp;
	uint32 half = 1<<(a_bits-1), *c[16];
	int im;

	//Encoder setup
	init_prob(d, q_bits, c);
	range = top; low = 0; j=0;
	//bits = find_msb_bit(num);
	//tmp = 1<<(bits+1);

	//Start encoding
	for(i=0; i<size; i++) {
		im = q[img[i] + half];
		range = range/sz;
		low1 = low;
		cu = get_cum(im, c, q_bits);
		low += range*cu;
		//if(i<100)	printf("%5d low = %8X low1 = %8X range = %8X  im = %4d img = %4d cu = %8X sz = %8X\n", i, low, low1, range, im, img[i], cu, sz);
		range = range*(d[im]-1);
		if(low < low1) { for(k=1; !(++buff[j-k]); k++);}
		if(i != size1){
			while(range <= bot) {
				buff[j++] = (low>>24);
				range <<= sh;
				low <<= sh;
			}
			sz++;
		}
	}
	buff[j++] = (low>>24);
	buff[j++] = (low>>16) & 0xFF;
	buff[j++] = (low>>8)  & 0xFF;
	buff[j++] = low & 0xFF;
	//make_distrib(img, d, size, a_bits, q_bits, q);
	return j;
}

uint32  range_decoder(int16 *img, uint32 size, uint32 a_bits , uint32 q_bits, uint8 *buff, int *q, uint32 *d)
/*! \fn uint32  range_encoder(uint8 *img, uint32 *distrib, const uint32 size, const uint8 bits)
	\brief Range decoder.
    \param img	 	The pointer to encoding message data.
    \param d		The pointer to array of distribution probabilities of the message.
	\param size		The size of the  message
	\param a_bits	Bits per symbols befor quantization.
	\param q_bits	Bits per symbols after quantization.
	\param buff		The encoded output  buffer
	\retval			The encoded message size in byts .
*/
{
	uint32 num = (1<<q_bits), sz = num, sum = 0, out, out1, out2, f, cf, sh = 8;
	uint32 top = 0xFFFFFFFF, bot = (top>>sh), low = 0, range;
	uint32 i, j, tmp;
	uint32 half = num>>1, bits, *c[16];
	int dif, fin;

	//Decoder setup
	init_prob(d, q_bits, c);
	range = top; j=4;
	//bits = find_msb_bit(num);
	//tmp = 1<<(bits+1);

	low =  ((uint32)buff[0]<<24) | ((uint32)buff[1]<<16) | ((uint32)buff[2]<<8) | (uint32)buff[3];

	// Start decoding
	for(i=0; i<size; i++) {
		while(range <= bot) {
			range <<=sh;
			low = (low<<sh) | (uint32)buff[j++];
		}
		range = range/sz;
		out = low/range;
		out1 = get_pix(out, c, q_bits, &f, &cf);
		//if(img[i]-q[out1]) {
		//if(out2-out!=0) {
		//if(i<100) printf("%5d low = %8X range = %8X out = %8X out2 = %8X out1 = %3d img = %4d q[out1] = %4d diff = %d out2 = %8X dif = %d\n",
		//			i, low, range, out, out1, out2, img[i], q[out1], img[i]-q[out1], out2, out2-out);
			//return 0;
		//}
		low -= cf*range;
		range = f*range;
		//set_freq(out1, d, q_bits);
		sz++;
		img[i] = q[out1];
	}
	return j;
}

static uint32 make_distrib1(int16 *img, uint32 *d, uint32 *cu, uint32 size, uint32 a_bits ,uint32 q_bits, int *q)
/*! \fn uint32  range_encoder(uint8 *img, uint32 *distrib, const uint32 size, const uint8 bits)
	\brief Range encoder.
    \param img	 	The pointer to encoding message data.
    \param d		The pointer to array of distribution probabilities of the message.
	\param size		The size of the  message
	\param a_bits	Bits per symbols befor quantization.
	\param q_bits	Bits per symbols after quantization.
	\param buff		The encoded output  buffer
*/
{
	uint32 i, num = 1<<q_bits, half = 1<<(a_bits-1), max, maxi, msb , sum = 0, sum1, sum2 = 0, bits;
	//uint32 *cu = &d[num];
	//for(i=0; i < num; i++) printf("%d ", q[i]);
	//printf("\n");
	memset(d, 0, num);
	//Fill distribution array after quantization
	for(i=0; i < size; i++) d[q[img[i] + half]]++;
	//for(i=0; i < num; i++) printf("%d ", d[i]);
	//printf("img = %d half = %d q = %d d = %d", img[i], half, q[img[i] + half], d[q[img[i] + half]]);

	//Find sum and max element of array
	max = d[0]; sum += d[0];
	for(i=1; i<num; i++)  {
		if(max < d[i]) { max = d[i]; maxi = i; }
		sum += d[i];
	}
	bits = find_msb_bit(sum)+1;
	sum1 = 1<<bits;
	msb = (sum1<<8)/sum;
	//Make distribution with cumulative frequency power of 2
	for(i=0; i<num; i++) {
		//printf(" d[%d] = %d ", i, d[i]);
		if(d[i]) {
			d[i] = d[i]*msb>>8;
			//msb = find_msb_bit(d[i]);
			//d[i] = (d[i] == (1<<msb)) ? 1<<msb : 1<<msb+1;
		}
		//printf(" d[%d] = %d ", i, d[i]);
		sum2 += d[i];
	}
	//printf("\n");
	d[maxi] += sum1 - sum2;
	//for(i=0; i<num; i++)  { sum3 += d[i]; }
	cu[0] = 0;
	for(i=1; i<=num; i++)  {
		cu[i] = cu[i-1] + d[i-1];
		//printf(" cu[%d] = %d ", i, cu[i]);
	}
	//printf("\n size = %d sum = %d sum1 = %d sum2 = %d cu[i-1] = %d bits = %d\n", size, sum, sum1, sum2, cu[i-1], bits);
	return bits;
}

static void  dist_quant(uint32 *din, uint32 *dout,  uint32 a_bits, uint32 q_bits)
/// \fn static uint32*  dist_quant(uint32 *din, uint32 *dout,  uint32 a_bits, uint32 q_bits)
///	\brief Make distribution of probabilities after quantization
///	\param din 			Pointer to input distribution
/// \param dout			Pointer to output distribution
///	\param a_bits 		Bits for representation of input distribution
/// \param q_bits		Bits for representation of output distribution
{
	int i, j, st = (a_bits - q_bits);
	int range = 1<<(a_bits-1), half = 1<<(q_bits-1);

	for(j=(1-(1<<st)); j< (1<<st); j++) dout[half] += din[range + j];
	for(i=1; i < half; i++){
		for(j= (i<<st); j< ((i+1)<<st); j++) {
			dout[half + i] += din[range + j];
			dout[half - i] += din[range - j];
		}
	}
	dout[0] = din[0];
	//return dout;
	//for(i=-range; i < range; i++) printf("%d  ", q[range+i]);
}


static uint32 make_distrib(uint32 *d, uint32 *dq, uint32 *cu, uint32 a_bits, uint32 q_bits, uint32 *msb, uint32 *buff)
/*! \fn uint32  range_encoder(uint8 *img, uint32 *distrib, const uint32 size, const uint8 bits)
	\brief Range encoder.
    \param img	 	The pointer to encoding message data.
    \param d		The pointer to array of distribution probabilities of the message.
	\param size		The size of the  message
	\param a_bits	Bits per symbols befor quantization.
	\param q_bits	Bits per symbols after quantization.
	\param buff		The encoded output  buffer
*/
{
	uint32 i, j=0, num = 1<<q_bits, max, maxi, pw, sum = 0, sum1, sum2 = 0, bits, b, half, full;
	int *d1 = buff, *d2 = &buff[num];
	int cn = 0, co = 0, t1 = 0, t2 = 0, nl, nr;
	memset(d2, 0, sizeof(uint32)*num);
	//Fill distribution array after quantization
	if(a_bits == q_bits) for(i=0; i < num; i++) dq[i] = d[i];
	else dist_quant(d, dq, a_bits, q_bits);


	//for(i=0; i<size; i++) d[q[img[i] + half]]++;
	//Find sum and max element of array
	//printf("num = %d\n", num);
	max = dq[0]; sum += dq[0];
	for(i=1; i<num; i++)  {
		if(max < dq[i]) { max = dq[i]; maxi = i; }
		sum += dq[i];
	}
	//The max bits for distribution representation
	bits = find_msb_bit(sum)+1;
	sum1 = 1<<bits;
	pw = (sum1<<8)/sum;
	//Make distribution with total cumulative frequency power of 2
	for(i=0; i<num; i++) {
		dq[i] = dq[i]*pw>>8;
		sum2 += dq[i];
	}
	dq[maxi] += sum1 - sum2;
	*msb = find_msb_bit(dq[maxi])+1;
	//printf("sum = %d new = %d dq[max] = %d msb = %d\n", sum, sum2, dq[maxi], 1<<*msb);

	//Make distribution with each frequency power of 2
	for(i=0; i < num; i++) d1[i] = dq[i];
	for(i=0; i < num; i++){
		if(d1[i]){
			b = find_msb_bit(d1[i]);
			full = 1<<(b+1);
			half = 1<<b;
			if(d1[i] != half){
				//np numbers of pixels to get of give then d[i] should be power of 2
				nl = d1[i] - half;
				nr = full - d1[i];
				if(nl > nr){
					//Should get pixels from neighbors
					 d1[i] += nr; d1[i+1] -= nr; d2[i] = nr;
				} else {
					//Should give pixels to neighbors
					d1[i] -= nl; d1[i+1] += nl; d2[i] = -nl;
				}
				printf("dq = %d d = %d d2 = %d d+ = %d half = %d nl = %d nr = %d\n", dq[i], d1[i], d2[i], d1[i+1], half, nl, nr);
			} else  printf("dq = %d d = %d d2 = %d d+ = %d half = %d nl = %d nr = %d\n", dq[i], d1[i], d2[i], d1[i+1], half, nl, nr);
		}// else d2[i] = 0;
	}
	//for(i=0; i < num; i++) printf("%d  ", dq[i]);
	//printf("num = %d\n", num);
	co = 0; cn = 0; t1 = 0; t2 = 0;
	for(i=0; i < num; i++) {  t1+=dq[i]; t2+=d1[i]; if(dq[i] == 1) co++; if(d1[i] == 1) cn++; } //printf("%d %d  ", dq[i], d1[i]);
	printf("\n dq = %d d1 = %d dq 1 = %d d1 1 = %d\n", t1, t2, co, cn);
	for(i=0; i < num; i++) printf("%d ", d1[i]);
	printf("\n");
	for(i=0; i < num; i++) printf("%d ", d2[i]);
	//printf("\n size = %d sum = %d sum1 = %d sum2 = %d cu[i-1] = %d bits = %d\n", size, sum, sum1, sum2, cu[i-1], bits);
	return bits;
}

static void  make_cum_freq(uint32 *d, uint32 *cu, uint32 q_bits)
{
	uint32 i, num = 1<<q_bits;
	cu[0] = 0;
	for(i=1; i<=num; i++)  {
		cu[i] = cu[i-1] + d[i-1];
	}
}


static inline write_bits_pos(uint8 *buff, uint32 *poz, uint32 st, uint32 bits)
{
	uint32 p, i, r, mask;
	while(1){
		i = *poz>>3; p = *poz&7; r = 8-p; r = bits > r ? r : bits;
		mask = (1<<r) - 1;
		if(!p) buff[i] = 0;
		buff[i] |= (mask & st)<<p;
		if(r - bits) { *poz += r; bits = bits - r; st>>=r;}
		else { *poz += bits; break; }
	}
}

static inline read_bits_pos(uint8 *buff, uint32 *poz, uint32 *st, uint32 bits)
{
	uint32 p, i, r, mask, sd = 0;
	*st = 0;
	while(1){
		i = *poz>>3; p = *poz&7; r = 8-p; r = bits > r ? r : bits;
		mask = (1<<r) - 1 ;
		*st |= (((buff[i] & (mask<<p))>>p)<<sd);
		if(r - bits) { *poz += r; bits = bits - r; sd += r;}
		else { *poz += bits; break; }
	}
}

uint32 write_distrib(uint32 *d, uint32 q_bits, uint32 msb, uint32 sz, uint8 *buff)
{
	uint32 i, il, ir, num = 1<<q_bits, poz = 0;

	//Write msb
	write_bits_pos(buff, &poz, msb, 8);
	//Write size of distribution
	write_bits_pos(buff, &poz, sz, 8);

	//Write left zero counts
	for(i=0; ; i++) if(d[i]) break;
	if(i < 256) { il = i; 	write_bits_pos(buff, &poz, il, 8);}
	else  		{ il = 255; write_bits_pos(buff, &poz, il, 8);}

	//Write right  zero counts
	for(i=0; ; i++) if(d[num-1-i]) break;
	if(i < 256) { ir = i; 	write_bits_pos(buff, &poz, ir, 8);}
	else  		{ ir = 255;	write_bits_pos(buff, &poz, ir, 8);}

	//printf("msb = %d sz = %d il = %d ir = %d \n", msb, sz, il, ir);
	//Write all another
	for(i=il; i < num - ir; i++) {
		write_bits_pos(buff, &poz, d[i], msb);
		//printf("poz = %d ", poz);
	}
	//printf("\n");
	return (poz>>3) + 1;
}

static uint32 read_distrib(uint32 *d, uint32 q_bits, uint32 *sz, uint8 *buff)
{
	uint32 i, il, ir, num = 1<<q_bits, poz = 0, msb;

	read_bits_pos(buff, &poz, &msb, 8);
	read_bits_pos(buff, &poz, sz, 8);
	read_bits_pos(buff, &poz, &il, 8);
	read_bits_pos(buff, &poz, &ir, 8);

	//printf("msb = %d sz = %d il = %d ir = %d \n", msb, *sz, il, ir);
	for(i=0; i < il; i++) d[i] = 0;

	for(i=il; i < num - ir; i++) {
		read_bits_pos(buff, &poz, &d[i], msb);
		//printf("poz = %d ", poz);
	}
	for(i=num - ir; i < num; i++) d[i] = 0;
	//printf("\n");
	return (poz>>3) + 1;
}

static inline uint32 get_cum_f(uint32 out, uint32 *cu, uint32 size)
{
	uint32 i, j;
	//if(!out) return 0;
	for(i = size>>1, j = i; ; ){
		//if(test>10) break;
		//if (bool) printf("size = %d out = %d cu[%d] = %d j = %d \n", size, out, i, cu[i], j);
		if(out >= cu[i]) {
			if(out < cu[i+1]) {
				//if (bool) printf("cu[%d] = %d\n", i, cu[i]);
				return i;
			}
			else { j >>=1; i+=j;}
		}
		else { j = (j==0) ? 1 : j>>1; i-=j;}
		//else { j >>=1; i-=j;}
	}
}

static inline uint32 get_cum_f1(uint32 out, uint32 *cu, uint32 i, uint32 j)
{
	//uint32 i, j;
	//if(!out) return 0;
	for(; ;){
		//if(test>10) break;
		//if (bool) printf("size = %d out = %d cu[%d] = %d j = %d \n", size, out, i, cu[i], j);
		if(out >= cu[i]) {
			if(out < cu[i+1]) {
				//if (bool) printf("cu[%d] = %d\n", i, cu[i]);
				return i;
			}
			else { j>>=1; i+=j; }
		}
		else { j = (j==0) ? 1 : j>>1; i-=j;}
		//else { j >>=1; i-=j;}
	}
}

uint32  range_encoder1(int16 *img, uint32 *d, uint32 size, uint32 a_bits , uint32 q_bits, uint8 *buff, int *q, uint32 *buff1)
/*! \fn uint32  range_encoder(uint8 *img, uint32 *distrib, const uint32 size, const uint8 bits)
	\brief Range encoder.
    \param img	 	The pointer to encoding message data.
    \param d		The pointer to array of distribution probabilities of the message.
	\param size		The size of the  message
	\param a_bits	Bits per symbols befor quantization.
	\param q_bits	Bits per symbols after quantization.
	\param buff		The encoded output  buffer
	\retval			The encoded message size in byts .
*/
{
	uint32 num = (1<<q_bits), sh = 8, size1 = size-1, sz;
	uint32 top = 0xFFFFFFFF, bot = (top>>sh), low=0, low1=0, range;
	uint32 i, j, k=0 , bits, tmp, msb;
	uint32 half = 1<<(a_bits-1), *dq = buff1, *cu = &buff1[num];
	int im;

	//Encoder setup
	sz = make_distrib(d, dq, cu, a_bits, q_bits, &msb, &buff1[num<<1]);
	make_cum_freq(dq, cu, q_bits);
	//printf("finesh make_distrib sz = %d num = %d\n", sz, num);

	tmp = write_distrib(dq, q_bits, msb, sz, buff);
	//printf("write = %d \n", tmp);
	//for(i=0; i < num; i++) printf("%d ", dq[i]);
	//printf("\n");

	buff = &buff[tmp];

	//j = (tmp&7) ? (tmp>>3) + 1 : (tmp>>3);
	//printf("dist_size = %d bits %d byts\n", tmp, j);
	j=0;

	//init_prob(d, q_bits, c);
	range = top; low = 0;

	//for(i=0; i < num; i++) printf("%d ", q[i]);
	//printf("\n");
	//for(i=0; i < num; i++) printf("%d ", d[i]);
	//printf("\n");
	//Start encoding
	for(i=0; i<size; i++) {
		im = q[img[i] + half];
		//if(i<10) printf("img[%d] = %d  half = %d sz = %d im = %d cu[im] = %d d[im] = %d dq[im] = %d\n", i, img[i], half, sz, im, cu[im], d[im], dq[im]);

		range = range>>sz;
		//cu = get_cum(im, c, q_bits);
		low1 = low;
		low += range*cu[im];
		//if(i<10)	printf("%5d low = %8X low1 = %8X range = %8X  out = %4d img = %4d\n", i, low, low1, range, im, img[i]);
		//range = range*d[im];
		range = range*dq[im];
		//update_dist(im, d, ds, ab, sw, size_ab, &get, &set, &swc);
		if(low < low1) { for(k=1; !(++buff[j-k]); k++);}
		if(i != size1){
			while(range <= bot) {
				buff[j++] = (low>>24);
				range <<= sh;
				low <<= sh;
			}
		}
	}
	buff[j++] = (low>>24);
	buff[j++] = (low>>16) & 0xFF;
	buff[j++] = (low>>8)  & 0xFF;
	buff[j++] = low & 0xFF;
	return j+tmp;
}

uint32  range_decoder1(int16 *img, uint32 *d, uint32 size, uint32 a_bits , uint32 q_bits, uint8 *buff, int *q, uint32 *buff1)
/*! \fn uint32  range_encoder(uint8 *img, uint32 *distrib, const uint32 size, const uint8 bits)
	\brief Range decoder.
    \param img	 	The pointer to encoding message data.
    \param d		The pointer to array of distribution probabilities of the message.
	\param size		The size of the  message
	\param a_bits	Bits per symbols befor quantization.
	\param q_bits	Bits per symbols after quantization.
	\param buff		The encoded output  buffer
	\retval			The encoded message size in byts .
*/
{
	uint32 num = (1<<q_bits), sz, sum = 0, out, out1, out2, f, cf, sh = 8;
	uint32 top = 0xFFFFFFFF, bot = (top>>sh), low = 0, range;
	uint32 i, j, tmp, im, cu_sz;
	uint32 half = num>>1, bits, *dq = buff1, *cu = &buff1[num];
	int dif, fin;

	//Decoder setup
	tmp = read_distrib(dq, q_bits, &sz, buff);
	//printf("read = %d \n", tmp);
	//for(i=0; i < num; i++) printf("%d ", dq[i]);
	//printf("\n");
	make_cum_freq(dq, cu, q_bits);

	//for(i=0; i < num; i++) printf("%d ", cu[i]);
	//printf("\n");
	//sz = read_distrib(buff, d, q_bits);
	//sz = read_distrib1(buff, d, q_bits);
	buff = &buff[tmp];

	range = top; j=4;

	low =  ((uint32)buff[0]<<24) | ((uint32)buff[1]<<16) | ((uint32)buff[2]<<8) | (uint32)buff[3];

	// Start decoding
	for(i=0; i<size; i++) {
		while(range <= bot) {
			range <<=sh;
			low = (low<<sh) | (uint32)buff[j++];
		}
		range = range>>sz;
		//out = divide1(low, range);
		out = low/range;
		//im = get_cum_f(out, cu, cu_sz);
		//im = get_cum_f(out, cu, num);
		im = get_cum_f1(out, cu, half, half);
		//im = half;
		//if(img[i] != q[im])
		//if(i >=0 && i< 5)
		//printf("%5d low = %8X low = %8X range = %8X range = %8X range = %8X out = %8d im = %8d  img = %4d q[im] = %4d d = %8d  cu = %8d diff = %d\n",
		//			i, low, low-cu[im]*range, range<<sz, range, d[im]*range, out, im, img[i], q[im], d[im], cu[im], img[i]-q[im]);
		low -= cu[im]*range;
		range = dq[im]*range;
		//set_freq(out1, d, q_bits);
		//sz++;
		img[i] = q[im];
	}
	return j+tmp;
}

