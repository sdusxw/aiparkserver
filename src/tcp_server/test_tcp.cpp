#include "tcp_server.h"

#include <scy/application.h>
#include <scy/logger.h>


using std::endl;
using namespace scy;


const uint16_t TcpPort = 7666;


struct Servers
{
    net::TCPEchoServer tcp;
};


int main(int argc, char** argv)
{
    {
        Servers srvs;
        srvs.tcp.start("0.0.0.0", TcpPort);
        
        std::cout << "TCP Lerver listening on " << srvs.tcp.server->address() << std::endl;
        
        waitForShutdown([&](void*) {
            srvs.tcp.shutdown();
        });
    }
    return 0;
}

