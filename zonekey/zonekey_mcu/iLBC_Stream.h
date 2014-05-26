#pragma once

#include "stream.h"

class iLBC_Stream : public Stream
{
public:
	iLBC_Stream(int id, const char *desc, bool support_sink = true, bool support_publisher = false);
	~iLBC_Stream(void);

protected:
	virtual MSFilter *get_source_filter();
	virtual MSFilter *get_sink_filter();

private:
	MSFilter *decoder_, *encoder_;
	MSFilter *resample_in_, *resample_out_;	// ������ 8k ת���� 16k������ǰ�� 16k ת���� 8k
};
