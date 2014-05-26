#include <stdio.h>
#include <stdlib.h>
#include <mediastreamer2/mediastream.h>
#include <mediastreamer2/zk.video.mixer.h>
#include <mediastreamer2/rfc3984.h>
#include <cc++/thread.h>
#include <assert.h>
#include <deque>
#include <vector>
#include <zonekey/zqsender.h>
extern "C" {
#	include <libswscale/swscale.h>
}

#include "Canvas.h"
#include "InputSource.h"
#include "X264.h"

static void (*_log)(const char *, ...) = 0;

// ���� audio mixer
#define MIXER_MAX_CHANNELS ZONEKEY_VIDEO_MIXER_MAX_CHANNELS

// ָ��Ԥ�� output pin
#define PREVIEW_PIN MIXER_MAX_CHANNELS

// ָ�� zqpkt ���
#define ZQPKT_PIN PREVIEW_PIN+1

static double time_now()
{
	struct timeval tv;
	ost::gettimeofday(&tv, 0);
	return tv.tv_sec + tv.tv_usec / 1000000.0;
}

/** TODO List:
		version 0.1: 
			1. process() �����ݻ��浽 InputChannel�����쵽ָ����С��
			2. mixer thread ���� zorder���� InputChannel ��ȡ�� yuv ���ݣ����� InputChannel ������( x, y, alpha...������������
			   �������У�ͨ�� x264 ѹ��������ʹ�� rfc3984 ��������浽��Ӧ�� outputs �У�

		version 0.2: δ����
			���ƴ��ڹ��������ػ�û�б����ǵĲ��֣����������Ч�ʣ���������������Ƶ����һ������£���������Ƶ���ǵ�����ģ���ʹ�У�
			����Ҳ��С���ڵĸ��ǣ���Ч�ʵ�ʵ��Ӱ�죬��Ҫ��������������

		version 0.3: 
			v0.1 �У���mixer thread ��ֱ��д�� filter_->outputs �ǲ��Եģ���Ϊ�������������߳�ͬʱ���� output queue �ľ�����
			��0.3�и�Ϊʹ�� fifo_���������� MS_FILTER_IS_PUMP ѡ��

		version 0.4: zonekey.mixer.video ��ʵ����Ҫ���ÿ�� input ��Ӧһ�� output��Ӧ��������� outputs��
			һ��Ϊ h264 һ������ tee��һ��Ϊ yuv һ������ yuv sink
 */

#define VERSION "0.4.1"

namespace
{
	/** ��������
	 */
	struct RECT
	{
		int x, y;
		int width, height;
	};

	/** ��Ӧһ����Ƶ�������ͬʱ֧�� MIXER_MAX_CHANNELS ����������ÿ¼����Դ��
				input: [yuv420p, width, height, x, y, dx, dy, alpha] ÿ¼������� yuv ���ݣ� ���������ϵ�λ�ã�͸����
				output: outputs[0]: [yuv420p] ��Ϻ�� yuv ���ݣ�����Ԥ�ര��
				        outputs[1]: [h264 rtp profile] ���Ϊ���������� h264 ѹ����������ʹ�� rfc3984 ���������ֱ������ rtp sender ����

			mixer ����Ӧ���и� ticker��
			    ÿ�� channel д��󣬱��浽��Ӧ�Ļ����У��� ticker �У����� Z-order���������е�ͼ�񻭵������ϣ�ѹ����Ȼ�������ÿ�� channel ��Ӧ�� output ��
	 */
	class CTX : ost::Thread
	{
		X264 *x264_;	// encoder
		InputSource *channels_[ZONEKEY_VIDEO_MIXER_MAX_CHANNELS+1];		// ���п���ʹ�õ�
		std::deque<MSQueue*> fifo_; // ���ڱ��� mixer thread ��������ݣ�process() ����ȡ��
		ost::Mutex cs_fifo_;
		int zorder_[ZONEKEY_VIDEO_MIXER_MAX_CHANNELS+1];	// ��¼ zorder�������Ȼ� zorder_[0], .....
														// Ԫ��Ϊ channal id
		Canvas *canvas_;		// ����
		ost::Mutex cs_channels_;	// ���� channels �仯
		ost::Event evt_req_, evt_res_; // ���� reload() 
		int req_, res_;		// ��������������Ϣ
		bool quit_;	// ����Ƿ� attach �� ticker

