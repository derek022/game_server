

#ifndef __SYLAR_IOMANAGER_H__
#define __SYLAR_IOMANAGER_H__

#include "scheduler.h"


namespace sylar {

class IOManager : Scheduler{
public:
    typedef std::shared_ptr<IOManager> ptr;
    typedef RWMutex RWMutexType;
    enum Event {
        NONE = 0x0,
        READ = 0x1,
        WRITE = 0X2
    };

private:
    struct FdContext{
        typedef Mutex MutexType;
        struct EventContext{
            Scheduler* scheduler = nullptr;
            Fiber::ptr fiber;
            std::function<void()> cb;
        };

        EventContext& getContext(Event event);
        void resetContext(EventContext& ctx);
        void triggerEvent(Event event);

        EventContext read;
        EventContext write;
        int fd = 0;
        Event events = NONE;
        MutexType mutex;
    };

public:
    IOManager(size_t threads = 1, bool use_caller = true, const std::string& name = "");
    ~IOManager();

    int addEvent(int fd, Event event, std::function<void()> cb = nullptr);
    bool delEvent(int fd, Event event);

    bool cancelEvent(int fd, Event event);

    bool cancelAll(int fd); 
protected:
    void tickle() override;
    bool stopping() override;
    void idle() override;
    
        /**
     * @brief 重置socket句柄上下文的容器大小
     * @param[in] size 容量大小
     */
    void contextResize(size_t size);

private:
    int m_epfd = 0;
    // pipe 文件句柄
    int m_tickleFds[2];
    std::atomic<size_t> m_pendingEventCount = {0};
    RWMutexType m_mutex;
    std::vector<FdContext*> m_fdContexts;

};


}

#endif
