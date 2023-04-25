
#ifndef __SYLAR_TIMER_H__
#define __SYLAR_TIMER_H__

#include <memory>
#include "mutex.h"
#include <set>
#include <vector>


namespace sylar{

class TimerManager;
class Timer : public std::enable_shared_from_this<Timer>{
friend class TimerManager;
public:
    typedef std::shared_ptr<Timer> ptr;
    bool cancel();
    /**
     * @brief 刷新设置定时器的执行时间
     */
    bool refresh();
    
    /**
     * @brief 重置定时器时间
     * @param[in] ms 定时器执行间隔时间(毫秒)
     * @param[in] from_now 是否从当前时间开始计算
     */
    bool reset(uint64_t ms, bool from_now);
private:
    Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimerManager* manager);
    Timer(uint64_t next);

private:
    bool m_recurring = false;
    // 执行周期
    uint64_t m_ms = 0;
    // 精确的执行时间
    uint64_t m_next = 0;
    std::function<void()> cb;
    TimerManager* m_manager = nullptr;

private:
    struct Comparator{
        bool operator()(const Timer::ptr& lhs, const Timer::ptr& rhs) const;
    };
};


class TimerManager {
friend class Timer;

public:
    typedef RWMutex RWMutexType;

    TimerManager();
    virtual ~TimerManager();

    Timer::ptr addTimer(uint64_t ms,std::function<void()> cb, bool recurring = false);

    Timer::ptr addConditionTimer(uint64_t ms,std::function<void()> cb
                            , std::weak_ptr<void> weak_cond
                            , bool recurring = false);

    uint64_t getNextTimer();

    void listExpiredCb(std::vector<std::function<void()> >& cbs);

    bool hasTimer();

protected:
    /**
     * @brief 当有新的定时器插入到定时器的首部,执行该函数
     */
    virtual void ontimerInsertedAtFront() = 0;

    void addTimer(Timer::ptr  val, RWMutexType::WriteLock& lock);

private:
    /**
     * @brief 检测服务器时间是否被调后了
     */
    bool detectClockRollover(uint64_t now_ms);

private:
    RWMutexType m_mutex;

    std::set<Timer::ptr, Timer::Comparator> m_timers;
    // 是否触发 onTimerInsertAtFront() 方法
    bool m_tickled = false;
    uint64_t m_previouseTimer = 0;

};



}

#endif
