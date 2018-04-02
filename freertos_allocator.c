
#include <stdlib.h>
#include "FreeRTOS.h"
#include "allocator.h"

void *gen_alloc(size_t size) {
    return pvPortMalloc(size);
}

void gen_free(void *p) {
    vPortFree(p);
}
