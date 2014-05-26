/** server.h ���� zkrobot_ex��ʹ�÷������� acs
 */

#pragma once

#include <zonekey/zkrobot_mse_ex.h>
#include <mediastreamer2/mediastream.h>
#include "Conference.h"

#define MSE_SERVICE_TYPE "acs"

// FIXME: ���Ӧ��ʹ��������mac��ַ����Щ.
#define MSE_SERVICE_NAME "acs"

// ������������ xmpp_domain ����.
#define MSE_DOMAIN "app.zonekey.com.cn"

class Server
{
public:
	Server();
	~Server();

	// ִ�в�������ֱ���յ� ctrl+c �˳�. 
	void run();

private:
	zkrobot_mse_t *mse_;

	// Ŀǰ����һ�� conference ��.
	Conference *conference_;

	static void _mse_command(zkrobot_mse_t *mse_ins, const char *remote_jid, const char *str_cmd, const char *str_option, 
	                int is_delay, void *userdata, void *token);
	static void _mse_login(zkrobot_mse_t * mse_ins, int is_successful, void *userdata);
};
