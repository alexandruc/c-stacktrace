#include <stdlib.h>
#include <unistd.h>
#include <iostream>

#define EXE_PATH "/home/alex/workarea/c-stacktrace-build/testcpp/testcpp"
extern "C" {
#include "../stacktrace.h"
}

static void sig_hup(int sig,  struct sigcontext ctx)
{
    std::cout << "sig_hup: got signal %d" << sig << std::endl;
    switch(sig)
    {
    case SIGSEGV:
    {
        static const unsigned int size = 2048;
        char outBuf[size];
        printStackTrace(outBuf, size, EXE_PATH, &ctx);
        std::cout << outBuf << std::endl;
        break;
    }
    }

    _exit(1); //prevent looping
}

class CCrashMe
{
public:
    void callCrashMe()
    {
        crashMe();
    }

    /* dummy crash function */
    void crashMe()
    {
        char* p = 0;
        *p = 'a';
    }
};


int main(int argc, char* argv[])
{
    initSigHandler(&sig_hup);

    //generate a crash
    CCrashMe crasher;
    crasher.callCrashMe();

    return 0;
}
