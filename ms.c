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

int main(void) {

    return 0;

}