#pragma once
#include <windows.h>
#include "Byteset.h"
#include "System.h"
typedef class Thread;//提前定义
typedef void (*Thread_callback)(Thread*);//线程回调函数类型
//线程状态
enum Thread_status {
	THREAD_STATUS_NONE,// 未启动
	THREAD_STATUS_RUNNING,// 运行中
	THREAD_STATUS_STOPPED,// 已停止
};
//线程模式
enum Thread_type {
	THREAD_TYPE_NONE,//不执行
	THREAD_TYPE_ONE_SHOT,//立即执行一次
	THREAD_TYPE_LOOP,//循环执行,根据优化规则，循环的线程必须等待上一个同源线程执行完后才会再次执行,间隔会根据方法性能变长
};
typedef struct _Thread_info {
	Byteset code;//线程标识符
	Thread_status status;//线程状态
	Thread_callback callback;//线程回调函数
	HANDLE handle;//线程句柄
	Thread_type type;//线程模式
	__int64 interval[3];//定时器间隔,0:间隔，1:时间戳，2:运行中线程
	PVOID none1;//预留(内部处理)
	/**
	* @brief 构造函数
	* @param code 线程标识符,默认NULL
	* @param callback 线程回调函数,默认NULL
	* @param type 线程模式,默认NONE
	* @param interval 定时器间隔,单位毫秒,默认1000ms
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
	std::vector<Thread_info> thread_info_list;//线程信息列表
	PVOID timer;//定时器
	int loop_interval;//定时器间隔
	bool is_running;//定时器是否正在运行
public:
	/**
	 * @brief 构造函数
	 * @param loop_interval 定时器间隔,单位毫秒,默认500ms
	 */
	Thread(int loop_interval = 500);
	~Thread();
	/**
	 * @brief 创建线程
	 * @param code 线程标识符
	 * @param callback 线程回调函数
	 * @param type 线程模式,默认不启动
	 * @param interval 定时器间隔,单位毫秒,默认1000ms
	 * @return 是否成功,失败原因可能是线程标识符已存在,或callback为空
	 */
	bool WINAPI add_thread(Byteset code, Thread_callback callback, Thread_type type = Thread_type::THREAD_TYPE_NONE, __int64 interval = 1000);
	/**
	 * @brief 销毁线程
	 * @param code 线程标识符
	 * @return 是否成功
	 */
	bool WINAPI remove_thread(Byteset code);
	/**
	 * @brief 启动线程
	 * @param code 线程标识符
	 * @param join 是否等待线程结束,默认false
	 * @return 是否成功,失败则找不到线程或已经在运行
	 */
	bool WINAPI start_thread(Byteset code, bool join = false);
	/**
	 * @brief 停止线程(结束线程)
	 * @param code 线程标识符
	 * @return 是否成功,失败则找不到线程或已经停止
	 */
	bool WINAPI stop_thread(Byteset code);
}*PThread;