#include "garbage.h"


void print_object(HEAP_OBJECT *obj) {

    if (!obj) {
        printf("NIL");
    }

    switch(obj->type) {
        case 0:
            printf("%d", obj->value); 
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
    print_heap();

    garbage_collect();
    print_heap();

    free(HEAP.memory_pool);

    return 0;
}
