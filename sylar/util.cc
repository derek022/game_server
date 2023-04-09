#include"util.h"

namespace sylar {

// static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

pid_t GetThreadId() {
    return syscall(SYS_gettid);
}

uint32_t GetFiberId() {
    // return sylar::Fiber::GetFiberId();
    return 1;
}

};
