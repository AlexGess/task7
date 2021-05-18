#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>


const size_t max_length = 1024;

int main(void)
{
    boost::asio::io_context io_context;

    char request[max_length];
    char reply[max_length];

    try {
        boost::asio::ip::udp::socket sock(io_context, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0));
        boost::asio::ip::udp::resolver resolver(io_context);
        boost::asio::ip::udp::resolver::results_type endpoints = resolver.resolve(boost::asio::ip::udp::v4(), "127.0.0.1", "3200");

        std::cout << "Enter message: ";

        std::cin.getline(request, max_length);
        size_t request_length = std::strlen(request);
        sock.send_to(boost::asio::buffer(request, request_length), *endpoints.begin());

        boost::asio::ip::udp::endpoint sender_endpoint;
        size_t reply_length = sock.receive_from(boost::asio::buffer(reply, max_length), sender_endpoint);
        std::cout << "Reply: ";
        std::cout.write(reply, reply_length);
        std::cout << "\n";
    }

    catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
