#include <stdio.h>
#include <stdlib.h>

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

    // null - 0 for non-null
    // null - 1 for null
    unsigned char null;

    // marked - 0 if not marked
    // marked - 1 if marked
    unsigned char marked;

    // 0 if not a root object
    // 1 if a root object
    //unsigned char root;

    // "address" in FREE_LIST, 
    // or index in the free_list array
    int address;

    union {
        int value;

        struct {
            struct heap_object *car;
            struct heap_object *cdr;
            // if car or cdr is NULL, what to do?
        };
    };
} HEAP_OBJECT;


// total size of heap is 100 cons_object objects
// make this global
// let's pre-allocate memory
struct free_storage {
    unsigned char *heap_space;
    int current_write_spot;
} FREE_LIST;

void init_heap() {
    FREE_LIST.heap_space = malloc(sizeof(struct heap_object) * HEAP_SIZE);
    FREE_LIST.current_write_spot = 0; 
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

void print_bitarray() {
    int i;
    printf("\r\n");
    for (i = 0; i < HEAP_SIZE; i++) {
        printf("%d", BITARRAY.bitarr[i]);
    }
    printf("\r\n");
}

// need a way to cycle through heap if next slot isn't free
void update_heap(HEAP_OBJECT *new_object) {

    int index = (FREE_LIST.current_write_spot) / sizeof(HEAP_OBJECT);

    if (index >= HEAP_SIZE) {
        printf("Whoops, went too far along the heap\n");
        exit(1);
    }

    // FREE_LIST.heap_space + FREE_LIST.current_write_spot = new_object;
    // FREE_LIST.current_write_spot += sizeof(HEAP_OBJECT);

    update_bitarray();
}

// creates, doesn't set value
HEAP_OBJECT * create_heap_object() {
    HEAP_OBJECT *ptr = (HEAP_OBJECT *)(FREE_LIST.heap_space + FREE_LIST.current_write_spot);
    ptr->marked = 0;
    ptr->address = FREE_LIST.current_write_spot;

    FREE_LIST.current_write_spot += sizeof(HEAP_OBJECT);

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

int main() {

    //init bitarray
    init_bitarray();

    init_heap();
    HEAP_OBJECT *new_object = create_integer(7);
    HEAP_OBJECT *null_obj = create_NULL();
    HEAP_OBJECT *nptr = create_cons(new_object, null_obj);

    print_object(nptr);

    print_bitarray();

    free(FREE_LIST.heap_space);

    return 0;
}
