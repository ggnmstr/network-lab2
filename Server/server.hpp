#pragma once
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
class server
{

public:
  server(boost::asio::io_context& context,
         boost::asio::ip::tcp::endpoint& endpoint);
  ~server();

private:
  std::string uploadPathstr;
  boost::asio::ip::tcp::socket socket;
  boost::asio::ip::tcp::acceptor acceptor;

  void initUploadDir(std::string& uploadPathstr);
  void doAccept();
};
