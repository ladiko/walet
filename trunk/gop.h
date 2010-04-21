#ifndef _GOP_H_
#define _GOP_H_

typedef struct {
	imgtype			*buf;		//Temporal buffer for DWT, IDW, and range coder
	Frame			*frames;	//Pointer to frames array
	Subband			*sub[4];	//Subband location and size structure
	int 			*q;			//Quatntization value array
}	GOP;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void walet_init(StreamData *sd, GOP *gop);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //_GOP_HH_
