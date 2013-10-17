
all: acc_device acc_data acc_kernel

acc_device: acc_device.c acc_compiler_data.c
	gcc -I../include ../lib/libopenacc.o acc_device.c acc_compiler_data.c -o acc_device

acc_data: acc_data.c acc_compiler_data.c
	gcc -I../include ../lib/libopenacc.o acc_data.c acc_compiler_data.c -o acc_data

acc_kernel: acc_kernel.c acc_compiler_data.c
	gcc -I../include ../lib/libopenacc.o acc_kernel.c acc_compiler_data.c -o acc_kernel

check:
	./acc_device

clean:
	rm -f acc_device acc_data acc_kernel
