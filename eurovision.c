#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "../eurovision.h"
#include "../state.c"
#include "../judge.c"
#include "../list.h"
#define ZERO 0

struct eurovision_t{
    State states;
    Judge judges;
};

static bool isValidName(const char * name);
static int rankToScore(int rank);
static int findScore (int* scoreArray ,int stateId);
static EurovisionResult updateStatesScores (State state);
static EurovisionResult updateJudgeScores (Judge judge , State state);
static EurovisionResult calculateFinalScore (State state , int audiencePercent);

////functions application

Eurovision eurovisionCreate(){

    Eurovision  eurovision =malloc(sizeof(*eurovision));
    if (eurovision==NULL){
        return NULL;
    }
    return eurovision;
}

void eurovisionDestroy(Eurovision eurovision){
    if (eurovision == NULL){
        return;
    }
    stateDestroy(eurovision->states);
    judgeDestroy(eurovision->judges);
    free(eurovision);
}

///checking the validity of a given name: small letter or space only
static bool isValidName(const char * name) {
    assert(name != NULL);
    int i = 0;
    while (name[i]) {
        if ( name[i] + i < 'a' || name[i] > 'z') {
            if (name[i] != ' ' ) {
                return false;
            }
        }
        i++;
    }
    return true;
}

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

EurovisionResult eurovisionAddJudge(Eurovision eurovision, int judgeId,
                                    const char *judgeName,
                                    int *judgeResults) {
    if (eurovision == NULL || judgeName == NULL || judgeResults == NULL) {
        return EUROVISION_NULL_ARGUMENT;
    }
    ///checking id validity
    if (judgeId < ZERO) {
        return EUROVISION_INVALID_ID;
    }
    for (int i=0; i<LEN;i++) {
        if (judgeResults[i] < ZERO) {
            return EUROVISION_INVALID_ID;
        }
    }
    ///checking if a stateId is repeated
    for (int i=0; i<LEN-1;i++) {
        for (int j=i+1; j<LEN; j++){
            if (judgeResults[i] == judgeResults[j]){
                return EUROVISION_INVALID_ID;
            }
        }
    }
    ///judgeName validity
    if (!isValidName(judgeName)) {
        return EUROVISION_INVALID_NAME;
    }
    ///state's existence in the array judgeResults
    for (int i = 0; i < LEN; i++) {
        int current_stateId = judgeResults[i];
        if (!stateContain(eurovision->states, current_stateId)) {
            return EUROVISION_STATE_NOT_EXIST;
        }
    }
    if (judgeContain(eurovision->judges, judgeId)) {
        return EUROVISION_JUDGE_ALREADY_EXIST;
    }
    ///All parameters are legal, we can try to add the judge
    JudgeResult result = judgeAdd(eurovision->judges, judgeId, judgeName, judgeResults);
    ///checking results from judgeAdd function
    if (result == JUDGE_NULL_ARGUMENT) {
        return EUROVISION_NULL_ARGUMENT;
    }
    if (result == JUDGE_OUT_OF_MEMORY) {
        eurovisionDestroy(eurovision);
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

EurovisionResult eurovisionRemoveJudge(Eurovision eurovision, int judgeId){
    if (eurovision == NULL){
        return EUROVISION_NULL_ARGUMENT;
    }
    if (judgeId<ZERO){
        return EUROVISION_INVALID_ID;
    }
    if (!judgeContain(eurovision->judges, judgeId)){
        return EUROVISION_JUDGE_NOT_EXIST;
    }
    ///All parameters are legal, we can try to remove the judge
    JudgeResult result = judgeRemove(eurovision->judges, judgeId);
    if(result == JUDGE_NULL_ARGUMENT){
        return EUROVISION_NULL_ARGUMENT;
    }
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


EurovisionResult eurovisionRemoveVote(Eurovision eurovision, int stateGiver,
                                      int stateTaker) {
    if (eurovision == NULL) {
        return EUROVISION_NULL_ARGUMENT;
    }
    if (stateGiver < ZERO || stateTaker < ZERO) {
        return EUROVISION_INVALID_ID;
    }
    if (!stateContain(eurovision->states, stateGiver) || !stateContain(eurovision->states, stateTaker)) {
        return EUROVISION_STATE_NOT_EXIST;
    }
    if (stateGiver == stateTaker) {
        return EUROVISION_SAME_STATE;
    }
    ///All parameters are legal, we can try to remove a vote from stateTaker
    State ptrvoter = stateFind(eurovision->states, stateGiver);
    Map map = ptrvoter->stateVotes;
    ///option 1: stateGiver has not voted to stateTaker in the first place.
    if (!mapContains(map, (int*) stateTaker)) {
        return EUROVISION_SUCCESS;
    }
    ///option 2: stateGiver has voted to stateTaker before
    //reaching the map cell with votes to stateTaker
    int* currentKey = mapGetFirst(map);
    while (currentKey != NULL) {
        if (*currentKey == stateTaker) {
            break;
        }
        currentKey=mapGetNext(map);
    }

    ///suboption 2.1: there is 1 vote only to remove, thus we remove the votes details of this stateTaker
    if (*(int *) mapGet(map,currentKey) == 1) {
        mapRemove(map, currentKey);
        return EUROVISION_SUCCESS;
    }
    ///suboption 2.2: there are more than one vote, thus we reduce the votes number by 1. using mapPut
    int data=*(int*)mapGet(map, currentKey) - 1;
    MapResult result = mapPut(map,currentKey, (int*)data);
    if (result == MAP_NULL_ARGUMENT){
        return EUROVISION_NULL_ARGUMENT;
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

/// returns the score if not found returns 0
static int findScore (int* scoreArray ,int stateId){
    //assert
    int i=0;
    while (i<LEN){
        if (scoreArray[i]==stateId){
            return rankToScore(i);
        }
        i++;
    }
    return ZERO;
}

///updates the stateScores according to the state Array
static EurovisionResult updateStatesScores (State state){
    assert(state!=NULL);
    State  help_iterator = state;
    while (help_iterator!=NULL){
        State help2 = state;
        double  sum =0 ;
        while (help2!=NULL){
            if (help_iterator->stateId==help2->stateId){
                continue;
            }
            int current=findScore(help2->stateVotedScores,help_iterator->stateId);
            if (current>0){
                sum=sum+current;
            }
            help2=help2->stateNext;
        }
        help_iterator->stateStatesScore=sum/(stateGetSize(state)-1);
        help_iterator= help_iterator->stateNext;
    }
    return  EUROVISION_SUCCESS;
}

/// updates average the state score according to the judge array
static EurovisionResult updateJudgeScores (Judge judge , State state){
    if (judge ==NULL || state==NULL){
        return  EUROVISION_NULL_ARGUMENT;
    }

    State  help_iterator = state;
    while (help_iterator!=NULL){
        Judge help2 = judge;
        double  sum =0 ;
        while (help2!=NULL){
            int current=findScore(help2->judgeResult,help_iterator->stateId);
            if (current>0){
                sum=sum+current;
            }
            help2=help2->judgeNext;
        }
        help_iterator->stateJudgesScore=sum/(judgeGetSize(judge));
        help_iterator= help_iterator->stateNext;
    }
    return  EUROVISION_SUCCESS;
}

///calculate weightedScores for each state
static EurovisionResult calculateFinalScore (State state , int audiencePercent){
    assert(state!=NULL);
    State  help_iterator =state;
    while (help_iterator!= NULL){
        int judgePercent = 100-audiencePercent;
        help_iterator->stateWeightedScore= help_iterator->stateStatesScore*(audiencePercent/100.0)+
                                           help_iterator->stateJudgesScore*(judgePercent/100.0);
        help_iterator= help_iterator->stateNext;

    }
    return EUROVISION_SUCCESS;
}




