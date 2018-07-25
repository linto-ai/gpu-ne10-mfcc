#include "vadengine.h"

VADEngine::VADEngine()
{
    speechEnergy = new std::deque<float>(QUEUE_SIZE, 15);
    nSpeechEnergy = new std::deque<float>(QUEUE_SIZE, 5);

}

void VADEngine::setInput(BlockingQueue<std::valarray<float>>* queue)
{
    inputQueue = queue;
}

void VADEngine::run()
{
    std::valarray<float> values; // Hold the values passed from input
    float energy; // Current frame energy
    float fbar; // Current frame frequency ratio
    float mean_speech_en, mean_nSpeech_en; // Mean of the last X frames (speech and non-speech)

    while (true){
        values = inputQueue->pop();
        energy = values[0];
        fbar = values[1];

        // Detect speech
        if (energy > energy_th && fbar > params.fbar_th) // Is speech
        {
            if (detecting)
            {
                speech_frames++;
                //std::cout<< speech_frames << std::endl;
                silence_frames = 0;
                if (speech_frames >= params.speech_fram_tr) speechDetected = true; 
            }
            // Add energy to array for energy threshold update
            if (speechEnergy->size() >= QUEUE_SIZE)
            {
                speechEnergy->pop_front();
            }
            speechEnergy->push_back(energy);
        } else { // Is not speech
            if (detecting && speechDetected)
            {
                silence_frames++;
                if (silence_frames >= params.silence_fram_th)
                {
                    status = thresholdReached;
                    cond.notify_one();
                }
            }
            // Add energy to array for energy threshold update
            if (nSpeechEnergy->size() >= QUEUE_SIZE)
            {
                nSpeechEnergy->pop_front();
            }
            nSpeechEnergy->push_back(energy);
        }

        // Update energy threshold
        mean_speech_en = 0.0f;
        for (float value : *speechEnergy)
        {
            mean_speech_en += value;
        }
        mean_speech_en /= speechEnergy->size();
        mean_nSpeech_en = 0.0f;
        for (float value : *nSpeechEnergy)
        {
            mean_nSpeech_en += value;
        }
        mean_nSpeech_en /= nSpeechEnergy->size();
        energy_th = (mean_speech_en + mean_nSpeech_en) / 2.0f;

    }
}

UtteranceStatus VADEngine::detectUtterance()
{
    std::unique_lock<std::mutex> lock(mutx);
    speech_frames = 0;
    silence_frames = 0;
    speechDetected = false;
    std::cout << "Start detecting" << std::endl;
    detecting = true;
    std::thread timeout_thread(&VADEngine::timeout_guard, this);
    cond.wait(lock);
    detecting = false;
    timeout_thread.join();
    if (status == thresholdReached) 
    {
        std::cout << "Threshold reached" << std::endl;
    } else {
        std::cout << "Timeout" << std::endl;
    }
    return status;
}

void VADEngine::timeout_guard()
{
    std::cout << "Starting timeout_guard" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(params.timeout));
    if (!speechDetected)
    {
        std::cout << "Timeout !" << std::endl;
        status = timeoutreached;
        cond.notify_one();
    }
    
}