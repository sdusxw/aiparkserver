#include <scy/net/sslsocket.h>
#include <scy/net/tcpsocket.h>


using namespace scy;
using namespace scy::net;

using namespace std;

class TcpServer : public SocketAdapter
{
public:
    TCPSocket::Ptr server;
    TCPSocket::Vec sockets;
    typedef std::map<std::string, TCPSocket::Ptr> named_sockets;

    TcpServer()
        : server(std::make_shared<TCPSocket>())
    {
    }

    TcpServer()
    {
        shutdown();
    }

    void start(const std::string& host, uint16_t port)
    {

        server->bind(Address(host, port));
        server->listen();
        server->setReusePort();
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
        sockets.push_back(socket);
        cout << "On accept: " << socket->address().host() <<"\t" << socket->address().port() << endl;
        cout << "On accept: peerAddress" << socket->peerAddress().host() <<"\t" << socket->peerAddress().port() << endl;
    }

    void onSocketRecv(Socket& socket, const MutableBuffer& buffer, const Address& peerAddress)
    {
        cout << "On recv: " << &socket << ": " << buffer.str() << endl;
        cout << "On recv: " << socket.address().host() <<"\t" << socket.address().port() << endl;
        cout << "On recv: peerAddress " << socket.peerAddress().host() <<"\t" << socket.peerAddress().port() << endl;
        // Echo it back
        socket.send(bufferCast<const char*>(buffer), buffer.size());
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



