#ifndef JUDGE_H_
#define JUDGE_H_

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#define LEN 10

typedef struct judge {
    int judgeId;
    char *judgeName;
    int *judgeResult;
    struct judge *judgeNext;
}*Judge;

typedef enum JudgeResult_t {
    JUDGE_SUCCESS,
    JUDGE_NULL_ARGUMENT,
    JUDGE_OUT_OF_MEMORY,
    JUDGE_ALREADY_EXISTS,
    JUDGE_NOT_EXIST,
}JudgeResult;


void judgeDestroy(Judge judge);

int judgeGetSize(Judge judge);

bool judgeContain(Judge judge , int judgeId);

Judge judgeFind (Judge judge , int judgeId);

JudgeResult judgeAdd (Judge* judge , int judgeId , const char* judgeName, int * judgeResults);

JudgeResult judgeRemove (Judge* judge , int judgeId);

JudgeResult judgeRemoveState (Judge judge , int stateId);



#endif /*  JUDGE_H_ */