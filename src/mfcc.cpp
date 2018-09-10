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

MFCC::MFCC(size_t winLengthSamples,double coef) {
    this->winLengthSamples = winLengthSamples;
    preEmphCoef = coef;
    initBlackman();
    initPovey();
}

// Hertz to Mel conversion
float MFCC::hz2mel (float f) {
    return 1127.0*std::log(1+f/700);
}

// Mel to Hertz conversion
float MFCC::mel2hz (float m) {
    return 700*(std::pow(10,m/2595)-1);
}



void MFCC::initBlackman(void) {
    blackman = (float*)(malloc(sizeof(float)*winLengthSamples));
    for (int i=0;i<winLengthSamples;i++) {
        blackman[i] = 0.42 - 0.5*cos(2*pi*i/winLengthSamples) +0.08*cos(4*pi*i/winLengthSamples);
    }
}

void MFCC::initPovey(void) {
    povey = (float*)(malloc(sizeof(float)*winLengthSamples));
    for (int i=0;i<winLengthSamples;i++) {
        povey[i] = pow(0.5-0.5*cos(2*pi*i/(winLengthSamples-1)),0.85);
    }
}

float MFCC::computeAverage(int16_t* frame,int size) {
    float value = 0;
    for (int i=0;i<size;i++) {
        value += frame[i];
    }
    return value/size;
}

float* MFCC::lessAverage(int16_t* frame,int size, float value) {
    float* new_frame = (float*)malloc(sizeof(float)*size);
    for (int i=0;i<size;i++) {
        new_frame[i]=frame[i]-value;
    }
    return new_frame;
}


float MFCC::computeEnergy(float* window,int size)
{
    float sum = 0;
    for (int i = 0; i < size; i++)
    {
         sum += std::pow(window[i], 2);
    }
    return std::log2(sum / (double)size);
}

int32_t MFCC::computeVecEnergy(int16_t* frame) {
    //TODO
    return 0;
}

//// Frame processing routines
// Pre-emphasis and Hamming window
float* MFCC::preEmphPov(float* frame, size_t size) {
    float* procFrame = (float*)malloc(sizeof(float)*size);
    procFrame[0] = povey[0]*(frame[0]-preEmphCoef * frame[0]); // frame[0] -= preEmphCoef * frame[0];
    for (int i=1; i<size; i++) {    
        procFrame[i] = povey[i] * (frame[i] - preEmphCoef * frame[i-1]);
    }
    return procFrame;
}

// Power spectrum computation
float* MFCC::computePowerSpec(float* frame, int size) {
    float* fft_out = (float*)malloc(sizeof(float)*size); 
    float* powerSpectralCoef = (float*)malloc(sizeof(float)*size);
    for (int k=0; k < size; k++) {
        frame[k] *= hamming[k];
    }
    fft_out = fft(frame,size);
    for (int i=0; i<size/2; i++) { 
        complex<int16_t> c (fft_out[2*i],fft_out[2*i+1]);
        powerSpectralCoef[i] = abs(c); //keep classic square
    }
    free(fft_out);
    return powerSpectralCoef; 
}

// FFT using NE10 library
float* MFCC::fft(float* frame,int length) {
    ne10_int32_t fftSize = length;
    ne10_fft_r2c_cfg_float32_t cfg = ne10_fft_alloc_r2c_float32(fftSize);      // Allocate a configuration structure for R2C/C2R FP32 NEON FFTs of size fftSize
    ne10_float32_t *fft_in          = (ne10_float32_t*)malloc(fftSize * sizeof(ne10_float32_t));         // Allocate an input array of samples
    ne10_fft_cpx_float32_t *fft_out = (ne10_fft_cpx_float32_t*)malloc(fftSize * sizeof(ne10_fft_cpx_float32_t)); // Allocate an (oversized) output array of samples
	// FFT
	for (int k=0;k<length;k++) {
		fft_in[k] = frame[k];
	}
    ne10_fft_r2c_1d_float32_neon(fft_out, fft_in, cfg); // Call the R2C NEON implementation directly
    float* out = (float*)malloc(sizeof(float)*length); 
    for (int k=0;k<length/2;k++) {
		out[2*k] = fft_out[k].r; // Real part
        out[2*k+1] = fft_out[k].i; // Imaginary part
	}
    NE10_FREE(fft_out);                    // Free the allocated output array
    NE10_FREE(fft_in);                     // Free the allocated input array
    ne10_fft_destroy_r2c_float32(cfg); // Free the allocated configuration structure
    return out;
}

