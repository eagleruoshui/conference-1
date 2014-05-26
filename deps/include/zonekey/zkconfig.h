/** �˽ӿ����ڷ������ã�����ʱ��������

		ģ�� getenv()/putenv() �����������������˻����������ÿ��Զ�Ӧ����ʵ��

		��Ϊ�̰߳�ȫ�⣬����api�����⣡

		
 */

#ifndef _zkconfig__hh
#define _zkconfig__hh

#ifdef __cplusplus
extern "C" {
#endif // c++

typedef struct zkconfig_t zkconfig_t;

/** ���ļ����� zkconfig_t ʵ���������ļ�Ϊ���͵� key=value ��ʽ
 */
zkconfig_t *zkcfg_openfile (const char *filename);

/** ����һ���յ� zkconfig_t ʵ��
 */
zkconfig_t *zkcfg_open ();

/** �ͷ�ʵ�� */
void zkcfg_close (zkconfig_t *cfg);

/** ��ȡ���ã��粻���ڣ����� null
 */
const char *zkcfg_get (zkconfig_t *cfg, const char *key);

/** �޸����ã��粻����key���򴴽�
 */
void zkcfg_set (zkconfig_t *cfg, const char *key, const char *value);

/** ��ȡ���ã��粻���ڣ�����ͨ�� getenv() ��ȡ
 */
const char *zkcfg_getenv (zkconfig_t *cfg, const char *key);

/** dump �������õ��ļ� */
int zkcfg_savefile (zkconfig_t *cfg, const char *filename);

#ifdef __cplusplus
}
#endif // c++

#endif // zkconfig.h
