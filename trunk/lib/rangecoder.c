#include <stdio.h>
#include <string.h>
#include <walet.h>
#include <rangecoder.h>

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
}



static uint32 dist_tot_pow_2(uint32 *din, uint32 *dout, uint32 a_bits, uint32 q_bits, uint32 *msb)
/// \fn uint32 dist_tot_pow_2(uint32 *din, uint32 *dout, uint32 a_bits, uint32 q_bits, uint32 *msb)
///	\brief Make distribution with sum equal power of 2
/// \param din	 	The input distribution.
/// \param dout		The output distribution.
///	\param a_bits	Bits per symbols before quantization.
///	\param q_bits	Bits per symbols after quantization.
///	\param msb		The maximum bits for output distribution representation.
///	\retval			The log2 of distribution sum.

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

static void dist_each_pow_2(uint32 *din, int  *dout, uint32 *don, uint32 a_bits, uint32 q_bits, uint32 *msb)
/// \fn uint32 dist_tot_pow_2(uint32 *din, uint32 *dout, uint32 a_bits, uint32 q_bits, uint32 *msb)
///	\brief Make distribution with sum equal power of 2
/// \param din	 	The input distribution with sum is equal power of 2.
/// \param dout		The output distribution with each elements is equal power of 2.
/// \param don		The array of difference.
///	\param a_bits	Bits per symbols before quantization.
///	\param q_bits	Bits per symbols after quantization.
///	\param msb		The maximum bits for output distribution representation.
///	\retval			The log2 of distribution sum.
{
	uint32 i, num = 1<<q_bits, max, b, half, full, t;
	int nl, nr;
	//memset(dout, 0, sizeof(uint32)*num);
	memset(don, 0, sizeof(uint32)*num<<1);

	//Make distribution with each frequency power of 2
	for(i=0; i < num; i++) dout[i] = din[i];
	max = 0;
	for(i=0; i < num; i++){
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
			} dout[i] = b;
			//printf(" din = %d dout = %d d = %d b = %d b1 = %d l = %d r = %d max = %d \n", din[i], dout[i], t, b, b+1, don[(i<<1)], don[(i<<1)+1], max);
		}// else d2[i] = 0;
	}
	*msb = find_msb_bit(max)+1;

	//for(i=0; i < num; i++) printf("%d  ", dq[i]);
	//printf("num = %d\n", num);
	/*
	int t1 = 0, t2 = 0;
	for(i=0; i < num; i++) {  t1+=din[i]; t2+=dout[i];} //printf("%d %d  ", dq[i], dout[i]);
	printf("\n din = %d dout = %d max = %d \n", t1, t2, max);
	for(i=0; i < num; i++) printf("%d ", dout[i]);
	printf("\n");
	for(i=0; i < num; i++) printf("%d %d %d %d   ", din[i], don[i*2], don[i*2+1], dout[i]);
	printf("\n");
	*/

	//printf("\n size = %d sum = %d sum1 = %d sum2 = %d cu[i-1] = %d bits = %d\n", size, sum, sum1, sum2, cu[i-1], bits);
}

static int quant(uint16 img, int *q, uint32 *don)
{
	uint32 i = img<<1;
	if(don[i]) {don[i]--; return q[img-1]; }
	if(don[i+1]) {don[i+1]--; return q[img+1]; }
	return q[img];
}

static void cum_freq(uint32 *d, uint32 *cu, uint32 q_bits)
{
	uint32 i, num = 1<<q_bits;
	cu[0] = 0;
	for(i=1; i<=num; i++)  {
		cu[i] = cu[i-1] + d[i-1];
	}
}

static void cum_freq1(uint32 *d, uint32 *cu, uint32 q_bits)
{
	uint32 i, num = 1<<q_bits;
	cu[0] = 0;
	for(i=1; i<=num; i++)  {
		cu[i] = cu[i-1] + (1<<d[i-1]);
	}
}

static inline void write_bits_pos(uint8 *buff, uint32 *poz, uint32 st, uint32 bits)
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

static inline void read_bits_pos(uint8 *buff, uint32 *poz, uint32 *st, uint32 bits)
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

static uint32 write_dist(uint32 *d, uint32 q_bits, uint32 msb, uint32 sz, uint8 *buff)
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

