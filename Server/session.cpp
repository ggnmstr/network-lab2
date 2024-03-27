#include "session.hpp"
#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>

session::session(boost::asio::ip::tcp::socket t_socket)
  : socket(std::move(t_socket))
{
}

void
session::startReading()
{
  auto self = shared_from_this();
  async_read_until(socket,
                   metadataBuf,
                   "\n\n",
                   [this, self](boost::system::error_code ec, size_t bytes) {
                     if (!ec) {
                       processMetaData(bytes);
                     } else {
                       std::cout << "Session: Error start_reading" << std::endl;
                     }
                   });
}

void
session::processMetaData(size_t t_bytesTransferred)
{

  std::istream requestStream(&metadataBuf);
  std::vector<uint8_t> bytes;
  uint8_t byte;
  while (true) {
    requestStream.read(reinterpret_cast<char*>(&byte), sizeof(byte));
    if (byte == '\n') {
      break;
    }
    bytes.push_back(byte);
  }
  fileName = std::string(bytes.begin(), bytes.end());
  requestStream >> fileSize;

  // create file and directory
  boost::filesystem::path filepath("./upload/"+fileName);
  if (boost::filesystem::exists(filepath)) {
    std::cout << "error: file already exists" << std::endl;
    return;
  }
  outputFileStream.open("./upload/" + fileName, std::ios_base::binary);
  if (!outputFileStream) {
    return;
  }

  requestStream.read(recvBuf.data(), 2);
  do {
    // std::cout << "Extra chars: " << requestStream.gcount() << std::endl;
    requestStream.read(recvBuf.data(), recvBuf.size());

    // write recvBuf's data to outputfile
    outputFileStream.write(recvBuf.data(), requestStream.gcount());
  } while (requestStream.gcount() > 0);

  auto self = shared_from_this();
  socket.async_read_some(
    boost::asio::buffer(recvBuf.data(), recvBuf.size()),
    [this, self](boost::system::error_code ec, size_t bytes) {
      if (!ec)
        doReadFileContent(bytes);
      else
        std::cout << "Session: error reading metadata" << std::endl;
    });
}

void
session::doReadFileContent(size_t bytesTransferred)
{
  // boost::posix_time::ptime cur_time =
  //   boost::posix_time::second_clock::local_time();
  //   auto diff = (cur_time - it.second).total_seconds();

  // boost::posix_time::ptime cur_time =
  //   boost::posix_time::microsec_clock::local_time();
  boost::posix_time::ptime cur_time =
    boost::posix_time::second_clock::local_time();
  if (last_recvd_time == boost::posix_time::not_a_date_time) {
    last_recvd_time = cur_time;
    bytes_recvd_period = 0;
  }
  bytes_recvd_period += bytesTransferred;
  times_sent++;
  auto diff = (cur_time - last_recvd_time);
  if (diff.total_seconds() >= 3) {
    double speed =
      (double)bytes_recvd_period / diff.total_seconds() / (1024 * 1024);
    totalspd += speed;
    double avgspeed = totalspd / times_sent;
    std::cout << "File: " << fileName;
    std::cout << " Instantaneous speed:" << speed << " megabytes/sec";
    std::cout << " Avg speed: " << avgspeed << " megabytes/sec" << std::endl;
    last_recvd_time = cur_time;
    bytes_recvd_period = 0;
  }

  if (bytesTransferred > 0) {
    outputFileStream.write(recvBuf.data(),
                           static_cast<std::streamsize>(bytesTransferred));

    if (outputFileStream.tellp() >= static_cast<std::streamsize>(fileSize)) {
      // std::cout << "Received file: " << fileName << std::endl;
      std::cout << "sizes are the same" << std::endl;
      return;
    }
  }
  auto self = shared_from_this();
  socket.async_read_some(
    boost::asio::buffer(recvBuf.data(), recvBuf.size()),
    [this, self](boost::system::error_code ec, size_t bytes) {
      doReadFileContent(bytes);
    });
}
