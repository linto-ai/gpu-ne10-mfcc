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
#include "VAD.h"

/**
* Returns the minimum value of a specific array
* Param: Pointer to an array
*/
uint64_t Min(uint64_t *array) {
  uint64_t minimum_value = 10000000000;
  int i;
  for (i=0;i<NELEM(array);i++) {
    if (array[i] < minimum_value) {
      minimum_value = array[i];
    }
  }
  return minimum_value;
}


/**
* Returns the short terme energy of a frame
* Param: Pointer to the current frame
* Param: Number of samples in the frame
*/
uint64_t Energy(int16_t* frame) {
  int i;
  uint64_t results = 0;
  for (i=0;i<sizeof(frame)/sizeof(frame[0]);i++) {
    results += frame[i]*frame[i];
    //printf("%llu ",results);
  }
  return results/(sizeof(frame)/sizeof(frame[0]));
}

/**
* Returns the geometric mean
* Param: Pointer to an array
* Param: Number of samples in the array
*/
float GeometricMean(double *arr, int size)
{
    // declare sum variable
    float sum = 0;
    // Compute the sum of all the elements in the array.
    for (int i = 0; i < size; i++)
        sum = sum + log(arr[i]);
    // compute geometric mean through formula
    // antilog(((log(1) + log(2) + . . . + log(n))/n)
    // and return the value to main function.
    sum = sum / size;
    return exp(sum);
}

/**
* Frequency Band Amplitude Ratio returns the ratio of a given frequency band whitin the frequencies domain.
* Param: The spectrum of a frame
* Param: The lower bound of the frequency band
* Param: The upper bound of the frequency band
*/
void ComputeFBAR(double* spectrum,int freq_low, int freq_high,double* R) {
  int start_id = freq_low*BUFSIZE/RATE;
  int end_id = freq_high*BUFSIZE/RATE;
  int i = 0;
  double total = 0;
  double results = 0;
  for (i ; i < BUFSIZE/2 ; i++) {
    total += spectrum[i];
    if (i>= start_id && i<= end_id) {
      results += spectrum[i];
    }
  }
  *R = results/total;
}

/**
* Returns the Spectral Flatness Measure
* Param: Pointer to a frame
* Param: Number of samples in the frame
*/
uint64_t ComputeSFM(double *spectral_frame, int size ) {
  int i;
  uint64_t ar_mean = 0;
  // Compute the mean of the spectral_frame
  for (i=0;i<size;i++) { ar_mean += spectral_frame[i];}
  ar_mean /= size;
  return 10*log(ar_mean/GeometricMean(spectral_frame,size));
}

/**
* Returns the most dominant frequency of the spectrum
* Param: Pointer to a spectral frame
* Param: Number of samples in the frame
*/
void MostDominantFrequency(double *spectral_frame, int size,int* MDF ) {
  int most_dominant_sample = 0 ;
  double max_sp = 0.0;
  int i;
  for (i=0;i<size/2;i++) {
    if (abs(spectral_frame[i]) > max_sp) {
      max_sp = spectral_frame[i];
      most_dominant_sample = i;
    }
  }
  *MDF = most_dominant_sample*(RATE/2)/size;
}

/**
* Determine if there is vocal activity or not using energy, FBAR and MDF
* Param: Pointer to a spectral frame
* Param: Pointer to the audio buffer
*/
void Compute_VAD(double* current_spectral_frame, int16_t* current_time_frame) {
  // Currently, we use 3 features which are FBAR, Energy and the most dominant frequency
  // If 2 of 3 features reach a specific threshold, it's a speech frame
  double FBAR = 0;
  int MDF = 0;
  int i ;
  ComputeFBAR(current_spectral_frame,100,355,&FBAR);
  uint64_t energy = Energy(current_time_frame);
  MostDominantFrequency(current_spectral_frame,BUFSIZE,&MDF);
  float FBAR_Thresh = 0.3;
  int Energy_Thresh = 10000;
  int MDF_Low_Thresh = 80;
  int MDF_High_Thresh = 400;
  int res = 0;
  res += (FBAR > FBAR_Thresh) ? 1 : 0;
  res += (energy > Energy_Thresh) ? 1 : 0;
  res += (MDF > MDF_Low_Thresh && MDF < MDF_High_Thresh) ? 1 : 0;
  //printf("FBAR:%f Energy:%llu MDF:%d\n",FBAR,energy,MDF);
  if (res >= 2) {
    printf("Voice activity !\n");
  }
  else {
    //printf("No Voice activity!\n");
  }
}

