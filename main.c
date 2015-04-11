#include <stdio.h>

#include <stdio.h>
#include <execinfo.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#define s_exePath "/home/alex/workarea/c-stacktrace-build/c-stacktrace"
//static char s_exePath[1024];

void printStackTrace(char outBuf[], const unsigned int outBufSize, const char exePath[], struct sigcontext ctx);

static void sig_hup(int sig,  struct sigcontext ctx)
{
    printf("sig_hup: got signal %d\n", sig);
    switch(sig)
    {
    case SIGSEGV:
    {
        static const unsigned int size = 100;
        char outBuf[size];
        printStackTrace(outBuf, size, s_exePath, ctx);
        fflush(stderr);
        fflush(stdout);
        break;
    }
    }

    _exit(1); //prevent loop
}

void printStackTrace(char outBuf[], const unsigned int outBufSize, const char exePath[], struct sigcontext ctx)
{
    static const int traceSize = 16;
    void *trace[traceSize];
    char **messages = (char **)NULL;
    int i, trace_size = 0;

    trace_size = backtrace(trace, traceSize);
    /* overwrite sigaction with caller's address */
#ifndef __x86_64__
    trace[1] = (void *)ctx.eip; //32 bit instruction pointer register
#else
    trace[1] = (void *)ctx.rip; //64 bit instruction pointer register
#endif
    messages = backtrace_symbols(trace, trace_size);
    /* skip first stack frame (points here) */
    printf("[bt] Execution path:\n");
    for (i=1; i<trace_size; ++i)
    {
        static const int buflen = 1024;
        char buf[buflen];
        memset(buf, 0, buflen*sizeof(char));
        snprintf(buf, buflen, "[bt] #%d %s", i, messages[i] );
        printf("%s : ", buf);
        if(NULL == trace[i]){
            printf("\n");
            continue; //no stack info here
        }

        char syscom[buflen];
        snprintf(syscom, buflen,"addr2line %p -e %s", trace[i], exePath); //last parameter is the name of this app
        FILE *f = popen(syscom, "r");
        if (f != NULL)
        {
            char buffer[buflen];
            memset(buffer, 0, buflen*sizeof(char));
            while(fgets(buffer, sizeof(buffer), f) != NULL)
            {
                printf("%s", buffer);
            }
            pclose(f);
        }
    }
}

void crashMe()
{
    char* p = 0;
    *p = 'a';
}

int main(int argc, char* argv[])
{
    //register for sigsegv
    struct sigaction sa;
    sa.sa_handler = (__sighandler_t)sig_hup;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGSEGV, &sa, NULL);

    //strcpy(s_exePath, argv[0]);

    crashMe();

    return 0;
}

