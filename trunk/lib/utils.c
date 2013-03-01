#include <walet.h>
#include <stdio.h>
#include <stdlib.h>
//#include <malloc.h>
#include <string.h>
#include <math.h>
#if !defined(__APPLE__)
#include <malloc.h>
#endif

#define YY(r, g, b) (((306*((r) - (g)) + 117*((b) - (g)))>>10) + (g))
#define clip(x) (((x) > 255) ? 255 : (x))

/** \brief Copy image from the buffer
    \param in       The input buffer.
    \param out		The output image.
    \param w		The image width.
    \param h		The image height.
    \param bpp		The bits per pixel.
*/
void utils_image_copy_n(uint8 *in, int16 *out, uint32 w, uint32 h, uint32 bpp)
{
    uint32 i, size = w*h;
    //uint8 *in1 = &in[1];

    if(bpp > 8){
        for(i=0; i<size; i++) {
            //For Aptina sensor
            //out[i] = ((in[(i<<1)+1]<<8) | in[(i<<1)]);
            //For Sony sensor
            out[i] = ((in[(i<<1)]<<8) | in[(i<<1)+1]);
            //img[i] = ((buff[(i<<1)]<<8) | buff[(i<<1)+1]);
            //printf("MSB = %d LSB = %d img = %d shift = %d\n", buff[(i<<1)], buff[(i<<1)+1], ((buff[(i<<1)]) | buff[(i<<1)+1]<<8), shift);
        }
    } else
        for(i=0; i<size; i++) out[i] = in[i];
}

/**	\brief Draw a gray 8 bits image
    \param in	 	The input image.
    \param out	 	The output RGB buffer.
    \param w 		The image width.
    \param h 		The image height.
    \retval         The output RGB buffer.
*/
uint8* utils_grey_draw8_n(uint8 *in, uint8 *out, uint32 w, uint32 h)
{
    int i, j, dim = h*w*3;
    for(i = 0,  j= 0; j < dim; j+=3, i++){
        out[j]     = in[i];
        out[j + 1] = in[i];
        out[j + 2] = in[i];
    }
    return out;
}

/**	\brief Transform 16 bits image to grey 8 bits in rgb24 format.
    \param in	 		The input 16 bits image.
    \param out	 		The output rgb24 image.
    \param w 			The image width.
    \param h 			The image height.
    \param bpp          The bits per pixel.
    \param par          If 0 - direct, 1 - with scale, 2 - with scale and shift,
                        3 - from min to max, 4 - with shift from min to max.
    \retval             The output RGB buffer.
*/
uint8* utils_gray16_rgb8(int16 *in, uint8 *out, uint32 w, uint32 h, uint32 bpp, uint32 par)
{
    int i, j, sz = w*h, dim = h*w*3, sh = bpp - 8, shift = 1<<(bpp-1);
    int df, tmp, max, min;

    if(par == 0){
        for(i = 0,  j= 0; j < dim; j+=3, i++) out[j] = out[j+1] = out[j+2] = in[i];
    } else if(par == 1){
        for(i = 0,  j= 0; j < dim; j+=3, i++) out[j] = out[j+1] = out[j+2] = in[i] >> sh;
    } else if(par == 2){
        for(i = 0,  j= 0; j < dim; j+=3, i++) out[j] = out[j+1] = out[j+2] = (in[i] + shift) >> sh;
    } else if(par == 3){
        max = in[0]; min = in[0];
        for(i=1; i < sz; i++){
            if      (in[i] > max) max = in[i];
            else if (in[i] < min) min = in[i];
        }
        df = max - min;
        printf("min = %d max = %d \n", min, max);
        for(i = 0,  j= 0; j < dim; j+=3, i++){
            tmp = ((in[i] - min)<<8)/df;
            out[j] = out[j+1] = out[j+2] = tmp;
        }

    } else if(par == 4){
        max = in[i]; min = in[i];
        for(i=1; i < sz; i++){
            if      (in[i] > max) max = in[i];
            else if (in[i] < min) min = in[i];
        }
        df = max - min;

        for(i = 0,  j= 0; j < dim; j+=3, i++){
            tmp = ((in[i] - min + shift)<<8)/df;
            out[j] = out[j+1] = out[j+2] = tmp;
        }
    }
    return out;
}

/**	\brief Transform 16 bits rgb24 image to grey 8 bits rgb24 image.
    \param in	 		The input 16 bits rgb24 image.
    \param out	 		The output 8 bits rgb24 image.
    \param w 			The image width.
    \param h 			The image height.
    \param bpp          The bits per pixel.
    \param par          If 0 - direct, 1 - with scale, 2 - with scale and shift,
                        3 - from min to max, 4 - with shift from min to max.
    \retval             The output RGB buffer.
*/
uint8* utils_rgb16_rgb8(int16 *in, uint8 *out, uint32 w, uint32 h, uint32 bpp, uint32 par)
{
    int i, j, sz = w*h, dim = h*w*3, sh = bpp - 8, shift = 1<<(bpp-1);
    int df, tmp, max, min;

    if(par == 0){
        for(i = 0; i < dim; i++) out[i] = in[i];
    } else if(par == 1){
        for(i = 0; i < dim; i++) out[i] = in[i] >> sh;
    } else if(par == 2){
        for(i = 0; i < dim; i++) out[i] = (in[i] + shift) >> sh;
    } else if(par == 3){
        max = in[0]; min = in[0];
        for(i=1; i < dim; i++){
            if      (in[i] > max) max = in[i];
            else if (in[i] < min) min = in[i];
        }
        df = max - min;
        printf("min = %d max = %d \n", min, max);
        for(i = 0; i < dim; i++) out[i] = ((in[i] - min)<<8)/df;
    } else if(par == 4){
        max = in[i]; min = in[i];
        for(i=1; i < dim; i++){
            if      (in[i] > max) max = in[i];
            else if (in[i] < min) min = in[i];
        }
        df = max - min;

        for(i = 0; i < dim; i++) out[i] = ((in[i] - min + shift)<<8)/df;
    }
    return out;
}

/**	\brief Zoom out image twice.
    \param in	 		The input image.
    \param out	 		The output image.
    \param buff	 		The temporary buffer, should include 1 row.
    \param w 			The image width.
    \param h 			The image height.
*/
void utils_resize_down_2x(uint8 *in, uint8 *out, uint8 *buff, uint32 w, uint32 h)
{
    int x, y, yw, yx, yw1, w1 = w>>1, h1 = h>>1;
    int16 *l = (int16*)buff;

    for(y=0; y < h1; y++){
        yw = y*w1;
        yw1 = (y<<1)*w;
        for(x=0; x < w1; x++) l[x]  = in[yw1 + (x<<1)] + in[yw1 + (x<<1)+1];
        yw1 = yw1 + w;
        for(x=0; x < w1; x++) l[x] += in[yw1 + (x<<1)] + in[yw1 + (x<<1)+1];

        for(x=0; x < w1; x++){
            yx = yw + x;
            out[yx] = l[x]>>2;
        }
    }
}

/**	\brief Zoom out the gray 16 bits image.
    \param in	 		The input image.
    \param out	 		The output image.
    \param buff	 		The temporary buffer, should include 1 row of image.
    \param zoom 		The zoom parameter 1 - no zoom, 2 - twice, 3 - three times ...
    \param w 			The image width.
    \param h 			The image height.
*/
void utils_zoom_out_gray(uint16 *in, uint16 *out, uint32 *buff, uint32 zoom, uint32 w, uint32 h)
{
    int i, j, x, x1, y, y1, yw, yx, sq = zoom*zoom, w1 = w/zoom;
    uint32 max = 1<<31, sh = 0;

    memset(buff, 0, sizeof(uint32)*w1);
    //Find zoom value when / can changed to >>
    for(i=2; i < max; i<<=1) if((i|zoom) == i) sh++;

    for(y=0, y1=0; y < h; y+=zoom, y1++){

        for(j=0; j < zoom; j++){
            yw = (y+j)*w;
            for(x=0, x1=0; x < w; x+=zoom, x1++){
                yx = yw + x;
                for(i=0; i < zoom; i++) buff[x1] += in[yx+i];
                if(j == zoom-1) {
                    out[y1*w1 + x1] =  sh ? buff[x1]>>zoom : buff[x1]/sq;
                    buff[x1] = 0;
                }
            }
        }
    }
}

/**	\brief Zoom out the rgb24 16 bits image.
    \param in	 		The input image.
    \param out	 		The output image.
    \param buff	 		The temporary buffer, should include 1 row of image.
    \param zoom 		The zoom parameter 1 - no zoom, 2 - twice, 3 - three times ...
    \param shift 		The value removed from each color
    \param w 			The image width.
    \param h 			The image height.
*/
void utils_zoom_out_rgb16_to_rgb16(uint16 *in, uint16 *out, uint32 *buff, uint32 zoom, uint32 shift, uint32 w, uint32 h)
{
    int i, j, x, x1, y, y1, yw, yx, yxi, sq = zoom*zoom, w1 = w/zoom;
    uint32 max = 1<<31, sh = 0;
    uint32 *buff1, *buff2;

    buff1 = &buff[w1]; buff2 = &buff1[w1];

    memset(buff, 0, sizeof(uint32)*w1*3);

    //Find zoom value when / can changed to >>
    for(i=2; i < max; i<<=1) if((i|zoom) == i) sh++;

    for(y=0, y1=0; y < h; y+=zoom, y1++){

        for(j=0; j < zoom; j++){
            yw = (y+j)*w;
            for(x=0, x1=0; x < w; x+=zoom, x1++){
                yx = yw + x;
                for(i=0; i < zoom; i++) {
                    yxi = (yx+i)*3;
                    buff [x1] += (in[yxi  ] - shift);
                    buff1[x1] += (in[yxi+1] - shift);
                    buff2[x1] += (in[yxi+2] - shift);
                }
                if(j == zoom-1) {
                    yxi = (y1*w1+x1)*3;
                    out[yxi  ] =  sh ? buff [x1]>>zoom : buff [x1]/sq;
                    out[yxi+1] =  sh ? buff1[x1]>>zoom : buff1[x1]/sq;
                    out[yxi+2] =  sh ? buff2[x1]>>zoom : buff2[x1]/sq;
                    buff[x1] = buff1[x1] = buff2[x1] = 0;
                }
            }
        }
    }
}

/**	\brief Zoom out of bayer image and convert to rgb24 format.
    \param in	 	The input bayer image.
    \param out	 	The output image in r,g,b,r1,g1,b1... format.
    \param buff	 	The temporary buffer, should include 2 row of bayer image.
    \param zoom 	The zoom parameter 1 - 2x, 2 - 4x, 3 - 6x times ...
    \param bay		The Bayer grids pattern.
    \param w 		The image width.
    \param h 		The image height.
*/
void utils_zoom_out_bayer16_to_rgb16(uint16 *in, uint16 *out, uint32 *buff, uint32 zoom, BayerGrid bay, uint32 w, uint32 h)
{
    /*
       All RGB cameras use one of these Bayer grids:

        BGGR  0         GRBG 1          GBRG  2         RGGB 3
          0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5
        0 B G B G B G	0 G R G R G R	0 G B G B G B	0 R G R G R G
        1 G R G R G R	1 B G B G B G	1 R G R G R G	1 G B G B G B
        2 B G B G B G	2 G R G R G R	2 G B G B G B	2 R G R G R G
        3 G R G R G R	3 B G B G B G	3 R G R G R G	3 G B G B G B
     */
    int i, j, x, x1, y, y1, yw, yx, sq = zoom*zoom, zoom2 = zoom<<1, w1 = w/zoom2;
    uint32 max = 1<<31, sh = 0;
    uint32 *c[4];   //Three color buffer

    c[0] = buff; c[1] = &c[0][w1]; c[2] = &c[1][w1]; c[3] = &c[2][w1];

    memset(c[0], 0, sizeof(uint32)*w1<<2);

    //Find zoom value when / can changed to >>
    for(i=1; i < max; i<<=1) if((i|zoom) == i) sh++;
    zoom = (zoom == 1) ? 0 : zoom;
    printf("zoom = %d sh = %d\n", zoom, sh);

    for(y=0, y1=0; y < h; y+=zoom2, y1++){

        for(j=0; j < zoom2; j+=2){
            yw = (y+j)*w;
            for(x=0, x1=0; x < w; x+=zoom2, x1++){
                yx = yw + x;
                for(i=0; i < zoom2; i+=2) {
                    c[0][x1] += in[yx+i];
                    c[1][x1] += in[yx+i+1];
                    c[2][x1] += in[yx+i+w];
                    c[3][x1] += in[yx+i+w+1];
                }
                if(j == zoom2-2) {
                    switch(bay){
                    case(BGGR):{
                        out[(y1*w1+x1)*3]   = sh ? c[3][x1]>>zoom : c[3][x1]/sq;
                        out[(y1*w1+x1)*3+1] = sh ? (c[1][x1]+c[2][x1])>>(zoom+1) : (c[1][x1]+c[2][x1])/(sq*2);
                        out[(y1*w1+x1)*3+2] = sh ? c[0][x1]>>zoom : c[0][x1]/sq;
                        break;
                    }
                    case(GRBG):{
                        out[(y1*w1+x1)*3]   = sh ? c[1][x1]>>zoom : c[01][x1]/sq;
                        out[(y1*w1+x1)*3+1] = sh ? (c[0][x1]+c[3][x1])>>(zoom+1) : (c[0][x1]+c[3][x1])/(sq*2);
                        out[(y1*w1+x1)*3+2] = sh ? c[2][x1]>>zoom : c[2][x1]/sq;
                        break;
                    }
                    case(GBRG):{
                        out[(y1*w1+x1)*3]   = sh ? c[2][x1]>>zoom : c[2][x1]/sq;
                        out[(y1*w1+x1)*3+1] = sh ? (c[0][x1]+c[3][x1])>>(zoom+1) : (c[0][x1]+c[3][x1])/(sq*2);
                        out[(y1*w1+x1)*3+2] = sh ? c[1][x1]>>zoom : c[1][x1]/sq;
                        break;
                    }
                    case(RGGB):{
                        out[(y1*w1+x1)*3]   = sh ? c[0][x1]>>zoom : c[0][x1]/sq;
                        out[(y1*w1+x1)*3+1] = sh ? (c[1][x1]+c[2][x1])>>(zoom+1) : (c[1][x1]+c[2][x1])/(sq*2);
                        out[(y1*w1+x1)*3+2] = sh ? c[3][x1]>>zoom : c[3][x1]/sq;
                        break;
                    }
                    }
                    c[0][x1] = c[1][x1] = c[2][x1] = c[3][x1] = 0;
                }
            }
        }
    }
}

/**	\brief Calculate the white balance multiplier for read and blue color of 16 bits rgb24 image.
    \param in	The input 16 bits rgb24 image.
    \param rm   The pointer to the red multiplier.
    \param bm   The pointer to the blue multiplier.
    \param w    The image width.
    \param h 	The image height.
*/
void utils_wb(int16 *in, float *rm, float *bm, uint32 w, uint32 h)
{
    int i, j, sz = w*h, size3 = h*w*3;
    uint32 d, d1, r = 0, g = 0, b = 0, cn = 0, min, max, mx, Y, hs = 4096, sum, ts = sz*2/10;
    float s = 0.01, m, mr, mb, th = 0.5;
    uint32 hi[hs];

    //Gray world algorithm the first step of iteration
    min = max = in[1];
    for(i = 0; i < size3; i+=3) {
        r += in[i  ];
        g += in[i+1];
        b += in[i+2];
        Y = (306*in[i] + 601*in[i+1] + 117*in[i+2])>>10;
        hi[Y]++;

        if(Y < min) min = Y;
        else if(Y > max) max = Y;
    }

    sum = 0;
    for(i=0; i < hs; i++) {
        sum += hi[i];
        if(sum > ts) break;
    }

    mx = i;

    r = r/sz; g = g/sz; b = b/sz;
    mr = (double)g/(double)r;
    mb = (double)g/(double)b;
    printf("mr = %f mb = %f\n",mr, mb);

    // New algorithm for white balancing
    //Get only pixeles with (G-R)/G and (G-B)/G difference less then threshoud

    for(i = 0; i < size3; i+=3) {
        Y = (306*in[i] + 601*in[i+1] + 117*in[i+2])>>10;

        if(Y < mx) in[i] = in[i+1] = in[i+2] = 0;
    }

    printf("cn = %d sz = %d p = %f\n", cn, sz, (double)(sz - cn)/(double)sz);

    //Red color
    d = 0; m = mr;
    for(i = 0; i < size3; i+=3) d += abs(in[i+1] - in[i]*m);
    for(j=0; ;j++){
        m = m + s;
        d1 = 0;
        for(i = 0; i < size3; i+=3) d1 += abs(in[i+1] - in[i]*m);
        printf("j = %d d = %d d1 = %d m = %f\n", j, d, d1, m);
        if(!j && d1 > d) s = -s;
        if( j && d1 > d) break;
        d = d1;
    }
    *rm = m;

    //Blue color
    d = 0; m = mb;
    for(i = 0; i < size3; i+=3) d += abs(in[i+1] - in[i+2]*m);
    for(j=0; ;j++){
        m = m + s;
        d1 = 0;
        for(i = 0; i < size3; i+=3) d1 += abs(in[i+1] - in[i+2]*m);
        printf("j = %d d = %d d1 = %d m = %f\n", j, d, d1, m);
        if(!j && d1 > d) s = -s;
        if( j && d1 > d) break;
        d = d1;
    }
    *bm = m;
}

/**	\brief White balance 16 bits rgb24 image.
    \param in	The input 16 bits rgb24 image.
    \param out	The output 16 bits rgb24 image.
    \param buff	The temporary buffer.
    \param bits The image bits per pixel.
    \param w    The image width.
    \param h 	The image height.
*/
void utils_wb_rgb24(int16 *in, int16 *out, int16 *buff, uint32 bits, uint32 w, uint32 h)
{
    int i, j, sz = w*h, size3 = h*w*3, sh = 3, zoom = 1<<sh, w1 = w>>sh, h1 = h>>sh, max = (1<<bits)-1;
    float rm, bm;
    //Find the lowest color value
    //for(i=1; i < size3; i++) if (in[i] < min) min = in[i];

    utils_zoom_out_rgb16_to_rgb16(in, buff, (uint32*)&buff[w1*h1*3], zoom, 0, w, h);

    utils_wb(buff, &rm, &bm, w1, h1);
    printf("rm = %f bm = %f\n", rm, bm);
    //rm = 2.578; bm = 1.430;

    for(i = 0; i < size3; i+=3) {
        out[i]   = in[i]*rm;    out[i] = out[i] > max ? max : out[i];
        out[i+1] = in[i+1];
        out[i+2] = in[i+2]*bm;  out[i+2] = out[i+2] > max ? max : out[i+2];
    }
}

/**	\brief Transform image in rgb24 format to 8bits in the same format
    \param in	 	The input image.
    \param out	 	The output image.
    \param buff     The temporary buffer (size = 6*(1<<bits)).
    \param bits     The input image bits per pixel.
    \param b        If b = 0 - liniar, b = 100 - integral image transform.
    \param w        The image width.
    \param h        The image height.
*/
void utils_transorm_to_8bits(const int16 *in, uint8 *out, uint8 *buff, const uint32 bits,
                             const uint32 b, const uint32 w, const uint32 h)
{
    int y, y1, i, j, st, lp, df = bits-8, hmax = 1<<bits, sum, sum1, low, top, mll, tmp, sp;
    int  size = w*h, size3 = h*w*3, max, min, d = 1<<8, sh = 16;
    double lowt = 0.01, topt = 0.01, a;

    //The LUT array hmax bytes
    //uint8 *lut = buff;
    //Histogramm array 4*hmax bytes
    uint32 *hist = (uint32*)buff;
    //The multiplier array 2*hmax bytes
    int16 *ml = (int16*)&hist[hmax];
    int p[d+1];

    //memset(lut, 0, sizeof(uint8)*hmax);
    memset(hist, 0, sizeof(uint32)*hmax);
    memset(p, 0, sizeof(int)*(d+1));

    //for(i=0; i < size3; i++) hist[in[i]]++;

    for(i=0; i < size3; i+=3) {
        //printf("YY = %d", YY(in[i], in[i+1], in[i+2]));
        //hist[YY(in[i], in[i+1], in[i+2])]++;
        hist[in[i]]++; hist[in[i+1]]++; hist[in[i+2]]++;
    }
    //for(i = 0; i < hmax; i++) printf("%d hist = %d \n", i, hist[i]);

    //New algorithm
    //Find min and max value of histogram
    for(i=0; !hist[i]; i++);
    p[0] = i;
    printf("min = %d\n", i);
    for(i=hmax-1; !hist[i]; i--);
    p[d] = i;
    printf("max = %d\n", i);

    for(st=1, d=256; st < 9; st++, d>>=1){
        for(j=0; j < 256; j+=d){
            for(i=p[j], sum1=0; i < p[j+d]; i++) sum1 += hist[i];
            //Remove holes in histogramm
            sp = sum1/d;
            for(i=p[j]; i < p[j+d]; i++) if(hist[i] > sp) { hist[i] = sp; sum1 -= (sp-hist[i]); }

            sum1>>=1;
            for(i=p[j], sum=0; sum < sum1; i++){
                sum += hist[i];
                //printf("i = %d sum = %d\n", i, sum);
            }
            lp = (p[j+d] + p[j])>>1;
            p[j+(d>>1)] = lp + (i - lp)*(int)b/100;
            //printf("size = %d st = %d d = %d j = %d lp = %d ip = %d p[%d] = %d p[%d] = %d p[%d] = %d\n",
            //       size3>>st, st, d, j, lp, i, j+d, p[j+d], j, p[j], j+(d>>1), p[j+(d>>1)]);
        }
    }
    //for(i = 0; i <= 256; i++) printf("p[%d] = %d\n", i, p[i]);

    //Make LUT
    for(j=0; j < 256; j++){
        //mll = (j<<sh)/p[j+1];
        for(i=p[j]; i < p[j+1]; i++){
            //lut[i] = j;
            if(i) ml[i] = (j<<sh)/i;
            //ml[i] = mll;
        }
    }
    ml[p[j]] = ml[p[j]-1];
    printf("p[255] = %d\n", p[255]);
    //ml[p[j]] = mll;
    //lut[p[j]] = 255;

    //Make LUT table liniar
    /*
    sum = 0;
    for(i=0; (double)sum/(double)size < lowt ; i++) sum += hist[i];
    low = i;
    sum = 0;
    for(i=hmax-1; (double)sum/(double)size < topt ; i--) sum += hist[i];
    top = i;

    a = 255./(double)(top - low);

    for(i = 0; i < low; i++) lut[i] = 0;
    for(i = low; i < top; i++) lut[i] = (uint32)(a*(double)(i-low));
    for(i = top; i < hmax; i++) lut[i] = 255;


    //Make LUT table integral
    b = (1<<30)/size3;

    //Check if one bin in historgamm more then one bin in LUT------------------------------------
    sum = 0;
    max = size3>>8;

    for(i = 0; i < hmax; i++) {
        if(hist[i] > max) {
            sum += hist[i] - max;
            hist[i] = max;
        }
    }
    b = (1<<30)/(size3 - sum);

    //---------------------------------------------------------------------------------------------
    sum = 0;
    for(i = 0; i < hmax; i++) { sum += hist[i]; lut[i] = sum*b>>22; }
    */

    //for(i = 0; i < hmax; i++) printf("%d hist = %d ml = %d\n", i, hist[i], ml[i]);

    for(i=0; i < size3; i+=3) {
        /*
        y = YY(in[i], in[i+1], in[i+2]);
        tmp = in[i  ]*ml[y]>>sh; out[i  ] = (tmp > 255) ? 255 : tmp;// ((tmp < 0) ? 0 : tmp);
        tmp = in[i+1]*ml[y]>>sh; out[i+1] = (tmp > 255) ? 255 : tmp;// ((tmp < 0) ? 0 : tmp);
        tmp = in[i+2]*ml[y]>>sh; out[i+2] = (tmp > 255) ? 255 : tmp;// ((tmp < 0) ? 0 : tmp);
        */
        tmp = in[i  ]*ml[in[i  ]]>>sh; out[i  ] = (tmp > 255) ? 255 : tmp;// ((tmp < 0) ? 0 : tmp);
        tmp = in[i+1]*ml[in[i+1]]>>sh; out[i+1] = (tmp > 255) ? 255 : tmp;// ((tmp < 0) ? 0 : tmp);
        tmp = in[i+2]*ml[in[i+2]]>>sh; out[i+2] = (tmp > 255) ? 255 : tmp;// ((tmp < 0) ? 0 : tmp);

    }

    //for(i=0; i < size3; i++) out[i] = lut[in[i]];
}

/**	\brief Image transform.
    \param img	 		The input image.
    \param img1	 		The output image.
    \param look 		The LUT.
    \param w            The image width.
    \param h            The image height.
*/
void utils_bits12to8(int16 *img, uint8 *img1, uint32 *look, uint32 w, uint32 h)
{
    uint32 i, sz = w*h;
    for(i=0; i < sz; i++) img1[i] = look[img[i]];
}

void utils_resize_down_2x_(uint8 *in, uint8 *out, uint8 *buff, uint32 w, uint32 h)
{
    int x, y, yw, yx, yw1, yw2, wn, hn;
    int16 *l = (int16*)buff;

    wn = ((w-2)>>1); hn = ((h-2)>>1);

    for(y=0; y < hn; y++){
        yw = (y+1)*(wn+2);
        yw1 = (y<<1)*w + w;
        for(x=0; x < wn; x++) l[x]  = in[yw1 + (x<<1)+1] + in[yw1 + (x<<1)+2];
        yw1 = yw1 + w;
        for(x=0; x < wn; x++) l[x] += in[yw1 + (x<<1)+1] + in[yw1 + (x<<1)+2];

        for(x=0; x < wn; x++){
            yx = yw + x+1;
            out[yx] = l[x]>>2;
        }
    }
}

/**	\brief Resize image up to two times on x and y (bilinear interpolation).
    \param in	 		The input image.
    \param out	 		The output image.
    \param buff	 		The temporary buffer, should include 1 row.
    \param w 			The width of bigger image.
    \param h 			The height of bigger image.
*/
void utils_resize_up_2x(int16 *in, int16 *out, int16 *buff, uint32 w, uint32 h)
{
    int x, x2, y, yw, yx, yw1, w2 = (w>>1) + (w&1);
    int16 *l0 = buff, *l1 = &buff[w2+2], *l2 = &buff[(w2+2)<<1], *tm;

    l0[0] = in[0]; for(x=0; x < w2; x++) l0[x+1] = in[x]; l0[x+1] = in[x-1];
    l1[0] = in[0]; for(x=0; x < w2; x++) l1[x+1] = in[x]; l1[x+1] = in[x-1];
    for(y=0; y < h; y++){
        yw = y*w;
        if(!(y&1)) {
            if(y > 0) { tm = l0; l0 = l1; l1 = l2; l2 = tm; }
            yw1 = (y == h-1) ? (y>>1)*w2 : ((y>>1)+1)*w2;
            l2[0] = in[yw1]; for(x=0; x < w2; x++) l2[x+1] = in[x+yw1]; l2[x+1] = in[x-1+yw1];
            //printf("y = %d yw1 = %d \n", y, yw1);
        }
        for(x=0; x < w; x++){
            yx = yw + x;
            x2 = (x>>1) + 1;
            //out[yx] = l1[x2];
            if(!(x&1) && !(y&1)) out[yx] = (l1[x2]*9 + l1[x2-1]*3 + l0[x2]*3 + l0[x2-1])>>4;
            else if ((x&1) && !(y&1)) out[yx] = (l1[x2]*9 + l1[x2+1]*3 + l0[x2]*3 + l0[x2+1])>>4;
            else if (!(x&1) && (y&1)) out[yx] = (l1[x2]*9 + l1[x2-1]*3 + l2[x2]*3 + l2[x2-1])>>4;
            else out[yx] = (l1[x2]*9 + l1[x2+1]*3 + l2[x2]*3 + l2[x2+1])>>4;
        }
    }
}

