# LinTO-Voice-Module

### Compute Library
Regarding the Compute Library (based on ARM Neon), installation is quite simple.
```
git clone https://github.com/Arm-software/ComputeLibrary.git 
cd ComputeLibrary 
scons Werror=1 debug=0 asserts=0 neon=1 opencl=0 examples=1 build=native -j4
```
I have not had the opportunity to use it for the moment but it will certainly be useful for the future, especially during the phase of decoding with GRU on keyword detection.
Compute Library: <https://arm-software.github.io/ComputeLibrary/latest/>

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

### MQTT Client
Currently, I use the PAHO MQTT C library, already used for the Python / C demonstrator. It is possible to use PAHO MQTT C++.
It still misses the management of the messages so that the good thread recovers the good message. No idea if it's possible without the C++ version.
Doc Paho MQTT C : <https://github.com/eclipse/paho.mqtt.c>

### Parser JSON
I use RapidJSON, because it's rapid :D . To be honest, I used the first library I found. It seems to be enough at this time.

### VAD
Rudy has already finished implementing the part allowing the VAD detection.

### MFCC computation
The calculation of the MFCC is possible for a window at the moment. The extraction takes about 2% of the relative time (0.3 ms for 25 ms of audio).