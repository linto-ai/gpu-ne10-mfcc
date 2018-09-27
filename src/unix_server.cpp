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
#include "../include/unix_server.h"

using boost::bind;
using boost::asio::io_service;
using boost::asio::async_write;
using boost::system::error_code;
using boost::asio::buffer;
using boost::asio::local::stream_protocol;
using boost::asio::placeholders::error;
using boost::asio::placeholders::bytes_transferred;

session::session(io_service& io_service): socket_(io_service)
{
    for (int i=0;i<num_cep;i++){
        //mfcc_input_1[i] = (char)i;
        //mfcc_input_1[i]= (char)i;
    }
}
 
stream_protocol::socket & session::socket()
{
	return socket_;
}

void session::start()
{
    socket_.async_read_some(buffer(data_),bind(&session::handle_read,shared_from_this(),error,bytes_transferred));
    async_write(socket_,buffer(mfcc_input_1, num_cep*sizeof(char)),bind(&session::handle_write,shared_from_this(),error));
    async_write(socket_,buffer(mfcc_input_2, num_cep*sizeof(char)),bind(&session::handle_write,shared_from_this(),error));
}

void session::handle_read(const error_code & error,size_t bytes_transferred)
{
    if (!error)
    {
      	async_write(socket_,buffer(data_, bytes_transferred),bind(&session::handle_write,shared_from_this(),error));
    }
}

void session::handle_write(const error_code & error)
{
   if (!error)
   {
        socket_.async_read_some(buffer(data_),bind(&session::handle_read,shared_from_this(),error,bytes_transferred));
    }
}

void session::set_num_cep(int v) {
    this->num_cep = v;
}

server::server(boost::asio::io_service& io_service, const std::string & file)
    :io_service_(io_service),acceptor_(io_service, stream_protocol::endpoint(file))
{
    session_ptr new_session(new session(io_service_));
    acceptor_.async_accept(new_session->socket(),bind(&server::handle_accept, this, new_session,error));
}

void server::set_num_cep(int v) {
    this->num_cep = v;
    mfcc_input_1 = new float[v];
    mfcc_input_2 = new float[v];
}

void server::handle_accept(session_ptr new_session,const error_code & error)
{
    if (!error)
    {
      new_session->set_num_cep(num_cep);
      sessions.push_back(new_session);  
      new_session->start();
    }
    new_session.reset(new session(io_service_));
    acceptor_.async_accept(new_session->socket(),bind(&server::handle_accept, this, new_session,error));
}

