#ifndef _TCP_SERVER
#define _TCP_SERVER

#include <scy/net/sslsocket.h>
#include <scy/net/tcpsocket.h>
#include <jsoncpp/json/json.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <atomic>

using namespace scy;
using namespace scy::net;

using namespace std;

long get_utc();

typedef struct
{
    atomic_bool done;
    pthread_t *pth;
    char msg[1024];
}sem_msg, *p_sem_msg;

class TcpConnection : public SocketAdapter
{
public:
    std::string park_id;
    Socket * p_socket;
    std::map<std::string, p_sem_msg> map_sem_msg;
    
    //转发消息，并接收返回
    bool trans_recv(std::string msg_in, std::string & msg_out, pthread_t * pth)
    {
        bool b_ret = false;
        p_socket->send(msg_in.c_str(), msg_in.length());
        Json::Reader reader;
        Json::Value json_object;
        
        if (!reader.parse(msg_in, json_object))
        {
            //JSON格式错误导致解析失败
            cout << "[json]解析失败" << endl;
        }
        else
        {
            //根据openid来作为信号量的标识
            std::string openid = json_object["openid"].asString();
            sem_msg the_sem_msg;
            the_sem_msg.done = false;
            the_sem_msg.pth = pth;
            map_sem_msg[openid] = &the_sem_msg;
            while(!the_sem_msg.done)
            {
                
            }
            /*if (ret == -1)
            {
                b_ret = false;
            }
            else*/
            {
                b_ret = true;
                msg_out = std::string((const char *)(the_sem_msg.msg), strlen((const char *)(the_sem_msg.msg)));
            }
        }
        return b_ret;
    }
    
    void onSocketRecv(Socket& socket, const MutableBuffer& buffer, const Address& peerAddress)
    {
        Json::Reader reader;
        Json::Value json_object;
        
        if (!reader.parse(buffer.str(), json_object))
        {
            //JSON格式错误导致解析失败
            cout << "[json]解析失败" << endl;
        }
        else
        {
            //根据cmd来进入相应处理分支
            std::string string_cmd = json_object["cmd"].asString();
            if(string_cmd == "heartbeat")  //心跳消息
            {
                std::string park_id = json_object["park_id"].asString();
                long unix_ts = get_utc();
                Json::Value json_hb_msg;
                json_hb_msg["cmd"] = Json::Value("heartbeat");
                json_hb_msg["timestamp"] = Json::Value((int)unix_ts);
                std::string msg_hb = json_hb_msg.toStyledString();
                socket.send((const char *)msg_hb.c_str(), msg_hb.length());
            }else{  //非心跳消息则转发
                //根据openid来查找map对应的信号量
                std::string openid = json_object["openid"].asString();
                std::map<std::string, p_sem_msg>::iterator iter = map_sem_msg.find(openid);
                
                if( map_sem_msg.end() != iter )//找到openid对应的sem_msg
                {
                    p_sem_msg the_p_sem_msg = iter->second;
                    memcpy((void*)the_p_sem_msg->msg, buffer.data(), buffer.size());
                    (the_p_sem_msg->msg)[buffer.size()]='\0';
                    the_p_sem_msg->done = true;
                }else{
                    cout << "未找到" << openid << "对应的sem_msg" << endl;
                }
            }
        }
    }
};

class TcpServer : public SocketAdapter
{
public:
    TCPSocket::Ptr server;
    TCPSocket::Vec sockets;
    std::map<std::string, TcpConnection*> named_sockets;
    

    TcpServer()
        : server(std::make_shared<TCPSocket>())
    {
    }

    ~TcpServer()
    {
        shutdown();
    }

    void start(const std::string& host, uint16_t port)
    {

        server->bind(Address(host, port));
        server->listen();
        server->setReusePort();
        server->setNoDelay(true);
        server->AcceptConnection += slot(this, &TcpServer::onAcceptConnection);
    }

    void shutdown()
    {
        server->close();
        sockets.clear();
    }

    void onAcceptConnection(const TCPSocket::Ptr& socket)
    {
        socket->addReceiver(this);
        socket->setKeepAlive(true, 30);
        socket->setNoDelay(true);
        sockets.push_back(socket);
        cout << "On accept: " << socket->address().host() <<"\t" << socket->address().port() << endl;
        cout << "On accept: peerAddress" << socket->peerAddress().host() <<"\t" << socket->peerAddress().port() << endl;
    }

    void onSocketRecv(Socket& socket, const MutableBuffer& buffer, const Address& peerAddress)
    {    
        Json::Reader reader;
        Json::Value json_object;
        
        if (!reader.parse(buffer.str(), json_object))
        {
            //JSON格式错误导致解析失败
            cout << "[json]解析失败" << endl;
        }
        else
        {
            //根据cmd来进入相应处理分支
            std::string string_cmd = json_object["cmd"].asString();
            if (string_cmd == "init_parkid")    //硬件配置信息
            {
                std::string park_id = json_object["park_id"].asString();
                cout << "Init Park ID:\t" << park_id << endl;
                TcpConnection *p_tcp_conn = new TcpConnection();
                p_tcp_conn->park_id = park_id;
                p_tcp_conn->p_socket = & socket;
                socket.addReceiver(p_tcp_conn);
                socket.removeReceiver(this);
                named_sockets[park_id] = p_tcp_conn;
            }
        }
    }
    //转发消息，并处理
    bool trans_mesg(std::string msg_in, std::string & msg_out, pthread_t * pth)
    {
        Json::Reader reader;
        Json::Value json_object;
        
        if (!reader.parse(msg_in, json_object))
        {
            //JSON格式错误导致解析失败
            cout << "[json]解析失败" << endl;
        }
        else
        {
            //根据park_id来确定转发目标
            std::string park_id = json_object["park_id"].asString();
            std::map<std::string, TcpConnection*>::iterator iter = named_sockets.find(park_id);
            
            if( named_sockets.end() != iter )//找到park_id对应的tcp连接
            {
                TcpConnection *p_tcp_conn = iter->second;
                return p_tcp_conn->trans_recv(msg_in, msg_out, pth);
            }
            cout << "未找到" << park_id << "对应的tcp连接" << endl;
        }
        return false;
    }

    void onSocketError(Socket& socket, const Error& error)
    {
        cout << "On error: " << error.message << std::endl;
    }

    void onSocketClose(Socket& socket)
    {
        for (typename TCPSocket::Vec::iterator it = sockets.begin();
            it != sockets.end(); ++it) {
            if (it->get() == &socket) {
                std::cout << "some client closed" << std::endl;

                // All we need to do is erase the socket in order to
                // deincrement the ref counter and destroy the socket
                sockets.erase(it);
                return;
            }
        }
    }
};

#endif