		ZonekeyVideoMixerZOrder t_channel_zorder_;	// ���ڴ��ݵ������߳�
		ZonekeyVideoMixerEncoderSetting t_encoder_desc;	// 

		MSFilter *filter_;	// ����mixer thread���� filter ����
		int want_mode_;		

		bool has_pending_;	// �Ƿ�����Ҫ�ύ�� output ������

		ZonekeyVideoMixerStats stat_;
		int enabled_;	// �Ƿ�����

#ifdef _DEBUG
		void save_yuv_file(const char *prefix, MSPicture pic)
		{
			return;
			char filename[128];
			snprintf(filename, sizeof(filename), "%s%dx%d.yuv", prefix, pic.w, pic.h);
			FILE *fp = fopen(filename, "wb");
			if (fp) {
				unsigned char *y = pic.planes[0];
				for (int i = 0; i < pic.h; i++) {
					fwrite(y, 1, pic.w, fp);
					y += pic.strides[0];
				}

				y = pic.planes[1];
				for (int i = 0; i < pic.h/2; i++) {
					fwrite(y, 1, pic.w/2, fp);
					y += pic.strides[1];
				}

				y = pic.planes[2];
				for (int i = 0; i < pic.h/2; i++) {
					fwrite(y, 1, pic.w/2, fp);
					y += pic.strides[2];
				}

				fclose(fp);
			}
		}
#endif // 

		MSPicture avpic_mspic(AVPicture *pic, int w, int h)
		{
			MSPicture p;
			p.w = w, p.h = h;
			for (int i = 0; i < 3; i++) {
				p.planes[i] = pic->data[i];
				p.strides[i] = pic->linesize[i];
			}

			return p;
		}

		void run()
		{
			/** �����̣߳�����ÿ��ѭ����ʵ�����ĵ�ʱ�䣬��̬������һ�εȴ���ʱ�䳤�ȣ������������֡��

					FIXME: ����ʹ�ü�֡��ƽ��ֵ��������Щ��
			 */

			canvas_ = 0;
			x264_ = 0;

			int to_wait = 40;	// ȱʡ 25fps
			int delta = 0;		// 

			// ȱʡ������960x540��25fps, 800kbps
			X264Cfg x264_cfg;
			x264_cfg.width = 960;
			x264_cfg.height = 540;
			x264_cfg.fps = 1000.0 / to_wait;
			x264_cfg.kbitrate = 800;
			x264_cfg.gop = 50;

			x264_ = new X264(&x264_cfg);
			canvas_ = new Canvas(x264_cfg.width, x264_cfg.height);

			double last = time_now();
			double begin = last;

			bytes_ = 0;
			last_stamp_ = 0.0;
			first_stamp_ = 0.0;
			totle_bytes_ = 0;
			frames_ = 0;
			total_qp_ = qp_ = 0;

			while (!quit_) {
				if (has_req()) {
					// ���µ���������
					switch (req_) {
					case -1:
						// ��ʱҪ�����
						res_ = -1;
						reply();
						quit_ = true;
						continue;
						break;

					case 1:
						// ��ʱ������ Canvas Encoding ���ԣ�һ����Ҫ�޸� x264, Canvas ...
						res_ = reset(&t_encoder_desc);
						to_wait = (int)(1000.0 / t_encoder_desc.fps);	// ��Ҫ�����ȴ�ʱ��
						break;

					case 3:
						// ��ʱϣ����ȡ canvas encoding ����
						res_ = get_encoding_params(&t_encoder_desc);
						break;

					case 2:
						// ��ʱ��Ҫ���� zorder�����������ʵ�������߳���Ҳû��ϵ
						res_ = rezorder(&t_channel_zorder_);
						break;
					}

					reply();
				}

				if (enabled_)
					one_step(last - begin, delta);	//
				else {
					//fprintf(stderr, "O");
				}

				double curr = time_now();
				int tick = (int)((curr - last) * 1000.0);	// ʵ������ʱ��
				if (tick > to_wait + 1) {
					delta++;	// ̫��ʱ����
				}
				else if (tick < to_wait - 1) {
					delta--;    // ������
				}

				last = curr;

				if (delta > to_wait) delta = to_wait;	// ��ȫ���
				ost::Thread::sleep(to_wait - delta);	// ʵ�ʵȴ���ʱ��
			}

			delete x264_;
			x264_ = 0;

			delete canvas_;
			canvas_ = 0;
		}

