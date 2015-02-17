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

int main(void) {

    return 0;

}