/**	\brief Automatic Color Enhancement.
    \param in	The input 16 bits rgb24 image.
    \param out	The output 16 bits rgb24 image.
    \param buff	The temporary buffer.
    \param bits The image bits per pixel.
    \param w    The image width.
    \param h 	The image height.
*/
void utils_ACE(int16 *in, int16 *out, int16 *buff, uint32 bits, uint32 w, uint32 h)
{
    int x, x1, y, y1, yx, yx1, yw, yw1;
    int hs = 200, ws = 200, df;
    //int R, max = 0., min = 0.;
    double R = 0., max = 0., max1 = 0., dd;

    for(y1=-hs; y1 <= hs; y1+=1){
        for(x1=-ws; x1 <= ws; x1+=1){
            if(!(x1 == 0 && y1 == 0)){
                //df = 1;
                R += 1.;///sqrt((double)(y1*y1 + x1*x1));
            }
        }
    }
    max = R;
    printf("max = %f\n", max);

    for(y=hs; y < h-hs; y++){
        yw = y*w;
        for(x=ws; x < w-ws; x++){
            yx = yw + x;
            R = 0.;
            for(y1=y-hs; y1 <= y+hs; y1+=1){
                yw1 = y1*w;
                for(x1=x-ws; x1 <= x+ws; x1+=1){
                    yx1 = yw1 + x1;
                    if(!(x == x1 && y == y1)){
                        df = in[yx] - in[yx1];
                        //df =  (df < -3) ? -1 : ((df > 3) ? 1 : 0);
                        df =  (df < 0) ? -1 : ((df >= 0) ? 1 : 0);
                        if(df) R += (double)df;///sqrt((double)((y-y1)*(y-y1) + (x-x1)*(x-x1))));
                        //dd = (double)df/4096.;
                        //if(df) R += (dd/sqrt((double)((y-y1)*(y-y1) + (x-x1)*(x-x1))));
                    }
                }
            }
            //printf("R = %d\n", R);
            R = R/max;
            out[yx] = (uint8)(127.5 + 127.5*R);
            if(R > max1) max1 = R;
            //if(R < min) min = R;
            //else if(R > max) max = R;
            //printf("min = %d max = %d R = %d\n", min, max, R);

        }
    }
    printf("max = %f max1 = %f R = %f\n", max, max1, R);
    //printf("min = %d max = %d\n", min, max);
    //printf("min = %f max = %f\n", min, max);
}

/**	\brief Fast Automatic Color Enhancement algorithm.
    \param in	The input 16 bits rgb24 image.
    \param out	The output 16 bits rgb24 image.
    \param buff	The temporary buffer.
    \param bits The image bits per pixel.
    \param w    The image width.
    \param h 	The image height.
*/
void utils_ACE_fast(int16 *in, int16 *out, int16 *buff, uint32 bits, uint32 w, uint32 h)
{
    int x, x1, y, y1, yx, yx1, yw, yw1, hs = 1<<12;
    int df, sz = w*h;
    //int R, max = 0., min = 0.;
    double R = 0., max = 0., max1 = 0., dd;
    int *hi, *hl, *hr;
    int b = (1<<30)/sz;

    printf("b = %d\n", b);

    hi = (int*)buff; hl = &hi[hs]; hr = &hl[hs];

    //Fill historgam
    memset(hi, 0, sizeof(int)*hs);

    for(x=0; x < sz; x++) hi[in[x]]++;

    hl[0] = 0; hl[1] = hi[0]; for(x=2; x < hs; x++) hl[x] = hl[x-1] + hi[x-1];

    hr[hs-1] = 0; hr[hs-2] = hi[hs-1]; for(x=hs-3; x >= 0; x--) hr[x] = hr[x+1] + hi[x+1];

    //for(x=0; x < hs; x++) printf("%4d hi = %d hl = %d hr = %d\n", x, hi[x], hl[x], hr[x]);

    for(y=0; y < h; y++){
        yw = y*w;
        for(x=0; x < w; x++){
            yx = yw + x;
            //R = (double)(hl[in[yx]] - hr[in[yx]])/(double)sz;
            //out[yx] = (uint8)(127.5 + 127.5*R);
            out[yx] = 128 + (b*(hl[in[yx]] - hr[in[yx]])>>23);
        }
    }
    //printf("max = %f max1 = %f R = %f\n", max, max1, R);
    //printf("min = %d max = %d\n", min, max);
    //printf("min = %f max = %f\n", min, max);
}


#define hsh(w,x) ((x == -2) ? -w-w :(x == 2))

// Red and blue pattern for bayer median filter
int rb[9][2] = {
		{-2,-2,},{ 0,-2,},{ 2,-2,},    	// R   R   R
		{-2, 0,},{ 0, 0,},{ 2, 0,},		//
		{-2, 2,},{ 0, 2,},{ 2, 2,},		// R   R   R
};										//
										// R   R   R
// Green pattern for bayer median filter
int g[9][2] = {
		{ 0,-2,},{ 1,-1,},{ 2, 0,},		//     G
		{-1,-1,},{ 0, 0,},{ 1, 1,},		//   G   G
		{-2, 0,},{-1, 1,},{ 0,-2,},		// G   G   G
};										//   G   G
										//     G

#define ll(step, x, y) img[x*step + y*step*width];
#define hl(step, x, y) img[x*step + (step>>1)  + y*step*width];
#define lh(step, x, y) img[x*step + (step>>1)*width  + y*step*width];
#define hh(step, x, y) img[x*step + (step>>1) + (step>>1)*width  + y*step*width];

//#define clip(x)		abs(x);
#define lb(x) (((x+128) < 0) ? 0 : (((x+128) > 255) ? 255 : (x+128)))
#define lb1(x) (((x) < 0) ? 0 : (((x) > 255) ? 255 : (x)))
#define oe(a,x)	(a ? x&1 : (x+1)&1)
#define border(x,w)	(x) < 0 ? (-x) : (((x) < w) ? x : (w<<1) - (x) - 2)



static inline void drawrect(uint8 *rgb, int16 *pic, uint32 x0, uint32 y0, uint32 w, uint32 h, uint32 size, uint32 shift)
{
	uint32 x, y, tmp;
	for(y=0; y < h; y++ ){
		for(x=0; x < w; x++){
			//tmp = rnd(im[y*w+x] < 0 ? -im[y*w+x]<<1 : im[y*w+x]<<1);
			tmp = lb1(shift + pic[y*w+x]);
			//if(tmp > 255) printf("tmp = %d pic = %d\n", tmp, pic[y*w+x]);
			rgb[3*((y+y0)*size +x0 +x)]   = tmp; //rnd(shift+im[y*w+x]); //im[y*w+x] ? 255 : 0; //
			rgb[3*((y+y0)*size +x0 +x)+1] = tmp; //rnd(shift+im[y*w+x]); //im[y*w+x] ? 255 : 0; //
			rgb[3*((y+y0)*size +x0 +x)+2] = tmp; //rnd(shift+im[y*w+x]); //im[y*w+x] ? 255 : 0; //
		}
	}
}

static inline void drawrect8(uint8 *rgb, uint8 *pic, uint32 x0, uint32 y0, uint32 w, uint32 h, uint32 size, uint32 shift)
{
	uint32 x, y, tmp;
	for(y=0; y < h; y++ ){
		for(x=0; x < w; x++){
			//tmp = rnd(im[y*w+x] < 0 ? -im[y*w+x]<<1 : im[y*w+x]<<1);
			tmp = lb1(shift + pic[y*w+x]);
			//if(tmp > 255) printf("tmp = %d pic = %d\n", tmp, pic[y*w+x]);
			rgb[3*((y+y0)*size +x0 +x)]   = tmp; //rnd(shift+im[y*w+x]); //im[y*w+x] ? 255 : 0; //
			rgb[3*((y+y0)*size +x0 +x)+1] = tmp; //rnd(shift+im[y*w+x]); //im[y*w+x] ? 255 : 0; //
			rgb[3*((y+y0)*size +x0 +x)+2] = tmp; //rnd(shift+im[y*w+x]); //im[y*w+x] ? 255 : 0; //
		}
	}
}

static inline void drawrect8_rgb(uint8 *rgb, uint8 *r, uint8 *g, uint8 *b, uint32 x0, uint32 y0, uint32 w, uint32 h, uint32 size, uint32 shift)
{
	uint32 x, y, tmp;
	for(y=0; y < h; y++ ){
		for(x=0; x < w; x++){
			//tmp = rnd(im[y*w+x] < 0 ? -im[y*w+x]<<1 : im[y*w+x]<<1);
			//tmp = lb1(shift + pic[y*w+x]);
			//if(tmp > 255) printf("tmp = %d pic = %d\n", tmp, pic[y*w+x]);
			rgb[3*((y+y0)*size +x0 +x)]   = lb1(shift + r[y*w+x]);; //rnd(shift+im[y*w+x]); //im[y*w+x] ? 255 : 0; //
			rgb[3*((y+y0)*size +x0 +x)+1] = lb1(shift + g[y*w+x]);; //rnd(shift+im[y*w+x]); //im[y*w+x] ? 255 : 0; //
			rgb[3*((y+y0)*size +x0 +x)+2] = lb1(shift + b[y*w+x]);; //rnd(shift+im[y*w+x]); //im[y*w+x] ? 255 : 0; //
		}
	}
}

static inline void drawrect_rgb(uint8 *rgb, uint8 *im, uint32 w0, uint32 h0, uint32 w, uint32 h, uint32 w1)
{
	uint32 x, y, tmp;
	for(y=0; y < h; y++ ){
		for(x=0; x < w; x++){
			rgb[3*((y+h0)*w1+w0+x)  ] = im[3*(y*w+x)  ];
			rgb[3*((y+h0)*w1+w0+x)+1] = im[3*(y*w+x)+1];
			rgb[3*((y+h0)*w1+w0+x)+2] = im[3*(y*w+x)+2];
		}
	}
}

uint8* utils_dwt_image_draw(Image *img, uint8 *rgb, uint32 steps)
{
	uint32 i, j, x, y, w = img->w;

	if(steps != 0){
		for(j=0; j < steps; j++){
			printf("img->l[j].s[0].w = %d img->l[j].s[0].h = %d\n", img->l[j].s[0].w, img->l[j].s[0].h);
			drawrect(rgb, img->l[j].s[1].pic, img->l[j].s[0].w,	0,                img->l[j].s[1].w, img->l[j].s[1].h, w, 128);
			drawrect(rgb, img->l[j].s[2].pic, 0,                img->l[j].s[0].h, img->l[j].s[2].w, img->l[j].s[2].h, w, 128);
			drawrect(rgb, img->l[j].s[3].pic, img->l[j].s[0].w, img->l[j].s[0].h, img->l[j].s[3].w, img->l[j].s[3].h, w, 128);
		}
		drawrect(rgb, img->l[steps-1].s[0].pic, 0, 0, img->l[steps-1].s[0].w, img->l[steps-1].s[0].h, w, 128);
	}
	return rgb;
}

uint8* utils_resize_draw(Pic8u *p, uint8 *rgb, uint32 steps, uint32 w)
{
	uint32 i, j, x, y;

	if(steps != 0){
		x = 0; y = 0;
		for(j=0; j < steps ; j++){
			drawrect8(rgb, p[j].pic, x, y, p[j].w, p[j].h, w, 0);
			x += p[j].w; y += p[j].h;
		}
	}
	return rgb;
}

uint8* utils_resize_draw_rgb(Pic8u *r, Pic8u *g, Pic8u *b, uint8 *rgb, uint32 steps, uint32 w)
{
	uint32 i, j, x, y;

	if(steps != 0){
		x = 0; y = 0;
		for(j=0; j < steps ; j++){
			drawrect8_rgb(rgb, r[j].pic, g[j].pic, b[j].pic, x, y, r[j].w, r[j].h, w, 0);
			x += r[j].w; y += r[j].h;
		}
	}
	return rgb;
}

uint8* utils_contour(Pic8u *p, uint8 *rgb, uint32 n)
{
	uint32 i, j, x, y, w;
	uint32 yw, yx, yx3, yy, xy, s = 1<<n;

	w = p[n].w*s;

	for(y=0; y < p[n].h; y++){
		yw = y*w*s;
		for(x=0; x < p[n].w; x++){
			for(i=0; i < s; i++){
				yy = yw + i*w;
				for(j=0; j < s; j++){
					yx = yy + x*s + j;
					yx3 = yx*3;
					xy = y*p[n].w + x;
					rgb[yx3  ] = p[n].pic[xy];
					rgb[yx3+1] = p[n].pic[xy];
					rgb[yx3+2] = p[n].pic[xy];
				}
			}
		}
	}
	return rgb;
}

uint8* utils_contour32(Pic32u *p, uint8 *rgb, uint32 n)
{
	uint32 i, j, x, y, w;
	uint32 yw, yx, yx3, yy, xy, s = 1<<n;

	w = p[n].w*s;

	for(y=0; y < p[n].h; y++){
		yw = y*w*s;
		for(x=0; x < p[n].w; x++){
			for(i=0; i < s; i++){
				yy = yw + i*w;
				for(j=0; j < s; j++){
					yx = yy + x*s + j;
					yx3 = yx*3;
					xy = y*p[n].w + x;
					rgb[yx3  ] = p[n].pic[xy]&0xFF;
					rgb[yx3+1] = p[n].pic[xy]&0xFF;
					rgb[yx3+2] = p[n].pic[xy]&0xFF;
				}
			}
		}
	}
	return rgb;
}

uint8* utils_contour_rgb(Pic8u *r, Pic8u *g, Pic8u *b, uint8 *rgb, uint32 n)
{
	uint32 i, j, x, y, w;
	uint32 yw, yx, yx3, yy, xy, s = 1<<n;

	w = r[n].w*s;

	for(y=0; y < r[n].h; y++){
		yw = y*w*s;
		for(x=0; x < r[n].w; x++){
			for(i=0; i < s; i++){
				yy = yw + i*w;
				for(j=0; j < s; j++){
					yx = yy + x*s + j;
					yx3 = yx*3;
					xy = y*r[n].w + x;
					rgb[yx3  ] = r[n].pic[xy];
					rgb[yx3+1] = g[n].pic[xy];
					rgb[yx3+2] = b[n].pic[xy];
				}
			}
		}
	}
	return rgb;
}

uint8* utils_dwt_bayer_draw(GOP *g, uint32 fn, WaletConfig *wc, uint8 *rgb, uint8 steps)
{
	uint32 i, j, x, y, w;
	Frame *f = &g->frames[fn];
	w = f->img[0].w + f->img[1].w;
	if(steps == wc->steps && wc->steps){
		for(j=0; j < steps; j++){
			for(i=0; i < 4; i++){
				if		(i == 0){ x = 0; 			y = 0;}
				else if	(i == 1){ x = f->img[0].w; 	y = 0;}
				else if (i == 2){ x = 0; 			y = f->img[0].h;}
				else			{ x = f->img[0].w;	y = f->img[0].h;}
				if(j == steps-1){
					drawrect(rgb, f->img[i].l[j].s[0].pic, x, y, f->img[i].l[j].s[0].w, f->img[i].l[j].s[0].h, w, 128);
					//if(i) 	drawrect(rgb, f->img[i].l[j].s[0].pic, x, y, f->img[i].l[j].s[0].w, f->img[i].l[j].s[0].h, w, 128);
					//else 	drawrect(rgb, f->img[i].l[j].s[0].pic, x, y, f->img[i].l[j].s[0].w, f->img[i].l[j].s[0].h, w, 0);
				}
				drawrect(rgb, f->img[i].l[j].s[1].pic, x + f->img[i].l[j].s[0].w,	y,                     		f->img[i].l[j].s[1].w, f->img[i].l[j].s[1].h, w, 128);
				drawrect(rgb, f->img[i].l[j].s[2].pic, x,                     		y + f->img[i].l[j].s[0].h, 	f->img[i].l[j].s[2].w, f->img[i].l[j].s[2].h, w, 128);
				drawrect(rgb, f->img[i].l[j].s[3].pic, x + f->img[i].l[j].s[0].w, 	y + f->img[i].l[j].s[0].h, 	f->img[i].l[j].s[3].w, f->img[i].l[j].s[3].h, w, 128);
			}
		}
	} else {
		if(wc->steps == 0){
			drawrect(rgb, f->img[0].p, 0,         		0,         		f->img[0].w, f->img[0].h, w, 128);
			drawrect(rgb, f->img[1].p, f->img[0].w, 	0,         		f->img[1].w, f->img[1].h, w, 128);
			drawrect(rgb, f->img[2].p, 0,         		f->img[0].h, 	f->img[2].w, f->img[2].h, w, 128);
			drawrect(rgb, f->img[3].p, f->img[0].w, 	f->img[0].h, 	f->img[3].w, f->img[3].h, w, 128);
		} else {
			j = steps;
			for(i=0; i < 4; i++){
				if(steps == 0){
					if		(i == 0){ x = 0; 			y = 0;}
					else if	(i == 1){ x = f->img[0].w; 	y = 0;}
					else if (i == 2){ x = 0; 			y = f->img[0].h;}
					else			{ x = f->img[0].w; 	y = f->img[0].h;}
				} else {
					if		(i == 0){ x = 0; 						y = 0;}
					else if	(i == 1){ x = f->img[i].l[j-1].s[0].w; 	y = 0;}
					else if (i == 2){ x = 0; 						y = f->img[i].l[j-1].s[0].h;}
					else			{ x = f->img[i].l[j-1].s[0].w;	y = f->img[i].l[j-1].s[0].h;}
				}
				drawrect(rgb, f->img[i].l[j].s[0].pic, x, y, f->img[i].l[j].s[0].w, f->img[i].l[j].s[0].h, w, 128);
				//if(i)	drawrect(rgb, f->img[i].l[j].s[0].pic, x, y, f->img[i].l[j].s[0].w, f->img[i].l[j].s[0].h, w, 128);
				//else	drawrect(rgb, f->img[i].l[j].s[0].pic, x, y, f->img[i].l[j].s[0].w, f->img[i].l[j].s[0].h, w, 0);
				drawrect(rgb, f->img[i].l[j].s[0].pic, x,                     		y,                     		f->img[i].l[j].s[0].w, f->img[i].l[j].s[0].h, w, 128);
				drawrect(rgb, f->img[i].l[j].s[1].pic, x + f->img[i].l[j].s[0].w, 	y,                 			f->img[i].l[j].s[1].w, f->img[i].l[j].s[1].h, w, 128);
				drawrect(rgb, f->img[i].l[j].s[2].pic, x,                     		y + f->img[i].l[j].s[0].h, 	f->img[i].l[j].s[2].w, f->img[i].l[j].s[2].h, w, 128);
				drawrect(rgb, f->img[i].l[j].s[3].pic, x + f->img[i].l[j].s[0].w, 	y + f->img[i].l[j].s[0].h, 	f->img[i].l[j].s[3].w, f->img[i].l[j].s[3].h, w, 128);
			}

		}
	}

	return rgb;
}

void shift_b_to_w(uint8 *in, int8 *out, int shift, uint32 size)
{
	uint32 i;
	for(i=0; i < size; i++) out[i] = in[i] + shift;
}

void shift_w_to_b(int8 *in, uint8 *out, int shift, uint32 size)
{
	uint32 i;
	int tmp;
	for(i=0; i < size; i++) {
		tmp = in[i] + shift;
		out[i] = in[i] + shift;
	}
}

uint8* utils_shift(int16 *img, uint8 *rgb, uint32 w, uint32 h, int sh)
{
	int i, sz = h*w;
	for(i=0; i < sz; i++) { rgb[i] = img[i] + sh; rgb[i] = rgb[i] < 2 ? 2 : rgb[i]; }
	return rgb;
}

int16* utils_shift16(int16 *img, int16 *rgb, uint32 w, uint32 h, int sh)
{
	int i, sz = h*w;
	for(i=0; i < sz; i++) rgb[i] = img[i] + sh;
	return rgb;
}

uint8* utils_grey_draw(int16 *img, uint8 *rgb, uint32 w, uint32 h, uint32 bpp)
{
    int i, j, dim = h*w*3, sh = 1<<bpp-1, sh1 = bpp - 8 - 2;
    for(i = 0,  j= 0; j < dim; j+=3, i++){
        rgb[j]     = (img[i] + sh)>>sh1;
        rgb[j + 1] = (img[i] + sh)>>sh1;
        rgb[j + 2] = (img[i] + sh)>>sh1;
    }
    return rgb;
}

uint8* utils_grey_draw_rgb(int16 *R, int16 *G, int16 *B, uint8 *rgb, uint32 w, uint32 h, uint32 sh)
{
    int i, j, dim = h*w*3;
    for(i = 0,  j= 0; j < dim; j+=3, i++){
        rgb[j]     = R[i] + sh;
        rgb[j + 1] = G[i] + sh;
        rgb[j + 2] = B[i] + sh;
    }
    return rgb;
}

uint8* utils_grey_draw8(uint8 *img, uint8 *rgb, uint32 w, uint32 h, uint32 sh)
{
	int i, j, dim = h*w*3;
	for(i = 0,  j= 0; j < dim; j+=3, i++){
		rgb[j]     = img[i] + sh;
		rgb[j + 1] = img[i] + sh;
		rgb[j + 2] = img[i] + sh;
	}
	return rgb;
}

void utils_turn_on_180(int16 *img, uint16 *buff, uint32 w, uint32 h)
{
	uint32 x, y, yw, yw1, h2 = h>>1;

	for(y=0; y < h2; y++){
		yw = y*w;
		for(x=0; x < w; x++){
			buff[x] = img[yw + x];
		}
		yw1 = (h-y)*w-1;
		for(x=0; x < w; x++){
			img[yw + x] = img[yw1 - x];
		}
		for(x=0; x < w; x++){
			img[yw1 - x] = buff[x];
		}
	}
	if(h2%2){
		yw = h2*w;
		for(x=0; x < w; x++){
			buff[x] = img[yw + x];
		}
		yw = (h2+1)*w-1;
		for(x=0; x < w; x++){
			img[yw - x] = buff[x];
		}
	}
}


//#define lb(x) (x&0xFF)

uint8* utils_bayer_draw(int16 *img, uint8 *rgb, uint32 w, uint32 h, BayerGrid bay){
/// \fn void bayer_to_rgb(uint8 *rgb)
///	\brief DWT picture transform.
///	\param	rgb 	The pointer to rgb array.

/*
   All RGB cameras use one of these Bayer grids:

	BGGR  0         GRBG 1          GBRG  2         RGGB 3
	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5
	0 B G B G B G	0 G R G R G R	0 G B G B G B	0 R G R G R G
	1 G R G R G R	1 B G B G B G	1 R G R G R G	1 G B G B G B
	2 B G B G B G	2 G R G R G R	2 G B G B G B	2 R G R G R G
	3 G R G R G R	3 B G B G B G	3 R G R G R G	3 G B G B G B
 */
	uint32 x, y, wy, xwy, xwy3, y2, x2, a, b, h1 = h, w1 = w, yw, yw1;

	switch(bay){
		case(BGGR):{ a = 1; b = 1; break;}
		case(GRBG):{ a = 0; b = 1; break;}
		case(GBRG):{ a = 1; b = 0; break;}
		case(RGGB):{ a = 0; b = 0; break;}
	}

	for(y=0, yw=0, yw1=0 ; y < h1; y++, yw+=w, yw1+=w1){
		for(x=0; x < w1; x++){
			y2 = oe(a,y);
			x2 = oe(b,x);
			xwy = x + yw;
			wy 	= x + yw1;
			xwy3 = wy + wy + wy;
			rgb[xwy3    ] = y2 ? (x2 ?  lb(img[xwy    ]) : lb(img[xwy+1])) : (x2 ? lb(img[xwy+w]) : lb(img[xwy+w+1]));
			rgb[xwy3 + 1] = y2 ? (x2 ? (lb(img[xwy+w  ]) + lb(img[xwy+1]))>>1 :   (lb(img[xwy  ]) + lb(img[xwy+w+1]))>>1) :
					(x2 ? (lb(img[xwy+w+1]) + lb(img[xwy  ]))>>1 :   (lb(img[xwy+1]) + lb(img[xwy+w  ]))>>1);
			rgb[xwy3 + 2] = y2 ? (x2 ?  lb(img[xwy+w+1]) : lb(img[xwy+w])) : (x2 ? lb(img[xwy+1]) : lb(img[xwy    ]));
		}
	}
	return rgb;
}

void utils_bayer_to_RGB_fast(int16 *img, uint8 *r, uint8 *g, uint8 *b, uint32 w, uint32 h, BayerGrid bay, uint32 sh){
/*
   All RGB cameras use one of these Bayer grids:

	BGGR  0         GRBG 1          GBRG  2         RGGB 3
	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5
	0 B G B G B G	0 G R G R G R	0 G B G B G B	0 R G R G R G
	1 G R G R G R	1 B G B G B G	1 R G R G R G	1 G B G B G B
	2 B G B G B G	2 G R G R G R	2 G B G B G B	2 R G R G R G
	3 G R G R G R	3 B G B G B G	3 R G R G R G	3 G B G B G B
 */
	int x, y, x1, y1, wy, wy1, yx, yx1, w1 = w>>1, h1 = h>>1;

	switch(bay){
		case(BGGR):{
			break;
		}
		case(GRBG):{
			break;
		}
		case(GBRG):{
			break;
		}
		case(RGGB):{
			for(y=0; y < h1; y++){
				wy = y*w1;
				wy1 = (y<<1)*w;
				for(x=0; x < w1; x++){
					yx 	= x + wy;
					yx1 = (x<<1) + wy1;
					r[yx] = img[yx1] + sh;
					g[yx] = ((img[yx1 + 1] + img[yx1 + w])>>1) + sh;
					b[yx] = img[yx1 + w + 1] + sh;
				}
			}
			break;
		}
	}
}

void utils_bayer_to_RGB_fast_(int16 *img, uint8 *r, uint8 *g, uint8 *b, uint32 w, uint32 h, BayerGrid bay, uint32 sh){
/*
   All RGB cameras use one of these Bayer grids:

	BGGR  0         GRBG 1          GBRG  2         RGGB 3
	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5
	0 B G B G B G	0 G R G R G R	0 G B G B G B	0 R G R G R G
	1 G R G R G R	1 B G B G B G	1 R G R G R G	1 G B G B G B
	2 B G B G B G	2 G R G R G R	2 G B G B G B	2 R G R G R G
	3 G R G R G R	3 B G B G B G	3 R G R G R G	3 G B G B G B
 */
	int x, y, x1, y1, wy, wy1, yx, yx1, w1 = w>>1, h1 = h>>1;

	switch(bay){
		case(BGGR):{
			break;
		}
		case(GRBG):{
			break;
		}
		case(GBRG):{
			break;
		}
		case(RGGB):{
			for(y=0; y < h1; y++){
				wy =(y+1)*(w1+2);
				wy1 = (y<<1)*w;
				for(x=0; x < w1; x++){
					yx 	= wy + x + 1;
					yx1 = (x<<1) + wy1;
					r[yx] = img[yx1] + sh;
					g[yx] = ((img[yx1 + 1] + img[yx1 + w])>>1) + sh;
					b[yx] = img[yx1 + w + 1] + sh;
				}
			}
			break;
		}
	}
}

uint8* utils_bayer_to_Y_fast_(int16 *img, uint8 *Y, uint32 w, uint32 h, uint32 sh)
/*
   All RGB cameras use one of these Bayer grids:

	BGGR  0         GRBG 1          GBRG  2         RGGB 3
	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5
	0 B G B G B G	0 G R G R G R	0 G B G B G B	0 R G R G R G
	1 G R G R G R	1 B G B G B G	1 R G R G R G	1 G B G B G B
	2 B G B G B G	2 G R G R G R	2 G B G B G B	2 R G R G R G
	3 G R G R G R	3 B G B G B G	3 R G R G R G	3 G B G B G B
 */
{
	int x, y, x1, y1, wy, wy1, yx, yx1, w1 = w>>1, h1 = h>>1;

	for(y=0; y < h1; y++){
		wy =(y+1)*(w1+2);
		wy1 = (y<<1)*w;
		for(x=0; x < w1; x++){
			yx 	= wy + x + 1;
			yx1 = (x<<1) + wy1;
			Y[yx] = ((img[yx1] + img[yx1 + 1] + img[yx1 + w] + img[yx1 + w + 1])>>2) + sh;
		}
	}
	return Y;
}


uint8* utils_bayer_to_Y_fast(int16 *img, uint8 *Y, uint32 w, uint32 h, uint32 sh){
/*
   All RGB cameras use one of these Bayer grids:

	BGGR  0         GRBG 1          GBRG  2         RGGB 3
	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5
	0 B G B G B G	0 G R G R G R	0 G B G B G B	0 R G R G R G
	1 G R G R G R	1 B G B G B G	1 R G R G R G	1 G B G B G B
	2 B G B G B G	2 G R G R G R	2 G B G B G B	2 R G R G R G
	3 G R G R G R	3 B G B G B G	3 R G R G R G	3 G B G B G B
 */
	int x, y, x1, y1, wy, wy1, yx, yx1, w1 = w>>1, h1 = h>>1;

	for(y=0; y < h1; y++){
		wy = y*w1;
		wy1 = (y<<1)*w;
		for(x=0; x < w1; x++){
			yx 	= x + wy;
			yx1 = (x<<1) + wy1;
			Y[yx] = ((img[yx1] + img[yx1 + 1] + img[yx1 + w] + img[yx1 + w + 1])>>2) + sh;
		}
	}
	return Y;
}

