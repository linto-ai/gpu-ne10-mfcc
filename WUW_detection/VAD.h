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
//#include <mygpu_fft_lib.h>
#include "Audio.h"
#define NELEM(x) (sizeof(x)/sizeof(x[0]))

uint64_t Min(uint64_t *array);
uint64_t Energy(int16_t* frame);
float GeometricMean(double *arr, int size);
void ComputeFBAR(double* spectrum,int freq_low, int freq_high,double* R);
uint64_t ComputeSFM(double* fft_frame, int size);
void MostDominantFrequency(double *spectral_frame, int size, int* MDF);
void Compute_VAD(double* current_spectral_frame,  int16_t* current_time_frame);
int Run_VAD();
int test_FFT(int N,int loops);
float sinewave(float indice, float Frequency,unsigned long N);
int test_FFTW3(int N, int loops);