		double first_stamp_;
		double last_stamp_;
		unsigned int bytes_;
		unsigned int totle_bytes_;
		unsigned int total_qp_, qp_;
		unsigned int frames_;

		// now Ϊ���������̺߳������
		void one_step(double now, int odelta)
		{
			stat_.time = now;
			stat_.last_delta = odelta;

			unsigned int bytes = 0;
			int active = 0;

			if (!running_) return;

			/** 	�� InputSource ��ȡ�� yuv ���ݣ����� zorder_ ��ϵ���������Ҫ���Ĵ��ڣ����� Canvas ��
					���л���󣬴� Canvas ��ȡ��������ʹ�� x264 ѹ����Ȼ���ٽ�ѹ��������ݷŵ�ÿ�� channels �� output ��
			 */
			// Ŀǰ������򵥵ģ�ֱ�Ӹ��� zorder_ ��˳�����λ���������
			if (!canvas_ || !x264_) return;

			// step 0: ��ջ���
			canvas_->clear();

			// step 1: ���δ� InputSource ��ȡ�������һ���������
			do {
				ost::MutexLock al(cs_channels_);
				for (int i = 0; zorder_[i] != -1; i++) {
					if (channels_[zorder_[i]]) {
						active++;
						InputSource *source = channels_[zorder_[i]];
						YUVPicture pic = source->get_pic();
						if (pic.width > 0 && pic.height > 0) {
							canvas_->draw_yuv(pic.pic.data, pic.pic.linesize, pic.x, pic.y, pic.width, pic.height, pic.alpha);
							avpicture_free(&pic.pic);	// ��Ӧ get_pic() �е� avpicture_alloc()
						}
					}
				}
			} while (0);

			stat_.active_input = active;

			// step 2: �ӻ�����ȡ����ѹ��
			AVPicture pic = canvas_->get_pic();
			X264FrameState state;
			MSQueue nals;
			mblk_t *m;
			ms_queue_init(&nals);

			x264_->encode(pic.data, pic.linesize, &nals, &state);
			bytes = state.bytes;

			stat_.sent_bytes += bytes;
			stat_.sent_frames ++;
			qp_ += state.qp;

#ifdef _DEBUG
			//static int __cnt = 0;
			//__cnt++;
			//if (__cnt % 100 == 0) {
			//	MSPicture mspic = avpic_mspic(&pic, canvas_->width(), canvas_->height());
			//	char filename_prefix[1024];
			//	snprintf(filename_prefix, sizeof(filename_prefix), "canvas_%d_", __cnt/100);
			//	fprintf(stderr, "%s.yuv saved\n", filename_prefix);
			//	save_yuv_file(filename_prefix, avpic_mspic(&pic, canvas_->width(), canvas_->height()));
			//}
#endif // 

			// step 3: �� h264 nals ���Ϊ rtp profile
			MSQueue *rtps = ms_new(MSQueue, 1);	// rtps ���浽 fifo �У��� process() ʱȡ�����ͷ�
			ms_queue_init(rtps);
			x264_->rfc3984_pack(&nals, rtps, (uint32_t)(now*90000.0));	// �����ݴ��ݵ� rtps queue �ˣ�rfc3984�����Ҫ�� 90khz

			// step 4: �� rtps д�� fifo_ �У����� process() �У��� fifo_ ��ȡ����д�� outputs[1]
			do {
				ost::MutexLock al(cs_fifo_);
				fifo_.push_back(rtps);
			} while (0);


			has_pending_ = true;

			bytes_ += bytes;
			totle_bytes_ += bytes;

			if (state.nals > 0) {
				frames_++;
				total_qp_ += state.qp;
			}

			if (now < first_stamp_ + 1.0) // ��ʱ�����ǲ��Եģ��� 1 �����˵
				return;
			double delta = now - last_stamp_;
			if (delta > 5.0) {
				stat_.last_fps = frames_ / delta;
				stat_.last_kbps = bytes_ / delta;
				stat_.last_qp = qp_ * 1.0 / frames_;

				//fprintf(stderr, "[mixer thread]: delta=%d, qp=%d, lfps=%.2f, afps=%.3f, lbr=%.2fkbps: abr=%.3fkbps, aqp=%.2f\n", 
				//	odelta, state.qp, stat_.last_fps, stat_.avg_fps,
				//	bytes_/delta/125.0, totle_bytes_/(now - first_stamp_)/125.0, qp_*1.0/frames_);

				last_stamp_ = now;

				bytes_ = 0;
				frames_ = 0;
				qp_ = 0;
			}

			stat_.avg_fps = stat_.sent_frames / (now - first_stamp_);	// ��ƽ��
			stat_.avg_kbps = stat_.sent_bytes / (now - first_stamp_);  // ��ƽ��

			if (now < 5.0) {
				stat_.last_fps = stat_.avg_fps;
				stat_.last_kbps = stat_.avg_kbps;
			}
		}

