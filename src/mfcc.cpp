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
#include "../include/mfcc.h"

using namespace std;
// Hertz to Mel conversion
double MFCC::hz2mel (double f) {
    return 2595*std::log10 (1+f/700);
}

// Mel to Hertz conversion
double MFCC::mel2hz (double m) {
    return 700*(std::pow(10,m/2595)-1);
}

// FFT using NE10 library
int16_t* MFCC::fft(int16_t* frame,int length) {
    ne10_int32_t fftSize = length;
    ne10_fft_r2c_cfg_int16_t cfg = ne10_fft_alloc_r2c_int16(fftSize);      // Allocate a configuration structure for R2C/C2R FP32 NEON FFTs of size fftSize
    ne10_int16_t *fft_in          = (ne10_int16_t*)malloc(fftSize * sizeof(ne10_int16_t));         // Allocate an input array of samples
    ne10_fft_cpx_int16_t *fft_out = (ne10_fft_cpx_int16_t*)malloc(fftSize * sizeof(ne10_fft_cpx_int16_t)); // Allocate an (oversized) output array of samples
	// FFT
	for (int k=0;k<length;k++) {
		fft_in[k] = frame[k];
	}
    ne10_fft_r2c_1d_int16_neon(fft_out, fft_in, cfg, 0); // Call the R2C NEON implementation directly
    int16_t* out = (int16_t*)malloc(sizeof(int16_t)*2*length);
    for (int k=0;k<2*length;k+=2) {
		out[k] = fft_out[k].r;
        out[k+1] = fft_out[k].i;
	}
    NE10_FREE(fft_out);                    // Free the allocated output array
    NE10_FREE(fft_in);                     // Free the allocated input array
    ne10_fft_destroy_r2c_int16(cfg); // Free the allocated configuration structure
    return out;
}

//// Frame processing routines
// Pre-emphasis and Hamming window
int16_t* MFCC::preEmphHam(int16_t* frame, int16_t* hamming, size_t size) {
    int16_t* procFrame = (int16_t*)malloc(sizeof(int16_t)*size);
    procFrame[0] = hamming[0]*frame[0];
    for (int i=1; i<size; i++) {
        procFrame[i] = hamming[i] * (frame[i] - preEmphCoef * frame[i-1]);
    }
    return procFrame;
}

//// Frame processing routines
// Pre-emphasis and Hamming window
void MFCC::testNE10(int16_t* frame,int num,int fftSize) {
    int16_t* out = (int16_t*)malloc(sizeof(int16_t)*2*fftSize);
    clock_t begin = clock();
    for (int k=0;k<num;k++) {
        out = fft(frame,fftSize);
    }
    clock_t end = clock();
    double elapsed_secs = 1000 * double(end - begin) / CLOCKS_PER_SEC ;
    cout << "Temps moyen pour " << fftSize << ", " << elapsed_secs/num << " ms"<< endl;
}



// Power spectrum computation
void MFCC::computePowerSpec(void) {
}