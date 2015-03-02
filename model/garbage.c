#include <stdio.h>

#define HEAP_SIZE   100

// inefficient char array first to represent bit array
// Global structure
struct bitarray {
    unsigned char bitarr[HEAP_SIZE];
    int curr_free_index;
} BITARRAY;

// a cons_object is 16 BYTES 
typedef struct heap_object {
    // type - 0 for integer
    // type - 1 for cons
    unsigned char type;

    // marked - 0 if not marked
    // marked - 1 if marked
    unsigned char marked;

    // 0 if not a root object
    // 1 if a root object
    unsigned char root;

    // "address" in FREE_LIST, 
    // or index in the free_list array
    int address;

    union {
        int value;

        struct {
            struct heap_object *car;
            struct heap_object *cdr;
        };
    };
} HEAP_OBJECT;


// total size of heap is 100 cons_object objects
// do I need to keep track of roots?
typedef struct free_storage {
    HEAP_OBJECT *free_list[HEAP_SIZE];
    HEAP_OBJECT *next_free;
    int next_free_index;

} FREE_LIST;

FREE_LIST* init_heap() {
    FREE_LIST fl;
    FREE_LIST *fptr = &fl;

    fptr->next_free = fptr->free_list[0];
    fptr->next_free_index = 0; 

    return fptr;
}

void init_bitarray() {
    int i;
    for (i = 0; i < HEAP_SIZE; i++) {
        BITARRAY.bitarr[i] = 0;
    }
}

void update_bitarray() {
    BITARRAY.bitarr[BITARRAY.curr_free_index] = 1;
    BITARRAY.curr_free_index++;
}


// we will use sequential/first fit allocation for now since it is the simplest
// void allocate() {

// }

int main() {

    printf("size of cons_object %d\n", (int)sizeof(HEAP_OBJECT));

    printf("size of HEAP %d\n", (int)sizeof(FREE_LIST));

    //init bitarray
    init_bitarray();

    FREE_LIST *fptr = init_heap();
    HEAP_OBJECT new_object;
    new_object.type = 0;
    new_object.marked = 0;
    new_object.address = BITARRAY.curr_free_index;
    new_object.root = 1;
    new_object.value = 17;

    fptr->next_free = &new_object;
    fptr->next_free += sizeof(HEAP_OBJECT);

    return 0;
}
