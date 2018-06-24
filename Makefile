CC=gcc
CCFLAGS=-Wall -Wextra -Werror -O4 -std=c99
LFLAGS=-lncurses -lm

SOURCES=main.c gamestate.c corpus.c
OBJECTS=$(SOURCES:.c=.o)

BINARY=termracer

.PHONY: all clean

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) $(LFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CCFLAGS) -c $<

clean:
	rm $(BINARY) $(OBJECTS)
