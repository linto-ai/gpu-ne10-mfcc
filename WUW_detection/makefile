CCFLAGS=-Wall -lpulse-simple -lpulse -lm -ldl -lfftw3 -lrt -lc -O3 -lpaho-mqtt3cs
LIB=-L/usr/lib/arm-linux-gnueabihf -L/usr/local/lib/


all: WUW_detection

WUW_detection : main.o Audio.o VAD.o mqtt.o
	gcc -o WUW_detection main.o Audio.o VAD.o mqtt.o $(CCFLAGS) $(LIB)
main.o : main.c
	gcc -c main.c $(CCFLAGS)
VAD.o : VAD.c VAD.h
	gcc -c VAD.c $(CCFLAGS)
Audio.o : Audio.c Audio.h
	gcc -c Audio.c $(CCFLAGS)
mqtt.o : mqtt.c mqtt.h
	gcc -c mqtt.c $(CCFLAGS)
clean :
	rm -rf *.o
	rm -rf WUW_detection
