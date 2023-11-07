#include <stdlib.h>
#include "gc.h"

typedef struct blokk {
    void *pointer;
    int refCount;
    struct blokk *next;
} blokk_t;

struct gc {
    blokk_t *head;
};

gc_t *gc_init(void) {
    gc_t *gc = (gc_t *)malloc(sizeof(gc_t));
    if (gc != NULL) {
        gc->head = NULL;
    }
    return gc;
}

void *gc_malloc(gc_t *gc, size_t size) {
    void *mem = malloc(size);
    if (mem != NULL) {
        blokk_t *block = (blokk_t *)malloc(sizeof(blokk_t));
        if (block != NULL) {
            block->pointer = mem;
            block->refCount = 1;
            block->next = gc->head;
            gc->head = block;
        }
    }
    return mem;
}

void gc_register(gc_t *gc, void *ptr) {
    blokk_t *block = gc->head;
    while (block != NULL) {
        if (block->pointer == ptr) {
            block->refCount++;
            break;
        }
        block = block->next;
    }
}

void gc_free(gc_t *gc, void *ptr) {
    blokk_t *prev = NULL;
    blokk_t *current = gc->head;

    while (current != NULL) {
        if (current->pointer == ptr) {
            if (current->refCount > 0) {
                current->refCount--;
                if (current->refCount == 0) {
                    free(current->pointer);
                    if (prev != NULL) {
                        prev->next = current->next;
                    } else {
                        gc->head = current->next;
                    }
                    free(current);
                }
            }
            break;
        }
        prev = current;
        current = current->next;
    }
}

void gc_cleanup(gc_t *gc) {
    blokk_t *current = gc->head;
    while (current != NULL) {
        blokk_t *next = current->next;
        free(current->pointer);
        free(current);
        current = next;
    }
    free(gc);
}
