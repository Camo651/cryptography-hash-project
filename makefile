CC=gcc
CFLAGS=-O2 -Wall -Wextra -I.
SRC=main.c clean_sha.c
OBJ=$(SRC:.c=.o)

all: hash
hash: $(OBJ)
	$(CC) $(OBJ) -o $@

clean:
	rm -f $(OBJ) hash
