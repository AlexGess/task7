// LDFLAGS += -pthread -lboost_thread-mt -lboost_chrono -lboost_system
//

#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <string>
#include <memory>
#include <functional>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>

class Connection : public std::enable_shared_from_this<Connection> {
public:
    Connection(boost::asio::io_service &io_service);
    void start(void);
    std::shared_ptr<boost::asio::ip::tcp::socket> getSocket(void);
private:
    void doWrite(void);
    void doRead(void);
    void handleWrite(const boost::system::error_code &e, size_t n);
    void handleRead(const boost::system::error_code &e, size_t n);
private:
    enum { MAX_LENGTH = 512 };
private:
    std::shared_ptr<boost::asio::ip::tcp::socket> m_socket;
    char m_data[MAX_LENGTH];
    size_t curr_size;
}; // End of class

Connection::Connection(boost::asio::io_service &io_service)
    : m_socket(std::make_shared<boost::asio::ip::tcp::socket>(io_service))
{}

void Connection::start(void)
{
    doRead();
}

std::shared_ptr<boost::asio::ip::tcp::socket> Connection::getSocket(void)
{
    return m_socket;
}

void Connection::doWrite(void)
{
    m_socket->async_send(boost::asio::buffer(m_data, curr_size),
        std::bind(&Connection::handleWrite, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void Connection::handleWrite(const boost::system::error_code &error, size_t n)
{
    if (!error) {
        doRead();
    }
}

void Connection::doRead(void)
{
    m_socket->async_receive(boost::asio::buffer(m_data, MAX_LENGTH),
        std::bind(&Connection::handleRead, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void Connection::handleRead(const boost::system::error_code &e, size_t n)
{
    if (!n) {
        return;
    }

    if (1) { // check end of message
        curr_size = n;
        printf("client: %.*s\n", n, m_data);
        doWrite();
    }
    else {
        curr_size += n;
        doRead();
    }
}

class Server {
public:
    Server(const boost::asio::ip::address &address, uint16_t port);
    void run(void);
private:
    void startAccept(void);
    void handleAccept(std::shared_ptr<Connection> connection, const boost::system::error_code &error);
private:
    boost::asio::io_service m_service;
    boost::asio::ip::tcp::acceptor m_acceptor;
};  // End of class


Server::Server(const boost::asio::ip::address &address, uint16_t port)
    : m_service()
    , m_acceptor(m_service, boost::asio::ip::tcp::endpoint(address, port))
{
    startAccept();
}

void Server::run(void)
{
    m_service.run();
}

void Server::startAccept(void)
{
    std::shared_ptr<Connection> connect(std::make_shared<Connection>(m_service));
    m_acceptor.async_accept(*(connect->getSocket()),
        std::bind(&Server::handleAccept, this, connect, std::placeholders::_1));
}

void Server::handleAccept(std::shared_ptr<Connection> connection, const boost::system::error_code &error)
{
    if (!error) {
        connection->start();
    }
    startAccept();
}


int main(void)
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
