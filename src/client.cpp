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

Client::Client(int port,string address,int queue_max_size) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    this->port = port;
    this->address = address;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cout << "Socket creation error" << endl;
    }
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if(inet_pton(AF_INET, address.c_str(), &serv_addr.sin_addr)<=0) 
    {
        cout << "Invalid address/ Address not supported" << endl;
    }
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        cout << "Connection Failed " << endl;
    }
    this->sock = sock;

    //BlockingQueue<int16_t> queue(queue_max_size);
    //this->queue = &queue;
}

void Client::sendData(void* buffer,int size) {
    send(this->sock , buffer , size , 0 );
}

int Client::getPort() {
    return this->port;
}

string Client::getAddress() {
    return this->address;
}

Client::~Client() {
    close(this->sock);
}