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




static int getMaxIdMap(Map* map_copy) {
    assert(map_copy != NULL);

    mapGetFirst(*map_copy);
    if ((*map_copy)->iterator==NULL){
        return NOMOREMAX;
    }
    int max_id = NOMOREMAX;
    int max_votes = 0;
    while ((*map_copy)->iterator->next!=NULL){
        if (*((int*)(*map_copy)->iterator->data)> max_votes){
            max_votes = *((int*)(*map_copy)->iterator->data);
            max_id = *((int*) (*map_copy)->iterator->key);
        }else if (*((int*)(*map_copy)->iterator->data)== max_votes){
            max_id =  *((int*) (*map_copy)->iterator->key) < max_id?
                      *((int*) (*map_copy)->iterator->key):max_id;
        }
        (*map_copy)->iterator = (*map_copy)->iterator->next;
    }

    if (*((int*)(*map_copy)->iterator->data)> max_votes){
        max_votes = *((int*)(*map_copy)->iterator->data);
        max_id = *((int*) (*map_copy)->iterator->key);
    }else if (*((int*)(*map_copy)->iterator->data)== max_votes){
        max_id =  *((int*) (*map_copy)->iterator->key) < max_id?
                  *((int*) (*map_copy)->iterator->key):max_id;
    }
    return max_id;
}
