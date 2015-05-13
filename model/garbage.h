#include <stdio.h>
#include <stdlib.h>

/* --------------------

The heap will hold 100 fixed sized objects.

--------------------- */
#define HEAP_SIZE   100

/* ------------------------

A global variable to represent NIL objects in cons cells

------------------------ */
#define NIL     NULL

void update_m_free();
void garbage_collect();

/* ------------------------

Not a true bit vector/an inefficient char array to represent a bit vector
for the purposes of modeling. 
    
------------------------ */
struct bitarray {

    unsigned char m_map[HEAP_SIZE];

} BITARRAY;

/* ------------------------

a heap object / CHUNK in the heap.

------------------------*/
typedef struct heap_object {

    /* type - 0 for integer */
    /* type - 1 for cons */
    unsigned char type;

    /* marked - 0 if not marked */
    /* marked - 1 if marked */
    unsigned char marked;

    /* next object in the linked list of allocated objects */
    struct heap_object *next;

    /* An "address" in FREE_LIST, or index in the free_list array */
    unsigned char * address;
    
    /* ------------------------
    
    A union object representing the
    data itself in a heap object. A heap object can
    either be an Integer, or a CONS cell.
    
    ------------------------ */
    union {
        /* An actual integer value */
        int value;

        struct {
            /* CAR and CDR pointers in a CONS cell */
            struct heap_object *car;
            struct heap_object *cdr;
        };
    };
} HEAP_OBJECT;

/* ------------------------

An arbitrary representation of the root set using an
array of HEAP_OBJECTs. Clearly this is not the most
efficient way of doing this, but for the purposes of 
a model, suffices. The representation of the root set
is also dependent on the eventual programming language
environment. 

------------------------*/

struct root_set {
    HEAP_OBJECT * roots[HEAP_SIZE];
} ROOT_SET;

/* ------------------------

The total size of heap is 100 heap objects of fixed size. 
The free list is a list of free chunks of memory.
On request for memory allocation, a free block is chosen. 
The m_free pointer points to the next free block. 
For simplicity, we are using contiguous/sequential allocation.

------------------------*/

struct heap {
    /* pointer to entire memory pool */
    unsigned char *memory_pool;

    /* The index or "pointer" to the next free space in the free list */ 
    int m_free;

    /* head of linked list of allocated objects */
    HEAP_OBJECT *head;

    unsigned char m_map[HEAP_SIZE];

    /* number of objects allocated on the heap */
    int num_objects;

} HEAP;

void init_heap() {
    /* let's pre-allocate a memory pool */
    HEAP.memory_pool = malloc(sizeof(struct heap_object) * HEAP_SIZE);

    /* the first available spot in the memory pool */
    HEAP.m_free = 0; 
    
    HEAP.head = NULL;
    HEAP.num_objects = 0;

    int i;
    for (i = 0; i < HEAP_SIZE; i++) {
        HEAP.m_map[i] = 0;
    }
}

void mark_bitarray(int index) {
    // mark as not free
    HEAP.m_map[index] = 1;
}

void unmark_bitarray(int index) {
    // mark as free
    HEAP.m_map[index] = 0;
}

void print_bitarray() {
    int i;
    printf("\r\n");
    for (i = 0; i < HEAP_SIZE; i++) {
        printf("%d", HEAP.m_map[i]);
    }
    printf("\r\n");
}

void update_m_free() {

    if (HEAP.num_objects >= HEAP_SIZE) {
        // call garbage collect
        printf("Stopping to collect garbage\n");
        garbage_collect();
    }

    // if for some reason even after garbage collection
    // there is no space on the heap, return
    if (HEAP.num_objects >= HEAP_SIZE) {
        printf("Heap is currently full\n");
        return;
    }

    // this is the memory at the addr that was just allocated
    // we need to find new chunk of free space for the next
    // allocation
    int index = (HEAP.m_free) / sizeof(HEAP_OBJECT);

    int i, bit_addr;
    for (i = index; i < (index + HEAP_SIZE); i++) {
        bit_addr = i % HEAP_SIZE;
        // if 0, we've found an available chunk of memory
        if (HEAP.m_map[bit_addr] == 0) {
            // break out of the loop
            break;
        }
    }
   
    // update the free list ot the next available chunk of memory
    HEAP.m_free = sizeof(HEAP_OBJECT) * bit_addr;
}

// actually finding the free space in the memory pool
HEAP_OBJECT * halloc() {
    HEAP_OBJECT *ptr;
    ptr = (HEAP_OBJECT *)(HEAP.memory_pool + HEAP.m_free);
    // set object as not marked
    ptr->marked = 0;
    // give object an address in the memory pool
    ptr->address = HEAP.memory_pool + HEAP.m_free;
    HEAP.num_objects++;

    mark_bitarray((HEAP.m_free) / sizeof(HEAP_OBJECT));
    // get the next halloc ready to allocate chunk by finding the
    // next available spot of memory
    update_m_free();
    return ptr;
}

void hfree(int i) {
    unmark_bitarray(i);
    HEAP.num_objects--;
}

// creates, doesn't set value
HEAP_OBJECT * create_heap_object() {
    HEAP_OBJECT *ptr = halloc();
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