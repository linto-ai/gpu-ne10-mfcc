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


#include "../include/mqtt.h"
#include "../include/Audio.h"
#include "../include/jsmn.h"
extern enum event e;
extern pthread_mutex_t wuw_mutex;
extern pthread_cond_t WUW_cond;
extern pthread_cond_t VAD_end_cond;
MQTTClient* mqtt_client;
char* ip;
char* port;
char* sub_topic;
char* sub_keyword_start;
char* sub_keyword_stop;
char* sub_topic_bis;

 volatile MQTTClient_deliveryToken deliveredtoken;


 /**
 * Fonction called back when a message is delivered
 * Param: context
 * Param: delivery token
 */
 void delivered(void *context, MQTTClient_deliveryToken dt) {
     printf("Message with token value %d delivery confirmed\n", dt);
     deliveredtoken = dt;
 }


 /**
 * Fonction called back when a message arrives
 * Param: context
 * Param: Related topic name
 * Param: topic length
 * Param: Message strcuture received
 */
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
     int i;
     int error;
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
     jsmn_parser parser; //Init JSON parser
     jsmntok_t tokens[5]; //Init avaible tokens
     jsmn_init(&parser); //Init parser
     error = jsmn_parse(&parser, data, strlen(data), tokens, 5);
     char* value = malloc(sizeof(char)*50);
     if (error == JSMN_ERROR_INVAL || error == JSMN_ERROR_NOMEM || error == JSMN_ERROR_PART) {
       printf("Failed to read JSON %d\n",error);
       MQTTClient_freeMessage(&message);
       MQTTClient_free(topicName);
       return 1;
     }
     else {
       strncpy(value,data+tokens[4].start,tokens[4].end-tokens[4].start);
       printf("Value : %s \n",value);
     }
     if (strcmp(sub_topic,topicName)==0) {
       pthread_cond_signal(&WUW_cond);
       e = Wakeword;
     }
     else if (strcmp(sub_topic_bis,topicName)==0) {
       pthread_cond_signal(&VAD_end_cond);
       e = VAD_end;
     }
     free(value);
     MQTTClient_freeMessage(&message);
     MQTTClient_free(topicName);
     return 1;
}
void connlost(void *context, char *cause)
{
   printf("\nConnection lost ");
   printf("  context:%s   cause: %s\n",context, cause);
}

/**
* Fonction tp subscribe to a topic on MQTT
* Param: pointer to an MQTT client
* Param: Related topic name
*/
int subscribe(MQTTClient* client) {
   MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
   int rc;
   char* ADDRESS = malloc(sizeof(char)*255);
   ADDRESS = strcat(ADDRESS,"tcp://");
   ADDRESS = strcat(ADDRESS,ip);
   ADDRESS = strcat(ADDRESS,":");
   ADDRESS = strcat(ADDRESS,port);
   MQTTClient_create(client, ADDRESS, CLIENTID,MQTTCLIENT_PERSISTENCE_NONE, NULL);
   conn_opts.keepAliveInterval = 20;
   conn_opts.cleansession = 1;
   MQTTClient_setCallbacks(*client, NULL, connlost, msgarrvd, delivered);
   while ( (rc = MQTTClient_connect(*client, &conn_opts)) != MQTTCLIENT_SUCCESS )
   {
     sleep(1);
   }
   printf("Connected to broker %s\n",ADDRESS);
   MQTTClient_subscribe(*client,sub_topic, QOS);
   MQTTClient_subscribe(*client,sub_topic_bis, QOS);
   printf("Correctly subscibed to %s and %s\n",sub_topic,sub_topic_bis);
   mqtt_client = client;
   free(ADDRESS);
   return rc;
}
