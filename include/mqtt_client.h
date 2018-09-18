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

#ifndef MQTT_H
#define MQTT_H
#include <iostream>
#include <string>
#include "blockingqueue.h"
#include "MQTTClient.h"
using namespace std;

struct mqtt_message {
    std::string topic;
    std::string payload;
};

extern BlockingQueue<mqtt_message> *mqtt_queue;
extern int new_message;

void delivered(void *context, MQTTClient_deliveryToken dt);
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);
void connlost(void *context, char *cause);
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);

class MQTT_Client {
    public:
    MQTT_Client(string ip,int16_t port,int QOS,string topics[], int elements);
    ~MQTT_Client();

    private:
    string* topic_subscribed;
    MQTTClient mqtt_client;
    string ip;
    int16_t port;

};
#endif