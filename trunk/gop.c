#include <walet.h>

#include <stdio.h>
#include <stdlib.h>

void walet_init(StreamData *sd, GOP *gop)
{
	int i;
	//Temp buffer init
	gop->buf = (imgtype *)calloc(sd->size.x*sd->size.y, sizeof(imgtype));
	printf("Buffer init\n");
	//Frames init
	gop->frames = (Frame *)calloc(sd->gop_size, sizeof(Frame));
	printf("Frames  create\n");
	for(i=0; i<sd->gop_size; i++) frames_init(&gop->frames[i], &sd->size, sd->color, sd->bits);
	printf("Frames  init\n");
	//Subband init
	if(sd->color == BAYER) 	subband_init_bayer(gop->sub   , sd->size.x, sd->size.y, sd->steps, sd->bits);
	else 					subband_init	  (gop->sub[0], sd->size.x, sd->size.y, sd->steps, sd->bits);
	if(sd->color == CS444 || sd->color == RGB) 	subband_init(gop->sub[1], sd->size.x   , sd->size.y, sd->steps, sd->bits);
	if(sd->color == CS422) 						subband_init(gop->sub[1], sd->size.x>>1, sd->size.y, sd->steps, sd->bits);
}
