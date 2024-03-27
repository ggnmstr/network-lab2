#include "client.hpp"
#include <boost/asio/ip/address.hpp>
#include <iostream>
#include <string>

int main(int argc, char ** argv){
    if (argc < 3){
        std::cout << "Usage: ./client.out <server-address> <port> <path_to_file>" << std::endl;
        return 0;
    }
    boost::asio::ip::tcp::endpoint server_endpoint(boost::asio::ip::address::from_string(argv[1]),std::stoi(argv[2]));
    auto filepath = argv[3];
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::resolver r(io_context);
    boost::asio::ip::tcp::resolver::results_type res = r.resolve(argv[1],argv[2]);
    client client(io_context, server_endpoint, filepath);
    io_context.run();
    return 0;
}