/**	\brief Bilinear algorithm for bayer to RGB interpolation use 3 rows buffer.
    \param img	 	The input Bayer image.
 	\param rgb		The output RGB image.
 	\param buff		The temporary 3 rows buffer
	\param w		The image width.
	\param h		The image height.
	\param bay		The Bayer grids pattern.
	\param shift	The image shift for display.
	\retval			Output RGB image..
*/
uint8* utils_bayer_to_RGB24(int16 *img, uint8 *rgb, int16 *buff, uint32 w, uint32 h, BayerGrid bay, uint32 bpp){
/*
   All RGB cameras use one of these Bayer grids:

    BGGR  0         GRBG 1          GBRG  2         RGGB 3
      0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5
    0 B G B G B G	0 G R G R G R	0 G B G B G B	0 R G R G R G
    1 G R G R G R	1 B G B G B G	1 R G R G R G	1 G B G B G B
    2 B G B G B G	2 G R G R G R	2 G B G B G B	2 R G R G R G
    3 G R G R G R	3 B G B G B G	3 R G R G R G	3 G B G B G B
 */
    int x, x1, x2, xs, ys, y = 0, wy, xwy3, w2 = w<<1, yw = 0, h1, w1, h2, shift = 1<<(bpp-1), sh = bpp - 8;
    int16 *l0, *l1, *l2, *tm;
    l0 = buff; l1 = &buff[w+2]; l2 = &buff[(w+2)<<1];
    shift = 0;
    //printf("bpp = %d shift = %d\n", bpp, shift);

    switch(bay){
        case(BGGR):{ xs = 1; ys = 1; w1 = w+1; h1 = h+1; break;}
        case(GRBG):{ xs = 1; ys = 0; w1 = w+1; h1 = h; break;}
        case(GBRG):{ xs = 0; ys = 1; w1 = w; h1 = h+1; break;}
        case(RGGB):{ xs = 0; ys = 0; w1 = w; h1 = h;   break;}
    }
    h2 = h1-1;
    //Create 3 rows buffer for transform
    l0[0] = img[w+1]; for(x=0; x < w; x++) l0[x+1] = img[w+x];  l0[w+1] = l0[w-1];
    l1[0] = img[1];   for(x=0; x < w; x++) l1[x+1] = img[x];    l1[w+1] = l1[w-1];

    for(y=ys, yw=0; y < h1; y++, yw+=w){
        wy = (y == h2) ? yw - w : yw + w;
        l2[0] = img[wy+1]; for(x=0; x < w; x++) l2[x+1] = img[wy + x];  l2[w+1] = l2[w-1];

        for(x=xs, x1=0; x < w1; x++, x1++){
            wy 	= x1 + yw;
            x2 = x1 + 1;
            xwy3 = wy*3;

            if(!(y&1) && !(x&1)){
                rgb[xwy3] 	= 	lb1((l1[x2] + shift)>>sh);
                rgb[xwy3+1] = 	lb1((((l0[x2] + l2[x2] + l1[x2-1] + l1[x2+1])>>2) + shift)>>sh);
                rgb[xwy3+2] = 	lb1((((l0[x2+1] + l2[x2-1] + l0[x2-1] + l2[x2+1])>>2) + shift)>>sh);
            }else if (!(y&1) && (x&1)){
                rgb[xwy3] = 	lb1((((l1[x2-1] + l1[x2+1])>>1) + shift)>>sh);
                rgb[xwy3+1] = 	lb1((l1[x2] + shift)>>sh);
                rgb[xwy3+2] =	lb1((((l0[x2] + l2[x2])>>1) + shift)>>sh);
            }else if ((y&1) && !(x&1)){
                rgb[xwy3] = 	lb1((((l0[x2] + l2[x2])>>1) + shift)>>sh);
                rgb[xwy3+1] = 	lb1((l1[x2] + shift)>>sh);
                rgb[xwy3+2] =	lb1((((l1[x2-1] + l1[x2+1])>>1) + shift)>>sh);
            }else {
                rgb[xwy3] = 	lb1((((l0[x2+1] + l2[x2-1] + l0[x2-1] + l2[x2+1])>>2) + shift)>>sh);
                rgb[xwy3+1] = 	lb1((((l0[x2] + l2[x2] + l1[x2-1] + l1[x2+1])>>2) + shift)>>sh);
                rgb[xwy3+2] = 	lb1((l1[x2] + shift)>>sh);
            }
        }
        tm = l0; l0 = l1; l1 = l2; l2 = tm;
    }
    return rgb;
}

void utils_bayer_to_RGB24_white_balance(int16 *img, int16 *img1, uint32 w, uint32 h, BayerGrid bay, uint32 bpp)
{
/*
   All RGB cameras use one of these Bayer grids:

    BGGR  0         GRBG 1          GBRG  2         RGGB 3
      0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5
    0 B G B G B G	0 G R G R G R	0 G B G B G B	0 R G R G R G
    1 G R G R G R	1 B G B G B G	1 R G R G R G	1 G B G B G B
    2 B G B G B G	2 G R G R G R	2 G B G B G B	2 R G R G R G
    3 G R G R G R	3 B G B G B G	3 R G R G R G	3 G B G B G B
 */
    int x, x1, x2, xs, ys, y = 0, yx, yw = 0, h1, w1, h2, shift = 1<<(bpp-1), sh = bpp - 8;
    int16 *l0, *l1, *l2, *tm;
    uint32 sumr = 0, sumb = 0, sumr1 = 0, sumb1 = 0, cr = 0, cb = 0;
    double r = 0., b = 0., sr = 1, sb = 1;

    //GBRG
    //printf("bpp = %d shift = %d\n", bpp, shift);

    switch(bay){
        case(BGGR):{ xs = 1; ys = 1; w1 = w+1; h1 = h+1; break;}
        case(GRBG):{ xs = 1; ys = 0; w1 = w+1; h1 = h  ; break;}
        case(GBRG):{ xs = 0; ys = 1; w1 = w  ; h1 = h+1; break;}
        case(RGGB):{ xs = 0; ys = 0; w1 = w  ; h1 = h  ; break;}
    }

    for(y=0; y < h; y+=2){
        yw = y*w;
        for(x=0; x < w; x+=2){
            yx = yw + x;
            //img1[yx+w] = (int16)((double)(img[yx+w]+shift)*r) - shift;
            //img1[yx+1] = (int16)((double)(img[yx+1]+shift)*b) - shift;
            img1[yx+w] = img[yx+w];
            img1[yx+1] = img[yx+1];

            //img1[yx]   = img[yx];
            //img1[yx+w+1]   = img[yx+w+1];
            sumr1 += abs(((img[yx] + img[yx+w+1])>>1) - img1[yx+w]);
            sumb1 += abs(((img[yx] + img[yx+w+1])>>1) - img1[yx+1]);
        }
    }

    while(cr < 2 || cb < 2) {
        r = r + sr; b = b + sb;
        sumr = 0; sumb = 0;
        for(y=0; y < h; y+=2){
            yw = y*w;
            for(x=0; x < w; x+=2){
                yx = yw + x;
                //img1[yx+w] = (int16)((double)(img[yx+w]+shift)*r) - shift;
                //img1[yx+1] = (int16)((double)(img[yx+1]+shift)*b) - shift;
                img1[yx+w] = img[yx+w]+r;
                img1[yx+1] = img[yx+1]+b;
                sumr += abs(((img[yx] + img[yx+w+1])>>1) - img1[yx+w]);
                sumb += abs(((img[yx] + img[yx+w+1])>>1) - img1[yx+1]);

                //img1[yx+w] = img[yx+w];
                //img1[yx+1] = img[yx+1];
                img1[yx]   = img[yx];
                img1[yx+w+1]  = img[yx+w+1];
            }
        }
        if(sumr > sumr1) { sr = -sr; cr++; }
        if(sumb > sumb1) { sb = -sb; cb++; }
        sumr1 = sumr; sumb1 = sumb;
        printf("sumr = %d sumb = %d r = %f b = %f cr = %d cb = %d\n", sumr, sumb, r, b, cr, cb);
    }
}

void utils_bayer_local_hdr(int16 *img, int16 *img1, uint32 w, uint32 h, BayerGrid bay, uint32 bpp, int low, int top)
{
/*
   All RGB cameras use one of these Bayer grids:

    BGGR  0         GRBG 1          GBRG  2         RGGB 3
      0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5
    0 B G B G B G	0 G R G R G R	0 G B G B G B	0 R G R G R G
    1 G R G R G R	1 B G B G B G	1 R G R G R G	1 G B G B G B
    2 B G B G B G	2 G R G R G R	2 G B G B G B	2 R G R G R G
    3 G R G R G R	3 B G B G B G	3 R G R G R G	3 G B G B G B
 */
    int i, x, x1, xs, ys, y, y1, yx, yx1, yw, yw1, h1, w1, h2, shift = 1<<(bpp-1), sh = bpp - 8, size = w*h;
    int max, min, ll, df, st, diff, diffn, sp = 4, Y[4], Y1, dfn;
    int maxg, ming, maxr, minr, maxb, minb, maxy, miny, avr, avrn, st1, th;
    int b, d, tmp, in;
    double a, c;

    //GBRG
    //printf("bpp = %d shift = %d\n", bpp, shift);

    switch(bay){
        case(BGGR):{ xs = 1; ys = 1; w1 = w+1; h1 = h+1; break;}
        case(GRBG):{ xs = 1; ys = 0; w1 = w+1; h1 = h  ; break;}
        case(GBRG):{ xs = 0; ys = 1; w1 = w  ; h1 = h+1; break;}
        case(RGGB):{ xs = 0; ys = 0; w1 = w  ; h1 = h  ; break;}
    }
    //for(i=0; i < size; i++){
    //    img1[yx1] = 0;
    //}
    b = (1<<30)/(top - low);
    printf("b = %d top = %d low = %d\n", b, top, low);

    for(y=0; y < h; y+=2){
        yw = y*w;
        for(x=0; x < w; x+=2){
            yx = yw + x;
            max  = 0; min  = 4095;
            maxg = 0; ming = 4095;
            maxr = 0; minr = 4095;
            maxb = 0; minb = 4095;
            maxy = 0; miny = 4095;
            avr = 0;
            i=0;
            for(y1=0; y1 < sp; y1+=2){
                yw1 = yx + y1*w;
                for(x1=0; x1 < sp; x1+=2){
                    yx1 = yw1 + x1;

                    if(img[yx1]+shift > maxg) maxg = img[yx1]+shift;
                    if(img[yx1]+shift < ming) ming = img[yx1]+shift;
                    if(img[yx1+w+1]+shift > maxg) maxg = img[yx1+w+1]+shift;
                    if(img[yx1+w+1]+shift < ming) ming = img[yx1+w+1]+shift;

                    if(img[yx1+w]+shift > maxr) maxr = img[yx1+w]+shift;
                    if(img[yx1+w]+shift < minr) minr = img[yx1+w]+shift;

                    if(img[yx1+1]+shift > maxb) maxb = img[yx1+1]+shift;
                    if(img[yx1+1]+shift < minb) minb = img[yx1+1]+shift;

                    if((img[yx1]+img[yx1+1]+img[yx1+w]+img[yx1+w+1] + (shift<<2))>>2 > maxy)
                        maxy = (img[yx1]+img[yx1+1]+img[yx1+w]+img[yx1+w+1] + (shift<<2))>>2;
                    if((img[yx1]+img[yx1+1]+img[yx1+w]+img[yx1+w+1] + (shift<<2))>>2 < miny)
                        miny = (img[yx1]+img[yx1+1]+img[yx1+w]+img[yx1+w+1] + (shift<<2))>>2;

                    min = (minr > minb) ? (minb > ming ? ming : minb) : (minr > ming ? ming : minr);
                    max = (maxr > maxb) ? (maxr > maxg ? maxr : maxg) : (maxb > maxg ? maxb : maxg);

                    avr += abs(img[yx1]-img[yx1+1]-img[yx1+w]+img[yx1+w+1]);


                    Y[i] = ((img[yx1]+img[yx1+1]+img[yx1+w]+img[yx1+w+1])>>2) + shift;
                    if(Y[i] > maxy) maxy = Y[i];
                    if(Y[i] < miny) miny = Y[i];
                    i++;
                }
            }
            //printf("Ymai=%4d Yma=%4d Yd=%4d  Rmi=%4d Rma=%4d Rd=%4d  Gmi=%4d Gma=%4d Gd=%4d  Bmi=%4d Bma=%4d Bd=%4d\n",
            //       min, max, max-min, minr, maxr, maxr-minr, ming, maxg, maxg-ming, minb, maxb, maxb-minb);
            /*
            avr = avr>>4;

            diff = max - min;

            //New
            b = (1<<31)/(top - low);
            c = 256./(double)(top - low);
            df = ((max+min)>>1) - low;
            dfn = c*df;

            if(diff > 128) {
                a = 128./(double)diff;
                diff = 128;
            } else {
                a = 1.;
            }
            //df = (max+min)>>1;

            st = dfn - (diff>>1);
            if(dfn < (diff>>1)) st = 0;
            if(dfn + (diff>>1) > 255)  st = 255 - diff;
            printf("low = %d top = %d min = %d max = %d df = %d dfn = %d avr = %d avrn = %d diff = %d diffn = %d\n",
                   low, top, min, max, df, dfn, avr, (int)(c*avr), diff, (int)(diff*c));
            */
            //Old

            max = max > top ? top : max;
            min = min < low ? low : min;

            diff = max - min;
            dfn = (min + max)>>1;
            //diff = maxy - miny;
            th = 100;
            a = 1.;
            c = 256./(top - low);
            //b = (1<<31)/(top - low);
            //minn = min*c;
            //maxn = max*c;


            //if(min*c)
            if(diff > th){
                a = (double)th/(double)diff;
                diff = th;
            }

            ll = top - low; df = 256 - diff;
            //a = 1.;
            if(!df) st = 0;
            else {   st = (min-low)*df/(top - low);}

            //printf("low = %d top = %d min = %d max = %d diff = %d df = %d st = %d min = %d max = %d a = %f\n",
            //       low, top, min, max, diff, df, st, st + (int)(a*(min - min)), st + (int)(a*(max - min)), a);

            /*
            avr = avr>>4;
            diffn = (max - min)*b>>22;
            avrn = avr*b>>22;
            st1 = (((min+max)>>1) - low);
            st = st1*b>>22;

            if(!avrn) d = b<<2;
            else d = b;
            */
            /*
            if(diff > 256){
                st = 0; a = 256./(double)diff;
            } else {
                ll = min - low + top - max; df = 256 - diff;
                //printf("min = %d max = %d diff = %d ll = %d df = %d\n", min, max, diff, ll, df);
                a = 1.;
                if(!df) st = 0;
                else { st = ll/df; st = min/st; }
            }
            */
            //printf("low = %d top = %d min = %d max = %d avr = %d avrn = %d diff = %d diffn = %d st = %d st1 = %d\n",
            //       low, top, min, max, avr, avrn, diff, diffn, st, st1);

            //st = 0;
            for(y1=0; y1 < sp; y1+=1){
                yw1 = yx + y1*w;
                for(x1=0; x1 < sp; x1+=1){
                    yx1 = yw1 + x1;
                    tmp = st + a*(img[yx1]+shift - min);

                    img1[yx1] += tmp < 0 ? 0 : (tmp > 255 ? 255 : tmp);
                }
            }
        }
    }

    for(i=0; i < size; i++){
        //img1[i] = (img1[i]) - 128;
        img1[i] = (img1[i]>>2) - 128;
    }
}

void utils_bayer_local_hdr1(int16 *img, int16 *img1, uint32 w, uint32 h, BayerGrid bay, uint32 bpp)
{
/*
   All RGB cameras use one of these Bayer grids:

    BGGR  0         GRBG 1          GBRG  2         RGGB 3
      0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5
    0 B G B G B G	0 G R G R G R	0 G B G B G B	0 R G R G R G
    1 G R G R G R	1 B G B G B G	1 R G R G R G	1 G B G B G B
    2 B G B G B G	2 G R G R G R	2 G B G B G B	2 R G R G R G
    3 G R G R G R	3 B G B G B G	3 R G R G R G	3 G B G B G B
 */
    int i, x, x1, xs, ys, y, y1, yx, yx1, yw, yw1, h1, w1, h2, sh = bpp - 8, size = w*h;//shift = 1<<(bpp-1),
    int max, min, ll, df, st, diff, sp = 4, Y, Y1, dfn;
    int maxg, ming, maxr, minr, maxb, minb, maxy, miny;
    double aw, ah, c;
    int ws = 80, hs = 60, sz = ws*hs, hz = 1<<bpp, ws2 = ws>>1, hs2 = hs>>1;
    uint32 hist[4096], look[4096],  sum, b;
    double wt[ws*hs];

    ah = 0.5/(double)(hs2-1);
    aw = 0.5/(double)(ws2-1);

    //Make weight matrix
    for(y=0; y < hs2; y++){
        yw = y*ws;
        for(x=0; x < ws2; x++){
            yx = yw +x;
            wt[yx] = x*aw + y*ah;
        }
        for(x=ws2; x < ws; x++){
            yx = yw +x;
            wt[yx] = (ws-1-x)*aw + y*ah;
        }
    }
    for(y=hs2; y < hs; y++){
        yw = y*ws;
        for(x=0; x < ws2; x++){
            yx = yw +x;
            wt[yx] = x*aw + (hs-1-y)*ah;
        }
        for(x=ws2; x < ws; x++){
            yx = yw +x;
            wt[yx] = (ws-1-x)*aw + (hs-1-y)*ah;
        }
    }
    /*
    for(y=0; y < hs; y++){
        yw = y*ws;
        for(x=0; x < ws; x++){
            yx = yw +x;
            printf("%f ", wt[yx]);
        }
        printf("\n\n");
    }*/


    //GBRG
    //printf("bpp = %d shift = %d\n", bpp, shift);

    //for(i=0; i < size; i++){
    //    img1[yx1] = 0;
    //}

    for(y=0; y < h; y+=hs){
        yw = y*w;
        for(x=0; x < w; x+=ws){
            yx = yw + x;
            //Make local histogramm
            memset(hist, 0, sizeof(uint32)*(4096));

            for(y1=0; y1 < hs; y1+=1){
                yw1 = yx + y1*w;
                for(x1=0; x1 < ws; x1+=1){
                    yx1 = yw1 + x1;
                    hist[img[yx1]]++;
                }
            }
            //Make LUT table integral
            //b = (1<<31)/sz;
            sum = 0;
            max = sz>>8;

            for(i = 0; i < hz; i++) {
                if(hist[i] > max) {
                    sum += hist[i] - max;
                    hist[i] = max;
                }
            }
            b = (1<<30)/(sz - sum);

            sum = 0;
            for(i = 0; i < hz; i++) { sum += hist[i]; look[i] = sum*b>>22;}//   printf("%d ", look[i]);}

            //printf("\n");

            for(y1=0; y1 < hs; y1+=1){
                yw1 = yx + y1*w;
                for(x1=0; x1 < ws; x1+=1){
                    yx1 = yw1 + x1;
                    img1[yx1] = look[(img[yx1])];//*wt[y1*ws + x1];
                }
            }
        }
    }

    /*
    for(y=hs2; y < h-hs2; y+=hs){
        yw = y*w;
        for(x=ws2; x < w-ws2; x+=ws){
            yx = yw + x;
            //Make local histogramm
            memset(hist, 0, sizeof(uint32)*(4096));

            for(y1=0; y1 < hs; y1+=1){
                yw1 = yx + y1*w;
                for(x1=0; x1 < ws; x1+=1){
                    yx1 = yw1 + x1;
                    hist[img[yx1]+shift]++;
                }
            }
            //Make LUT table integral
            //b = (1<<31)/sz;
            sum = 0;
            max = sz>>8;

            for(i = 0; i < hz; i++) {
                if(hist[i] > max) {
                    sum += hist[i] - max;
                    hist[i] = max;
                }
            }
            b = (1<<31)/(sz - sum);

            sum = 0;
            for(i = 0; i < hz; i++) { sum += hist[i]; look[i] = sum*b>>23;}//   printf("%d ", look[i]);}

            //printf("\n");

            for(y1=0; y1 < hs; y1+=1){
                yw1 = yx + y1*w;
                for(x1=0; x1 < ws; x1+=1){
                    yx1 = yw1 + x1;
                    img1[yx1] += look[(img[yx1]+shift)]*wt[y1*ws + x1];
                }
            }
        }
    }
    */
    //for(i=0; i < size; i++){
    //    img1[i] = img1[i] - 128;
   // }
}

void utils_bayer_local_hdr2(int16 *img, int16 *img1, int16 *buff, uint32 w, uint32 h, BayerGrid bay, uint32 bpp)
{
    int i, j, k=0, x, x1, xs, ys, y, y1, yx, yx1, yw, yw1, h1, w1, h2, sh = bpp - 8, size = w*h; //shift = 1<<(bpp-1),
    int max1, min1, ll, df, st, diff, sp = 4, dfn;
    int maxg, ming, maxr, minr, maxb, minb, maxy, miny;
    double aw, ah, c;
    int ws = 128, hs = 128, sz = ws*hs, hz = 1<<bpp, ws2 = ws>>1, hs2 = hs>>1;
    int xa, ya, ywa, yxa, ha = h/hs+2, wa = w/ws+2, Y, yx3, yn;
    uint32 hist[4096], look[4096], sum;
    int16 *max = buff;
    int16 *min = &max[ha*wa];
    int16 *avg = &min[ha*wa];
    int16 *dif = &avg[ha*wa];
    int16 *lmax = &dif[ha*wa];
    int *a = (int*)&lmax[ha*wa];
    int *b = &a[ha*wa];

    int dr[8] = { -1, -1-wa, -wa, +1-wa, 1, 1+wa, wa, -1+wa };

    for(y=0, ya=1; y < h-hs; y+=hs, ya++){
        yw = y*w; ywa = ya*wa;
        for(x=0, xa=1; x < w-ws; x+=ws, xa++){
            yx = yw + x; yxa = ywa + xa;
            //Make local histogramm
            //memset(hist, 0, sizeof(uint32)*4096);

            sum = 0; min1 = 1<<bpp; max1 = 0;
            for(y1=0; y1 < hs; y1+=1){
                yw1 = yx + y1*w;
                for(x1=0; x1 < ws; x1+=1){
                    yx1 = yw1 + x1;
                    //hist[img[yx1]]++;
                    yx3 = yx1*3;
                    Y = YY(img[yx3], img[yx3+1], img[yx3+2]);

                    if(Y < min1) min1 = Y;
                    else if(Y > max1) max1 = Y;
                    /*
                    for(i=0; i < 3; i++){
                        if(img[yx3+i] < min1) min1 = img[yx3+1];
                        else if(img[yx3+i] > max1) max1 = img[yx3+i];
                    }
                    */
                    sum += Y;
                }
            }
            min[yxa] = min1;
            max[yxa] = max1;
            avg[yxa] = sum/(hs*ws);
            dif[yxa] = max1-min1;

            /*
            //Make LUT table integral
            //b = (1<<31)/sz;
            sum = 0;
            max1 = sz>>8;

            for(i = 0; i < hz; i++) {
                if(hist[i] > max1) {
                    sum += (hist[i] - max1);
                    hist[i] = max1;
                }
            }
            b = (1<<30)/(sz - sum);

            sum = 0;
            for(i = 0; i < hz; i++) { sum += hist[i]; look[i] = sum*b>>22; } //   printf("%d ", look[i]);}

            //printf("\n");
            //printf("y = %d x = %d\n",y, x);

            for(y1=0; y1 < hs; y1+=1){
                yw1 = yx + y1*w;
                for(x1=0; x1 < ws; x1+=1){
                    yx1 = yw1 + x1;
                    img1[yx1] = look[(img[yx1])]; //*wt[y1*ws + x1];
                }
            }
            */
        }
    }

    //Make the borders
    //Top
    for(x=1; x < wa-1; x++) {
        min[x] = min[x+wa]; max[x] = max[x+wa]; avg[x] = avg[x+wa]; dif[x] = dif[x+wa];
    }
    //Bottom
    for(x=1; x < wa-1; x++) {
        yx = (ha-1)*wa;
        min[yx] = min[yx-wa]; max[yx] = max[yx-wa]; avg[yx] = avg[yx-wa]; dif[yx] = dif[yx-wa];
    }
    //Sides
    for(y=0; y < ha; y++){
        yw = y*wa;
        min[yw] = min[yw+1]; max[yw] = max[yw+1]; avg[yw] = avg[yw+1]; dif[yw] = dif[yw+1];
        yw = yw + wa - 1;
        min[yw] = min[yw-1]; max[yw] = max[yw-1]; avg[yw] = avg[yw-1]; dif[yw] = dif[yw-1];
    }

    for(y=1; y < ha-1; y++){
        yw = y*wa;
        for(x=1; x < wa-1; x++){
            yx = yw + x;
            min1 = min[yx];
            for(i=0; i < 8; i++) if(min[yx+dr[i]] < min1) min1 = min[yx+dr[i]];
            //a[yx] = (1<<28)/(avg[yx] - min1);
            //b[yx] = -128*min1/(avg[yx] - min1);
            if(dif[yx] > 255) {
                a[yx] = (1<<26)/(avg[yx] - min[yx]);
                b[yx] = -128*min[yx]/(avg[yx] - min[yx]);
            } else {
                a[yx] = 1<<19;
                b[yx] = -(avg[yx] - 128);
            }

            printf("lmax y = %2d x = %2d dif = %d min = %d max = %d avg = %d a = %d b = %d\n",
                   yx/wa, yx%wa, dif[yx], min[yx], max[yx], avg[yx], a[yx], b[yx]);

            j=0;
            for(i=0; i < 8; i++) if(dif[yx+dr[i]] > dif[yx]) j++;
            if(!j){
                lmax[k++] = yx;
                printf("lmax y = %2d x = %2d dif = %d min = %d max = %d avg = %d\n",
                       yx/wa, yx%wa, dif[yx], min[yx], max[yx], avg[yx]);
                for(i=0; i < 8; i++)
                    printf("     y = %2d x = %2d dif = %d min = %d max = %d avg = %d\n",
                    (yx+dr[i])/wa, (yx+dr[i])%wa, dif[yx+dr[i]], min[yx+dr[i]], max[yx+dr[i]], avg[yx+dr[i]]);
            }
        }
    }

    //Image transform
    for(y=0, ya=1; y < h-hs; y+=hs, ya++){
        yw = y*w; ywa = ya*wa;
        for(x=0, xa=1; x < w-ws; x+=ws, xa++){
            yx = yw + x; yxa = ywa + xa;

            for(y1=0; y1 < hs; y1+=1){
                yw1 = yx + y1*w;
                for(x1=0; x1 < ws; x1+=1){
                    yx1 = yw1 + x1;
                    yx3 = yx1*3;
                    Y = YY(img[yx3], img[yx3+1], img[yx3+2]);

                    yn = (a[yxa]*Y>>19) + b[yxa];
                    yn = yn > 255 ? 255 : (yn < 0 ? 0 : yn);
                    if(Y){
                        img1[yx3] = img[yx3]*yn/Y; img1[yx3] = img1[yx3] > 255 ? 255 : (img1[yx3] < 0 ? 0 : img1[yx3]);
                        yx3++;
                        img1[yx3] = img[yx3]*yn/Y; img1[yx3] = img1[yx3] > 255 ? 255 : (img1[yx3] < 0 ? 0 : img1[yx3]);
                        yx3++;
                        img1[yx3] = img[yx3]*yn/Y; img1[yx3] = img1[yx3] > 255 ? 255 : (img1[yx3] < 0 ? 0 : img1[yx3]);
                    }
                    //img1[yx1] = (a[yxa]*img[yx1]>>21) + b[yxa];
                    //img1[yx1] = img1[yx1] > 255 ? 255 : (img1[yx1] < 0 ? 0 : img1[yx1]);
                    if(y==0 && x==0){
                        //printf("in = %d out = %d min = %d avg = %d a = %d b = %d\n",
                        //       img[yx1], img1[yx1], min[yxa], avg[yxa], a[yxa], b[yxa]);
                    }
                }
            }
        }
    }
}

