#include <iostream>
#include <thread>

#include "audio.h"
#include "blockingqueue.h"
#include "vadfeatprocessor.h"
#include "vadengine.h"


int main(int argc, char* argv[])
{
    AudioParameter audioParams;
        audioParams.sampleRate = 16000;
        audioParams.channels = 1;
        audioParams.chunkSize = 1024;
    // Create modules
    AudioInput* input = new AudioInput(&audioParams);
    VADFeatProcessor* vadFeatProcessor = new VADFeatProcessor();
    VADEngine* vadEngine = new VADEngine();

    // Link modules
    vadFeatProcessor->set_input(input->subscribe());
    vadEngine->setInput(vadFeatProcessor->subscribe());
    
    // Run modules in threads
    std::thread vadfeat_thread(&VADFeatProcessor::run, vadFeatProcessor);
    std::thread audio_thread(&AudioInput::run, input);
    std::thread vad_thread(&VADEngine::run, vadEngine);
    

    std::cin.ignore();

    audio_thread.join();
    //vadfeat_thread.join();

    return 0;
}