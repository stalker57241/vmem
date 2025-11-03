#include <stdio.h>
#include <stdlib.h>
#include <vm.h>
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
    vmInit(MEMORY_SIZE); // 8 * 1 kilobyte (1024 bytes) = 8192 bytes
    int status = 0;
    void* seg1 = vmAlloc(4, &status);
    printf("[test_single][vmAlloc] usage: %ld/%ld\n", vmGetFreeMemory(), MEMORY_SIZE);
    vmFree(seg1);
    printf("[test_single][vmFree] usage: %ld/%ld\n", vmGetFreeMemory(), MEMORY_SIZE);
    vmTerminate();
    return status;
}

int test_multi() {
    vmInit(MEMORY_SIZE); // 8 * 1 kilobyte (1024 bytes) = 8192 bytes
    for (int x = 0; x < TEST_MULTI_OBJECTS; x++) {
        int status = 0;
        #ifdef TEST_DEBUG
        printf("%d:\t", x);
        #endif
        vmAlloc(8, &status);
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
    vmTerminate();
    return 0;
}

int test_free() {
    vmInit(MEMORY_SIZE); // 8 * 1 kilobyte (1024 bytes) = 8192 bytes
    void* o0 = vmAlloc(4, NULL);
    vmShowRegions("o0 alloc");
    void* o1 = vmAlloc(4, NULL);
    vmShowRegions("o1 alloc");
    printf("[test_free][o0 o1 alloc] \e[7musage\e[0m: %ld/%ld\n", vmGetFreeMemory(), MEMORY_SIZE);
    uintptr_t absposition0 = (uintptr_t) o0;
    uintptr_t absposition1 = (uintptr_t) o1;
    vmShowRegions("status");
    vmFree(o0);
    printf("[test_single][vmFree(o0)] \e[7musage\e[0m: %ld/%ld\n", vmGetFreeMemory(), MEMORY_SIZE);
    vmShowRegions("o0 free");
    void* o2 = vmAlloc(1, NULL);
    void* o3 = vmAlloc(1, NULL);
    uintptr_t absposition2 = (uintptr_t) o2;
    uintptr_t absposition3 = (uintptr_t) o3;
    vmShowRegions("o2 o3 alloc");
    printf("[test_single][vmAlloc o2, o3] \e[7musage\e[0m: %ld/%ld\n", vmGetFreeMemory(), MEMORY_SIZE);
    void* o4 = vmAlloc(3, NULL);
    vmShowRegions("o4 alloc");
    printf("[test_single][vmAlloc o4] \e[7musage\e[0m: %ld/%ld\n", vmGetFreeMemory(), MEMORY_SIZE);
    vmTerminate();
    return 0;
}