float* MFCC::ifft(float* frame,int ifftSize) {
    ne10_int32_t fftSize = ifftSize;
    ne10_fft_r2c_cfg_float32_t cfg = ne10_fft_alloc_r2c_float32(fftSize);      // Allocate a configuration structure for R2C/C2R FP32 NEON FFTs of size fftSize
    ne10_fft_cpx_float32_t *fft_in          = (ne10_fft_cpx_float32_t*)malloc(fftSize * sizeof(ne10_fft_cpx_float32_t));         // Allocate an input array of samples
    ne10_float32_t  *fft_out = (ne10_float32_t*)malloc(fftSize * sizeof(ne10_float32_t)); // Allocate an (oversized) output array of samples
    for (int k=0;k<ifftSize;k++) {
		fft_in[k].r = frame[k];
        fft_in[k].i = 0;
	}
    ne10_fft_c2r_1d_float32_neon(fft_out, fft_in, cfg); // Call the R2C NEON implementation directly
    float* out = (float*)malloc(sizeof(float)*ifftSize); 
    for (int k=0;k<ifftSize/2;k++) {
		out[k] = fft_out[k];
	}
    NE10_FREE(fft_out);                    // Free the allocated output array
    NE10_FREE(fft_in);                     // Free the allocated input array
    ne10_fft_destroy_r2c_float32(cfg); // Free the allocated configuration structure
    return out;
}



float* MFCC::MelFilterBank(float* powerSpect, int size,int rate, int numSpec) {
    float* x2 = (float*)malloc(sizeof(float)*size/2);
    float* x1 = (float*)malloc(sizeof(float)*size/2);
    int16_t* nm = (int16_t*)malloc(sizeof(int16_t)*size/2);
    float* x2sig = (float*)malloc(sizeof(float)*size/2);
    float* x1sig = (float*)malloc(sizeof(float)*size/2);
    float* vecteur = (float*)malloc(sizeof(float)*(numSpec+1));
    cout << "After init" << endl;
    for (int i = 1; i < size/2+1;i++) {
        nm[i-1]=0;
        x1[i-1]=0;
        float f=(rate/1000*500*i)/(size/2);
        //cout << "numSpec: " << numSpec << endl;
        for (int n = 1; n<(numSpec+2);n++) {
            if (f>=fcoup[n-1] && f<fcoup[n]) {
                nm[i-1]=n;
                x1[i-1]=(f-fcoup[n-1])/(fcoup[n]-fcoup[n-1]);
                x2[i-1]=1-x1[i-1];
            } 
            //cout << "i: " << i << " n: " << n << endl;  
        }
    }
    cout << "After 1st loop" << endl;
    for (int m=0;m<size/2;m++) {
        x2sig[m]=x2[m]*powerSpect[m];
        x1sig[m]=x1[m]*powerSpect[m];
    }
    cout << "After 2nd loop" << endl;
    for (int i=1;i<size/2;i++) {
        if (nm[i-1]-2 > -1) {
            vecteur[nm[i-1]-2]+=x2sig[i-1];
        }  
        if (nm[i-1]-1 > -1 && nm[i-1]-1 <= numSpec-1) {
            vecteur[nm[i-1]-1]+=x1sig[i-1];
        }   
    }
    cout << "After 3rd loop" << endl;
    free(x2);
    free(x1);
    free(nm);
    free(x2sig);
    free(x1sig);
    return vecteur;    
}