		// �����̵߳��ã�����Ƿ�������
		bool has_req()
		{
			bool rc = evt_req_.wait(0);
			if (rc)
				evt_req_.reset();
			return rc;
		}

		// �����̵߳��ã���ִ���� req ��֪ͨ���߳�
		void reply()
		{
			evt_req_.reset();
			evt_res_.signal();	// ִ֪ͨ�����
		}

		// ���̵߳��ã������̷߳�����Ϣ���ȴ������̴߳�������󣬷���ִ�н��
		int req(int cmd)
		{
			// �������ã����ҵȴ��������
			req_ = cmd;
			evt_req_.signal();
			evt_res_.wait();
			evt_res_.reset();

			return res_;
		}

		void swap(int *a, int *b)
		{
			int t = *a;
			*a = *b;
			*b = t;
		}

		// ���� zorder���ɹ����� 0
		int rezorder(ZonekeyVideoMixerZOrder *desc)
		{
			ost::MutexLock al(cs_channels_);

			if (desc->id < 0 || desc->id >= ZONEKEY_VIDEO_MIXER_MAX_CHANNELS)	// ��Чid
				return -1;

			if (desc->order_oper == ZONEKEY_VIDEO_MIXER_ZORDER_UP) {
				// ��ǰ��ʵ����������һ������λ��
				for (int i = 0; zorder_[i+1] != -1; i++) {
					if (zorder_[i] == desc->id) {
						swap(&zorder_[i], &zorder_[i+1]);
						break;
					}
				}
			}
			else if (desc->order_oper == ZONEKEY_VIDEO_MIXER_ZORDER_DOWN) {
				// �ú�ʵ������ǰһ������λ��
				int i = 0;
				for (; zorder_[i] != -1; i++) {
					if (zorder_[i] == desc->id)
						break;
				}

				if (i > 0 && zorder_[i] != -1)
					swap(&zorder_[i-1], &zorder_[i]);
			}
			else if (desc->order_oper == ZONEKEY_VIDEO_MIXER_ZORDER_TOP) {
				// �ö����ŵ����е������
				for (int i = 0; zorder_[i+1] != -1; i++) {
					if (zorder_[i] == desc->id) {
						swap(&zorder_[i], &zorder_[i+1]);
					}
				}
			}
			else if (desc->order_oper == ZONEKEY_VIDEO_MIXER_ZORDER_BOTTOM) {
				// �õף�ʵ�����Ƿŵ���ǰ��
				int i = 0;
				for (; zorder_[i] != -1; i++) {
					if (zorder_[i] == desc->id)
						break;
				}

				if (zorder_[i] != -1) {
					// ����
					for (int j = 0; j < i; j++)
						zorder_[j+1] = zorder_[j];

					zorder_[0] = desc->id;
				}
			}

			return 0;
		}

		// �������� canvas �� h264 ���ԣ�����ɹ������� 0��ʧ�ܷ��� -1
		int reset(ZonekeyVideoMixerEncoderSetting *setting)
		{
			// �Ƿ���Ҫ���� Canvas
			if (canvas_ && (canvas_->width() != setting->width || canvas_->height() != setting->height)) {
				delete canvas_;
				canvas_ = 0;
			}

			if (!canvas_) 
				canvas_ = new Canvas(setting->width, setting->height);

			if (x264_) {
				delete x264_;
				x264_ = 0;
			}

			X264Cfg cfg;
			cfg.width = setting->width;
			cfg.height = setting->height;
			cfg.kbitrate = setting->kbps;
			cfg.fps = setting->fps;
			cfg.gop = setting->gop;

			x264_ = new X264(&cfg);

			return 0;
		}

