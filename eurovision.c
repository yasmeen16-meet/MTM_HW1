#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "../eurovision.h"
#include "state.c"
#include "judge.c"
#include "../list.h"
#include "../map.h"


struct eurovision_t{
    State states;
    Judge judges;
};

static bool isValidName(char * name);





////functions application

///checking the validity of a given name: small letter or space only
static bool isValidName(char * name) {
    assert(name != NULL);
    int i = 0;
    while (name[i]) {
        if (name[i] != ' ' || name[i] < 'a' || name[i] > 'z') {
            return false;
        }
        i++;
    }
    return true;
}