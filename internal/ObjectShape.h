void _ClearObject(auto ObjectData){
  for(uint32_t i = 0; i < ObjectData->ShapeList.Current; i++){
    auto ShapeData = &ObjectData->ShapeList.ptr[i];
    switch(ShapeData->ShapeEnum){
      case ShapeEnum_t::Circle:{
        this->UnlinkRecycleOrphanShape_Circle(ShapeData->ShapeID);
        break;
      }
      case ShapeEnum_t::Rectangle:{
        this->UnlinkRecycleOrphanShape_Rectangle(ShapeData->ShapeID);
        break;
      }
      case ShapeEnum_t::DPF:{
        this->UnlinkRecycleOrphanShape_DPF(ShapeData->ShapeID);
        break;
      }
    }
  }
}

void RecycleObject(
  ObjectID_t ObjectID
){
  auto ObjectData = &this->ObjectList[ObjectID];
  _ClearObject(ObjectData);
  ShapeList_Close(&ObjectData->ShapeList);
  this->ObjectList.Recycle(ObjectID);
}

/* _ClearObject is internal and doesnt set current to 0 */
void ClearObject(ObjectID_t ObjectID){
  auto ObjectData = &this->ObjectList[ObjectID];
  _ClearObject(ObjectData);
  ObjectData->ShapeList.Current = 0;
}
