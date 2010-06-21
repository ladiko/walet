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

static inline uint32 find_bits(uint32 b)
{
	uint32 bits;
	bits = (b>>16) ? 16 : 0;
	bits = (b>>(bits+=8)) ? bits : bits - 8;
	bits = (b>>(bits+=4)) ? bits : bits - 4;
	bits = (b>>(bits+=2)) ? bits : bits - 2;
	bits = (b>>(bits+=1)) ? bits : bits - 1;
	return bits;
}

static inline uint32  division(uint32 a, uint32 b, uint32 bits)
{
        uint32 rest = b - (1<<bits);
        return (a>>bits)*(div_look_up[bits > 8 ? rest>>(bits-8) : rest<<(8-bits)] + 1024)>>11;
}

static inline uint32 divide(uint32 n, uint32 d)
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

static inline void set_freq(imgtype img, uint32 *d, uint32 bits)
/*! \fn void update_friq(imgtype img, uint32 *d, uint32 *c, const uit32 num)
	\brief Update cummulative frequency.
	\param img	 	The next data.
	\param d		The pointer to array of distribution probabilities of the message.
	\param bits		The numbers of bits per  symbols.
*/
//	d[0]	|0 |1 |2 |3 |4 |5 |6 |7 |8 |9 |10|11|12|13|14|15|
//	d[1]	|0    |1    |2   *|3    |4    |5    |6    |7    |
//	d[2]	|0          |1          |2          |3          |
//	d[3]	|0                    * |1                      |
{
	uint32 i, ind;
	for(i=0, ind=0; i<bits; ind+=(1<<(bits-i)), i++) {
		d[ind + (img>>i)]++;
		//if(ind + (img>>i) >= 1024) printf(" set_freq = %d ", ind + (img>>i));
	}
		//printf("img = %d  d[%d + %d] = %d\n", img, ind, img>>i, d[ind + (img>>i)]);

}

static inline uint32 get_freq(imgtype img, uint32 *d, uint32 bits)
/*! \fn void update_friq(imgtype img, uint32 *d, uint32 *c, const uit32 num)
	\brief Update cummulative frequency.
	\param img	 	The next data.
	\param d		The pointer to array of distribution probabilities of the message.
	\param bits		The numbers of bits per  symbols.
*/
{
	uint32 i, c=0, bit = bits-1, ind = (1<<(bits+1))-4;
	//for(i=0; i<bits; i++) if((img>>(bit-i))%2) c+=d[bit-i][(img>>(bit-i))-1];
	for(i=0; i<bits; i++, ind-=(1<<(i+1))) if((img>>(bit-i))%2) {
		c+=d[ind +(img>>(bit-i))-1];
		//if(ind +(img>>(bit-i))-1 >= 1024) printf(" get_freq = %d ", ind +(img>>(bit-i))-1);
	}
	return c;
}

