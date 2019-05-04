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
static EurovisionResult updateStatesScores (State* state);
static EurovisionResult updateJudgeScores (Judge judge , State* state);
static EurovisionResult calculateFinalScore (State state , int audiencePercent);
///returns the stateId with max vote from a map of votes\
/// needs to check if two votes amounts equal, if so returns the smaller id in number
static int getMaxIdMap(Map* map);
///prepares the stateVotesScores array of each state
static EurovisionResult updateStateArray(State* state);
///returns the stateId ith max votes from State (it's copy)
static int getMaxIdState(State state_copy);
/** Function to be used for copying a char as a data to the map */
static ListElement copyDataChar(ListElement n);
/** Function to be used by the list for freeing elements */
static void freeChar(ListElement n);
static void printArray (int* scoreArray );
static void printStatesScore (Eurovision eurovision);
static void printJudgesScore (Eurovision eurovision);
static void printFinalScore (Eurovision eurovision);
static char* strCat(char *dest, char *src);
void sortStrings(List listOfSrtings);
