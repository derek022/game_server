#ifndef __SYLAR_HTTP_HTTP_PARSER_H__
#define __SYLAR_HTTP_HTTP_PARSER_H__

#include "http.h"
#include "http11_parser.h"
#include "httpclient_parser.h"


namespace sylar{
namespace http{

class HttpRequestParser{

public:
    typedef std::shared_ptr<HttpRequest> ptr;
    HttpRequestParser();
    size_t execute(char* data, size_t len);
    int isFinished();
    int hasError();

    HttpRequest::ptr getData() const { return m_data; }
    void setError(int v){ m_error = v; }
    uint64_t getContentLength();
private:
    http_parser m_parser;
    HttpRequest::ptr m_data;

    int m_error;
};






}
}



#endif