#include <scy/net/sslsocket.h>
#include <scy/net/tcpsocket.h>


using namespace scy;
using namespace scy::net;

using namespace std;

/// The TCP echo server accepts a template argument
/// of either a TCPSocket or a SSLSocket.
class EchoServer : public SocketAdapter
{
public:
    TCPSocket::Ptr server;
    Socket::Vec sockets;

    EchoServer()
        : server(std::make_shared<TCPSocket>())
    {
    }

    ~EchoServer()
    {
        shutdown();
    }

    void start(const std::string& host, uint16_t port)
    {

        server->bind(Address(host, port));
        server->listen();
        server->AcceptConnection += slot(this, &EchoServer::onAcceptConnection);
    }

    void shutdown()
    {
        server->close();
        sockets.clear();
    }

    void onAcceptConnection(const TCPSocket::Ptr& socket)
    {
        socket->addReceiver(this);
        // socket->Recv += slot(this, &EchoServer::onClientSocketRecv);
        // socket->Error += slot(this, &EchoServer::onClientSocketError);
        // socket->Close += slot(this, &EchoServer::onClientSocketClose);
        sockets.push_back(socket);
        cout << "On accept: " << socket->address().host() <<"\t" << socket->address().port() << endl;
    }

    void onSocketRecv(Socket& socket, const MutableBuffer& buffer, const Address& peerAddress)
    {
        cout << "On recv: " << &socket << ": " << buffer.str() << endl;

        // Echo it back
        socket.send(bufferCast<const char*>(buffer), buffer.size());

        // Send a HTTP packet
        // std::ostringstream res;
        // res << "HTTP/1.1 200 OK\r\n"
        //     << "Connection: close\r\n"
        //     << "Content-Length: 0" << "\r\n"
        //     << "\r\n";
        // std::string response(res.str());
        // socket.send(response.c_str(), response.size());
    }

    void onSocketError(Socket& socket, const Error& error)
    {
        LDebug("On error: ", error.err, ": ", error.message)
    }

    void onSocketClose(Socket& socket)
    {
        LDebug("On close")

        for (typename Socket::Vec::iterator it = sockets.begin();
            it != sockets.end(); ++it) {
            if (it->get() == &socket) {
                LDebug("Removing: ", &socket)

                // All we need to do is erase the socket in order to
                // deincrement the ref counter and destroy the socket
                sockets.erase(it);
                return;
            }
        }
        assert(0 && "unknown socket");
    }
};



