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
