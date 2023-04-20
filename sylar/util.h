#ifndef __SYLAR_UTIL_H__
#define __SYLAR_UTIL_H__

#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/syscall.h>
#include <stdarg.h>

namespace sylar{



/**
 * @brief 返回当前线程的ID
 */
pid_t GetThreadId();

/**
 * @brief 返回当前协程的ID
 */
uint32_t GetFiberId();


};

#endif