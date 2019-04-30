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
static int rankToScore(int rank);



////functions application

///rankToScore function
static int rankToScore(int rank){

    switch (rank){
        case 0:
            return 12;
        case 1:
            return 10;
        case 2:
            return 8;
        case 3:
            return 7;
        case 4:
            return 6;
        case 5:
            return 5;
        case 6:
            return 4;
        case 7:
            return 3;
        case 8:
            return 2;
    }
    return 1;
}

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