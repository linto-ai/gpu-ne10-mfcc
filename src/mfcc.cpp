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


MFCC::MFCC(size_t size,float coef,enum windows_type type) {
    this->size = size;
    preEmphCoef = coef;
    fftSize = 2 << (int16_t)log2(size);
    switch(type) {
    case blackman:
        initBlackman();
        break;
    case povey:
        initPovey();
        break;
    case hamming:
        initHamming();
        break;
    case hann:
        initHann();
        break;
    }
    initMelFilterBank();
    initDCTMatrix();
}

void MFCC::setInput(BlockingQueue<int16_t*>* queue){
    input_queue = queue;
}

// Hertz to Mel conversion
float MFCC::hz2mel (float f) {
    return 1127.0*std::log(1+f/700);
}

// Mel to Hertz conversion
float MFCC::mel2hz (float m) {
    return 700*(std::pow(10,m/2595)-1);
}


//Initialize the Hann window 
void MFCC::initHann(void) {
    window = (float*)(malloc(sizeof(float)*size));
    for (int i=0;i<size;i++) {
        window[i] = 0.54-0.46*cos(2*pi*i/(size));
    }
}

//Initialize the Blackman window
void MFCC::initBlackman(void) {
    window = (float*)(malloc(sizeof(float)*size));
    for (int i=0;i<size;i++) {
        window[i] = 0.42 - 0.5*cos(2*pi*i/size) +0.08*cos(4*pi*i/size);
    }
}

//Initialize the Povey window
void MFCC::initPovey(void) {
    window = (float*)(malloc(sizeof(float)*size));
    for (int i=0;i<size;i++) {
        window[i] = pow(0.5-0.5*cos(2*pi*i/(size-1)),0.85);
    }
}

//Initialize the Hamming window
void MFCC::initHamming(void) {
    window = (float*)(malloc(sizeof(float)*size));
    for (int i=0;i<size;i++) {
        window[i] = 0.54-0.46*cos(2*pi*i/(size));
    }
}

// Compute the average in float format of a specific frame
float MFCC::computeAverage(int16_t* frame) {
    float value = 0;
    for (int i=0;i<size;i++) {
        value += frame[i];
    }
    return value/size;
}

// Return the frame less average value for every sample
float* MFCC::lessAverage(int16_t* frame, float value) {
    float* new_frame = (float*)malloc(sizeof(float)*size);
    for (int i=0;i<size;i++) {
        new_frame[i]=frame[i]-value;
    }
    return new_frame;
}


// Compute the energy of the frame
float MFCC::computeEnergy(float* frame)
{
    float sum = 0;
    for (int i = 0; i < size; i++)
    {
         sum += frame[i]*frame[i];
    }
    return std::log(sum);
}

// Pre-emphasis and window
float* MFCC::preEmph(float* frame) {
    float* procFrame = (float*)malloc(sizeof(float)*size);
    procFrame[0] = window[0]*(frame[0]-preEmphCoef * frame[0]); // frame[0] -= preEmphCoef * frame[0];
    for (int i=1; i<size; i++) {    
        procFrame[i] = window[i] * (frame[i] - preEmphCoef * frame[i-1]);
    }
    return procFrame;
}

// Initialize coefs of Matrix for DCT computation and lifter vector
void MFCC::initDCTMatrix(void) {
    //TODO Use NE10 matrix
    matrix = new float*[numCep];
    lifter_coefs = (float*)malloc(sizeof(float)*numCep);
    for (int i = 0; i < numCep; ++i) {
        matrix[i] = new float[numBins]; 
        if (lift) {
            lifter_coefs[i] = 1.0+0.5*cepstralLifter*std::sin(pi*i/cepstralLifter);
        }
        else {
            lifter_coefs[i]=1.0;
        }
    }
    for (int j=0;j<numBins;j++) {
        matrix[0][j] = std::sqrt(1.0/numBins);
    }
    for (int k =1 ;k<numCep;k++) {
        for (int n=0;n<numBins;n++) {
            matrix[k][n] = std::sqrt(2.0/numBins) * std::cos(pi/numBins*(n+0.5)*k);
        }
    }
}

// Compute the DCT using matrix and vector multiplication
float* MFCC::computeDCT(float* vec) {
    //TODO Use NE10 matrix / vector and relatives functions
    float* mfcc = (float*)malloc(sizeof(float)*numCep);
    for (int i=0 ;i<numCep;i++) {
        mfcc[i] = 0;
        for (int j=0;j<numBins;j++) {
            mfcc[i] += matrix[i][j] * vec[j];
        }
        mfcc[i] *= lifter_coefs[i];
    }
    return mfcc;
}

// FFT using NE10 library
float* MFCC::fft(float* frame) {
    ne10_int32_t fftSize = this->fftSize;
    ne10_fft_r2c_cfg_float32_t cfg = ne10_fft_alloc_r2c_float32(fftSize);      // Allocate a configuration structure for R2C/C2R FP32 NEON FFTs of size fftSize
    ne10_float32_t *fft_in          = (ne10_float32_t*)malloc(fftSize * sizeof(ne10_float32_t));         // Allocate an input array of samples
    ne10_fft_cpx_float32_t *fft_out = (ne10_fft_cpx_float32_t*)malloc(fftSize * sizeof(ne10_fft_cpx_float32_t)); // Allocate an (oversized) output array of samples
	// FFT
	for (int k=0;k<size;k++) {
		fft_in[k] = frame[k];
	}
    ne10_fft_r2c_1d_float32_neon(fft_out, fft_in, cfg); // Call the R2C NEON implementation directly
    float* out = (float*)malloc(sizeof(float)*fftSize); 
    for (int k=0;k<fftSize/2;k++) {
		out[2*k] = fft_out[k].r; // Real part
        out[2*k+1] = fft_out[k].i; // Imaginary part
	}
    NE10_FREE(fft_out);                    // Free the allocated output array
    NE10_FREE(fft_in);                     // Free the allocated input array
    ne10_fft_destroy_r2c_float32(cfg); // Free the allocated configuration structure
    return out;
}

