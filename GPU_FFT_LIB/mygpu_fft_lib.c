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

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "mailbox.h"
#include "gpu_fft.h"

/**
* This function returns a file descriptor to a mailbox which is used to communicate between CPU and GPU.
*/
int Open_Mailbox() {
  return mbox_open();
}

/**
* This function closes a file descriptor connected to a mailbox.
* Param: The file descriptor to close.
*/
void Close_Mailbox(int mb) {
  mbox_close(mb);
}

/**
* This function computes a fft using the GPU.
* Param: input, the buffer to transform.
* Param: log2_N, the transform length between 8 and 22. (2^8 and 2^22 samples)
* Param: ouput, the buffer after FFT.
* Param: direction, 1 for FFT and 0 for iFFT.
* Param: sampling_interval, the interval between the different samples. (use 1 by default)
* Param: mb, the file descriptor to the mailbox.
*/
int Compute_fft(float * input , int log2_N, float * output, int direction ,double sampling_interval, int mb)
{
 int jobs=1, i;
 unsigned long N = 1 << log2_N;

 float FloatN = (float) N;
 float HalfN  = FloatN / 2.0;
 struct GPU_FFT_COMPLEX * base;
 struct GPU_FFT * fft;
 struct GPU_FFT_COMPLEX *DataIn, *DataOut;
 int ret = gpu_fft_prepare(mb, log2_N, direction == 1 ? GPU_FFT_FWD : GPU_FFT_REV , jobs, &fft);
 base = fft->in;

 for(i=0;i<N;i++)
  {
   base[i].re =  input[i]; base[i].im =0.0;
  }

 gpu_fft_execute(fft);
 base = fft->out;

 for(i=0;i<N;i++)
    output[i]= (base[i].re * base[i].re) + (base[i].im * base[i].im);

 gpu_fft_release(fft);
}

/**
* This function returns a value of a sine.
* Param: indice, the indice in the array.
* Param: Frequency, the frequency of the sine.
* Param: N, the array size.
*/
float Sinewave(float indice, float Frequency,unsigned long N)
{
  float Factor = Frequency  * 2.0 * 3.141592654/ N;
  return sin( Factor  * indice);
}
