/*
    @author: stalker320
*/
/// VirtualMemory
#ifndef VM_H
#   define VM_H

#   include <stddef.h>

#   define KILO(value) 1024 * (value)
#   define MEGA(value) KILO(KILO(value))
#   define GIGA(value) MEGA(KILO(value))
// typedef unsigned char
//     byte_t,     BYTE,
//     word_t[2],  WORD[2],
//     dword_t[4], DWORD[4],
//     qword_t[8], QWORD[8];
typedef void *MemoryAddress;

int vmInit(size_t size);
void vmTerminate();
MemoryAddress vmAlloc(size_t size, int* status);
void vmFree(MemoryAddress address);

#endif