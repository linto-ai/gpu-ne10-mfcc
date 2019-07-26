# Paho MQTT in C
* Clone the repository
```
git clone https://github.com/eclipse/paho.mqtt.c
```
* Moove in the folder:
```
cd paho.mqtt.c
```
* Compile and install it:
```
make
sudo make install
```
* Results
You can find the files to include at:
```
/usr/include/
```
And the library at:
```
/usr/local/lib/
```

* Use in your project:
Add the following options to your makefile:
```
-L/usr/local/lib/
-lpaho-mqtt3cs
```
# JSMN in C
* Clone the repository
```
git clone https://github.com/zserge/jsmn
```
* Moove in the folder:
```
cd jsmn
```
* Compile and install it:
```
make
```
* Moove the files
```
sudo mv libjsmn.a /usr/local/lib
sudo mv jsmn.h /usr/include
```
