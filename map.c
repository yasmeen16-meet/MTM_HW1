#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "map.h"
#define EMPTY -1

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

static void moveIteratorToInitialPosition(Map map, Node initial_position) ;

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
    map->copy_data=copyDataElement;
    map->copy_key=copyKeyElement;
    map->free_data= freeDataElement;
    map->free_key= freeKeyElement;
    map->compare_keys= compareKeyElements;
    return map;
}
void mapDestroy(Map map){
    if (map==NULL){
        return;
    }
    mapClear(map);
    free(map);
}
int mapGetSize(Map map){
    if (map==NULL){
        return EMPTY;
    }
    return map->size ;
}
static Node node_Create (Map map ,MapKeyElement keyElement,  MapDataElement dataElement){
    Node node =malloc(sizeof(*node));
    if (node==NULL){
        return NULL;
    }
    node->next=NULL;
    node->previous=NULL;
    node->key=map->copy_key(keyElement);
    if (node->key==NULL){
        free(node);
        return NULL;
    }
    node->data=map->copy_data(dataElement);
    if (node->data==NULL){
        map->free_key(node->key);
        free(node);
        return NULL;
    }
    return node;
}
static MapResult edit_Data(Map* map, Node* iterator ,MapKeyElement keyElement,  MapDataElement dataElement){
    //mapGetFirst(map);
//    Node ptr = *iterator;
    while (*iterator!=NULL){
        if ((*map)->compare_keys(keyElement  , (*iterator)->key)==0){
            MapDataElement data = (*map)->copy_data(dataElement);
            if (data==NULL){
                return MAP_OUT_OF_MEMORY;
            }
            (*map)->free_data((*iterator)->data);
            (*iterator)->data=data;
            break;
        }
        (*iterator)=(*iterator)->next;
    }
    return MAP_SUCCESS;
}

static MapResult add_key(Map map ,MapKeyElement key ,MapDataElement data){
    Node node = node_Create(map,key,data);
    if (node==NULL){
        return MAP_OUT_OF_MEMORY;
    }
    if (map->size==0){
        map->iterator=node;
        return MAP_SUCCESS;
    }
    Node current = map->iterator;
    mapGetFirst(map);
    Node ptr = map->iterator;
    moveIteratorToInitialPosition(map,current);
    while (ptr!=NULL){
        if (map->compare_keys(ptr->key,key)>0){
            node->next=ptr;
            if(ptr->previous==NULL){
                node->previous=NULL;
                map->iterator->previous=node;

            } else{
                ptr=ptr->previous;
                ptr->next=node;
                node->previous=ptr;
                ptr=ptr->next->next;
                ptr->previous=node;
            }
            moveIteratorToInitialPosition(map,ptr);
            return MAP_SUCCESS;
        }
        ptr=ptr->next;
    }
    mapGetFirst(map);
    Node ptr2 = map->iterator;
    while (ptr2->next!=NULL){
        ptr2= ptr2->next;
    }
    ptr2->next=node;
    node->previous=ptr2;
    moveIteratorToInitialPosition(map,ptr2->next);
    return MAP_SUCCESS;

}
MapResult mapPut(Map map, MapKeyElement keyElement, MapDataElement dataElement){
    if (map==NULL||keyElement==NULL ||dataElement==NULL){
        return MAP_NULL_ARGUMENT;
    }
    if (mapContains(map,keyElement)==true) {
        mapGetFirst(map);
        MapResult result = edit_Data(&map,&map->iterator, keyElement, dataElement);
        mapGetFirst(map);
        return result;
    }
    MapResult result=add_key(map,keyElement,dataElement);
    if(result==MAP_SUCCESS){

        map->size=map->size+ 1 ;
    }
    return result;

}
MapResult mapRemove(Map map, MapKeyElement keyElement){
    if(map==NULL || keyElement==NULL){
        return MAP_NULL_ARGUMENT;
    }
    if (!mapContains(map,keyElement)){
        return MAP_ITEM_DOES_NOT_EXIST;
    }
    if (map->size <=1){
        return  mapClear(map);
    }
    mapGetFirst(map);
    if(map->compare_keys(keyElement,map->iterator->key)==0) {
        Node toDelete =map->iterator;
        map->iterator=map->iterator->next;
        map->iterator->previous=NULL;
        map->free_data(toDelete->data);
        map->free_key(toDelete->key);
        free(toDelete);
        map->size--;
        return MAP_SUCCESS;
    }

    while (map->iterator->next!=NULL){
        if (map->compare_keys(keyElement,map->iterator->key)==0){

            Node toDelete = map->iterator;
            map->iterator=map->iterator->next;
            map->iterator->previous=map->iterator->previous->previous;
            map->iterator= map->iterator->previous;

            map->iterator->next=map->iterator->next->next;

            map->free_data(toDelete->data);
            map->free_key(toDelete->key);

            toDelete->next=NULL;
            toDelete->previous=NULL;
            free(toDelete);
            map->size--;
            mapGetFirst(map);
            return MAP_SUCCESS;
        }
        map->iterator=map->iterator->next;
    }
    Node toDelete = map->iterator;
    map->iterator=map->iterator->previous;
    map->iterator->next=NULL;
    map->free_data(toDelete->data);
    map->free_key(toDelete->key);
    free(toDelete);

    map->size--;
    return MAP_SUCCESS;
}


MapKeyElement mapGetNext(Map map){
    if (map==NULL|| map->iterator==NULL||map->iterator->next==NULL){
        return NULL;
    }
    map->iterator=map->iterator->next;
    assert(map->iterator!=NULL);
    return map->iterator->key;
}

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
    mapGetFirst(toAdd);
    Node iterator_help = NULL;
    for (iterator_help = toAdd->iterator; iterator_help != NULL; iterator_help = iterator_help->next) {
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
    if (map->iterator==NULL){
        return false;
    }
    Node current = map->iterator;
    mapGetFirst(map);
    Node iterator_help  = map->iterator;
    ///main iterator goes to the first key

    while(iterator_help != NULL){
        if (map->compare_keys(element, iterator_help->key) == 0) {


            return true;
        }

        iterator_help = iterator_help->next;
    }

    return false;
}


///mapGet function:
static void moveIteratorToInitialPosition(Map map, Node initial_position) {
    assert(map != NULL && initial_position != NULL);
    ///main iterator goes to the first key
    mapGetFirst(map);
    while (map->compare_keys(map->iterator->key, initial_position->key)!=0) {
        map->iterator=map->iterator->next;
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
            data = map->copy_data(iterator_help->data);
            if (data==NULL){
                return NULL;
            }
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
    if (map->iterator == NULL) {
        return NULL;
    }
    while (map->iterator->previous != NULL) {
        map->iterator = map->iterator->previous;
    }
    assert(map->iterator != NULL);
    return map->iterator->key;
}


///mapClear function:
MapResult mapClear(Map map) {
    if (map == NULL) {
        return MAP_NULL_ARGUMENT;
    }
    mapGetFirst(map);
    while (map->iterator != NULL) {
        Node current_key = map->iterator;
        map->iterator=map->iterator->next;
        map->free_data(current_key->data);
        current_key->previous=NULL;
        current_key->next=NULL;
        map->free_key(current_key->key);
        free(current_key);
    }
    map->size = 0;
    return  MAP_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////


