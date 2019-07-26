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
#include "../include/Audio.h"

extern uint16_t MAX_BUF_SIZE;
int16_t *circular_buff;
extern int32_t indexW;
extern pthread_mutex_t mutex;
extern pthread_cond_t WUW_cond;
extern pthread_cond_t VAD_end_cond;
FILE *f;
char* channels;
char* rate;
extern char* ip;
extern char* port;
extern char* sub_topic;
extern char* sub_keyword_start;
extern char* sub_keyword_stop;
extern char* sub_topic_bis;

void create_pipe(char* path) {
  mkfifo(path, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
}

int main(int argc, char** argv) {
  if (argc<11) {
    printf("Usage: PipeFile Rate Channels Circular_Buffer_Time Broker_ip Broker_port Sub_Topic Sub_Keyword_Start Sub_Keyword_Stop Pub_Topic Pub_Keyword\n");
    printf("PipeFile: Name of the named pipe or the file to save data\n");
    printf("Rate: audio sampling rate\n");
    printf("Channels: Number of channels to be used, 1 for mono, 2 for stereo etc...\n");
    printf("Circular_Buffer_Time: Number of seconds the circular_buff should save data\n");
    printf("Broker_ip: The IP address of the MQTT local broker, use localhost by default\n");
    printf("Broker_port: The port of the MQTT local broker, use 1883 by default\n");
    printf("Sub_Topic: The topic name to receive messages\n");
    printf("Sub_Keyword_Start: The message to start recording\n");
    printf("Sub_Keyword_Stop: The message to spot recording\n");
    printf("Sub_Topic_bis: The topic name to receive stop messages\n");
    return 1;
  }
  circular_buff = malloc(sizeof(int16_t)* MAX_BUF_SIZE);
  MQTTClient client;
  channels = argv[3];
  rate = argv[2];
  ip = argv[5];
  port = argv[6];
  sub_topic = argv[7];
  sub_keyword_start= argv[8];
  sub_keyword_stop= argv[9];
  sub_topic_bis= argv[10];
  MAX_BUF_SIZE = strtof(argv[4],NULL)*strtol(rate,NULL,10)*strtol(channels,NULL,10);
  subscribe(&client);
  if (argc == 12 && strcmp(argv[11],"pipe") == 0) {
    create_pipe(argv[1]);
  }
  pthread_t recorder;
  mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER; // Create the mutex to protect shared variables
  pthread_mutex_t wuw_mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_t vad_mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
  //pthread_mutex_lock(&wuw_mutex);
  //pthread_mutex_lock(&vad_mutex);
  f = fopen(argv[1],"w+b"); // Open the binary file
  if (f == NULL) {
    printf("Failed to open %s !\n",argv[1]);
    return 1;
  }
  if(pthread_create(&recorder, NULL, &record, (void*) NULL) == -1) { // Create the thread to record data
    printf("Thread error\n");
    return 1;
  }
  //fseek(f,0,SEEK_END); // Going to the end of the file
  //int c = 0;
  //do c=getchar(); while (c!='\n' && c!=EOF); // Wait until the next event (input)
  for(;;) {
    pthread_cond_wait(&WUW_cond, &wuw_mutex);
    f = fopen(argv[1],"w+b"); // Open the binary file
    if (f == NULL) {
      printf("Failed to open%s !\n",argv[1]);
      return 1;
    }
    pthread_mutex_lock(&mutex);
    if (MAX_BUF_SIZE > BUFSIZE) {
      fwrite(circular_buff+indexW,sizeof(int16_t),MAX_BUF_SIZE-indexW,f); // Write the circular buffer in the pipe
      fwrite(circular_buff,sizeof(int16_t),indexW,f);
    }
    pthread_mutex_unlock(&mutex);
    printf("Start record\n");
    pthread_cond_wait(&VAD_end_cond, &vad_mutex);
    printf("End record\n");
    fclose(f);
  }
  pthread_join(recorder, NULL); //Wait fot the thread to finish
  free(circular_buff);
  MQTTClient_disconnect(client, 10000);
  MQTTClient_destroy(&client);
  return 0;
}
