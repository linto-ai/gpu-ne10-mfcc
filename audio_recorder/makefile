CCFLAGS= -lpulse-simple -lpulse -lpthread -lpaho-mqtt3cs -ljsmn
LIB=-L/usr/lib/arm-linux-gnueabihf -L/usr/local/lib/



all: audio_recorder clean

audio_recorder : Circular.o Audio.o main.o mqtt.o
	gcc -o audio_recorder main.o Circular.o Audio.o mqtt.o $(CCFLAGS) $(LIB)
main.o : src/main.c
	gcc -c src/main.c $(CCFLAGS)
mqtt.o : src/mqtt.c include/mqtt.h
	gcc -c src/mqtt.c $(CCFLAGS)
Audio.o : src/Audio.c include/Audio.h
	gcc -c src/Audio.c $(CCFLAGS)
Circular.o : src/Circular.c include/Circular.h
	gcc -c src/Circular.c $(CCFLAGS)
clean :
	rm -rf *.o

cleanall :
	rm -rf *.o
	rm -rf audio_recorder
