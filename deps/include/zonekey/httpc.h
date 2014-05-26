#ifndef _httpclient_hh_
#define _httpclient_hh_

/** һ���ǳ���ª�� http msg parser
 */

#ifdef __cplusplus
extern "C" {
#endif

enum HttpParserState
{
	HTTP_UNKNOWN,
	HTTP_STARTLINE,
	HTTP_HEADERS,
	HTTP_BODY,
	HTTP_COMPLETE,
	HTTP_CR,
	HTTP_CRLF,
	HTTP_LSW,
};

/** key = value */
struct KeyValue
{
	char *key;
	char *value;
};
typedef struct KeyValue KeyValue;

/** url 
		http://192.168.1.103:7789/sample/get?caller=xxx#1

	TODO: ��չ֧�� ftp://sunkw:sunkw@192.168.1.107/xxx ��ʽ
*/
struct Url
{
	char *schema;	/** http*/
	char *host;		/** 192.168.1.103:7789 */
	char *path;		/** /sample/get */
	char *query_str; /** caller=xxx */
	char *fragment;	/** 1 */

	KeyValue *query;	/** caller=xxx */
	int query_cnt;		/** 1 */
};
typedef struct Url Url;

/** header */
typedef struct KeyValue HttpHeader;

/** message */
struct HttpMessage
{
	/** start line */
	struct {
		/** GET / HTTP/1.0 */
		/** HTTP/1.0 200 OK */
		char *p1;
		char *p2;
		char *p3;
	} StartLine;

	/** headers */
	HttpHeader *headers;
	int header_cnt;

	/** body */
	char *body;
	int bodylen;

	/** private data */
	void *pri_data;
};
typedef struct HttpMessage HttpMessage;

/** ���ַ���, ������ȡ url
 */
Url *httpc_url_create (const char *url);

/** �ͷ� url ռ����Դ */
void httpc_url_release (Url *url);

/** ����һ���� message */
HttpMessage *httpc_Message_create ();

/** �ͷ� */
void httpc_Message_release (HttpMessage *msg);

/** ����message start line */
void httpc_Message_setStartLine (HttpMessage *msg,
		const char *p1, const char *p2, const char *p3);

/** ����/��ȡ/ɾ�� header */
int httpc_Message_setValue (HttpMessage *msg, const char *key, const char *value);
int httpc_Message_getValue (HttpMessage *msg, const char *key, const char **value);
int httpc_Message_delValue (HttpMessage *msg, const char *key);
int httpc_Message_setValue_printf (HttpMessage *msg, const char *key, const char *vfmt, ...);

/** body */
int httpc_Message_appendBody (HttpMessage *msg, const char *body, int len);
void httpc_Message_clearBody (HttpMessage *msg);
int httpc_Message_getBody (HttpMessage *msg, const char **body);

/** ��������message���л�, ��Ҫռ�ö����ֽ� */
int httpc_Message_getLength (HttpMessage *msg);
/** ���л� */
void httpc_Message_encode (HttpMessage *msg, char *buf);

/** ���ؽ��� message �м�״̬ */
enum HttpParserState httpc_Message_state (HttpMessage *msg);
/** ������Ϣ */
HttpMessage *httpc_parser_parse (HttpMessage *saved, 
		const char *data, int len, int *used);

#ifdef __cplusplus
}
#endif

#endif /** httpclient.h */
