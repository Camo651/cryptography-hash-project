CC=gcc
CFLAGS=-O2 -Wall -Wextra -I.
SRC=main.c clean_sha.c
OBJ=$(SRC:.c=.o)

all: sha256
sha256: $(OBJ)
	$(CC) $(OBJ) -o $@

clean:
	rm -f $(OBJ) sha256
