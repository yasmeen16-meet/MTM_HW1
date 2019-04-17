#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "map.h"


typedef struct node {
    struct node *previous;
    MapKeyElement key;
    MapDataElement data;
    struct node *next;
}*Node;

struct Map_t {
    ///in C we can't have a static variable inside structs.
    int size;
    Node iterator;
    copyMapDataElements copy_data;
    copyMapKeyElements copy_key;
    freeMapDataElements free_data;
    freeMapKeyElements free_key;
    compareMapKeyElements compare_keys;
};

Map mapCreate(copyMapDataElements copyDataElement,copyMapKeyElements copyKeyElement,freeMapDataElements freeDataElement,
              freeMapKeyElements freeKeyElement,compareMapKeyElements compareKeyElements) {
    if (!copyDataElement || !copyKeyElement || !freeDataElement || !freeKeyElement || !compareKeyElements) {
        return NULL;
    }
    Map map = malloc(sizeof(*map));
    if (map == NULL) {
        return NULL;
    }
    map->size = 0;
    map->iterator = NULL;
    map->copy_data = copyDataElement;
    map->copy_key = copyKeyElement;
    map->free_data = freeDataElement;
    map->free_key = freeKeyElement;
    map->compare_keys = compareKeyElements;
    return map;
}


///////////////////////YASMEEN'S CODE GOES HERE/////////////////////////////

///mapCopy function:
static MapResult addOrDestroy(Map map, MapKeyElement keyElement, MapDataElement dataElement) {
    assert(map != NULL && keyElement != NULL && dataElement != NULL);
    MapResult result = mapPut(map, keyElement, dataElement);
    if (result == MAP_OUT_OF_MEMORY) {
        mapDestroy(map);
    }
    return result;
}

static  MapResult addAllOrDestroy(Map map, Map toAdd) {
    assert(map != NULL && toAdd != NULL);
    ///move the main iterator to the beginning
    mapGetFirst(map);
    Node iterator_help = NULL;
    for (iterator_help = map->iterator; iterator_help != NULL; iterator_help = iterator_help->next) {
        if (addOrDestroy(map, iterator_help->key, iterator_help->data) == MAP_OUT_OF_MEMORY) {
            return MAP_OUT_OF_MEMORY;
        }
    }
    return MAP_SUCCESS;
}

Map mapCopy (Map map) {
    if (map == NULL) {
        return NULL;
    }
    Map newMap = mapCreate(map->copy_data, map->copy_key, map->free_data, map->free_key, map->compare_keys);
    if (newMap == NULL) {
        return NULL;
    }
    if (addAllOrDestroy(newMap, map) == MAP_OUT_OF_MEMORY) {
        return NULL;
    }

    ///TO CHECK///
    ///is the iterator of newMap defined to a place somewhere after using mapPut???///

    return newMap;
}


///mapContains function:
bool mapContains(Map map, MapKeyElement element) {
    if (map == NULL || element == NULL) {
        return false;
    }
    mapGetFirst(map);
    Node iterator_help = NULL;
    for (iterator_help = map->iterator; iterator_help != NULL; iterator_help = iterator_help->next) {
        if (map->compare_keys(element, iterator_help->key) == 0) {
            return true;
        }
    }
    return false;
}


///mapGet function:
static void moveIteratorToInitialPosition(Map map, Node initial_position) {
    assert(map != NULL && initial_position != NULL);
    ///main iterator goes to the first key
    mapGetFirst(map);
    while (map->iterator != initial_position) {
        mapGetNext(map);
        assert(map->iterator != NULL);
    }
    ///when the while loop ends, meaning it's pointing to where initial_position is
}

MapDataElement  mapGet(Map map, MapKeyElement keyElement) {
    if (map == NULL || keyElement == NULL) {
        return NULL;
    }
    if (!mapContains(map, keyElement)) {
        return NULL;
    }
    ///if gets here meaning the map isn't empty AND the map contains this key
    ///iterator status must not be changed, thus saving initial position
    Node initial_position = map->iterator;
    MapDataElement data = NULL;
    ///main iterator moves to the first key
    mapGetFirst(map);
    Node iterator_help = NULL;
    for (iterator_help = map->iterator; iterator_help != NULL; iterator_help = iterator_help->next) {
        if (map->compare_keys(keyElement, iterator_help->key) == 0) {
            data = map->iterator->data;
            moveIteratorToInitialPosition(map, initial_position);
            break;
        }
    }
    return data;
}

///mapGetFirst function:
MapKeyElement mapGetFirst(Map map) {
    if (map == NULL) {
        return NULL;
    }
    ///checks if the map is empty
    if (map->size == 0) {
        return NULL;
    }
    while (map->iterator->previous != NULL) {
        map->iterator = map->iterator->previous;
    }
    assert(map->iterator != NULL);
    return map->iterator->key;
}


///mapClear function:
MapResult mpClear(Map map) {
    if (map == NULL) {
        return MAP_NULL_ARGUMENT;
    }
    mapGetFirst(map);
    while (map->iterator != NULL) {
        Node current_key = map->iterator;
        mapGetNext(map);
        map->free_data(current_key->data);
        map->free_key(current_key->key);
        free(current_key);
    }
}

////////////////////////////////////////////////////////////////////////////

