#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <boost/asio.hpp>


class Server {
public:
    Server(const boost::asio::ip::address &address, uint16_t port);
    void doReceive(void);
    void doSend(size_t length);
    void run(void);
private:
    enum { max_length = 512 };
private:
    boost::asio::io_service m_service;
    boost::asio::ip::udp::socket m_socket;
    boost::asio::ip::udp::endpoint m_sender_endpoint;
    char m_data[max_length];
};

Server::Server(const boost::asio::ip::address &address, uint16_t port)
    : m_service()
    , m_socket(m_service, boost::asio::ip::udp::endpoint(address, port))
{
    doReceive();
}

void Server::doReceive(void)
{
    auto f = [this](boost::system::error_code ec, size_t n) {
        if (!ec && n > 0) {
            printf("client: %.*s\n", n, m_data);
            doSend(n);
        }
        else {
            doReceive();
        }
    };

    m_socket.async_receive_from(boost::asio::buffer(m_data, max_length), m_sender_endpoint, f);
}

void Server::doSend(size_t length)
{
    auto f = [this](boost::system::error_code, size_t) { doReceive(); };

    m_socket.async_send_to(boost::asio::buffer(m_data, length), m_sender_endpoint, f);
}

void Server::run(void)
{
    m_service.run();
}


int main(int argc, char* argv[])
{
    try {
        Server server(boost::asio::ip::address::from_string("0.0.0.0"), 3200);
        server.run();
    }
    catch (std::exception &e) {
        printf("%s\n", e.what());
    }

    return 0;
}
