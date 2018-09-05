/*
 * 文件：aipayserver.cpp
 * 作者：Xiwei Sun
 * 日期：2018年9月1日
 * 描述：重构的人工智能支付服务程序
 */
#include "aipayserver.h"


int main()
{
    char log_chars[1024];
    std::string log_str;
    //日志初始化
    log_init();
    //打印系统启动消息
    log_str = "AiPayServer系统启动...版本号V3.14_20180901";
    log_output(log_str);
    //初始化TCP Server
    pay_tcp_svr.start(PAY_SVR_IP, PAY_TCP_PORT);
    sprintf(log_chars, "Tcp Server 启动成功，监听端口：%d", PAY_TCP_PORT);
    log_str = log_chars;
    log_output(log_str);
    //初始化HTTP Server
    const scy::net::Address address(PAY_SVR_IP, PAY_HTTP_PORT);
    p_http_srv = new scy::http::Server(address);
    p_http_srv->start();
    //设置http消息接收处理函数
    p_http_srv->Connection += [](http::ServerConnection::Ptr conn) {
        conn->Payload += [](http::ServerConnection& conn, const MutableBuffer& buffer) {
            mesg_sock ms;
            ms.message = buffer.str();
            ms.psocket = conn;
            pthread_t tid_msg_handle;
            pthread_create(&tid_msg_handle,NULL,http_msg_handle, &ms);
            pthread_detach(tid_msg_handle);
            //conn.send(bufferCast<const char*>(buffer), buffer.size());
            //conn.close();
        };
    };
    sprintf(log_chars, "Http Server 启动成功，监听端口：%d", PAY_HTTP_PORT);
    log_str = log_chars;
    log_output(log_str);
    waitForShutdown();
}

//HTTP 消息处理线程
void * http_msg_handle(void *arg)
{
    p_mesg_sock pms = (p_mesg_sock)arg;
    std::cout << pms->message << std::endl;
    return NULL;
}