/**
* Returns the value of a sinewave using the indice and the frequency
* Param: Indice of the current value
* Param: Frequency of the sinewave
* Param: Number of elements
*/
float sinewave(float indice, float Frequency,unsigned long N)
{
  float Factor = Frequency  * 2.0 * 3.141592654;
  return sin( Factor  * indice/N);
}

/**
* Determine if there is vocal activity using the default Pulse Audio input
*/
/*int Run_VAD()
{
  int16_t *buff;
  buff = malloc(sizeof(int16_t)*BUFSIZE);
  pa_simple *s_record = NULL;
  fftw_plan plan = NULL;
  fftw_complex C[BUFSIZE];
  int i = 0;
  float OutT[BUFSIZE];
  if (Audio_Init(&s_record,"record","16","16000","1") < 0) {
    printf("Problème d'initialisation du lien Pulse Audio\n");
    return -1;
  }
  for(;;) {
    Audio_Input(s_record,(size_t)BUFSIZE,buff);
    plan = fftw_plan_dft_r2c_1d(BUFSIZE,(float*)buff,C,0);
    fftw_execute(plan);
    for(i=0;i<BUFSIZE;i++) {
      OutT[i]= C[i][0] *C[i][0] + C[i][1] * C[i][1];
    }
    Compute_VAD(OutT,buff);
  }
  pa_simple_free(s_record);
  return 0;
}*/

/**
* Run a test of FFT using GPU-FFT
* Param: Number of samples in a frame
* Param: Number of time the process will be performed
*/
/*
int test_FFT(int N,int loops) {
  int i=0;
  int loop = 0;
  float InT[N];
  float OutT[N];
  struct timeval stop, start;
  int mb = Open_Mailbox();
  suseconds_t total = 0;
  // create dummy  random * F sine wave
  // first select amplitude
  for (loop;loop<loops;loop++) {
    float Amplitude = (float) (rand() % 100);
    float Frequency = (float) (rand() % (N/2));

    //printf("======================\n");
    //printf("Create sine wave  Frequency = %0.f  Amplitude = %0.f\n",Frequency,Amplitude);

    // generate the sine wave
    for(i=0;i<N;i++) {
             InT[i]=  Amplitude * sinewave(i,Frequency,N);
    }
    //printf("Compute FFT  ... ");
    gettimeofday(&start, NULL);
    Compute_fft(InT,log(N)/log(2),OutT,1,1,mb);
    gettimeofday(&stop, NULL);
    suseconds_t t = stop.tv_usec - start.tv_usec;
    total+= t;
    //printf("Done in %lu microseconds\n", t);
    int Peaki=0;
    float PeakValue=0;
    for(i=1;i<=(N/2);i++)
    {
     if(OutT[i] > PeakValue)
        {
            PeakValue = OutT[i];
            Peaki = i;
        }
      }
      //printf("FFT Found Peak at Frequency= %d Hz and Amplitude = %.0f\n\n",Peaki, sqrt(PeakValue)/ (N/2.0));
  }
  printf("Average : %lu microseconds with %d samples, %d loops and GPU_FFT\n",total/loops,N,loops);
  Close_Mailbox(mb);
  return 0;
}
*/
/**
* Run a test of FFT using CMSIS library
* Param: Number of the samples in a frame
* Param: Number of time the process will be performed
*/
/*int test_CMSIS(int N,int loops) {
  int i=0;
  int loop = 0;
  arm_status status;
  float InT[N];
  float OutT[N];
  struct timeval stop, start;
  arm_rfft_fast_instance_f32 *S;
  //int mb = Open_Mailbox();
  status = arm_rfft_fast_init_f32(S,N);
  if (status != ARM_MATH_SUCCESS) {
    printf("FFT init failed...\n");
    return -1;
  }
  suseconds_t total = 0;
  // create dummy  random * F sine wave
  // first select amplitude
  for (loop;loop<loops;loop++) {
    float Amplitude = (float) (rand() % 100);
    float Frequency = (float) (rand() % (N/2));

    printf("======================\n");
    printf("Create sine wave  Frequency= %0.f  Amplitude= %0.f\n",Frequency,Amplitude);

    // generate the sine wave
    for(i=0;i<N;i++) {
             InT[i]=  Amplitude * sinewave(i,Frequency,N);
    }
    printf("Compute FFT  ... ");
    gettimeofday(&start, NULL);
    arm_rfft_fast_init_f32(S,InT,OutT,0);
    gettimeofday(&stop, NULL);
    suseconds_t t = stop.tv_usec - start.tv_usec;
    total+=t;
    printf("Done in %lu microseconds\n", t);
    int Peaki=0;
    float PeakValue=0;
    for(i=1;i<=(N/2);i++)
    {
     if(OutT[i] > PeakValue)
        {
            PeakValue= OutT[i];
            Peaki = i;
        }
      }
      printf("FFT Found Peak at Frequency= %d Hz and Amplitude = %.0f\n\n",Peaki, sqrt(PeakValue)/ (N/2.0));
  }
  printf("Average : %lu microseconds with %d samples and %d loops\n",total/loops,N,loops);
  Close_Mailbox(mb);
  return 0;
}*/

