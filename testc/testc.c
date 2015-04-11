#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define EXE_PATH "/home/alex/workarea/c-stacktrace-build/testc/testc"

#include "../stacktrace.h"

static void sig_hup(int sig,  struct sigcontext ctx)
{
    printf("sig_hup: got signal %d\n", sig);
    switch(sig)
    {
    case SIGSEGV:
    {
        static const unsigned int size = 2048;
        char outBuf[size];
        printStackTrace(outBuf, size, EXE_PATH, &ctx);
        printf("%s", outBuf);
        fflush(stderr);
        fflush(stdout);
        break;
    }
    }

    _exit(1); //prevent looping
}

/* dummy crash function */
static void crashMe()
{
    char* p = 0;
    *p = 'a';
}

int main(int argc, char* argv[])
{
    initSigHandler(&sig_hup);

    crashMe();

    return 0;
}
