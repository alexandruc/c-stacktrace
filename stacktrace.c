#include <stdio.h>
#include <execinfo.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define EXE_PATH

#include "stacktrace.h"

void printStackTrace(char outBuf[], const unsigned int outBufSize, const char exePath[], const struct sigcontext *ctx)
{
    static const int traceSize = 16;
    void *trace[traceSize];
    char **messages = (char **)NULL;
    int i, trace_size = 0;
    int len = 0;

    if(NULL == ctx){
        return;
    }

    trace_size = backtrace(trace, traceSize);
    /* overwrite sigaction with caller's address */
#ifndef __x86_64__
    trace[1] = (void *)ctx->eip; //32 bit instruction pointer register
#elif defined(__arm__)
    trace[1] = (void *)ctx->arm_ip; //arm arch
#else //x86
    trace[1] = (void *)ctx->rip; //64 bit instruction pointer register
#endif
    messages = backtrace_symbols(trace, trace_size);
    /* skip first stack frame (points here) */
    len += snprintf(outBuf, outBufSize, "[bt] Execution path:\n");
    for (i=1; i<trace_size; ++i)
    {
        static const int buflen = 1024;
        char buf[buflen];
        memset(buf, 0, buflen*sizeof(char));
        snprintf(buf, buflen, "[bt] #%d %s", i, messages[i] );
        len += snprintf(outBuf+len, outBufSize, "%s", buf);
        if(NULL == trace[i]){
            len += snprintf(outBuf+len, outBufSize, "\n");
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
                len += snprintf(outBuf+len, outBufSize, "%s", buffer);
            }
            pclose(f);
        }
    }
}

void initSigHandler(pfsig_hup sigHandler)
{
    //register for sigsegv
    struct sigaction sa;
    sa.sa_handler = (__sighandler_t)sigHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGSEGV, &sa, NULL);
}
