/*
 * decoder.c
 *	Sample walet decoder
 *  Created on: 29.04.2010
 *      Author: vadim
 */

#include "walet.h"

uchar* decoder(const char *filename)
{
	FILE *in;
	GOP gop;
	StreamData sd;
	uchar* data;

	in = fopen(filename, "rb");

	walet_read_header(&sd, in);
	walet_init(&sd, &gop);
	data = walet_decode_frame(in);

	fclose(in);
	walet_finish(&sd, &gop);
	return data;
}

