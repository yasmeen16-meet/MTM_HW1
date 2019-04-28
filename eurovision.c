#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "../eurovision.h"
#include "state.c"
#include "judge.c"
#include "../map.h"


struct eurovision_t{
    State states;
    Judge judges;
};