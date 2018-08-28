CC = g++
CCFLAGS= -Wall -pthread -Iinclude -lpaho-mqtt3cs
EXEC_NAME= audio_test
LIBS= -L/usr/local/lib/ 
OBJ = main.o circular.o recordmanager.o client.o  mqtt_client.o features.o

all: $(EXEC_NAME) clean

$(EXEC_NAME) : $(OBJ)
	$(CC) -o $(EXEC_NAME) $(OBJ) $(CCFLAGS) $(LIBS)

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

features.o : include/features/h src/features.cpp
	$(CC) $(CCFLAGS) -c src/features.cpp

clean :
	rm -rf *.o

cleanall :
	rm -rf *.o
	rm $(EXEC_NAME)