static uint32 read_dist(uint32 *d, uint32 q_bits, uint32 *sz, uint8 *buff)
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

static inline uint32 get_cum_f(uint32 out, uint32 *cu, uint32 half)
{
	uint32 i, j;
	//if(!out) return 0;
	for(i = half, j = half; ; ){
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
	uint32 half = 1<<(a_bits-1), *dq = buff1, *cu = &buff1[num], *don = &buff1[num<<1];
	int im;

	//Encoder setup
	sz = dist_tot_pow_2(d, d, a_bits, q_bits, &msb);

	dist_each_pow_2(d, dq, don, a_bits, q_bits, &msb);

	//cum_freq(dq, cu, q_bits);
	cum_freq1(dq, cu, q_bits);
	//printf("finesh make_distrib sz = %d num = %d\n", sz, num);

	tmp = write_dist(dq, q_bits, msb, sz, buff);
	printf("write = %d \n", tmp);
	//for(i=0; i < num; i++) printf("%d ", dq[i]);
	//printf("\n");

	buff = &buff[tmp];

	//j = (tmp&7) ? (tmp>>3) + 1 : (tmp>>3);
	//printf("dist_size = %d bits %d byts\n", tmp, j);
	j=0;

	//init_prob(d, q_bits, c);
	//range = top;
	range = 32; low = 0;

	//for(i=0; i < num; i++) printf("%d ", q[i]);
	//printf("\n");
	//for(i=0; i < num; i++) printf("%d ", d[i]);
	//printf("\n");
	//Start encoding
	for(i=0; i<size; i++) {
		//im = q[img[i] + half];
		im = quant(img[i] + half, q, don);
		//if(i<10) printf("img[%d] = %d  half = %d sz = %d im = %d cu[im] = %d d[im] = %d dq[im] = %d\n", i, img[i], half, sz, im, cu[im], d[im], dq[im]);

		//range = range>>sz;
		range = range-sz;


		//cu = get_cum(im, c, q_bits);
		low1 = low;
		//low += range*cu[im];
		low += cu[im]<<range;
		//if(i<10)	printf("%5d low = %8X low1 = %8X range = %8X  out = %4d img = %4d\n", i, low, low1, range, im, img[i]);
		range = range+dq[im];
		//range = range*dq[im];
		//update_dist(im, d, ds, ab, sw, size_ab, &get, &set, &swc);
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
	tmp = read_dist(dq, q_bits, &sz, buff);
	//printf("read = %d \n", tmp);
	//for(i=0; i < num; i++) printf("%d ", dq[i]);
	//printf("\n");

	//cum_freq(dq, cu, q_bits);
	cum_freq1(dq, cu, q_bits);

	//for(i=0; i < num; i++) printf("%d ", cu[i]);
	//printf("\n");
	//sz = read_distrib(buff, d, q_bits);
	//sz = read_distrib1(buff, d, q_bits);
	buff = &buff[tmp];

	range = 32; j=4;

	low =  ((uint32)buff[0]<<24) | ((uint32)buff[1]<<16) | ((uint32)buff[2]<<8) | (uint32)buff[3];

	// Start decoding
	for(i=0; i<size; i++) {
		while(range <= 24) {
			range +=sh;
			low = (low<<sh) | (uint32)buff[j++];
		}
		range = range-sz;
		//range = range>>sz;
		//out = divide1(low, range);
		out = low>>range;
		//im = get_cum_f(out, cu, cu_sz);
		im = get_cum_f(out, cu, num);
		//im = get_cum_f1(out, cu, half, half);
		//im = half;
		//if(img[i] != q[im])
		//if(i >=0 && i< 5)
		//printf("%5d low = %8X low = %8X range = %8X range = %8X range = %8X out = %8d im = %8d  img = %4d q[im] = %4d d = %8d  cu = %8d diff = %d\n",
		//			i, low, low-cu[im]*range, range<<sz, range, d[im]*range, out, im, img[i], q[im], d[im], cu[im], img[i]-q[im]);
		low -= cu[im]<<range;
		//range = dq[im]*range;
		range = range + dq[im];
		//set_freq(out1, d, q_bits);
		//sz++;
		img[i] = q[im];
	}
	return j+tmp;
}

