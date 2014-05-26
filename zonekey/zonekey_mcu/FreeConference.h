#pragma once

#include "Conference.h"
#include <cc++/thread.h>
#include <vector>
#include <assert.h>

/** ���ɻ��飬
		ÿһ·���� source ---> publisher
		ÿһ·ʹ��һ�������� ticker
 */
class FreeConference : public Conference
{
public:
	FreeConference(int id);
	~FreeConference(void);

protected:
	virtual ConferenceType type() const { return CT_FREE; }
	virtual int add_source(Source *s, KVS &params);
	virtual int del_source(Source *s);
	virtual int add_sink(int sid, Sink *s, KVS &params);
	virtual int del_sink(Sink *s);
	
	// should never called
	virtual int add_stream(Stream *s, KVS &params)
	{
		assert(0);
		return -1;
	}

	// should never called
	virtual int del_stream(Stream *s)
	{
		assert(0);
		return -1;
	}

private:
	// ��Ӧ��һ· publisher
	class Graph
	{
		int id_;
		MSTicker *ticker_;
		Source *source_;
		MSFilter *publisher_;
		std::vector<Sink*> sinks_;
		ost::Mutex cs_sinks_;

	public:
		Graph(Source *s, int id);
		~Graph();

		int add_sink(Sink *s);
		int del_sink(Sink *s);
		bool has_sink(Sink *s);
		int id() const;
		int sink_num();
		Source *source();
	};

	typedef std::vector<Graph *> GRAPHICS;
	GRAPHICS graphics_;
	ost::Mutex cs_graphics_;

	// ���� sid �ҵ�ƥ��� Graph
	Graph *find_graph(int sid);
};
