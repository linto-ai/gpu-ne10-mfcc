* Install GPU_FFT:
```
sudo rpi-update && sudo reboot
```
* Moove to the folder:
```
cd /opt/vc/src/hello_pi/hello_fft
sudo mknod char_dev c 100 0
```
* Add the 3 files to the previous folder:
* Run the command:
```
make installmygpu_fft_lib
```
