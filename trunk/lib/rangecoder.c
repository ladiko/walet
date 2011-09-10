/**
* File:          $RCSfile: rangecoder.c,v $
*
* Revision:      $Revision: 1.41 $
* Last edited:   $Date: 2005/10/18 22:01:50 $
* Author:        $Author: pm $
*/

/** \file rangecoder.c
    \brief Rande coder and decoder library

    There are tree type of of range coder:\n
    1. Adaptive range coder, not need any probability distribution information;\n
    2. Non adaptive range coder, need probability distribution array for coding;\n
    3. Special range coder, free of any division and multiplication operation, may\n
    use only for wavelet subband compression and need probability distribution\n
    array with each probability equal power of 2, and sum of distribution should\n
    be equal power of 2.\n
    This 32 bits version on range coder support maximum  subband size less than 2^24 (16 Mpixeles),\n
    the total image size should  be less than 2^26 (64 Mpixeles).
*/

#include <stdio.h>
#include <string.h>
#include <walet.h>
#include <rangecoder.h>

/** \brief Find most significant bit of \a b.
    \param b 	The value.
    \retval		The most significant bit position.
*/
static inline uint32 find_msb_bit(uint32 b)
{
	uint32 bit=0;
	if(b>>16) bit = 16;
	if(!(b>>(bit+=8))) bit =  bit - 8;
	if(!(b>>(bit+=4))) bit =  bit - 4;
	if(!(b>>(bit+=2))) bit =  bit - 2;
	if(!(b>>(bit+=1))) bit =  bit - 1;
	return bit;
}

