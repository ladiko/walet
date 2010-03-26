#include <stdio.h>
#include <string.h>
#include <walet.h>
//#include <range-coder.h>


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
	for(i=0, ind=0; i<bits; ind+=(1<<(bits-i)), i++) d[ind + (img>>i)]++;
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
	for(i=0; i<bits; i++, ind-=(1<<(i+1))) if((img>>(bit-i))%2) c+=d[ind +(img>>(bit-i))-1];
	return c;

}

static inline uint32 get_freq1(imgtype img, uint32 *d, uint32 bits)
/*! \fn void update_friq(imgtype img, uint32 *d, uint32 *c, const uit32 num)
	\brief Update cummulative frequency.
	\param img	 	The next data.
	\param d		The pointer to array of distribution probabilities of the message.
	\param bits		The numbers of bits per  symbols.
*/
{
	uint32 i, c=0, bit = bits-1, ind = (1<<(bits+1))-4;
	//for(i=0; i<bits; i++) if((img>>(bit-i))%2) c+=d[bit-i][(img>>(bit-i))-1];
	for(i=0; i<bits; i++, ind-=(1<<(i+1))) if((img>>(bit-i))%2) c+=d[ind +(img>>(bit-i))-1];
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

uint32  range_encoder(imgtype *img, uint32 *d, uint32 size, uint32 a_bits , uint32 q_bits, uchar *buff)
/*! \fn uint32  range_encoder(imgtype *img, uint32 *distrib, const uint32 size, const uchar bits)
	\brief Range encoder.
    \param img	 	The pointer to encoding message data.
    \param d		The pointer to array of distribution probabilities of the mesage.
	\param size		The size of the  message
	\param a_bits	Bits per symbols befor quantization.
	\param q_bits	Bits per symbols after quantization.
	\param buff		The encoded output  buffer
	\retval			The encoded message size in byts .
*/
{
	uint32 shift = 48, num = (1<<q_bits), sz = num, sum = 0, out;
	uint64 top = 0xFFFFFFFFFFFFFFFF, bot = (top>>16), low=0, low1=0, low2=0, range;
	uint32 i, j, k=0 , cu, del = a_bits-q_bits, sub = (1<<del)>>1;
	uint32 half = num>>1;
	int im;

	memset(d, 0, sizeof(uint32)*num*2);
	for(i=0; i<num; i++) set_freq(i, d, q_bits);

	//Ecoder setup
	range = top; low = 0;
	j=0;
	//printf(" top = %16LX bot = %16LX\n", top, bot);
	for(i=0; i<size; i++) {
		im = (img[i] > 0 ? (img[i]-sub)>>del : (img[i] <0 ? -((-img[i]-sub)>>del) : 0)) + half;
		//printf("img = %d", im);
		range = range/sz; //range1 = range;
		low2 = low;
		cu = get_freq(im, d, q_bits);
		low += range*cu;
		//low += range*c[img[i]+HALF1];
		if(low < low2) low1++;
		//range = range*d[img[i]+HALF1];
		range = range*d[im];
		//printf(" %2d  img = %3d f = %4u c = %4u  new_c = %4u low = %16LX range = %16LX\n", i, img[i], d[img[i]+HALF1], c[img[i]+HALF1], cu, low, range);
		while(range <= bot && i<(size-1)) {
			out = (low>>shift);
			k+=2;
			if(k>8) { buff[j] = low1>>56; buff[j+1] = low1>>48; j+=2;}
			low1 = (low1 <<16) | out;
			range <<= 16;
			low <<= 16;
		}
		set_freq(im, d, q_bits);
		sz++;
	}
	for(i=0; i<8; i++) buff[j+i] = (low1>>(56-(i<<3))) & 0xFF; j+=8;
	for(i=0; i<8; i++) buff[j+i] = (low >>(56-(i<<3))) & 0xFF; j+=8;
	//printf(" size = %d size1 = %d per = %f\n", size, j, (double)j/(double)size);
	return j;
}

uint32  range_decoder(imgtype *img, uint32 *d, uint32 size, uint32 a_bits , uint32 q_bits, uchar *buff)
/*! \fn uint32  range_encoder(imgtype *img, uint32 *distrib, const uint32 size, const uchar bits)
	\brief Range decoder.
    \param img	 	The pointer to encoding message data.
    \param d		The pointer to array of distribution probabilities of the mesage.
	\param size		The size of the  message
	\param a_bits	Bits per symbols befor quantization.
	\param q_bits	Bits per symbols after quantization.
	\param buff		The encoded output  buffer
	\retval			The encoded message size in byts .
*/
{
	uint32 shift = 48, num = (1<<q_bits), sz = num, sum = 0, out, out1, f, cf;
	uint64 top = 0xFFFFFFFFFFFFFFFF, bot = (top>>16), low = 0, range;
	uint32 i, j, del = a_bits-q_bits, sub = (1<<del)>>1;;
	uint32 half = num>>1;
	int dif, fin;

	//Initial setup
	//memset(c, 0, num);
	//d[0] = 1; c[0] = 0;
	//for(i=1; i<num; i++) { d[i] = 1; c[i] = sum += d[i-1];}

	memset(d, 0, sizeof(uint32)*num*2);
	for(i=0; i<num; i++) set_freq(i, d, q_bits);

	//Ecoder setup
	range = top;
	low =  ((uint64)buff[0]<<56) | ((uint64)buff[1]<<48) | ((uint64)buff[2]<<40) | ((uint64)buff[3]<<32) |
		   ((uint64)buff[4]<<24) | ((uint64)buff[5]<<16) | ((uint64)buff[6]<<8 ) |  (uint64)buff[7];
	j=8;
	//printf("range = %16LX low = %16LX\n", range, low);
	for(i=0; i<size; i++) {
		//while(0) {
		while(range <= bot) {
			range <<=16;
			low = (low<<16) | ((uint64)buff[j]<<8) | ((uint64)buff[j+1]); j+=2;
		}
		range = range/sz;
		out = low/range;
		//out1 = find_cum_frq(out, d, c, num, f, cf);
		//out1=1;
		out1 =  get_freq_cum(out, d, q_bits, &f, &cf);
		low -= cf*range;
		range = range*f;
		set_freq(out1, d, q_bits);
		//update_freq(out1, d, c, num);
		sz++;
		//if(0){
		dif = out1 - half;
		fin = dif > 0 ? (dif<<del)+sub : (dif < 0 ? -((-dif)<<del)-sub : 0);
		if(img[i]- fin)
			printf("low = %16LX range = %16LX out = %3d sim = %3d img = %3d fin = %3d dif = %d f = %4d cf = %7d diff = %d\n",
					low, range, out, out1, img[i], fin, dif, f, cf, img[i]- out1+half);


	}
	//printf("Decoder fineshed!\n");
	return j;
}

