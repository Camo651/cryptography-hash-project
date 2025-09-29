#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

// This file is used to prompt user input key. This feature prompt user for a
// passphrase as stdin and not echo back (not store it in the terminal command
// history for security)

// returns 0 on success; *out is malloc'd
int prompt_passphrase(char **out) 
{
    
    *out = NULL;

    // Prefer the controlling terminal so pipelines still work
    FILE *tty = fopen("/dev/tty", "r+");
    FILE *in  = tty ? tty : stdin;
    FILE *outp= tty ? tty : stderr;
    int   fd  = fileno(in);

    struct termios oldt, newt;
    int have_tty = isatty(fd) && tcgetattr(fd, &oldt) == 0;

    if (have_tty) 
    {
        newt = oldt;
        newt.c_lflag &= ~(ECHO);
        tcsetattr(fd, TCSAFLUSH, &newt);
    }

    fputs("Enter SHA256_MDHK hash passphrase: ", outp);
    fflush(outp);

    char *line = NULL;
    size_t cap = 0;
    ssize_t n = getline(&line, &cap, in);

    if (have_tty) 
    {
        tcsetattr(fd, TCSAFLUSH, &oldt);
        fputc('\n', outp);
        fflush(outp);
    }

    if (tty) fclose(tty);
    if (n < 0) { free(line); return -1; }

    // strip CR/LF
    while (n > 0 && (line[n-1] == '\n' || line[n-1] == '\r')) line[--n] = 0;

    *out = line;
    return 0;
}
