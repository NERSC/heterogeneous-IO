TARGETS=test_get_procid
OBJS=test_get_procid.o get_procid.o

CC=cc
CFLAGS=-g -Wall

.PHONY: all clean

all: $(TARGETS)

$(TARGETS): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
clean:
	rm -f $(TARGETS) $(OBJS)
