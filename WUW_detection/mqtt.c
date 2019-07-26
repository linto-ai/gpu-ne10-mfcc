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

 #include "mqtt.h"
 #include "Audio.h"

 volatile MQTTClient_deliveryToken deliveredtoken;
 void delivered(void *context, MQTTClient_deliveryToken dt) {
     printf("Message with token value %d delivery confirmed\n", dt);
     deliveredtoken = dt;
 }

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
     int i;
     char* data = malloc(sizeof(char)*(message->payloadlen+1));
     printf("Message arrived");
     printf("     topic: %s", topicName);
     printf("   message: ");
     char* payloadptr = message->payload;
     data = message->payload;
     for(i=0; i<message->payloadlen; i++)
     {
         putchar(*payloadptr++);
     }
     putchar('\n');
     data[message->payloadlen]='\0';
     MQTTClient_freeMessage(&message);
     MQTTClient_free(topicName);
     return 1;
}
void connlost(void *context, char *cause)
{
   printf("\nConnection lost\n");
   printf("     cause: %s\n", cause);
}


int subscribe(MQTTClient* client,char* TOPIC) {
   MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
   int rc;
   int ch;
   MQTTClient_create(client, ADDRESS, CLIENTID,
       MQTTCLIENT_PERSISTENCE_NONE, NULL);
   conn_opts.keepAliveInterval = 20;
   conn_opts.cleansession = 1;
   MQTTClient_setCallbacks(*client, NULL, connlost, msgarrvd, delivered);
   if ((rc = MQTTClient_connect(*client, &conn_opts)) != MQTTCLIENT_SUCCESS)
   {
       printf("Failed to connect, return code %d\n", rc);
       exit(EXIT_FAILURE);
   }
   MQTTClient_subscribe(*client, TOPIC, QOS);

   return rc;
}
