#include "audio.h"
#include <iostream>

AudioInput::AudioInput(AudioParameter *params)
{
    // Init parameters
    ss.format = PA_SAMPLE_S16LE;
    ss.channels = params->channels;
    ss.rate = params->sampleRate;
    chunk_size = params->chunkSize;

    // Open connexion
    s = pa_simple_new(NULL,"Audio", PA_STREAM_RECORD, NULL, "ASR", &ss, NULL, NULL, NULL);

}

AudioInput::~AudioInput(){
    for (BlockingQueue<int16_t*> * queue : audioQueues)
        {
            queue->flush();
        }
}

void AudioInput::run()
{
    int ret;
    int error;
    int16_t *buffer;
    while (true) {
        buffer = new int16_t[chunk_size];
        ret = pa_simple_read(s,buffer,chunk_size,&error);

        for (BlockingQueue<int16_t*> * queue : audioQueues)
        {
            queue->push(buffer);
        }
    }
}

BlockingQueue<int16_t*>* AudioInput::subscribe()
{
   BlockingQueue<int16_t*> *new_queue = new BlockingQueue<int16_t*>(MAX_QUEUE_SIZE);
   audioQueues.push_back(new_queue);
   return new_queue;
}