/** \brief Create cumulative probability array for adaptive coding.
    \param d	The probability distribution .
    \param bits	The bits per pixel.
    \param c	The auxiliary array for cumulative frequency update and calculation.
*/
static inline void init_prob( uint32 *d, uint32 bits, uint32 **c){
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

/** \brief Get the cumulative frequency and update array.
    \param img	The pointer to encoding data.
    \param c 	The auxiliary array for cumulative frequency update and calculation.
    \param bits	The bits per pixel.
    \retval		The cumulative frequency.
*/
static inline uint32 get_cum(uint16 img, uint32 **c, uint32 bits){
	uint32 i, cf=0, ind;
	for(i=0; i < bits; i++) {
		ind = img>>i;
		if(ind&1) cf += c[i][ind-1];
		c[i][ind]++;
	}
	return cf;
}

/** \brief Get the symbol from cumulative frequency.
    \param cum	The cumulative frequency.
    \param c 	The auxiliary array for cumulative frequency update and calculation.
    \param bits	The bits per pixel.
    \param f	The frequency of symbol.
    \param cf	The cumulative frequency of symbol.
    \retval		The symbol value.
*/
static inline uint32 get_pix(uint32 cum, uint32 **c, uint32 bits, uint32 *f, uint32 *cf){
//

	uint32 i, j=0 , cu = cum;
	for(i=bits-1; i ; i--) {
		if (cu >= c[i][j]) { cu -= c[i][j]; c[i][j+1]++; j = (j<<1)+2; }
		else { c[i][j]++; j<<=1; }
	}
	if(cu >= c[0][j]) { *cf = cum - cu + c[0][j]; *f = c[0][j+1]; c[0][j+1]++; return j+1;  }
	else { *cf = cum - cu; *f = c[0][j]; c[0][j]++; return j; }
}

/** \brief Adaptive range encoder.
    \param img	 	The pointer to encoding data.
 	\param size		The size of the  input data.
	\param a_bits	Bits per symbols befor quantization.
	\param q_bits	Bits per symbols after quantization.
	\param buff		The encoded output  buffer
    \param q		The pointer to quantization array.
    \param d		The pointer to array of probability distribution .
	\retval			The encoded message size in byts .
*/
uint32 range_encoder_ad(int16 *img, uint32 size, uint32 a_bits , uint32 q_bits, uint8 *buff, int *q, uint32 *d)
{
	uint32 num = (1<<q_bits), sz = num, sh = 8, size1 = size-1;
	uint32 top = 0xFFFFFFFF, bot = (top>>sh), low=0, low1=0, range;
	uint32 i, j, k=0 , cu, bits, tmp;
	uint32 half = 1<<(a_bits-1), *c[16];
	int im;

	//Encoder setup
	init_prob(d, q_bits, c);
	range = top; low = 0; j=0;

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
	return j;
}

/** \brief Adaptive range decoder.
    \param img	 	The pointer to encoding data.
 	\param size		The size of the  input data.
	\param a_bits	Bits per symbols befor quantization.
	\param q_bits	Bits per symbols after quantization.
	\param buff		The encoded output  buffer
    \param q		The pointer to quantization array.
    \param buff1	The pointer to temporal buffer.
	\retval			The encoded message size in byts .
*/
uint32  range_decoder_ad(int16 *img, uint32 size, uint32 a_bits , uint32 q_bits, uint8 *buff, int *q, uint32 *buff1)
{
	uint32 num = (1<<q_bits), sz = num, sum = 0, out, out1, out2, f, cf, sh = 8;
	uint32 top = 0xFFFFFFFF, bot = (top>>sh), low = 0, range;
	uint32 i, j, tmp;
	uint32 half = num>>1, bits, *c[16], *d = buff1;
	int dif, fin;

	//Decoder setup
	init_prob(d, q_bits, c);
	range = top; j=4;

	// Start decoding
	low =  ((uint32)buff[0]<<24) | ((uint32)buff[1]<<16) | ((uint32)buff[2]<<8) | (uint32)buff[3];
	for(i=0; i<size; i++) {
		while(range <= bot) {
			range <<=sh;
			low = (low<<sh) | (uint32)buff[j++];
		}
		range = range/sz;
		out = low/range;
		out1 = get_pix(out, c, q_bits, &f, &cf);
		//if(i<100) printf("%5d low = %8X range = %8X out = %8X out2 = %8X out1 = %3d img = %4d q[out1] = %4d diff = %d out2 = %8X dif = %d\n",
		//			i, low, range, out, out1, out2, img[i], q[out1], img[i]-q[out1], out2, out2-out);
		low -= cf*range;
		range = f*range;
		sz++;
		img[i] = q[out1];
	}
	return j;
}

/** \brief Make distribution of probabilities after quantization
	\param din 			The input distribution
	\param dout			The output distribution
	\param a_bits 		Bits for representation of input distribution
	\param q_bits		Bits for representation of output distribution
*/
static void  dist_quant(uint32 *din, uint32 *dout,  uint32 a_bits, uint32 q_bits)
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
}

/** \brief Make distribution with sum equal power of 2
	\param din 		The input distribution.
	\param dout		The output distribution.
	\param a_bits	Bits per symbols before quantization.
	\param q_bits	Bits per symbols after quantization.
	\param msb		The maximum bits for output distribution representation.
	\retval			The log2 of distribution sum.
*/
static uint32 dist_tot_pow_2(uint32 *din, uint32 *dout, uint32 a_bits, uint32 q_bits, uint32 *msb)
{
	uint32 i, num = 1<<q_bits, max, maxi, pw, sum = 0, sum1, sum2 = 0, bits;
	//Fill distribution array after quantization
	if(a_bits == q_bits) for(i=0; i < num; i++) dout[i] = din[i];
	else dist_quant(din, dout, a_bits, q_bits);

	max = dout[0]; sum += dout[0];
	for(i=1; i<num; i++)  {
		if(max < dout[i]) { max = dout[i]; maxi = i; }
		sum += dout[i];
	}
	//The max bits for distribution representation
	bits = find_msb_bit(sum)+1;
	sum1 = 1<<bits;
	pw = (sum1<<8)/sum;
	//Make distribution with total cumulative frequency power of 2
	for(i=0; i<num; i++) {
		dout[i] = dout[i]*pw>>8;
		sum2 += dout[i];
	}
	dout[maxi] += sum1 - sum2;
	*msb = find_msb_bit(dout[maxi])+1;
	//printf("sum = %d new = %d dout[max] = %d msb = %d\n", sum, sum2, dout[maxi], 1<<*msb);

	return bits;
}

