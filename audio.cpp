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

void AudioInput::TestRun()
{
    int ret;
    int error;
    int16_t buffer[chunk_size];

    for (int i=0;i<10;i++) {
        ret = pa_simple_read(s,buffer ,chunk_size,&error);
        if (ret != 0)
        {
            std::cout<<pa_strerror(error) << std::endl;
        } else {
            std::cout << buffer[0] << std::endl;
        }
        
    }
}