/**
 * Copyright (C) 2018 Linagora
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cstdio>
#include <iostream>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

using namespace boost::asio::local;
using boost::asio::local::stream_protocol;


class Session
  : public boost::enable_shared_from_this<Session>
{
  public:
  Session(boost::asio::io_service& io_service) : socket(io_service)
  {
  }

  boost::asio::local::stream_protocol::socket& getSocket()
  {
    return socket;
  }

  void start()
  {
    socket.async_read_some(boost::asio::buffer(data_),
        boost::bind(&Session::handle_read,
          shared_from_this(),
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
  }

  void handle_read(const boost::system::error_code& error,
      size_t bytes_transferred)
  {
    if (!error)
    {
      boost::asio::async_write(socket,
          boost::asio::buffer(data_, bytes_transferred),
          boost::bind(&Session::handle_write,
            shared_from_this(),
            boost::asio::placeholders::error));
    }
  }

  void handle_write(const boost::system::error_code& error)
  {
    if (!error)
    {
      socket.async_read_some(boost::asio::buffer(data_),
          boost::bind(&Session::handle_read,
            shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }
  }

private:
  // The socket used to communicate with the client.
  boost::asio::local::stream_protocol::socket socket;

  // Buffer used to store data received from the client.
  boost::array<char, 1024> data_;
};

typedef boost::shared_ptr<Session> Session_ptr;

class Server
{
public:
  Server(boost::asio::io_service& io_service, const std::string& file)
    : io_service_(io_service),
      acceptor_(io_service, boost::asio::local::stream_protocol::endpoint(file))
  {
    Session_ptr new_session(new Session(io_service_));
    acceptor_.async_accept(new_session->getSocket(),
        boost::bind(&Server::handle_accept, this, new_session,
          boost::asio::placeholders::error));
  }

  void handle_accept(Session_ptr new_session,
      const boost::system::error_code& error)
  {
    if (!error)
    {
      new_session->start();
    }

    new_session.reset(new Session(io_service_));
    acceptor_.async_accept(new_session->getSocket(),
        boost::bind(&Server::handle_accept, this, new_session,
          boost::asio::placeholders::error));
  }

private:
  boost::asio::io_service& io_service_;
  boost::asio::local::stream_protocol::acceptor acceptor_;
};
