CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS =

SOURCES = main.c sha.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = cryptohash

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
.PHONY: all clean
