
all: libopenacc.o

libopenacc.o: libopenacc.c \
              ../include/OpenACC/openacc.h \
              ../include/OpenACC/libopenacc.h \
              ../include/OpenACC/libopenacc-internal.h \
              ../include/OpenACC/libopenacc-device.h \
              ../include/OpenACC/libopenacc-host-to-device.h
	gcc -I../include/ -g -c libopenacc.c -o libopenacc.o

check:

clean:
	rm -rf *.o

