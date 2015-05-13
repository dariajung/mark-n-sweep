#include "garbage.h"
#include <assert.h>

void print_object(HEAP_OBJECT *obj) {

    if (obj == NIL) {
        printf("NIL");

    } else {

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
}

void test() {

    //printf("nothing allocated %p\n", HEAP.m_free);

    HEAP_OBJECT *obj1 = create_integer(7);
    //printf("m_free obj1: %p\n", HEAP.m_free);

    HEAP_OBJECT *obj2 = create_integer(8);
    //printf("m_free obj2: %p\n", HEAP.m_free);

    HEAP_OBJECT *null_obj = create_NIL();
    //printf("m_free null object: %p\n", HEAP.m_free);

    HEAP_OBJECT *cons = create_cons(obj1, null_obj);
    //printf("m_free cons: %p\n", HEAP.m_free);

    printf("obj1 addr %p\n", obj1->address);
    printf("cons addr %p\n", cons->address);

    print_object(cons);
    //printf("\nnumber of objects on heap %d\n", HEAP.num_objects);
    print_heap();

    // deliberately mark cons as a root object
    add_to_root_set(cons);
    mark_all();

    assert(obj1->marked == 1);
    assert(obj2->marked == 0);
    assert(cons->marked == 1);

    sweep();

    //garbage_collect();
    print_heap();
    printf("current heap index %d\n", HEAP.index);
}

int main() {
    init_root_set();
    init_heap();
    test();
    free(HEAP.memory_pool);

    return 0;
}