		int get_encoding_params(ZonekeyVideoMixerEncoderSetting *setting)
		{
			if (canvas_) {
				setting->width = canvas_->width();
				setting->height = canvas_->height();
			}
			else {
				setting->width = setting->height = 0;
			}

			if (x264_) {
				setting->fps = x264_->cfg()->fps;
				setting->kbps = x264_->cfg()->kbitrate;
				setting->gop = x264_->cfg()->gop;
			}

			return 0;
		}

	public:
		bool running_;

		CTX(MSFilter *f)
			: filter_(f)
		{
			for (int i = 0; i <= ZONEKEY_VIDEO_MIXER_MAX_CHANNELS; i++) {
				zorder_[i] = -1;	// ��Ч������
				channels_[i] = new InputSource;
			}

			has_pending_ = false;
			running_ = false;
			enabled_ = -1;

			memset(&stat_, 0, sizeof(stat_));

			// ���������߳�
			quit_ = false;
			start();
		}

		~CTX()
		{
			req(-1);	// Ҫ�����
			join();

			for (int i = 0; i <= ZONEKEY_VIDEO_MIXER_MAX_CHANNELS; i++) {
				delete channels_[i];
			}
		}

		int get_channel()
		{
			for (int i = 0; i < ZONEKEY_VIDEO_MIXER_MAX_CHANNELS; i++) {
				if (channels_[i]->idle()) {
					channels_[i]->employ();

					ost::MutexLock al(cs_channels_);

					// �½��������Ƿ��� zorder ��������
					int z;
					for (z = 0; zorder_[z] != -1; z++){}
					zorder_[z] = i;

					return i;
				}
			}
			return -1;	// �Ҳ������У�������
		}

		// �ͷ�ָ���� channel
		int free_channel(int cid)
		{
			if (cid >= 0 && cid < ZONEKEY_VIDEO_MIXER_MAX_CHANNELS) {
				ost::MutexLock al(cs_channels_);
	
				channels_[cid]->disemploy();	// �ͷţ�����

				// �� zorder_ ��ɾ��
				int i;
				for (i = 0; i < ZONEKEY_VIDEO_MIXER_MAX_CHANNELS; i++) {
					if (zorder_[i] == cid)
						break;
				}

				// ��ʱ i ָ����Ҫɾ���Ľڵ㣬ֱ��ʹ�ú���ĸ���ǰ��ļ���
				for (; zorder_[i] != -1; i++) {
					zorder_[i] = zorder_[i+1];
				}
				zorder_[i] = -1;

				return 0;
			}
			else
				return -1;
		}

		// ���� channel ���ԣ���������Ƕ�ִ̬�еģ��細�ڴ�С�仯֮���
		int set_channel_desc(ZonekeyVideoMixerChannelDesc *desc)
		{
			// ֱ�����õ���Ӧ�� InputSource �ϼ���
			ost::MutexLock al(cs_channels_);

			if (desc->id < 0 || desc->id >= ZONEKEY_VIDEO_MIXER_MAX_CHANNELS)
				return -1;

			if (channels_[desc->id]->idle())
				return -1;

			channels_[desc->id]->set_param(desc->x, desc->y, desc->width, desc->height, desc->alpha);
			return 0;
		}

		int get_channel_desc(ZonekeyVideoMixerChannelDesc *desc)
		{
			ost::MutexLock al(cs_channels_);

			if (desc->id < 0 || desc->id >= ZONEKEY_VIDEO_MIXER_MAX_CHANNELS)
				return -1;

			if (channels_[desc->id]->idle()) {
				desc->x = desc->y = desc->width = desc->height = desc->alpha = 0;
			}
			else {
				InputSource *s = channels_[desc->id];
				// FIXME: ����ով��� set channel desc�����ȡ x()... ���������ֱ�ӻ�ȡ wanted_x
				//desc->x = s->x();
				//desc->y = s->y();
				//desc->width = s->width();
				//desc->height = s->height();
				//desc->alpha = s->alpha();

				desc->x = s->want_x();
				desc->y = s->want_y();
				desc->width = s->want_width();
				desc->height = s->want_height();
				desc->alpha = s->want_alpha();
			}

			return 0;
		}

