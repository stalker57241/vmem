/*  @author: stalker320
    @date: 2.11.2025 (DD.MM.YYYY)
 */
#include <vm.h>
#include <stdlib.h>
#include <errors.h>
#include <stdbool.h>
#include <stdint.h>

static struct VMData {
    char* baseAddress;
    size_t capacity;
    void* null;
    struct Region* regionStack;
}* vmData = NULL;
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
int vmInit(size_t size) {
    vmData = malloc(sizeof(struct VMData));
    if (vmData == NULL) return ERR_ALLOCATE;
    if (size < 0)       return ERR_ALLOCATE;
    size_t real_size = size;
    vmData->baseAddress = malloc(size + 1);
    vmData->capacity = real_size;
    vmData->regionStack = malloc(sizeof(struct Region));
    vmData->regionStack->used = false;
    vmData->regionStack->capacity = real_size;
    vmData->regionStack->next = NULL;
    vmData->null = vmAlloc(1, NULL);
    return OK;
}
// Utility
int vmMarkRegion(size_t size, struct Region* region) {
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
void vmRegionTakeNext(struct Region** pregion, size_t* poffset) {
    if (poffset != NULL) {
        *poffset += (*pregion)->capacity;
    }
    *pregion = (*pregion)->next;
}
// Utility
struct Region* vmFindUnmarkedRegion(size_t size, size_t* poffset) {
    // Required region:
    // used = false
    // capacity < size
    struct Region* region = vmData->regionStack;
    *poffset = 0;
    while (region != NULL) {
        if (region->used) {
            vmRegionTakeNext(&region, poffset);
        }
        else if (region->capacity < size) { // used = false
            vmRegionTakeNext(&region, poffset);
        } // used = false, capacity < size
        else {
            return region;
        }
    }
    return NULL;
}
void vmMergeRegions() {
    struct Region* region;
    while ((region = vmFindUnmarkedRegion(0, NULL)) != NULL) {
        if (region->next->used != false) continue;
        struct Region* newNext = region->next->next;
        size_t joinedCapacity = region->capacity + region->next->capacity;
        region->capacity = joinedCapacity;
        free(region->next);
        region->next = newNext;
    }
}
MemoryAddress vmAlloc(size_t size, int* status) {
    if (size > vmData->capacity) { if (status) *status = ERR_ALLOCATE; return NULL; }
    size_t offset = 0;
    struct Region* region = vmFindUnmarkedRegion(size, &offset);
    if (region == NULL) { if (status) *status = ERR_ALLOCATE; return NULL; }
    if (vmMarkRegion(size, region) != OK) {if (status) *status = ERR_ALLOCATE; return NULL; }
    if (status) *status = OK;
    
    uintptr_t address = (uintptr_t) vmData->baseAddress + (uintptr_t) offset;
    return (MemoryAddress) address;
}

void vmFree(MemoryAddress address) {
    if (address == NULL) return;
    size_t offset = 0;
    struct Region* region = vmData->regionStack;
    uintptr_t iaddr = (uintptr_t) address;
    while (offset != iaddr) {
        vmRegionTakeNext(&region, &offset);
        if (region == NULL) return;
    }
    region->used = false;
    if (region->next->used == false) {
        vmMergeRegions();
    }
}

void vmTerminate() {
    struct Region* region = vmData->regionStack;
    while (region) {
        struct Region* temp = region;
        region = region->next;
        free(temp);
    }
    free(vmData->baseAddress);
    free(vmData);
}