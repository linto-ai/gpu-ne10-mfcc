S = hex/shader_256.hex \
    hex/shader_512.hex \
    hex/shader_1k.hex \
    hex/shader_2k.hex \
    hex/shader_4k.hex \
    hex/shader_8k.hex \
    hex/shader_16k.hex \
    hex/shader_32k.hex \
    hex/shader_64k.hex \
    hex/shader_128k.hex \
    hex/shader_256k.hex \
    hex/shader_512k.hex \
    hex/shader_1024k.hex

C = mailbox.c gpu_fft.c gpu_fft_base.c gpu_fft_twiddles.c gpu_fft_shaders.c

CO = mailbox.o gpu_fft.o gpu_fft_base.o gpu_fft_twiddles.o gpu_fft_shaders.o mygpu_fft_lib.o


C1D = $(C) hello_fft.c
C2D = $(C) hello_fft_2d.c gpu_fft_trans.c

H1D = gpu_fft.h
H2D = gpu_fft.h gpu_fft_trans.h hello_fft_2d_bitmap.h

F = -lrt -lm -ldl

installmygpu_fft_lib:	mygpu_fft_lib.a
	sudo cp  mygpu_fft_lib.a  /usr/local/lib/mygpu_fft_lib.a
	sudo cp  mygpu_fft_lib.h /usr/local/include/mygpu_fft_lib.h


mygpu_fft_lib.o:	mygpu_fft_lib.c
	gcc -c -o mygpu_fft_lib.o $(F) mygpu_fft_lib.c

mailbox.o:	mailbox.c
	gcc -c -o mailbox.o $(F) mailbox.c

gpu_fft.o:	gpu_fft.c
	gcc -c -o gpu_fft.o $(F) gpu_fft.c

gpu_fft_base.o:	gpu_fft_base.c
	gcc -c -o gpu_fft_base.o $(F) gpu_fft_base.c

gpu_fft_twiddles.o:	gpu_fft_twiddles.c
	gcc -c -o gpu_fft_twiddles.o $(F) gpu_fft_twiddles.c

gpu_fft_shaders.o:	gpu_fft_shaders.c
	gcc -c -o gpu_fft_shaders.o $(F) gpu_fft_shaders.c

mygpu_fft_lib.a:  $(S) $(CO)
	ar rcs mygpu_fft_lib.a  $(CO)


hello_fft.bin:	$(S) $(C1D) $(H1D)
	gcc -o hello_fft.bin $(F) $(C1D)

hello_fft_2d.bin:	$(S) hex/shader_trans.hex $(C2D) $(H2D)
	gcc -o hello_fft_2d.bin $(F) $(C2D)

clean:
	rm -f *.bin
	rm -f *.o
	rm -f mygpu_fft_lib.a
