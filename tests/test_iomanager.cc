#include "sylar/macro.h"
#include "sylar/iomanager.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

int sock = 0;

void test_fiber()
{
    SYLAR_LOG_INFO(g_logger)<< "test fiber sock=" << sock ;


    sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sock, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;
    memset(&addr, 0 ,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET,"127.0.0.1",&addr.sin_addr.s_addr);

    if(!connect(sock, (const sockaddr*)&addr, sizeof(addr))){
        return;
    }else if(errno == EINPROGRESS){
        SYLAR_LOG_INFO(g_logger) << " add event errno=" << errno << " " << strerror(errno);

        sylar::IOManager::GetThis()->addEvent(sock,sylar::IOManager::READ,[](){
            SYLAR_LOG_INFO(g_logger) << " read callback";
        });


        sylar::IOManager::GetThis()->addEvent(sock,sylar::IOManager::WRITE,[](){
            SYLAR_LOG_INFO(g_logger) << " write callback";

            sylar::IOManager::GetThis()->cancelEvent(sock,sylar::IOManager::READ);
            close(sock);
        });
    }else{
        SYLAR_LOG_INFO(g_logger) << " else " << errno << " " << strerror(errno);
    }
}

void test1()
{
    std::cout << "EPOLLIN=" << EPOLLIN
              << " EPOLLOUT=" << EPOLLOUT << std::endl;
    sylar::IOManager iom(2,false);
    iom.schedule(&test_fiber);
}


int main(int argc,char** argv)
{
    test1();

    return 0;
}