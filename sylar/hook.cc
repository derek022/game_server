#include "hook.h"
#include "fiber.h"
#include "iomanager.h"

#include <dlfcn.h>


namespace sylar{

static thread_local bool t_hook_enable = false;

#define HOOK_FUN(XX)    \
    XX(sleep)           \
    XX(usleep)          \


void hook_init(){
    static bool is_inited = false;
    if(is_inited)
        return;
#define XX(name) name ## _f = (name ## _fun) dlsym(RTLD_NEXT, #name);
    HOOK_FUN(XX);
#undef XX
}


struct _HookIniter{
    _HookIniter(){
        hook_init();
    }

};

// static argument
static _HookIniter s_hook_initer;



bool is_hook_enable()
{
    return t_hook_enable;
}

void set_hook_enable(bool flag)
{
    t_hook_enable = flag;
}

}

extern "C"{

#define XX(name) name ## _fun name ## _f = nullptr;
    HOOK_FUN(XX);
#undef XX


unsigned int sleep(unsigned int seconds)
{
    if(sylar::t_hook_enable){
        return sleep_f(seconds);
    }

    sylar::Fiber::ptr fiber = sylar::Fiber::GetThis();
    sylar::IOManager* iom = sylar::IOManager::GetThis();
    iom->addTimer(seconds * 1000, std::bind((void(sylar::Scheduler::*)
            (sylar::Fiber::ptr, int thread))&sylar::IOManager::schedule,
            iom,fiber,-1));

    sylar::Fiber::YieldToHold();
    return 0;
}



}



