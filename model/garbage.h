#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* =========================================
The heap will hold 100 fixed sized objects.
========================================== */
#define HEAP_SIZE   100

/* ======================================================
A global variable to represent NIL objects in cons cells
====================================================== */
#define NIL     NULL

void update_m_free();
void garbage_collect();

/* ================================================
Struct representing a heap object / CHUNK in the heap. 
Here, we are emulating a simple LISP object that may
only be one of two types, INTEGER or CONS.
================================================== */
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
    void * address;
    
    /* ============================================
    A union object representing the
    data itself in a heap object. A heap object can
    either be an Integer, or a CONS cell.
    ============================================ */
    union {
        /* An actual integer value */
        int value;

        struct {
            /* CAR (head) and CDR (tail) pointers in a CONS cell */
            struct heap_object *car;
            struct heap_object *cdr;
        };
    };
} HEAP_OBJECT;

/* ==============================================================
An arbitrary representation of the root set using an array of
ints, which represent indicies in the heap's internal array. 

Clearly this is not the most efficient way of doing this; 
a linked list would be much better.

The representation of the root set is also dependent on the 
eventual programming language environment. 
============================================================== */
struct root_set {

    void * roots[HEAP_SIZE];
    int curr;

} ROOT_SET;

/* =============================================================
    The total size of heap is 100 heap objects of fixed size. 
    The free list is a list of free chunks of memory.
    On request for memory allocation, a free block is chosen. 
    The m_free pointer points to the next free block. 
    For simplicity, we are using contiguous/sequential allocation.
=============================================================== */

struct heap {

    /* pointer to beginning of the heap */
    void * memory_pool;

    /* pointer to the end of the heap */
    void * memory_boundary;

    size_t heap_size;

    /* The pointer to the next free address in the heap */ 
    void * m_free;

    /* Actual index within bit vector representation */
    int index;

    /* ================================================
        Head of linked list of allocated objects.
        Ended up being unused in this implementation,
        but would make it more convenient to traverse
        all objects occupying allocated space in the heap.
    ================================================  */
    HEAP_OBJECT *head;

    /* ================================================
        Not a true bit vector/an inefficient char array
        for the purposes of modeling. 

        A map denoting which objects within the heap 
        have been allocated or are free.
    ================================================ */
    unsigned char m_map[HEAP_SIZE];

    /* number of objects allocated on the heap */
    int num_objects;

} HEAP;

void init_heap() {
    int i;

    /* let's pre-allocate a memory pool that will serve as our "heap" */
    HEAP.memory_pool = malloc(sizeof(struct heap_object) * HEAP_SIZE);
    HEAP.memory_boundary = HEAP.memory_pool + (sizeof(HEAP_OBJECT)) * HEAP_SIZE;

    /* ================================================
        The first available spot in the memory pool is 
        the address of where the memory pool itself begins. 
    ================================================ */
    HEAP.m_free = HEAP.memory_pool; 
    HEAP.heap_size = HEAP.memory_boundary - HEAP.memory_pool;
    HEAP.index = 0;
    
    HEAP.head = NULL;
    HEAP.num_objects = 0;

    /* =================================================
        Initialize the heap memory map to all 0s to denote
        that we are starting from a clean heap with no 
        objects allocated on it. 
    =================================================== */
    for (i = 0; i < HEAP_SIZE; i++) {
        HEAP.m_map[i] = 0;
    }
}

void mark_bitarray(int index) {
    /* mark index/chunk as allocated */
    HEAP.m_map[index] = 1;
}

void unmark_bitarray(int index) {
    /* mark index/chunk as free */
    HEAP.m_map[index] = 0;
}

/* Print the current state of the heap */
void print_heap() {
    int i;
    printf("\r\n");
    for (i = 0; i < HEAP_SIZE; i++) {
        printf("%d", HEAP.m_map[i]);
    }
    printf("\r\n");
}

/* Update the index of m_free */
void update_m_free() {

    printf("update m free called\n");

    /* ==============================================
        Call garbage collect, we no longer have
        any free space for allocating new objects. 
    ================================================ */
    if (HEAP.num_objects > HEAP_SIZE || HEAP.m_free > HEAP.memory_boundary) {
        printf("Stopping to collect garbage\n");
        garbage_collect();
    }

    /* ================================================
        If for some reason even after garbage collection
        there is no space on the heap, return 
    ================================================ */
    if (HEAP.num_objects > HEAP_SIZE || HEAP.m_free > HEAP.memory_boundary) {
        printf("Heap is currently full. Exiting\n");
        exit(1);
    }

    /* =======================================================
        This is the memory at the addr that was just allocated
        we need to find new chunk of free space for the next
        allocation.
    ======================================================== */

    int i;
    for (i = HEAP.index; i < (HEAP.index + HEAP_SIZE); i++) {
        // if 0, we've found an available chunk of memory
        if (HEAP.m_map[(i % HEAP_SIZE)] == 0) {
            /* ================================================
                Break out of the loop.
                Update m_free to point to the address of the
                next available chunk of memory.
            ================================================ */
            HEAP.m_free = HEAP.memory_pool + sizeof(HEAP_OBJECT)*(i % HEAP_SIZE);
            HEAP.index = i % HEAP_SIZE;
            break;
        }
    }
}