void utils_bayer_local_hdr3(int16 *img, int16 *img1, uint32 w, uint32 h, BayerGrid bay, uint32 bpp)
{
    int i, x, x1, xs, ys, y, y1, yx, yx1, yw, yw1, h1, w1, h2, sh = bpp - 8, size = w*h;//shift = 1<<(bpp-1),
    int max, min, ll, df, st, diff, sp = 4, Y, Y1, dfn, tmp, tmp1;
    int maxg, ming, maxr, minr, maxb, minb, maxy, miny, out, a, cn;
    double aw, ah, c;
    int ws = 128, hs = 128, wt = (ws>>2)<<1, ht = (hs>>2)<<1, sz = ws*hs, hz = 1<<bpp, th = size>>8;
    uint32 hist[4096], look[4096], sum, b;
    //double wt[ws*hs];

    memset(img1, 0, sizeof(int16)*(size));

    memset(hist, 0, sizeof(uint32)*4096);

    for(i=0; i < size; i++) hist[img[i]]++;
    /*
    sum = 0;
    for(i=0; sum < th; i++ ) sum += hist[i];
    ming = i;
    sum = 0;
    for(i=4095; sum < th; i-- ) sum += hist[i];
    maxg = i;
    */
    for(i=0; !hist[i]; i++ ); ming = i;
    for(i=4095; !hist[i]; i-- ) maxg = i;

    //sz = w+h;
    //sz = h;
    printf("min = %d max = %d\n", ming, maxg);
    b = (1<<30)/sz;
    //a = (256<<20)/(maxg - ming);
    max = sz>>8;

    for(y=0; y < h-hs; y+=1){
        yw = y*w;
        for(x=0; x < w-ws; x+=1){
            yx = yw + x;
            //Make local histogramm
            memset(hist, 0, sizeof(uint32)*4096);
            cn = 0;
            min = 4095; max = 0;
            for(y1=y; y1 < hs+y; y1+=1){
                yw1 = y1*w;
                for(x1=x; x1 < ws+x; x1+=1){
                    yx1 = yw1 + x1;
                    //tmp = yx1; tmp1 = img[tmp];
                    hist[img[yx1]]++;
                }
            }

            //Make LUT table integral
            /*
            sum = 0;
            for(i = 0; i < hz; i++) {
                if(hist[i] > max) {
                    sum += hist[i] - max;
                    hist[i] = max;
                }
            }
            b = (1<<30)/(sz - sum);
            */
            sum = 0;
            for(i = 0; i < hz; i++) { sum += hist[i]; look[i] = sum*b>>22;}//   printf("%d ", look[i]);}

            tmp = yx + ht*w + wt;
            img1[tmp] = look[(img[tmp])];
        }
    }
    /*
    for(y=0; y < h; y+=1){
        yw = y*w;
        for(x=0; x < w; x+=1){
            yx = yw + x;
            //Make local histogramm
            memset(hist, 0, sizeof(uint32)*4096);
            cn = 0;
            min = 4095; max = 0;

            for(y1=0; y1 < h; y1+=1){
                yw1 = y1*w + x;
                hist[img[yw1]]++;
            }

            for(x1=0; x1 < w; x1+=1){
                 yw1 = yw + x1;
                 hist[img[yw1]]++;
            }

            sum = 0;
            for(i = 0; i < hz; i++) { sum += hist[i]; look[i] = sum*b>>22; }//   printf("%d ", look[i]);}

            //tmp = yx + ht*w + wt;
            img1[yx] = look[img[yx]];
        }
    }
    */
    //for(i=0; i < size; i++){
        //img1[i] = img1[i] - 128;
        //img1[i] = img1[i];
    //}

}

/**	\brief Bilinear algorithm for bayer to 3 image R, G, B interpolation use 3 rows buffer.
    \param img	 	The input Bayer image.
    \param R		The output red image.
    \param G		The output green image.
    \param b		The output blue image.
    \param buff		The temporary 3 rows buffer
    \param w		The image width.
    \param h		The image height.
    \param bay		The Bayer grids pattern.
    \param shift	The image shift for display.
    \retval			Output RGB image..
*/
void utils_bayer_to_RGB(int16 *img, int16 *R, int16 *G, int16 *B, int16 *buff, uint32 w, uint32 h, BayerGrid bay)
{
    int x, x1, x2, xs, ys, y = 0, wy, w2 = w<<1, yw = 0, h1, w1, h2;
    int16 *l0, *l1, *l2, *tm;
    l0 = buff; l1 = &buff[w+2]; l2 = &buff[(w+2)<<1];

    switch(bay){
        case(BGGR):{ xs = 1; ys = 1; w1 = w+1; h1 = h+1; break;}
        case(GRBG):{ xs = 1; ys = 0; w1 = w+1; h1 = h; break;}
        case(GBRG):{ xs = 0; ys = 1; w1 = w; h1 = h+1; break;}
        case(RGGB):{ xs = 0; ys = 0; w1 = w; h1 = h;   break;}
    }
    h2 = h1-1;
    //Create 3 rows buffer for transform
    l0[0] = img[w+1]; for(x=0; x < w; x++) l0[x+1] = img[w+x];  l0[w+1] = l0[w-1];
    l1[0] = img[1];   for(x=0; x < w; x++) l1[x+1] = img[x];    l1[w+1] = l1[w-1];

    for(y=ys, yw=0; y < h1; y++, yw+=w){
        wy = (y == h2) ? yw - w : yw + w;
        l2[0] = img[wy+1]; for(x=0; x < w; x++) l2[x+1] = img[wy + x];  l2[w+1] = l2[w-1];

        for(x=xs, x1=0; x < w1; x++, x1++){
            wy 	= x1 + yw;
            x2 = x1 + 1;
            if(!(y&1) && !(x&1)){
                R[wy] = l1[x2];
                G[wy] = (l0[x2] + l2[x2] + l1[x2-1] + l1[x2+1])>>2;
                B[wy] = (l0[x2+1] + l2[x2-1] + l0[x2-1] + l2[x2+1])>>2;
            }else if (!(y&1) && (x&1)){
                R[wy] = (l1[x2-1] + l1[x2+1])>>1;
                G[wy] = l1[x2];
                B[wy] =	(l0[x2] + l2[x2])>>1;
            }else if ((y&1) && !(x&1)){
                R[wy] = (l0[x2] + l2[x2])>>1;
                G[wy] = l1[x2];
                B[wy] =	(l1[x2-1] + l1[x2+1])>>1;
            }else {
                R[wy] = (l0[x2+1] + l2[x2-1] + l0[x2-1] + l2[x2+1])>>2;
                G[wy] = (l0[x2] + l2[x2] + l1[x2-1] + l1[x2+1])>>2;
                B[wy] = l1[x2];
            }
            //if(x1 < 10) printf("%3d ", R[wy]);
        }
        //printf("\n");
        tm = l0; l0 = l1; l1 = l2; l2 = tm;
    }
}

void inline static bayer_cp_line(int16 *img, int16 *c, uint32 w, uint32 sh, BayerGrid bay)
{
    uint32 i, l;
    switch(bay){
    case(BGGR):{
        for(i=0; i < sh; i++) c[i] = 0;
        for(i=0, l = sh; i < w; i++, l+=3) {
            if(i&1) {
                c[l]   = 0;
                c[l+1] = img[i];
                c[l+2] = 0;
            } else {
                c[l  ] = 0;
                c[l+1] = 0;
                c[l+2] = img[i];
            }
        }
        break;
    }
    case(GRBG):{
        for(i=0; i < sh; i++) c[i] = 0;
        for(i=0, l = sh; i < w; i++, l+=3) {
            if(i&1) {
                c[l]   = img[i];
                c[l+1] = 0;
                c[l+2] = 0;
            } else {
                c[l  ] = 0;
                c[l+1] = img[i];
                c[l+2] = 0;
            }
        }
        break;
    }
    case(GBRG):{
        for(i=0; i < sh; i++) c[i] = 0;
        for(i=0, l = sh; i < w; i++, l+=3) {
            if(i&1) {
                c[l]   = 0;
                c[l+1] = 0;
                c[l+2] = img[i];
            } else {
                c[l  ] = 0;
                c[l+1] = img[i];
                c[l+2] = 0;
            }
        }
        break;
    }
    case(RGGB):{
        for(i=0; i < sh; i++) c[i] = 0;
        for(i=0, l = sh; i < w; i++, l+=3) {
            if(i&1) {
                c[l]   = 0;
                c[l+1] = img[i];
                c[l+2] = 0;
            } else {
                c[l  ] = img[i];
                c[l+1] = 0;
                c[l+2] = 0;
            }
        }
        break;
    }
    }
}

static  inline uint32  min_4(uint32 *s)
{
    if(s[0] > s[1]){
        if(s[1] > s[2]) return 2;
        else return 1;
    } else {
        if(s[0] > s[2]) return 2;
        else return 0;
    }
}

inline static int16 g_on_br(int16 *c0, int16 *c1, int16 *c2, int16 *c3, int16 *c4, uint32 x, uint32 col1, uint32 col2)
{
    uint32 i, ima, imi, h, v, n[4], cl[4], in[2], max, min, x3 = 3*x;
    x = x3 + col1; //Green
    cl[0] = c1[x]; cl[1] = c3[x]; cl[2] = c2[x-3]; cl[3] = c2[x+3];
    h = abs(cl[2] - cl[3]);
    v = abs(cl[0] - cl[1]);
    x = x3 + col2; //Blue
    n[0] = v + abs(c0[x] - c2[x]);
    n[1] = v + abs(c2[x] - c4[x]);
    n[2] = h + abs(c2[x-6] - c2[x]);
    n[3] = h + abs(c2[x+6] - c2[x]);

    max = n[0]; min = n[0]; ima = 0; imi = 0;
    for(i=1; i < 4; i++){
        if      (n[i] >= max) { max = n[i]; ima = i;}
        else if (n[i] <= min) { min = n[i]; imi = i; }
    }
    //x = x3 + col1; //Green
    i = 0;
    if(ima != 0 && imi != 0) in[i++] = 0;
    if(ima != 1 && imi != 1) in[i++] = 1;
    if(ima != 2 && imi != 2) in[i++] = 2;
    if(ima != 3 && imi != 3) in[i++] = 3;

    //printf("n = %3d s = %3d w = %3d e = %3d ima = %d imi = %3d n1 = %3d n2 = %3d\n",
    //       n[0], n[1], n[2], n[3], ima, imi, in[0], in[1]);

    if(n[in[0]] > n[in[1]]) return (cl[imi] + cl[in[1]])>>1;
    else return (cl[imi] + cl[in[0]])>>1;

}

inline static int16 g_on_br1(int16 *c0, int16 *c1, int16 *c2, uint32 x, uint32 col)
{
    x = 3*x + col;
    return (c0[x] + c2[x] + c1[x-3] + c1[x+3])>>2;
}

inline static int16 rb_on_br(int16 *c0, int16 *c1, int16 *c2, uint32 x, uint32 col1, uint32 col2)
{
    uint32 i, ima, imi, h, v, n[4], cl[4], in[2], max, min, x3 = 3*x;
    x = x3 + col1;
    cl[0] = c0[x-3]; cl[1] = c2[x+3]; cl[2] = c0[x+3]; cl[3] = c2[x-3];
    h = abs(c0[x-3] - c2[x+3]);
    v = abs(c0[x+3] - c2[x-3]);
    x = x3 + col2; //Blue
    n[0] = h + abs(c0[x-3] - c1[x]);
    n[1] = h + abs(c2[x+3] - c1[x]);
    n[2] = v + abs(c0[x+3] - c1[x]);
    n[3] = v + abs(c2[x-3] - c1[x]);

    max = n[0]; min = n[0]; ima = 0; imi = 0;
    for(i=1; i < 4; i++){
        if      (n[i] >= max) { max = n[i]; ima = i;}
        else if (n[i] <= min) { min = n[i]; imi = i; }
    }
    //x = x3 + col1; //Green
    i = 0;
    if(ima != 0 && imi != 0) in[i++] = 0;
    if(ima != 1 && imi != 1) in[i++] = 1;
    if(ima != 2 && imi != 2) in[i++] = 2;
    if(ima != 3 && imi != 3) in[i++] = 3;

    //printf("n = %3d s = %3d w = %3d e = %3d ima = %d imi = %3d n1 = %3d n2 = %3d\n",
    //       n[0], n[1], n[2], n[3], ima, imi, in[0], in[1]);

    if(n[in[0]] > n[in[1]]) return (cl[imi] + cl[in[1]])>>1;
    else return (cl[imi] + cl[in[0]])>>1;
}

inline static int16 rb_on_br1(int16 *c0, int16 *c1, int16 *c2, uint32 x, uint32 col)
{
    x = 3*x + col;
    return (c0[x-3] + c2[x+3] + c0[x+3] + c2[x-3])>>2;
}

inline static int16 rb_on_g(int16 *c0, int16 *c1, int16 *c2, uint32 x, uint32 col1, uint32 col2)
{
    uint32 i, ima, imi, h, v, n[4], cl[4], in[2], max, min, x3 = 3*x;
    x = x3 + col1;
    cl[0] = c0[x]; cl[1] = c2[x]; cl[2] = c1[x-3]; cl[3] = c1[x+3];
    h = abs(c1[x-3] - c1[x+3]);
    v = abs(c0[x] - c2[x]);
    x = x3 + col2;
    n[0] = v + abs(c0[x] - c1[x]);
    n[1] = v + abs(c2[x] - c1[x]);
    n[2] = h + abs(c1[x-3] - c1[x]);
    n[3] = h + abs(c1[x+3] - c1[x]);

    max = n[0]; min = n[0]; ima = 0; imi = 0;
    for(i=1; i < 4; i++){
        if      (n[i] >= max) { max = n[i]; ima = i;}
        else if (n[i] <= min) { min = n[i]; imi = i; }
    }
    //x = x3 + col1; //Green
    i = 0;
    if(ima != 0 && imi != 0) in[i++] = 0;
    if(ima != 1 && imi != 1) in[i++] = 1;
    if(ima != 2 && imi != 2) in[i++] = 2;
    if(ima != 3 && imi != 3) in[i++] = 3;

    //printf("n = %3d s = %3d w = %3d e = %3d ima = %d imi = %3d n1 = %3d n2 = %3d\n",
    //       n[0], n[1], n[2], n[3], ima, imi, in[0], in[1]);

    if(n[in[0]] > n[in[1]]) return (cl[imi] + cl[in[1]])>>1;
    else return (cl[imi] + cl[in[0]])>>1;
}

inline static int16 rb_on_g1(int16 *c0, int16 *c1, int16 *c2, uint32 x, uint32 col)
{
    x = 3*x + col;
    return (c0[x] + c2[x] + c1[x-3] + c1[x+3])>>2;
}

inline static int16 g_on_b(int16 *c0, int16 *c1, int16 *c2, int16 *c3, int16 *c4, uint32 x)
{
    uint32 h, v, n, s, w, e, d, max;
    x = 3*x + 1; //Green
    h = abs(c2[x-3] - c2[x+3]);
    v = abs(c1[x]   - c3[x]);
    x = x+1; //Blue
    n = v + abs(c0[x] - c2[x]); max = n;
    s = v + abs(c2[x] - c4[x]); if(s > max) max = s;
    w = h + abs(c2[x-6] - c2[x]); if(w > max) max = w;
    e = h + abs(c2[x+6] - c2[x]); if(e > max) max = e;
    d = (max<<2) - n - s - w - e;
    x = x - 1; //Green
    if(d) return ((max - n)*c1[x] + (max - s)*c3[x] + (max - w)*c2[x-3] + (max - e)*c2[x+3])/d;
    else return c1[x];

}

inline static int16 g_on_r(int16 *c0, int16 *c1, int16 *c2, int16 *c3, int16 *c4, uint32 x)
{
    uint32 h, v, n, s, w, e, d, max;
    x = 3*x + 1; //Green
    h = abs(c2[x-3] - c2[x+3]);
    v = abs(c1[x]   - c3[x]);
    x = x-1; //Red
    n = v + abs(c0[x] - c2[x]); max = n;
    s = v + abs(c2[x] - c4[x]); if(s > max) max = s;
    w = h + abs(c2[x-6] - c2[x]); if(w > max) max = w;
    e = h + abs(c2[x+6] - c2[x]); if(e > max) max = e;
    d = (max<<2) - n - s - w - e;
    x = x + 1; //Green
    if(d) return ((max - n)*c1[x] + (max - s)*c3[x] + (max - w)*c2[x-3] + (max - e)*c2[x+3])/d;
    else return c1[x];

}

inline static int16 r_on_b(int16 *c0, int16 *c1, int16 *c2, uint32 x)
{
    uint32 h, v, n, s, w, e, d, max;
    x = 3*x; //Red
    h = abs(c0[x-3] - c2[x+3]);
    v = abs(c0[x+3] - c2[x-3]);
    x = x+1; //Green
    n = h + abs(c0[x-3] - c1[x]); max = n;
    s = h + abs(c2[x+3] - c1[x]); if(s > max) max = s;
    w = v + abs(c0[x+3] - c1[x]); if(w > max) max = w;
    e = v + abs(c2[x-3] - c1[x]); if(e > max) max = e;
    d = (max<<2) - n - s - w - e;
    x = x-1; //Red
    if(d) return ((max - n)*c0[x-3] + (max - s)*c2[x+3] + (max - w)*c0[x+3] + (max - e)*c2[x-3])/d;
    else return c0[x-3];
}

inline static int16 b_on_r(int16 *c0, int16 *c1, int16 *c2, uint32 x)
{
    uint32 h, v, n, s, w, e, d, max;
    x = 3*x+2; //Blue
    h = abs(c0[x-3] - c2[x+3]);
    v = abs(c0[x+3] - c2[x-3]);
    x = x-1; //Green
    n = h + abs(c0[x-3] - c1[x]); max = n;
    s = h + abs(c2[x+3] - c1[x]); if(s > max) max = s;
    w = v + abs(c0[x+3] - c1[x]); if(w > max) max = w;
    e = v + abs(c2[x-3] - c1[x]); if(e > max) max = e;
    d = (max<<2) - n - s - w - e;
    x = x+1; //Blue
    if(d) return ((max - n)*c0[x-3] + (max - s)*c2[x+3] + (max - w)*c0[x+3] + (max - e)*c2[x-3])/d;
    else return c0[x-3];
}

inline static int16 r_on_g(int16 *c0, int16 *c1, int16 *c2, uint32 x)
{
    uint32 h, v, n, s, w, e, d, max;
    x = 3*x; //Red
    h = abs(c1[x-3] - c1[x+3]);
    v = abs(c0[x] - c2[x]);
    x = x+1; //Green
    n = v + abs(c0[x] - c1[x]); max = n;
    s = v + abs(c2[x] - c1[x]); if(s > max) max = s;
    w = h + abs(c1[x-3] - c1[x]); if(w > max) max = w;
    e = h + abs(c1[x+3] - c1[x]); if(e > max) max = e;
    d = (max<<2) - n - s - w - e;
    x = x-1; //Red
    if(d) return ((max - n)*c0[x] + (max - s)*c2[x] + (max - w)*c1[x-3] + (max - e)*c1[x+3])/d;
    else return c0[x];
}

inline static int16 b_on_g(int16 *c0, int16 *c1, int16 *c2, uint32 x)
{
    uint32 h, v, n, s, w, e, d, max;
    x = 3*x+2; //Blue
    h = abs(c1[x-3] - c1[x+3]);
    v = abs(c0[x] - c2[x]);
    x = x-1; //Green
    n = v + abs(c0[x] - c1[x]); max = n;
    s = v + abs(c2[x] - c1[x]); if(s > max) max = s;
    w = h + abs(c1[x-3] - c1[x]); if(w > max) max = w;
    e = h + abs(c1[x+3] - c1[x]); if(e > max) max = e;
    d = (max<<2) - n - s - w - e;
    x = x+1; //Blue
    if(d) return ((max - n)*c0[x] + (max - s)*c2[x] + (max - w)*c1[x-3] + (max - e)*c1[x+3])/d;
    else return c0[x];
}

inline static void copy_color(int16 *R, int16 *G, int16 *B, int16 *c, uint32 w, uint32 sh)
{
    uint32 x, x3;
    for(x=0; x < w; x++){
        x3 = x*3 + sh;
        R[x] = c[x3  ];
        G[x] = c[x3+1];
        B[x] = c[x3+2];
    }
}

inline static void print_color(int16 *c0, int16 *c1, int16 *c2, int16 *c3, int16 *c4, int16 *c5, uint32 w)
{
    uint32 x, x3, b = 0;
    for(x=0; x < w; x++){ x3 = x*3; printf("%3d %3d %3d | ", c0[x3]+b, c0[x3+1]+b, c0[x3+2]+b);}
    printf("\n");
    for(x=0; x < w; x++){ x3 = x*3; printf("%3d %3d %3d | ", c1[x3]+b, c1[x3+1]+b, c1[x3+2]+b);}
    printf("\n");
    for(x=0; x < w; x++){ x3 = x*3; printf("%3d %3d %3d | ", c2[x3]+b, c2[x3+1]+b, c2[x3+2]+b);}
    printf("\n");
    for(x=0; x < w; x++){ x3 = x*3; printf("%3d %3d %3d | ", c3[x3]+b, c3[x3+1]+b, c3[x3+2]+b);}
    printf("\n");
    for(x=0; x < w; x++){ x3 = x*3; printf("%3d %3d %3d | ", c4[x3]+b, c4[x3+1]+b, c4[x3+2]+b);}
    printf("\n");
    for(x=0; x < w; x++){ x3 = x*3; printf("%3d %3d %3d | ", c5[x3]+b, c5[x3+1]+b, c5[x3+2]+b);}
    printf("\n");
}


/**	\brief Directionally Weighted Gradient Based Interpolation
    \param img	 	The input Bayer image.
    \param R		The output red image.
    \param G		The output green image.
    \param b		The output blue image.
    \param buff		The temporary 3 rows buffer
    \param w		The image width.
    \param h		The image height.
    \param bay		The Bayer grids pattern.
    \param shift	The image shift for display.
    \retval			Output RGB image..
*/
void utils_bayer_to_RGB_DWGI(int16 *img, int16 *R, int16 *G, int16 *B, int16 *buff, uint32 w, uint32 h, BayerGrid bay)
/*
   All RGB cameras use one of these Bayer grids:

    BGGR  0         GRBG 1          GBRG  2         RGGB 3
      0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5
    0 B G B G B G	0 G R G R G R	0 G B G B G B	0 R G R G R G
    1 G R G R G R	1 B G B G B G	1 R G R G R G	1 G B G B G B
    2 B G B G B G	2 G R G R G R	2 G B G B G B	2 R G R G R G
    3 G R G R G R	3 B G B G B G	3 R G R G R G	3 G B G B G B
 */
{
    int i, j, x, x3, yx, x1, x2, xs, ys, yw1, yw2, y = 0, y1, wy, w2 = w<<1, yw = 0, h1, w1 = w+2, h2 = h+2, w4 = w+5;
    int16 *c[6], *tm;

    c[0] = buff;
    for(i=1; i < 6; i++) c[i] = &c[i-1][w4*3];

    h1 = h+2; ys = 2; w1 = w+2; xs = 2;
    switch(bay){
    case(BGGR):{
        bayer_cp_line(&img[w*2], c[0], w, 6, BGGR);
        bayer_cp_line(&img[w  ], c[1], w, 6, GRBG);
        bayer_cp_line(&img[0  ], c[2], w, 6, BGGR);
        bayer_cp_line(&img[w  ], c[3], w, 6, GRBG);
        bayer_cp_line(&img[w*2], c[4], w, 6, BGGR);
        bayer_cp_line(&img[w*3], c[5], w, 6, GRBG);
        h1 = h; w1 = w+2;
        break;
    }
    case(GRBG):{
        bayer_cp_line(&img[w*3], c[0], w, 6, BGGR);
        bayer_cp_line(&img[w*2], c[1], w, 6, GRBG);
        bayer_cp_line(&img[w  ], c[2], w, 6, BGGR);
        bayer_cp_line(&img[0  ], c[3], w, 6, GRBG);
        bayer_cp_line(&img[w  ], c[4], w, 6, BGGR);
        bayer_cp_line(&img[w*2], c[5], w, 6, GRBG);
        h1 = h+1; w1 = w+2;
        break;
    }
    case(GBRG):{
        bayer_cp_line(&img[w*2], c[0], w, 9, GBRG);
        bayer_cp_line(&img[w  ], c[1], w, 9, RGGB);
        bayer_cp_line(&img[0  ], c[2], w, 9, GBRG);
        bayer_cp_line(&img[w  ], c[3], w, 9, RGGB);
        bayer_cp_line(&img[w*2], c[4], w, 9, GBRG);
        bayer_cp_line(&img[w*3], c[5], w, 9, RGGB);
        h1 = h; w1 = w+3;
        break;
    }
    case(RGGB):{
        bayer_cp_line(&img[w*3], c[0], w, 9, GBRG);
        bayer_cp_line(&img[w*2], c[1], w, 9, RGGB);
        bayer_cp_line(&img[w  ], c[2], w, 9, GBRG);
        bayer_cp_line(&img[0  ], c[3], w, 9, RGGB);
        bayer_cp_line(&img[w  ], c[4], w, 9, GBRG);
        bayer_cp_line(&img[w*2], c[5], w, 9, RGGB);
        h1 = h+1; w1 = w+3;
        break;
    }
    }
    print_color(c[0], c[1], c[2], c[3], c[4], c[5], 10);
    //Prepare for the loop
    //Green on the blue
    //for(x=2, x3 = 7; x < w1; x+=2, x3+=6)  c[2][x3] = g_on_b(c[0], c[1], c[2], c[3], c[4], x);
    //for(x=2, x3 = 7; x < w1; x+=2, x3+=6)  c[2][x3] = g_on_br(c[0], c[1], c[2], c[3], c[4], x, 1, 2);
    for(x=2, x3 = 7; x < w1; x+=2, x3+=6)  c[2][x3] = g_on_br1(c[1], c[2], c[3], x, 1);
    //Green on the red pixel
    //for(x=3, x3 = 10; x < w1; x+=2, x3+=6)  c[3][x3] = g_on_b(c[1], c[2], c[3], c[4], c[5], x);
    //for(x=3, x3 = 10; x < w1; x+=2, x3+=6)  c[3][x3] = g_on_br(c[1], c[2], c[3], c[4], c[5], x, 1, 0);
    for(x=3, x3 = 10; x < w1; x+=2, x3+=6)  c[3][x3] = g_on_br1(c[2], c[3], c[4], x, 1);
    //Red on the Blue
    //for(x=2, x3 = 6; x < w1; x+=2, x3+=6)  c[2][x3] = r_on_b(c[1], c[2], c[3], x);
    //for(x=2, x3 = 6; x < w1; x+=2, x3+=6)  c[2][x3] = rb_on_br(c[1], c[2], c[3], x, 0, 2);
    for(x=2, x3 = 6; x < w1; x+=2, x3+=6)  c[2][x3] = rb_on_br1(c[1], c[2], c[3], x, 0);
    //Red on the Green
    //for(x=3, x3 = 9; x < w1; x+=2, x3+=6)  c[2][x3] = r_on_g(c[1], c[2], c[3], x);
    //for(x=3, x3 = 9; x < w1; x+=2, x3+=6)  c[2][x3] = rb_on_g(c[1], c[2], c[3], x, 0, 1);
    for(x=3, x3 = 9; x < w1; x+=2, x3+=6)  c[2][x3] = rb_on_g1(c[1], c[2], c[3], x, 0);

    tm = c[0]; c[0] = c[1]; c[1] = c[2]; c[2] = c[3]; c[3] = c[4]; c[4] = c[5]; c[5] = tm;

    switch(bay){
    case(BGGR):{ yw1 = w*4; bayer_cp_line(&img[yw1], c[5], w, 6, BGGR); break; }
    case(GRBG):{ yw1 = w*3; bayer_cp_line(&img[yw1], c[5], w, 6, BGGR); break; }
    case(GBRG):{ yw1 = w*4; bayer_cp_line(&img[yw1], c[5], w, 9, GBRG); break; }
    case(RGGB):{ yw1 = w*3; bayer_cp_line(&img[yw1], c[5], w, 9, GBRG); break; }
    }

    printf("\n");
    print_color(c[0], c[1], c[2], c[3], c[4], c[5], 10);
    yw2 = 0;
    for(y=0; y < h1; y++){
        if(!(y&1)){
            //Green on the blue
            //for(x=2, x3 = 7; x < w1; x+=2, x3+=6)  c[3][x3] = g_on_b(c[1], c[2], c[3], c[4], c[5], x);
            //for(x=2, x3 = 7; x < w1; x+=2, x3+=6)  c[3][x3] = g_on_br(c[1], c[2], c[3], c[4], c[5], x, 1, 2);
            for(x=2, x3 = 7; x < w1; x+=2, x3+=6)  c[3][x3] = g_on_br1(c[2], c[3], c[4], x, 1);
            //Blue on the Red
            //for(x=3, x3 = 11; x < w1; x+=2, x3+=6)  c[2][x3] = b_on_r(c[1], c[2], c[3], x);
            //for(x=3, x3 = 11; x < w1; x+=2, x3+=6)  c[2][x3] = rb_on_br(c[1], c[2], c[3], x, 2, 0);
            for(x=3, x3 = 11; x < w1; x+=2, x3+=6)  c[2][x3] = rb_on_br1(c[1], c[2], c[3], x, 2);
            //Blue on the Green
            //for(x=2, x3 = 8; x < w1; x+=2, x3+=6)  c[2][x3] = b_on_g(c[1], c[2], c[3], x);
            //for(x=2, x3 = 8; x < w1; x+=2, x3+=6)  c[2][x3] = rb_on_g(c[1], c[2], c[3], x, 2, 1);
            for(x=2, x3 = 8; x < w1; x+=2, x3+=6)  c[2][x3] = rb_on_g1(c[1], c[2], c[3], x, 2);
            //Blue on the Green
            //for(x=3, x3 = 11; x < w1; x+=2, x3+=6)  c[1][x3] = b_on_g(c[0], c[1], c[2], x);
            //for(x=3, x3 = 11; x < w1; x+=2, x3+=6)  c[1][x3] = rb_on_g(c[0], c[1], c[2], x, 2, 1);
            for(x=3, x3 = 11; x < w1; x+=2, x3+=6)  c[1][x3] = rb_on_g1(c[0], c[1], c[2], x, 2);
        } else {
            //Green on the Red
            //for(x=3, x3 = 10; x < w1; x+=2, x3+=6)  c[3][x3] = g_on_r(c[1], c[2], c[3], c[4], c[5], x);
            //for(x=3, x3 = 10; x < w1; x+=2, x3+=6)  c[3][x3] = g_on_br(c[1], c[2], c[3], c[4], c[5], x, 1, 0);
            for(x=3, x3 = 10; x < w1; x+=2, x3+=6)  c[3][x3] = g_on_br1(c[2], c[3], c[4], x, 1);
            //Red on the Blue
            //for(x=2, x3 = 6; x < w1; x+=2, x3+=6)  c[2][x3] = r_on_b(c[1], c[2], c[3], x);
            //for(x=2, x3 = 6; x < w1; x+=2, x3+=6)  c[2][x3] = rb_on_br(c[1], c[2], c[3], x, 0, 2);
            for(x=2, x3 = 6; x < w1; x+=2, x3+=6)  c[2][x3] = rb_on_br1(c[1], c[2], c[3], x, 0);
            //Red on the Green
            //for(x=3, x3 = 9; x < w1; x+=2, x3+=6)  c[2][x3] = r_on_g(c[1], c[2], c[3], x);
            //for(x=3, x3 = 9; x < w1; x+=2, x3+=6)  c[2][x3] = rb_on_g(c[1], c[2], c[3], x, 0, 1);
            for(x=3, x3 = 9; x < w1; x+=2, x3+=6)  c[2][x3] = rb_on_g1(c[1], c[2], c[3], x, 0);
            //Red on the Green
            //for(x=2, x3 = 6; x < w1; x+=2, x3+=6)  c[1][x3] = r_on_g(c[0], c[1], c[2], x);
            //for(x=2, x3 = 6; x < w1; x+=2, x3+=6)  c[1][x3] = rb_on_g(c[0], c[1], c[2], x, 0, 1);
            for(x=2, x3 = 6; x < w1; x+=2, x3+=6)  c[1][x3] = rb_on_g1(c[0], c[1], c[2], x, 0);
        }

        //printf("\n");

        tm = c[0]; c[0] = c[1]; c[1] = c[2]; c[2] = c[3]; c[3] = c[4]; c[4] = c[5]; c[5] = tm;
        yw1 = yw1 + w;
        switch(bay){
        case(BGGR):{
            copy_color(&R[yw2], &G[yw2], &B[yw2], c[0], w, 6);
            yw2 = yw2 + w;
            if(y&1) bayer_cp_line(&img[yw1], c[5], w, 6, BGGR);
            else    bayer_cp_line(&img[yw1], c[5], w, 6, GRBG);
            break;
        }
        case(GRBG):{
            if(y > 0){
                copy_color(&R[yw2], &G[yw2], &B[yw2], c[0], w, 6);
                yw2 = yw2 + w;
            }
            if(y&1) bayer_cp_line(&img[yw1], c[5], w, 6, BGGR);
            else    bayer_cp_line(&img[yw1], c[5], w, 6, GRBG);
            break;
        }
        case(GBRG):{
            copy_color(&R[yw2], &G[yw2], &B[yw2], c[0], w, 9);
            yw2 = yw2 + w;
            if(y&1) bayer_cp_line(&img[yw1], c[5], w, 9, GBRG);
            else    bayer_cp_line(&img[yw1], c[5], w, 9, RGGB);
            break;
        }
        case(RGGB):{
            if(y > 0){
                copy_color(&R[yw2], &G[yw2], &B[yw2], c[0], w, 9);
                yw2 = yw2 + w;
            }
            if(y&1) bayer_cp_line(&img[yw1], c[5], w, 9, GBRG);
            else    bayer_cp_line(&img[yw1], c[5], w, 9, RGGB);
            break;
        }
        }
        if(y < 10) {
            printf("\n");
            print_color(c[0], c[1], c[2], c[3], c[4], c[5], 10);
        }
    }
}