/** \brief Make cumulative frequency array.
	\param d 		The input distribution of probability array.
	\param cu		The output cumulative frequency.
	\param sz		The size of distribution.
*/
static void cum_freq(uint32 *d, uint32 *cu, uint32 sz)
{
	uint32 i;
	cu[0] = 0;
	for(i=1; i<=sz; i++)  {
		cu[i] = cu[i-1] + d[i-1];
	}
}

/** \brief Write /a bits from /a st to the /a buff to the /a pos position.
	\param buff 	The output buffer.
	\param pos		The bits from begin of the /a buff .
	\param st		The input buffer.
	\param bits		The number of bits to write.
*/
static inline void write_bits_pos(uint8 *buff, uint32 *pos, uint32 st, uint32 bits)
{
	uint32 p, i, r, mask;
	while(1){
		i = *pos>>3; p = *pos&7; r = 8-p; r = bits > r ? r : bits;
		mask = (1<<r) - 1;
		if(!p) buff[i] = 0;
		buff[i] |= (mask & st)<<p;
		if(r - bits) { *pos += r; bits = bits - r; st>>=r;}
		else { *pos += bits; break; }
	}
}

/** \brief Read /a bits to /a st from the /a buff from the /a pos position.
	\param buff 	The input buffer.
	\param pos		The bits from begin of the /a buff .
	\param st		The output buffer.
	\param bits		The number of bits to read.
*/
static inline void read_bits_pos(uint8 *buff, uint32 *pos, uint32 *st, uint32 bits)
{
	uint32 p, i, r, mask, sd = 0;
	*st = 0;
	while(1){
		i = *pos>>3; p = *pos&7; r = 8-p; r = bits > r ? r : bits;
		mask = (1<<r) - 1 ;
		*st |= (((buff[i] & (mask<<p))>>p)<<sd);
		if(r - bits) { *pos += r; bits = bits - r; sd += r;}
		else { *pos += bits; break; }
	}
}

/** \brief Write /a d distribution of probability to the /a buff buffer.
	\param d 		The input distribution of probability.
	\param sz		The size of distribution.
	\param msb		The maximum bits for distribution representation.
	\param szb		The log2 of total quantity of the distribution.
	\param buff		The output buffer.
	\retval			The write bytes.
*/
static uint32 write_dist(uint32 *d, uint32 sz, uint32 msb, uint32 szb, uint8 *buff)
{
	uint32 i, il, ir, poz = 0;

	//Write msb
	write_bits_pos(buff, &poz, msb, 8);
	//Write size of distribution
	write_bits_pos(buff, &poz, szb, 8);

	//Write left zero counts
	for(i=0; ; i++) if(d[i]) break;
	if(i < 256) { il = i; 	write_bits_pos(buff, &poz, il, 8);}
	else  		{ il = 255; write_bits_pos(buff, &poz, il, 8);}

	//Write right  zero counts
	for(i=0; ; i++) if(d[sz-1-i]) break;
	if(i < 256) { ir = i; 	write_bits_pos(buff, &poz, ir, 8);}
	else  		{ ir = 255;	write_bits_pos(buff, &poz, ir, 8);}

	//printf("msb = %d sz = %d il = %d ir = %d \n", msb, sz, il, ir);
	//Write all another
	for(i=il; i < sz - ir; i++) {
		write_bits_pos(buff, &poz, d[i], msb);
		//printf("poz = %d ", poz);
	}
	//printf("\n");
	return (poz>>3) + 1;
}

