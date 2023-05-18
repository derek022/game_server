#ifndef __SYLAR_FIBER_H__
#define __SYLAR_FIBER_H__

#include <ucontext.h>
#include <memory>
#include <functional>


namespace sylar {
    class Fiber : public std::enable_shared_from_this<Fiber>{
        friend class Scheduler;
        public:
            typedef std::shared_ptr<Fiber> ptr;
            enum State{
                INIT,
                HOLD,
                EXEC,
                TERM,
                READY,
                EXCEPT
            };
        private:
            Fiber();
        public:
        
            Fiber(std::function<void()> cb, size_t stacksize = 0, bool use_caller = false);
            ~Fiber();
            void reset(std::function<void()> cb);
            void swapIn();
            void swapOut();
            void call();
            void back();

            uint64_t getId() const { return m_id; }
            State getState() const { return m_state; }
        public:
            static void SetThis(Fiber* f);
            /**
            * @brief 返回当前所在的协程
            */
            static Fiber::ptr GetThis();

            /**
            * @brief 将当前协程切换到后台,并设置为READY状态
            * @post getState() = READY
            */
            static void YieldToReady();
            /**
            * @brief 将当前协程切换到后台,并设置为HOLD状态
            * @post getState() = HOLD
            */
            static void YieldToHold();

            /**
            * @brief 返回当前协程的总数量
            */
            static uint64_t TotalFibers();

            static void MainFunc();

            static void CallerMainFunc();

            static uint64_t GetFiberId();
        private:
            uint64_t m_id = 0;
            uint32_t m_stacksize = 0;
            State m_state = INIT;
            ucontext_t m_ctx;
            void* m_stack = nullptr;
            std::function<void()> m_cb;
    };



}

#endif