void inline static cp_line_in(int16 *img, int16 *c, uint32 w)
{
    uint32 i;
    c[0] = img[2]; c[1] = img[1];
    for(i=0; i < w; i++)  c[i+2] = img[i];
    c[i+2] = img[w-2]; c[i+3] = img[w-3];
}

void inline static cp_line_out(int16 *img, int16 *c, uint32 w)
{
    uint32 i;
    for(i=0; i < w; i++)  img[i] = c[i+2];
}

void utils_bayer_denoise(int16 *img, int16 *img1, int16 *buff, uint32 w, uint32 h, BayerGrid bay)
/*
   All RGB cameras use one of these Bayer grids:

    BGGR  0         GRBG 1          GBRG  2         RGGB 3
      0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5
    0 B G B G B G	0 G R G R G R	0 G B G B G B	0 R G R G R G
    1 G R G R G R	1 B G B G B G	1 R G R G R G	1 G B G B G B
    2 B G B G B G	2 G R G R G R	2 G B G B G B	2 R G R G R G
    3 G R G R G R	3 B G B G B G	3 R G R G R G	3 G B G B G B
 */
{
    int i, j, x, x3, yx, x1, x2, xs, ys, yw1, yw2, y = 0, y1, wy, w2 = w<<1, yw = 0, h1, w1 = w+2, h2 = h+2, w4 = w+4;
    int16 *c[6], *tm;

    c[0] = buff;
    for(i=1; i < 5; i++) c[i] = &c[i-1][w4];

    cp_line_in(&img[w*2], c[0], w);
    cp_line_in(&img[w  ], c[1], w);
    cp_line_in(&img[0  ], c[2], w);
    cp_line_in(&img[w  ], c[3], w);
    cp_line_in(&img[w*2], c[4], w);
    //print_color(c[0], c[1], c[2], c[3], c[4], c[5], 10);


    for(y=0; y < h1; y++){
        if(!(y&1)){

            for(x=2; x < w1; x+=2){
                //Blue
                //c[2][x]   = noise_rb(c[0], c[1], c[2], c[3], c[4], x);
                //Green
                //c[2][x+1] = noise_g(c[0], c[1], c[2], c[3], c[4], x+1);
            }
        } else {
            for(x=2; x < w1; x+=2){
                //Green
                //c[2][x]   = noise_g(c[0], c[1], c[2], c[3], c[4], x);
                //Red
                //c[2][x+1] = noise_rb(c[0], c[1], c[2], c[3], c[4], x+1);
            }
        }

        yw1 = yw1 + w;

        tm = c[0]; c[0] = c[1]; c[1] = c[2]; c[2] = c[3]; c[3] = c[4]; c[4] = tm;

        if(y < 10) {
            printf("\n");
            print_color(c[0], c[1], c[2], c[3], c[4], c[5], 10);
        }

    }
}

/**	\brief Transform bayer image to YUV444 format.
    \param img	 	The input Bayer image.
    \param Y		The output Y image.
	\param U		The output U image.
	\param V		The output V image.
 	\param buff		The temporary 3 rows buffer.
	\param w		The image width.
	\param h		The image height.
	\param bay		The Bayer grids pattern.
*/
void utils_bayer_to_YUV444(int16 *img, int16 *Y, int16 *U, int16 *V, int16 *buff, uint32 w, uint32 h, BayerGrid bay){
/*
   All RGB cameras use one of these Bayer grids:

	BGGR  0         GRBG 1          GBRG  2         RGGB 3
	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5
	0 B G B G B G	0 G R G R G R	0 G B G B G B	0 R G R G R G
	1 G R G R G R	1 B G B G B G	1 R G R G R G	1 G B G B G B
	2 B G B G B G	2 G R G R G R	2 G B G B G B	2 R G R G R G
	3 G R G R G R	3 B G B G B G	3 R G R G R G	3 G B G B G B
 */
    int x, x1, x2, xs, ys, y = 0, wy, yw = 0, h1, w1, h2;
	int16 *l0, *l1, *l2, *tm;
	int r, g, b;
    l0 = buff; l1 = &l0[w+2]; l2 = &l1[w+2];

	switch(bay){
		case(BGGR):{ xs = 1; ys = 1; w1 = w+1; h1 = h+1; break;}
        case(GRBG):{ xs = 1; ys = 0; w1 = w+1; h1 = h;   break;}
        case(GBRG):{ xs = 0; ys = 1; w1 = w;   h1 = h+1; break;}
        case(RGGB):{ xs = 0; ys = 0; w1 = w;   h1 = h;   break;}
	}
	h2 = h1-1;
	//Create 3 rows buffer for transform

	l0[0] = img[w+1]; for(x=0; x < w; x++) l0[x+1] = img[w+x];  l0[w+1] = l0[w-1];
	l1[0] = img[1];   for(x=0; x < w; x++) l1[x+1] = img[x];    l1[w+1] = l1[w-1];

    for(y=ys, yw=0; y < h1; y++, yw+=w){
		wy = (y == h2) ? yw - w : yw + w;
		l2[0] = img[wy+1]; for(x=0; x < w; x++) l2[x+1] = img[wy + x];  l2[w+1] = l2[w-1];

        for(x=xs, x1=0; x < w1; x++, x1++){
			wy = x1 + yw;
			x2 = x1 + 1;
			//xwy3 = wy + wy + wy;

			if(!(y&1) && !(x&1)){
				r = l1[x2];
				g = (l0[x2] + l2[x2] + l1[x2-1] + l1[x2+1])>>2;
				b = (l0[x2+1] + l2[x2-1] + l0[x2-1] + l2[x2+1])>>2;
			}else if (!(y&1) && (x&1)){
				r = (l1[x2-1] + l1[x2+1])>>1;
				g = l1[x2];
				b =	(l0[x2] + l2[x2])>>1;
			}else if ((y&1) && !(x&1)){
				r = (l0[x2] + l2[x2])>>1;
				g = l1[x2];
				b =	(l1[x2-1] + l1[x2+1])>>1;
			}else {
				r = (l0[x2+1] + l2[x2-1] + l0[x2-1] + l2[x2+1])>>2;
				g = (l0[x2] + l2[x2] + l1[x2-1] + l1[x2+1])>>2;
				b = l1[x2];
			}
			Y[wy] = ((306*(r - g) + 117*(b - g))>>10) + g;
            U[wy] = 578*(b - Y[wy])>>10;
            V[wy] = 730*(r - Y[wy])>>10;
            //printf("Y = %d x = %d y = %d x = %d y = %d\n", Y[wy], wy%w, wy/w, x, y);
		}
		tm = l0; l0 = l1; l1 = l2; l2 = tm;
	}
}

void utils_bayer_to_YUV420_16(int16 *img, uint8 *Y, uint8 *U, uint8 *V, int16 *buff, uint32 w, uint32 h, BayerGrid bay){
/*
   All RGB cameras use one of these Bayer grids:

	BGGR  0         GRBG 1          GBRG  2         RGGB 3
	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5
	0 B G B G B G	0 G R G R G R	0 G B G B G B	0 R G R G R G
	1 G R G R G R	1 B G B G B G	1 R G R G R G	1 G B G B G B
	2 B G B G B G	2 G R G R G R	2 G B G B G B	2 R G R G R G
	3 G R G R G R	3 B G B G B G	3 R G R G R G	3 G B G B G B
 */
	int x, x1, x2, xs, ys, y = 0, y1, wy, wy1, w2 = w<<1,  w3 = w>>1, yw, h1, w1, h2;
	int16 *l0, *l1, *l2, *tm;
	int r, g, b;
	l0 = buff; l1 = &buff[w+2]; l2 = &buff[(w+2)<<1];

	switch(bay){
		case(BGGR):{ xs = 1; ys = 1; w1 = w+1; h1 = h+1; break;}
		case(GRBG):{ xs = 1; ys = 0; w1 = w+1; h1 = h; break;}
		case(GBRG):{ xs = 0; ys = 1; w1 = w; h1 = h+1; break;}
		case(RGGB):{ xs = 0; ys = 0; w1 = w; h1 = h;   break;}
	}
	h2 = h1-1;
	//Create 3 rows buffer for transform

	l0[0] = img[w+1]; for(x=0; x < w; x++) l0[x+1] = img[w+x];  l0[w+1] = l0[w-1];
	l1[0] = img[1];   for(x=0; x < w; x++) l1[x+1] = img[x];    l1[w+1] = l1[w-1];

	for(y=ys, y1=0; y < h1; y++, y1++){
		yw = y1*w;
		wy = (y == h2) ? yw - w : yw + w;
		l2[0] = img[wy+1]; for(x=0; x < w; x++) l2[x+1] = img[wy + x];  l2[w+1] = l2[w-1];

		for(x=xs, x1=0; x < w1; x++, x1++){
			wy = x1 + yw;
			x2 = x1 + 1;
			wy1 = (x1>>1) + (y1>>1)*w3;

			if(!(y&1) && !(x&1)){
				r = l1[x2];
				g = (l0[x2] + l2[x2] + l1[x2-1] + l1[x2+1])>>2;
				b = (l0[x2+1] + l2[x2-1] + l0[x2-1] + l2[x2+1])>>2;
				Y[wy] = ((306*(r - g) + 117*(b - g))>>10) + g;
				V[wy1] = 730*(r - Y[wy])>>10;
			}else if (!(y&1) && (x&1)){
				r = (l1[x2-1] + l1[x2+1])>>1;
				g = l1[x2];
				b =	(l0[x2] + l2[x2])>>1;
				Y[wy] = ((306*(r - g) + 117*(b - g))>>10) + g;
			}else if ((y&1) && !(x&1)){
				r = (l0[x2] + l2[x2])>>1;
				g = l1[x2];
				b =	(l1[x2-1] + l1[x2+1])>>1;
				Y[wy] = ((306*(r - g) + 117*(b - g))>>10) + g;
			}else {
				r = (l0[x2+1] + l2[x2-1] + l0[x2-1] + l2[x2+1])>>2;
				g = (l0[x2] + l2[x2] + l1[x2-1] + l1[x2+1])>>2;
				b = l1[x2];
				Y[wy] = ((306*(r - g) + 117*(b - g))>>10) + g;
				U[wy1] = 578*(b - Y[wy])>>10;
			}
		}
		tm = l0; l0 = l1; l1 = l2; l2 = tm;
	}
}

/**	\brief Transform bayer image to YUV420 format.
    \param img	 	The input Bayer image.
    \param Y		The output Y image.
	\param U		The output U image.
	\param V		The output V image.
 	\param buff		The temporary 3 rows buffer.
	\param w		The image width.
	\param h		The image height.
	\param bay		The Bayer grids pattern.
*/
void utils_bayer_to_YUV420(int16 *img, uint8 *Y, uint8 *U, uint8 *V, int16 *buff, uint32 w, uint32 h, BayerGrid bay){
/*
   All RGB cameras use one of these Bayer grids:

	BGGR  0         GRBG 1          GBRG  2         RGGB 3
	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5
	0 B G B G B G	0 G R G R G R	0 G B G B G B	0 R G R G R G
	1 G R G R G R	1 B G B G B G	1 R G R G R G	1 G B G B G B
	2 B G B G B G	2 G R G R G R	2 G B G B G B	2 R G R G R G
	3 G R G R G R	3 B G B G B G	3 R G R G R G	3 G B G B G B
 */
	int x, x1, x2, xs, ys, y = 0, y1, wy, wy1, w2 = w<<1,  w3 = w>>1, yw, h1, w1, h2;
	int16 *l0, *l1, *l2, *tm;
    int r, g, b, sh = 0;
	l0 = buff; l1 = &buff[w+2]; l2 = &buff[(w+2)<<1];

	switch(bay){
		case(BGGR):{ xs = 1; ys = 1; w1 = w+1; h1 = h+1; break;}
		case(GRBG):{ xs = 1; ys = 0; w1 = w+1; h1 = h; break;}
		case(GBRG):{ xs = 0; ys = 1; w1 = w; h1 = h+1; break;}
		case(RGGB):{ xs = 0; ys = 0; w1 = w; h1 = h;   break;}
	}
	h2 = h1-1;
	//Create 3 rows buffer for transform

	l0[0] = img[w+1]; for(x=0; x < w; x++) l0[x+1] = img[w+x];  l0[w+1] = l0[w-1];
	l1[0] = img[1];   for(x=0; x < w; x++) l1[x+1] = img[x];    l1[w+1] = l1[w-1];

	for(y=ys, y1=0; y < h1; y++, y1++){
		yw = y1*w;
		wy = (y == h2) ? yw - w : yw + w;
		//yw = (y1+1)*(w+2);
		//wy = (y == h2) ? yw - (w+2) : yw + (w+2);
		l2[0] = img[wy+1]; for(x=0; x < w; x++) l2[x+1] = img[wy + x];  l2[w+1] = l2[w-1];

		for(x=xs, x1=0; x < w1; x++, x1++){
			//wy = x1 + yw;
			//x2 = x1 + 1;
			//wy1 = (x1>>1) + (y1>>1)*w3;
			wy = x1 + (y1+1)*(w+2) +1;
			x2 = x1 + 1;
			wy1 = (x1>>1) + ((y1>>1)+1)*(w3+2) + 1;

			if(!(y&1) && !(x&1)){
				r = l1[x2];
				g = (l0[x2] + l2[x2] + l1[x2-1] + l1[x2+1])>>2;
				b = (l0[x2+1] + l2[x2-1] + l0[x2-1] + l2[x2+1])>>2;
				Y[wy] = lb1(((306*(r - g) + 117*(b - g))>>10) + g + sh);
				V[wy1] = lb1((730*(r - Y[wy])>>10) + sh);
			}else if (!(y&1) && (x&1)){
				r = (l1[x2-1] + l1[x2+1])>>1;
				g = l1[x2];
				b =	(l0[x2] + l2[x2])>>1;
				Y[wy] = lb1(((306*(r - g) + 117*(b - g))>>10) + g + sh);
			}else if ((y&1) && !(x&1)){
				r = (l0[x2] + l2[x2])>>1;
				g = l1[x2];
				b =	(l1[x2-1] + l1[x2+1])>>1;
				Y[wy] = lb1(((306*(r - g) + 117*(b - g))>>10) + g + sh);
			}else {
				r = (l0[x2+1] + l2[x2-1] + l0[x2-1] + l2[x2+1])>>2;
				g = (l0[x2] + l2[x2] + l1[x2-1] + l1[x2+1])>>2;
				b = l1[x2];
				Y[wy] = lb1(((306*(r - g) + 117*(b - g))>>10) + g + sh);
				U[wy1] = lb1((578*(b - Y[wy])>>10) + sh);
			}
		}
		tm = l0; l0 = l1; l1 = l2; l2 = tm;
	}
}

/**	\brief Simple gradient method of bayer interpolation algorithm.
    \param img	 	The input Bayer image.
 	\param rgb		The output RGB image.
	\param w		The image width.
	\param h		The image height.
	\retval			Output RGB image..
*/
uint8* utils_bayer_to_rgb_grad(int16 *img, uint8 *rgb, uint32 w, uint32 h, BayerGrid bay, int shift){
	//TODO: Work only for RGGB, need to make for all Bayer grids
	uint32 x, y, wy, xwy, xwy3, y2, x2, a, b, yw, w2 = w<<1;

	for(y=2, yw=w2; y < h-2; y++, yw+=w){
		for(x=2; x < w-2; x++){
			//y2 = oe(a,y);
			//x2 = oe(b,x);
			wy 	= x + yw;
			xwy3 = wy + wy + wy;
			if(!(y&1) && !(x&1)){
				rgb[xwy3] 	= 	lb1(img[wy] + shift);
				rgb[xwy3+1] = 	lb1((abs(img[wy-1] - img[wy+1]) > abs(img[wy-w] - img[wy+w]) ?
								(img[wy-w] + img[wy+w])>>1 : (img[wy-1] + img[wy+1])>>1) + shift);
				rgb[xwy3+2] = 	lb1((abs(img[wy-1-w] - img[wy+1+w]) > abs(img[wy+1-w] - img[wy-1+w]) ?
								(img[wy+1-w] + img[wy-1+w])>>1 : (img[wy-1-w] + img[wy+1+w])>>1) + shift);
			}else if (!(y&1) && (x&1)){
				rgb[xwy3] = 	lb1((abs(img[wy-1] - img[wy+1]) > abs(img[wy-w2-1] + img[wy-w2+1] - img[wy+w2-1] - img[wy+w2+1])>>1 ?
								(img[wy-w2-1] + img[wy-w2+1] + img[wy+w2-1] + img[wy+w2+1])>>2 : (img[wy-1] + img[wy+1])>>1) + shift);
				rgb[xwy3+1] = 	lb1(img[wy] + shift);
				rgb[xwy3+2] =	lb1((abs(img[wy-w] - img[wy+w]) > abs(img[wy-w-2] + img[wy+w-2] - img[wy-w+2] - img[wy+w+2])>>1 ?
								(img[wy-w-2] + img[wy+w-2] + img[wy-w+2] + img[wy+w+2])>>2 : (img[wy-w] + img[wy+w])>>1) + shift);
			}else if ((y&1) && !(x&1)){
				rgb[xwy3] = 	lb1((abs(img[wy-w] - img[wy+w]) > abs(img[wy-w-2] + img[wy+w-2] - img[wy-w+2] - img[wy+w+2])>>1 ?
								(img[wy-w-2] + img[wy+w-2] + img[wy-w+2] + img[wy+w+2])>>2 : (img[wy-w] + img[wy+w])>>1) + shift);
				rgb[xwy3+1] = 	lb1(img[wy] + shift);
				rgb[xwy3+2] =	lb1((abs(img[wy-1] - img[wy+1]) > abs(img[wy-w2-1] + img[wy-w2+1] - img[wy+w2-1] - img[wy+w2+1])>>1 ?
								(img[wy-w2-1] + img[wy-w2+1] + img[wy+w2-1] + img[wy+w2+1])>>2 : (img[wy-1] + img[wy+1])>>1) + shift);
			}else {
				rgb[xwy3] = 	lb1((abs(img[wy-1-w] - img[wy+1+w]) > abs(img[wy+1-w] - img[wy-1+w]) ?
								(img[wy+1-w] + img[wy-1+w])>>1 : (img[wy-1-w] + img[wy+1+w])>>1) + shift);
				rgb[xwy3+1] = 	lb1((abs(img[wy-1] - img[wy+1]) > abs(img[wy-w] - img[wy+w]) ?
								(img[wy-w] + img[wy+w])>>1 : (img[wy-1] + img[wy+1])>>1) + shift);
				rgb[xwy3+2] = 	lb1(img[wy] + shift);
			}
		}
	}
	return rgb;
}

/** \brief Convert RGB24 image to YUV444.
	\param rgb 	The input RGB image.
    \param y	The output Y image.
	\param u	The output U image.
	\param v	The output V image.
    \param w	The image width.
    \param h	The image height.
    \param pad		If pad = 1 need to pad each line to uint32 boundary, if pad = 0 no need
*/
void utils_RGB24_to_YUV444(uint8 *rgb, int16 *Y, int16 *U, int16 *V, uint32 w, uint32 h, uint32 bpp, uint32 pad)
{
	/*
	Y = 0.299*R + 0.587*G + 0.114*B
	U = -0.169*R – 0.331*G + 0.5*B
	V = 0.5*R - 0.419*G - 0.081*B
	*/
	int i, i3, sz = w*h, shift = 1<<(bpp-1);
	uint32 x, x1, y, yw, yw1 , yx3, yx1, w1 = (((w*3)>>2) + ((w*3)&3 ? 1 : 0))<<2;

	if(!pad){
		for(i=0; i < sz; i++){
			i3 = i*3;
			Y[i] = ((306*(rgb[i3]-rgb[i3 + 1]) + 117*(rgb[i3 + 2]-rgb[i3 + 1]))>>10) + rgb[i3 + 1] - shift;
			U[i] = 578*(rgb[i3 + 2]-Y[i]-shift)>>10;
			V[i] = 730*(rgb[i3]-Y[i]-shift)>>10;
		}
	} else {
		//For each line padded to a uint32 boundary.
		//printf("w = %d w1 = %d\n", w*3, w1);
		for(y=0; y < h; y++) {
			yw = y*w1;
			yw1 = y*w;
			for(x=0, x1 = 0; x < w1; x+=3, x1++) {
				yx3 = x+yw;
				yx1 = yw1 + x1;
				Y[yx1] = ((306*(rgb[yx3]-rgb[yx3 + 1]) + 117*(rgb[yx3 + 2]-rgb[yx3 + 1]))>>10) + rgb[yx3 + 1] - shift;
				U[yx1] = 578*(rgb[yx3 + 2]-Y[yx1]-shift)>>10;
				V[yx1] = 730*(rgb[yx3]-Y[yx1]-shift)>>10;
			}
		}
	}
	//y[i] = ((306*rgb[i3] + 601*rgb[i3 + 1] + 117*rgb[i3 + 2])>>10) - 128;
	//u[i] = (rgb[i3 + 2]>>1) - ((173*rgb[i3] + 339*rgb[i3 + 1])>>10);
	//v[i] = (rgb[i3    ]>>1) - ((429*rgb[i3 + 1] + 83*rgb[i3 + 2])>>10);
	//printf("%d %d %d  ", y[i], u[i], v[i]);
}

/** \brief Convert RGB24 image to YUV420.
	\param rgb 	The input RGB image.
    \param y	The output Y image.
	\param u	The output U image.
	\param v	The output V image.
    \param w	The image width.
    \param h	The image height.
    \param pad		If pad = 1 need to pad each line to uint32 boundary, if pad = 0 no need
*/
void utils_RGB24_to_YUV420(uint8 *rgb, int16 *Y, int16 *U, int16 *V, uint32 w, uint32 h, uint32 bpp, uint32 pad)
{
	/*
	Y = 0.299*R + 0.587*G + 0.114*B
	U = -0.169*R – 0.331*G + 0.5*B
	V = 0.5*R - 0.419*G - 0.081*B
	*/
	int y, x, yw, yx, yx2, yx3, w2 = w>>1, shift = 1<<(bpp-1);
	uint32  x1, yw1 , yx1, w1 = (((w*3)>>2) + ((w*3)&3 ? 1 : 0))<<2;

	if(!pad){
		for(y=0; y < h; y++){
			yw = y*w;
			for(x=0; x < w; x++){
				yx = yw + x;
				yx3 = yx*3;
				yx2 = (x>>1) + (y>>1)*w2;

				Y[yx] = ((306*(rgb[yx3]-rgb[yx3 + 1]) + 117*(rgb[yx3 + 2]-rgb[yx3 + 1]))>>10) + rgb[yx3 + 1] - shift;
				U[yx2] += 578*(rgb[yx3 + 2]-Y[yx]-shift)>>10;
				V[yx2] += 730*(rgb[yx3]-Y[yx]-shift)>>10;
				if(x&1 && y&1) {
					U[yx2]>>=2;
					V[yx2]>>=2;
				}
			}
		}
	} else {
		//For each line padded to a uint32 boundary.
		//printf("w = %d w1 = %d\n", w*3, w1);
		for(y=0; y < h; y++) {
			yw = y*w1;
			yw1 = y*w;
			for(x=0, x1 = 0; x < w1; x+=3, x1++) {
				yx3 = x+yw;
				yx1 = yw1 + x1;
				yx2 = (x1>>1) + (y>>1)*w2;
				Y[yx1] = ((306*(rgb[yx3]-rgb[yx3 + 1]) + 117*(rgb[yx3 + 2]-rgb[yx3 + 1]))>>10) + rgb[yx3 + 1] - shift;
				U[yx2] += 578*(rgb[yx3 + 2]-Y[yx1]-shift)>>10;
				V[yx2] += 730*(rgb[yx3]-Y[yx1]-shift)>>10;
				if(x1&1 && y&1) {
					U[yx2]>>=2;
					V[yx2]>>=2;
				}
			}
		}
	}
	//y[i] = ((306*rgb[i3] + 601*rgb[i3 + 1] + 117*rgb[i3 + 2])>>10) - 128;
	//u[i] = (rgb[i3 + 2]>>1) - ((173*rgb[i3] + 339*rgb[i3 + 1])>>10);
	//v[i] = (rgb[i3    ]>>1) - ((429*rgb[i3 + 1] + 83*rgb[i3 + 2])>>10);
	//printf("%d %d %d  ", y[i], u[i], v[i]);
}

