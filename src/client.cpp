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

#include "../include/client.h"

using namespace std;
using namespace boost::asio::generic;

/**
 * Init client class
 * Param: Port string of server
 * Param: Address string of server
 */
Client::Client(string port,string address): s_tcp(io_service),s(io_service) {    
    this->port = port;
    this->address = address;
    unix_domain_socket = false;
    boost::asio::ip::tcp::resolver resolver(io_service);
    std::cout << "Connecting to server..." << std::endl;
    boost::asio::connect(s_tcp, resolver.resolve({address, port}));
    std::cout << "Connection successful to " << address <<":"<< port <<  std::endl;
}

/**
 * Init client class
 * Param: Pathname to socket.sock
 */
Client::Client(string pathname): s_tcp(io_service),s(io_service) {   
    this->pathname = pathname;
    unix_domain_socket = true;
    std::cout << "Connecting to server..." << std::endl;
    boost::asio::local::stream_protocol::endpoint ep(pathname);
    s.connect(ep);
    std::cout << "Connection successful" << std::endl;
}

/**
 * Function to send data through socket
 * Param: buffer to send
 * Param: size of buffer
 */
void Client::sendData(const char* buffer,int size) {
    if (unix_domain_socket == true) {
        boost::asio::write(s, boost::asio::buffer(buffer, size));
    }
    else {
        boost::asio::write(s_tcp, boost::asio::buffer(buffer, size));
    }
}

/**
 * Receives data through socket
 * Param: Buffer to save data
 * Param: Size of data to read
 */
void Client::receiveData(char* msg,int size) {
    if (unix_domain_socket == true) {
        boost::asio::read(s, boost::asio::buffer(msg,size));
    }
    else {
        boost::asio::read(s_tcp, boost::asio::buffer(msg,size));
    }
}

string Client::getPort() {
    return this->port;
}

string Client::getAddress() {
    return this->address;
}

Client::~Client() {

}