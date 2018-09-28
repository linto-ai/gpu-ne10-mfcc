
#ifndef USERV_B_HXX
#define USERV_B_HXX

#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

class session:public boost::enable_shared_from_this<session>
{
public:
  session(boost::asio::io_service& io_service);
  boost::asio::local::stream_protocol::stream_protocol::socket& socket();
  void start();
  void handle_read(const boost::system::error_code& error,size_t bytes_transferred);
  void handle_write(const boost::system::error_code& error);

private:
  // The socket used to communicate with the client.
  boost::asio::local::stream_protocol::socket socket_;

  // Buffer used to store data received from the client.
  boost::array<char, 1024> data_;
};

using session_ptr = boost::shared_ptr<session>;

class server
{
public:
  server(boost::asio::io_service& io_service, const std::string& file);
  void handle_accept(session_ptr new_session,const boost::system::error_code & error);

private:
  boost::asio::io_service& io_service_;
  boost::asio::local::stream_protocol::acceptor acceptor_;
};

#endif
