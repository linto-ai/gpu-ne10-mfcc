#ifndef VADENGINE_H
#define VADENGINE_H
#include <valarray>
#include <deque>
#include <valarray>
#include <iostream>

#include "blockingqueue.h"

const uint QUEUE_SIZE = 30;

struct VADParams{
    float_t fbar_th = 0.5;
    u_int16_t speech_threshold = 3;
    u_int16_t silence_threshold = 8;
};

enum UtteranceStatus {
    thresholdReached,
    timeout
};

class VADEngine{
    public:
    VADEngine();
    ~VADEngine();
    void setInput(BlockingQueue<std::valarray<float>>* queue);
    void setCallBackFunction();
    void run();
    UtteranceStatus detectUtterance();

    private:
    float energy_th, fbar_th;
    VADParams params;
    bool detecting = false;
    std::deque<float>* speechEnergy;
    std::deque<float>* nSpeechEnergy;
    BlockingQueue<std::valarray<float>>* inputQueue = NULL;

};

#endif