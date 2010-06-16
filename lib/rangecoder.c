#include <stdio.h>
#include <string.h>
#include <walet.h>
#include <rangecoder.h>

uint32 div_look_up1[256] = {
		2048, 2040, 2032, 2024, 2016, 2008, 2001, 1993,
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
		1040, 1038, 1036, 1034, 1032, 1030, 1028, 1026,
};

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

static inline uint32  division(uint32 a, uint32 b, uint32 bits)
{
	uint32 rest = b - (1<<bits);
	//sh = bits-8, vl = rest>>sh, ch = ((1<<sh)-1);
	//printf("i = %d look = %d\n ",bits > 8 ? (rest>>(bits-8)) : (rest<<(8-bits)), div_look_up[bits > 8 ? (rest>>(bits-8)) : (rest<<(8-bits))]);
	//((1<<(sh))-1)&rest
	//if(bits > 8)
	//return	(a>>(11+bits))*(div_look_up[sh] -((div_look_up[sh] - div_look_up[sh+1])>>sh)*(ch&rest));

	return (a>>bits)*(div_look_up[bits > 8 ? (rest>>(bits-8)) : (rest<<(8-bits))]+1024)>>11;
	//return (a>>(11+bits))*(div_look_up[bits > 8 ? (rest>>(bits-8)) : (rest<<(8-bits))]+1024);
	//return (a>>(11+bits))*div_look_up[bits > 8 ? (rest>>(bits-8)) : (rest<<(8-bits))];
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
	uint32 num = (1<<q_bits), sz = num, sh = 8;
	uint32 top = 0xFFFFFFFF, bot = (top>>sh), low=0, low1=0, range;
	uint32 i, j, k=0 , cu, bits;
	uint32 half = 1<<(a_bits-1);
	//uint16 *buff = (uint16*) buf;
	uchar  *buff = buf;
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

		//if((1<<(bits+1)) & sz) bits++;
		//range = division(range, sz, bits);

		range = range/sz;
		//printf(" %2d  sz = %u bits = %u range = %16LX range1 = %16LX diff = %16LX\n", i, sz, bits, range, range1, range - range1);

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
	printf("%5d low = %8X \n", i, low);
	buff[j++] = (low>>24);
	buff[j++] = (low>>16) & 0xFF;
	//if((low>>8)  & 0xFF) buff[j++] = (low>>8)  & 0xFF;
	buff[j++] = (low>>8)  & 0xFF;
	if(low & 0xFF) buff[j++] = low & 0xFF;
	//printf("size = %d Encoder size  = %d first = %4X end = %4X img = %d %d %d %d \n", size, j<<1, buff[0], buff[j-1], img[0], img[1], img[2], img[3]);
	//return (j<<1);
	//for(i=j; i>j-5; i--) printf("buff[%d] = %2X\n",i, buff[i]);
	return j;
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
	uint32 num = (1<<q_bits), sz = num, sum = 0, out, out1, f, cf, sh = 8;
	uint32 top = 0xFFFFFFFF, bot = (top>>sh), low = 0, range;
	uint32 i, j, del = a_bits-q_bits, sub = (1<<del)>>1;
	uint32 half = num>>1, bits, bits1;
	int dif, fin;
	//uint16 *buff = (uint16*) buf;
	uchar  *buff = buf;

	//Initial setup
	memset(d, 0, sizeof(uint32)*num*2);
	for(i=0; i<num; i++) set_freq(i, d, q_bits);

	//Ecoder setup
	range = top;
	//printf("bits = %d\n", bits);
	//for(bits=0; ;bits++) if(!(num>>=1)) break;
	for(bits=1; ;bits++) if(!(num>>bits)) break;
	bits--;
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
		//if((1<<(bits+1)) & sz) bits++;
		//range = division(range, sz, bits);

		range = range/sz;
		out = low/range;
		//for(bits1=1; ;bits1++) if(!(range>>bits1)) break;
		//printf("range = %16LX bits1 = %d\n", range, bits1);
		//out = division(low, range, bits1-1);
		//printf("bits1 = %d\n", bits);

		out1 =  get_freq_cum(out, d, q_bits, &f, &cf);
		if(img[i]-q[out1]) {//printf("i=%d ", i);
			if(i<1 )printf("%5d low = %8X range = %8X out = %3d out1 = %3d img = %4d q[out1] = %4d diff = %d\n",
					i, low, range, out, out1, img[i], q[out1], img[i]-q[out1]);
			//else return 0;
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

