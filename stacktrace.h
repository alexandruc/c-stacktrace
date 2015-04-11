#include <signal.h>

//some check so that the path to the executable is defined at compile time
#ifndef EXE_PATH
#error EXE_PATH is not defined before including <stacktrace.h>
#endif

/**
 * signature of the signal handler function
 */
typedef void (*pfsig_hup)(int ,  struct sigcontext );

/**
 * @brief Prints the stack trace in a buffer
 * @param outBuf Buffer to be filled with the stack trace
 * @param outBufSize size of the buffer
 * @param exePath path to the executable
 * @param ctx sigcontext structure received by the signal handler
 */
extern void printStackTrace(char outBuf[], const unsigned int outBufSize, const char exePath[], const struct sigcontext *ctx);

/**
 * @brief Attaches to SIGSEGV signal
 * @param sigHandler pointer to signal handler function
 */
extern void initSigHandler(pfsig_hup sigHandler);
