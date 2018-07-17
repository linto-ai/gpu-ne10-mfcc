#ifndef AUDIO_H
#define AUDIO_H
#include <pulse/pulseaudio.h>
#include <pulse/simple.h>
#include <pulse/error.h>

using namespace std;

struct AudioParameter 
{
    uint16_t sampleRate;
    uint8_t channels;
    uint16_t chunkSize;
};

class AudioInput
{
    public:
    AudioInput(AudioParameter *params);
    void TestRun();

    private:
    uint16_t chunk_size;
    pa_simple *s;
    pa_sample_spec ss;
    
};


#endif