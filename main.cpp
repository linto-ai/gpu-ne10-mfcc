#include <iostream>
#include <thread>

#include "audio.h"
#include "blockingqueue.h"


int main(int argc, char* argv[])
{
    AudioParameter audioParams;
        audioParams.sampleRate = 16000;
        audioParams.channels = 1;
        audioParams.chunkSize = 1024;
    AudioInput* input = new AudioInput(&audioParams);

    std::thread audio_thread(&AudioInput::run, input);
    int16_t value;
    BlockingQueue<int16_t*>* receiver = input->subscribe();
    BlockingQueue<int16_t*>* receiver2 = input->subscribe();
    while (true) {
        std::cout << receiver->pop()[0] << std::endl;
        std::cout << receiver2->pop()[0] << std::endl;
    }
    audio_thread.join();

    return 0;
}