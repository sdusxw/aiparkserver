#include "tcp_server.h"

#include <scy/application.h>
#include <scy/logger.h>


using std::endl;
using namespace scy;


const uint16_t TcpPort = 7666;


TcpServer tcp_svr;


int main(int argc, char** argv)
{
    Logger::instance().add(new ConsoleChannel("debug", Level::Debug));
    Logger::instance().setWriter(new AsyncLogWriter);
    {
        tcp_svr.start("0.0.0.0", TcpPort);
        
        std::cout << "TCP Server listening on " << tcp_svr.server->address() << std::endl;
        
        waitForShutdown([&](void*) {
            tcp_svr.shutdown();
        });
    }
    return 0;
}
