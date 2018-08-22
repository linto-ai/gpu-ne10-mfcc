#include "vadengine.h"

VADEngine::VADEngine()
{
    speechEnergy = new std::deque<float>(QUEUE_SIZE, 30);
    nSpeechEnergy = new std::deque<float>(QUEUE_SIZE, 5);

}

void VADEngine::setInput(BlockingQueue<std::valarray<float>>* queue)
{
    inputQueue = queue;
}

void VADEngine::run()
{
    std::valarray<float> values;
    float energy;
    float fbar;
    float mean_speech_en, mean_nSpeech_en;
    while (true){
        values = inputQueue->pop();
        energy = values[0];
        fbar = values[1];

        // Detect speech
        if (energy > energy_th && fbar > fbar_th) // Is speech
        {
            if (speechEnergy->size() >= QUEUE_SIZE)
            {
                speechEnergy->pop_front();
            }
            speechEnergy->push_back(energy);
            //std::cout << "!!!! " << "En=" << energy << '/' << energy_th << "        \r";
        } else { // Is not speech
            if (nSpeechEnergy->size() >= QUEUE_SIZE)
            {
                nSpeechEnergy->pop_front();
            }
            nSpeechEnergy->push_back(energy);
            //std::cout << ".... " << "En=" << energy << '/' << energy_th << "            \r";
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