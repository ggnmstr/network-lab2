#include "server.hpp"
#include "session.hpp"

server::server(boost::asio::io_context& context,
               boost::asio::ip::tcp::endpoint& endpoint)
  : socket(context)
  , acceptor(context, endpoint)
  , uploadPathstr("./upload")
{
  initUploadDir(uploadPathstr);
  doAccept();
}

server::~server() {}

void
server::initUploadDir(std::string& uploadPathstr)
{
  boost::filesystem::path uploadPath(uploadPathstr);
  if (!boost::filesystem::exists(uploadPath) &&
      !boost::filesystem::create_directory(uploadPath)) {
    std::cout << "fail to init in upload directory!" << std::endl;
    return;
  }
}

void
server::doAccept()
{

  std::cout << "Server: waiting for clients..."  << std::endl;
  // std::cout << "Server waiting on " << acceptor.local_endpoint().port()
  //           << std::endl;
  acceptor.async_accept(socket, [this](boost::system::error_code ec) {
    if (!ec) {
      std::cout << "Connection established with "
                << socket.remote_endpoint().address() << std::endl;
      std::make_shared<session>(std::move(socket))->startReading();
    }
    doAccept();
  });
}
