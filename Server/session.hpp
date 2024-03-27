#pragma once

#include <array>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/filesystem.hpp>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <istream>
#include <memory>
#include <string>

#define MSG_SIZE 4096

class session : public std::enable_shared_from_this<session>
{
public:
  session(boost::asio::ip::tcp::socket t_socket);

  void startReading();

private:
  void processMetaData(size_t t_bytesTransferred);
  void readData(std::istream& stream);
  void doReadFileContent(size_t t_bytesTransferred);

  double totalspd = 0;
  size_t times_sent = 0;

  size_t fileSize;
  std::string fileName;

  size_t bytes_recvd_period;
  boost::posix_time::ptime last_recvd_time;

  boost::asio::ip::tcp::socket socket;
  std::array<char, MSG_SIZE> recvBuf;
  boost::asio::streambuf metadataBuf;
  std::ofstream outputFileStream;
};
