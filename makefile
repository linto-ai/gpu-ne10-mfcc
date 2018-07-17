CC = g++
CCFLAGS= -lpulse-simple -lpulse -Wall
EXEC_NAME= audio_test
LIBS= -L/usr/local/lib/

all: $(EXEC_NAME) clean

$(EXEC_NAME) : audio.o blockingqueue.o main.o 
	$(CC) -o $(EXEC_NAME) audio.o blockingqueue.o main.o $(CCFLAGS) $(LIBS)

main.o : main.cpp blockingqueue.h audio.h
	$(CC) -c main.cpp $(CCFLAGS)

blockingqueue.o: blockingqueue.h blockingqueue.cpp
	$(CC) -c blockingqueue.cpp $(CCFLAGS)

audio.o : audio.h audio.cpp
	$(CC) -c audio.cpp $(CCFLAGS)

clean :
	rm -rf *.o

cleanall :
	rm -rf *.o
	rm $(EXEC_NAME)
