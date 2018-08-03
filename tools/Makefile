SRCS := $(wildcard *.c)

BINS := $(SRCS:%.c=%)

CFLAGS += -Wall -I../driver

all: $(BINS)

clean:
	$(RM) $(BINS)

.PHONY: all clean
