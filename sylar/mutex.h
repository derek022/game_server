//
//  mutex.hpp
//  game_server
//
//  Created by derek on 2023/4/10.
//

#ifndef __SYLAR_MUTEX_H__
#define __SYLAR_MUTEX_H__
#include <stdexcept>
#include <pthread.h>
#include <memory>
#include <functional>
#include <string>
#include <semaphore.h>

#include "noncopyable.h"

namespace sylar{


class Semaphore : Noncopyable{
public:
    /**
     * @brief 构造函数
     * @param[in] count 信号量值的大小
     */
    Semaphore(uint32_t count = 0);

    /**
     * @brief 析构函数
     */
    ~Semaphore();

    /**
     * @brief 获取信号量
     */
    void wait();

    /**
     * @brief 释放信号量
     */
    void notify();
private:
    sem_t m_semaphore;
};

template<class T>
struct ScopedLockImpl{
public:
    ScopedLockImpl(T& mutex):m_mutex(mutex)
    {
        m_mutex.lock();
        m_locked = true;
    }
    
    void lock()
    {
        if(!m_locked)
        {
            m_mutex.lock();
            m_locked = true;
        }
    }
    
    void unlock()
    {
        if(m_locked)
        {
            m_mutex.unlock();
            m_locked =false;
        }
    }
private:
    T& m_mutex;
    bool m_locked;
};

class Mutex : Noncopyable{

public:
    typedef ScopedLockImpl<Mutex> Lock;
    
    Mutex(){
        pthread_mutex_init(&m_mutex, nullptr);
    }
    
    ~Mutex(){
        pthread_mutex_destroy(&m_mutex);
    }
    
    void lock()
    {
        pthread_mutex_lock(&m_mutex);
    }
    
    
    void unlock()
    {
        pthread_mutex_unlock(&m_mutex);
    }
    
private:
    pthread_mutex_t m_mutex;
};


}

#endif /* __SYLAR_MUTEX_H__ */