/* Actually allocating the free space in the memory pool */
HEAP_OBJECT * halloc() {
    //printf("halloc\n");
    HEAP_OBJECT *ptr;
    ptr = (HEAP_OBJECT *)(HEAP.m_free);
    /* set object as not marked */
    ptr->marked = 0;
    /* give object an address in the memory pool */
    ptr->address = HEAP.m_free;
    HEAP.num_objects++;

    mark_bitarray(HEAP.index);
    print_heap();

    printf("HEAP INDEX %d\n", HEAP.index);


    /* ================================================
        Get the next halloc ready to allocate chunk by 
        finding the next available spot of memory.
    ================================================ */
    update_m_free();
    return ptr;
}

/* ================================================
    "Frees" the object from the heap, and its address 
    is marked as available for memory allocation.
================================================ */
void hfree(int i) {
    printf("hfreeing %dth index\n", i);
    unmark_bitarray(i);
    HEAP.num_objects--;
}

// creates a new heap object, doesn't set value
HEAP_OBJECT * create_heap_object() {
    HEAP_OBJECT *ptr = halloc();
    return ptr;
}

HEAP_OBJECT * create_integer(int value) {
    //printf("create integer\n");
    HEAP_OBJECT *ptr = create_heap_object();
    ptr->type = 0; // 0 because int
    ptr->value = value; // set value to value

    return ptr;
}

HEAP_OBJECT * create_NIL() {
    HEAP_OBJECT *ptr = NIL;

    return ptr;
}

HEAP_OBJECT * create_cons(HEAP_OBJECT *car, HEAP_OBJECT *cdr) {
    HEAP_OBJECT *ptr = create_heap_object();
    ptr->type = 1;
    ptr->car = car;
    ptr->cdr = cdr;

    return ptr;
}

void mark(HEAP_OBJECT *obj) {

    printf("Marking!\n");

    if (!obj) {
        return;
    }

    if (obj->type == 0) {
        printf("it's an int: %d\n", obj->value);
    }

    if (obj->marked == 1) return;

    obj->marked = 1;

    // type 1 means cons cell
    if (obj->type == 1) {
        printf("it's a cons\n");
        mark(obj->car);
        mark(obj->cdr);
    }
}

void mark_all() {
    // annoying, O(N)
    int i;

    HEAP_OBJECT * obj;
    for (i = 0; i < HEAP_SIZE; i++) {
        if (ROOT_SET.roots[i]) {
            obj = (HEAP_OBJECT *)ROOT_SET.roots[i];
            mark(obj);
        }
    }
}


/* Look at this in a bit */
void sweep() {
    printf("sweepy sweep\n");
    int i;
    for (i = 0; i < HEAP_SIZE; i++) {

        /* access object at ith chunk of memory in heap */
        HEAP_OBJECT *ptr = (HEAP_OBJECT *)(HEAP.memory_pool + sizeof(HEAP_OBJECT)*i);

        printf("%p\n", (HEAP.memory_pool + sizeof(HEAP_OBJECT)*i));

        /* this was something that was unreachable */
        if (ptr->marked == 0) {
            printf("unreachable\n");
            memset(ptr, 0, sizeof(HEAP_OBJECT));
            hfree(i);
        } else {
            printf("reachable\n");
            ptr->marked = 0;
        }
    }

    update_m_free();
}

/* ================================================== */
/*  Functions for mimicking the behavior of a rootset */
/* ================================================== */

void init_root_set() {
    int i;

    for (i = 0; i < HEAP_SIZE; i++) {
        ROOT_SET.roots[i] = NULL;
    }
    ROOT_SET.curr = 0;
}

void add_to_root_set(HEAP_OBJECT *obj) {
    printf("I've been called!\n");
    ROOT_SET.roots[ROOT_SET.curr] = obj->address;
    ROOT_SET.curr++;
}

void garbage_collect() {
    mark_all(); // pass root set?

    printf("we are sweeping now\n");
    sweep();
}
