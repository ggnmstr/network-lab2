#include "server.hpp"
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/basic_endpoint.hpp>
#include <iostream>
#include <string>

int main(int argc, char ** argv){
    if (argc != 2){
        std::cout << "Usage: ./server.out  <port>" << std::endl;
        return 0;
    }
    int port = std::stoi(argv[1]);
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address_v4::any(),port);
    boost::asio::io_context io_context;
    server server(io_context,endpoint);
    io_context.run();
    return 0;
}