/** \brief Copy 24 bits rgb format image to three images R, G, B
	\param img	 	The input RGB24 image.
    \param r		The output red image.
    \param g		The output green image.
    \param b		The output blue image.
    \param w		The image width.
    \param h		The image height.
    \param bpp		The bits per pixel.
    \param pad		If pad = 1 need to pad each line to uint32 boundary, if pad = 0 no need
*/
void utils_RGB24_to_RGB(uint8 *img, int16 *r, int16 *g, int16 *b, uint32 w, uint32 h, uint32 bpp, uint32 pad)
{
	uint32 i, i3, size = w*h, shift = 1<<(bpp-1);
	uint32 x, x1, y, yw, yw1 , yx3, yx1, w1 = (((w*3)>>2) + ((w*3)&3 ? 1 : 0))<<2;
	if(!pad){
		for(i=0; i<size; i++) {
			i3 = i*3;
			r[i] = img[i3]   - shift;
			g[i] = img[i3+1] - shift;
			b[i] = img[i3+2] - shift;
		}
	} else {
		//For each line padded to a uint32 boundary.
		//printf("w = %d w1 = %d\n", w*3, w1);
		for(y=0; y < h; y++) {
			yw = y*w1;
			yw1 = y*w;
			for(x=0, x1 = 0; x < w1; x+=3, x1++) {
				yx3 = x+yw;
				yx1 = yw1 + x1;
				r[yx1] = img[yx3]   - shift;
				g[yx1] = img[yx3+1] - shift;
				b[yx1] = img[yx3+2] - shift;
			}
		}
	}
}

/** \brief Copy three images R, G, B to 24 bits rgb format.
	\param buff 	The output RGB24 image.
    \param r		The input red image.
    \param g		The input green image.
    \param b		The input blue image.
    \param w		The image width.
    \param h		The image height.
    \param bpp		The bits per pixel.
*/
void utils_RGB_to_RGB24(uint8 *img, int16 *r, int16 *g, int16 *b, uint32 w, uint32 h, uint32 bpp)
{
	uint32 i, i3, size = w*h, shift = 1<<(bpp-1), sh = bpp - 8;
	for(i=0; i<size; i++) {
		i3 = i*3;
		img[i3]   = lb1((r[i] + shift)>>sh);
		img[i3+1] = lb1((g[i] + shift)>>sh);
		img[i3+2] = lb1((b[i] + shift)>>sh);
	}
}

uint8* utils_RGB_to_RGB24_8(uint8 *img, uint8 *r, uint8 *g, uint8 *b, uint32 w, uint32 h, uint32 bpp)
{
    uint32 i, i3, size = w*h;
    for(i=0; i<size; i++) {
        i3 = i*3;
        img[i3]   = r[i];
        img[i3+1] = g[i];
        img[i3+2] = b[i];
    }
    return img;
}


/** \brief Convert YUV444 image to RGB.
	\param rgb 	The output RGB image.
    \param Y	The output Y image.
	\param U	The output U image.
	\param V	The output V image.
    \param w	The image width.
    \param h	The image height.
    \param bpp	The bits per pixel.
    \retval	rgb	The output RGB image.
*/
uint8* utils_YUV444_to_RGB24(uint8 *rgb, int16 *Y, int16 *U, int16 *V, uint32 w, uint32 h, uint32 bpp)
{
	/*
	R = Y + 1.4026 * V
	G = Y – 0.3444 * U – 0.7144 * V
	B = Y + 1.7730 * U
	*/
	int i, i3, sz = w*h, shift = 1<<(bpp-1);
	//int R, G, B;
	for(i=0; i < sz; i++){
		i3 = i*3;
		rgb[i3    ] = lb1(shift + Y[i] + ((1436*V[i])>>10));
		rgb[i3 + 1] = lb1(shift + Y[i] - ((732*V[i] + 353*U[i])>>10));
		rgb[i3 + 2] = lb1(shift + Y[i] + ((1816*U[i])>>10));

		//if(R < 0 || R > 255) printf("R=%d ",R);
		//if(G < 0 || G > 255) printf("G=%d y = %d u = %d v = %d ",G, y[i], u[i], v[i]);
		//if(B < 0 || B > 255) printf("v=%d ",B);
	}
	return rgb;
}

/** \brief Convert YUV420 image to RGB.
	\param rgb 	The output RGB image.
    \param y	The output Y image.
	\param u	The output U image.
	\param v	The output V image.
    \param w	The image width.
    \param h	The image height.
    \retval	rgb	The output RGB image.
*/
uint8* utils_YUV420_to_RGB24(uint8 *rgb, int16 *Y, int16 *U, int16 *V, uint32 w, uint32 h, uint32 bpp)
{
	/*
	R = Y + 1.4026 * V
	G = Y – 0.3444 * U – 0.7144 * V
	B = Y + 1.7730 * U
	*/
	int y, x, yw, yx, yx2, yx3, w2 = w>>1, shift = 1<<(bpp-1);;
	for(y=0; y < h; y++){
		yw = y*w;
		for(x=0; x < w; x++){
			yx = yw + x;
			yx3 = yx*3;
			yx2 = (x>>1) + (y>>1)*w2;

			rgb[yx3    ] = lb1(shift + Y[yx] + ((1436*V[yx2])>>10));
			rgb[yx3 + 1] = lb1(shift + Y[yx] - ((732*V[yx2] + 353*U[yx2])>>10));
			rgb[yx3 + 2] = lb1(shift + Y[yx] + ((1816*U[yx2])>>10));
		}
	}
	return rgb;
}

/** \brief Make new image with specular borders.
	\param img 	The input image.
    \param img1	The output image.
    \param w	The image width.
    \param h	The image height.
    \param bor	The border size.
    \retval	img1	The output image.
*/
int16* utils_specular_border(int16 *img, int16 *img1, uint32 w, uint32 h, uint32 bor)
{
	int y, x, yw, ywb, w1 = w + (bor<<1), h1 = h + (bor<<1);
	for(y=0; y < h; y++){
		yw = y*w;
		ywb = (y+bor)*w1 + bor;
		for(x=0; x < w; x++) img1[ywb + x] = img[yw + x];
		for(x=0; x < bor; x++) img1[ywb - bor + x] = img1[ywb + bor - x];
		for(x=w+bor; x < w1; x++) img1[ywb - bor + x] = img1[ywb + bor + (w<<1) - x - 2 ];
	}
	for(y=0; y < bor; y++){
		ywb = y*w1;
		yw = ((bor<<1)-y)*w1;
		for(x=0; x < w1; x++) img1[ywb + x] = img1[yw + x];
	}
	for(y=h+bor; y < h1; y++){
		ywb = y*w1;
		yw = (((h+bor)<<1) - y - 2 )*w1;
		for(x=0; x < w1; x++) img1[ywb + x] = img1[yw + x];
	}

	return img1;
}

/** \brief Copy image from the buffer
	\param buff 	The input buffer.
    \param img		The output image.
    \param w		The image width.
    \param h		The image height.
    \param bpp		The bits per pixel.
*/
void utils_image_copy(uint8 *buff, int16 *img, uint32 w, uint32 h, uint32 bpp)
{
	uint32 i, size = w*h, shift = 1<<(bpp-1);
	//printf("Start copy  x = %d y = %d p = %p \n", im->w, im->h, im->p);
	//printf("utils_image_copy : bpp = %d shift = %d\n", bpp, shift);

	if(bpp > 8) for(i=0; i<size; i++) {
        //For Aptina sensor
        img[i] = ((buff[(i<<1)]) | buff[(i<<1)+1]<<8) - shift;
        //For Sony sensor
        //img[i] = ((buff[(i<<1)]<<8) | buff[(i<<1)+1]) - shift;
		//printf("MSB = %d LSB = %d img = %d shift = %d\n", buff[(i<<1)], buff[(i<<1)+1], ((buff[(i<<1)]) | buff[(i<<1)+1]<<8), shift);
	}
	else 		for(i=0; i<size; i++) img[i] = buff[i] - shift;
}

uint8* utils_draw_scale_color(uint8 *rgb, uint8 *img,  uint32 w0, uint32 h0, uint32 w, uint32 h,   uint32 wp, BayerGrid bay){
/*! \fn void bayer_to_rgb(uint8 *rgb)
	\brief DWT picture transform.
  	\param	rgb 	The pointer to rgb array.
*/

/*
   All RGB cameras use one of these Bayer grids:

	BGGR  0         GRBG 1          GBRG  2         RGGB 3
	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5
	0 B G B G B G	0 G R G R G R	0 G B G B G B	0 R G R G R G
	1 G R G R G R	1 B G B G B G	1 R G R G R G	1 G B G B G B
	2 B G B G B G	2 G R G R G R	2 G B G B G B	2 R G R G R G
	3 G R G R G R	3 B G B G B G	3 R G R G R G	3 G B G B G B
 */
	uint32 y, x, yx, yw, yx3, i, sq = w*h - w, w1 = w-1, h1 = h-1;
	switch(bay){
		case RGGB :
			for(y=1; y < h1; y++){
				for(x=1; x < w1; x++){
					yx = y*w+x;
					yx3 = ((y+h0)*wp + x + w0)*3;
					rgb[yx3  ] = (!(x&1) && !(y&1)) ? img[yx] : ((x&1 && y&1) ? (img[yx-1-w] + img[yx+1-w] + img[yx-1+w] + img[yx+1+w])>>2 :
					(x&1 && !(y&1) ? (img[yx-1] + img[yx+1])>>1 : (img[yx-w] + img[yx+w])>>1));
					rgb[yx3+1] = ((!(x&1) && y&1) || ( x&1 && !(y&1))) ? img[yx] : (img[yx-1] + img[yx-w] + img[yx+1] + img[yx+w])>>2;
					rgb[yx3+2] = (x&1 && y&1) ? img[yx] : ((!(x&1) && !(y&1)) ? (img[yx-1-w] + img[yx+1-w] + img[yx-1+w] + img[yx+1+w])>>2 :
					(!(x&1) && y&1 ? (img[yx-1] + img[yx+1])>>1 : (img[yx-w] + img[yx+w])>>1));
				}
			}
	}
	return rgb;
}

void fill_bayer_hist(int16 *img, uint32 *r, uint32 *g, uint32 *b, uint32 w, uint32 h,  BayerGrid bay, uint32 bits){
//
//   All RGB cameras use one of these Bayer grids:
//
//	BGGR  0         GRBG 1          GBRG  2         RGGB 3
//	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5
//	0 B G B G B G	0 G R G R G R	0 G B G B G B	0 R G R G R G
//	1 G R G R G R	1 B G B G B G	1 R G R G R G	1 G B G B G B
//	2 B G B G B G	2 G R G R G R	2 G B G B G B	2 R G R G R G
//	3 G R G R G R	3 B G B G B G	3 R G R G R G	3 G B G B G B
//
	uint32 x, y, i, size = h*w, shift = 1<<(bits-1);
	uint32 *c[4];
	memset(r, 0, sizeof(uint32)*(1<<bits));
	memset(g, 0, sizeof(uint32)*(1<<bits));
	memset(b, 0, sizeof(uint32)*(1<<bits));

	switch(bay){
		case(BGGR):{ c[0] = r; c[1] = g; c[2] = g; c[3] = b; break;}
		case(GRBG):{ c[0] = g; c[1] = b; c[2] = r; c[3] = g; break;}
		case(GBRG):{ c[0] = g; c[1] = r; c[2] = b; c[3] = g; break;}
		case(RGGB):{ c[0] = b; c[1] = g; c[2] = g; c[3] = r; break;}
	}

	for(i=0, x=0, y=0; i < size; i++, x++){
		if(x == w) { x=0; y++;}
		if(y&1)
			if(x&1) c[0][img[i]+shift]++;
			else 	c[1][img[i]+shift]++;
		else
			if(x&1)	c[2][img[i]+shift]++;
			else 	c[3][img[i]+shift]++;
	}
}

void fill_hist(int16 *img, uint32 *h, uint32 size, uint32 bits)
{
    uint32 i, shift = 1<<(bits-1);
    memset(h, 0, sizeof(uint32)*(1<<bits));

    for(i=0; i < size; i++) h[img[i]+shift]++;

    for(i=0; i < (1<<bits); i++) printf("%d  %d\n", i, h[i]);
}

void make_hist(int16 *img, uint32 *h, uint32 size, uint32 ibit, int *low, int *top)
{
    uint32 i, hz = 1<<ibit, sum, shift = 1<<(ibit-1);
    uint32 th = size/256;
    memset(h, 0, sizeof(uint32)*(1<<ibit));

    for(i=0; i < size; i++) h[img[i]+shift]++;
    //for(i=0; i < hz; i++) printf("look[%d] = %d\n", i, h[i]);

    sum = 0;
    for(i=0; sum < th; i++) sum += h[i];
    *low = i;
    sum = 0;
    for(i=hz-1; sum < th ; i--) sum += h[i];
    *top = i;

}

void make_lookup1(int16 *img, uint32 *hist, uint32 *look, uint32 w, uint32 h, uint32 ibit, uint32 hbit)
///	\fn make_lookup1(int16 *img, uint32 *hist, uint32 *look, uint32 w, uint32 h, uint32 ibit, uint32 hbit)
///	\brief Make intergal LUT.
///	\param img	 		The input image.
///	\param hist 		The histogram.
///	\param look 		The LUT.
/// \param w            The image width.
/// \param h            The image height.
/// \param ibit         The image bits per pixel.
/// \param hbit         The histogram bits.
{
    uint32 i, df = ibit-hbit, hz = 1<<hbit, sum, low, top, shift = 1<<(ibit-1);
    double lowt = 0.01, topt = 0.01, a;
    uint32 size = w*h, b, max;

    memset(hist, 0, sizeof(uint32)*(1<<hbit));
    memset(look, 0, sizeof(uint32)*(1<<hbit));

    for(i=0; i < size; i++) hist[(img[i]+shift)>>df]++;

    //Make LUT table liniar
    /*
    sum = 0;
    for(i=0; (double)sum/(double)size < lowt ; i++) sum += h[i];
    low = i;
    sum = 0;
    for(i=hz-1; (double)sum/(double)size < topt ; i--) sum += h[i];
    top = i;

    a = 255./(double)(top - low);
    printf("low = %d top = %d a = %f\n", low, top, a);

    for(i = 0; i < low; i++) look[i] = 0;
    for(i = low; i < top; i++) look[i] = (uint32)(a*(double)(i-low));
    for(i = top; i < hz; i++) look[i] = 255;
    */

    //Make LUT table integral
    b = (1<<30)/size;

    //Check if one bin in historgamm more then one bin in LUT------------------------------------

    sum = 0;
    max = size>>8;

    for(i = 0; i < hz; i++) {
        if(hist[i] > max) {
            sum += hist[i] - max;
            hist[i] = max;
        }
    }
    b = (1<<30)/(size - sum);

    //---------------------------------------------------------------------------------------------
    sum = 0;
    //With double
    //for(i = 0; i < hz; i++) { sum += hist[i]; look[i] = (uint32)(255.*(double)sum/(double)size); }
    //With int
    for(i = 0; i < hz; i++) { sum += hist[i]; look[i] = sum*b>>22; }

    for(i = 0; i < hz; i++) printf("%d hist = %d look = %d\n", i, hist[i], look[i]);
}

void util_make_hdr(int16 *in, int16 *inm, int16 *im8, int16 *out, int16 *df, uint32 w, uint32 h)
{
    int x, y, yw, yx, th = 200;
    for(y=0; y < h; y++){
        yw = y*w;
        for(x=0; x < w; x++){
            yx = yw + x;
            //out[yx] = im8[yx];
            //if(df[yx] < 100) out[yx] = im8[yx] + (in[yx] - inm[yx]);
            //else out[yx] = im8[yx];
            out[yx] = im8[yx] + (in[yx] - inm[yx])*th/(df[yx]+1);
            //out[yx] = df[yx] > th ? im8[yx] : 0;
        }
    }
}

void make_lookup2(int16 *img, uint32 *hist, int16 *look, uint32 w, uint32 h)
{
    int i, j, d = 1, div = 1<<d, div2 = div<<1, shift = 1<<11;
    int size = w*h, size1 = (w>>d)*(h>>d); //size1 = size>>d;
    int b = (1<<30)/size1, min = size1>>5;

    int x, y, yw, yx, yx1, w2 = w<<1;
    int temp_data, sum = 0, sum1, gamma_corr_old = 0, gamma_corr = 0;


    memset(hist, 0, sizeof(uint32)*(4096));
    memset(look, 0, sizeof(int16)*(1024));
    //Если прокатит это преобразование
    //short int *test_addr1 = (short int *)test_addr;

    for(i=0; i < size; i++) hist[(img[i]+shift)]++;

    /*
    for (y=0; y < h; y+=div){
        yw = y*w;
        for (x=0; x < w; x+=div){
            yx = yw + x;
            temp_data = img[yx]+shift;
            yx1 = yx + 1;
            temp_data += img[yx1]+shift;
            yx1 = yx1 + w;
            temp_data += img[yx1]+shift;
            yx1 = yx1 - 1;
            temp_data += img[yx1]+shift;
            //printf("%d ", temp_data>>2);
            //hist[temp_data>>5]++;
            hist[temp_data>>2]++;
        }
    }
    */
    /*
    for (y=0; y < h; y+=div){
        yw = y*w;
        for (x=0; x < w; x+=div){
            yx = (yw + x)<<1;
            temp_data = test_addr[yx] | (test_addr[yx+1] << 8);
            yx1 = yx + 2;
            temp_data += test_addr[yx1] | (test_addr[yx1+1] << 8);
            yx1 = yx1 + w2;
            temp_data += test_addr[yx1] | (test_addr[yx1+1] << 8);
            yx1 = yx1 - 2;
            temp_data += test_addr[yx1] | (test_addr[yx1+1] << 8);
            hist[temp_data>>5]++;
        }
    }

    for (i=0; i < size2; i += div2)
    {
        temp_data = test_addr[i] | (test_addr[i+1] << 8);
        hist[temp_data>>3]++;
    }
    */
    /*
    for(i = 0; i < 512; i++){
        sum += hist[i];
        //gamma_corr = (sum*1023*div)/size;
        if(sum > min){
            //sum += hist[i];

            gamma_corr+=32;

            look[i<<1] = gamma_corr;
            look[(i<<1)+1] = 32;
            //gamma_corr = gamma_corr_old + 32;
            sum = 0;

        }
        //gamma_corr_old = gamma_corr;
    }*/

    for(i = 0; i < 512; i++){
        //sum1 = 0;
        //for(j=0; j < 8; j++){
        //    sum1 += hist[(i<<3)+j];
        //}
        if(hist[i] > min){

        } else {
            sum += hist[i];
        }

        //gamma_corr = (sum*1023*div)/size;
        gamma_corr = sum*b>>20;

        //if ((gamma_corr - gamma_corr_old) > 511)
        //    gamma_corr = gamma_corr_old + 511;

        //b = (gamma_corr_old << 10) | (gamma_corr - gamma_corr_old);

        look[(i>>3)<<1] = gamma_corr_old;
        look[((i>>3)<<1)+1] = gamma_corr - gamma_corr_old;

        //CammaG[i] = b;
        //CammaB[i] = b;

        gamma_corr_old = gamma_corr;
    }

    for(i = 0; i < 4096; i++) printf("%4d hist = %5d %4d %4d %4d\n", i, hist[i], (i>>3)<<1, look[(i>>3)<<1]<<2, look[((i>>3)<<1)+1]);
    //for(i = 0; i < 512; i++) printf("%4d hist = %5d   %4d %4d\n", i, hist[i], look[i<<1], look[(i<<1)+1]);
}


/**	\brief Image transform.
    \param img	 		The input image.
    \param img1	 		The output image.
    \param look 		The LUT.
    \param w            The image width.
    \param h            The image height.
    \param ibit         The image bits per pixel.
    \param hbit         The histogram bits.
*/
void bits12to8(int16 *img, int16 *img1, uint32 *look, uint32 w, uint32 h, uint32 ibit, uint32 hbit)
{
    uint32 i, df = ibit-hbit, shift = 1<<(ibit-1), size = w*h;
    for(i=0; i < size; i++) img1[i] = look[(img[i]+shift)>>df]-128;
}

void bits12to8_1(int16 *img, int16 *img1, int16 *look, uint32 w, uint32 h)
///	\fn bits12to8(int16 *img, int16 *img1, uint32 *look, uint32 w, uint32 h, uint32 ibit, uint32 hbit)
///	\brief Image transform.
///	\param img	 		The input image.
///	\param img1	 		The output image.
///	\param look 		The LUT.
/// \param w            The image width.
/// \param h            The image height.
/// \param ibit         The image bits per pixel.
/// \param hbit         The histogram bits.
{
    int i, size = w*h, top, bot, in, shift = 1<<11;

    for(i=0; i < size; i++) {
        in = (img[i]+shift)>>3;
        top = look[in<<1];
        bot = (look[in+1]*(((img[i]+shift)&7)<<3))>>6;
        img1[i] = ((top + bot)>>2)-128;
        //img1[i] =  (top>>1) - 128;
        //printf("%d %d  ", in, top);
    }
    //for(i = 0; i < 512; i++) printf("%d  %d %d\n", i, look[i<<1], look[(i<<1)+1]);
}

uint8* utils_color_draw(uint8 *img, uint8 *rgb, uint32 w, uint32 h, uint32 col)
{
    int i, j, dim = h*w*3;
    if(col ==  0) for(i=0, j=0; j < dim; j+=3, i++) rgb[j  ] = img[i]; // Red
    if(col ==  1) for(i=0, j=0; j < dim; j+=3, i++) rgb[j+1] = img[i]; // Red
    if(col ==  2) for(i=0, j=0; j < dim; j+=3, i++) rgb[j+2] = img[i]; // Red
    return rgb;
}

uint8* utils_draw(uint8 *img, uint8 *rgb, uint32 w, uint32 h)
{
	int j, dim = h*w*3;
	for(j= 0; j < dim; j+=3){
		rgb[j]     = img[j];
		rgb[j+1] = img[j+1];
		rgb[j+2] = img[j+2];
		//printf("y_w[%d] = %4d\n",i,mod(yuv_buffer->y_w[i]));
	}
	return rgb;
}

uint8* utils_reg_draw(uint32 *img, uint8 *rgb, uint32 w, uint32 h)
{
	int i, j, dim = h*w*3;
	for(i = 0,  j= 0; j < dim; j+=3, i++){
		rgb[j]     = img[i]&0xFF;
		rgb[j + 1] = img[i]&0xFF;
		rgb[j + 2] = img[i]&0xFF;
		//printf("y_w[%d] = %4d\n",i,mod(yuv_buffer->y_w[i]));
	}
	return rgb;
}

uint8* utils_cat(uint8 *img, uint8 *img1, uint32 w, uint32 h, uint32 bits)
{
	int i, dim = h*w, sh = bits-8;
    for(i = 0; i < dim; i++) img1[i] = img[i];
	return img1;
}

uint8* utils_bayer_to_Y1(uint8 *img, uint8 *img1, uint32 w, uint32 h)
{
	uint32 x, y, wy, xwy, y2, x2, a, b, h1 = h-1, w1 = w-1, yw, yw1;

	for(y=0, yw=0, yw1=0 ; y < h1; y++, yw+=w, yw1+=w1){
		for(x=0; x < w1; x++){
			xwy = x + yw;
			wy = x + yw1;
			//xwy3 = wy + wy + wy;
			img1[wy] = 	(img[xwy ] + img[xwy+1] + img[xwy+w] + img[xwy+w+1])>>2;
		}
	}
	return img1;
}

uint8* utils_ppm_to_bayer(uint8 *img, uint8 *img1, uint32 w, uint32 h)
{
	uint32 x, y, yx, h1 = w*h, w1 = w<<1;

	for(y=0; y < h1; y+=w1) {
		for(x=0; x < w; x+=2){
			yx = y+x;
			//img1[yx] = 255;
			//img1[yx+1] = 255;
			//img1[yx+w] = 255;
			//img1[yx+w+1] = 255;
			img1[yx] = img[yx*3];
			img1[yx+1] = img[(yx+1)*3+1];
			img1[yx+w] = img[(yx+w)*3+1];
			img1[yx+w+1] = img[(yx+w+1)*3+2];
		}
	}
	return img1;
}

void utils_bayer_to_4color(uint8 *img, uint32 w, uint32 h, uint8 *p0, uint8 *p1, uint8 *p2, uint8 *p3)
{
	uint32 i = 0, x, y, yx, h1 = w*((h>>1)<<1), w1 = w<<1;
	//uint8 *p[4];
	//p[0] = img1; p[1] = &img1[h1>>2]; p[2] = &img1[h1>>1]; p[3] = &img1[(h1>>2)*3];

	for(y=0; y < h1; y+=w1) {
		for(x=0; x < w; x+=2){
			yx = y+x;
			//img1[yx] = 255;
			//img1[yx+1] = 255;
			//img1[yx+w] = 255;
			//img1[yx+w+1] = 255;
			p0[i] = img[yx];
			p1[i] = img[yx+1];
			p2[i] = img[yx+w];
			p3[i] = img[yx+w+1];
			i++;
		}
	}
}

uint8* utils_4color_draw(uint8 *img, uint8 *rgb, uint32 w, uint32 h, uint8 *p0, uint8 *p1, uint8 *p2, uint8 *p3)
{
	uint32 i=0, j=0, x, y, yx, h1 = w*((h>>1)<<1), w1 = w<<1;
	//uint8 *p[4];
	//p[0] = img; p[1] = &img[h1>>2]; p[2] = &img[h1>>1]; p[3] = &img[(h1>>2)*3];


	for(y=0; y < (h1>>1); y+=w) {
		for(x=0; x < (w>>1); x++){
			yx = y+x;
			rgb[yx*3]     = p0[i];
			rgb[yx*3 + 1] = p0[i];
			rgb[yx*3 + 2] = p0[i]; i++;
		}
		for(x=w>>1; x < w; x++){
			yx = y+x;
			rgb[yx*3]     = p1[j];
			rgb[yx*3 + 1] = p1[j];
			rgb[yx*3 + 2] = p1[j]; j++;
		}
	}
	i=0, j=0;
	for(y=(h1>>1); y < h1; y+=w) {
		for(x=0; x < (w>>1); x++){
			yx = y+x;
			rgb[yx*3]     = p2[i];
			rgb[yx*3 + 1] = p2[i];
			rgb[yx*3 + 2] = p2[i]; i++;
		}
		for(x=w>>1; x < w; x++){
			yx = y+x;
			rgb[yx*3]     = p3[j];
			rgb[yx*3 + 1] = p3[j];
			rgb[yx*3 + 2] = p3[j]; j++;
		}
	}
	return rgb;
}
/*
uint8* utils_4color_scale_draw(uint8 *rgb, uint32 w, uint32 h, Picture *p0,  Picture *p1,  Picture *p2,  Picture *p3)
{
	uint32 i=0, sx, sy;
	drawrect(rgb, p0[0].pic, 0			, 0 							, p0[0].width, p0[0].height, w, 0);
	drawrect(rgb, p0[1].pic, p0[0].width, 0 							, p0[1].width, p0[1].height, w, 0);
	drawrect(rgb, p0[2].pic, p0[0].width, p0[1].height				 	, p0[2].width, p0[2].height, w, 0);
	drawrect(rgb, p0[3].pic, p0[0].width, p0[1].height+p0[2].height 	, p0[3].width, p0[3].height, w, 0);

	sx = p0[0].width + p0[1].width;
	drawrect(rgb, p1[0].pic, sx			, 0 							, p1[0].width, p1[0].height, w, 0);
	drawrect(rgb, p1[1].pic, p1[0].width+sx, 0 							, p1[1].width, p1[1].height, w, 0);
	drawrect(rgb, p1[2].pic, p1[0].width+sx, p1[1].height				, p1[2].width, p1[2].height, w, 0);
	drawrect(rgb, p1[3].pic, p1[0].width+sx, p1[1].height+p1[2].height 	, p1[3].width, p1[3].height, w, 0);

	sy = p0[0].height;
	drawrect(rgb, p1[0].pic, 0			, sy 							, p1[0].width, p1[0].height, w, 0);
	drawrect(rgb, p1[1].pic, p1[0].width, sy 							, p1[1].width, p1[1].height, w, 0);
	drawrect(rgb, p1[2].pic, p1[0].width, p1[1].height+sy				, p1[2].width, p1[2].height, w, 0);
	drawrect(rgb, p1[3].pic, p1[0].width, p1[1].height+p1[2].height+sy 	, p1[3].width, p1[3].height, w, 0);

	drawrect(rgb, p1[0].pic, sx			, sy 							, p1[0].width, p1[0].height, w, 0);
	drawrect(rgb, p1[1].pic, p1[0].width+sx, sy 							, p1[1].width, p1[1].height, w, 0);
	drawrect(rgb, p1[2].pic, p1[0].width+sx, p1[1].height+sy				, p1[2].width, p1[2].height, w, 0);
	drawrect(rgb, p1[3].pic, p1[0].width+sx, p1[1].height+p1[2].height+sy 	, p1[3].width, p1[3].height, w, 0);

	return rgb;
}

uint8* utils_scale_draw(uint8 *rgb, uint32 w, uint32 h, Picture *p)
{
	uint32 i=0, sx, sy;
	drawrect(rgb, p[0].pic, 0		  , 0 							, p[0].width, p[0].height, w, 0);
	drawrect(rgb, p[1].pic, p[0].width, 0 							, p[1].width, p[1].height, w, 0);
	drawrect(rgb, p[2].pic, p[0].width, p[1].height				 	, p[2].width, p[2].height, w, 0);
	drawrect(rgb, p[3].pic, p[0].width, p[1].height+p[2].height 	, p[3].width, p[3].height, w, 0);

	return rgb;
}
*/
uint8* utils_rgb_scale_draw(uint8 *rgb, uint32 w, uint32 h, Pic8u *p)
{

	drawrect_rgb(rgb, p[0].pic, 0		  , 0 						, p[0].w, p[0].h, w);
	drawrect_rgb(rgb, p[1].pic, p[0].w, 0 						, p[1].w, p[1].h, w);
	drawrect_rgb(rgb, p[2].pic, p[0].w, p[1].h				, p[2].w, p[2].h, w);
	drawrect_rgb(rgb, p[3].pic, p[0].w, p[1].h+p[2].h , p[3].w, p[3].h, w);

	return rgb;
}

