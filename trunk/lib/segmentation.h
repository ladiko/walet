/*
 * segmentation.h
 *
 *  Created on: 21.08.2010
 *      Author: vadim
 */

#ifndef SEGMENTATION_H_
#define SEGMENTATION_H_

typedef struct {
	uint16		x;			//The point location
	uint8 		cl[4];		//The left color
	uint8 		cr[4];		//The right color
} Point;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//New segmentation interface
void seg_find_intersect	(uint8 *grad, uint8 *out, uint32 w, uint32 h);
void seg_find_intersect1(uint8 *grad, uint8 *con, uint32 w, uint32 h, uint32 th);
void seg_find_intersect2(uint8 *grad, uint8 *con, uint8 *di, uint32 w, uint32 h);
void seg_find_intersect3(uint8 *grad, uint32 *con, uint8 *di, uint32 w, uint32 h);
uint32 seg_vertex(uint8 *con, Vertex *vx, Vertex **vp, Line *ln, Line **lp, uint32 w, uint32 h);
uint32 seg_vertex1(uint8 *con, uint8 *di, Vertex *vx, Vertex **vp, uint32 w, uint32 h);
uint32 seg_vertex2(uint32 *con, uint8 *di, Vertex *vx, Vertex **vp, uint32 w, uint32 h);
uint32 seg_vector(uint8 *con, Vertex *vx, Vertex **vp, Line *ln, uint32 vxc, uint32 w);
void seg_grad_RGB(uint8 *R, uint8 *G, uint8 *B, uint8 *grad, uint32 w, uint32 h, uint32 th);
uint32 seg_remove_virtex(Vertex **vp, uint32 vxc, uint32 w, uint32 h);
uint32 seg_fill_reg(uint32 *reg, uint32 *buff, uint32 w, uint32 h);
void seg_remove_contour(uint32 *reg, uint32 w, uint32 h);
void seg_vertex_draw1(uint8 *img, Vertex **vp, uint32 vxc, uint32 w, uint32 h, uint32 k);
void seg_vertex_draw2(uint8 *img, Vertex **vp, uint32 vxc, uint32 w, uint32 h, uint32 w1, uint32 h1);
void seg_vertex_draw3(uint8 *img, Vertex **vp, uint32 vxc, uint32 w, uint32 h, uint32 w1, uint32 h1);
uint32 seg_draw_color_one(uint8 *img, uint8 *col, uint32 *buff, uint32 w, uint32 h);
void seg_draw_line_one(uint8 *img, uint32 w, uint32 h);

void seg_vertex_draw(uint8 *r, uint8 *g, uint8 *b, Vertex **vp, uint32 vxc, uint32 w);
void seg_draw_line_color(uint8 *r, uint8 *g, uint8 *b, Line *ln, uint32 lc, uint32 w, uint32 h);

uint32  seg_get_color(uint8 *r, uint8 *g, uint8 *b, uint8 *r1, uint8 *g1, uint8 *b1, uint8 *col, uint32 w, uint32 h);
uint32  seg_draw_color(uint8 *r1, uint8 *g1, uint8 *b1, uint8 *col, uint32 w, uint32 h);

uint32 seg_get_color1(uint8 *r, uint8 *g, uint8 *b, uint8 *r1, uint32 *col, uint32 *colp, uint8 *pon, uint32 *buf, uint32 w, uint32 h);
uint32 seg_draw_color1(uint8 *r, uint8 *g, uint8 *b, uint8 *r1, uint32 *col, uint32 *buf, uint32 w, uint32 h);

uint32 seg_get_color2(uint8 *r, uint8 *g, uint8 *b, uint8 *r1, uint8 *col, uint32 *l1, uint32 *l2, uint32 w, uint32 h);
uint32 seg_draw_color2(uint8 *r, uint8 *g, uint8 *b, uint8 *col, uint32 *l1, uint32 *l2, uint32 w, uint32 h);
uint32 seg_init_regs(uint8 *r, uint8 *g, uint8 *b, uint8 *r1, uint16 *rg, uint32 *col, uint32 *l1, uint32 *l2, uint32 w, uint32 h);
uint32 seg_fill_regs(uint8 *r, uint8 *g, uint8 *b, uint8 *r1, uint16 *rg, uint32 *col, uint32 *l1, uint32 rgc, uint32 w, uint32 h);

void seg_max_rise(uint8 *img, uint8 *img1, uint32 w, uint32 h);
void seg_max_con(uint8 *img, uint8 *img1, uint32 w, uint32 h);