/** \brief Read /a d distribution of probability from the /a buff buffer.
	\param d 		The output distribution of probability.
	\param sz		The size of distribution.
	\param szb		The log2 of total quantity of the distribution.
	\param buff		The input buffer.
	\retval			The read bytes.
*/
static uint32 read_dist(uint32 *d, uint32 sz, uint32 *szb, uint8 *buff)
{
	uint32 i, il, ir, poz = 0, msb;

	read_bits_pos(buff, &poz, &msb, 8);
	read_bits_pos(buff, &poz, szb, 8);
	read_bits_pos(buff, &poz, &il, 8);
	read_bits_pos(buff, &poz, &ir, 8);

	//printf("msb = %d sz = %d il = %d ir = %d \n", msb, *sz, il, ir);
	for(i=0; i < il; i++) d[i] = 0;

	for(i=il; i < sz - ir; i++) {
		read_bits_pos(buff, &poz, &d[i], msb);
		//printf("poz = %d ", poz);
	}
	for(i=sz - ir; i < sz; i++) d[i] = 0;
	//printf("\n");
	return (poz>>3) + 1;
}

/**	\brief Get array index from cumulative frequency, it's universal algorithm logarithm search.
    \param in	 	Input cumulative frequency.
 	\param cu		Cumulative frequency array.
	\param i		Tha half of cumulative frequency array size.
	\param j		The start interval from half should be the same as half.
	\retval			The array index.
*/
static inline uint32 get_cum_log(uint32 in, uint32 *cu, uint32 i, uint32 j)
{
	for(; ; ){
		//if(test>10) break;
		//printf("out = %d cu[%d] = %d cu[%d] = %d j = %d \n", out, i, cu[i], i+1, cu[i+1], j);
		if(in >= cu[i]) {
			if(in < cu[i+1]) {
				//printf("0\n");
				return i;
			}
			else { j >>=1; i+=j;} // printf("+");}
		}
		else { j = (j==0) ? 1 : j>>1; i-=j;}// printf("-");}
	}
}

/**	\brief Get array index from cumulative frequency, it's a fast algorithm for laplace distribution.
    \param in	 	Input cumulative frequency.
 	\param cu		Cumulative frequency array.
	\param i		Tha half of cumulative frequency array size.
	\param j		The start interval from half should be 1.
	\retval			The array index.
*/
static inline uint32 get_cum_f(uint32 in, uint32 *cu, uint32 i, uint32 j)
{

	if(in >= cu[i]){
		if(in < cu[i+1]) return i;
		i+=j;
		for(; ; ){
			if (in >= cu[i]){
				if(in < cu[i+1]) return i;
				j<<=1; i+=j;
			} else {
				j>>=1; i-=j;
				for(; ; ){
					if (in >= cu[i]){
						if(in < cu[i+1]) return i;
						j>>=1; i+=j;
					} else {
						j>>=1; i-=j;
					}
				}
			}
		}
	}
	else {
		i-=j;
		for(; ; ){
			if (in >= cu[i]){
				if(in < cu[i+1]) return i;
				j>>=1; i+=j;
				for(; ; ){
					if (in >= cu[i]){
						if(in < cu[i+1]) return i;
						j>>=1; i+=j;
					} else {
						j>>=1; i-=j;
					}
				}
			} else {
				j<<=1; i-=j;
			}
		}
	}
}

