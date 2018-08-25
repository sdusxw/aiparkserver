#include "tcp_server.h"

#include <scy/application.h>
#include <scy/logger.h>


using std::endl;
using namespace scy;


const uint16_t TcpPort = 7666;


struct Servers
{
    TCPEchoServer tcp;
};


int main(int argc, char** argv)
{
    Logger::instance().add(new ConsoleChannel("debug", Level::Debug));
    Logger::instance().setWriter(new AsyncLogWriter);
    {
        Servers srvs;
        srvs.tcp.start("0.0.0.0", TcpPort);
        
        std::cout << "TCP Server listening on " << srvs.tcp.server->address() << std::endl;
        
        waitForShutdown([&](void*) {
            srvs.tcp.shutdown();
        });
    }
    return 0;
}
