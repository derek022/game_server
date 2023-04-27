#include "sylar/hook.h"
#include "sylar/log.h"
#include "sylar/iomanager.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void test_hook()
{
    sylar::IOManager iom(1);

    iom.schedule([](){
        SYLAR_LOG_INFO(g_logger) << "start sleep 2";
        sleep(2);
        SYLAR_LOG_INFO(g_logger) << " sleep 2";
    });


    iom.schedule([](){
        sleep(3);
        SYLAR_LOG_INFO(g_logger) << " sleep 3";
    });

    SYLAR_LOG_INFO(g_logger) << " test_sleep";
}


int main(int argc ,char** argv)
{   
    test_hook();

    return 0;
}
