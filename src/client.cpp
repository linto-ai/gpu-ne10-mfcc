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

Client::Client(string port,string address,int queue_max_size): s_tcp(io_service),s(io_service) {    
    this->port = port;
    this->address = address;
    unix_domain_socket = false;
    boost::asio::ip::tcp::resolver resolver(io_service);
    std::cout << "Connecting to server..." << std::endl;
    boost::asio::connect(s_tcp, resolver.resolve({address, port}));
    std::cout << "Connection successful to " << address <<":"<< port <<  std::endl;
}

Client::Client(string pathname): s_tcp(io_service),s(io_service) {   
    this->pathname = pathname;
    unix_domain_socket = true;
    std::cout << "Connecting to server..." << std::endl;
    boost::asio::local::stream_protocol::endpoint ep(pathname);
    s.connect(ep);
    std::cout << "Connection successful" << std::endl;
}

void Client::sendData(const char* buffer,int size) {
    if (unix_domain_socket == true) {
        boost::asio::write(s, boost::asio::buffer(buffer, size));
    }
    else {
        boost::asio::write(s_tcp, boost::asio::buffer(buffer, size));
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