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

### MFCC computation
The calculation of the MFCC is possible for a window at the moment. The extraction takes about 2% of the relative time (0.3 ms for 25 ms of audio).

### Pulse Audio
This program uses pulse audio API to get audio in real-time. 