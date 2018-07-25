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
    pa_simple *s;
    pa_sample_spec ss;
    std::vector<BlockingQueue<int16_t*>*> outPutQueues;
    
};

#endif