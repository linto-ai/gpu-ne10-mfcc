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

extern uint32_t indexW;
int16_t *circular_buff;
extern pthread_mutex_t mutex;
extern FILE *f;
enum event e = None;
extern char* channels;
extern char* rate;
extern uint16_t MAX_BUF_SIZE;

/**
* Returns 0 if the pulse audio stream is correctly connected, -1 in other cases
* Param: Pointer to a device
* Param: Type of device, record of playback
* Param: Format of samples, 8 or 16 bits
* Param: Rate frequency in Hz
* Param: Number of channels
*/
int Audio_Init(pa_simple **s,char* type, char* format, char* rate, char* channels) {
  int error;
  char* endptr;
  pa_sample_spec ss;
  if (strcmp(format,"8") == 0 ) {
    ss.format = PA_SAMPLE_U8; // 8 bits
    ss.rate = strtol(rate,&endptr,10); // rate
    ss.channels = strtol(channels,&endptr,10); // 1 = mono, 2 = stereo
  }
  else if (strcmp(format,"16") == 0 ) {
    ss.format = PA_SAMPLE_S16LE; // 16 bits, Little Endian
    ss.rate = strtol(rate,&endptr,10); // rate
    ss.channels = strtol(channels,&endptr,10); // 1 = mono, 2 = stereo
  }
  /* Create a new playback stream */
  if (strcmp(type,"record") == 0 ) {
    if (!(*s = pa_simple_new(
      NULL,  // server name or NULL for default
      "LinTO", // application name
      PA_STREAM_RECORD, // Direction, Playback, record or upload
      NULL, // Sink name or NULL for default
      "record", // Descriptive name for this stream
      &ss, // The sample type to use
      NULL, // The channel map to use, or NULL for default
      NULL, // Buffering attributes, or NULL for default
      &error // A pointer where the error code is stored when the routine returns NULL
    ))) {
        printf("pa_simple_new() failed: %s\n", pa_strerror(error));
        return -1;
    }
  }
  else if (strcmp(type,"playback") == 0 ) {
    if (!(*s = pa_simple_new(
      NULL,  // server name or NULL for default
      "LinTO", // application name
      PA_STREAM_PLAYBACK, // Direction, Playback, record or upload
      NULL, // Sink name or NULL for default
      "playback", // Descriptive name for this stream
      &ss, // The sample type to use
      NULL, // The channel map to use, or NULL for default
      NULL, // Buffering attributes, or NULL for default
      &error // A pointer where the error code is stored when the routine returns NULL
    ))) {
        printf("pa_simple_new() failed: %s\n", pa_strerror(error));
        return -1;
    }
  }
  pa_usec_t latency;
  if ((latency = pa_simple_get_latency(*s, &error)) == (pa_usec_t /* uint64 */) -1) {
      printf("pa_simple_get_latency() failed: %s\n", pa_strerror(error));
      return -1;
  }
  return 0;
}

/**
* Get data from a device
* Param: Pointer to a device
* Param: Size of data to read
* Param: Pointer to a buffer
*/
int Audio_Input(pa_simple *s,int size, int16_t* buff) {
  /* Read some data ... */
  int error = 0;
  if (pa_simple_read(s, buff, size, &error) < 0) {
      printf("pa_simple_read() failed: %s\n", pa_strerror(error));
  }
  return error;
}

/**
* Send data to a device
* Param: Pointer to a device
* Param: Size of data to write
* Param: Pointer to a buffer containing the data to be read
*/
int Audio_Output(pa_simple *s, size_t size, int16_t* buff) {
  int error = 0;
  if (pa_simple_write(s, buff, size, &error) < 0) {
      printf("pa_simple_write() failed: %s\n", pa_strerror(error));
  }
  return error;
}


/**
* Test the Pulse Audio sound server by playing the microphone input
* Param: Format in bits (8 or 16)
* Param: Rate in Hz
* Param: Number of channels (1 to 4)
*/
int record() {
  int16_t* buff=malloc(sizeof(int16_t)*BUFSIZE);
  int error = 0;
  pa_simple *s_playback = NULL;
  pa_simple *s_record = NULL;
  int i = 0;
  Audio_Init(&s_playback,"playback","16",rate,channels);
  Audio_Init(&s_record,"record","16",rate,channels);
  for (;;) {
      if ((error = Audio_Input(s_record,BUFSIZE*sizeof(int16_t),buff)) < 0) {
          printf("Input Error : %d !\n",error);
      }
      pthread_mutex_lock(&mutex);
      indexW = add(indexW,buff,BUFSIZE);
      if (e == Wakeword && f != NULL) {
        fwrite(buff,sizeof(int16_t),BUFSIZE,f);
      }
      pthread_mutex_unlock(&mutex);
      //Audio_Output(s_playback,BUFSIZE*sizeof(int16_t),buff);
    }
  free(buff);
}
