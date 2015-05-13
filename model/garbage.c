#include "garbage.h"


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

// void mark(HEAP_OBJECT *obj) {
//     if (obj->marked) return;

//     obj->marked = 1;

//     // type 1 means cons cell
//     if (obj->type == 1) {
//         mark(obj->car);
//         mark(obj->cdr);
//     }
// }

// void mark_all() {
//     // annoying, O(N)
//     int i;
//     for (i = 0; i < HEAP_SIZE; i++) {
//         if (ROOT_SET[i]) {
//             mark(ROOT_SET[i]);
//         }
//     }
// }

void sweep() {
    printf("sweepy sweep\n");
    int i;
    for (i = 0; i < HEAP_SIZE; i++) {
        // access object at ith chunk of memory in
        // memory pool
        HEAP_OBJECT *ptr = (HEAP_OBJECT *)(HEAP.memory_pool + sizeof(HEAP_OBJECT)*i);
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
    //mark_all(); // pass root set?
    sweep();
}

int main() {

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

    free(HEAP.memory_pool);

    return 0;
}
