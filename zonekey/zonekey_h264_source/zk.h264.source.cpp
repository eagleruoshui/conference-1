/** һ�� mediastreamer2 �� source filter�����Ϊ���� rfc3894 ����� h264 ��������ֱ������ rtpsender filter

		�ṩһ�� method ����һ������ָ�룬����д�� h264 ֡����
 */

#include <stdio.h>
#include <stdlib.h>
#include <mediastreamer2/msfilter.h>
#include <mediastreamer2/rfc3984.h>
#include <mediastreamer2/msticker.h>
#include <mediastreamer2/allfilters.h>
#include <mediastreamer2/zk.h264.source.h>
#include <deque>
#include <cc++/thread.h>

namespace {
typedef struct CTX
{			
	Rfc3984Context *rfc3984;	// ���ڴ��.
	std::deque<MSQueue *> fifo_;		// �� writer() ��д�룬�� _process() ����ȡ.
	ost::Mutex cs_fifo_;
} CTX;
};

static void _init(MSFilter *f)
{
	CTX *ctx = new CTX;
	f->data = ctx;

	ctx->rfc3984 = rfc3984_new();
	rfc3984_set_mode(ctx->rfc3984, 1);
}

static void _uninit(MSFilter *f)
{
	CTX *ctx = (CTX*)f->data;
	rfc3984_destroy(ctx->rfc3984);
	delete ctx;
}

static void _preprocess(MSFilter *f)
{
	CTX *ctx = (CTX*)f->data;
}

static void _postprocess(MSFilter *f)
{
	CTX *ctx = (CTX*)f->data;

	// TODO: �����ͷŵ�����û�б�ȡ����.
}

static MSQueue *next_queue(CTX *ctx)
{
	MSQueue *queue = 0;
	ost::MutexLock al(ctx->cs_fifo_);
	if (!ctx->fifo_.empty()) {
		queue = ctx->fifo_.front();
		ctx->fifo_.pop_front();
	}
	return queue;
}

static void _process(MSFilter *f)
{
	CTX *ctx = (CTX*)f->data;
	MSQueue *queue = next_queue(ctx);

	while (queue) {
		/**  FIXME: ����ֱ��ʹ�����ʱ����ǲ��Եģ�Ӧ��ʹ�� writer() �е�ʱ���
					������� _process �ĵ���Ƶ�������Ƶ֡����˵���Ƚϸ� (100fps�����Կ���ʱ�����ƫ�Ʋ���̫���� :)
		 */
		uint32_t ts=f->ticker->time*90LL;

		// ʹ�� rfc3984 ���������д����һ�� filter
		rfc3984_pack(ctx->rfc3984, queue, f->outputs[0], ts);
		ms_queue_destroy(queue);	// ����ʹ��.

		// ����
		queue = next_queue(ctx);
	}
}

// �� annex b ���У�������һ�� startcode�������� 00 00 00 01 ���� 00 00 01
// from  libavformat/avc.c
static const uint8_t *ff_avc_find_startcode_internal(const uint8_t *p, const uint8_t *end)
{
#if 0
	for (; p + 3 < end; p++) {
		if (p[0] == 0 && p[1] == 0) {
			if (p[2] == 1) return p;
			else if (p[2] == 0 && p[3] == 1) return p;
		}

		p++;
	}

	return end;
#else
    const uint8_t *a = p + 4 - ((intptr_t)p & 3);

    for (end -= 3; p < a && p < end; p++) {
        if (p[0] == 0 && p[1] == 0 && p[2] == 1)
            return p;
    }

    for (end -= 3; p < end; p += 4) {
        uint32_t x = *(const uint32_t*)p;
        if ((x - 0x01010101) & (~x) & 0x80808080) { // generic
            if (p[1] == 0) {
                if (p[0] == 0 && p[2] == 1)
                    return p;
                if (p[2] == 0 && p[3] == 1)
                    return p+1;
            }
            if (p[3] == 0) {
                if (p[2] == 0 && p[4] == 1)
                    return p+2;
                if (p[4] == 0 && p[5] == 1)
                    return p+3;
            }
        }
    }

    for (end += 3; p < end; p++) {
        if (p[0] == 0 && p[1] == 0 && p[2] == 1)
            return p;
    }

    return end + 3;
#endif // 
}

/** �� 00 00 00 01 67 .... �� h264 ����ת��Ϊ MSQueue
	���� sps, pps ...
 */
static MSQueue *annexb2queue(const void *data, int len)
{
	MSQueue *queue = (MSQueue *)ms_new(MSQueue, 1);

	const unsigned char *head = (unsigned char*)data;
	const unsigned char *tail = head + len;
	const unsigned char *nal_start = ff_avc_find_startcode_internal(head, tail);
	const unsigned char *nal_end;
	mblk_t *nal;

	ms_queue_init(queue);

	for (;; ) {
		while (nal_start < tail && !*(nal_start++));
		if (nal_start == tail)
			break;

		nal_end = ff_avc_find_startcode_internal(nal_start, tail);

		// ��ʱ nal_start --> nal_end
		nal = allocb(nal_end - nal_start+10, 0);	// �������10���ֽ�.
		memcpy(nal->b_wptr, nal_start, nal_end - nal_start);
		nal->b_wptr += nal_end - nal_start;

		// �� mblk ��ӵ� queue ��.
		ms_queue_put(queue, nal);
		
		// ��һ��
		nal_start = nal_end;
	}

	return queue;
}

/** Ӧ�ó����ⲿ���ã�д�� h264 ���ݣ��ŵ������У�
	�� _process() ʱ���ٴ��ݵ���һ�� filter
 */
static int _write_h264(void *c, const void *data, int len, double stamp)
{
	CTX *ctx = (CTX*)c;
	if (len > 0) {
		MSQueue *slices = annexb2queue(data, len);

		// ��ʱ slices �Ѿ��ֽ�Ϊ slice�����浽 fifo �У��ȴ� process() ����.
		ost::MutexLock al(ctx->cs_fifo_);
		ctx->fifo_.push_back(slices);
	}
	return 0;
}

static int _method_get_writer_param(MSFilter *f, void *args)
{
	ZonekeyH264SourceWriterParam *param = (ZonekeyH264SourceWriterParam*)args;
	param->ctx = f->data;
	param->write = _write_h264;
	return 0;
}

static MSFilterMethod _methods [] = 
{
	{ ZONEKEY_METHOD_H264_SOURCE_GET_WRITER_PARAM, _method_get_writer_param, },
	{ 0, 0, },
};

static MSFilterDesc _desc = 
{
	MS_FILTER_PLUGIN_ID,
	"ZonekeyH264Source",
	"zonekey h264 source filter",
	MS_FILTER_OTHER,
	0,
	0,	// no input
	1,	// output
	_init,
	_preprocess,
	_process,
	_postprocess,
	_uninit,
	_methods,
	0,
};

MS2_PUBLIC void zonekey_h264_source_register()
{
	ms_filter_register(&_desc);
	ms_message("zonekey h264 source filter registered!");
}
