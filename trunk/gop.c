#include <walet.h>

#include <stdio.h>
#include <stdlib.h>

void walet_init(StreamData *sd, GOP *gop)
{
	int i;
	//Temp buffer init
	gop->buf = (imgtype *)calloc(sd->size.x*sd->size.y, sizeof(imgtype));
	gop->q = (int *)calloc(1<<(sd->bits+2), sizeof(int));
	printf("Buffer init\n");
	//Frames init
	gop->frames = (Frame *)calloc(sd->gop_size, sizeof(Frame));
	printf("Frames  create\n");
	for(i=0; i<sd->gop_size; i++) frames_init(&gop->frames[i], &sd->size, sd->color, sd->bits, sd->steps, gop->buf);
	printf("Frames  init\n");

	//Subband init

	subband_init(gop->sub, 0, sd->color, sd->size.x, sd->size.y, sd->steps, sd->bits, gop->q);
	gop->frames[0].img[0].sub = gop->sub[0];

	if(sd->color == CS444 || sd->color == RGB) 	{
		subband_init(gop->sub, 1, sd->color, sd->size.x   , sd->size.y, sd->steps, sd->bits, gop->q);
		subband_init(gop->sub, 2, sd->color, sd->size.x   , sd->size.y, sd->steps, sd->bits, gop->q);
		gop->frames[0].img[1].sub = gop->sub[1];
		gop->frames[0].img[2].sub = gop->sub[2];
	}
	if(sd->color == CS422){
		subband_init(gop->sub, 1, sd->color, sd->size.x>>1, sd->size.y, sd->steps, sd->bits, gop->q);
		subband_init(gop->sub, 2, sd->color, sd->size.x>>1, sd->size.y, sd->steps, sd->bits, gop->q);
		gop->frames[0].img[1].sub = gop->sub[1];
		gop->frames[0].img[2].sub = gop->sub[2];
	}

}
