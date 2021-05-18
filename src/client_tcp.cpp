#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>

const size_t max_length = 1024;

int main(void) {

    boost::asio::io_context io_context;
    char request[max_length];
    char reply[max_length];

    try {

        boost::asio::ip::tcp::socket sock(io_context);
        boost::asio::ip::tcp::resolver resolver(io_context);
        boost::asio::connect(sock, resolver.resolve("127.0.0.1", "3200"));

        std::cout << "Enter message: ";

        std::cin.getline(request, max_length);
        size_t request_length = std::strlen(request);
        boost::asio::write(sock, boost::asio::buffer(request, request_length));

        size_t reply_length = boost::asio::read(sock, boost::asio::buffer(reply, request_length));
        std::cout << "Reply: ";
        std::cout.write(reply, reply_length);
        std::cout << "\n";
    }

    catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