uint8* utils_color_scale_draw(uint8 *rgb, uint32 w, uint32 h, Pic8u *p)
{

	utils_draw_scale_color(rgb, p[0].pic, 0		  , 0 							, p[0].w, p[0].h, w, 3);
	utils_draw_scale_color(rgb, p[1].pic, p[0].w, 0 						, p[1].w, p[1].h, w, 3);
	utils_draw_scale_color(rgb, p[2].pic, p[0].w, p[1].h				, p[2].w, p[2].h, w, 3);
	utils_draw_scale_color(rgb, p[3].pic, p[0].w, p[1].h+p[2].h	, p[3].w, p[3].h, w, 3);

	return rgb;
}


void utils_resize_2x(uint8 *img, uint8 *img1, uint32 w, uint32 h)
{
	uint32 x, y, yx, h1 = ((h>>1)<<1)*w, w2 = w<<1, w1 = ((w>>1)<<1), i=0;
	for(y=0; y < h1; y+=w2){
		for(x=0; x < w1; x+=2){
			yx = y + x;
			img1[i++] = (img[yx] + img[yx+1] + img[yx+w] + img[yx+w+1])>>2;
		}
	}
}

/** \brief Resize bayer image down to two times on each axis.
    \param img 	The input image.
    \param img1	The output image.
    \param w	The image width.
    \param h	The image height.
*/
void utils_resize_bayer_2x(int16 *img, int16 *img1, uint32 w, uint32 h)
{
	uint32 x, y, yx, yx1, yx2, h1 = ((h>>2)<<2)*w, w2 = w<<2, w1 = ((w>>2)<<2), wn = w>>1, i=0;
	for(y=0; y < h1; y+=w2){
		for(x=0; x < w1; x+=4){
			yx = y + x;
			yx1 = (y>>2) + (x>>1);
			yx2 = yx+w2;
			img1[yx1] 		= (img[yx] 		+ img[yx+2] 	+ img[yx+w2] 		+ img[yx+w2+2])>>2;
			img1[yx1+1] 	= (img[yx+1] 	+ img[yx+3] 	+ img[yx+w2+1] 		+ img[yx+w2+3])>>2;
			img1[yx1+wn] 	= (img[yx+w] 	+ img[yx+2+w] 	+ img[yx+w2+w] 		+ img[yx+w2+2+w])>>2;
			img1[yx1+wn+1]	= (img[yx+w+1] 	+ img[yx+3+w]	+ img[yx+w2+w+1]	+ img[yx+w2+3+w])>>2;
			//if(yx1 >= wn*(h>>1)) printf(" ind = %d\n", yx1);
		}
		if((w>>1)&1){
			yx = y + x;
			yx1 = (y>>2) + (x>>1);
			img1[yx1] 		= (img[yx] 		+ img[yx+w2] 	)>>1;
			img1[yx1+1] 	= (img[yx+1] 	+ img[yx+w2+1] 	)>>1;
			img1[yx1+wn] 	= (img[yx+w] 	+ img[yx+w2+w] 	)>>1;
			img1[yx1+wn+1]	= (img[yx+w+1] 	+ img[yx+w2+w+1])>>1;
		}
	}
	if((h>>1)&1){
		for(x=0; x < w1; x+=4){
			yx = y + x;
			yx1 = (y>>2) + (x>>1);
			img1[yx1] 		= (img[yx] 		+ img[yx+2]  )>>1;
			img1[yx1+1] 	= (img[yx+1] 	+ img[yx+3]  )>>1;
			img1[yx1+wn] 	= (img[yx+w] 	+ img[yx+2+w])>>1;
			img1[yx1+wn+1]	= (img[yx+w+1] 	+ img[yx+3+w])>>1;
			//if(yx1 >= wn*(h>>1)) printf(" ind = %d\n", yx1);
		}
		if((w>>1)&1){
			yx = y + x;
			yx1 = (y>>2) + (x>>1);
			img1[yx1] 		= img[yx]    ;
			img1[yx1+1] 	= img[yx+1]  ;
			img1[yx1+wn] 	= img[yx+w]  ;
			img1[yx1+wn+1]	= img[yx+w+1];
		}
	}
}

void utils_resize_bayer_2x_new(int16 *img, int16 *img1, uint32 w, uint32 h)
{
	uint32 x, y, yx, yx1, yx2, h1 = ((h>>2)<<2)*w, w2 = w<<2, w1 = ((w>>2)<<2), wn = w>>1, i=0;
	for(y=0; y < h1; y+=w2){
		for(x=0; x < w1; x+=4){
			yx = y + x;
			yx1 = (y>>2) + (x>>1);
			img1[yx1] 		= ((img[yx]<<1) + img[yx+1+w] + ((img[yx+2] + img[yx+(w<<1)])>>1))>>2;
			yx2 = yx+3;
			img1[yx1+1] 	= ((img[yx2]<<1) + ((img[yx2-2] + img[yx2+(w<<1)])>>1) + (img[yx2] + img[yx2-2] + img[yx2+(w<<1)] + img[yx2+(w<<1)-2])>>2)>>2;
			yx2 = yx+3*w;
			img1[yx1+wn] 	= ((img[yx2]<<1) + ((img[yx2+2] + img[yx2+(w<<1)])>>1) + (img[yx2] + img[yx2+2] + img[yx2+(w<<1)] + img[yx2+(w<<1)+2])>>2)>>2;
			yx2 = yx+2+(w<<1);
			img1[yx1+wn+1]	= ((img[yx2]<<1) + img[yx2+1+w] + ((img[yx2+2] + img[yx2+(w<<1)])>>1))>>2;
			//if(yx1 >= wn*(h>>1)) printf(" ind = %d\n", yx1);
		}
	}
}

void utils_resize_rgb_2x(uint8 *img, uint8 *img1, uint32 w, uint32 h)
{
	uint32 x, y, yx, h1 = ((h>>1)<<1)*w*3, w2 = (w<<1)*3, w1 = ((w>>1)<<1)*3, i=0;
	for(y=0; y < h1; y+=w2){
		for(x=0; x < w1; x+=6){
			yx = y + x;
			img1[i++] = img[yx];
			img1[i++] =	img[yx+1];
			img1[i++] =	img[yx+2];
		}
	}
}

void utils_bayer_to_rgb(uint8 *img, uint8 *rgb, uint32 w, uint32 h)
{
	uint32 x, y, yx, yx3, h1 = ((h>>1)<<1)*w, w2 = w<<1, w1 = ((w>>1)<<1), wn = w>>1, i=0;
	for(y=0; y < h1; y+=w2){
		for(x=0; x < w1; x+=2){
			yx = y + x;
			yx3 = ((y>>2) + (x>>1))*3;
			rgb[yx3] 	= img[yx];
			rgb[yx3+1] 	= (img[yx+1] 	+ img[yx+w])>>1;
			rgb[yx3+2] 	= img[yx+w+1];
		}
	}
}

void utils_bayer_to_Y(uint8 *img, uint8 *img1, uint32 w, uint32 h)
{
	uint32 x, x1, y, y1, yx, yx1, h1 = ((h>>1)<<1)*w, w2 = w<<1, w1 = ((w>>1)<<1), w3 = w>>1;
	for(y=0, y1=0; y < h1; y+=w2, y1+=w3){
		for(x=0, x1=0; x < w1; x+=2, x1++){
			yx = y + x;
			yx1 = y1 + x1;
			img1[yx1] 	= (img[yx] + img[yx+1] + img[yx+w] + img[yx+w+1])>>2;
		}
	}
}

uint8* utils_bayer_to_gradient(uint8 *img, uint8 *img1, uint32 w, uint32 h, BayerGrid bay, uint32 thresh)
{
	uint32 x, y, wy, xwy, y2, x2, a, b, h1 = h-1, w1 = w-1, yw, yw1;

    switch(bay){
            case(BGGR) : { a = 1; b = 1; break; }
            case(GRBG) : { a = 0; b = 1; break; }
            case(GBRG) : { a = 1; b = 0; break; }
            case(RGGB) : { a = 0; b = 0; break; }
    }

//	for(y=0, yw=0, yw1=0 ; y < h1; y++, yw+=w, yw1+=w1){
//		for(x=0; x < w1; x++){
	for(y=1, yw=w, yw1=w1 ; y < h1-1; y++, yw+=w, yw1+=w1){
		for(x=1; x < w1-1; x++){
		//for(y=1, yw=w, yw1=w1 ; y < 2; y++, yw+=w, yw1+=w1){
		//	for(x=1; x < 2; x++){
			y2 = oe(a,y);
			x2 = oe(b,x);
			xwy = x + yw;
			wy = (x + yw1);
			//printf("y2 = %d %d x2 = %d %d\n", y2,!y2, x2, !x2);
			//printf("xwy+w+1 = %d xwy-(w<<1) = %d xwy = %d\n", xwy+w+1, xwy-(w<<1), xwy);

			//Green
            img1[wy] = y2 ? (x2 ? abs(img[xwy+w  ] - img[xwy+1  ]) : abs(img[xwy    ] - img[xwy+w+1])) :
                            (x2 ? abs(img[xwy+w+1] - img[xwy    ]) : abs(img[xwy+1  ] - img[xwy+w  ])) ;
            //Red
			img1[wy] += y2 ? (x2 ? abs(img[xwy  ]   - ((img[xwy]     + img[xwy+2]   + img[xwy+(w<<1)]   + img[xwy+(w<<1)+2])>>2)) :
								   abs(img[xwy+1]   - ((img[xwy+1]   + img[xwy-1]   + img[xwy+(w<<1)+1] + img[xwy+(w<<1)-1])>>2))):
							 (x2 ? abs(img[xwy+w]   - ((img[xwy+w]   + img[xwy+2+w] + img[xwy-w]        + img[xwy-w+2])>>2)) :
								   abs(img[xwy+w+1] - ((img[xwy+w+1] + img[xwy-1+w] + img[xwy-w+1]      + img[xwy-w-1])>>2)));
			//Blue
			img1[wy] += !y2 ? (!x2 ? abs(img[xwy  ]   - ((img[xwy]     + img[xwy+2]   + img[xwy+(w<<1)]   + img[xwy+(w<<1)+2])>>2)) :
								     abs(img[xwy+1]   - ((img[xwy+1]   + img[xwy-1]   + img[xwy+(w<<1)+1] + img[xwy+(w<<1)-1])>>2))):
							  (!x2 ? abs(img[xwy+w]   - ((img[xwy+w]   + img[xwy+2+w] + img[xwy-w]        + img[xwy-w+2])>>2)) :
								     abs(img[xwy+w+1] - ((img[xwy+w+1] + img[xwy-1+w] + img[xwy-w+1]      + img[xwy-w-1])>>2)));
			img1[wy] = img1[wy] > thresh ? img1[wy] : 0;
 		}
	}
	return img1;
}

uint8* utils_bayer_gradient(uint8 *img, uint8 *img1, uint32 w, uint32 h, BayerGrid bay, uint32 thresh)
{
	uint32 x, y, yx, h1 = (h-1)*w, w1 = w-1, yw;

	for(y=w ; y < h1; y+=w){
		for(x=1; x < w1; x++){
			yx = y + x;

			img1[yx] =   abs(img[yx-1] + img[yx-w] + img[yx-w-1] - img[yx+1] - img[yx+w] - img[yx+w+1])>>2;
			img1[yx] +=  abs(img[yx+1] + img[yx-w] + img[yx-w+1] - img[yx-1] - img[yx+w] - img[yx+w-1])>>2;
			//img1[yx] +=  abs(img[yx-1-w] + img[yx-w] + img[yx-w+1] - img[yx-1+w] - img[yx+w] - img[yx+w+1])>>2;
			//img1[yx] +=  abs(img[yx-1-w] + img[yx-1] + img[yx+w-1] - img[yx+1-w] - img[yx+1] - img[yx+w+1])>>2;

			img1[yx] = img1[yx] > thresh ? img1[yx] : 0;
 		}
	}
	return img1;
}

void utils_copy_border(uint8 *img, uint8 *img1, uint32 b, uint32 w, uint32 h)
{
	uint32 x, y, yx, sq = h*w, l = b*w, l1 = sq-l, l2 = w-b;
	//Top border
	for(y=0; y < l; y+=w)
		for(x=0; x < w; x++){
			yx = y + x;
			//Segmentation fault !!!!!!!!!!!!!!!!!!
			img1[yx] = img[yx];
		}
	//Bottom border
	for(y=l1; y < sq; y+=w)
		for(x=0; x < w; x++){
			yx = y + x;
			img1[yx] = img[yx];
		}
	//Left border
	for(y=l; y < l1; y+=w)
		for(x=0; x < b; x++){
			yx = y + x;
			img1[yx] = img[yx];
		}
	//Right border
	for(y=l; y < l1; y+=w)
		for(x=l2; x < w; x++){
			yx = y + x;
			img1[yx] = img[yx];
		}
}

void inline local_max(uint8 *img, uint8 *img1, uint32 w)
{
	uint32 x, w1 = w-1;
	for(x=1; x < w1; x++){
		if(img[x-1])
		img1[x] = (img[x-1] <= img[x] && img[x] >= img[x+1]) ? img[x] : 0;
	}
}

static inline void check_min(uint8 *img, uint32 x, int w , uint32 *min)
{
	uint32 y = x+w;
	if(img[x] > img[y]) if(img[*min] > img[y]) *min = y;
}

uint8* utils_watershed(uint8 *img, uint8 *img1, uint32 w, uint32 h)
{
	uint32 y=0, sq = w*(h-1), x, w1 = w-1, min, yx;
	//img[0] = 255;
	y=0;
	for(y=w; y < sq; y+=w) {
		x = 0;
		for(x=1; x < w1; x++){
			yx = y+x;
			if(img[yx]){
				min = yx;
				check_min(img, yx, -1 , &min);
				check_min(img, yx, -w , &min);
				check_min(img, yx,  1 , &min);
				check_min(img, yx,  w , &min);
				//check_min(img, yx, -1-w , &min);
				//check_min(img, yx,  1-w , &min);
				//check_min(img, yx,  w+1 , &min);
				//check_min(img, yx,  w-1 , &min);
				img1[min] = 0;
				//if(min != yx ) img1[min] = 0;

				//if(min) { img1[min] = 255; img1[yx] = img[yx]; }
				//else if (img[yx] > 0 ) img1[yx] = 255;
			}
		}
	}
	return img1;
}


void utils_min_region(uint8 *img, uint32 *ind, uint32 *arg, uint32 w, uint32 h)
{
	uint32 y=0, sq = w*(h-1), x, w1 = w-1, min, yx, zc = 0, mc = sq>>1 ;

	ind[0] = img[0] ? mc++ : zc++;
	for(x=1; x < w1; x++){
		yx = y+x;
		ind[yx] = img[yx] ? mc++ : (img[yx-1] ? zc++ : ind[yx-1]);
	}
	//printf("ind[%d] = %d\n", yx, ind[yx]);
	for(y=w; y < sq; y+=w) {
		for(x=1; x < w1; x++){
			yx = y+x;
			if(img[yx] == 0) {
				if(img[yx-1] == 0 ) {
					ind[yx] = ind[yx-1];
				} else ind[yx] = zc++;

				if (img[yx-w] == 0 && img[yx-1] == 0 ) {
					//if(img[yx-w-1]) ind[yx] = zc++;
					if(ind[yx] != ind[yx-w]) {
						arg[ind[yx-w]] = ind[yx];
						//if( ind[yx] > ind[yx-w]) arg[ind[yx]] = ind[yx-w];
						//else arg[ind[yx-w]] = ind[yx];
					}
					//ind[yx] = ind[yx-w];
				}
			} else {
				//ind[yx] = mc++;
				ind[yx] = zc++;
			}
		}
	}
}

void utils_steep_descent(uint8 *img, uint32 *ind, uint32 *arg, uint32 w, uint32 h)
{
	uint32 y, sq = w*(h-1), x, w1 = w-1, min, yx, tmp;
	//y=w;{
	for(y=w; y < sq; y+=w) {
		for(x=1; x < w1; x++){
			yx = y+x;
			min = yx;
			if(img[yx] != 0) {
				check_min(img, yx, -1 , &min);
				check_min(img, yx, -w , &min);
				check_min(img, yx,  1 , &min);
				check_min(img, yx,  w , &min);
				arg[ind[yx]] = (min == yx) ? ind[yx] : ind[min];
			}
		}
	}
}

void utils_connect_region(uint8 *img, uint32 *ind, uint32 *arg, uint32 w, uint32 h)
{
	uint32 y, sq = w*(h-1), x, w1 = w-1, min, yx, tmp;
	//y=w;{
	for(y=w; y < sq; y+=w) {
		for(x=1; x < w1; x++){
			yx = y+x;
			//if(img[yx] == 0) {
			for(tmp = arg[ind[yx]]; arg[tmp] != tmp; tmp = arg[tmp]);
				//printf("yx = %d ind[yx] = %d arg[ind] = %d arg[arg[ind]] = %d  \n", yx, ind[yx], arg[ind[yx]], arg[tmp]);
			ind[yx] = tmp;
			//}
		}
	}
}

void utils_print_img(uint8* img, uint32* ind, uint32 w, uint32 h,  uint32 bx, uint32 by,  uint32 lx, uint32 ly)
{
	uint32 x, y;
	for(y=by; y < ly+by; y++) {
		printf("%5d ", y);
		for(x=bx; x < lx+bx; x++){
			printf("%5d ", img[y*w+x]);
		}
		printf("\n");
	//}for(y=by; y < ly+by; y++) {
		printf("%5d ", y);
		for(x=bx; x < lx+bx; x++){
			printf("%5d ", ind[y*w+x]);
		}
		printf("\n");
	}
	printf("\n");
}

void utils_print_ind(uint8* img, uint32 w, uint32 h,  uint32 bx, uint32 by,  uint32 lx, uint32 ly)
{
	uint32 x, y;
	for(y=by; y < ly; y++) {
		for(x=bx; x < lx; x++){
			printf("%3d ", img[y*w+x]);
		}
		printf("\n");
	}
}


double utils_dist(uint8 *before, uint8 *after, uint32 dim, uint32 d){
/// \fn double dist(uint8 *before, uint8 *after, uint32 dim, uint32 d)
/// \brief Calculate distortion of two image.
/// \param before	Pointer to first image.
/// \param after	Pointer to second image.
/// \param dim 		Size of image height*width.
/// \param d 		d = 1 if gray image, d = 3 if color image.
/// \retval 		The distortion.

	uint32 i;
	double ape;
	unsigned long long r = 0, g = 0, b = 0;
	if(d==3){
		for(i = 0; i < dim; i = i + 3){
			//if(before[i]     - after[i]) printf("x = %4d y = %4d r = %4d\n",i/3,i/3,before[i]     - after[i]);
			r += (before[i]     - after[i])*(before[i]     - after[i]);
			g += (before[i+1] - after[i+1])*(before[i+1] - after[i+1]);
			b += (before[i+2] - after[i+2])*(before[i+2] - after[i+2]);
		}
		ape = (double)(r + g + b)/((double)dim*3.);
		return ape;
	}
	if(d==1){
		for(i = 0; i < dim; i++){
			//if(before[i]- after[i]) printf("i = %9d b = %4d a = %4d r = %4d\n",i,before[i], after[i], before[i] - after[i] );
			r += (before[i] - after[i])*(before[i] - after[i]);
			//printf("a = %3d  b = %3d ", before[i], after[i]);
		}
		//printf("r = %d  dim = %d ", r, dim);
		ape = (double)r/(double)dim;
		//printf(" dist = %f\n",ape);
		return ape;
	}
	else{printf("ape: ERROR\n");
		 return 0.;
	}
}

double utils_ape(uint8 *before, uint8 *after, uint32 dim, uint32 d){
/// \fn double ape(uint8 *before, uint8 *after, uint32 dim, uint32 d)
/// \brief Calculate APE of two image.
/// \param before	Pointer to first image.
/// \param after	Pointer to second image.
/// \param dim 		Size of image height*width.
/// \param d 		d = 1 if gray image, d = 3 if color image.
/// \retval 		The APE.

	uint32 i;
	double ape;
	unsigned long long r = 0, g = 0, b = 0;
	if(d==3){
		for(i = 0; i < dim; i = i + 3){
			//if(before[i]     - after[i]) printf("x = %4d y = %4d r = %4d\n",i/3,i/3,before[i]     - after[i]);
			r += abs((int)(before[i]     - after[i]));
			g += abs((int)(before[i+1] - after[i+1]));
			b += abs((int)(before[i+2] - after[i+2]));
		}
		ape = (double)(r + g + b)/((double)dim*3.);
		return ape;
	}
	if(d==1){
		for(i = 0; i < dim; i++){
			//if(before[i] - after[i] ) printf("i = %4d b = %4d a = %4d diff = %4d\n", i, before[i], after[i], before[i] - after[i]);
			r += abs((int)(before[i] - after[i] ));
		}
		ape = (double)r/(double)dim;
		return ape;
	}
	else{printf("ape: ERROR\n");
		 return 0.;
	}
}

double utils_ape_16(int16 *before, int16 *after, uint32 dim, uint32 d){
/// \fn double ape(uint8 *before, uint8 *after, uint32 dim, uint32 d)
/// \brief Calculate APE of two image.
/// \param before	Pointer to first image.
/// \param after	Pointer to second image.
/// \param dim 		Size of image height*width.
/// \param d 		d = 1 if gray image, d = 3 if color image.
/// \retval 		The APE.

	uint32 i;
	double ape;
	unsigned long long r = 0, g = 0, b = 0;
	if(d==3){
		for(i = 0; i < dim; i = i + 3){
			//if(before[i]     - after[i]) printf("x = %4d y = %4d r = %4d\n",i/3,i/3,before[i]     - after[i]);
			r += abs((int)(before[i]     - after[i]));
			g += abs((int)(before[i+1] - after[i+1]));
			b += abs((int)(before[i+2] - after[i+2]));
		}
		ape = (double)(r + g + b)/((double)dim*3.);
		return ape;
	}
	if(d==1){
		for(i = 0; i < dim; i++){
			//if(before[i] - after[i] ) printf("i = %4d b = %4d a = %4d diff = %4d\n", i, before[i], after[i], before[i] - after[i]);
			r += abs(before[i] - after[i]);
		}
		ape = (double)r/(double)dim;
		return ape;
	}
	else{printf("ape: ERROR\n");
		 return 0.;
	}
}

/** \brief Calculate SSIM of two gray image.
	\param im1	Pointer to first image.
	\param im2	Pointer to second image.
	\param size		Size of image height*width.
	\param bbp 		Bits per pixel.
	\param d 		d = 1 if gray image, d = 3 if color image.
	\retval 		The SSIM.
*/
double utils_ssim_16(int16 *im1, int16 *im2, uint32 w, uint32 h, uint32 bbp, int bd, uint32 d){

	int x, y, i, ix, iy, size, yx, xy, wc = bd*2 + 1, wc2 = wc*wc;
	double ssim = 0, ss, c1, c2, *c;
	double av1=0., av2=0., d1=0., d2=0., d12=0., t1, t2;
	double ssimr, ssimg, ssimb, tmp = 0;
	double avr1=0., avg1=0., avb1=0., avr2=0., avg2=0., avb2=0.;
	double dr1=0., dg1=0.,db1=0., dr2=0., dg2=0., db2=0., dr12=0., dg12=0., db12=0.;
	uint8 *img1, *img2;
	img1 = (uint8*)im1; img2 = (uint8*)im2;

	c = (double *)calloc(wc*wc, sizeof(double));

	c1 = (0.01*((1<<bbp)-1))*(0.01*((1<<bbp)-1));
	c2 = (0.03*((1<<bbp)-1))*(0.03*((1<<bbp)-1));

	//Gaussian Blur kernel
	for(y = 0; y < wc; y++) for(x = 0; x < wc; x++) c[y*wc + x] = 1./(double)wc2;

	//SSIM calculation
	for(y = bd; y < h-bd; y+=wc){
		for(x = bd; x < w-bd; x+=wc){
			av1 = 0.; av2 = 0.;
			for(iy=-bd; iy <= bd; iy++) {
				yx = (y + iy)*w;
				for(ix=-bd; ix <= bd; ix++) {
					xy = yx + x + ix;
					//av1 += im1[xy]*c[(iy+bd)*wc + ix+bd];
					//av2 += im2[xy]*c[(iy+bd)*wc + ix+bd];
					av1 += im1[xy];
					av2 += im2[xy];
					//printf("av1 = %f av2 = %f c[%d][%d] = %f\n", av1, av2, ix+bd, iy+bd, c[(iy+bd)*wc + ix+bd]);
				}
			}
			av1 = av1/(double)wc2;
			av2 = av2/(double)wc2;
			d1=0.; d2=0.; d12=0.;
			for(iy=-bd; iy <= bd; iy++) {
				yx = (y + iy)*w;
				for(ix=-bd; ix <= bd; ix++) {
					xy = yx + x + ix;
					t1 = (av1 - im1[xy]);
					t2 = (av2 - im2[xy]);
					//d1 += t1*t1*c[(iy+bd)*wc + ix+bd];
					//d2 += t2*t2*c[(iy+bd)*wc + ix+bd];
					//d12 += t1*t2*c[(iy+bd)*wc + ix+bd];
					d1 += t1*t1;
					d2 += t2*t2;
					d12 += t1*t2;
				}
			}
			d1 = d1/(double)wc2;
			d2 = d2/(double)wc2;
			d12 = d12/(double)wc2;
			ss = ((2.*av1*av2 + c1)*(2.*d12 + c2))/((av1*av1 + av2*av2 + c1)*(d1 + d2 + c2));
			//printf("SSIM = %f av1 = %f av2 = %f d1 = %f d2 = %f d12 = %f \n", ss, av1, av2, d1, d2, d12);
			ssim += ss;
		}
	}
	//return ssim/(double)((h-2*bd)*(w-2*bd));
	return ssim/(double)((h/wc)*(w/wc));


	if(d==1){
		size = w*h;
		for(i = 0; i < size; i++) av1 += im1[i];
		for(i = 0; i < size; i++) av2 += im2[i];
		av1 = av1/(double)size;
		av2 = av2/(double)size;
		for(i = 0; i < size; i++) d1 += (av1 - im1[i])*(av1 - im1[i]);
		for(i = 0; i < size; i++) d2 += (av2 - im2[i])*(av2 - im2[i]);
		for(i = 0; i < size; i++) d12 += (av1 - im1[i])*(av2 - im2[i]);
		d1 = d1/(double)size;
		d2 = d2/(double)size;
		d12 = d12/(double)size;
		printf("av1 = %f av2 = %f d1 = %f d2 = %f d12 = %f \n", av1, av2, d1, d2, d12);
		ssim = ((2.*av1*av2 + c1)*(2.*d12 + c2))/((av1*av1 + av2*av2 + c1)*(d1 + d2 + c2));
		return ssim;
	}
	if(d==3){
		size = w*h*3;
		//for(i = 0; i < size; i+=3) {tmp += img1[i] - img2[i]; tmp += img1[i+1] - img2[i+1]; tmp += img1[i+2] - img2[i+2]; }
		//printf("diff = %f\n", tmp);

		for(i = 0; i < size; i+=3) { avr1 += img1[i]; avg1 += img1[i+1]; avb1 += img1[i+2]; }//printf("%d %d %d  ", img1[i], img1[i+1], img1[i+2]); }
		for(i = 0; i < size; i+=3) { avr2 += img2[i]; avg2 += img2[i+1]; avb2 += img2[i+2]; }
		avr1 = avr1*3./(double)size; avg1 = avg1*3./(double)size; avb1 = avb1*3./(double)size;
		avr2 = avr2*3./(double)size; avg2 = avg2*3./(double)size; avb2 = avb2*3./(double)size;

		for(i = 0; i < size; i+=3) {
			dr1 += (avr1 - img1[i]  )*(avr1 - img1[i]  );
			dg1 += (avg1 - img1[i+1])*(avg1 - img1[i+1]);
			db1 += (avb1 - img1[i+2])*(avb1 - img1[i+2]);
		}
		for(i = 0; i < size; i+=3) {
			dr2 += (avr2 - img2[i]  )*(avr2 - img2[i]  );
			dg2 += (avg2 - img2[i+1])*(avg2 - img2[i+1]);
			db2 += (avb2 - img2[i+2])*(avb2 - img2[i+2]);
		}
		for(i = 0; i < size; i+=3) {
			dr12 += (avr1 - img1[i]  )*(avr2 - img2[i]  );
			dg12 += (avg1 - img1[i+1])*(avg2 - img2[i+1]);
			db12 += (avb1 - img1[i+2])*(avb2 - img2[i+2]);
		}
		dr1 = dr1*3./(double)size; dg1 = dg1*3./(double)size; db1 = db1*3./(double)size;
		dr2 = dr2*3./(double)size; dg2 = dg2*3./(double)size; db2 = db2*3./(double)size;
		dr12 = dr12*3./(double)size; dg12 = dg12*3./(double)size; db12 = db12*3./(double)size;

		ssimr = ((2.*avr1*avr2 + c1)*(2.*dr12 + c2))/((avr1*avr1 + avr2*avr2 + c1)*(dr1 + dr2 + c2));
		printf("Red   ssim = %f av1 = %f av2 = %f d1 = %f d2 = %f d12 = %f\n", ssimr, avr1, avr2, dr1, dr2, dr12);

		ssimg = ((2.*avg1*avg2 + c1)*(2.*dg12 + c2))/((avg1*avg1 + avg2*avg2 + c1)*(dg1 + dg2 + c2));
		printf("Green ssim = %f av1 = %f av2 = %f d1 = %f d2 = %f d12 = %f\n", ssimg, avg1, avg2, dg1, dg2, dg12);

		ssimb = ((2.*avb1*avb2 + c1)*(2.*db12 + c2))/((avb1*avb1 + avb2*avb2 + c1)*(db1 + db2 + c2));
		printf("Blue  ssim = %f av1 = %f av2 = %f d1 = %f d2 = %f d12 = %f\n", ssimb, avb1, avb2, db1, db2, db12);

		ssim = (ssimr + ssimg + ssimb)/3.;

		return ssim;
	}
}

