#include <stdio.h>
#include <stdlib.h>
#include <vmem.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define TEST_MULTI_OBJECTS 1023
#define TEST_DEBUG

#define TEST_EXEC(test_method_name) \
    if ((status = test_method_name()) != 0) return status;

#define MEMORY_SIZE ((size_t) KILO(8))

int tests();
int test_single();
int test_multi();
int test_free();

int main() {
    int status = 0;
    if ((status = tests()) != 0) {
        status = 1;
    }
    printf("Status: %d\n", status);
    return status;
}

int tests () {
    int status = 0;
    printf("Single\n");
    TEST_EXEC(test_single)
    printf("\tOK\nMulti\n");
    TEST_EXEC(test_multi)
    printf("\tOK\nFree\n");
    TEST_EXEC(test_free)
    printf("\tOK\n");
    return 0;
}


int test_single() {
    vmemInit(MEMORY_SIZE); // 8 * 1 kilobyte (1024 bytes) = 8192 bytes
    int status = 0;
    void* seg1 = vmemAlloc(4, &status);
    printf("[test_single][vmemAlloc] usage: %ld/%ld\n", vmemGetFreeMemory(), MEMORY_SIZE);
    vmemFree(seg1);
    printf("[test_single][vmemFree] usage: %ld/%ld\n", vmemGetFreeMemory(), MEMORY_SIZE);
    vmemTerminate();
    return status;
}

int test_multi() {
    vmemInit(MEMORY_SIZE); // 8 * 1 kilobyte (1024 bytes) = 8192 bytes
    for (int x = 0; x < TEST_MULTI_OBJECTS; x++) {
        int status = 0;
        #ifdef TEST_DEBUG
        printf("%d:\t", x);
        #endif
        vmemAlloc(8, &status);
        if (status != 0) {
            #ifdef TEST_DEBUG
            printf("E%d", status);
            if (x % 4 == 0 && x != 0) {
                printf("\n");
                if (x % 32 == 0 && x != 0) {
                    printf("\n");
                } 
            }
            else {
                printf("\t");
            }
            #endif
            return status;
        }
        #ifdef TEST_DEBUG
        printf("%d", status);
        if (x % 4 == 0 && x != 0) {
            printf("\n");
            if (x % 32 == 0 & x != 0) {
                printf("\n");
            } 
        }
        else {
            printf("\t");
        }
        #endif
    }
    #ifdef TEST_DEBUG
    printf("\n");
    #endif
    vmemTerminate();
    return 0;
}

int test_free() {
    vmemInit(MEMORY_SIZE); // 8 * 1 kilobyte (1024 bytes) = 8192 bytes
    void* o0 = vmemAlloc(4, NULL);
    vmemShowRegions("o0 alloc");
    void* o1 = vmemAlloc(4, NULL);
    vmemShowRegions("o1 alloc");
    printf("[test_free][o0 o1 alloc] \e[7musage\e[0m: %ld/%ld\n", vmemGetFreeMemory(), MEMORY_SIZE);
    uintptr_t absposition0 = (uintptr_t) o0;
    uintptr_t absposition1 = (uintptr_t) o1;
    vmemShowRegions("status");
    vmemFree(o0);
    printf("[test_single][vmemFree(o0)] \e[7musage\e[0m: %ld/%ld\n", vmemGetFreeMemory(), MEMORY_SIZE);
    vmemShowRegions("o0 free");
    void* o2 = vmemAlloc(1, NULL);
    void* o3 = vmemAlloc(1, NULL);
    uintptr_t absposition2 = (uintptr_t) o2;
    uintptr_t absposition3 = (uintptr_t) o3;
    vmemShowRegions("o2 o3 alloc");
    printf("[test_single][vmemAlloc o2, o3] \e[7musage\e[0m: %ld/%ld\n", vmemGetFreeMemory(), MEMORY_SIZE);
    void* o4 = vmemAlloc(3, NULL);
    vmemShowRegions("o4 alloc");
    printf("[test_single][vmemAlloc o4] \e[7musage\e[0m: %ld/%ld\n", vmemGetFreeMemory(), MEMORY_SIZE);
    vmemTerminate();
    return 0;
}