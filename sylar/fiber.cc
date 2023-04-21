#include "fiber.h"
#include "config.h"
#include "macro.h"

#include <atomic>

namespace sylar {

static Logger::ptr g_logger = SYLAR_LOG_NAME("system");

static std::atomic<uint64_t> s_fiber_id { 0 };
static std::atomic<uint64_t> s_fiber_count { 0 };

static thread_local Fiber* t_fiber = nullptr;
static thread_local Fiber::ptr t_threadFiber = nullptr;

static ConfigVar<uint32_t>::ptr g_fiber_stack_size = 
    Config::Lookup<uint32_t>("fiber.stack_size", 128*1024, "fiber stack size");

class MallocStackAllocator{
public:
    static void* Alloc(size_t size){
        return malloc(size);
    }

    static void Dealloc(void* vp,size_t size){
        return free(vp);
    }
};

using StackAllocator = MallocStackAllocator;

Fiber::Fiber(){
    m_state = EXEC;
    SetThis(this);

    if(getcontext(&m_ctx)){
        SYLAR_ASSERT2(false,"getcontext");
    }

    ++s_fiber_count;

    SYLAR_LOG_DEBUG(g_logger) << "Fiber::Fiber main";
}

Fiber::Fiber(std::function<void()> cb,size_t stacksize, bool use_caller)
    :m_id(++s_fiber_id) ,m_cb(cb){

    ++s_fiber_count;
    m_stacksize = stacksize ? stacksize : g_fiber_stack_size->getValue();

    m_stack = StackAllocator::Alloc(m_stacksize);
    if(getcontext(&m_ctx)){
        SYLAR_ASSERT2(false,"getcontext");
    }
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    if(!use_caller){
        makecontext(&m_ctx, &Fiber::MainFunc, 0);
    }else{
        makecontext(&m_ctx, &Fiber::CallerMainFunc, 0);
    }

    SYLAR_LOG_DEBUG(g_logger) << "Fiber::Fiber id=" << m_id;
}

Fiber::~Fiber(){
    --s_fiber_count;
    if(m_stack){
        SYLAR_ASSERT(m_state == TERM 
            || m_state == EXCEPT 
            || m_state == INIT);

        StackAllocator::Dealloc(m_stack,m_stacksize);
    }else{
        SYLAR_ASSERT(!m_cb);
        SYLAR_ASSERT(m_state == EXEC);

        Fiber* cur = t_fiber;
        if(cur == this) {
            SetThis(nullptr);
        }
    }
    SYLAR_LOG_DEBUG(g_logger) << "Fiber::~Fiber id=" << m_id
                              << " total=" << s_fiber_count;

}


void Fiber::reset(std::function<void()> cb)
{

}
            void Fiber::swapIn(){

            }
            void Fiber::swapOut(){

            }
            void Fiber::call(){

            }
            void Fiber::back(){

            }

            void Fiber::SetThis(Fiber* f){

            }
            /**
            * @brief 返回当前所在的协程
            */
            Fiber::ptr Fiber::GetThis(){
                if(t_fiber){
                    return t_fiber->shared_from_this();
                }
                Fiber::ptr main_fiber(new Fiber);
                SYLAR_ASSERT(t_fiber == main_fiber.get());
                t_threadFiber = main_fiber;

                return t_fiber->shared_from_this();
            }

            /**
            * @brief 将当前协程切换到后台,并设置为READY状态
            * @post getState() = READY
            */
            void Fiber::YieldToReady(){

            }
            /**
            * @brief 将当前协程切换到后台,并设置为HOLD状态
            * @post getState() = HOLD
            */
            void Fiber::YieldToHold(){

            }

            /**
            * @brief 返回当前协程的总数量
            */
            uint64_t Fiber::TotalFibers(){
                return s_fiber_count;
            }

            void Fiber::MainFunc(){

            }

            void Fiber::CallerMainFunc(){

            }

            uint64_t Fiber::GetFiberId(){
                if(t_fiber) {
                    return t_fiber->getId();
                }
                return 0;
            }

}

