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
#ifndef CLIENT_H
#define CLIENT_H
#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include "boost/asio.hpp"
#include "blockingqueue.h"
using namespace std;
using namespace boost::asio::generic;

class Client {
    public:
    Client(string port,string address);
    Client(string pathname);
    string getPort();
    string getAddress();
    void sendData(const char* buffer,int size);
    void receiveData(char* msg,int size);
    ~Client();
    
    private:
    bool unix_domain_socket = true;
    boost::asio::io_service io_service;
    boost::asio::local::stream_protocol::socket s;
    boost::asio::ip::tcp::socket s_tcp;
    string pathname;
    string port;
    string address;
    BlockingQueue<int16_t> *input_queue;
};

#endif