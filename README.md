# VMem
Library for launching Virtual Memory
# Build
Generation:
```sh
$ mkdir build
$ cmake -S . -B build/
```
Compilation:
```sh
$ cmake --build build
```
## Execution
### Linux
```sh
$ build/test
```
### Windows
```cmd
build/test.exe
```
# Usage
```C
#include <vmem.h>

int main() {
  vmemInit(KILO(8);
  void* mem = vmemAlloc(8, NULL);
  vmemFree(mem);
  vmemTerminate();
  return 0;
}
```
