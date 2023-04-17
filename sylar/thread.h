//
//  thread.hpp
//  game_server
//
//  Created by derek on 2023/4/10.
//

#ifndef __SYLAR_THREAD_H__
#define __SYLAR_THREAD_H__

#include "mutex.h"
#include <stdio.h>


namespace sylar{


class Thread: Noncopyable{
public:
    typedef std::shared_ptr<Thread> ptr;
    
    Thread(std::function<void()> cb,const std::string& name);
    
    ~Thread();
    
    pid_t getId() const {return m_id;}

	void join();

	static Thread* GetThis();

    /**
    * @brief 线程名称
    */
	const std::string& getName() const { return m_name;}
    
	static const std::string& GetName();
	static void SetName(const std::string& name);
private:
	static void* run(void* arg);
private:
    /// 线程id
    pid_t m_id = -1;
    /// 线程结构
    pthread_t m_thread = 0;
    /// 线程执行函数
    std::function<void()> m_cb;
    /// 线程名称
    std::string m_name;
    /// 信号量
    Semaphore m_semaphore;
    
};


}



#endif /* __SYLAR_THREAD_H__ */