		int get_zorder_array(ZonekeyVideoMixerZorderArray *arr)
		{
			ost::MutexLock al(cs_channels_);
			for (int i = 0; i < ZONEKEY_VIDEO_MIXER_MAX_CHANNELS + 1; i++)
				arr->orders[i] = zorder_[i];

			return 0;
		}

		// �޸� channel �� z-order
		int set_zorder(ZonekeyVideoMixerZOrder *desc)
		{
			t_channel_zorder_ = *desc;
			return req(2);	// ���ڹ����߳���ִ�� rezorder()
		}

		// ���� x264 �������ԣ�֧���м��޸� :)
		int set_encoder(ZonekeyVideoMixerEncoderSetting *setting)
		{
			t_encoder_desc = *setting;
			return req(1);	// ���ڹ����߳���ִ�� reset()
		}

		int get_encoder(ZonekeyVideoMixerEncoderSetting *setting)
		{
			req(3);
			*setting = t_encoder_desc;
			return 0;
		}

		void encode_enable(int en)
		{
			enabled_ = en;
		}

		// ����ͳ����Ϣ
		int get_stats(ZonekeyVideoMixerStats *stat)
		{
			*stat = stat_;
			return 0;
		}

		void preprocess()
		{
			// ��� fifo_
			ost::MutexLock al(cs_fifo_);
			while (!fifo_.empty()) {
				MSQueue *q = fifo_.front();
				ms_queue_flush(q);
				ms_free(q);
				fifo_.pop_front();
			}

			x264_->force_key_frame();	// ǿ�ƹؼ�֡
			running_ = true;
		}

		void postprocess()
		{
			running_ = false;
		}

		// ���� process()
		void channel_process(MSFilter *f)
		{
			/** �˵�������ÿ�� rtp recv ���̣߳�

				step 1:
					���� zorder�������������Ҫ�ػ��Ĳ��֣�һ��Ϊһ�������б�
					����Ҫ�ػ��Ĳ���ֱ�ӻ��������ϡ�

				step 2:
					�� fifos_ ��ȡ�� queue������д�� outputs

			 */
			for (int i = 0; i < ZONEKEY_VIDEO_MIXER_MAX_CHANNELS; i++) {
				if (f->inputs[i]) {
					mblk_t *m = ms_queue_get(f->inputs[i]);
					while (m) {
						if (enabled_) {
							MSPicture pic;
							ms_yuv_buf_init_from_mblk(&pic, m);
							channels_[i]->save_pic(&pic);	// ����
						}
						freemsg(m);	// ����ʹ�ã�
						m = ms_queue_get(f->inputs[i]);
					}
				}
			}

			if (!has_pending_) return;

			// ��� fifo_����������ݣ�ȡ�����ӵ� outputs[1] ��
			do {
				ost::MutexLock al(cs_fifo_);
				while (!fifo_.empty()) {
					MSQueue *q = fifo_.front();
					if (f->outputs[1]) {
						mblk_t *m = ms_queue_get(q);
						while (m) {
							ms_queue_put(f->outputs[1], m);	// д�� output[1] 
							m = ms_queue_get(q);
						}
					}
					ms_free(q);	// ��Ӧ�� rtps = ms_new()
					fifo_.pop_front();
				}
			} while (0);

			has_pending_ = false;
		}
	};
};

static void _init(MSFilter *f)
{
	CTX *ctx = new CTX(f);
	f->data = ctx;
}

static void _uninit(MSFilter *f)
{
	CTX *ctx = (CTX*)f->data;
	delete ctx;
	f->data = 0;
}

static void _preprocess(MSFilter *f)
{
	CTX *ctx = (CTX*)f->data;
	ctx->preprocess();
}

static void _postprocess(MSFilter *f)
{
	CTX *ctx = (CTX*)f->data;
	ctx->postprocess();
}

static void _process(MSFilter *f)
{
	CTX *ctx = (CTX*)f->data;
	ctx->channel_process(f);
}

