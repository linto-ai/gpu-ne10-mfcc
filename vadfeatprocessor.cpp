#include "vadfeatprocessor.h"

VADFeatProcessor::VADFeatProcessor()
{
    // Setting cut frequencies indexes
    float i = 0.0;
    float step = (params.samplingRate / 2.0) / (params.window_width / 2.0);
    while (i * step < params.fbar_lfreq) i++;
    fbar_lfreq_index = (i-1);
    while (i *step < params.fbar_hfreq) i++;
    fbar_hfreq_index = i;
    isActive = true;
}

void VADFeatProcessor::set_input(BlockingQueue<int16_t*>* queue){
    input_queue = queue;
}

void VADFeatProcessor::run(){
    int16_t* input;
    std::valarray<float> res(2);
    while(true)
    {
        input = input_queue->pop();
        //TODO add buffer
        if (isActive) 
        {
            res[0] = computeEnergy(input, params);
            res[1] = computeFBAR(input, fbar_lfreq_index, fbar_hfreq_index, params);
            //std::cout << "E: " << res[0] << std::endl;
            //std::cout << "FBAR: " << res[1] << std::endl;
        }
        
        for (BlockingQueue<std::valarray<float>>* queue : outputQueues)
        {
            queue->push(res);
        }
    }
}

BlockingQueue<std::valarray<float>>* VADFeatProcessor::subscribe()
{
    BlockingQueue<std::valarray<float>>* queue = new BlockingQueue<std::valarray<float>>(10);
    outputQueues.push_back(queue);
    return queue;
}