double utils_psnr(uint8 *before, uint8 *after, uint32 dim, uint32 d){
/// \fn double psnr(uint8 *before, uint8 *after, uint32 dim, uint32 d)
/// \brief Calculate PSNR of two image.
/// \param before	Pointer to first image.
/// \param after	Pointer to second image.
/// \param dim 		Size of image height*width.
/// \param d 		d = 1 if gray image, d = 3 if color image.
/// \retval 		The PSNR.

	uint32 i;
	double psnr;
	unsigned long long r = 0, g = 0, b = 0;
	if(d==3){
		for(i = 0; i< dim; i = i + 3){
			r += (before[i]   - after[i]  )*(before[i]   - after[i]  );
			g += (before[i+1] - after[i+1])*(before[i+1] - after[i+1]);
			b += (before[i+2] - after[i+2])*(before[i+2] - after[i+2]);
		}

		if((r + g + b) == 0){
			return 0.;
		}

		psnr = 10.*log10((255.*255.)/((double)(r + g + b)/((double)dim*3.)));
		return psnr;
	}
	if(d==1){
		for(i = 0; i< dim; i++){
			r += (before[i]   - after[i] )*(before[i]   - after[i]);
		}

		if(r  == 0){
			return 0.;
		}

		psnr = 10.*log10((255.*255.)/((double)r/(double)dim));
		return psnr;
	}
	else{
		printf("psnr: ERROR\n");
		return 0.;
	}
}

double utils_psnr_16(int16 *before, int16 *after, uint32 dim, uint32 d){
/// \fn double psnr(uint8 *before, uint8 *after, uint32 dim, uint32 d)
/// \brief Calculate PSNR of two image.
/// \param before	Pointer to first image.
/// \param after	Pointer to second image.
/// \param dim 		Size of image height*width.
/// \param d 		d = 1 if gray image, d = 3 if color image.
/// \retval 		The PSNR.

	uint32 i;
	double psnr;
	unsigned long long r = 0, g = 0, b = 0;
	if(d==3){
		for(i = 0; i< dim; i = i + 3){
			r += (before[i]   - after[i]  )*(before[i]   - after[i]  );
			g += (before[i+1] - after[i+1])*(before[i+1] - after[i+1]);
			b += (before[i+2] - after[i+2])*(before[i+2] - after[i+2]);
		}

		if((r + g + b) == 0){
			return 0.;
		}

		psnr = 10.*log10((255.*255.)/((double)(r + g + b)/((double)dim*3.)));
		return psnr;
	}
	if(d==1){
		for(i = 0; i< dim; i++){
			r += (before[i]   - after[i] )*(before[i]   - after[i]);
		}

		if(r  == 0){
			return 0.;
		}

		psnr = 10.*log10((255.*255.)/((double)r/(double)dim));
		return psnr;
	}
	else{
		printf("psnr: ERROR\n");
		return 0.;
	}
}

uint8* wavelet_to_rgb(uint8 *img, uint8 *rgb, int height, int width, int step)
{ 
	int i, j, k, t, dim = height*width;
	int w3 = width*3;
	uint8 *p;
	
	if((p = (uint8*)malloc(dim * sizeof(uint8))) == NULL){
		printf("Out of memory\n");
		return NULL;
	}
	t = 1 << step;
	for(j = 0; j < height/t; j++){
		for(i = 0; i < width/t; i++){
			p[width*j + i] = ll(t, i, j);
		} 
	}
	for(k=0; k < step; k++){
		for(j = 0 ; j < height/t; j++){
			for(i = 0; i < width/t; i++){
				p[width*j + i+ width/t] = hl(t, i, j);
			}
		}
		t>>=1;
	}
	t = 1 << step;
	for(k=0; k < step; k++){
		for(j = 0 ; j < height/t; j++){
			for(i = 0; i < width/t; i++){
				p[width*j + (height*width)/t + i] = lh(t, i, j);
			}
		}
		t>>=1;
	}
	t = 1 << step;
	for(k=0; k < step; k++){
		for(j = 0 ; j < height/t; j++){
			for(i = 0; i < width/t; i++){
				p[width*j  + (height*width)/t + i + width/t] = hh(t, i, j);
			}
		}
		t>>=1;
	}
	for(k = 0; k < width; k++){
		for(i = 0; i < height; i++){
			//rgb[ i*w3 + 3*k]  		   = top(low(128 + p[i*width + k]));
			//rgb[ i*w3 + 3*k + 1] 		   = top(low(128 + p[i*width + k]));
			//rgb[ i*w3 + 3*k + 2] 		   = top(low(128 + p[i*width + k]));
			rgb[ i*w3 + 3*k]  			   = p[i*width + k];//+128
			rgb[ i*w3 + 3*k + 1] 		   = p[i*width + k];//+128
			rgb[ i*w3 + 3*k + 2] 		   = p[i*width + k];//+128
		}
	}
	free(p);
	return rgb;
}

uint8* wavelet_to_rgb1(uint8 *img, uint8 *rgb, int height, int width, int step)
{ 
	int i, j, k, t, dim = height*width;
	int w3 = width*3;
	uint8 *p;
	
	if((p = (uint8*)malloc(dim*sizeof(uint8))) == NULL){
		printf("Out of memory\n");
		return NULL;
	}
	t = 1 << step;
	for(j = 0; j < height/t; j++){
		for(i = 0; i < width/t; i++){
			p[width*j + i] = ll(t, i, j);
		} 
	}
	for(k=0; k < step; k++){
		for(j = 0 ; j < height/t; j++){
			for(i = 0; i < width/t; i++){
				p[width*j + i+ width/t] = hl(t, i, j);
			}
		}
		t>>=1;
	}
	t = 1 << step;
	for(k=0; k < step; k++){
		for(j = 0 ; j < height/t; j++){
			for(i = 0; i < width/t; i++){
				p[width*j + (height*width)/t + i] = lh(t, i, j);
			}
		}
		t>>=1;
	}
	t = 1 << step;
	for(k=0; k < step; k++){
		for(j = 0 ; j < height/t; j++){
			for(i = 0; i < width/t; i++){
				p[width*j  + (height*width)/t + i + width/t] = hh(t, i, j);
			}
		}
		t>>=1;
	}
	for(k = 0; k < width; k++){
		for(i = 0; i < height; i++){
			/*rgb[ i*w3 + 3*k]  			   = top(low(128 + p[i*width + k]));
			rgb[ i*w3 + 3*k + 1] 		   = top(low(128 + p[i*width + k]));
			rgb[ i*w3 + 3*k + 2] 		   = top(low(128 + p[i*width + k]));*/
			rgb[ i*w3 + 3*k]  			   = (uint8)clip(128 + p[i*width + k]);
			rgb[ i*w3 + 3*k + 1] 		   = (uint8)clip(128 + p[i*width + k]);
			rgb[ i*w3 + 3*k + 2] 		   = (uint8)clip(128 + p[i*width + k]);
		}
	}
	free(p);
	return rgb;
}

uint8* img_to_rgb128(uint8 *img, uint8 *rgb, int height, int width)
{ 
	int i, j, dim = height*width*3;
	
	for(i = 0,  j= 0; j < dim; j+=3, i++){
		rgb[j]     = rnd(img[i]+128);
		rgb[j + 1] = rnd(img[i]+128);
		rgb[j + 2] = rnd(img[i]+128);
		//printf("img[%d] = %4d\n",i,img[i]);
	}
	return rgb;
}


uint8* uint8_to_rgb(uint8 *img, uint8 *rgb, int height, int width)
{ 
	int i, j, dim = height*width*3;
	
	for(i = 0,  j= 0; j < dim; j+=3, i++){
		rgb[j]     = img[i];
		rgb[j + 1] = img[i];
		rgb[j + 2] = img[i];
		//printf("y_w[%d] = %4d\n",i,mod(yuv_buffer->y_w[i]));
	}
	return rgb;
}

//Copy image
void copy(uint8 *in, uint8 *out, int w, int h)
{
	int i, dim = h*w;
	for(i = 0; i < dim; i++){
		out[i] = in[i];
	}
}

//Compare two image
void compare(uint8 *in, uint8 *out, int w, int h)
{
	int x, y, c=0;
	for(y = 0; y < h; y++){
		for(x=0; x<w; x++){
			if(in[y*w+x] - out[y*w+x]) c++;
		}
		printf("y = %4d c = %4d\n", y, c);
		c=0;
	}
}

//Make image less for one pix horizontal and for one vertical 
void resizeonepix(uint8 *in, int w, int h)
{
	int x, y;
	for(y = 0; y < (h-1); y++){
		for(x=0; x < (w-1); x++){
			in[y*(w-1)+x] = in[y*w+x];
		}
	}
}

uint8* malet_to_rgb(uint8 *img, uint8 *rgb, int h, int w, int step)
{ 
	int x, y, k, t;
	int ws[4], hs[4], s[4], tlx[4], tly[4];
	
	ws[0] = (w>>1) + w%2; hs[0] = (h>>1) + h%2;
	ws[1] = (w>>1); 	  hs[1] = (h>>1) + h%2;
	ws[2] = (w>>1) + w%2; hs[2] = (h>>1);
	ws[3] = (w>>1); 	  hs[3] = (h>>1);
	s[0] = 0; s[1] = ws[0]*hs[0]; s[2] = s[1] + ws[1]*hs[1]; s[3] = s[2] + ws[2]*hs[2];
	tlx[0] = 0    ; tly[0] = 0;
	tlx[1] = ws[0]; tly[1] = 0;
	tlx[2] = 0    ; tly[2] = hs[0];
	tlx[3] = ws[0]; tly[3] = hs[0];
		
	for(t=0; t<step; t++){
		ws[0] = (ws[0]>>1) + ws[0]%2; 	hs[0] = (hs[0]>>1) + hs[0]%2;
		ws[1] = (ws[0]>>1); 	  		hs[1] = (hs[0]>>1) + hs[0]%2;
		ws[2] = (ws[0]>>1) + ws[0]%2; 	hs[2] = (hs[0]>>1);
		ws[3] = (ws[0]>>1); 	  		hs[3] = (hs[0]>>1);
		s[0] = 0; s[1] = ws[0]*hs[0]; s[2] = s[1] + ws[1]*hs[1]; s[3] = s[2] + ws[2]*hs[2];
		tlx[0] = 0    ; tly[0] = 0;
		tlx[1] = ws[0]; tly[1] = 0;
		tlx[2] = 0    ; tly[2] = hs[0];
		tlx[3] = ws[0]; tly[3] = hs[0];
		
		
		for(k=1; k < 4; k++){
			for(y=0; y < hs[k]; y++ ){
				for(x=0; x < ws[k]; x++){
					//if(y==0) printf("rgb[%d] = %d img[%d] = %d \n", 3*((y+tly[k])*w + tlx[k] +x), rgb[3*((y+tly[k])*w + tlx[k] +x)], s[k] + y*ws[k] + x, img[s[k] + y*ws[k] + x]);
					rgb[3*((y+tly[k])*w + tlx[k] +x)]   = rnd(128+img[s[k] + y*ws[k] + x]);
					rgb[3*((y+tly[k])*w + tlx[k] +x)+1] = rnd(128+img[s[k] + y*ws[k] + x]);
					rgb[3*((y+tly[k])*w + tlx[k] +x)+2] = rnd(128+img[s[k] + y*ws[k] + x]);
				}
			}
		}
	}
	k=0;
	for(y=0; y < hs[k]; y++ ){
		for(x=0; x < ws[k]; x++){
			rgb[3*((y+tly[k])*w + tlx[k] +x)]   = rnd(img[s[k] + y*ws[k] + x]);
			rgb[3*((y+tly[k])*w + tlx[k] +x)+1] = rnd(img[s[k] + y*ws[k] + x]);
			rgb[3*((y+tly[k])*w + tlx[k] +x)+2] = rnd(img[s[k] + y*ws[k] + x]);
		}
	}
	
	return rgb;
}


double dist3(uint8 *before, uint8 *after, uint32 dim, uint32 d){
/*! \fn double dist(uint8 *before, uint8 *after, uint32 dim, uint32 d)
    \brief Calculate distortion of two image.
    \param before	Pointer to first image.
    \param after	Pointer to second image.
    \param dim 		Size of image height*width.
    \param d 		d = 1 if gray image, d = 3 if color image.
    \retval 		The distortion.
*/

	uint32 i;
	double ape;
	unsigned long long r = 0, g = 0, b = 0;
	if(d==3){
		for(i = 0; i < dim; i = i + 3){
			//if(before[i]     - after[i]) printf("x = %4d y = %4d r = %4d\n",i/3,i/3,before[i]     - after[i]);
			r += (before[i]     - after[i])*(before[i]     - after[i]);
			g += (before[i+1] - after[i+1])*(before[i+1] - after[i+1]);
			b += (before[i+2] - after[i+2])*(before[i+2] - after[i+2]);
		}
		ape = (double)(r + g + b)/((double)dim*3.);
		return ape;
	}
	if(d==1){
		for(i = 0; i < dim; i++){
			//if(before[i]- after[i]) printf("i = %9d b = %4d a = %4d r = %4d\n",i,before[i], after[i], before[i] - after[i] );
			r += (before[i] - after[i])*(before[i] - after[i]);
			//printf("a = %3d  b = %3d ", before[i], after[i]);
		}
		//printf("r = %d  dim = %d ", r, dim);
		ape = (double)r/(double)dim;
		//printf(" dist = %f\n",ape);
		return ape;
	}
	else{printf("ape: ERROR\n");
		 return 0.;
	}
}

double ape(uint8 *before, uint8 *after, uint32 dim, uint32 d){
/*! \fn double ape(uint8 *before, uint8 *after, uint32 dim, uint32 d)
    \brief Calculate APE of two image.
    \param before	Pointer to first image.
    \param after	Pointer to second image.
    \param dim 		Size of image height*width.
    \param d 		d = 1 if gray image, d = 3 if color image.
    \retval 		The APE.
*/

	uint32 i;
	double ape;
	unsigned long long r = 0, g = 0, b = 0;
	if(d==3){
		for(i = 0; i < dim; i = i + 3){
			//if(before[i]     - after[i]) printf("x = %4d y = %4d r = %4d\n",i/3,i/3,before[i]     - after[i]);
			r += abs((int)(before[i]     - after[i]));
			g += abs((int)(before[i+1] - after[i+1]));
			b += abs((int)(before[i+2] - after[i+2]));
		}
		ape = (double)(r + g + b)/((double)dim*3.);
		return ape;
	}
	if(d==1){
		for(i = 0; i < dim; i++){
			//if(before[i] - after[i] && i < 1920*2) printf("i = %4d b = %4d a = %4d\n", i, before[i], after[i]);
			r += abs((int)(before[i] - after[i] ));
		}
		ape = (double)r/(double)dim;
		return ape;
	}
	else{printf("ape: ERROR\n");
		 return 0.;
	}
}

double ape3(uint8 *before, uint8 *after, uint32 dim, uint32 d){
/*! \fn double ape(uint8 *before, uint8 *after, uint32 dim, uint32 d)
    \brief Calculate APE of two image.
    \param before	Pointer to first image.
    \param after	Pointer to second image.
    \param dim 		Size of image height*width.
    \param d 		d = 1 if gray image, d = 3 if color image.
    \retval 		The APE.
*/

	uint32 i;
	double ape;
	unsigned long long r = 0, g = 0, b = 0;
	if(d==3){
		for(i = 0; i < dim; i = i + 3){
			//if(before[i]     - after[i]) printf("x = %4d y = %4d r = %4d\n",i/3,i/3,before[i]     - after[i]);
			r += abs((int)(before[i]     - after[i]));
			g += abs((int)(before[i+1] - after[i+1]));
			b += abs((int)(before[i+2] - after[i+2]));
		}
		ape = (double)(r + g + b)/((double)dim*3.);
		return ape;
	}
	if(d==1){
		for(i = 0; i < dim; i++){
			//if(before[i]- after[i]) printf("i = %4d r = %4d\n",i,before[i] - after[i]);
			r += abs((int)(before[i] - after[i] ));
		}
		ape = (double)r/(double)dim;
		return ape;
	}
	else{printf("ape: ERROR\n");
		 return 0.;
	}
}


double psnr3(uint8 *before, uint8 *after, uint32 dim, uint32 d){
/*! \fn double psnr(uint8 *before, uint8 *after, uint32 dim, uint32 d)
    \brief Calculate PSNR of two image.
    \param before	Pointer to first image.
    \param after	Pointer to second image.
    \param dim 		Size of image height*width.
    \param d 		d = 1 if gray image, d = 3 if color image.
    \retval 		The PSNR.
*/

	uint32 i;
	double psnr;
	unsigned long long r = 0, g = 0, b = 0;
	if(d==3){
		for(i = 0; i< dim; i = i + 3){
			r += (before[i]   - after[i]  )*(before[i]   - after[i]  );
			g += (before[i+1] - after[i+1])*(before[i+1] - after[i+1]);
			b += (before[i+2] - after[i+2])*(before[i+2] - after[i+2]);
		}
		
		if((r + g + b) == 0){
			return 0.;
		}
		
		psnr = 10.*log10((255.*255.)/((double)(r + g + b)/((double)dim*3.)));
		return psnr;
	}
	if(d==1){
		for(i = 0; i< dim; i++){
			r += (before[i]   - after[i] )*(before[i]   - after[i]);
		}
		
		if(r  == 0){
			return 0.;
		}
		
		psnr = 10.*log10((255.*255.)/((double)r/(double)dim));
		return psnr;
	}
	else{
		printf("psnr: ERROR\n");
		return 0.;
	}
}

uint8* YUV_to_RGB(uint8 *rgb, uint8 *y, uint8 *u, uint8 *v, uint32 sq)
{
	int C, D, E, i;
	for(i=0; i<sq; i++){
		C = y[i] - 16;
		D = u[i] - 128;
		E = v[i] - 128;
		rgb[i*3    ] = clip(( 298*C         + 409*E + 128) >> 8);
		rgb[i*3 + 1] = clip(( 298*C - 100*D - 208*E + 128) >> 8);
		rgb[i*3 + 2] = clip(( 298*C + 516*D         + 128) >> 8);
	}
	return rgb;
}

uint8* YUV420p_to_RGB(uint8 *rgb, uint8 *y, uint8 *u, uint8 *v, uint32 w, uint32 h)
{
	int C, D, E, i, j;
	for(i=0; i<h; i++){
		for(j=0; j< w; j++){
			C = y[j + w*i] - 16;
			D = u[(j>>1) + (w>>1)*(i>>1)] - 128;
			E = v[(j>>2) + (w>>1)*(i>>2)] - 128;
			rgb[(j+w*i)*3    ] = clip(( 298*C         + 409*E + 128) >> 8);
			rgb[(j+w*i)*3 + 1] = clip(( 298*C - 100*D - 208*E + 128) >> 8);
			rgb[(j+w*i)*3 + 2] = clip(( 298*C + 516*D         + 128) >> 8);
		}
	}
	return rgb;
}

uint32 utils_read_ppm(const char *filename, uint32 *w, uint32 *h, uint32 *bpp, uint8 *img)
{
    FILE *wl;
    uint8 line[100];
    uint32 byts;

    wl = fopen(filename, "rb");
    if(wl == NULL) {
        printf("Can't open file %s\n", filename);
        return 0;
    }
    //byts = fscanf(wl, "%s%s%s%s%s%s%s", line[0], line[1], line[2], line[3], line[4], line[5], line[6]);
    byts = fscanf(wl, "%s", line);
    if (strcmp(line, "P6") != 0) {
        printf ("It's not PPM file");
        return 0;
    }
    byts = fscanf(wl, "%s", line);
    if(line[0] == '#'){
        if(fgets(line, 100, wl) == NULL) { printf("Can't get header\n"); return; }
    } else {
        *w = atoi(line);
        byts = fscanf(wl, "%s", line); *h = atoi(line);
        byts = fscanf(wl, "%s", line); *bpp = (atoi(line) > 256) ? 2 : 1;
    }

    byts = fscanf(wl, "%s", line); *w = atoi(line);
    byts = fscanf(wl, "%s", line); *h = atoi(line);
    byts = fscanf(wl, "%s", line); *bpp = (atoi(line) > 256) ? 2 : 1;
    printf("w = %d h = %d bpp = %d\n", *w, *h, *bpp);
    fgetc(wl);

    //*img = (uint8 *)calloc((*w)*(*h)*(*bpp)*3, sizeof(uint8));
    byts = fread(img, sizeof(uint8), (*w)*(*h)*(*bpp)*3,  wl);
    if(byts != (*w)*(*h)*(*bpp)*3){ printf("Image read error\n");}
    //printf("byts = %d size = %d\n", byts, (*w)*(*h)*(*bpp)*3);
    fclose(wl);
    //fclose(wl);
    return byts;
}

uint32 utils_read_pgm_whb(FILE **wl, const char *filename, uint32 *w, uint32 *h, uint32 *bpp)
{
    //FILE *wl;
    uint8 line[100];
    uint32 byts;

    *wl = fopen(filename, "rb");
    if(*wl == NULL) {
        printf("Can't open file %s\n", filename);
        return 0;
    }

    byts = fscanf(*wl, "%s", line);
    if (strcmp(line, "P5") != 0) {
        printf ("It's not PPM file");
        return 0;
    }
    //printf("byts = %d p = %p\n", byts, (*wl)->_IO_read_ptr);

    byts += fscanf(*wl, "%s", line); *w = atoi(line);
    byts += fscanf(*wl, "%s", line); *h = atoi(line);
    byts += fscanf(*wl, "%s", line); *bpp = (atoi(line) > 256) ? 2 : 1;
    //byts += fscanf(*wl, "%s", line);
    printf("w = %d h = %d bpp = %d\n", *w, *h, *bpp);
    //printf("byts = %d p = %p\n", byts, (*wl)->_IO_read_ptr);


    return byts;
}

uint32 utils_read_pgm_img(FILE **wl, uint32 *w, uint32 *h, uint32 *bpp, uint8 *img)
{
    uint32 byts;
    //printf("wl = %p\n", *wl);

    //printf("byts = %d p = %p\n", byts, (*wl)->_IO_read_ptr);
    //printf("size = %d\n", (*w)*(*h)*(*bpp));
    if(*bpp > 1) byts = fread(img, sizeof(uint8), 1,  *wl); //I don't know

    byts = fread(img, sizeof(uint8), (*w)*(*h)*(*bpp),  *wl);
    //printf("byts = %d size = %d\n", byts, (*w)*(*h)*(*bpp));
    if(byts != (*w)*(*h)*(*bpp)){ printf("Image read error\n");}
    //printf("byts = %d\n", byts);

    fclose(*wl);
    return byts;
}

void utils_rgb2bayer(uint8 *rgb, int16 *bay, uint32 w, uint32 h)
{
	uint32 w1 = w<<1, h1 = h<<1, w3 = w*3, x, x1, y, y1, yx, yx1;
	for(y=0, y1=0; y < h1; y+=2, y1++){
		for(x=0, x1=0; x < w1; x+=2, x1+=3){
			yx = y*w1 + x;
			yx1 = y1*w3 + x1;
            bay[yx] = rgb[yx1];
			bay[yx+1] = rgb[yx1+1];
			bay[yx+w1] = rgb[yx1+1];
			bay[yx+w1+1] = rgb[yx1+2];
		}
	}
}

void utils_subtract(uint8 *img1, uint8 *img2, uint8 *sub, uint32 w, uint32 h)
{
	uint32 sq = w*h, x, y, yx;
	for(y=0; y < sq; y+=w){
		for(x=0; x < w; x++){
			yx = y + x;
			sub[yx] = abs(img2[yx] - img1[yx]);
		}
	}

}

/* \brief Calculate image entropy.
	\param img 	The input image.
	\param buf 	The buffer for probability distribution.
	\param w 	The image width.
	\param h 	The image height.
	\retval 	The entropy (bits per pixel).
 */
double entropy8(uint8 *img, uint32 *buf, uint32 w, uint32 h, uint32 bpp)
{
	double s = 0., s0 = log2(w*h);
	uint32  half = 1<<(bpp-1), size = 1<<bpp, sz = w*h;
	int i;

	for(i=0; i < size; i++) buf[i] = 0;
    //for(i=0; i < sz; i++) buf[img[i]+half]++;
    for(i=0; i < sz; i++) buf[img[i]]++;
    //for(i=0; i < size; i++) printf("%d ",  buf[i]);
	//printf("\n");

	for(i=0; i < size; i++){
		if(buf[i]) s -= buf[i]*(log2(buf[i]) - s0);
	}
	return s/sz;
}

double entropy16(int16 *img, uint32 *buf, uint32 w, uint32 h, uint32 bpp)
{
	double s = 0., s0 = log2(w*h);
	uint32  half = 1<<(bpp-1), size = 1<<bpp, sz = w*h;
	int i;

	for(i=0; i < size; i++) buf[i] = 0;
	for(i=0; i < sz; i++) buf[img[i]+half]++;
	//for(i=0; i < size; i++) printf("%d ",  buf[i]);
	//printf("\n");

	for(i=0; i < size; i++){
		if(buf[i]) s -= buf[i]*(log2(buf[i]) - s0);
	}
	return s/sz;
}

void utils_subtract1(uint8 *img1, uint8 *img2, int16 *img3, uint32 w, uint32 h)
{
    uint32 x, y, yw, yw1, yx, h1 = h-1, w1 = w-1, w2 = w-2;

    for(y=1; y < h1; y++){
        yw = y*w;  yw1 = (y-1)*w2;
        for(x=1; x < w1; x++){
            yx = yw + x;
            img3[yw1 + x - 1] = img1[yx] - img2[yx];
            /*
            if(img3[yw1 + x - 1]){
                img3[yw1 + x - 1] = 255;
                printf("yx = %d\n", yx);
            }*/
        }
    }
}

void utils_remove_border(uint8 *img1, uint8 *img2, uint32 w, uint32 h)
{
    uint32 x, y, yw, yw1, yx, h1 = h-1, w1 = w-1, w2 = w-2;

    for(y=1; y < h1; y++){
        yw = y*w;  yw1 = (y-1)*w2;
        for(x=1; x < w1; x++){
            yx = yw + x;
            img2[yw1 + x - 1] = img1[yx];
        }
    }
}

