CC=gcc
CFLAGS=
WFLAGS=-Wall -Werror
ifeq ($(shell uname -s),Darwin)
	WFLAGS=-Wall
endif

oem-objs = oem-sign.o

all: oem-sign

%.o: %.c
	$(CC) $(CFLAGS) $(WFLAGS) -c -o $@ $<

oem-sign: $(oem-objs)
	$(CC) -o $@ $? $(LDFLAGS)

clean:
	-rm -f *.o oem-sign
