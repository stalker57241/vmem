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

typedef void *MemoryAddress;

int vmInit(size_t size);
void vmTerminate();
MemoryAddress vmAlloc(size_t size, int* status);
void vmFree(MemoryAddress address);
void vmShowRegions(char* prefix);
size_t vmGetFreeMemory();
#endif