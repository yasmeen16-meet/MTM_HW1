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
EurovisionResult eurovisionAddState(Eurovision eurovision, int stateId, const char *stateName, const char *songName){
    if (eurovision==NULL || stateName ==NULL || songName==NULL){
        return EUROVISION_NULL_ARGUMENT;
    }
    if (stateId < 0){
        return EUROVISION_INVALID_ID;
    }
    if (!isValidName(stateName)||!isValidName(songName)){
        return EUROVISION_INVALID_NAME;
    }
    if(stateContain(eurovision->states,stateId)){
        return EUROVISION_STATE_ALREADY_EXIST;
    }
    StateResult result = stateAdd(eurovision->states,stateId,stateName,songName);
    if (result == STATE_OUT_OF_MEMORY){
        //  eurovisionDestroy(eurovision);
        return EUROVISION_OUT_OF_MEMORY;
    }
    return EUROVISION_SUCCESS;
}

EurovisionResult eurovisionRemoveState(Eurovision eurovision, int stateId){
    if (eurovision==NULL ){
        return  EUROVISION_NULL_ARGUMENT;
    }
    if (stateId < 0){
        return EUROVISION_INVALID_ID;
    }
    if (!stateContain(eurovision->states,stateId)){
        return EUROVISION_STATE_NOT_EXIST;
    }
    judgeRemoveState(eurovision->judges, stateId);
    deleteOutState(eurovision->states,stateId);
    stateRemove(eurovision->states,stateId);
    return EUROVISION_SUCCESS;
}

EurovisionResult eurovisionAddVote(Eurovision eurovision, int stateGiver, int stateTaker) {
    if (eurovision == NULL) {
        return EUROVISION_NULL_ARGUMENT;
    }
    if (stateGiver < 0 || stateTaker) {
        return EUROVISION_INVALID_ID;
    }
    if (!stateContain(eurovision->states, stateGiver) || !stateContain(eurovision->states, stateTaker)) {
        return EUROVISION_STATE_NOT_EXIST;
    }
    if (stateGiver == stateTaker) {
        return EUROVISION_SAME_STATE;
    }
    State stateGive = stateFind(eurovision->states, stateGiver);
    Map map = stateGive->stateVotes;
    if (mapContains(map, (int*)stateTaker)) {
        MapResult result = mapPut(map, (int *) stateTaker, mapGet(map, (int *) (stateTaker + 1)));
        if (result == MAP_OUT_OF_MEMORY) {
            // eurovisionDestroy(eurovision);
            return EUROVISION_OUT_OF_MEMORY;
        }
        if (result == MAP_NULL_ARGUMENT) {
            return EUROVISION_NULL_ARGUMENT;
        }
    } else {
        int j = 0;
        MapResult result = mapPut(map, (int *) stateTaker, mapGet(map, (int *) (j + 1)));
        if (result == MAP_OUT_OF_MEMORY) {
            // eurovisionDestroy(eurovision);
            return EUROVISION_OUT_OF_MEMORY;
        }
        if (result == MAP_NULL_ARGUMENT) {
            return EUROVISION_NULL_ARGUMENT;
        }
    }
    return EUROVISION_SUCCESS;

}
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