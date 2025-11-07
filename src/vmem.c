/*  @author: stalker320
    @date: 2.11.2025 (DD.MM.YYYY)
 */
#include <vmem.h>
#include <stdlib.h>
#include <errors.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

static struct vmemData {
    char* baseAddress;
    size_t capacity;
    void* null;
    struct Region* regionStack;
}* vmemData = NULL;
// Utility
struct Region {
    struct {
        bool used:1;
    };
    size_t capacity;
    struct Region* next;
};
/// @brief 
/// @param size always incremented by 1, for null-pointer offset
/// @return 
int vmemInit(size_t size) {
    vmemData = malloc(sizeof(struct vmemData));
    if (vmemData == NULL) return ERR_ALLOCATE;
    if (size < 0)       return ERR_ALLOCATE;
    size_t real_size = size + 1;
    vmemData->baseAddress = malloc(real_size);
    vmemData->capacity = real_size;
    vmemData->regionStack = malloc(sizeof(struct Region));
    vmemData->regionStack->used = false;
    vmemData->regionStack->capacity = real_size;
    vmemData->regionStack->next = NULL;
    int status = 0;
    vmemData->null = vmemAlloc(1, &status);
    printf("Null creation status: %s\n", status == 0 ? "OK" : "FAILED");
    vmemShowRegions(NULL);
    return OK;
}
// Utility
int vmemMarkRegion(size_t size, struct Region* region) {
    size_t base_size = region->capacity;
    if (size > base_size) return ERR_ALLOCATE;
    if (region->used == true) return ERR_ALLOCATE;
    struct Region* next = region->next;
    struct Region* newNext = malloc(sizeof(struct Region));
    if (base_size - size > 0) {
        newNext->next = next;
        newNext->capacity = base_size - size;
        newNext->used = false;
    }
    region->next = newNext;
    region->capacity = size;
    region->used = true;
    return OK;
}
// Utility
void vmemRegionTakeNext(struct Region** pregion, size_t* poffset) {
    if (poffset != NULL) {
        *poffset += (*pregion)->capacity;
    }
    *pregion = (*pregion)->next;
}
// Utility
struct Region* vmemFindUnmarkedRegion(size_t size, size_t* poffset) {
    // Required region:
    // used = false
    // capacity < size
    if (vmemData == NULL) return NULL;
    struct Region* region = vmemData->regionStack;
    *poffset = 0;
    while (region != NULL) {
        if (region->used) {
            vmemRegionTakeNext(&region, poffset);
        }
        else if (region->capacity < size) { // used = false
            vmemRegionTakeNext(&region, poffset);
        } // used = false, capacity < size
        else {
            return region;
        }
    }
    return NULL;
}
void vmemMergeRegion(struct Region* region) {
    if (region == NULL) return;
    if (region->next->used != false) return;
    struct Region* newNext = region->next->next;
    size_t joinedCapacity = region->capacity + region->next->capacity;
    region->capacity = joinedCapacity;
    free(region->next);
    region->next = newNext;
}
MemoryAddress vmemAlloc(size_t size, int* status) {
    if (vmemData == NULL) {
        *status = ERR_INIT;
        return NULL;
    }
    if (size > vmemData->capacity) { if (status) *status = ERR_ALLOCATE; return NULL; }
    size_t offset = 0;
    struct Region* region = vmemFindUnmarkedRegion(size, &offset);
    if (region == NULL) { if (status) *status = ERR_ALLOCATE; return NULL; }
    if (vmemMarkRegion(size, region) != OK) {if (status) *status = ERR_ALLOCATE; return NULL; }
    if (status) *status = OK;
    
    uintptr_t address = (uintptr_t) vmemData->baseAddress + (uintptr_t) offset;
    return (MemoryAddress) address;
}

int vmemFree(MemoryAddress address) {
    if (vmemData == NULL) return ERR_INIT;
    if (address == NULL) return ERR_OPEN;
    struct Region* region = vmemData->regionStack;
    size_t offset = 0;
    while (offset < (uintptr_t) address - (uintptr_t) vmemData->baseAddress) {
        vmemRegionTakeNext(&region, &offset);
        if (region == NULL) return ERR_OPEN;
    }
    region->used = false;
    if (region->next->used == false) {
        vmemMergeRegion(region);
    }
    return 0;
}
int vmemShowRegions(char* prefix) {
    if (vmemData == NULL) return ERR_INIT;
    if (prefix != NULL)
        printf("%s\n", prefix);
    struct Region* region = vmemData->regionStack;
    size_t offset = 0;
    printf("[%ld..%ld] used: %s\tcapacity: %ld\n", 
        offset, offset + region->capacity,
        region->used ? "true" : "false", region->capacity);
    offset += region->capacity;
    while ((region = region->next) != NULL) {
        printf("%s[%ld..%ld(%ld)]\e[0m", 
            region->used ? "\e[41;30m" : "\e[42;30m",
            offset, offset + region->capacity,
            region->capacity
        );
        offset += region->capacity;
    }
    printf("\n");
    return OK;
}
void vmemTerminate() {
    struct Region* region = vmemData->regionStack;
    while (region) {
        struct Region* temp = region;
        region = region->next;
        free(temp);
    }
    free(vmemData->baseAddress);
    free(vmemData);
}

size_t vmemGetFreeMemory() {
    if (vmemData == NULL) return 0;
    struct Region* region = vmemData->regionStack;
    size_t capacity = 0;
    while ((region = region->next) != NULL) {
        if (!region->used)
            capacity += region->capacity;
    }
    return capacity;
}