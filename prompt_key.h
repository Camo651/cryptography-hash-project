// prompt_key.h (drop-in)
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

static int noecho_getline(const char *prompt, char **out) {
    struct termios oldt, newt;
    *out = NULL;

    if (isatty(STDIN_FILENO) && tcgetattr(STDIN_FILENO, &oldt) == 0) {
        newt = oldt; newt.c_lflag &= ~(ECHO);
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &newt);
        fputs(prompt, stderr); fflush(stderr);
        size_t cap = 0; ssize_t n = getline(out, &cap, stdin);
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldt);
        fputc('\n', stderr);
        if (n < 0) { free(*out); *out = NULL; return -1; }
    } else {
        fputs(prompt, stderr); fflush(stderr);
        size_t cap = 0; ssize_t n = getline(out, &cap, stdin);
        if (n < 0) { free(*out); *out = NULL; return -1; }
    }

    // strip CR/LF
    size_t n = strlen(*out);
    while (n && ((*out)[n-1] == '\n' || (*out)[n-1] == '\r')) (*out)[--n] = 0;
    return 0;
}

// encrypt: verify twice; decrypt: once
// returns 0 on success; *out is malloc'd (caller wipes+frees)
static int prompt_passphrase(int encrypt, const char *cipher_name, char **out) {
    char *p1 = NULL, *p2 = NULL;
    char msg1[160], msg2[200];

    snprintf(msg1, sizeof msg1, "enter %s hash password: ",
             cipher_name ? cipher_name : "hash");
    if (noecho_getline(msg1, &p1) != 0) return -1;

    if (encrypt) {
        snprintf(msg2, sizeof msg2, "Verifying - enter %s hash password: ",
                 cipher_name ? cipher_name : "hash");
        if (noecho_getline(msg2, &p2) != 0) { free(p1); return -1; }
        if (strcmp(p1, p2) != 0) { free(p1); free(p2); fputs("verify failure\n", stderr); return -1; }
        free(p2);
    }
    *out = p1;
    return 0;
}
