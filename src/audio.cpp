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
#include "../include/audio.h"
#include <iostream>


/**
 * Init Audio class
 * Param: chunk size, sample rate and audio format
 */
AudioInput::AudioInput(AudioParameter *params)
{
    // Init parameters
    ss.format = PA_SAMPLE_S16LE;
    ss.channels = params->channels;
    ss.rate = params->sampleRate;
    chunk_size = params->chunkSize;

    // Open connexion
    s = pa_simple_new(NULL,"Audio", PA_STREAM_RECORD, NULL, "ASR", &ss, NULL, NULL, NULL);
    s_play = pa_simple_new(NULL,"Audio", PA_STREAM_PLAYBACK, NULL, "ASR", &ss, NULL, NULL, NULL);
}

/**
 * Delete Audio class
 */
AudioInput::~AudioInput(){
    for (BlockingQueue<int16_t*> * queue : outPutQueues)
        {
            queue->flush();
        }
}

/**
 * Main thread function to get audio sample
 */
void AudioInput::run()
{
    int ret;
    int error;
    int16_t *buffer = new int16_t[chunk_size];
    while (true) {
        if ((ret = pa_simple_read(s,buffer,chunk_size*sizeof(int16_t),&error)) != 0) {
            cout << "Can't read input data" << endl;
        }
        for (BlockingQueue<int16_t*> * queue : outPutQueues)
        {
            queue->push(buffer);
        }
    }
}

/**
 * Return the audio queue
 */
BlockingQueue<int16_t*>* AudioInput::subscribe()
{
   BlockingQueue<int16_t*> *new_queue = new BlockingQueue<int16_t*>(MAX_QUEUE_SIZE);
   outPutQueues.push_back(new_queue);
   return new_queue;
}