static int _method_get_channel(MSFilter *f, void *args)
{
	CTX *ctx = (CTX*)f->data;
	int cid = ctx->get_channel();
	if (cid >= 0) {
		*(int*)args = cid;
		return 0;
	}
	return -1;
}

static int _method_free_channel(MSFilter *f, void *args)
{
	CTX *ctx = (CTX*)f->data;
	if (ctx->free_channel(*(int*)args) < 0)
		return -1;
	else
		return 0;
}

static int _method_set_channel_desc(MSFilter *f, void *args)
{
	CTX *ctx = (CTX*)f->data;
	return ctx->set_channel_desc((ZonekeyVideoMixerChannelDesc*)args);
}

static int _method_set_channel_zorder(MSFilter *f, void *args)
{
	CTX *ctx = (CTX*)f->data;
	return ctx->set_zorder((ZonekeyVideoMixerZOrder*)args);
}

static int _method_set_encoder(MSFilter *f, void *args)
{
	CTX *ctx = (CTX*)f->data;
	return ctx->set_encoder((ZonekeyVideoMixerEncoderSetting*)args);
}

static int _method_get_encoder(MSFilter *f, void *args)
{
	CTX *ctx = (CTX*)f->data;
	return ctx->get_encoder((ZonekeyVideoMixerEncoderSetting*)args);
}

static int _method_get_stats(MSFilter *f, void *args)
{
	CTX *ctx = (CTX*)f->data;
	return ctx->get_stats((ZonekeyVideoMixerStats*)args);
}

static int _method_get_zorder_array(MSFilter *f, void *args)
{
	CTX *ctx = (CTX*)f->data;
	return ctx->get_zorder_array((ZonekeyVideoMixerZorderArray*)args);
}

static int _method_get_channel_desc(MSFilter *f, void *args)
{
	CTX *ctx = (CTX*)f->data;
	return ctx->get_channel_desc((ZonekeyVideoMixerChannelDesc*)args);
}

static int _method_enable(MSFilter *f, void *args)
{
	CTX *ctx = (CTX*)f->data;
	ctx->encode_enable((int)args);
	return 0;
}

static MSFilterMethod _methods[] = 
{
	{ ZONEKEY_METHOD_VIDEO_MIXER_GET_CHANNEL, _method_get_channel },
	{ ZONEKEY_METHOD_VIDEO_MIXER_FREE_CHANNEL, _method_free_channel },
	{ ZONEKEY_METHOD_VIDEO_MIXER_SET_CHANNEL_DESC, _method_set_channel_desc },
	{ ZONEKEY_METHOD_VIDEO_MIXER_SET_ZORDER, _method_set_channel_zorder },
	{ ZONEKEY_METHOD_VIDEO_MIXER_SET_ENCODER_SETTING, _method_set_encoder },
	{ ZONEKEY_METHOD_VIDEO_MIXER_GET_ENCODER_SETTING, _method_get_encoder },
	{ ZONEKEY_METHOD_VIDEO_MIXER_GET_STATS, _method_get_stats },
	{ ZONEKEY_METHOD_VIDEO_MIXER_GET_ZOEDER, _method_get_zorder_array },
	{ ZONEKEY_METHOD_VIDEO_MIXER_GET_CHANNEL_DESC, _method_get_channel_desc },
	{ ZONEKEY_METHOD_VIDEO_MIXER_ENABLE, _method_enable },
	{ 0, 0, },
};

static MSFilterDesc _desc = 
{
	MS_FILTER_PLUGIN_ID,
	"ZonekeyVideoMixer",
	"zonekey video mixer filter",
	MS_FILTER_OTHER,
	0,
	MIXER_MAX_CHANNELS,		// inputs
	2,	// outputs: 0: yuv Ԥ�������1: h264 �������
	_init,
	_preprocess,
	_process,
	_postprocess,
	_uninit,
	_methods,
	MS_FILTER_IS_PUMP,   // ��Ϊ _process() ��ֱ�ӽ� input �����ݴ��� output �У����Ǽ�� fifos_ ���Ƿ�������
};

void zonekey_video_mixer_register()
{
	ms_filter_register(&_desc);
}

void zonekey_video_mixer_set_log_handler(void (*func)(const char *, ...))
{
	_log = func;
}
