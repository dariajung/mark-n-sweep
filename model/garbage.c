#include <stdio.h>
#include <stdlib.h>

#define HEAP_SIZE   100

void update_m_free();
void garbage_collect();

// inefficient char array first to represent bit array
struct bitarray {
    unsigned char m_map[HEAP_SIZE];
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
    // allocated objects
    struct heap_object *next;

    // "address" in FREE_LIST, 
    // or index in the free_list array
    unsigned char * address;

    union {
        int value;

        struct {
            struct heap_object *car;
            struct heap_object *cdr;
        };
    };
} HEAP_OBJECT;

// not sure how to format this
// as an array?
struct root_set {
    unsigned char * roots[HEAP_SIZE];
} ROOT_SET;

// total size of heap is 100 cons_object objects
// free list is a list of free chunks of memory
// on request for allocation, choose a free block
// m_free is the next free block
// we are using sequential allocation
struct free_list {
    unsigned char *memory_pool; // pointer to entire memory pool
    int m_free; // int or pointer in the memory pool

    /* head of linked list of allocated objects */
    HEAP_OBJECT *head;

    /* number of objects allocated on the heap */
    int num_objects;

} FREE_LIST;

void init_heap() {
    // let's pre-allocate a memory pool
    FREE_LIST.memory_pool = malloc(sizeof(struct heap_object) * HEAP_SIZE);

    // the first available spot in the memory pool
    FREE_LIST.m_free = 0; 

    FREE_LIST.head = NULL;
    FREE_LIST.num_objects = 0;
}

// unclear if this is necessary
void init_bitarray() {
    int i;
    for (i = 0; i < HEAP_SIZE; i++) {
        BITARRAY.m_map[i] = 0;
    }
}

void mark_bitarray(int index) {
    // mark as not free
    BITARRAY.m_map[index] = 1;
}

void unmark_bitarray(int index) {
    // mark as free
    BITARRAY.m_map[index] = 0;
}

void print_bitarray() {
    int i;
    printf("\r\n");
    for (i = 0; i < HEAP_SIZE; i++) {
        printf("%d", BITARRAY.m_map[i]);
    }
    printf("\r\n");
}

void update_m_free() {

    if (FREE_LIST.num_objects >= HEAP_SIZE) {
        // call garbage collect
        printf("Stopping to collect garbage\n");
        garbage_collect();
    }

    // if for some reason even after garbage collection
    // there is no space on the heap, return
    if (FREE_LIST.num_objects >= HEAP_SIZE) {
        printf("Heap is currently full\n");
        return;
    }

    // this is the memory at the addr that was just allocated
    // we need to find new chunk of free space for the next
    // allocation
    int index = (FREE_LIST.m_free) / sizeof(HEAP_OBJECT);

    int i, bit_addr;
    for (i = index; i < (index + HEAP_SIZE); i++) {
        bit_addr = i % HEAP_SIZE;
        // if 0, we've found an available chunk of memory
        if (BITARRAY.m_map[bit_addr] == 0) {
            // break out of the loop
            break;
        }
    }
   
    // update the free list ot the next available chunk of memory
    FREE_LIST.m_free = sizeof(HEAP_OBJECT) * bit_addr;
}

// actually finding the free space in the memory pool
HEAP_OBJECT * halloc() {
    HEAP_OBJECT *ptr;
    ptr = (HEAP_OBJECT *)(FREE_LIST.memory_pool + FREE_LIST.m_free);
    // set object as not marked
    ptr->marked = 0;
    // give object an address in the memory pool
    ptr->address = FREE_LIST.memory_pool + FREE_LIST.m_free;
    FREE_LIST.num_objects++;

    mark_bitarray((FREE_LIST.m_free) / sizeof(HEAP_OBJECT));
    // get the next halloc ready to allocate chunk by finding the
    // next available spot of memory
    update_m_free();
    return ptr;
}

void hfree(int i) {
    unmark_bitarray(i);
    FREE_LIST.num_objects--;
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

void mark(HEAP_OBJECT *obj) {
    if (obj->marked) return;

    obj->marked = 1;

    // type 1 means cons cell
    if (obj->type == 1) {
        mark(obj->car);
        mark(obj->cdr);
    }
}

void mark_all() {
    // annoying, O(N)
    int i;
    for (i = 0; i < HEAP_SIZE; i++) {
        if (ROOT_SET[i]) {
            mark(ROOT_SET[i]);
        }
    }
}

void sweep() {
    printf("sweepy sweep\n");
    int i;
    for (i = 0; i < HEAP_SIZE; i++) {
        // access object at ith chunk of memory in
        // memory pool
        HEAP_OBJECT *ptr = (HEAP_OBJECT *)(FREE_LIST.memory_pool + sizeof(HEAP_OBJECT)*i);
        // this was something that was unreachable
        if (ptr->marked == 0) {
            // need to "free"
            printf("unreachable\n");
            hfree(i);
        } else {
            printf("reachable");
            ptr->marked = 0;
        }
    }

    update_m_free();
}

void add_to_root_set() {

}

void garbage_collect() {
    mark_all(); // pass root set?
    sweep();
}

int main() {

    //init bitarray
    init_bitarray();

    init_heap();
    HEAP_OBJECT *new_object = create_integer(7);
    HEAP_OBJECT *null_obj = create_NULL();
    HEAP_OBJECT *nptr = create_cons(new_object, null_obj);

    printf("new_object addr %p\n", new_object->address);
    printf("nptr addr %p\n", nptr->address);

    print_object(nptr);
    print_bitarray();

    garbage_collect();
    print_bitarray();

    free(FREE_LIST.memory_pool);

    return 0;
}
