CC = g++
CCFLAGS= -lpulse-simple -lpulse -Wall -pthread
EXEC_NAME= audio_test
LIBS= -L/usr/local/lib/

all: $(EXEC_NAME) clean

$(EXEC_NAME) : audio.o features.o vadfeatprocessor.o vadengine.o main.o 
	$(CC) -o $(EXEC_NAME) audio.o features.o vadfeatprocessor.o vadengine.o main.o $(CCFLAGS) $(LIBS)

main.o : main.cpp blockingqueue.h  audio.h
	$(CC) $(CCFLAGS) -c -o main.o main.cpp 

audio.o : audio.h audio.cpp
	$(CC) $(CCFLAGS) -c audio.cpp

vadfeatprocessor.o : vadfeatprocessor.h blockingqueue.h features.h features.cpp vadfeatprocessor.cpp
	$(CC) $(CCFLAGS) -c vadfeatprocessor.cpp 

vadengine.o : vadengine.h blockingqueue.h vadengine.cpp
	$(CC) $(CCFLAGS) -c vadengine.cpp

features.o : features.h features.cpp
	$(CC) $(CCFLAGS) -c features.cpp
clean :
	rm -rf *.o

cleanall :
	rm -rf *.o
	rm $(EXEC_NAME)
