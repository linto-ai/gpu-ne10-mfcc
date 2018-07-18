CC = g++
CCFLAGS= -lpulse-simple -lpulse -Wall -pthread
EXEC_NAME= audio_test
LIBS= -L/usr/local/lib/

all: $(EXEC_NAME) clean

$(EXEC_NAME) : audio.o main.o 
	$(CC) -o $(EXEC_NAME) audio.o  main.o $(CCFLAGS) $(LIBS)

main.o : main.cpp blockingqueue.h  audio.h
	$(CC) -c -o main.o main.cpp $(CCFLAGS)

audio.o : audio.h audio.cpp
	$(CC) -c audio.cpp $(CCFLAGS)

clean :
	rm -rf *.o

cleanall :
	rm -rf *.o
	rm $(EXEC_NAME)
