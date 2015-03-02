#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef enum {
    OBJ_INT
    //OBJ_PAIR
} ObjectType;

// let's make this a node for a linked list!
typedef struct sObject {
    
    struct sObject *next;

    unsigned char mark_bit;
    ObjectType type;
    int value;

} Object;

// whoo let's define a tiny stack-based VM
#define STACK_MAX 256
#define INITIAL_GC_THRESHOLD 8

static void die(const char *message) {
    perror(message);
    exit(1);
}

typedef struct {
    Object *head;
    Object* stack[STACK_MAX];
    int size;

    int currentNum;
    int maxObjects;

} VM;

VM* newVM(void) {
    VM* vm = malloc(sizeof(VM));
    vm->size = 0;
    vm->head = NULL;
    vm->maxObjects = INITIAL_GC_THRESHOLD;
    return vm;
}

void push(VM *vm, Object *obj) {
    if (vm->size > STACK_MAX) {
        die("Uh oh, stack overflow!\n");
    }
    vm->stack[vm->size++] = obj;
}

Object* pop(VM *vm) {
    if (vm->size < 0) {
        die("Nothing on the stack\n");
    }
    return vm->stack[--vm->size];
}

static void mark(Object *object) {

    if (object->mark_bit) {
        return;
    }
    object->mark_bit = 1;
}

static void markAll(VM *vm) {
    int i;

    printf("vm size %d\n", vm->size);

    for (i = 0; i < vm->size; i++) {
        mark(vm->stack[i]);
    }
}

// traverse through Objects in VM
void sweep(VM *vm) {
    Object **obj = &vm->head;
    while(*obj) {

        // if not marked, need to free object
        if (! (*obj)->mark_bit) {

            Object *unref = *obj;
            *obj = unref->next;

            free(unref);
            vm->currentNum--;

        // if marked as 1, keep
        } else {
            // reset markbit to 0
            (*obj)->mark_bit = 0;
            obj = &(*obj)->next;
        }
    }
}

void gc(VM* vm) {

    int numObjects = vm->currentNum;

    markAll(vm);
    sweep(vm);

    printf("Objects originally on stack: %d, Collected: %d, Remaining: %d\n"
        , numObjects, numObjects - vm->currentNum, vm->currentNum);
}

Object* newObject(VM *vm, ObjectType type) {
    if (vm->currentNum == vm->maxObjects) {
        gc(vm);
    }

    Object *object = malloc(sizeof(Object));
    object->type = type;
    object->mark_bit = 0;

    object->next = vm->head;
    vm->head = object;
    vm->currentNum++;

    return object;
}

void pushInt(VM *vm, int value) {
    Object *object = newObject(vm, OBJ_INT);
    object->value = value;
    mark(object);

    push(vm, object);
}

void freeVM(VM *vm) {
    vm->size = 0;
    gc(vm);
    free(vm);
}

void test1() {
    printf("Test 1: Objects on stack are not removed.\n");
    VM *vm = newVM();

    pushInt(vm, 1);
    pushInt(vm, 2);

    gc(vm);

    if (vm->currentNum == 2) {
        printf("Items preserved\n");
    } else {
        printf("What\n");
    }

    freeVM(vm);
}

void test2() {
    printf("Test 2: Add >= max number of objects.\n");

    VM *vm = newVM();
    int i;

    pushInt(vm, 1);
    pushInt(vm, 2);

    pop(vm);
    pop(vm);

    gc(vm);

    if (vm->currentNum == 0) {
        printf("All items collected\n");
    } else {
        printf("Not all items were collected from garbage\n");
    }

    freeVM(vm);
}



int main(void) {

    //test1();
    test2();

    return 0;

}