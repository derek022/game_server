#ifndef __SYLAR_HOOK_H__
#define __SYLAR_HOOK_H__

#include <time.h>
#include <sys/types.h>

namespace sylar{

    bool is_hook_enable();
    void set_hook_enable(bool flag);
}

extern "C"{

typedef unsigned int (*sleep_fun)(unsigned int seconds);
extern sleep_fun sleep_f;

typedef int (*usleep_fun)(useconds_t usec);
extern usleep_fun usleep_f;


}

#endif