float* MFCC::MelFilterBankKaldi(float* powerSpect, int size, int rate, int numBins,int fftSize,int lowFrequency,int highFrequency) {
    float* output = (float*)malloc(sizeof(float)*2*numBins);
    bin* bins = (bin*)malloc(sizeof(bin)*numBins);
    float fft_bins = fftSize/2;
    float* this_bin= (float*)malloc(sizeof(float)*fft_bins);
    //int nyquist = rate/2;
    //CHECK value
    float fft_bin_width = (float)rate/(float)fftSize;
    float mel_low = hz2mel(lowFrequency);
    float mel_high = hz2mel(highFrequency);
    float mel_delta = (mel_high - mel_low)/(numBins+1);
    //cout << "Low " << mel_low << " High " << mel_high << " Delta " << mel_delta;
    for (int bin = 0; bin < numBins;bin++) {
        memset(this_bin,0.0f,fft_bins);
        float left_mel = mel_low + mel_delta * bin;
        float center_mel = mel_low + mel_delta * (bin+1);
        float right_mel = mel_low + mel_delta * (bin+2);
        //cout << "Left " << left_mel << " Center " << center_mel << " Right " << right_mel;
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
                //cout << "Weight: " << this_bin[i] << "Left " << left_mel<< "Right" << right_mel << endl;
                if (first_index == -1) {
                    first_index = i;
                }
                last_index = i;
            }
        }
        //cout << first_index << endl;
        bins[bin].index = first_index;
        bins[bin].vector = (float*)malloc(sizeof(float)*(last_index + 1 - first_index));
        //bins[bin].vector = (float*)malloc(sizeof(float)*fft_bins);
        //memcpy(bins[bin].vector,this_bin+first_index,last_index + 1 - first_index);
        cout << "[";
        for (int k=0;k<last_index + 1 - first_index;k++) {
            bins[bin].vector[k] = this_bin[k+first_index];
            cout << bins[bin].vector[k] << " " ;
        }
        cout << "]" <<endl;
    }
    //CHECK INDEX;
    
    return output;
}


void MFCC::test(int16_t* data,int size) {
    float average = computeAverage(data,size);
    //cout << "Average : " << average << endl;
    float* dataF = lessAverage(data,size,average);
    float energy = computeEnergy(dataF,size);
    //cout << "Log energy:" << energy << endl; 
    float* postPov = (float*)malloc(sizeof(float)*size);
    float* powerSpec = (float*)malloc(sizeof(float)*size/2);
    postPov = preEmphPov(dataF, (size_t)size);
    //cout << "Emph + Povey >>" << endl;
    /*for (int i=0;i<size;i++) {
        cout << postPov[i] << "    ";
    }*/
    int16_t fftSize = 2 << (int16_t)log2(size);
    //cout << "FFT Size:" << fftSize <<   "Size: " << size << endl;
    float* fftIn = (float*)malloc(sizeof(float)*fftSize);
    fftIn = postPov;
    memset(fftIn+size,0,fftSize-size);
    float* fftOut = fft(fftIn,fftSize);
    //cout << "Abs after FFT :" << endl;
    float first_energy = fftOut[0] * fftOut[0];
    float last_energy = fftOut[1] * fftOut[1];
    for (int i=1;i<fftSize/2;i++) {
        powerSpec[i] = fftOut[2*i]*fftOut[2*i] + fftOut[2*i+1]*fftOut[2*i+1];
    }
    powerSpec[0] = first_energy;
    powerSpec[fftSize/2-1] = last_energy;
    float* melVec = MelFilterBankKaldi(powerSpec,size,16000,40,fftSize,40,7800);
    //cout << "After Mel Filter Bank" << endl;
    /*for (int i=0;i<24;i++) {  
        melVec[i] = log(melVec[i]);
        //cout << melVec[i] << "    ";
    }*/
    // DCT OR IFFT
}  
// 




