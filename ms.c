#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef enum {
    OBJ_INT
    //OBJ_PAIR
} ObjectType;

typedef struct sObject {
    unsigned char mark_bit;
    ObjectType type;
    int value;
} Object;

// whoo let's define a tiny stack-based VM
#define STACK_MAX 256

static void die(const char *message) {
    perror(message);
    exit(1);
}

typedef struct {
    Object* stack[STACK_MAX];
    int size;
} VM;

VM* newVM(void) {
    VM* vm = malloc(sizeof(VM));
    vm->size = 0;
    return vm;
}

void push(VM *vm, Object *obj) {
    if (vm->size < STACK_MAX) {
        die("Uh oh, stack overflow!\n");
    }
    vm->stack[vm->size++] = obj;
}

Object* pop(VM *vm) {
    if (vm->size > 0) {
        die("Nothing on the stack\n");
    }
    return vm->stack[(vm->size)--];
}

Object* newObject(VM *vm, ObjectType type) {
    Object *object = malloc(sizeof(Object));
    object->type = type;
    return object;
}

static void mark(Object *object) {
    object->mark_bit = 1;
}

static void markAll(VM *vm) {
    int i;
    for (i = 0; i < vm->size; i++) {
        mark(vm->stack[i]);
    }
}

void pushInt(VM *vm, int value) {
    Object *object = newObject(vm, OBJ_INT);
    object->value = value;
    mark(object);
    push(vm, object);
}

int main(void) {

    return 0;

}