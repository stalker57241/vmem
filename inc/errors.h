#ifndef ERRORS_H
#   define ERRORS_H

/// Program finished successfully
#   define OK 0
/// Program failed to finish successfully
#   define FAILED 1
/// Program failed, unable to allocate something
#   define ERR_ALLOCATE 2
/// Program failed, unable to get something
#   define ERR_OPEN 3
/// Program failed, something not initialized
#   define ERR_INIT 4

#endif