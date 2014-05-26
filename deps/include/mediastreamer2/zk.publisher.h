/** ���� tee + rtp sender filters��������Ҫֹͣ����ͼ :-)

               ---------------------
    h264       |                    |
	   ------->|  zonekey.publisher |
	speex      |                    |
			   ---------------------
			             ^
						 |               add_remote(RtpSession *)
						 --------------  del_remote(RtpSession *)


		���͵ģ���ӵ� RtpSession ������ client �������ݣ���� client �� NAT ֮����������£������� socket �յ� client �����ݺ󣬲���֪�� client �� ip:port
		���Ե��� add_remote ʱ��һ������£��޷���ȷ��д remote_addr_and_port �ģ���������

		�� publisher ���ڲ�ʵ���У��� add_remote(RtpSession *) ���ڲ��������� RtpSession �� rtp socket�� rtcp socket �� recvfrom()����ʱ client �ڽ��ܵ�ͬʱ��Ҳ
		��Ҫ�����ض��İ������ recvfrom() �յ����ݣ����ʱ�� addr ���� client ��ͨѶ��ַ�ˣ��ټӵ� RtpSession �У�

		��Ϊ������ⲿû��ָ�� remote_ip_and_port�����ڲ�ִ�� recvfrom()����� RtpSession �Ѿ����� addr and port����ֱ�Ӽ��� sending list!!!!!

		ʹ�ò��裺

				zonekey_publisher_register();
				.....

				MSFilter *pubisher = ms_filter_new_from_name("ZonekeyPubisher");
				ms_fiter_link(source_filter, 0, publisher, 0);
				ms_ticker_attach(ticker, publisher);
				.....


				RtpSession *rs = rtp_session_new(RTP_SESSION_SENDONLY);
				rtp_session_set_payload_type(....
				rtp_session_set_local_addr(rs, myip, 0, 0);	// ʹ����� rtp/rtcp port����ʱ socket ���Ѿ�����
				int rtp_port = rs->rtp.loc_port, rtcp_port = rs->rtcp.loc_port; // �� rtp_port �� rtcp_port �� client��client ��Ҫ������������ݣ����ڴ򶴣�
				ms_filter_call_method(publisher, ZONEKEY_METHOD_PUBISHER_ADD_REMOTE, rs);
				......

				�� publisher �� add_remote() �ڲ�ʵ�֣�
					��ֱ�ӽ��� rs ���뷢���б����Ǽ��� pending �б�
						rtp_sock = rtp_session_get_rtp_socket(), rtcp_socket = rtp_session_get_rtcp_socket();
						
						�� process() �У����ÿ�� pending �б��� socket ���� ::recvfrom()����� rtp, rtcp ���õ� remote addr �ˣ������
						rtp_session_set_remote_addr_port()�����ҽ� rs �� pending �б����ƶ��������б��С�

				client �� rtp session ��Ҫ����Ϊ RTP_SESSION_SENDRECV ģʽ�����յ� publisher �� rtp port, rtcp port �󣬵��� rtp_session_add_remote_addr_port() 
				���뷢���б��������ڷ���һЩ���ݼ��ɡ�

 */

#ifndef zonekey_publisher__hh
#define zonekey_publisher__hh

#include "mediastream.h"
#include "allfilters.h"

#ifdef __cplusplus
extern "C" {
#endif // c++

MS2_PUBLIC void zonekey_publisher_register();
MS2_PUBLIC void zonekey_publisher_set_log_handler(void (*func)(const char *, ...));

// ϣ�����ͻ�� :)
#define ZONEKEY_ID_PUBLISHER (MSFilterInterfaceBegin+13)

// ���ɾ�������ߣ�һ�������߾���һ�� RtpSession��ice Э��֮��ģ���Ҫ���ⲿ�㶨��publisher �������ɵɵ�ص��� rtp_session_sendm_with_ts()
#define ZONEKEY_METHOD_PUBLISHER_ADD_REMOTE				MS_FILTER_METHOD(ZONEKEY_ID_PUBLISHER, 0, void*)
#define ZONEKEY_METHOD_PUBLISHER_DEL_REMOTE				MS_FILTER_METHOD(ZONEKEY_ID_PUBLISHER, 1, void*)

// ���� payload type
#define ZONEKEY_METHOD_PUBLISHER_SET_PAYLOAD			MS_FILTER_METHOD(ZONEKEY_ID_PUBLISHER, 2, int)

#ifdef __cplusplus
}
#endif // c++

#endif 
