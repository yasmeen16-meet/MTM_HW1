#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "map.c"


#define LEN 10
#define NOMOREMAX -1
typedef struct state{
    int flag;
    int stateId;
    char* stateName;
    char* songName;
    Map stateVotes;
    int* stateVotedScores;
    double stateJudgesScore;
    double stateStatesScore;
    double stateWeightedScore;
    struct state* stateNext;
}*State;

typedef enum StateResult_t{
    STATE_SUCCESS,
    STATE_NULL_ARGUMENT,
    STATE_OUT_OF_MEMORY,
    STATE_ALREADY_EXISTS,
    STATE_NOT_EXIST,
}StateResult;


State stateCreate(int stateId, const char* stateName, const char* songName);

///destroying one singal state
void stateSingalDestroy(State stateToDelete);

void stateDestroy(State state);

///uses mapCopy
State* stateCopy(State state, State* new_copy);

///needs help_iterator
int stateGetSize(State state);

bool stateContain(State state, int stateId);

State stateFind(State state, int stateId);

///uses mapCreate
StateResult stateAdd(State* state, int stateId, const char* stateName, const char* songName);

///uses stateAdd
StateResult stateAddWithMap(State* state, int stateId, const char* stateName, const char* songName, Map stateVotes,
        double stateWeightedScore);
///uses mapDestroy
StateResult stateRemove(State* state, int stateId);

StateResult deleteOutState (State state , int stateId);

///additional functions
static int* intCopy(int* num);
static void intFree(int* num);
static int intCompare(int* num1, int* num2);


//applications of the functions:
///function stateCreate
State stateCreate(int stateId, const char* stateName, const char* songName) {
    assert(stateName != NULL || songName != NULL);
    State new_state = malloc(sizeof(*new_state));
    if (new_state == NULL) {
        return NULL;
    }///stateName
    int len_stateName = strlen(stateName) + 1;
     new_state->stateName = malloc(sizeof(char) * (len_stateName));
    if (new_state->stateName == NULL) {
        free(new_state);
        return NULL;
    }
    strcpy_s(new_state->stateName,sizeof(char) * (len_stateName),stateName);
    ///songName
    int len_songName = strlen(songName) + 1;
    new_state->songName= malloc(sizeof(char) * (len_songName));
    if (new_state->songName == NULL) {
        free(new_state);
        free(new_state->stateName);
        return NULL;
    }
    strcpy_s(new_state->songName,sizeof(char) * (len_songName),songName);
    ///Map stateVotes
    int *(*ptrCopy)(int *) =intCopy;
    void (*ptrFree)(int *) =intFree;
    int (*ptrCompare)(int *, int *) =intCompare;
    Map new_map = mapCreate(ptrCopy, ptrCopy, ptrFree, ptrFree, ptrCompare);
    if (new_map == NULL) {
        free(new_state);
        free(new_state->stateName);
        free(new_state->songName);
        return NULL;
    }///stateVotedScores
    int *stateVotedScores = malloc(sizeof(int) * LEN);
    if (stateVotedScores == NULL) {
        free(new_state);
        free(new_state->stateName);
        free(new_state->songName);
        mapDestroy(new_map);
        return NULL;
    }
    for (int i = 0; i <LEN ; ++i) {
        stateVotedScores[i]=NOMOREMAX;
    }
    ///placing the fields
    new_state->flag=0;
    new_state->stateId = stateId;
    new_state->stateVotes = new_map;
    new_state->stateVotedScores = stateVotedScores;
    new_state->stateJudgesScore = 0.0;
    new_state->stateStatesScore = 0.0;
    new_state->stateWeightedScore = 0.0;
    new_state->stateNext=NULL;
    return new_state;
}

///destroying one singal state
void stateSingalDestroy(State stateToDelete){
    free(stateToDelete->stateName);
    free(stateToDelete->songName);
    mapDestroy(stateToDelete->stateVotes);
    free(stateToDelete->stateVotedScores);
    free(stateToDelete);
}

///function stateDestroy
void stateDestroy(State state) {
    if (state == NULL) {
        return;
    }
    while (state != NULL) {
        State stateToDelete = state;
        state = state->stateNext;
        stateSingalDestroy(stateToDelete);
    }
}

///function stateCopy**************
State* stateCopy(State state, State* new_copy) {
    if (state == NULL) {
        return NULL;
    }
    State help_iterator = state;
    while (help_iterator != NULL) {
        StateResult result = stateAddWithMap(new_copy, help_iterator->stateId, help_iterator->stateName,
                                             help_iterator->songName, help_iterator->stateVotes,
                                             help_iterator->stateWeightedScore);
        if (result != STATE_SUCCESS) {
            stateDestroy(*new_copy);
            return NULL;
        }
        help_iterator = help_iterator->stateNext;
    }
    return new_copy;
}

