# LinTO-MFCC-Module

### Ne10
The Ne10 library is a mathematical library (Convolution, transforms, matrices, vectors etc ...) based on NEON and optimized for ARM architecture.
Doc Ne10 : <http://projectne10.github.io/Ne10/doc/modules.html>
Building Ne10 : <https://github.com/projectNe10/Ne10/blob/master/doc/building.md#building-ne10>

```
git clone https://github.com/projectNe10/Ne10
cd Ne10
export NE10_LINUX_TARGET_ARCH=armv7
cmake -DGNULINUX_PLATFORM=ON
make
```

### Parser JSON
I use RapidJSON, because it's rapid :D . To be honest, I used the first library I found. It seems to be enough at this time.

### Communication
The communication between threads is possible through BlockingQueue. You just have to set the input queue of one thread with the output queue of a second one.
Audio thread can provide multiple queues. In this way, Record Manager and MFCC can get input samples.

### MFCC computation
MFCC computation using Kaldi pipeline for one frame.
* Compute average value of the frame
* Cast inputs to float
* Remove the average value for each sample
* Compute energy of the frame
* Apply pre-emphasis and windowing (Povey, Hamming, Hann or Blackman)
* Apply Fast Fourier Transform (using NE10)
* Compute abs(X)^2 for each sample
* Compute the last vector with MelFilterBank matrix (we should change matrix by NE10 matrix)
* Apply Discret cosine transform
* Replace first value by energy

To get MFCC for all frames, we compute the first frame from 0 to size samples. Then, we compute the frame from size-sliding_samples to 2*size-slinding_samples.
Currently, all class attributes are public, so you can easily modify MFCC parameters in main.cpp. (It should change with the time)

### Pulse Audio
This program uses pulse audio API to get audio in real-time. 