#include <stdio.h>
#include <string.h>
#include <walet.h>
#include <rangecoder.h>

uint32 div_look_up[256] = {
2040, 2032, 2024, 2016, 2008, 2001, 1993,
1985, 1978, 1971, 1963, 1956, 1949, 1941, 1934,
1927, 1920, 1913, 1906, 1899, 1892, 1885, 1879,
1872, 1865, 1859, 1852, 1846, 1839, 1833, 1826,
1820, 1814, 1807, 1801, 1795, 1789, 1783, 1777,
1771, 1765, 1759, 1753, 1747, 1741, 1736, 1730,
1724, 1718, 1713, 1707, 1702, 1696, 1691, 1685,
1680, 1675, 1669, 1664, 1659, 1653, 1648, 1643,
1638, 1633, 1628, 1623, 1618, 1613, 1608, 1603,
1598, 1593, 1588, 1583, 1579, 1574, 1569, 1565,
1560, 1555, 1551, 1546, 1542, 1537, 1533, 1528,
1524, 1519, 1515, 1510, 1506, 1502, 1497, 1493,
1489, 1485, 1481, 1476, 1472, 1468, 1464, 1460,
1456, 1452, 1448, 1444, 1440, 1436, 1432, 1428,
1424, 1420, 1416, 1413, 1409, 1405, 1401, 1398,
1394, 1390, 1387, 1383, 1379, 1376, 1372, 1368,
1365, 1361, 1358, 1354, 1351, 1347, 1344, 1340,
1337, 1334, 1330, 1327, 1323, 1320, 1317, 1314,
1310, 1307, 1304, 1300, 1297, 1294, 1291, 1288,
1285, 1281, 1278, 1275, 1272, 1269, 1266, 1263,
1260, 1257, 1254, 1251, 1248, 1245, 1242, 1239,
1236, 1233, 1230, 1227, 1224, 1222, 1219, 1216,
1213, 1210, 1208, 1205, 1202, 1199, 1197, 1194,
1191, 1188, 1186, 1183, 1180, 1178, 1175, 1172,
1170, 1167, 1165, 1162, 1159, 1157, 1154, 1152,
1149, 1147, 1144, 1142, 1139, 1137, 1134, 1132,
1129, 1127, 1125, 1122, 1120, 1117, 1115, 1113,
1110, 1108, 1106, 1103, 1101, 1099, 1096, 1094,
1092, 1089, 1087, 1085, 1083, 1081, 1078, 1076,
1074, 1072, 1069, 1067, 1065, 1063, 1061, 1059,
1057, 1054, 1052, 1050, 1048, 1046, 1044, 1042,
1040, 1038, 1036, 1034, 1032, 1030, 1028, 1026, 1024,
};

