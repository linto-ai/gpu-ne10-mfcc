#include <iostream>

#include "audio.h"
#include "blockingqueue.h"


int main(int argc, char* argv[])
{
    AudioParameter audioParams;
        audioParams.sampleRate = 16000;
        audioParams.channels = 1;
        audioParams.chunkSize = 1024;
    AudioInput input(&audioParams);

    std::cout<<"Salut monde"<<std::endl;
    BlockingQueue<int> lequeue(17);
    

}