void seg_fall_forest(uint8 *img, uint32 *img1, uint32 w, uint32 h);
uint32 seg_group_pixels(uint8 *r, uint8 *g, uint8 *b, uint8 *r1,  uint8 *con, uint32 *rg, uint8 *col, uint32 *l1, uint32 *l2, uint32 w, uint32 h);
void seg_draw_reg(uint8 *r, uint8 *g, uint8 *b, uint32 *rg, uint8 *col, uint32 w, uint32 h);
void seg_draw_grad(uint8 *grad, uint8 *out, uint32 *rg, uint32 w, uint32 h);

uint32 seg_new_contur(uint8 *grad, uint8 *con, uint32 *l1, uint32 *l2, uint32 w, uint32 h);

void seg_grad_sub(uint8 *img, uint8 *img1, uint32 w, uint32 h, uint32 th);


void seg_grad_buf(uint8 *img, uint8 *img1, uint8 *buff, uint32 w, uint32 h, uint32 th);
uint32 seg_group_reg(uint32 *reg, uint32 *buff, uint32 w, uint32 h);
void seg_grad_max(uint8 *img, uint8 *img1, uint32 w, uint32 h);
uint32 seg_remove_line(uint8 *con, uint8 *tmp, uint32 *buff, uint32 w, uint32 h);
uint32 seg_remove_line1(uint8 *con, uint32 w, uint32 h);
uint32 seg_remove_vertex(uint8 *con, uint32 *reg, uint32 w, uint32 h);

void seg_find_clusters(uint32 *i3d, uint16 *lut, int16 *r, int16 *g, int16 *b, uint32 w, uint32 h, uint32 rd,  uint32 bpp, p3d *q,uint32 *buf);
void seg_quantization(uint16 *lut, uint8 *rgb, int16 *r, int16 *g, int16 *b, uint32 w, uint32 h, uint32 bpp, p3d *q);
void seg_find_clusters_2d(uint8 *in, uint8 *out, uint32 w, uint32 h, uint32 ds, uint32 dc, uint32 bpp, uint32 *buf);
void seg_corner(int16 *img, uint8 *img1, uint32 w, uint32 h, uint32 th);
void seg_point(uint8 *con, uint32 w, uint32 h);

void seg_grad16(int16 *img, uint8 *img1, uint32 w, uint32 h, uint32 th);
void seg_grad(uint8 *img, uint8 *img1, uint32 w, uint32 h, int th);
void seg_local_max( Pixel *pix,  uint32 *npix, uint8 *img, uint32 w, uint32 h);
uint32 seg_line(Pixel *pix, Edge *edges, uint8 *img, uint32 w, uint32 h);
void seg_draw_lines(Pixel *pix, uint8 *img, uint32 w, uint32 h);
void seg_draw_edges(Pixel *pix, Edge *edge, uint32 nedge, uint8 *img, uint32 w, uint32 h, uint32 px, uint32 col);
void seg_draw_edges_des(Pixel *pix, Edge *edge, uint32 nedge, uint8 *img, uint32 w, uint32 h, uint32 px, uint32 col);
void seg_compare(Pixel *pix, Edge *edge, uint32 nedge, uint8 *grad1, uint8 *grad2, uint8 *img1, uint8 *img2, uint8 *mmb, uint32 w, uint32 h, uint32 mvs);
void seg_draw_vec(Pixel *pix, uint32 npix, uint8 *img, uint32 w, uint32 h);
void seg_draw_pix(Pixel *pix, uint8 *img, uint8 *grad, uint32 w, uint32 h, uint32 col);
void seg_intersect_pix(uint8 *img1, uint8 *img2, uint32 w, uint32 h);
void seg_mvector_copy(Pixel *pix, uint8 *grad1, uint8 *img1, uint8 *img2, uint32 w, uint32 h);
void seg_reduce_line(Pixel *pix, uint8 *img, uint32 w, uint32 h);
uint32 seg_pixels(Pixel *pix, uint8 *img, uint32 w, uint32 h);
uint32 seg_region(Pixel *pix, uint8 *img, uint32 w, uint32 h);
uint32 seg_points(uint8 *img, uint32 w, uint32 h);


void seg_quant(uint8 *img1, uint8 *img2, uint32 w, uint32 h, uint32 q);
//void seg_fall_forest(uint8 *img, uint32 *img1, uint32 w, uint32 h);
void seg_fall_forest1(uint8 *img, uint8 *img1, uint8 *img2, uint32 w, uint32 h);
void seg_grad1(uint8 *img, uint8 *img1,uint8 *img2, uint32 w, uint32 h, uint32 th);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SEGMENTATION_H_ */
