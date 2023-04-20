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
#include <stdint.h>
#include <atomic>
#include <iostream>

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

    ~ScopedLockImpl(){
        unlock();
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

template<class T>
struct ReadScopedLockImpl{
public:
    ReadScopedLockImpl(T& mutex)
        :m_mutex(mutex){
        m_mutex.rdlock();
        m_locked = true;
    }

    ~ReadScopedLockImpl(){
        unlock();
    }

    void lock(){
        if(!m_locked){
            m_mutex.lock();
            m_locked = true;
        }
    }


    void unlock(){
        if(m_locked){
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    T& m_mutex;
    bool m_locked;
};

template<class T>
struct WriteScopedLockImpl {
public:
    /**
     * @brief 构造函数
     * @param[in] mutex 读写锁
     */
    WriteScopedLockImpl(T& mutex)
        :m_mutex(mutex) {
        m_mutex.wrlock();
        m_locked = true;
    }

    /**
     * @brief 析构函数
     */
    ~WriteScopedLockImpl() {
        unlock();
    }

    /**
     * @brief 上写锁
     */
    void lock() {
        if(!m_locked) {
            m_mutex.wrlock();
            m_locked = true;
        }
    }

    /**
     * @brief 解锁
     */
    void unlock() {
        if(m_locked) {
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    /// Mutex
    T& m_mutex;
    /// 是否已上锁
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


class NullMutex: Noncopyable{
public:
    typedef ScopedLockImpl<NullMutex> Lock;

    NullMutex(){}

    ~NullMutex(){}

    void lock(){}

    void unlock(){}
};

class RWMutex : Noncopyable{
public:
    typedef ReadScopedLockImpl<RWMutex> ReadLock;
    typedef WriteScopedLockImpl<RWMutex> WriteLock;

    RWMutex(){
        pthread_rwlock_init(&m_lock,nullptr);
    }

    ~RWMutex(){
        pthread_rwlock_destroy(&m_lock);
    }

    void rdlock()
    {
        pthread_rwlock_rdlock(&m_lock);
    }

    void wrlock()
    {
        pthread_rwlock_wrlock(&m_lock);
    }

    void unlock()
    {
        pthread_rwlock_unlock(&m_lock);
    }

private:
    pthread_rwlock_t m_lock;

};


/**
 * @brief 空读写锁(用于调试)
 */
class NullRWMutex : Noncopyable {
public:
    /// 局部读锁
    typedef ReadScopedLockImpl<NullMutex> ReadLock;
    /// 局部写锁
    typedef WriteScopedLockImpl<NullMutex> WriteLock;

    /**
     * @brief 构造函数
     */
    NullRWMutex() {}
    /**
     * @brief 析构函数
     */
    ~NullRWMutex() {}

    /**
     * @brief 上读锁
     */
    void rdlock() {}

    /**
     * @brief 上写锁
     */
    void wrlock() {}
    /**
     * @brief 解锁
     */
    void unlock() {}
};



/**
 * @brief 自旋锁
 */
class Spinlock : Noncopyable {
public:
    /// 局部锁
    typedef ScopedLockImpl<Spinlock> Lock;

    /**
     * @brief 构造函数
     */
    Spinlock() {
        pthread_spin_init(&m_mutex, 0);
    }

    /**
     * @brief 析构函数
     */
    ~Spinlock() {
        pthread_spin_destroy(&m_mutex);
    }

    /**
     * @brief 上锁
     */
    void lock() {
        pthread_spin_lock(&m_mutex);
    }

    /**
     * @brief 解锁
     */
    void unlock() {
        pthread_spin_unlock(&m_mutex);
    }
private:
    /// 自旋锁
    pthread_spinlock_t m_mutex;
};

class CASLock : Noncopyable{
public:
    typedef ScopedLockImpl<CASLock> Lock;

    CASLock(){
        m_mutex.clear();
    }
            
    ~CASLock(){
    }

    void lock(){
        while(std::atomic_flag_test_and_set_explicit(&m_mutex,std::memory_order_acquire));
    }

    void unlock(){
        std::atomic_flag_clear_explicit(&m_mutex,std::memory_order_release);
    }

private:
    volatile std::atomic_flag m_mutex;
};


}

#endif /* __SYLAR_MUTEX_H__ */



