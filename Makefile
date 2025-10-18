CC = arm-linux-gnueabi-gcc
CCFLAGS = --static -march=armv7-a -mtune=cortex-a9
LDFLAGS = -lm

all: bin set sin rainbow pride baja_blast

bin:
	mkdir -p bin

set:
	$(CC) $(CCFLAGS) src/set.c -o bin/mx65rgb_set $(LDFLAGS)

sin:
	$(CC) $(CCFLAGS) src/sin.c -o bin/mx65rgb_sin $(LDFLAGS)

rainbow:
	$(CC) $(CCFLAGS) src/rainbow.c -o bin/mx65rgb_rainbow $(LDFLAGS)

pride:
	$(CC) $(CCFLAGS) src/pride.c -o bin/mx65rgb_pride $(LDFLAGS)

baja_blast:
	$(CC) $(CCFLAGS) src/baja_blast.c -o bin/mx65rgb_baja_blast $(LDFLAGS)

debug: CCFLAGS += -DDEBUG
debug: bin set sin rainbow

wow: CC = gcc
wow: CCFLAGS = -DMOCK -DDEBUG
wow: bin set sin rainbow

clean:
	rm -f bin/*
