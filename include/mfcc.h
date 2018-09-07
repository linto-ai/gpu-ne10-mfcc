/**
 * Copyright (C) 2018 Linagora
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <algorithm>
#include <numeric>
#include <complex>
#include <vector>
#include <map>
#include <math.h>
#include "NE10.h"

class MFCC {

private:
    const double pi = 4*atan(1.0);   // Pi = 3.14...
    size_t frameShiftSamples, numCepstra, numFFT, numFFTBins, numFilters;
    size_t winLengthSamples;
    double preEmphCoef, lowFreq, highFreq;
    int16_t* frame;
    float *hamming,*blackman,*povey;
    int fcoup[27] = {0,100,200,300,400,500,600,700,800,900,1000,1150,1300,1500,1700,2000,2350,2700,3100,3550,4000,4500,5050,5600,6200,6850,7500};

private:
    inline double hz2mel (double f);
    inline double mel2hz (double m);
    float* fft(float* frame, int length);
    float* computePowerSpec(float* frame, int size);
    float* ifft(float* frame,int ifftSize);
    void MelFilterBank(void);
    float* MelFilterBank(float* powerSpect, int size,int rate, int numSpec);
    void initBlackman(void);
    void initPovey(void);
public:
    MFCC(size_t winLengthSamples,double coef);
    float computeAverage(int16_t* frame,int size);
    float* preEmphPov(float* frame, size_t size);
    float* lessAverage(int16_t* frame,int size, float value);
    float computeEnergy(float* window,int size);
    int32_t computeVecEnergy(int16_t* frame);
    void test(int16_t* data,int size);
    
};