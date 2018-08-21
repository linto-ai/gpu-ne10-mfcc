CC = g++
CCFLAGS= -Wall -pthread
EXEC_NAME= audio_test
LIBS= -L/usr/local/lib/

all: $(EXEC_NAME) clean

$(EXEC_NAME) : main.o circular.o recordmanager.o client.o 
	$(CC) -o $(EXEC_NAME) recordmanager.o client.o circular.o main.o $(CCFLAGS) $(LIBS)

main.o : main.cpp
	$(CC) $(CCFLAGS) -c -o main.o main.cpp 

recordmanager.o : recordmanager.h recordmanager.cpp
	$(CC) $(CCFLAGS) -c recordmanager.cpp 

client.o : client.h client.cpp
	$(CC) $(CCFLAGS) -c client.cpp

circular.o : circular.h circular.cpp
	$(CC) $(CCFLAGS) -c circular.cpp

clean :
	rm -rf *.o

cleanall :
	rm -rf *.o
	rm $(EXEC_NAME)
