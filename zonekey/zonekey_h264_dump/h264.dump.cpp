#include <stdio.h>
#include <stdlib.h>
#include <mediastreamer2/mediastream.h>
#include <ortp/ortp.h>
#include <mediastreamer2/rfc3984.h>
#include <mediastreamer2/zk.h264.dump.h>

namespace
{
struct Ctx
{
	Rfc3984Context *rfc3984_;
	void (*push_)(void *opaque, const void *data, int len, double stamp, int key);
	void *opaque_;
	uint8_t *bitstream_;
	int bitstream_size_, bitstream_buf_;

	Ctx()
	{
		push_ = 0;
		rfc3984_ = rfc3984_new();
		rfc3984_set_mode(rfc3984_, 1);
		bitstream_ = (uint8_t*)malloc(128*1024);
	}

	~Ctx()
	{
		rfc3984_destroy(rfc3984_);
		free(bitstream_);
	}

	void process(MSFilter *f)
	{
		// �� filter �����߳��е��ã�
		// �� input ��ȡ rtp ����ʹ�� rfc3984 ������ַ��� push()
		MSQueue nalus;
		ms_queue_init(&nalus);

		mblk_t *m = ms_queue_get(f->inputs[0]);
		while (m) {
			double stamp = mblk_get_timestamp_info(m);
			rfc3984_unpack(rfc3984_, m, &nalus);
			if (!ms_queue_empty(&nalus)) {
				// ������ slices��
				void *bits;
				int need_init, key;
				int size = nalus2frame(&nalus, &bits, &need_init, &key);
				if (size > 0 && push_) {
					push_(opaque_, bits, size, stamp, key);
				}
			}
			m = ms_queue_get(f->inputs[0]);
		}
	}

	// �� nalus ��� frame �ֽ�����annexb ��ʽ
	int nalus2frame(MSQueue *nalus, void **bits, int *need_init, int *key)
	{
		mblk_t *m;
		uint8_t *dst = bitstream_;				// ����
		uint8_t *end = dst + bitstream_buf_;	// �������
		*need_init = 0;
		*key = 0;

		while (m = ms_queue_get(nalus)) {
			uint8_t *src = m->b_rptr;
			int nal_len = m->b_wptr - src;

			if (dst + nal_len + 32 > end) {
				// ��Ҫ��չ
				int pos = dst - bitstream_;
				int exp = (pos + nal_len + 32 + 4095) / 4096 * 4096;
				bitstream_ = (uint8_t*)realloc(bitstream_, exp);
				end = bitstream_ + exp;
				dst = pos + bitstream_;
			}

			if (src[0] == 0 && src[1] && src[2] == 0 && src[3] == 1) {
				// �Ǻǣ���Ӧ�ó�����������˵������ʱ��û�д� annexb ת��
				memcpy(dst, src, nal_len);
				dst += nal_len;
			}
			else {
				// �� nal ���ݸ��Ƶ� dst����� 00 00 00 01
				uint8_t type = (*src) & ((1 << 5) - 1);
				if (type == 7) {
					// TODO: sps������Ƿ�仯������仯�������� need_init
					*key = 1;
				}
				if (type == 8) {
					// TODO: pps������Ƿ�仯������仯�������� need_init
					*key = 1;
				}
				if (type == 5) {
					// IDR frame
					*key = 1;
				}

				// �������� 00 00 00 01
				*dst++ = 0;
				*dst++ = 0;
				*dst++ = 0;
				*dst++ = 1;

				*dst++ = *src++;	// ���� nal_type

				while (src < m->b_wptr - 3) {
					if (src[0] == 0 && src[1] == 0 && src[2] < 3) {
						// ��Ҫ���⴦��������ֶ�
						*dst++ = 0;
						*dst++ = 0;
						*dst++ = 3;
						src += 2;
					}
					*dst++ = *src++;
				}
				// ѭ��������3���ֽ�
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
			}

			freemsg(m);
		}

		return dst - bitstream_;
	}

	int set_callback(void (*push)(void *opaque, const void *data, int len, double stamp, int key), void *opaque)
	{
		push_ = push;
		opaque_ = opaque;
		return 0;
	}
};
};

static void _init(MSFilter *f)
{
	f->data = new Ctx;
}

static void _uninit(MSFilter *f)
{
	delete (Ctx*)f->data;
	f->data = 0;
}

static void _process(MSFilter *f)
{
	Ctx *ctx = (Ctx*)f->data;
	ctx->process(f);
}

static int _method_set_callback(MSFilter *f, void *args)
{
	Ctx *ctx = (Ctx*)f->data;
	ZonekeyH264DumpParam *param = (ZonekeyH264DumpParam*)args;
	return ctx->set_callback(param->push, param->opaque);
}

static MSFilterMethod _methods [] = 
{
	{ ZONEKEY_METHOD_H264_DUMP_SET_CALLBACK_PARAM, _method_set_callback, },
	{ 0, 0, },
};

static MSFilterDesc _desc = 
{
	MS_FILTER_PLUGIN_ID,
	"ZonekeyH264Dump",
	"zonekey h264 recver and dump",
	MS_FILTER_OTHER,
	0,
	1,
	0,
	_init,
	0,
	_process,
	0,
	_uninit,
	_methods,
	0,
};

void zonekey_h264_dump_register()
{
	ms_filter_register(&_desc);
}
