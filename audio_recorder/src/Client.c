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
 #include "../include/Client.h"
 /**
 * Fonction to connect the socket with hostname and port
 * Param: Socket descriptor
 * Param: Hostname
 * Param: Port number
 */
void connect_to_host(SOCKET sock, char* hostname, int PORT) {
  struct hostent *hostinfo = NULL;
  SOCKADDR_IN sin = { 0 };
  hostinfo = gethostbyname(hostname);
  if (hostinfo == NULL)
  {
      fprintf (stderr, "Unknown host %s.\n", hostname);
      exit(EXIT_FAILURE);
  }
  sin.sin_addr = *(IN_ADDR *) hostinfo->h_addr;
  sin.sin_port = htons(PORT);
  sin.sin_family = AF_INET;
  if(connect(sock,(SOCKADDR *) &sin, sizeof(SOCKADDR)) == SOCKET_ERROR)
  {
      printf("Failed to connect to %s on port %d",hostname,PORT);
      exit(errno);
  }
  printf("Connect to %s on port %d! \n",hostname,PORT);
}

/**
* Fonction to send data via a socket
* Param: Socket descriptor
* Param: pointer to data
* Param: size of data
*/
void send_data(SOCKET sock, int16_t* data,int size) {
  if(send(sock, data, size, 0) < 0)
  {
    perror("send()");
    exit(errno);
  }
}

/**
* Fonction to receive data via a socket
* Param: Socket descriptor
* Param: pointer to data
* Param: size of data
*/
void receive_data(SOCKET sock, int16_t* data,int size) {
  int n = 0;
  if((n = recv(sock, data, size, 0)) < 0)
  {
    perror("recv()");
    exit(errno);
  }
}