/**	\brief Non adaptive range encoder.
    \param img	 	The pointer to encoding data.
 	\param size		The size of the  input data.
	\param a_bits	Bits per symbol befor quantization.
	\param q_bits	Bits per symbol after quantization.
	\param buff		The encoded output  buffer
    \param q		The pointer to quantization array.
    \param d		The pointer to array of probability distribution.
    \param buff1	The pointer to temporal buffer.
	\retval			The encoded message size in byts .
*/
uint32  range_encoder(int16 *img, uint32 size, uint32 a_bits , uint32 q_bits, uint8 *buff, int *q,  uint32 *d, uint32 *buff1)
{
	uint32 num = (1<<q_bits), sh = 8, size1 = size-1, sz;
	uint32 top = 0xFFFFFFFF, bot = (top>>sh), low=0, low1=0, range;
	uint32 i, j, k=0 , bits, tmp, msb;
	uint32 half = 1<<(a_bits-1), *dq = buff1, *cu = &buff1[num];
	int im;
	uint32 *don = &buff1[num<<1];

	//Encoder setup
	sz = dist_tot_pow_2(d, dq, a_bits, q_bits, &msb);
	cum_freq(dq, cu, num);
	tmp = write_dist(dq, num, msb, sz, buff);
	buff = &buff[tmp];

	//Start encoding
	range = top; low = 0; j=0;
	for(i=0; i<size; i++) {
		im = q[img[i] + half];
		//if(i<10) printf("img[%d] = %d  half = %d sz = %d im = %d cu[im] = %d d[im] = %d dq[im] = %d\n", i, img[i], half, sz, im, cu[im], d[im], dq[im]);
		range = range>>sz;
		low1 = low;
		low += range*cu[im];
		//if(i<10)      printf("%5d low = %8X low1 = %8X range = %8X  out = %4d img = %4d\n", i, low, low1, range, im, img[i]);
		//range = range*d[im];
		range = range*dq[im];
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

/**	\brief Non adaptive range decoder.
    \param img	 	The pointer to encoding data.
 	\param size		The size of the  input data.
	\param a_bits	Bits per symbol befor quantization.
	\param q_bits	Bits per symbol after quantization.
	\param buff		The encoded output  buffer
    \param q		The pointer to quantization array.
    \param buff1	The pointer to temporal buffer.
	\retval			The encoded message size in byts .
*/
uint32  range_decoder(int16 *img, uint32 size, uint32 a_bits , uint32 q_bits, uint8 *buff, int *q, uint32 *buff1)
{
        uint32 num = (1<<q_bits), sz, sum = 0, out, out1, out2, f, cf, sh = 8;
        uint32 top = 0xFFFFFFFF, bot = (top>>sh), low = 0, range;
        uint32 i, j, tmp, im, cu_sz;
        uint32 half = num>>1, bits, *d = buff1, *cu = &buff1[num];
        int dif, fin;

        //Decoder setup
        tmp = read_dist(d, num, &sz, buff);
    	cum_freq(d, cu, num);
        buff = &buff[tmp];

        range = top; j=4;
        // Start decoding
        low =  ((uint32)buff[0]<<24) | ((uint32)buff[1]<<16) | ((uint32)buff[2]<<8) | (uint32)buff[3];
        for(i=0; i<size; i++) {
                while(range <= bot) {
                        range <<=sh;
                        low = (low<<sh) | (uint32)buff[j++];
                }
                range = range>>sz;
                out = low/range;
                im = get_cum_f(out, cu, half, 1);
                //printf("%5d low = %8X low = %8X range = %8X range = %8X range = %8X out = %8d im = %8d  img = %4d q[im] = %4d d = %8d  cu = %8d diff = %d\n",
                //                      i, low, low-cu[im]*range, range<<sz, range, d[im]*range, out, im, img[i], q[im], d[im], cu[im], img[i]-q[im]);
                low -= cu[im]*range;
                range = d[im]*range;
                img[i] = q[im];
        }
        return j+tmp;
}

/**	\brief Make distribution with each elements equal power of 2
	\param din	 	The input distribution with sum is equal power of 2.
	\param dout		The output distribution with each elements is equal power of 2.
	\param don		The array of difference.
	\param sz		The size of distribution.
	\param msb		The maximum bits for output distribution representation.
	\retval			The log2 of distribution sum.
*/
static void dist_each_pow_2(uint32 *din, int  *dout, uint32 *don, uint32 sz, uint32 *msb)
{
	uint32 i, max, b, half, full, t;
	int nl, nr;
	memset(don, 0, sizeof(uint32)*sz<<1);

	//Make distribution with each frequency power of 2
	//TODO: remove din from final release
	for(i=0; i < sz; i++) dout[i] = din[i];
	max = 0;
	for(i=0; i < sz; i++){
		if(dout[i]){
			b = find_msb_bit(dout[i]);
			full = 1<<(b+1);
			half = 1<<b;
			//printf("dout = %d ", dout[i]);
			if(dout[i] != half){
				//np numbers of pixels to get of give then d[i] should be power of 2
				nl = dout[i] - half;
				nr = full - dout[i];
				if(nl > nr && nr <= dout[i+1]){
					//Should get pixels from neighbors
					dout[i] += nr; dout[i+1] -= nr; don[i+1<<1] = nr; //d2[(i+1<<1)] -= nr;
					dout[i] = b+1;
					if(max < dout[i]) max = dout[i];
				} else {
					//Should give pixels to neighbors
					dout[i] -= nl; dout[i+1] += nl; don[(i<<1)+1] = nl; //d2[(i+1<<1)] += nl;
					dout[i] = b;
					if(max < dout[i]) max = dout[i];
				}
			} else dout[i] = b;
			dout[i]++; //Add one to all non zero value due to distinguish 1 from 0 in power of 2

			//printf(" din = %d dout = %d d = %d b = %d b1 = %d l = %d r = %d max = %d \n", din[i], dout[i], t, b, b+1, don[(i<<1)], don[(i<<1)+1], max);
		}// else d2[i] = 0;
	}
	*msb = find_msb_bit(max)+2;
	//for(i=0; i < num; i++) printf("%d  ", dq[i]);
	//printf("num = %d\n", num);
	/*
	int t1 = 0, t2 = 0;
	for(i=0; i < num; i++) {  t1+=din[i]; t2 += dout[i] ? (1<<(dout[i]-1)) : 0;} //printf("%d %d  ", dq[i], dout[i]);
	printf("\n din = %d dout = %d max = %d \n", t1, t2, max);
	for(i=0; i < num; i++) printf("%d ", dout[i]);
	printf("\n");
	for(i=0; i < num; i++) printf("%d %d %d %d   ", din[i], don[i*2], don[i*2+1], dout[i]);
	printf("\n");
	*/
}

/** \brief Make cumulative frequency array.
	\param d 		The input distribution of probability array.
	\param cu		The output cumulative frequency.
	\param sz		The size of distribution.
*/
static void cum_freq_pow(int *d, uint32 *cu, uint32 sz)
{
	uint32 i, val;
	uint32 min, max = 0, sum = 0, ind;
	cu[0] = 0;
	for(i=0; i < sz; i++)  {
		//printf("d = %d ", d[i]);
		if(d[i]) { d[i]--; val = 1<<d[i]; }
		//printf("d = %d val = %d ", d[i], val);}
		else val = 0;
		cu[i+1] = cu[i] + val;
		//printf("%d %d ", i+1, cu[i+1]);
	}
	//printf("\n");
	//Make lool-up table for the fast finding index of cumulative frequency array.
}

static void lookup_array(int *d, uint32 *lt, uint32 *min, uint32 *max, int *st, uint32 num)
{
	uint32 i, j, sum = 0, ind, sz;
	int  tmp;
	//Make lool-up table for the fast finding index of cumulative frequency array.
	*max = 0;
	for(i=0; i < num; i++)  if(*max < d[i]) { *max = d[i]; ind = i; }

	*st = (*max >= 7) ? *max - 7 : 0; tmp = *max;
	for(i=ind; ;i++) if((d[i] - *st) <= 0) break;
	*max = i;
	for(i=ind; ;i--) if((d[i] - *st) <= 0) break;
	*min = i+1;
	sum = 0;
	for(i=*min; i < *max; i++) {
		sum += 1<<(d[i]-*st);
		printf("%d ", d[i]-*st);
	}
	int sum1 = 0, sum2 = 0;
	for(i=0; i < *min; i++) sum1 += 1<<d[i];

	for(i=*max; i < num; i++) sum2 += 1<<d[i];

	printf("\n");
	printf("sum = %d sum1 = %d sum2 = %d msb = %d st = %d max = %d  min = %d max = %d\n", sum, sum1, sum2, 1<<(find_msb_bit(sum)+1), *st, tmp, *min, *max);
	for(i=*min, j=0; i < *max; i++){
		sz = 1<<(d[i]-*st);
		for(; --sz; j++) lt[j] = i;
		//printf("%d %d  ", d[i]-*st, lt[j]);
		printf("[%d] %d ", j, lt[j]);

	}
	printf("\nj = %d\n", j);
}

static int quant_pow(uint16 img, int *q, uint32 *don)
{
	uint32 i = img<<1;
	if(don[i]) { don[i]--; return q[img-1]; }
	if(don[i+1]) { don[i+1]--; return q[img+1]; }
	return q[img];
}

/**	\brief Get array index from cumulative frequency.
    \param in	 	Input cumulative frequency.
 	\param cu		Cumulative frequency array.
	\param half		Tha half of cumulative frequency array size.
	\retval			The array index.
*/
static inline uint32 get_cum_pow(uint32 in, uint32 *cu, uint32 *lt, uint32 min, uint32 max, uint32 st, uint32 num)
{
	uint32 i, j, half, val;
	if(in < cu[min]){
		val = min>>1; half = val;
	} else if(in >= cu[max]){
		val = (num + max)>>1; half = (num - max)>>1;
	} else {
		return lt[in>>st];
	}

	for(i = val, j = half; ; ){
		//printf("out = %d cu[%d] = %d cu[%d] = %d j = %d \n", out, i, cu[i], i+1, cu[i+1], j);
		if(in >= cu[i]) {
			if(in < cu[i+1]) {
				//printf("cu[%d] = %d\n", i, cu[i]);
				return i;
			}
			else { j >>=1; i+=j;}
		}
		else { j = (j==0) ? 1 : j>>1; i-=j;}
		//else { j >>=1; i-=j;}
	}
}


/**	\brief Range encoder free of division and multiplication.
    \param img	 	The pointer to encoding data.
 	\param size		The size of the  input data.
	\param a_bits	Bits per symbol befor quantization.
	\param q_bits	Bits per symbol after quantization.
	\param buff		The encoded output  buffer
    \param q		The pointer to quantization array.
    \param d		The pointer to array of probability distribution.
    \param buff1	The pointer to temporal buffer.
	\retval			The encoded message size in byts .
*/
uint32  range_encoder_fast(int16 *img, uint32 size, uint32 a_bits , uint32 q_bits, uint8 *buff, int *q,  uint32 *d, uint32 *buff1){
	uint32 num = (1<<q_bits), sh = 8, size1 = size-1, sz;
	uint32  low = 0, low1 = 0, range = 32;
	uint32 i, j = 0, k=0 , bits, tmp, msb;
	uint32 half = 1<<(a_bits-1), *dq = buff1, *cu = &dq[num], *don = &cu[num+1];
	int im;

	//Encoder setup
	sz = dist_tot_pow_2(d, d, a_bits, q_bits, &msb);
	dist_each_pow_2(d, dq, don, num, &msb);
	tmp = write_dist(dq, num, msb, sz, buff);
	buff = &buff[tmp];

	cum_freq_pow(dq, cu, num);

	//int min, max, st, *lt = &don[num<<1];
	//lookup_array(dq, lt, &min, &max, &st, num);

	for(i=0; i<size; i++) {
		im = quant_pow(img[i] + half, q, don);
		//if(i<10)
		//printf("img[%d] = %d  half = %d sz = %d im = %d cu[im] = %d d[im] = %d dq[im] = %d\n", i, img[i], half, sz, im, cu[im], d[im], dq[im]);
		range = range - sz;
		low1 = low;
		low += cu[im]<<range;
		//if(i<10)	printf("%5d low = %8X low1 = %8X range = %8X  out = %4d img = %4d\n", i, low, low1, range, im, img[i]);
		range = range + dq[im];
		if(low < low1) { for(k=1; !(++buff[j-k]); k++);}
		if(i != size1){
			while(range <= 24) {
				buff[j++] = (low>>24);
				range += sh;
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

/**	\brief Range decoder free of division and multiplication.
    \param img	 	The pointer to encoding data.
 	\param size		The size of the  input data.
	\param a_bits	Bits per symbols befor quantization.
	\param q_bits	Bits per symbols after quantization.
	\param buff		The encoded output  buffer
    \param q		The pointer to quantization array.
    \param buff1	The pointer to temporal buffer.
	\retval			The encoded message size in byts .
*/
uint32  range_decoder_fast(int16 *img, uint32 size, uint32 a_bits , uint32 q_bits, uint8 *buff, int *q, uint32 *buff1){
	uint32 num = (1<<q_bits), sz, out, sh = 8;
	uint32 low = 0, range = 32;
	uint32 i, j, tmp, im, cu_sz;
	uint32 half = num>>1, bits, *dq = buff1, *cu = &buff1[num];
	int dif, fin;

	//Decoder setup
	tmp = read_dist(dq, num, &sz, buff);
	buff = &buff[tmp];
	cum_freq_pow(dq, cu, q_bits);

	low =  ((uint32)buff[0]<<24) | ((uint32)buff[1]<<16) | ((uint32)buff[2]<<8) | (uint32)buff[3];
	j=4;
	// Start decoding
	//for(i=0; i<100; i++) {
	for(i=0; i<size; i++) {
		while(range <= 24) {
			range +=sh;
			low = (low<<sh) | (uint32)buff[j++];
		}
		range = range-sz;
		out = low>>range;
		im = get_cum_f(out, cu, half, 1);
		//if(i<10)
		//printf("%5d low = %8X low = %8X range = %8X range = %8X range = %8X out = %8d im = %8d  img = %4d q[im] = %4d cu = %8d diff = %d\n",
		//			i, low, low-cu[im]*range, range+sz, range, dq[im]+range, out, im, img[i], q[im], cu[im], img[i]-q[im]);

		low -= cu[im]<<range;
		range = range + dq[im];
		img[i] = q[im];
	}
	return j+tmp;
}

/**	\brief Write array without compression.
    \param img	 	The pointer to encoding data.
 	\param size		The size of the  input data.
	\param bits		The data dits per symbol.
	\param buff		The output  buffer
	\retval			The write buffer size in byts .
*/
uint32  write_array(int16 *img, uint32 size, uint32 bits, uint8 *buff, int *q)
{
	uint32 i,  poz = 0, half = 1<<(bits-1);

	//Write all another
	for(i=0; i < size; i++) {
		write_bits_pos(buff, &poz, q[img[i] + half], bits);
	}
	return (poz>>3) + 1;
}

/**	\brief Read non compressed array.
    \param img	 	The pointer to output data.
 	\param size		The size of the  input data.
	\param bits		The data dits per symbol.
	\param buff		The input  buffer
	\retval			The read buffer size in byts .
*/
uint32  read_array(int16 *img, uint32 size, uint32 bits, uint8 *buff, int *q)
{
	uint32 i, poz = 0, tm;

	//Write all another
	for(i=0; i < size; i++) {
		read_bits_pos(buff, &poz, &tm, bits);
		img[i] = q[tm];
		//printf("%d ", img[i]);
	}
	return (poz>>3) + 1;
}
