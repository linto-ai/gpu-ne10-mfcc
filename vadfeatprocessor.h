#ifndef VADFEATPROCESSOR_H
#define VADFEATPROCESSOR_H
#include <cmath>
#include <complex>
#include <valarray>
#include <iostream>

#include "blockingqueue.h"

const double PI = 3.14159265358979323846264338328L;
typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;
typedef std::valarray<double> DArray;

struct VADFeatsParams{
    u_int32_t samplingRate = 16000;
    u_int16_t window_width = 1024; //TODO: Change to 2048
    u_int16_t overlap = 0;
    u_int16_t fbar_lfreq = 80;
    u_int16_t fbar_hfreq = 1000;
};

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