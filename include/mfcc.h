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
#include <ctime>
#include "blockingqueue.h"
#include "NE10.h"

typedef struct bin {
    int index;
    int size;
    float* vector;
};

typedef enum windows_type {
    blackman,
    povey,
    hamming,
    hann
};


class MFCC {

public:
    const double pi = 4*atan(1.0);   // Pi = 3.14...
    int size,rate=16000,fftSize,lowFrequency=40,highFrequency=7800; //Parameters with default values
    float preEmphCoef,*window,*lifter_coefs,cepstralLifter = 22.0,** matrix;
    int numCep = 40,numBins = 40;
    bin* bins;
    bool lift=true;
    enum windows_type type;
    BlockingQueue<int16_t*>* input_queue;

private:
    float hz2mel (float f);
    float mel2hz (float m);
    float* fft(float* frame);
    float* ifft(float* frame);
    void initBlackman(void);
    void initPovey(void);
    void initHann(void);
    void initHamming(void);
    void initMelFilterBank(void);
    float* computeMelFilterBank(float* powerSpect);
    void initDCTMatrix(void);
    float* computeDCT(float* vec);
    float computeAverage(int16_t* frame);
    float* preEmph(float* frame);
    float* lessAverage(int16_t* frame, float value);
    float computeEnergy(float* frame);
    float vecMul(float* vec1, float* vec2,int size);
    int32_t computeVecEnergy(int16_t* frame);
public:
    MFCC(size_t size,float coef,enum windows_type type);
    void computeFrame(int16_t* data);
    void setInput(BlockingQueue<int16_t*>* queue);
    void compute(void);
};