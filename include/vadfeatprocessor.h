#ifndef VADFEATPROCESSOR_H
#define VADFEATPROCESSOR_H
#include <cmath>
#include <complex>
#include <valarray>
#include <iostream>

#include "blockingqueue.h"
#include "features.h"

typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;
typedef std::valarray<double> DArray;

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

    /**
     * @brief Compute log2 energy of an array.
     * 
     * @param window array of value to process
     * @return double log2 energy
     */
    double _computeEnergy(int16_t* window);
    float _computeFBAR(int16_t* window);
    DArray _computeSpectrum(int16_t* window);
    CArray _arrayToCArray(int16_t* window);

};

#endif