///needs help_iterator
int stateGetSize(State state) {
    if (state == NULL) {
        return 0;
    }
    State help_iterator = state;
    int size = 0;
    while (help_iterator != NULL) {
        size++;
        help_iterator = help_iterator->stateNext;
    }
    return size;
}


///function stateContain
bool stateContain(State state, int stateId) {
    if (state == NULL) {
        return false;
    }
    State help_iterator = state;
    while (help_iterator != NULL) {
        if (help_iterator->stateId == stateId) {
            return true;
        }
        help_iterator = help_iterator->stateNext;
    }
    return false;
}


///function stateFind
State stateFind(State state, int stateId) {
    if (state == NULL) {
        return NULL;
    }
    State help_iterator = state;
    while (help_iterator != NULL) {
        if (help_iterator->stateId == stateId) {
            return help_iterator;
        }
        help_iterator = help_iterator->stateNext;
    }
    return NULL;
}

///uses mapCreate
///uses mapCreate
StateResult stateAdd(State* state , int stateId, const char* stateName, const char* songName){
    if (stateName==NULL || songName==NULL){
        return STATE_NULL_ARGUMENT;
    }
    ///if the state is empty and we are adding the first state
    if (*(state) == NULL){
        State stateToAdd = stateCreate(stateId, stateName, songName);
        if (stateToAdd==NULL){
            return STATE_OUT_OF_MEMORY;
        }
        *(state) = stateToAdd;
        return STATE_SUCCESS;
    }
    ///the state is not empty
    ///checking if the stateId already exists
    if (stateContain(*state, stateId)){
        return STATE_ALREADY_EXISTS;
    }
    ///the state does not exist, thus it needs to be added
    State help_iterator = *state;
    ///bringing the help_iterator to the final state
    while (help_iterator->stateNext!=NULL) {
        help_iterator = help_iterator->stateNext;
    }
    State stateToAdd = stateCreate(stateId, stateName, songName);
    if (stateToAdd==NULL){
        return STATE_OUT_OF_MEMORY;
    }
    help_iterator->stateNext = stateToAdd;
    return STATE_SUCCESS;
}

///uses stateAdd
StateResult stateAddWithMap(State* state, int stateId, const char* stateName, const char* songName, Map stateVotes,
                            double stateWeightedScore) {
    if (stateName == NULL || songName == NULL || stateVotes == NULL) {
        return STATE_NULL_ARGUMENT;
    }
    StateResult result = stateAdd(state, stateId, stateName, songName);
    if (result != STATE_SUCCESS) {
        return result;
    }

    State added_state = stateFind(*state, stateId);
    assert(added_state != NULL);
    ///Destroying the previous allocated map in stateCreate() used above in stateAdd function
    mapDestroy(added_state->stateVotes);

    Map map = mapCopy(stateVotes);
    if (map == NULL) {
        return STATE_OUT_OF_MEMORY;
    }
    added_state->stateVotes = map;
    added_state->stateWeightedScore = stateWeightedScore;
    return STATE_SUCCESS;
}

///uses mapDestroy**************
StateResult stateRemove(State* state, int stateId) {
    if (*state == NULL) {
        return STATE_NULL_ARGUMENT;
    }
    ///checking if the stateId does not exist
    if (!stateContain(*state, stateId)) {
        return STATE_NOT_EXIST;
    }

    ///if we are removing the first state
    if ((*state)->stateId == stateId) {
        State stateToDelete = *state;
        *state = (*state)->stateNext;
        stateSingalDestroy(stateToDelete);
        return STATE_SUCCESS;
    }

    ///we to need search for the state after the first one
    State help_iterator = *state;
    while (help_iterator->stateNext != NULL) {
        if (help_iterator->stateNext->stateId == stateId) {
            State stateToDelete = help_iterator->stateNext;
            help_iterator->stateNext = help_iterator->stateNext->stateNext;
            stateToDelete->stateNext=NULL;
            stateSingalDestroy(stateToDelete);
            break;
        }
        help_iterator = help_iterator->stateNext;
    }
    return STATE_SUCCESS;
}



//needs to be changed
static int* intCopy(int* num){
    if (!num) {
        return NULL;
    }
    int *copy = malloc(sizeof(*copy));
    if (!copy) {
        return NULL;
    }
    *copy = * num;
    return copy;
}

static void intFree(int* num){
    free((int*)num);
}

static int intCompare(int* num1, int* num2){
    return *(num1) - *(num2);
}
StateResult deleteOutState (State state , int stateId){
    if (state==NULL){
        return STATE_NULL_ARGUMENT;
    }
    State help_iterator =state;
    while (help_iterator!=NULL){
        if (mapContains(help_iterator->stateVotes,(int *)stateId)){
            mapRemove(help_iterator->stateVotes,(int*)stateId );
        }
        help_iterator=help_iterator->stateNext;
    }
    return  STATE_SUCCESS;

}