static inline uint32 get_freq_cum(uint32 cum, uint32 *d, uint32 bits, uint32 *f, uint32 *cf)
/*! \fn void update_friq(imgtype img, uint32 *d, uint32 *c, const uit32 num)
	\brief Update cumulative frequency.
	\param cum	 	The next cumulitive frequency.
	\param d		The pointer to array of distribution probabilities of the message.
	\param bits		The numbers of bits per  symbols.
	\param f		The return frequency.
	\param cf		The return cumulative frequency
*/
{
	uint32 i, ind = (1<<(bits+1))-4, j=0, cu=cum;
	for(i=1; i<bits; i++, ind-=(1<<i)) {
		//printf("cum = %d cu = %d d[%d + %d] = %d\n",cum, cu, ind, j, d[ind + j]);
		if(cu >= d[ind + j]) { cu-= d[ind + j]; j = (j<<1) + 2; }
		else j<<=1;
	}
	//printf("d[%d] = %d\n",j, d[j] );
	if(cu >= d[j]) { (*cf) = cum-cu+d[j]; (*f) = d[j+1]; return j+1; }
	else { (*cf) = cum-cu; (*f) = d[j];  return j; }
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
	uint32 i, j, k=0 , cu, bits;
	uint32 half = 1<<(a_bits-1);
	int im;

	memset(d, 0, sizeof(uint32)*num*2);
	for(i=0; i<num; i++) set_freq(i, d, q_bits);
	//printf("num = %d\n", num*2);

	//Encoder setup
	range = top; low = 0; j=0;
	bits = find_bits(num);
	//for(bits=0; ;bits++) if(!(num>>=1)) break;
	//printf("sz = %d bits = %d\n", sz, bits);

	//printf(" top = %16LX bot = %16LX\n", top, bot);
	for(i=0; i<size; i++) {
		im = q[img[i] + half];

		if((1<<(bits+1)) & sz) bits++;
		range = division(range, sz, bits);
		//uint32  range1;
		//range = divid(range, sz);
		//range = range/sz;
		//printf("bits = %d\n", bits);
		//printf(" %2d  sz = %u range = %8X range1 = %8X diff = %8X\n", i, sz, range, range1, range - range1);

		low1 = low;
		cu = get_freq(im, d, q_bits);
		low += range*cu;
		//if(i<5)	printf("%5d low = %8X low1 = %8X range = %8X  out = %4d img = %4d\n", i, low, low1, range, im, img[i]);
		range = range*d[im];
		//printf("%8d low = %8X range = %8X  out = %4d img = %4d\n", i, low, range, im, img[i]);
		//if(low < low1) { if(buff[j-1] == 0X0000) printf("buff = %4X %4X", buff[j-1], buff[j-2]); for(k=1; !(++buff[j-k]); k++); if(k>1) printf("k = %d j = %d buff = %4X %4X\n", k, j, buff[j-1], buff[j-2]);}
		if(low < low1) { for(k=1; !(++buff[j-k]); k++);}
		//if(low < low1) { for(k=1; !(++buff[j-k]); k++); printf("k = %d\n", k);}
		//printf(" %2d  img = %3d f = %4u cu = %4u low = %16LX range = %16LX\n", i, im, d[im], cu, low, range);
		if(i != size1){
			while(range <= bot) {
				buff[j++]  = (low>>24);
				//if(j<5) printf("low = %d j=%d  buff[0] = %2X\n",low>>24, j, buff[j-1]);
				range <<= sh;
				low <<= sh;
				//if(j==1) printf("encoder img = %4X\n", buff[0]);
				//printf("                                  low = %16LX range = %16LX\n", low, range);
			}
			set_freq(im, d, q_bits);
			sz++;
		}
	}
	//printf("%5d low = %8X \n", i, low);
	buff[j++] = (low>>24);
	buff[j++] = (low>>16) & 0xFF;
	buff[j++] = (low>>8)  & 0xFF;
	buff[j++] = low & 0xFF;
	//printf("size = %d Encoder size  = %d first = %4X end = %4X img = %d %d %d %d \n", size, j<<1, buff[0], buff[j-1], img[0], img[1], img[2], img[3]);
	//return (j<<1);
	//for(i=j; i>j-5; i--) printf("buff[%d] = %2X\n",i, buff[i]);
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
	uint32 i, j, del = a_bits-q_bits, sub = (1<<del)>>1;
	uint32 half = num>>1, bits, bits1;
	int dif, fin;
	//uint16 *buff = (uint16*) buf;
	//uchar  *buff = buf;

	//Initial setup
	memset(d, 0, sizeof(uint32)*num*2);
	for(i=0; i<num; i++) set_freq(i, d, q_bits);

	//Ecoder setup
	range = top;
	//printf("bits = %d\n", bits);
	//for(bits=0; ;bits++) if(!(num>>=1)) break;
	//for(bits=1; ;bits++) if(!(num>>bits)) break;
	//bits--;
	bits = find_bits(num);
	//printf("bits = %d\n", bits);

	//printf("decoder img = %4X\n", buff[0]);
	//for(i=0; i<5; i++) printf("buff[%d] = %2X\n",i, buff[i]);
	low =  ((uint32)buff[0]<<24) | ((uint32)buff[1]<<16) | ((uint32)buff[2]<<8) | (uint32)buff[3];
	//low =  ((uint32)buff[0]<<16) | (uint32)buff[1];
	j=4;
	//printf("range = %16LX low = %16LX\n", range, low);
	for(i=0; i<size; i++) {
		while(range <= bot) {
			range <<=sh;
			low = (low<<sh) | (uint32)buff[j++];
		}
		if((1<<(bits+1)) & sz) bits++;
		range = division(range, sz, bits);
		//printf("bits = %d\n", bits);
		//range = range/sz;
		//range = divid(range, sz);
		//out2 = low/range;
		//uint32 out2 = divid(low, range);
		out = division(low, range, find_bits(range));
		out = low/range;
		//for(bits1=1; ;bits1++) if(!(range>>bits1)) break;
		//printf("range = %16LX bits1 = %d\n", range, bits1);
		//out = division(low, range, bits1-1);
		//printf("bits1 = %d\n", bits);

		out1 =  get_freq_cum(out, d, q_bits, &f, &cf);
		if(img[i]-q[out1]) {//printf("i=%d ", i);
		//if(out2-out) {//printf("i=%d ", i);
			if(i<1)printf("%5d low = %8X range = %8X out = %8X out1 = %3d img = %4d q[out1] = %4d diff = %d out2 = %8x\n",
					i, low, range, out, out1, img[i], q[out1], img[i]-q[out1], out2);
			return 0;
		}
		low -= cf*range;
		range = range*f;
		set_freq(out1, d, q_bits);
		sz++;
		img[i] = q[out1];
		//dif = out1 - half;
		//fin = dif > 0 ? (dif<<del)+sub : (dif < 0 ? -((-dif)<<del)-sub : 0);

	}
	//printf("size = %d Decoder size  = %d first = %4X end = %4X img = %d %d %d %d\n", size, j<<1, buff[0], buff[j-1], img[0], img[1], img[2], img[3]);
	//printf("size = %d\n", j);
	return j;
}

