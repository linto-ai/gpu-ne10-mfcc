CC = g++
CCFLAGS= -Wall -pthread -Iinclude
EXEC_NAME= audio_test
LIBS= -L/usr/local/lib/

all: $(EXEC_NAME) clean

$(EXEC_NAME) : main.o circular.o recordmanager.o client.o 
	$(CC) -o $(EXEC_NAME) recordmanager.o client.o circular.o main.o $(CCFLAGS) $(LIBS)

main.o : src/main.cpp
	$(CC) $(CCFLAGS) -c -o main.o src/main.cpp 

recordmanager.o : include/recordmanager.h src/recordmanager.cpp
	$(CC) $(CCFLAGS) -c src/recordmanager.cpp 

client.o : include/client.h src/client.cpp
	$(CC) $(CCFLAGS) -c src/client.cpp

circular.o : include/circular.h src/circular.cpp
	$(CC) $(CCFLAGS) -c src/circular.cpp

mqtt_client.o : include/mqtt_client.h src/mqtt_client.cpp
	$(CC) $(CCFLAGS) -c src/mqtt_client.cpp

clean :
	rm -rf *.o

cleanall :
	rm -rf *.o
	rm $(EXEC_NAME)
