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
* Test the FFT on data extracted from file
* Param: Format in bits (8 or 16)
* Param: Rate in Hz
* Param: Number of channels (1 to 4)
* Param: Path to the file
*/
int FFT_From_File(char* format,char* rate,char* channels,char* file) {
  pa_simple *s_playback = NULL;
  int16_t buff[BUFSIZE];
  double OutT[BUFSIZE];
  int i = 0;
  fftw_complex C[BUFSIZE];
  fftw_plan plan = NULL;
  Audio_Init(&s_playback,"playback",format,rate,channels);
  FILE *fp = fopen(file, "r");
  printf("open ok !  \n");
  fseek(fp,44,SEEK_CUR);
  printf("seek ok !\n");
  fread(buff,sizeof(buff[0]),BUFSIZE,fp);
  printf("read ok !\n");
  while (buff[i] != EOF) {
    for(i=0;i<BUFSIZE;i++) {
      printf("In :%" PRId16 "\n",buff[i]);
    }
    plan = fftw_plan_dft_r2c_1d(BUFSIZE,buff,C,0);
    fftw_execute(plan);
    for(i=0;i<BUFSIZE;i++) {
      OutT[i]= C[i][0] *C[i][0] + C[i][1] * C[i][1];
      //printf("Out: %0.f\n",OutT[i]);
    }
    fread(buff,sizeof(buff[0]),BUFSIZE,fp);
    sleep(5);
  }
  pa_simple_free(s_playback);
  fclose(fp);
  printf("File closed\n");
}

/**
* Test the Pulse Audio sound server by playing the microphone input
* Param: Format in bits (8 or 16)
* Param: Rate in Hz
* Param: Number of channels (1 to 4)
*/
int replay(char* format,char* rate,char* channels) {
  int16_t* buff=malloc(sizeof(int16_t)*BUFSIZE);
  double* buff_double=malloc(sizeof(double)*BUFSIZE);
  int error = 0;
  pa_simple *s_playback = NULL;
  pa_simple *s_record = NULL;
  uint64_t energy = 0;
  int i = 0;
  double Out[BUFSIZE];
  fftw_complex C[BUFSIZE];
  Audio_Init(&s_playback,"playback",format,rate,channels);
  Audio_Init(&s_record,"record",format,rate,channels);
  fftw_plan plan = NULL;
  for (;;) {
      if ((error = Audio_Input(s_record,BUFSIZE*sizeof(int16_t),buff)) < 0) {
          printf("Input Error : %d !\n",error);
      }
      for(i=0;i<BUFSIZE;i++) {
        buff_double[i] = (double)buff[i];
      }
      plan = fftw_plan_dft_r2c_1d(BUFSIZE,buff_double,C,0);
      fftw_execute(plan);
      for(i=0;i<BUFSIZE;i++) {
        Out[i]= C[i][0] * C[i][0] + C[i][1] * C[i][1];
        //printf("%f \n",Out[i]);
      }
      Compute_VAD(Out,buff);
      Audio_Output(s_playback,BUFSIZE*sizeof(int16_t),buff);
  }
}


int main(int argc, char** argv) {
  //printf("Usage: File Format Rate Channels \n");
  //FFT_From_File(argv[2],argv[3],argv[4],argv[1]);
  printf("Usage: Format Rate Channels \n");
  replay(argv[1],argv[2],argv[3]);
  //Run_VAD();
}
