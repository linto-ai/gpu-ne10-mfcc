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
#include "mqtt_client.h"

MQTT_Client::MQTT_Client(string ip,int16_t port,int QOS,string* topics) {
  this->ip = ip;
  this->port = port;
  string addr = "tcp://" + ip + ":" + to_string(port);
  MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
  MQTTClient_create(mqtt_client, addr.c_str(), "ASR",MQTTCLIENT_PERSISTENCE_NONE, NULL);
  conn_opts.keepAliveInterval = 20;
  conn_opts.cleansession = 1;
  MQTTClient_setCallbacks(*mqtt_client, NULL, connlost, msgarrvd, delivered);
  for (int i = 0; i < (sizeof(topics)/sizeof(*topics)); i++) {
    MQTTClient_subscribe(*mqtt_client,(topics[i]).c_str(), QOS);
  }
  MQTTClient_connect(*mqtt_client, &conn_opts);
}

int MQTT_Client::run() {

}


MQTT_Client::~MQTT_Client() {

}

void delivered(void *context, MQTTClient_deliveryToken dt) {
  cout << "Message with token value" << dt <<"delivery confirmed" << endl;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
  void *vp = static_cast<void*>(message->payload);
  string *sp = static_cast<std::string*>(vp);
  string value = *sp;
  delete sp;
  if (strcmp(topicName,"")==0 && value == "") {
    (void)0; 
  }
  else if (strcmp(topicName,"")==0 && value == "") {
    (void)0;
  }
  MQTTClient_freeMessage(&message);
  MQTTClient_free(topicName);
  return 1;
}
void connlost(void *context, char *cause) {
  cout << "Connection lost" << endl;
  cout << "  context: " << context <<  "cause: " << cause << endl;
}