/**
* Run a test of FFT using FFTW3 library
* Param: Number of the samples in a frame
* Param: Number of time the process will be performed
*/
int test_FFTW3(int N, int loops) {
  int i=0;
  int loop = 0;
  double InT[N];
  double OutT[N];
  fftw_complex C[N];
  struct timeval stop, start;
  fftw_plan plan = NULL;
  suseconds_t total = 0;
  // create dummy  random * F sine wave
  // first select amplitude

  for (loop;loop<loops;loop++) {
    float Amplitude = (float) (rand() % 100);
    float Frequency = (float) (rand() % (N/2));

    //printf("======================\n");
    //printf("Create sine wave  Frequency= %0.f  Amplitude= %0.f\n",Frequency,Amplitude);

    // generate the sine wave
    for(i=0;i<N;i++) {
             InT[i]=  Amplitude * sinewave(i,Frequency,N);
    }
    //printf("Compute FFT  ... ");
    gettimeofday(&start, NULL);
    plan = fftw_plan_dft_r2c_1d(N,InT,C,0);
    fftw_execute(plan);
    gettimeofday(&stop, NULL);
    suseconds_t t = stop.tv_usec - start.tv_usec;
    total+=t;
    //printf("Done in %lu microseconds\n", t);
    for(i=0;i<N;i++) {
      OutT[i]= C[i][0] *C[i][0] + C[i][1] * C[i][1];
      //printf("%0.f\n",OutT[i]);
    }
    int Peaki=0;
    float PeakValue=0;
    for(i=1;i<=(N/2);i++)
    {
     if(OutT[i] > PeakValue)
        {
            PeakValue = OutT[i];
            Peaki = i;
        }
      }
      //printf("FFT Found Peak at Frequency= %d Hz and Amplitude = %.0f\n\n",Peaki, sqrt(PeakValue)/ (N/2.0));
  }
  printf("Average : %lu microseconds with %d samples, %d loops and FFTW3\n",total/loops,N,loops);
  fftw_destroy_plan(plan);
  return 0;
}
