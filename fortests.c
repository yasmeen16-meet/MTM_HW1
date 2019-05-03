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
