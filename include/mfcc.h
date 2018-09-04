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
    const double PI = 4*atan(1.0);   // Pi = 3.14...
    size_t winLengthSamples, frameShiftSamples, numCepstra, numFFT, numFFTBins, numFilters;
    double preEmphCoef, lowFreq, highFreq;
    int16_t* frame,hamming;

private:
    // Hertz to Mel conversion
    inline double hz2mel (double f);
    // Mel to Hertz conversion
    inline double mel2hz (double m);
    // Cooley-Tukey DIT-FFT recursive function
    int16_t* fft(int16_t* frame, int length);
    //// Frame processing routines
    // Pre-emphasis and Hamming window
    int16_t* preEmphHam(int16_t* frame, int16_t* hamming, size_t size);
    
    // Power spectrum computation
    void computePowerSpec(void);
    // Applying log Mel filterbank (LMFB)
public: 
    void testNE10(int16_t* frame,int num,int fftSize);
};