// IFFT using NE10 library
float* MFCC::ifft(float* frame) {
    ne10_int32_t fftSize = this->fftSize;
    ne10_fft_r2c_cfg_float32_t cfg = ne10_fft_alloc_r2c_float32(fftSize);      // Allocate a configuration structure for R2C/C2R FP32 NEON FFTs of size fftSize
    ne10_fft_cpx_float32_t *fft_in = (ne10_fft_cpx_float32_t*)malloc(fftSize * sizeof(ne10_fft_cpx_float32_t));         // Allocate an input array of samples
    ne10_float32_t  *fft_out = (ne10_float32_t*)malloc(fftSize * sizeof(ne10_float32_t)); // Allocate an (oversized) output array of samples
    for (int k=0;k<fftSize;k++) {
		fft_in[k].r = frame[k];
        fft_in[k].i = 0;
	}
    ne10_fft_c2r_1d_float32_neon(fft_out, fft_in, cfg); // Call the R2C NEON implementation directly
    float* out = (float*)malloc(sizeof(float)*fftSize); 
    for (int k=0;k<fftSize/2;k++) {
		out[k] = fft_out[k];
	}
    NE10_FREE(fft_out);                    // Free the allocated output array
    NE10_FREE(fft_in);                     // Free the allocated input array
    ne10_fft_destroy_r2c_float32(cfg); // Free the allocated configuration structure
    return out;
}


void MFCC::initMelFilterBank(void) {
    bins = (bin*)malloc(sizeof(bin)*numBins);
    float* output = (float*)malloc(sizeof(float)*2*numBins);
    float fft_bins = fftSize/2;
    float* this_bin= (float*)malloc(sizeof(float)*fft_bins);
    float fft_bin_width = (float)rate/(float)fftSize;
    float mel_low = hz2mel(lowFrequency);
    float mel_high = hz2mel(highFrequency);
    float mel_delta = (mel_high - mel_low)/(numBins+1);
    for (int bin = 0; bin < numBins;bin++) {
        memset(this_bin,0.0f,fft_bins);
        float left_mel = mel_low + mel_delta * bin;
        float center_mel = mel_low + mel_delta * (bin+1);
        float right_mel = mel_low + mel_delta * (bin+2);
        int first_index = -1;
        int last_index = -1;
        for (int i = 0;i<fft_bins;i++) {
            float mel = hz2mel(fft_bin_width * i);
            if (mel > left_mel && mel < right_mel) {
                if (mel <= center_mel) {
                    this_bin[i]=(mel - left_mel)/(center_mel - left_mel);
                } 
                else {
                    this_bin[i]=(right_mel - mel)/(right_mel - center_mel);
                }
                if (first_index == -1) {
                    first_index = i;
                }
                last_index = i;
            }
        }
        bins[bin].index = first_index;
        bins[bin].vector = (float*)malloc(sizeof(float)*(last_index + 1 - first_index));
        bins[bin].size = last_index + 1 - first_index;
        for (int k=0;k<bins[bin].size;k++) {
            bins[bin].vector[k] = this_bin[k+first_index];
        }
    }
    free(output);
    free(this_bin);
}


float* MFCC::computeMelFilterBank(float* powerSpect) {
    float* mel_energies = (float*)malloc(sizeof(float)*numBins);
    for (int i=0;i<numBins;i++) {
        int offset = bins[i].index;
        float* vec = bins[i].vector;
        mel_energies[i] = log(vecMul(vec,powerSpect+offset,bins[i].size));
    }
    return mel_energies;
}



float MFCC::vecMul(float* vec1, float* vec2,int size) {
    float output = 0;
    for (int i=0;i<size;i++) {
        output += vec1[i]*vec2[i];
    }
    return output;
}




void MFCC::computeFrame(int16_t* data) {
    const clock_t begin_time = clock();
    // Computation of one frame using Kaldi pipeline
    float average = computeAverage(data);  // Frame average
    float* dataF = lessAverage(data,average); // Less average value
    float energy = computeEnergy(dataF); // Energ
    float* postWindow = (float*)malloc(sizeof(float)*size);
    float* powerSpec = (float*)malloc(sizeof(float)*size/2);
    postWindow = preEmph(dataF); // Povey window and pre-emphasis
    float* fftIn = (float*)malloc(sizeof(float)*fftSize);
    fftIn = postWindow;
    memset(fftIn+size,0,fftSize-size);
    float* fftOut = fft(fftIn); // FFT using NE10
    float first_energy = fftOut[0] * fftOut[0];
    float last_energy = fftOut[1] * fftOut[1];
    for (int i=1;i<fftSize/2;i++) {
        powerSpec[i] = fftOut[2*i]*fftOut[2*i] + fftOut[2*i+1]*fftOut[2*i+1]; //Computation of Power Spectrum
    }
    powerSpec[0] = first_energy;
    powerSpec[fftSize/2-1] = last_energy;
    float* log_mel_energies = computeMelFilterBank(powerSpec); // Multiplication with MelFilterBank matrix
    float* dct_out = computeDCT(log_mel_energies); // Compute DCT
    float* mfcc = (float*)malloc(sizeof(float)*numCep);
    mfcc[0] = energy;
    for (int i=1;i<numCep;i++) { // Get MFCC
        mfcc[i] = dct_out[i];
    }
    std::cout << "Time in ms: " << float( clock () - begin_time ) /(CLOCKS_PER_SEC)*1000<< endl;
}


void MFCC::compute(void) {

}



// 




