#pragma once

#include "stream.h"

/** ʵ�� PCMu ��ʽ��8k, 64kbps, rtp payload type 0
 */
class PCMuStream : public Stream
{
public:
	PCMuStream(int id, const char *desc, bool support_sink = true, bool support_publisher = false);
	~PCMuStream(void);

protected:
	virtual MSFilter *get_source_filter();
	virtual MSFilter *get_sink_filter();

private:
	MSFilter *decoder_, *encoder_;
	MSFilter *resample_in_, *resample_out_;	// ������ 8k ת���� 16k������ǰ�� 16k ת���� 8k
};
