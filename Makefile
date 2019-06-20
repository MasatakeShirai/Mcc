CFLAGS=-std=c11 -Wall -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

Mcc: $(OBJS)
	$(CC) -o Mcc $(OBJS) $(LDFLAGS)

$(OBJS): Mcc.h

test: Mcc
	./Mcc -test
	./test.sh


clean:
	rm -f Mcc *.o* *~
