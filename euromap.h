#ifndef EUROMAP_H_
#define EUROMAP_H_

#include <stdbool.h>

typedef struct node {
    struct node* previous;
    MapKeyElement key ;
    MapDataElement data;
    struct node* next ;
}*Node;

struct Map_t{
    int  size;
    Node iterator;
    copyMapDataElements copy_data;
    copyMapKeyElements copy_key;
    freeMapDataElements free_data;
    freeMapKeyElements free_key;
    compareMapKeyElements compare_keys ;
};

typedef enum MapResult_t {
    MAP_SUCCESS,
    MAP_OUT_OF_MEMORY,
    MAP_NULL_ARGUMENT,
    MAP_ITEM_ALREADY_EXISTS,
    MAP_ITEM_DOES_NOT_EXIST
} MapResult;

typedef void *MapDataElement;

typedef void *MapKeyElement;

typedef MapDataElement(*copyMapDataElements)(MapDataElement);

typedef MapKeyElement(*copyMapKeyElements)(MapKeyElement);

typedef void(*freeMapDataElements)(MapDataElement);

typedef void(*freeMapKeyElements)(MapKeyElement);

typedef int(*compareMapKeyElements)(MapKeyElement, MapKeyElement);

Map mapCreate(copyMapDataElements copyDataElement,
              copyMapKeyElements copyKeyElement,
              freeMapDataElements freeDataElement,
              freeMapKeyElements freeKeyElement,
              compareMapKeyElements compareKeyElements);

void mapDestroy(Map map);

Map mapCopy(Map map);

int mapGetSize(Map map);

bool mapContains(Map map, MapKeyElement element);

MapResult mapPut(Map map, MapKeyElement keyElement, MapDataElement dataElement);

MapDataElement mapGet(Map map, MapKeyElement keyElement);

MapResult mapRemove(Map map, MapKeyElement keyElement);

MapKeyElement mapGetFirst(Map map);

MapKeyElement mapGetNext(Map map);

MapResult mapClear(Map map);





#endif /* EUROMAP_H_ */