# Avancée du LinTO-Voice-Module

### Compute Library
Concernant la Compute Library (basée sur ARM Neon), l'installation est assez simple.
```
git clone https://github.com/Arm-software/ComputeLibrary.git 
cd ComputeLibrary 
scons Werror=1 debug=0 asserts=0 neon=1 opencl=0 examples=1 build=native -j4
```
Je n'ai pas eu l'occasion de l'utiliser pour le moment mais elle sera certainement utile pour la suite, notamment lors de la phase de décodage via GRU de la détection du mot-clé.
Doc Compute Library: <https://arm-software.github.io/ComputeLibrary/latest/>

### Ne10
La bibliothèque Ne10 est une bibliothèque de mathématiques (Convolution, transformées, matrices, vecteurs etc...) basée sur NEON et optimisée pour architecture ARM. 
Doc Ne10 : <http://projectne10.github.io/Ne10/doc/modules.html>
Compilation Ne10 : <https://github.com/projectNe10/Ne10/blob/master/doc/building.md#building-ne10>

```
git clone https://github.com/projectNe10/Ne10
cd Ne10
export NE10_LINUX_TARGET_ARCH=armv7
cmake -DGNULINUX_PLATFORM=ON
make
```

### Client MQTT
Actuellement, j'utilise la librairie PAHO MQTT C, déjà utilisée pour le démonstrateur en Python/C. Il est possible d'utiliser PAHO MQTT C++.
Il manque encore la gestion des messages pour que le bon thread récupère le bon message. À voir si c'est possible sans la version C++ de la bibliothèque.
Doc Paho MQTT C : <https://github.com/eclipse/paho.mqtt.c>

### Parser JSON
J'utilise RapidJSON, parce que c'est rapide (badum tsss). J'ai utilisé la première librairie que j'ai trouvée. Elle a l'air d'être suffisante, à voir si on a besoin de quelque chose de plus poussée plus tard.

### VAD
Rudy a terminé d'implémenter la partie permettant la détection de la VAD.

### Calcul des MFCC
Je suis en train de mettre en place les différentes classes, fonctions, ou algorithmes pour réaliser cette tâche. 

### Problème en cours
* Erreur de compilation dans les fichiers standards c? -> l'options -Iinclude de g++ ne fonctionne pas, on doit utiliser #include "../include/something.h" -> chiant :/=
* LDCONFIG ou déplacer les bibliothèques statiques directement dans /usr/local/lib (/usr/lib) ?
