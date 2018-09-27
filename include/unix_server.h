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
#ifndef USERV_B_H
#define USERV_B_H

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
  void set_num_cep(int v);

private:
  // The socket used to communicate with the client.
  boost::asio::local::stream_protocol::socket socket_;
  int num_cep;
  // Buffer used to store data received from the client.
  boost::array<char, 1024> data_;
  boost::array<char, 40> mfcc_input_1;
  boost::array<char, 40> mfcc_input_2;
};

using session_ptr = boost::shared_ptr<session>;

class server
{
public:
  server(boost::asio::io_service& io_service, const std::string& file);
  void handle_accept(session_ptr new_session,const boost::system::error_code & error);
  void set_num_cep(int v);
  float_t* mfcc_input_1;
  float_t* mfcc_input_2;
  std::vector<session_ptr> sessions;

private:
  boost::asio::io_service& io_service_;
  boost::asio::local::stream_protocol::acceptor acceptor_;
  int num_cep;
};

#endif
