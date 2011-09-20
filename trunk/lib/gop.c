#include <walet.h>

#include <stdio.h>
#include <stdlib.h>

void walet_decoder_init(GOP *gop, WaletConfig *wc)
{
}


void walet_encoder_init(GOP *gop, WaletConfig *wc)
{
	int i;
	//gop = (GOP*) malloc(sizeof(GOP));

	//Temp buffer init
	gop->buf = (uint8 *)calloc(wc->w*wc->h<<3, sizeof(uint8));
	wc->bpp = (wc->bpp < 15) ? wc->bpp : 14;
	gop->ibuf = (int *)calloc((1<<(wc->bpp+2))*8, sizeof(int));


	printf("Buffer init\n");
	//Frames init
	gop->frames = (Frame *)calloc(wc->gop_size, sizeof(Frame));

	for(i=0; i < wc->gop_size; i++) frames_init(gop, i, wc);
	printf("Frames  init\n");

	//Segmentation parts init
	gop->subs.w  = wc->w>>1;
	gop->subs.h = wc->h>>1;
	gop->subs.pic = (uint8 *)calloc(gop->subs.w*gop->subs.h, sizeof(uint8));
	gop->grad.w  = wc->w>>1;
	gop->grad.h = wc->h>>1;
	gop->grad.pic = (uint8 *)calloc(gop->grad.w*gop->grad.h, sizeof(uint8));
	gop->con.w  = wc->w>>1;
	gop->con.h = wc->h>>1;
	gop->con.pic = (uint8 *)calloc(gop->con.w*gop->con.h, sizeof(uint8));

	gop->cur_stream_frame = 0;
	gop->cur_gop_frame = 0;
	gop->state = 1;
}
//TODO: Free all memory
void walet_decoder_free(GOP *gop)
{
}

void walet_encoder_free(GOP *gop)
{
}

uint32 walet_write_stream(GOP *gop, WaletConfig *wc, uint32 num, const char *filename)
{
    FILE *wl;
    uint32 i;
    uint64 size = 0;
    uint16 marker = 0x776C;

    wl = fopen(filename, "wb");
    if(wl == NULL) {
    	printf("Can't write to file %s\n", filename);
    	return;
    }
    //Write marker
    fwrite (&marker, sizeof(marker), 1, wl); size += sizeof(marker);
    //Write header
    fwrite (wc, sizeof(*wc), 1, wl); size += sizeof(*wc);
    //Write frames
    for(i=0; i < num; i++) size += frame_write(gop, i, wc, wl);
    //Close file
    fclose(wl);
    printf("Write size = %Ld\n", size);
	printf("w = %d h = %d color = %d bg = %d bpp = %d step = %d gop = %d rates = %d comp = %d fb = %d rt = %d mv = %d\n",
			wc->w, wc->h, wc->color, wc->bg, wc->bpp, wc->steps, wc->gop_size, wc->rates, wc->comp, wc->fb, wc->rt, wc->mv);

    return size;
}

uint32 walet_read_stream(GOP *gop, WaletConfig *wc, uint32 num, const char *filename)
{
    FILE *wl;
    uint32 i;
    uint64 size = 0;
    uint16 marker;

	wl = fopen(filename, "rb");
    if(wl == NULL) {
    	printf("Can't open file %s\n", filename);
    	return;
    }

    //Read header
    if(fread(&marker, sizeof(marker), 1, wl) != 1) { printf("Not a walet file\n"); return; }
    if(marker != 0x776C ) { printf("It's not walet file\n"); return; }
    size += sizeof(marker);
    //Read header
    if(fread(wc, sizeof(*wc), 1, wl) != 1) { printf("Header read error\n"); return; }
    size += sizeof(*wc);
    //printf("size = %Ld\n", size);

    if(gop->state != 1) walet_encoder_init(gop, wc);
	printf("w = %d h = %d color = %d bg = %d bpp = %d step = %d gop = %d rates = %d comp = %d fb = %d rt = %d mv = %d\n",
			wc->w, wc->h, wc->color, wc->bg, wc->bpp, wc->steps, wc->gop_size, wc->rates, wc->comp, wc->fb, wc->rt, wc->mv);

    //Write frames
    for(i=0; i < num; i++) size += frame_read(gop, i, wc, wl);
    //Close file
    fclose(wl);
    printf("Read size = %Ld\n", size);

    return size;
}


