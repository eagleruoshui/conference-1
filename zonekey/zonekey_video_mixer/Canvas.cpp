#include "Canvas.h"

Canvas::Canvas(int width, int height)
	: width_(width)
	, height_(height)
{
	avpicture_alloc(&pic_, PIX_FMT_YUV420P, width_, height_);
	
	// ��ʼ��Ϊ������ Y=0, U=V=0x80
	unsigned char *y = pic_.data[0];
	for (int i = 0; i < height_; i++) {
		memset(y, 0, width_);
		y += pic_.linesize[0];
	}

	unsigned char *u = pic_.data[1];
	for (int i = 0; i < height_/2; i++) {
		memset(u, 0x80, width_/2);
		u += pic_.linesize[1];
	}

	unsigned char *v = pic_.data[2];
	for (int i = 0; i < height_/2; i++) {
		memset(v, 0x80, width_/2);
		v += pic_.linesize[2];
	}
}

Canvas::~Canvas(void)
{
	avpicture_free(&pic_);
}


/** һ�� yuv ͼ������������ yuv420p
 */
struct YUV
{
	unsigned char *data[4];
	int stride[4];

	int width, height;
};

struct POINT
{
	int x, y;
};

struct RECT
{
	POINT tl;	// ���Ͻ�
	POINT br;	// ���½�
};

#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) < (y) ? (x) : (y))

inline void do_pixel(unsigned char *pd, unsigned char *ps, unsigned char alpha)
{
	*pd = (*pd * (256 - alpha) + *ps * alpha) >> 8;
	//*pd += (((*ps - *pd) * alpha) >> 8);
}

/** yuv ���ݸ��ƻ��߻�ϣ�

		pt_dst Ϊ dst �е����Ͻ�
		pt_src Ϊ src �е����Ͻ�
		width, height Ϊ���δ�С

		ע�⣺�Ѿ���֤����Ҫ���Ƶľ�����ȫ�� dst �ķ�Χ����
		
 */
static void do_compiz(YUV dst, POINT pt_dst, YUV src, POINT pt_src, int width, int height, unsigned char alpha)
{
	unsigned char *pd_y = &dst.data[0][pt_dst.y * dst.stride[0] + pt_dst.x];
	unsigned char *ps_y = &src.data[0][pt_src.y * src.stride[0] + pt_src.x];
	unsigned char *pd_u = &dst.data[1][pt_dst.y/2 * dst.stride[1] + pt_dst.x/2];
	unsigned char *ps_u = &src.data[1][pt_src.y/2 * src.stride[1] + pt_src.x/2];
	unsigned char *pd_v = &dst.data[2][pt_dst.y/2 * dst.stride[2] + pt_dst.x/2];
	unsigned char *ps_v = &src.data[2][pt_src.y/2 * src.stride[2] + pt_src.x/2];

	if (alpha == 255) {
		// ��ʱ����Ҫ����ϣ�ֱ���ڴ渴��
		for (int i = 0; i <height/2; i++) {
			memcpy(pd_y, ps_y, width);	// Y ��
			pd_y += dst.stride[0], ps_y += src.stride[0];

			memcpy(pd_y, ps_y, width);	// Y 
			pd_y += dst.stride[0], ps_y += src.stride[0];

			memcpy(pd_u, ps_u, width/2);	// U
			pd_u += dst.stride[1], ps_u += src.stride[1];

			memcpy(pd_v, ps_v, width/2);	// V
			pd_v += dst.stride[2], ps_v += src.stride[2];
		}
	}
	else if (alpha == 0) {
	}
	else {
		// ��ʱ��Ҫ���
		for (int i = 0; i <height/2; i++) {
			for (int j = 0; j < width; j++)
				do_pixel(pd_y+j, ps_y+j, alpha);
			pd_y += dst.stride[0], ps_y += src.stride[0];

			for (int j = 0; j < width; j++)
				do_pixel(pd_y+j, ps_y+j, alpha);
			pd_y += dst.stride[0], ps_y += src.stride[0];

			for (int j = 0; j < width/2; j++)
				do_pixel(pd_u+j, ps_u+j, alpha);
			pd_u += dst.stride[1], ps_u += src.stride[1];

			for (int j = 0; j < width/2; j++)
				do_pixel(pd_v+j, ps_v+j, alpha);
			pd_v += dst.stride[2], ps_v += src.stride[2];
		}
	}
}

