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

/**
* Init MFCC class
* Param: size of samples window
* Param: number of samples for sliding window
* Param: type of window, Povey, Hamming, Hann, Blackman
*/
MFCC::MFCC(int size,int sliding_samples, enum windows_type type) {
    this->size = size;
    this->sliding_samples=sliding_samples;
    fft_size = 2 << (int16_t)log2(size);
    window = new float[size];
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
    mfcc = new float[num_cep]();
    mel_energies = new float[num_bins]();
    power_spec = new float[fft_size]();
    fft_vector = new float[fft_size]();
    data_float = new float[size]();
    lifter_coefs = new float[num_cep]();
    output_queue = new BlockingQueue<float*>(20);
    initMelFilterBank();
    initDCTMatrix();
}

/**
* Define input queue for audio data
* Param: audio queue from Pulse Audio
*/
void MFCC::setInput(BlockingQueue<int16_t*>* queue){
    input_queue = queue;
}

/**
* Return the conversion of hertz scale to mel scale
* Param: Frequency in hertz
*/
float MFCC::hz2mel (float f) {
    return 1127.0*std::log(1+f/700);
}

/**
* Return the conversion of mel scale to hertz scale
* Param: Frequency in mel
*/
float MFCC::mel2hz (float m) {
    return 700*(std::pow(10,m/2595)-1);
}


/**
* Initialize the Hann window
*/
void MFCC::initHann(void) {
    for (int i=0;i<size;i++) {
        window[i] = 0.54-0.46*cos(2*pi*i/(size));
    }
}

/**
* Initialize the Blackman window
*/
void MFCC::initBlackman(void) {
    for (int i=0;i<size;i++) {
        window[i] = 0.42 - 0.5*cos(2*pi*i/size) +0.08*cos(4*pi*i/size);
    }
}

/**
* Initialize the Povey window
*/
void MFCC::initPovey(void) {
    for (int i=0;i<size;i++) {
        window[i] = pow(0.5-0.5*cos(2*pi*i/(size-1)),0.85);
    }
}

/**
* Initialize the Hamming window
*/
void MFCC::initHamming(void) {
    for (int i=0;i<size;i++) {
        window[i] = 0.54-0.46*cos(2*pi*i/(size));
    }
}

/**
 * Compute the average float value of a specific frame
 * Param: Frame with "size" samples
 */
float MFCC::computeAverage(int16_t* frame) {
    float value = 0;
    for (int i=0;i<size;i++) {
        value += frame[i];
    }
    return value/size;
}

/**
 * Return the frame less average value for every sample
 * Param: Frame with "size" samples
 * Param: Average value of the frame
 */
void MFCC::lessAverage(float* new_frame, float value) {
    for (int i=0;i<size;i++) {
        new_frame[i]-=-value;
    }
}


/**
 * Compute the energy of the frame
 * Param: Frame with "size" samples
 */
float MFCC::computeEnergy(float* frame)
{
    float sum = 0;
    for (int i = 0; i < size; i++)
    {
         sum += frame[i]*frame[i];
    }
    return std::log(sum);
}

/**
 * Compute the pre-emphasis and window computation
 * Param: Frame with "size" samples
 */
void MFCC::preEmph(float* frame) {
    float* frame_save = new float[size];
    for (int i = 0;i<size;i++) {
        frame_save[i] = frame[i];
    }
    frame[0] = window[0]*(frame_save[0]-pre_emph_coef * frame_save[0]); // frame[0] -= pre_emph_coef * frame[0];
    for (int i=1; i<size; i++) {    
        frame[i] = window[i] * (frame_save[i] - pre_emph_coef * frame_save[i-1]);
    }
    free(frame_save);
}

/**
 * Initialize coefs of Matrix for DCT computation and lifter vector
 */
void MFCC::initDCTMatrix(void) {
    //TODO Use NE10 matrix
    matrix = new float*[num_cep];
    for (int i = 0; i < num_cep; ++i) {
        matrix[i] = new float[num_bins]; 
        if (lift) {
            lifter_coefs[i] = 1.0+0.5*cepstral_lifter_coef*std::sin(pi*i/cepstral_lifter_coef);
        }
        else {
            lifter_coefs[i]=1.0;
        }
    }
    for (int j=0;j<num_bins;j++) {
        matrix[0][j] = std::sqrt(1.0/num_bins);
    }
    for (int k =1 ;k<num_cep;k++) {
        for (int n=0;n<num_bins;n++) {
            matrix[k][n] = std::sqrt(2.0/num_bins) * std::cos(pi/num_bins*(n+0.5)*k);
        }
    }
}

/**
 * Compute the DCT using matrix and vector multiplication
 * Param: Frame with "fft_size" samples
 */
void MFCC::computeDCT(float* vec) {
    //TODO Use NE10 matrix / vector and relatives functions
    for (int i=0 ;i<num_cep;i++) {
        mfcc[i] = 0;
        for (int j=0;j<num_bins;j++) {
            mfcc[i] += matrix[i][j] * vec[j];
        }
        mfcc[i] *= lifter_coefs[i];
    }
}
/**
 * Compute the FFT using NE10 library
 * Param: Frame with "size" samples
 */
