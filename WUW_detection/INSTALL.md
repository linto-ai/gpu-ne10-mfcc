* Download FFTW3:
```
wget http://www.fftw.org/fftw-3.3.7.tar.gz
```
* Extract it:
```
tar -zxvf fftw-3.3.7.tar.gz
```
* Compile it:
```
cd fftw-3.3.7
sudo ./configure --prefix=$HOME/usr --enable-shared=yes
sudo make
sudo make install
```
* Results
You can find the file to include at:
```
/usr/include/fftw3.h
```
And the library at:
```
/usr/lib/arm-linux-gnueabihf/libfftw3.a
```

* Use in your project:
Add the following line to your header:
```
#include <fftw3.h>
```
Add the following options to your makefile:
```
-L/usr/lib/arm-linux-gnueabihf
-lfftw3
```