/** �� src yuv ͼ�񣬻��� dst

			���뿼�� dst & src Խ������

          (0,0)
			--------------------------------------------
			|                                  dst     |
			|                                          |
			|                                          |
			|                                          |
			|                             (x,y)        |
			|                              |-----------------------|
			|                              |///////////|       src |
			|                              |///////////|		   |	
			|                              |///////////|		   |
			|                              |///////////|		   |
			|                              |///////////|		   |
			|                              |///////////|		   |
			|------------------------------|------------		   |
			                               |					   |
										   ------------------------

		������Ҫ���Ƶ����ݣ��� dst �� src �еľ���

 */
static void do_compiz_safe(YUV dst, YUV src, int x, int y, unsigned char alpha)
{
	POINT pt_dst, pt_src;
	int width, height;

	if (x >= 0) {
		if (x >= dst.width) return;	// û�н���
		pt_dst.x = x;
		pt_src.x = 0;
		
		width = MIN(src.width, dst.width-x);
	}
	else {
		if (0 - x >= src.width) return;	// û�н���
		pt_dst.x = 0;
		pt_src.x = -x;

		width = src.width + x;
	}

	if (y >= 0) {
		if (y >= dst.height) return;	// û�н���
		pt_dst.y = y;
		pt_src.y = 0;

		height = MIN(src.height, dst.height-y);
	}
	else {
		if (0 - y >= src.height) return;	// û�н���
		pt_dst.y = 0;
		pt_src.y = -y;

		height = src.height + y;
	}
	
	do_compiz(dst, pt_dst, src, pt_src, width, height, alpha);
}

#if USING_MMX
/** ���� mmx ���Ż���ϣ��ÿ��ͬʱ����4���ֽ�
	alpha_src: 8�ֽ�
	alpha_dst: 8�ֽ�
 */
inline void do_pixel4_mmx(unsigned char *pd,  unsigned char *ps,  unsigned char *alpha_src, unsigned char *alpha_dst)
{
	// *pd = *pd + (*ps - *pd) * alpla / 256;  alpha = [0..256]
	// *pd = (*pd * *alpha_dst + *ps * alpha_src) >> 8;

	// mm0: Դ�ĸ��ֽ�
	// mm1: Ŀ���ĸ��ֽ�
	// mm2: 00 00 00 00 00 00 00 00 ���� unpack
	// mm3: 00 alpha 00 alpha 00 alpha 00 alpha    alpha src
	// mm4: 00 alpha 00 alpha 00 alpha 00 alpha    alpha dst
	__asm {
							// ��Ҫ��֤���ֽ���Ч
		pxor mm2, mm2;		// �� 0

		mov esi, alpha_src;
		movq mm3, [esi];	// alpha src

		mov esi, alpha_dst;
		movq mm4, [esi];	// alpha dst

		mov esi, ps;		// Դ��ַ
		movd mm0, [esi];	// Դ����4�ֽ�
		punpcklbw mm0, mm2;	// չ��		00 s3 00 s2 00 s1 00 s0

		mov edi, pd;		// Ŀ���ַ
		movd mm1, [edi];	// Ŀ������4�ֽ�
		punpcklbw mm1, mm2;	// չ��		00 d3 00 d2 00 d1 00 d0

		pmullw mm1, mm4;	// *pd * alpha_dst
		pmullw mm0, mm3;	// *ps * alpha_src

		paddw mm0, mm1;		// ps * alpha_src + pd * alpha_dst

		psrlw mm0, 8;			// (ps * alpha_src + pd * alpha_dst) / 255


		packuswb mm0, mm0;	// ����

		movd [edi], mm0;
	};
}

