#pragma once
#include <windows.h>
#include "Byteset.h"
#include "System.h"
typedef class Thread;//��ǰ����
typedef void (*Thread_callback)(Thread*);//�̻߳ص���������
//�߳�״̬
enum Thread_status {
	THREAD_STATUS_NONE,// δ����
	THREAD_STATUS_RUNNING,// ������
	THREAD_STATUS_STOPPED,// ��ֹͣ
};
//�߳�ģʽ
enum Thread_type {
	THREAD_TYPE_NONE,//��ִ��
	THREAD_TYPE_ONE_SHOT,//����ִ��һ��
	THREAD_TYPE_LOOP,//ѭ��ִ��,�����Ż�����ѭ�����̱߳���ȴ���һ��ͬԴ�߳�ִ�����Ż��ٴ�ִ��,�������ݷ������ܱ䳤
};
typedef struct _Thread_info {
	Byteset code;//�̱߳�ʶ��
	Thread_status status;//�߳�״̬
	Thread_callback callback;//�̻߳ص�����
	HANDLE handle;//�߳̾��
	Thread_type type;//�߳�ģʽ
	__int64 interval[3];//��ʱ�����,0:�����1:ʱ�����2:�������߳�
	PVOID none1;//Ԥ��(�ڲ�����)
	/**
	* @brief ���캯��
	* @param code �̱߳�ʶ��,Ĭ��NULL
	* @param callback �̻߳ص�����,Ĭ��NULL
	* @param type �߳�ģʽ,Ĭ��NONE
	* @param interval ��ʱ�����,��λ����,Ĭ��1000ms
	*/
	_Thread_info(Byteset code = NULL, Thread_callback callback = NULL, Thread_type type = Thread_type::THREAD_TYPE_NONE, __int64 interval = 1000)
		: status(Thread_status::THREAD_STATUS_NONE), handle(NULL),
		none1(NULL){
		this -> code = code;
		this -> callback = callback;
		this -> type = type;
		this -> interval[0] = interval;
	}
} Thread_info, *PThread_info;
typedef class Thread {
public:
	std::vector<Thread_info> thread_info_list;//�߳���Ϣ�б�
	PVOID timer;//��ʱ��
	int loop_interval;//��ʱ�����
	bool is_running;//��ʱ���Ƿ���������
public:
	/**
	 * @brief ���캯��
	 * @param loop_interval ��ʱ�����,��λ����,Ĭ��500ms
	 */
	Thread(int loop_interval = 500);
	~Thread();
	/**
	 * @brief �����߳�
	 * @param code �̱߳�ʶ��
	 * @param callback �̻߳ص�����
	 * @param type �߳�ģʽ,Ĭ�ϲ�����
	 * @param interval ��ʱ�����,��λ����,Ĭ��1000ms
	 * @return �Ƿ�ɹ�,ʧ��ԭ��������̱߳�ʶ���Ѵ���,��callbackΪ��
	 */
	bool WINAPI add_thread(Byteset code, Thread_callback callback, Thread_type type = Thread_type::THREAD_TYPE_NONE, __int64 interval = 1000);
	/**
	 * @brief �����߳�
	 * @param code �̱߳�ʶ��
	 * @return �Ƿ�ɹ�
	 */
	bool WINAPI remove_thread(Byteset code);
	/**
	 * @brief �����߳�
	 * @param code �̱߳�ʶ��
	 * @param join �Ƿ�ȴ��߳̽���,Ĭ��false
	 * @return �Ƿ�ɹ�,ʧ�����Ҳ����̻߳��Ѿ�������
	 */
	bool WINAPI start_thread(Byteset code, bool join = false);
	/**
	 * @brief ֹͣ�߳�(�����߳�)
	 * @param code �̱߳�ʶ��
	 * @return �Ƿ�ɹ�,ʧ�����Ҳ����̻߳��Ѿ�ֹͣ
	 */
	bool WINAPI stop_thread(Byteset code);
}*PThread;