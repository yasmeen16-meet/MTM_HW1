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

void judgeDestroy(Judge judge);//free-array

int judgeGetSize(Judge judge);

bool judgeContain(Judge judge , int judgeId);

Judge judgeFind (Judge judge , int judgeId);

JudgeResult judgeAdd (Judge* judge , int judgeId , const char* judgeName, int * judgeResults);
//the array needs to be copy

JudgeResult judgeRemove (Judge* judge , int judgeId); // free arrray

static Judge createJudge ( int judgeId , const char* judgeName, int * judgeResults);

JudgeResult judgeRemoveState (Judge judge , int stateId);

///functions

void judgeDestroy(Judge judge) {
    if (judge == NULL) {
        return;
    }
    while (judge != NULL) {
        Judge current = judge;
        free(judge->judgeResult);
        judge = judge->judgeNext;
        free(current);
    }
}

int judgeGetSize(Judge judge) {
    int size = 0;
    if (judge == NULL) {
        return size;
    }
    Judge help_iterator = judge;
    while (help_iterator != NULL) {
        size = size + 1;
        help_iterator = help_iterator->judgeNext;
    }
    return size;
}

bool judgeContain(Judge judge , int judgeId) {
    if (judge == NULL) {
        return false;
    }
    Judge help_iterator = judge;
    while (help_iterator != NULL) {
        if (help_iterator->judgeId == judgeId) {
            return true;
        }
        help_iterator = help_iterator->judgeNext;
    }
    return false;
}

Judge judgeFind (Judge judge , int judgeId) {
    if (judge == NULL) {
        return NULL;
    }
    Judge help_iterator = judge;
    while (help_iterator != NULL) {
        if (help_iterator->judgeId == judgeId) {
            break;
        }
        help_iterator = help_iterator->judgeNext;
    }
    return help_iterator;
}

static Judge createJudge ( int judgeId , const char* judgeName, int * judgeResults) {
    Judge new = malloc(sizeof(*new));
    if (new == NULL) {
        return NULL;
    }
    new->judgeNext = NULL;
    new->judgeId = judgeId;
    new->judgeResult = malloc(LEN * sizeof(int));
    if (new->judgeResult == NULL) {
        free(new);
        return NULL;
    }
    for (int i = 0; i < LEN; ++i) {
        new->judgeResult[i] = judgeResults[i];
    }
    int len_judgeName = strlen(judgeName) + 1;
    new->judgeName = malloc(len_judgeName * sizeof(char));
    strcpy_s(new->judgeName, (len_judgeName) * sizeof(char), judgeName);
    if (new->judgeName == NULL) {
        free(new->judgeResult);
        free(new);
        return NULL;
    }
    return new;
}

JudgeResult judgeAdd (Judge* judge , int judgeId , const char* judgeName, int * judgeResults) {
    if (judgeName == NULL || judgeResults == NULL) {
        return JUDGE_NULL_ARGUMENT;
    }
    if (*judge == NULL) {
        Judge new = createJudge(judgeId, judgeName, judgeResults);
        if (new == NULL) {
            return JUDGE_OUT_OF_MEMORY;
        }
        *judge = new;
        return JUDGE_SUCCESS;
    }
    if (judgeContain(*judge, judgeId)) {
        return JUDGE_ALREADY_EXISTS;
    }

    Judge help_iterator = *judge;
    while (help_iterator->judgeNext != NULL) {
        help_iterator = help_iterator->judgeNext;
    }
    Judge new = createJudge(judgeId, judgeName, judgeResults);
    if (new == NULL) {
        return JUDGE_OUT_OF_MEMORY;
    }
    help_iterator->judgeNext = new;
    return JUDGE_SUCCESS;
}

JudgeResult judgeRemove (Judge* judge , int judgeId) {

    if (*judge == NULL) {
        return JUDGE_NULL_ARGUMENT;
    }

    if (!judgeContain(*judge, judgeId)) {
        return JUDGE_NOT_EXIST;
    }

    if ((*judge)->judgeId == judgeId) {
        Judge current = *judge;
        (*judge) = (*judge)->judgeNext;
        free(current->judgeResult);
        free(current->judgeName);
        free(current);
        return JUDGE_SUCCESS;
    }

    Judge help_iterator = *judge;
    while (help_iterator->judgeNext != NULL) {
        if (help_iterator->judgeNext->judgeId == judgeId) {
            Judge current = help_iterator->judgeNext;
            help_iterator->judgeNext = help_iterator->judgeNext->judgeNext;
            free(current->judgeResult);
            free(current->judgeName);
            free(current);
            break;
        }
        help_iterator = help_iterator->judgeNext;
    }
    return JUDGE_SUCCESS;
}

JudgeResult judgeRemoveState (Judge judge , int stateId) {
    if (judge == NULL) {
        return JUDGE_NULL_ARGUMENT;
    }
    Judge help_iterator = judge;
    while (help_iterator != NULL) {
        bool isDeleted = false;
        for (int i = 0; i < LEN - 1 && isDeleted == false; ++i) {
            if (help_iterator->judgeResult[i] == stateId) {
                Judge current = judgeFind(judge, help_iterator->judgeId);
                judgeRemove(&judge, current->judgeId);
                isDeleted = true;
            }
        }
        help_iterator = help_iterator->judgeNext;
    }
    return JUDGE_SUCCESS;
}