CC      = gcc
CFLAGS  = -O2 -Wall -Wextra -I. -MMD -MP -D_POSIX_C_SOURCE=200809L

PROGS := sha256 sha256_MDHK

.PHONY: all clean
all: $(PROGS)

sha256: sha256.o clean_sha.o
	$(CC) $(CFLAGS) -o $@ $^

sha256_MDHK: sha256_MDHK.o modified_sha.o
	$(CC) $(CFLAGS) -o $@ $^

# compile
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# headers (auto via -MMD, but explicit helps first build)
clean_sha.o: clean_sha.h
modified_sha.o: modified_sha.h
sha256.o: clean_sha.h
sha256_MDHK.o: modified_sha.h prompt_key.h

clean:
	rm -f $(PROGS) *.o *.d

clean-sha256:
	rm -f sha256 sha256.o clean_sha.o sha256.d clean_sha.d

clean-sha256_MDHK:
	rm -f sha256_MDHK sha256_MDHK.o modified_sha.o sha256_MDHK.d modified_sha.d

-include *.d

                                  


