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

static inline uint32 get_cum(imgtype img, uint32 **c, uint32 bits)
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

uint32  range_encoder(imgtype *img, uint32 *d, uint32 size, uint32 a_bits , uint32 q_bits, uchar *buff, int *q)
/*! \fn uint32  range_encoder(imgtype *img, uint32 *distrib, const uint32 size, const uchar bits)
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
	bits = find_msb_bit(num);
	tmp = 1<<(bits+1);

	//Start encoding
	for(i=0; i<size; i++) {
		im = q[img[i] + half];

		if(tmp & sz) { bits++; tmp = 1<<(bits+1); }
		range = division(range, sz, bits);
		//range = range/sz;
		low1 = low;
		cu = get_cum(im, c, q_bits);
		//cu = get_freq(im, d, q_bits);
		//set_freq(im, d, q_bits);
		low += range*cu;
		//if(i<5)	printf("%5d low = %8X low1 = %8X range = %8X  out = %4d img = %4d\n", i, low, low1, range, im, img[i]);
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
	return j;
}

uint32  range_decoder(imgtype *img, uint32 *d, uint32 size, uint32 a_bits , uint32 q_bits, uchar *buff, int *q)
/*! \fn uint32  range_encoder(imgtype *img, uint32 *distrib, const uint32 size, const uchar bits)
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
	range = top;
	bits = find_msb_bit(num);
	tmp = 1<<(bits+1); j=4;
	low =  ((uint32)buff[0]<<24) | ((uint32)buff[1]<<16) | ((uint32)buff[2]<<8) | (uint32)buff[3];

	// Start decoding
	for(i=0; i<size; i++) {
		while(range <= bot) {
			range <<=sh;
			low = (low<<sh) | (uint32)buff[j++];
		}
		if(tmp & sz) { bits++; tmp = 1<<(bits+1); }
		range = division(range, sz, bits);
		//range = range/sz;
		out = division(low, range, find_msb_bit(range));
		if((out+1)*range <= low) out++;
		out2 = low/range;
		out1 = get_pix(out, c, q_bits, &f, &cf);
		//if(img[i]-q[out1]) {
		if(out-out2) {
			printf("%5d low = %8X range = %8X out = %8X out1 = %3d img = %4d q[out1] = %4d diff = %d out2 = %8x\n",
					i, low, range, out, out1, img[i], q[out1], img[i]-q[out1], out2);
			return 0;
		}
		low -= cf*range;
		range = range*f;
		//set_freq(out1, d, q_bits);
		sz++;
		img[i] = q[out1];
	}
	return j;
}

