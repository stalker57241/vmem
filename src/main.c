#include <stdio.h>
#include <stdlib.h>
#include <vm.h>

#define TEST_MULTI_OBJECTS 1024
#define TEST_DEBUG
int tests();
int test_single();
int test_multi();
int main() {
    vmInit(KILO(8)); // 8 * 1 kilobyte (1024 bytes) = 8192 bytes
    int status = 0;
    if ((status = tests()) != 0) {
        status = 1;
    }
    printf("Status: %d\n", status);
    vmTerminate();
    return status;
}

int tests () {
    int status = 0;
    printf("Single\n");
    if ((status = test_single()) != 0) return status;
    printf("\tOK\nMulti\n");
    if ((status = test_multi()) != 0) return status;
    printf("\tOK\n");
    return 0;
}


int test_single() {
    int status = 0;
    void* seg1 = vmAlloc(4, &status);
    vmFree(seg1);
    return status;
}

int test_multi() {
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
    return 0;
}