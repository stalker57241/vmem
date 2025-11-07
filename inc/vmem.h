/*
    @author: stalker320
*/
/// VirtualMemory
#ifndef VMEM_H
#   define VMEM_H

#   include <stddef.h>

#   define KILO(value) 1024 * (value)
#   define MEGA(value) KILO(KILO(value))
#   define GIGA(value) MEGA(KILO(value))

typedef void *MemoryAddress;

int vmemInit(size_t size);
void vmemTerminate();
MemoryAddress vmemAlloc(size_t size, int* status);
void vmemFree(MemoryAddress address);
void vmemShowRegions(char* prefix);
size_t vmemGetFreeMemory();
#endif