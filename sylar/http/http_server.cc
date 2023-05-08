#include "http_server.h"
#include "sylar/log.h"

namespace sylar{
namespace http{

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

HttpServer::HttpServer(bool keepalive
        , sylar::IOManager* worker
        , sylar::IOManager* accept_worker )
    :TcpServer(worker,accept_worker)
    ,m_isKeepAlive(keepalive)
{
    m_dispatch.reset(new ServletDispatch);
}

void HttpServer::handleClient(Socket::ptr client) 
{
    HttpSession::ptr session(new HttpSession(client));
    do{
        auto req = session->recvRequest();
        if(!req){
            SYLAR_LOG_WARN(g_logger) << " recv http request fail, errno="
                << errno << " errstr=" << std::strerror(errno)
                << " client:" << *client;
            break;
        }

        HttpResponse::ptr rsp(new HttpResponse(req->getVersion(), req->isClose() || !m_isKeepAlive));
        rsp->setHeader("Server", getName());
        m_dispatch->handle(req, rsp, session);

       SYLAR_LOG_INFO(g_logger) << "requst:" << std::endl
           << *req;
       SYLAR_LOG_INFO(g_logger) << "response:" << std::endl
           << *rsp;

        session->sendResponse(rsp);
    }while(m_isKeepAlive);
}

}
}