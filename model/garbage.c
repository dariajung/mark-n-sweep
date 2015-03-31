#include <stdio.h>
#include <stdlib.h>

#define HEAP_SIZE   100

// inefficient char array first to represent bit array
// Global structure
struct bitarray {
    unsigned char m_map[HEAP_SIZE];
    int b_free;
} BITARRAY;

// a heap object / CHUNK in memory pool
typedef struct heap_object {
    // type - 0 for integer
    // type - 1 for cons
    unsigned char type;

    // null - 0 for non-null
    // null - 1 for null
    unsigned char null;

    // marked - 0 if not marked
    // marked - 1 if marked
    unsigned char marked;

    // next object in the linked list of
    // allocated objects?
    // do i need this?
    // struct heap_object *next;

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
// free list is a list of free chunks of memory
// on request for allocation, choose a free block
// m_free is the next free block
// we are using sequential allocation
struct free_list {
    unsigned char *memory_pool; // pointer to entire memory pool
    int m_free; // int or pointer
} FREE_LIST;

void init_heap() {
    // let's pre-allocate a memory pool
    FREE_LIST.memory_pool = malloc(sizeof(struct heap_object) * HEAP_SIZE);
    // the first available spot in the memory pool
    FREE_LIST.m_free = 0; 
}

// unclear if this is necessary
void init_bitarray() {
    int i;
    for (i = 0; i < HEAP_SIZE; i++) {
        BITARRAY.m_map[i] = 0;
    }
}

void update_bitarray() {
    // mark as not free
    BITARRAY.m_map[BITARRAY.b_free] = 1;
    BITARRAY.b_free++;
}

void print_bitarray() {
    int i;
    printf("\r\n");
    for (i = 0; i < HEAP_SIZE; i++) {
        printf("%d", BITARRAY.m_map[i]);
    }
    printf("\r\n");
}

// need a way to cycle through heap if next slot isn't free
void update_heap(HEAP_OBJECT *new_object) {

    int index = (FREE_LIST.m_free) % sizeof(HEAP_OBJECT);

    if (index >= HEAP_SIZE) {
        printf("Whoops, went too far along the heap\n");
        exit(1);
    }

    // FREE_LIST.memory_pool + FREE_LIST.m_free = new_object;
    // FREE_LIST.m_free += sizeof(HEAP_OBJECT);

    update_bitarray();
}

HEAP_OBJECT * halloc() {
    int index = (FREE_LIST.m_free) % sizeof(HEAP_OBJECT);
    bool garbage_collect = 1;
    HEAP_OBJECT *ptr;

    if (index >= HEAP_SIZE) {
        // we are at the end of the heap, 
        // need to see if/where there are free chunks and if not
        // call the garbage collector
        int i;
        for (i = 0; i < HEAP_SIZE; i++) {
            // this spot is free
            if (BITARRAY.m_map[i] == 0) {
                index = i;
                garbage_collect = 0; // set garbage collect to false
                break;
            }
        }
    } 

    // else we can just go sequentially

    if (garbage_collect == 1) {
        // need to call garbage collect
        printf("Stopping to collect garbage");
        // once garbage collection done, call halloc again
        garbage_collect();
        halloc();
    } else {
        // we can allocate the chunk in the memory pool
        ptr = (HEAP_OBJECT *)(FREE_LIST.memory_pool + FREE_LIST.m_free);
    }

    // set object as not marked
    ptr->marked = 0;
    // give object an address in the memory pool
    ptr->address = FREE_LIST.m_free;

    // update the free list ot the next available chunk of memory
    // should this be done in another function?
    FREE_LIST.m_free += sizeof(HEAP_OBJECT);

    return ptr;
}


// creates, doesn't set value
HEAP_OBJECT * create_heap_object() {
    HEAP_OBJECT *ptr = halloc();
    update_heap(ptr);

    return ptr;
}

// 
HEAP_OBJECT * create_integer(int value) {
    HEAP_OBJECT *ptr = create_heap_object();
    ptr->type = 0; // 0 because int
    ptr->value = value; // set value to value

    return ptr;
}

HEAP_OBJECT * create_NULL() {
    HEAP_OBJECT *ptr = create_heap_object();
    // set nothing
    ptr->null = 1;

    return ptr;
}

HEAP_OBJECT * create_cons(HEAP_OBJECT *car, HEAP_OBJECT *cdr) {
    HEAP_OBJECT *ptr = create_heap_object();
    ptr->type = 1;
    ptr->car = car;
    ptr->cdr = cdr;

    return ptr;
}

void print_object(HEAP_OBJECT *obj) {

    switch(obj->type) {
        case 0:
            if (obj->null == 1) {
                printf("NIL");
            } else {
                printf("%d", obj->value); 
            }
            break;

        case 1:
            printf("(");
            print_object(obj->car);
            printf(", ");
            print_object(obj->cdr);
            printf(")");
            break;
    }
}

void garbage_collect() {
    mark(); // pass root set?
    sweep();
}

void mark() {

}

void sweep() {
    
}

int main() {

    //init bitarray
    init_bitarray();

    init_heap();
    HEAP_OBJECT *new_object = create_integer(7);
    HEAP_OBJECT *null_obj = create_NULL();
    HEAP_OBJECT *nptr = create_cons(new_object, null_obj);

    print_object(nptr);

    print_bitarray();

    free(FREE_LIST.memory_pool);

    return 0;
}
