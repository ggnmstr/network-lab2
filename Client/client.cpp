#include "client.hpp"
#include <cstdint>
#include <vector>

client::client(boost::asio::io_context& context,
               boost::asio::ip::tcp::endpoint& server_endpoint,
               std::string const& filepath)
  : socket(context)
{
  // connect to server, openfile and start writing
  socket.async_connect(
    server_endpoint, [this, filepath](boost::system::error_code ec) {
      if (!ec) {
        std::cout << "Client connected to "
                  << socket.remote_endpoint().address() << ":"
                  << socket.remote_endpoint().port() << std::endl;
        openfile(filepath);
        sendToSocket(m_request);
      } else {
        std::cout << "Error connecting to server" << std::endl;
      }
    });
}

client::~client()
{
  // close sourceFile
  sourceFileStream.close();
}

void
client::openfile(std::string const& filepath)
{
  // open file, read it as binary and go to the end
  sourceFileStream.open(filepath, std::ios_base::binary | std::ios_base::ate);
  if (sourceFileStream.fail()) {
    std::cout << "Fail opening file" << std::endl;
    return;
  }

  // get size
  auto fileSize = sourceFileStream.tellg();
  if (fileSize > 1099511627776) {
    std::cout << "Error: file's size is bigget than 1TB" << fileSize
              << std::endl;
    return;
  }

  // go to the beginning
  sourceFileStream.seekg(0, sourceFileStream.beg);

  std::ostream requestStream(&m_request);
  boost::filesystem::path p(filepath);
  std::string filename = p.filename().string();
  std::vector<uint8_t> utf8bytes(filename.begin(), filename.end());
  if (utf8bytes.size() > 4096) {
    std::cout << "Error: file's name in size is bigger than 4096 bytes"
              << std::endl;
    return;
  }

  uint8_t *data = utf8bytes.data();
  for (int i = 0; i < utf8bytes.size(); i++){
    requestStream << data[i];
  }
  requestStream << "\n" << fileSize << "\n\n";
}

template<class buffer>
void
client::sendToSocket(buffer& buftowrite)
{
  boost::asio::async_write(
    socket, buftowrite, [this](boost::system::error_code ec, size_t /*length*/) {
      writeFileToBuf(ec);
    });
}
void
client::writeFileToBuf(const boost::system::error_code& t_ec)
{
  if (t_ec) {
    std::cout << "doWriteFile error" << std::endl;
    return;
  }
  if (sourceFileStream) {
    // read buffer_.size() bytes to buffer from source file
    sourceFileStream.read(buffer_.data(), buffer_.size());
    if (sourceFileStream.fail() && !sourceFileStream.eof()) {
      std::cout << "sourceFileStream reading error" << std::endl;
      return;
    }

    // create buffer with buffer_'s data and size of amount read
    auto buf = boost::asio::buffer(
      buffer_.data(), static_cast<size_t>(sourceFileStream.gcount()));

    sendToSocket(buf);
  }
}
