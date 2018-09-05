/*
 * 文件：aipayserver.h
 * 作者：Xiwei Sun
 * 日期：2018年9月1日
 * 描述：重构的人工智能支付服务程序
 */
#ifndef AI_PAY_SERVER
#define AI_PAY_SERVER

#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <scy/application.h>
#include <scy/http/server.h>

#include "tcp_server.h"
#include "utils.h"

#define PAY_SVR_IP      ("0.0.0.0")
#define PAY_TCP_PORT    (7666)
#define PAY_HTTP_PORT   (7667)

//结构体声明
typedef struct
{
    std::string message;                    //JSON消息
    http::ServerConnection *psocket;        //socket指针
}mesg_sock, *p_mesg_sock;

//全局变量声明
TcpServer pay_tcp_svr;          //TCP服务器，接收AI_PAY_CLIENT的TCP连接，转发WEB的HTTP请求的JSON消息到对应的停车场
scy::http::Server *p_http_srv;  //HTTP服务器，接收用户请求并处理转发

//HTTP 消息处理线程
void * http_msg_handle(void *arg);


#endif  //AI_PAY_SERVER
