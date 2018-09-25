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
#include "../include/mqtt_client.h"

BlockingQueue<mqtt_message> *mqtt_queue;
int new_message = 0;


/**
* Init MQTT_Client class
* Param: IP string of MQTT broker
* Param: Port of MQTT broker
* Param: Quality of service of broker 
* Param: Array of topics to subscribe
* Param: Size of array
*/
MQTT_Client::MQTT_Client(string ip,int16_t port,int QOS,string topics[], int elements) {
  mqtt_queue = new BlockingQueue<mqtt_message>(20);
  this->ip = ip;
  this->port = port;
  string addr = "tcp://" + ip + ":" + to_string(port);
  MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
  if (MQTTClient_create(&mqtt_client, addr.c_str(), "ASR" ,MQTTCLIENT_PERSISTENCE_NONE, NULL) != MQTTCLIENT_SUCCESS) {
    cout << "MQTT_Client creation failed" << endl;
  }
  conn_opts.keepAliveInterval = 20;
  conn_opts.cleansession = 1;
  if (MQTTClient_setCallbacks(mqtt_client, NULL, connlost, msgarrvd, delivered) != MQTTCLIENT_SUCCESS) {
    cout << "MQTT_Client creation failed" << endl;
  }
  if (MQTTClient_connect(mqtt_client, &conn_opts) != MQTTCLIENT_SUCCESS) {
    cout << "MQTT connection failed" << endl;
  }
  else {
    cout << "Connected to " << addr.c_str() << endl;
  }
  for (int i = 0; i < elements; i++) {
    if (MQTTClient_subscribe(mqtt_client,(topics[i]).c_str(), QOS)!= MQTTCLIENT_SUCCESS) {
      cout << "Sub to " << (topics[i]).c_str() << " failed" << endl;
    }
  }
  
}

/**
* Delete MQTT_Client and destroy client
*/
MQTT_Client::~MQTT_Client() {
  MQTTClient_disconnect(mqtt_client, 100);
  MQTTClient_destroy(&mqtt_client);
}

/**
* Function called on delivered message
* Param: context of send
* Param: Unique token of message
*/
void delivered(void *context, MQTTClient_deliveryToken dt) {
  cout << "Message with token value" << dt <<"delivery confirmed" << endl;
}

/**
* Function called on message received
* Param: context
* Param: Name of topic
* Param: Size of topic 
* Param: Specific struct for mqtt message
*/
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
  mqtt_message msg;
  msg.topic = string((char*)topicName);
  msg.payload = string((char*)message->payload,message->payloadlen);
  mqtt_queue->push(msg);
  new_message++;
  MQTTClient_freeMessage(&message);
  MQTTClient_free(topicName);
  return 1;
}

/**
* Function called on connection lost
* Param: context
* Param: cause
*/
void connlost(void *context, char *cause) {
  cout << "Connection lost" << endl;
  cout << "Context: " << context <<  " cause: " << cause << endl;
}