void MFCC::fft(float* frame) {
    ne10_int32_t ne_size = fft_size;
    ne10_fft_r2c_cfg_float32_t cfg = ne10_fft_alloc_r2c_float32(ne_size);      // Allocate a configuration structure for R2C/C2R FP32 NEON FFTs of size fft_size
    ne10_float32_t *fft_in          = (ne10_float32_t*)malloc(ne_size * sizeof(ne10_float32_t));         // Allocate an input array of samples
    ne10_fft_cpx_float32_t *fft_out = (ne10_fft_cpx_float32_t*)malloc(ne_size * sizeof(ne10_fft_cpx_float32_t)); // Allocate an (oversized) output array of samples
	// FFT
	for (int k=0;k<fft_size;k++) {
		fft_in[k] = frame[k];
	}
    ne10_fft_r2c_1d_float32_neon(fft_out, fft_in, cfg); // Call the R2C NEON implementation directly

    for (int k=0;k<fft_size/2;k++) {
		frame[2*k] = fft_out[k].r; // Real part
        frame[2*k+1] = fft_out[k].i; // Imaginary part
	}
    NE10_FREE(fft_out);                    // Free the allocated output array
    NE10_FREE(fft_in);                     // Free the allocated input array
    ne10_fft_destroy_r2c_float32(cfg); // Free the allocated configuration structure
}

/**
 * Compute the IFFT using NE10 library
 * Param: Frame with "fft_size" samples
 */
float* MFCC::ifft(float* frame) {
    ne10_int32_t ne_size = this->fft_size;
    ne10_fft_r2c_cfg_float32_t cfg = ne10_fft_alloc_r2c_float32(ne_size);      // Allocate a configuration structure for R2C/C2R FP32 NEON FFTs of size fft_size
    ne10_fft_cpx_float32_t *fft_in = (ne10_fft_cpx_float32_t*)malloc(ne_size * sizeof(ne10_fft_cpx_float32_t));         // Allocate an input array of samples
    ne10_float32_t  *fft_out = (ne10_float32_t*)malloc(ne_size * sizeof(ne10_float32_t)); // Allocate an (oversized) output array of samples
    for (int k=0;k<fft_size;k++) {
		fft_in[k].r = frame[k];
        fft_in[k].i = 0;
	}
    ne10_fft_c2r_1d_float32_neon(fft_out, fft_in, cfg); // Call the R2C NEON implementation directly
    float* out = (float*)malloc(sizeof(float)*fft_size); 
    for (int k=0;k<fft_size/2;k++) {
		out[k] = fft_out[k];
	}
    NE10_FREE(fft_out);                    // Free the allocated output array
    NE10_FREE(fft_in);                     // Free the allocated input array
    ne10_fft_destroy_r2c_float32(cfg); // Free the allocated configuration structure
    return out;
}

/**
 * Initialize the mel filter bank matrix
 */
void MFCC::initMelFilterBank(void) {
    bins = (bin*)malloc(sizeof(bin)*num_bins);
    float* output = (float*)malloc(sizeof(float)*2*num_bins);
    float fft_bins = fft_size/2;
    float* this_bin= (float*)malloc(sizeof(float)*fft_bins);
    float fft_bin_width = (float)rate/(float)fft_size;
    float mel_low = hz2mel(low_frequency);
    float mel_high = hz2mel(high_frequency);
    float mel_delta = (mel_high - mel_low)/(num_bins+1);
    for (int bin = 0; bin < num_bins;bin++) {
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

/**
 * Compute the mel filter bank 
 * Param: Frame with "fft_size" samples
 */
void MFCC::computeMelFilterBank(float* powerSpect) {
    for (int i=0;i<num_bins;i++) {
        int offset = bins[i].index;
        float* vec = bins[i].vector;
        mel_energies[i] = log(vecMul(vec,powerSpect+offset,bins[i].size));
    }
}


/**
 * Compute the multiplication of 2 vectors
 * Param: First vector
 * Param: Second vector
 * Size of vector
 */
float MFCC::vecMul(float* vec1, float* vec2,int size) {
    float output = 0;
    for (int i=0;i<size;i++) {
        output += vec1[i]*vec2[i];
    }
    return output;
}

/**
 * Compute the MFCC for one frame
 * Param: Frame with "size" samples
 */
void MFCC::computeFrame(int16_t* data) {
    for (int i=0;i<size;i++) {
        data_float[i] = (float)data[i];
    }
    average = computeAverage(data);  // Frame average
    lessAverage(data_float,average);
    energy = computeEnergy(data_float); // Energy
    preEmph(data_float); // Povey window and pre-emphasis
    for (int i=0;i<size;i++) {
        fft_vector[i] = data_float[i];
    }
    for (int i=size;i<fft_size;i++) {
        fft_vector[i] = 0;
    }
    fft(fft_vector); // FFT using NE10
    first_energy = fft_vector[0] * fft_vector[0];
    last_energy = fft_vector[1] * fft_vector[1];
    for (int i=1;i<fft_size/2;i++) {
        power_spec[i] = fft_vector[2*i]*fft_vector[2*i] + fft_vector[2*i+1]*fft_vector[2*i+1]; //Computation of Power Spectrum
    }
    power_spec[0] = first_energy;
    power_spec[fft_size/2-1] = last_energy;
    computeMelFilterBank(power_spec); // Multiplication with MelFilterBank matrix
    computeDCT(mel_energies); // Compute DCT
    mfcc[0] = energy; 
}

/**
 * Compute the MFCC for all frames
 * Param: Frame with "size" samples
 */
void MFCC::compute(void) {
    int16_t* input;
    while(true)
    {
        input = input_queue->pop(); //2*size - sliding_samples
        computeFrame(input);
        output_queue->push(mfcc);
        usleep(100);
        computeFrame(input+size-sliding_samples);
        output_queue->push(mfcc);
    }
}

/**
 * Returns the MFCC queue
 */
BlockingQueue<float*>* MFCC::subscribe(void)
{
   return output_queue;
}