static inline uint64  division(uint64 a, uint32 b, uint32 bits)
{
	uint32 rest = b - (1<<bits);
	//printf("i = %d look = %d\n ",bits > 8 ? (rest>>(bits-8)) : (rest<<(8-bits)), div_look_up[bits > 8 ? (rest>>(bits-8)) : (rest<<(8-bits))]);
	return (a>>(11+bits))*div_look_up[bits > 8 ? (rest>>(bits-8)) : (rest<<(8-bits))];
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

uint32  range_encoder(imgtype *img, uint32 *d, uint32 size, uint32 a_bits , uint32 q_bits, uchar *buf, int *q)
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
	uint32 shift = 16, num = (1<<q_bits), sz = num, sum = 0, out;
	uint64 top = 0xFFFFFFFFFFFFFFFF, bot = (top>>16), low=0, low1=0, range;
	uint32 i, j, k=0 , cu, bits;
	uint32 half = 1<<(a_bits-1);
	uint16 *buff = (uint16*) buf;
	int im;

	memset(d, 0, sizeof(uint32)*num*2);
	for(i=0; i<num; i++) set_freq(i, d, q_bits);
	//printf("num = %d\n", num*2);

	//Ecoder setup
	range = top; low = 0; j=0;
	for(bits=0; ;bits++) if(!(num>>=1)) break;
	//printf("sz = %d bits = %d\n", sz, bits);

	//printf(" top = %16LX bot = %16LX\n", top, bot);
	for(i=0; i<size; i++) {
		im = q[img[i] + half];


		//uint64 range1;
		if((1<<(bits+1)) & sz) bits++;
		range = division(range, sz, bits);

		//range = range/sz;
		//printf(" %2d  sz = %u bits = %u range = %16LX range1 = %16LX diff = %16LX\n", i, sz, bits, range, range1, range - range1);

		low1 = low;
		cu = get_freq(im, d, q_bits);
		low += range*cu;
		if(i==0)printf("%5d low = %16LX range = %16LX out = %3d img = %4d\n", i, low, range, out, img[i]);
		range = range*d[im];
		//if(low < low1) { if(buff[j-1] == 0X0000) printf("buff = %4X %4X", buff[j-1], buff[j-2]); for(k=1; !(++buff[j-k]); k++); if(k>1) printf("k = %d j = %d buff = %4X %4X\n", k, j, buff[j-1], buff[j-2]);}
		if(low < low1) { for(k=1; !(++buff[j-k]); k++);}
		//printf(" %2d  img = %3d f = %4u cu = %4u low = %16LX range = %16LX\n", i, im, d[im], cu, low, range);

		while(range <= bot) {
			buff[j++]  = (low>>48);
			//if(j==1) printf("encoder img = %4X\n", buff[0]);
			range <<= 16;
			low <<= 16;
			//printf("                                  low = %16LX range = %16LX\n", low, range);
		}
		set_freq(im, d, q_bits);
		sz++;
	}

	buff[j++] = (low>>48);
	buff[j++] = (low>>32) & 0xFFFF;
	buff[j++] = (low>>16) & 0xFFFF;
	if(low & 0xFFFF) buff[j++] = low & 0xFFFF;
	//printf("size = %d Encoder size  = %d first = %4X end = %4X img = %d %d %d %d \n", size, j<<1, buff[0], buff[j-1], img[0], img[1], img[2], img[3]);
	return (j<<1);
}

uint32  range_decoder(imgtype *img, uint32 *d, uint32 size, uint32 a_bits , uint32 q_bits, uchar *buf, int *q)
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
	uint32 shift = 48, num = (1<<q_bits), sz = num, sum = 0, out, out1, f, cf;
	uint64 top = 0xFFFFFFFFFFFFFFFF, bot = (top>>16), low = 0, range;
	uint32 i, j, del = a_bits-q_bits, sub = (1<<del)>>1;;
	uint32 half = num>>1, bits;
	int dif, fin;
	uint16 *buff = (uint16*) buf;

	//Initial setup
	memset(d, 0, sizeof(uint32)*num*2);
	for(i=0; i<num; i++) set_freq(i, d, q_bits);

	//Ecoder setup
	range = top;
	for(bits=0; ;bits++) if(!(num>>=1)) break;

	//printf("decoder img = %4X\n", buff[0]);
	low =  ((uint64)buff[0]<<48) | ((uint64)buff[1]<<32) | ((uint64)buff[2]<<16) | (uint64)buff[3];
	j=4;
	//printf("range = %16LX low = %16LX\n", range, low);
	for(i=0; i<size; i++) {
		while(range <= bot) {
			range <<=16;
			low = (low<<16) | (uint64)buff[j++];
		}
		if((1<<(bits+1)) & sz) bits++;
		range = division(range, sz, bits);

		//range = range/sz;
		out = low/range;
		out1 =  get_freq_cum(out, d, q_bits, &f, &cf);
		low -= cf*range;
		if(img[i]- q[out1]) {//printf("i=%d ", i);
			printf("%5d low = %16LX range = %16LX out = %3d out1 = %3d img = %4d qimg = %4d diff = %d\n",
					i, low, range, out, out1, img[i], q[out1], img[i]- q[out1]);
			return 0;
		}
		range = range*f;
		set_freq(out1, d, q_bits);
		sz++;
		//img[i] = q[out1];
		//dif = out1 - half;
		//fin = dif > 0 ? (dif<<del)+sub : (dif < 0 ? -((-dif)<<del)-sub : 0);

	}
	//printf("size = %d Decoder size  = %d first = %4X end = %4X img = %d %d %d %d\n", size, j<<1, buff[0], buff[j-1], img[0], img[1], img[2], img[3]);
	return (j<<1);
}

