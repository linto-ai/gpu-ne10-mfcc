#ifndef VADFEATPROCESSOR_H
#define VADFEATPROCESSOR_H
#include <cmath>
#include <complex>
#include <valarray>
#include <iostream>

#include "blockingqueue.h"
#include "features.h"

class VADFeatProcessor
{
    public:
    VADFeatProcessor();
    void set_input(BlockingQueue<int16_t*>* queue);
    void run();
    BlockingQueue<std::valarray<float>>* subscribe();
    void pause();
    void resume();
    void stop();

    private:
    bool isActive;
    VADFeatsParams params;
    BlockingQueue<int16_t*>* input_queue;

    std::vector<BlockingQueue<std::valarray<float>>*> outputQueues; 

    uint16_t fbar_lfreq_index, fbar_hfreq_index;

};

#endif