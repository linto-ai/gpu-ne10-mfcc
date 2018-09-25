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
#ifndef AUDIO_H
#define AUDIO_H
#include <pulse/pulseaudio.h>
#include <pulse/simple.h>
#include <pulse/error.h>
#include "blockingqueue.h"

using namespace std;

const int MAX_QUEUE_SIZE = 10;



struct AudioParameter 
{
    uint16_t sampleRate;
    uint8_t channels;
    uint16_t chunkSize;
};

class AudioInput
{
    public:
    /**
     * @brief Construct a new Audio Input object
     * 
     * @param params Stream Parameters.
     */
    AudioInput(AudioParameter *params);
    ~AudioInput();
    void run();
    /**
     * @brief Returns a Queue of audio chunks
     * 
     * @return BlockingQueue<std::vector<int16_t>>
     */
    BlockingQueue<int16_t*>* subscribe();

    private:
    uint16_t chunk_size;
    pa_simple *s,*s_play;
    pa_sample_spec ss;
    std::vector<BlockingQueue<int16_t*>*> outPutQueues;
    
};

#endif