#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef enum {
    OBJ_INT
    //OBJ_PAIR
} ObjectType;

typedef struct sObject {
    ObjectType type;
    int value;
} Object;

// whoo let's define a tiny stack-based VM
#define STACK_MAX 256

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
    assert(vm->size < STACK_MAX, "Uh oh, stack overflow!\n");
    vm->stack[vm->size++] = obj;
}

Object* pop(VM *vm) {
    assert(vm->size > 0, "Nothing on the stack\n");
    return vm->stack[(vm->size)--];
}

int main(void) {

    return 0;

}