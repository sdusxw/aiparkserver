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
            memcpy(ms.message, buffer.data(), buffer.size());
            ms.message[buffer.size()] = '\0';
            ms.psocket = &conn;
            pthread_t tid_msg_handle;
            pthread_create(&tid_msg_handle,NULL,http_msg_handle, &ms);
            pthread_detach(tid_msg_handle);
            sleep(3000);
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
    std::string log_str;
    p_mesg_sock pms = (p_mesg_sock)arg;
    std::string send_msg = std::string(pms->message, strlen(pms->message));
    log_str = "收到HTTP消息: ";
    log_str += send_msg;
    log_output(log_str);
    std::string recv_msg;
    if(pay_tcp_svr.trans_mesg(send_msg, recv_msg))
    {
        pms->psocket->send(recv_msg.c_str(), recv_msg.length());
        log_str = "回复HTTP消息: ";
        log_str += recv_msg;
        log_output(log_str);
    }
    pms->psocket->close();
    return NULL;
}
