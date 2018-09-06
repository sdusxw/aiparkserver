#include <scy/http/server.h>
#include <iostream>
#include <scy/application.h>

using std::endl;
using namespace scy;

const uint16_t HttpPort = 7667;
const net::Address address("0.0.0.0", HttpPort);

int main(int argc, char** argv)
{
    http::Server srv(address);
    srv.start();
    
    srv.Connection += [](http::ServerConnection::Ptr conn) {
        conn->Payload += [](http::ServerConnection& conn, const MutableBuffer& buffer) {
            std::cout << buffer.str() << std::endl;
            while(true);
            conn.send(bufferCast<const char*>(buffer), buffer.size());
            conn.close();
        };
    };
    
    std::cout << "HTTP server listening on " << address << std::endl;
    waitForShutdown();
    return 0;
}
