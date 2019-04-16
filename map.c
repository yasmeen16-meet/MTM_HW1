#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "map.h"

typedef struct node {
    struct node* previous;
    MapKeyElement key ;
    MapDataElement data;
    struct node* next ;
}*Node;

struct Map_t{
    static int  size ;
    Node iterator;
    copyMapDataElements copy_data;
    copyMapKeyElements copy_key;
    freeMapDataElements free_data;
    freeMapKeyElements free_key;
    compareMapKeyElements compare_keys ;
};

Map mapCreate(copyMapDataElements copyDataElement,copyMapKeyElements copyKeyElement,freeMapDataElements freeDataElement,
        freeMapKeyElements freeKeyElement,compareMapKeyElements compareKeyElements){
    if(!copyDataElement || !copyKeyElement || !freeDataElement || !freeKeyElement || !compareKeyElements){
        return NULL;
    }
    Map map = malloc(sizeof(*map));
    if(map==NULL){
        return  NULL;
    }
    map->size=0;
    map->iterator =NULL;
    map->cope_data=copyDataElement;
    map->copy_key=copyKeyElement;
    map->free_data= freeDataElement;
    map->free_key= freeKeyElement;
    map->compare_keys= compareKeyElements;
    return map;
}
