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
            p_mesg_sock pms = (p_mesg_sock)malloc(sizeof(mesg_sock));
            memcpy(pms->message, buffer.data(), buffer.size());
            pms->message[buffer.size()] = '\0';
            pms->msg_len = buffer.size();
            pms->psocket = &conn;
            pthread_t tid_msg_handle;
            pthread_create(&tid_msg_handle,NULL,http_msg_handle, pms);
            pthread_detach(tid_msg_handle);
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
    std::string send_msg = std::string(pms->message, pms->msg_len);
    log_str = "收到HTTP消息: ";
    log_str += send_msg;
    log_output(log_str);
    char recv_msg[1024];
    int recv_len = 0;
    if(pay_tcp_svr.trans_mesg(send_msg, (char*)recv_msg, recv_len))
    {
        std::string msg_recv = std::string((char*)recv_msg, recv_len);
        pms->psocket->send((char*)recv_msg, recv_len);
        log_str = "回复HTTP消息: ";
        log_str += msg_recv;
        log_output(log_str);
    }
    else
    {
        std::string msg_response = "{\"ret\":\"fail\"}";
        pms->psocket->send((char*)recv_msg, recv_len);
        log_str = "回复HTTP消息:[error,超时无返回] ";
        log_str += msg_response;
        log_output(log_str);
    }
    pms->psocket->close();
    if(pms){free(pms);pms=NULL;}
    return NULL;
}
