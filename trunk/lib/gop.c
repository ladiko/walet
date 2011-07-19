#include <walet.h>

#include <stdio.h>
#include <stdlib.h>

GOP* walet_decoder_init(uint32 w, uint32 h, ColorSpace color, BayerGrid bg, uint32 bpp, uint32 steps, uint32 gop_size, uint32 rates, uint32 comp, FilterBank fb)
{
}


GOP* walet_encoder_init(uint32 w, uint32 h, ColorSpace color, BayerGrid bg, uint32 bpp, uint32 steps, uint32 gop_size, uint32 rates, uint32 comp, FilterBank fb, uint8 mvs)
{
	int i;
	GOP *gop = (GOP*) malloc(sizeof(GOP));
	gop->w  		= w;
	gop->h 			= h;
	gop->color  	= color;
	gop->bg			= bg;
	gop->bpp		= bpp;
	gop->steps		= steps;
	gop->gop_size	= gop_size;
	gop->rates		= rates;
	gop->comp		= comp;
	gop->fb			= fb;
	gop->mvs		= mvs;

	//Temp buffer init
	gop->buf = (uint8 *)calloc(w*h*3, sizeof(uint8));
	gop->q = (int *)calloc(1<<(bpp+3)+1, sizeof(int));
	gop->mmb = (uint8 *)calloc(((gop->mvs<<1)+1)*((gop->mvs<<1)+1)*6, sizeof(uint8));
	//The memory for segmentation
	/*
	gop->row = (Row *)calloc(width*height, sizeof(Row));
	gop->cor = (Corner *)calloc(width*height, sizeof(Corner));
	gop->region = (Region *)calloc(width*height, sizeof(Region));
	gop->obj = (Object *)calloc(width*height, sizeof(Object));
	gop->prow = (Row **)calloc(width*height, sizeof(Row*));
	gop->preg = (Region **)calloc(width*height, sizeof(Region*));*/

	//gop->edg = (Edge *)calloc(width*height, sizeof(Edge));
	//gop->pix = (Pixel *)calloc(width*height, sizeof(Pixel));
	//gop->pedg = (Edge **)calloc(width, sizeof(Edge*));
	//gop->edgel = (Edgelet *)calloc(width*height, sizeof(Edgelet));

	printf("Buffer init\n");
	//Frames init
	gop->frames = (Frame *)calloc(gop_size, sizeof(Frame));
	printf("Frames  create %d\n", gop_size);
	for(i=0; i<gop_size; i++) frames_init(gop, i);
	printf("Frames  init\n");

	//Segmentation parts init
	gop->subs.w  = w>>1;
	gop->subs.h = h>>1;
	gop->subs.pic = (uint8 *)calloc(gop->subs.w*gop->subs.h, sizeof(uint8));
	gop->grad.w  = w>>1;
	gop->grad.h = h>>1;
	gop->grad.pic = (uint8 *)calloc(gop->grad.w*gop->grad.h, sizeof(uint8));
	gop->con.w  = w>>1;
	gop->con.h = h>>1;
	gop->con.pic = (uint8 *)calloc(gop->con.w*gop->con.h, sizeof(uint8));

	/*
	//Subband init
	subband_init(gop->sub, 0, color, width, height, steps, bpp, gop->q);
	gop->frames[0].img[0].sub = gop->sub[0];  // Set pointer to subband to frame[0]
	printf("subband_init  init\n");

	if(color == CS444 || color == RGB) 	{
		subband_init(gop->sub, 1, color, width, height, steps, bpp, gop->q);
		subband_init(gop->sub, 2, color, width, height, steps, bpp, gop->q);
		gop->frames[0].img[1].sub = gop->sub[1]; // Set pointer to subband to frame[0]
		gop->frames[0].img[2].sub = gop->sub[2]; // Set pointer to subband to frame[0]
	}
	printf("subband_init  init\n");
	if(color == CS422){
		subband_init(gop->sub, 1, color, width>>1, height, steps, bpp, gop->q);
		subband_init(gop->sub, 2, color, width>>1, height, steps, bpp, gop->q);
		gop->frames[0].img[1].sub = gop->sub[1]; // Set pointer to subband to frame[0]
		gop->frames[0].img[2].sub = gop->sub[2]; // Set pointer to subband to frame[0]
	}
	printf("subband_init  init\n");
	*/
	gop->cur_stream_frame = 0;
	gop->cur_gop_frame = 0;
	return gop;
}

void walet_decoder_free(GOP *gop)
{
}

void walet_encoder_free(GOP *gop)
{
}

uint32 walet_write_stream(GOP *gop, uint32 num, const char *filename)
{
    FILE *wl;
    WaletHeader wh;
    uint32 i;
    uint64 size = 0;

    wh.marker	= 0x776C;
    wh.w		= gop->w;
    wh.h		= gop->h;
    wh.color	= gop->color;
    wh.bg		= gop->bg;
    wh.bpp		= gop->bpp;
    wh.steps	= gop->steps;
    wh.gop_size	= gop->gop_size;
    wh.rates	= gop->rates;
	wh.comp		= gop->comp;
	wh.fb		= gop->fb;

	wl = fopen(filename, "wb");
    if(wl == NULL) {
    	printf("Can't write to file %s\n", filename);
    	return;
    }
    //Write header
    fwrite (&wh, sizeof(wh), 1, wl); size += sizeof(wh);
    //Write frames
    for(i=0; i < num; i++) size += frame_write(gop, i, wl);
    //Close file
    fclose(wl);
    printf("Write size = %Ld\n", size);
	printf("w = %d h = %d color = %d bg = %d bpp = %d step = %d gop = %d rates = %d comp = %d fb = %d\n",
			wh.w, wh.h, wh.color, wh.bg, wh.bpp, wh.steps, wh.gop_size, wh.rates, wh.comp, wh.fb);

    return size;
}

uint32 walet_read_stream(GOP **gop, uint32 num, const char *filename)
{
    FILE *wl;
    WaletHeader wh;
    uint32 i;
    uint64 size = 0;

	wl = fopen(filename, "rb");
    if(wl == NULL) {
    	printf("Can't open file %s\n", filename);
    	return;
    }

    //Read header
    if(fread(&wh, sizeof(wh), 1, wl) != 1) { printf("Header read error\n"); return; }
    size += sizeof(wh);
    printf("size = %Ld\n", size);
    if(wh.marker != 0x776C ) { printf("It's not walet format file\n"); return; }
    *gop = walet_decoder_init(wh.w, wh.h, wh.color, wh.bg, wh.bpp, wh.steps, wh.gop_size, wh.rates, wh.comp, wh.fb);
	printf("w = %d h = %d color = %d bg = %d bpp = %d step = %d gop = %d rates = %d comp = %d fb = %d\n",
			wh.w, wh.h, wh.color, wh.bg, wh.bpp, wh.steps, wh.gop_size, wh.rates, wh.comp, wh.fb);

    //Write frames
    for(i=0; i < num; i++) size += frame_read(*gop, i, wl);
    //Close file
    fclose(wl);
    printf("Read size = %Ld\n", size);

    return size;
}


