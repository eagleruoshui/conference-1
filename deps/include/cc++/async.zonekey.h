/**
	ʵ��һ������ proactor ��ģ��

	class MyRecvfrom : public AsyncOperatorRecvfrom
	{
	     virtual void recvfrom_complete (....)
		 {
			// 
		 }
	};

	AsyncOperationProcessor aop;	// 
	
	AsyncHandler *sock = aop.socket(...);
	MyRecvfrom *readop(sock, 1024);	// �첽��������

	aop.async_exec(readop);		// �����첽����


	... ����ȡ���ݺ󣬽����ã�
			readop->recvfrom_complete(&read, result);
 **/
#ifndef _async_zonekey__hh
#define _async_zonekey__hh

#ifndef CCXX_CONFIG_H_
#include <cc++/config.h>
#endif

#include <cc++/socket.h>

#ifdef CCXX_NAMESPACES
namespace ost {
namespace zonekey {
#endif // namespace

// �첽��������
enum AsyncOperationType
{
	ASYNC_READ,
	ASYNC_WRITE,
	ASYNC_RECVFROM,
	ASYNC_SENDTO,
};

/** ����һ���첽��� 
 */
typedef struct AsyncHandler_t AsyncHandler_t;

/** �� AsyncHandler_t ��ȡ��ʵ handler
 */
int __EXPORT async_get_os_handler (AsyncHandler_t *h);

/** ����һ���첽����
 */
class __EXPORT AsyncOperation
{
	AsyncHandler_t *fd_;

#ifdef WIN32
	OVERLAPPED overlapped_;
#endif //

public:
	AsyncOperation (AsyncHandler_t *fd) : fd_(fd) 
	{
#ifdef WIN32
		overlapped_.hEvent = 0;
		overlapped_.Offset = overlapped_.OffsetHigh = 0;
#endif 
	}
	virtual ~AsyncOperation () {}

	virtual AsyncOperationType type() const = 0;
	virtual void complete(int err_code, int bytes) = 0;

	AsyncHandler_t *fd() const { return fd_; }

#ifdef WIN32
	OVERLAPPED *overlapped() { return &overlapped_; }
#endif // 
};

class __EXPORT AsyncOperation_Read : public AsyncOperation
{
	char *buf_, *outer_;
	int expsize_;

public:
	AsyncOperation_Read (AsyncHandler_t *fd, char *buf, int size)
		: AsyncOperation(fd)
	{
		buf_ = 0;
		outer_ = buf;
		expsize_ = size;		
	}
	AsyncOperation_Read (AsyncHandler_t *fd, int size)
		: AsyncOperation(fd)
	{
		buf_ = new char[size];
		outer_ = 0;
		expsize_ = size;
	}
	~AsyncOperation_Read ()
	{
		delete []buf_;
	}
	
	/** �����������أ�err_code = 0, bytesΪʵ�ʵõ����ֽ��������ʧ�ܣ�err_code Ϊ������ 

			@param err_code: 0 �ɹ�������Ϊ����ֵ
			@param buf: ���ݻ���ָ��
			@param bytes: buf����Ч���ݵĳ��ȣ����� socket�����0��˵���Է������ر�����
	 */
	virtual void read_complete (int err_code, char *buf, int bytes) = 0;

	/** ϣ����ȡ���ֽ��� */
	int expsize() const { return expsize_; }
	char *buf() const { return outer_ ? outer_ : buf_; }

private:
	void complete(int err, int bytes)
	{
		read_complete(err, outer_ ? outer_ : buf_, bytes);
	}
	AsyncOperationType type () const { return ASYNC_READ; }
};

class __EXPORT AsyncOperation_Write : public AsyncOperation
{
	const char *buf_;
	int expsize_;
public:
	AsyncOperation_Write (AsyncHandler_t *fd, const char *buf, int size) : AsyncOperation(fd)
	{
		buf_ = buf;
		expsize_ = size;
	}

	virtual void write_complete (int err_code, const char *buf, int buyes) = 0;

	int expsize() const { return expsize_; }
	const char *buf() const { return buf_; }

protected:
	void complete (int err, int bytes)
	{
		write_complete(err, buf_, bytes);
	}
	AsyncOperationType type() const { return ASYNC_WRITE; }
};

class __EXPORT AsyncOperation_Recvfrom : public AsyncOperation
{
	char *buf_, *outer_;
	int expsize_;
	sockaddr_in from_;
	int fromlen_;

public:
	AsyncOperation_Recvfrom (AsyncHandler_t *h, char *buf, int size)
		: AsyncOperation(h)
	{
		buf_ = 0;
		outer_ = buf;
		expsize_ = size;
	}
	AsyncOperation_Recvfrom (AsyncHandler_t *h, int size)
		: AsyncOperation(h)
	{
		outer_ = 0;
		buf_ = new char[size];
		expsize_ = size;
	}
	~AsyncOperation_Recvfrom ()
	{
		delete []buf_;
	}

	virtual void recvfrom_complete (int err_code, char *buf, int recved, sockaddr *from, int fromlen) = 0;

	char *buf() const { return outer_ ? outer_ : buf_; }
	int expsize() const { return expsize_; }
	sockaddr *from() { return (sockaddr*)&from_; }
	int *fromlenp() 
	{ 
		fromlen_ = sizeof(from_);
		return &fromlen_; 
	}

protected:
	AsyncOperationType type() const { return ASYNC_RECVFROM; }
	void complete (int err_code, int bytes)
	{
		recvfrom_complete(err_code, outer_ ? outer_ : buf_, bytes, (sockaddr*)&from_, fromlen_);
	}
};

class __EXPORT AsyncOperation_Sendto : public AsyncOperation
{
	const char *data_;
	int len_;
	std::string ipv4_;
	int port_;
public:
	AsyncOperation_Sendto (AsyncHandler_t *h, const char *data, int len, const char *ipv4, int port)
		: AsyncOperation(h)
	{
		data_ = data;
		len_ = len;
		ipv4_ = std::string(ipv4);
		port_ = port;
	}

	virtual void send_complete (int err_code) = 0;

	const char *data() const { return data_; }
	int datalen() const { return len_; }
	const char *ipv4() const { return ipv4_.c_str(); }
	int port() const { return port_; }

protected:
	AsyncOperationType type() const { return ASYNC_SENDTO; }
	void complete (int err_code, int bytes)
	{
		send_complete(err_code);
	}
};

/** ��Ӧ proactor �е� AOP
	windows ��ʹ����ɶ˿�ʵ��
	linux ��ʹ�� epoll ģ��
 */
class __EXPORT AsyncOperationProcessor
{
public:
	AsyncOperationProcessor ();
	~AsyncOperationProcessor ();

	/** ����һ���첽��� */
	AsyncHandler_t *socket(int af, int type, int protocol);	// ����һ��� socket ���
	AsyncHandler_t *open_socket_conn (const char *host, const char *service);	// ����һ�� tcp socket���Ѿ�����
	AsyncHandler_t *open_socket_exist (int af);			// ʹ�����е� socket����
	AsyncHandler_t *open_file(const char *filename, const char *mode);

	/** �رվ�� */
	void close (AsyncHandler_t *fd);

	/** ����һ���첽ִ�� */
	int async_exec (AsyncOperation *op);

private:
	void *internal_;
};

#ifdef CCXX_NAMESPACES
}; // zonekey
}; // ost
#endif // namespace

#endif // async_zonekey__hh
