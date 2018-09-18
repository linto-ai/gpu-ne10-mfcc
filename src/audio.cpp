#include "../include/audio.h"
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
    s_play = pa_simple_new(NULL,"Audio", PA_STREAM_PLAYBACK, NULL, "ASR", &ss, NULL, NULL, NULL);
}

AudioInput::~AudioInput(){
    for (BlockingQueue<int16_t*> * queue : outPutQueues)
        {
            queue->flush();
        }
}

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
        //pa_simple_write(s_play, buffer, chunk_size*sizeof(int16_t),&error);
    }
}

BlockingQueue<int16_t*>* AudioInput::subscribe()
{
   BlockingQueue<int16_t*> *new_queue = new BlockingQueue<int16_t*>(MAX_QUEUE_SIZE);
   outPutQueues.push_back(new_queue);
   return new_queue;
}

