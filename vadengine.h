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
    timeoutreached,
    canceled
};

class VADEngine{
    public:
    VADEngine();
    ~VADEngine();

    /**
     * @brief Set the Input object
     * 
     * @param queue The expected input is a valarray with energy as the first value and fbar as the second value.
     */

    void setInput(BlockingQueue<std::valarray<float>>* queue);
    /**
     * @brief Run the vadengin thread, update speech energy threshold continiously.
     * 
     */
    void run();

    /**
     * @brief Start utterance detection.
     * 
     * @return UtteranceStatus.timeout if no speech has been detected during the timeout delay or UtteranceStatus.thresholdReached if speech has been detected.
     */
    UtteranceStatus detectUtterance();

    void cancelUtteranceDetection();

    /**
     * @brief Set a timer thread that stop the detectUtterance after a specified delay if no speech is detected.
     * 
     */
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