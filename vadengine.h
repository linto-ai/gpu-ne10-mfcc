#ifndef VADENGINE_H
#define VADENGINE_H
#include <valarray>
#include <deque>
#include <valarray>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include "blockingqueue.h"

const uint QUEUE_SIZE = 30;

struct VADParams{
    float_t fbar_th = 0.5;
    uint speech_fram_tr = 3; // Number of speech frame required to recognize an utterance
    uint silence_fram_th = 8; // Number of silence frame required to determine utterance end
    uint timeout = 5; // Delay to stop utterance detection if no speech is detected
};

enum UtteranceStatus {
    thresholdReached,
    timeoutreached
};

class VADEngine{
    public:
    VADEngine();
    ~VADEngine();
    void setInput(BlockingQueue<std::valarray<float>>* queue);
    void run();
    UtteranceStatus detectUtterance();
    void timeout_guard();

    private:
    VADParams params;
    BlockingQueue<std::valarray<float>>* inputQueue = NULL;

    //Utterance detection
    bool detecting = false;
    UtteranceStatus status;
    uint speech_frames, silence_frames;
    bool speechDetected;

    //Timeout guard
    std::mutex mutx;
    std::condition_variable cond;

    // Energy threshold update
    std::deque<float>* speechEnergy;
    std::deque<float>* nSpeechEnergy;
    

    float energy_th, fbar_th;
    

};

#endif