static void do_compiz_mmx(YUV *dst, YUV *src, int x, int y, unsigned char alpha)
{
	// FIXME�����뿼�ǲ���4�ı����������
	unsigned char *pd_y = &dst->data[0][y * dst->stride[0] + x];
	unsigned char *ps_y = src->data[0];
	unsigned char *pd_u = &dst->data[1][y/2 * dst->stride[1] + x/2];
	unsigned char *ps_u = src->data[1];
	unsigned char *pd_v = &dst->data[2][y/2 * dst->stride[2] + x/2];
	unsigned char *ps_v = src->data[2];

	
	if (alpha == 255) {
		// ��ʱ����Ҫ����ϣ�ֱ���ڴ渴��
		for (int i = 0; i <src->height/2; i++) {
			memcpy(pd_y, ps_y, src->width);	// Y ��
			pd_y += dst->stride[0], ps_y += src->stride[0];

			memcpy(pd_y, ps_y, src->width);	// Y 
			pd_y += dst->stride[0], ps_y += src->stride[0];

			memcpy(pd_u, ps_u, src->width/2);	// U
			pd_u += dst->stride[1], ps_u += src->stride[1];

			memcpy(pd_v, ps_v, src->width/2);	// V
			pd_v += dst->stride[2], ps_v += src->stride[2];
		}
	}
	else if (alpha == 0) {
	}
	else {
	
		// Ԥ��չ�� alpha_src, alpha_dst
		unsigned __int64 a = alpha;
		a <<= 16;
		a |= alpha;
		a <<= 16;
		a |= alpha;
		a <<= 16;
		a |= alpha;

		unsigned __int64 b = 256 - alpha;
		b <<= 16;
		b |= (256 - alpha);
		b <<= 16;
		b |= (256 - alpha);
		b <<= 16;
		b |= (256 - alpha);

		// ÿ����Y��һ�� U/V
		int sy = src->stride[0], dy = dst->stride[0];
		for (int i = 0; i < src->height/2; i++) {
			// ÿ�δ��� 4 ��
			for (int j = 0; j < src->width/2; j += 4) {
				do_pixel4_mmx(pd_y+j, ps_y+j, (unsigned char*)&a, (unsigned char*)&b);	// ��һ�� Y ǰ���
				do_pixel4_mmx(pd_y+j+src->width/2, ps_y+j+src->width/2, (unsigned char*)&a, (unsigned char*)&b);	// ��һ�� Y ����
				do_pixel4_mmx(pd_y+dy+j, ps_y+sy+j, (unsigned char*)&a, (unsigned char*)&b);	// �ڶ��� Y ǰ���
				do_pixel4_mmx(pd_y+dy+j+src->width/2, ps_y+sy+j+src->width/2, (unsigned char*)&a, (unsigned char*)&b);	// �ڶ��� Y ����

				do_pixel4_mmx(pd_u+j, ps_u+j, (unsigned char*)&a, (unsigned char *)&b);	// U
				do_pixel4_mmx(pd_v+j, ps_v+j, (unsigned char*)&a, (unsigned char *)&b);	// V
			}
			pd_y += dy,  ps_y += sy;
			pd_y += dy,  ps_y += sy;
			pd_u += dst->stride[1],	ps_u += src->stride[1];
			pd_v += dst->stride[2],	ps_v += src->stride[2];
		}

		//__asm__ ("emms");
 
	}
}
#endif // USING_MMX

int Canvas::draw_yuv(unsigned char *data[4], int stride[4],
					 int x, int y, int width, int height, int alpha)
{
	YUV dst, src;
	dst.data[0] = pic_.data[0], dst.data[1] = pic_.data[1], dst.data[2] = pic_.data[2];
	dst.stride[0] = pic_.linesize[0], dst.stride[1] = pic_.linesize[1], dst.stride[2] = pic_.linesize[2];
	dst.width = width_, dst.height = height_;

	src.data[0] = data[0], src.data[1] = data[1], src.data[2] = data[2];
	src.stride[0] = stride[0], src.stride[1] = stride[1], src.stride[2] = stride[2];
	src.width = width, src.height = height;

	do_compiz_safe(dst, src, x, y, alpha & 0xff);

	return 0;
}

void Canvas::clear()
{
	// FIXME����ʵ��һ��ͼ��ת��������д
	//		����򵥵����� Y = 55, uv = 80 ��������ɫ

	unsigned char *p = pic_.data[0];
	for (int i = 0; i < height_; i++) {
		memset(p, 0x55, width_);
		p += pic_.linesize[0];
	}

	p = pic_.data[1];
	for (int i = 0; i < height_/2; i++) {
		memset(p, 0x80, width_/2);
		p += pic_.linesize[1];
	}

	p = pic_.data[2];
	for (int i = 0; i < height_/2; i++) {
		memset(p, 0x80, width_/2);
		p += pic_.linesize[2];
	}
}
