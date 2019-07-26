# PulseAudio-MQTT-Wakeword-ASD

The aim of this project is to produce a functional module able to detect voice activity and real-time Wakewords on a Raspberry type embedded system.
It is able to retrieve data extracted from the microphone and perform the temporal and spectral analysis.
It communicates with the system via an MQTT local bus.

### Before starting

The following libraries (and APIs) are necessary for the proper functioning of the program:

* (FFT-GPU : <https://www.raspberrypi.org/blog/accelerating-fourier-transforms-using-the-gpu/>)
* Pulse Audio : <https://freedesktop.org/software/pulseaudio/doxygen/index.html>
* PAHO MQTT : <https://github.com/eclipse/paho.mqtt.c>
* JSMN in C : <https://github.com/zserge/jsmn>

### Compilation

Change the paths to the libraries in the makefile.
Use only the "make" command on the target in the corresponding folder.

### Use

Currently, this program uses Pulse Audio default device.
The WUW program continuously reads the sound from the default input device, calculates the energy on frames of 2048 points, then reads the contents of the buffer and sends it to the default output device.
The Audio Recorder program reads the sound from the default input device, then stores it in a circular buffer. When, a specific message is received on the MQTT bus, the full data are send to a named pipe or a distant server.
