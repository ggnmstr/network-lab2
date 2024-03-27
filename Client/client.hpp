#pragma once

#include <boost/asio/connect.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <iostream>
#include <string>

#define MSG_SIZE 4096
class client
{

public:
  client(boost::asio::io_context& context,
         boost::asio::ip::tcp::endpoint& server_endpoint,
         std::string const& filepath);
  ~client();

private:
  void openfile(std::string const& filepath);

  template<class buffer>
  void sendToSocket(buffer& t_buffer);

  void writeFileToBuf(const boost::system::error_code& t_ec);

private:
  std::array<char, MSG_SIZE> buffer_;
  boost::asio::ip::tcp::socket socket;
  std::ifstream sourceFileStream;
  boost::asio::streambuf m_request;
};
