
#include <stdlib.h>
#include "allocator.h"

void* gen_alloc( size_t size ) {
    return malloc(size);
}

void gen_free(void *p) {
    free(p);
}
