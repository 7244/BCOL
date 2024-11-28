/* if multiple object is in same place, undefined behavior */
ObjectID_t GetObjectIDByPosition(
  _vf pos
){
  auto ObjectID = this->ObjectList.GetNodeFirst();
  while(ObjectID != this->ObjectList.dst){
    auto ObjectData = this->GetObjectData(ObjectID);

    ShapeID_t ShapeID;
    ShapeID.ID = 0;
    for(; ShapeID.ID < ObjectData->ShapeList.Current; ShapeID.ID++){
      auto ShapeEnum = ObjectData->ShapeList.ptr[ShapeID.ID].ShapeEnum;
      switch(ShapeEnum){
        case ShapeEnum_t::Circle:{
          auto CircleData = ShapeData_Circle_Get(ObjectData->ShapeList.ptr[ShapeID.ID].ShapeID);
          _vf diff = pos - ObjectData->Position + CircleData->Position;
          _f squaresum = 0;
          for(auto dc = _dc; dc--;){
            squaresum += diff[dc] * diff[dc];
          }
          if(squaresum <= CircleData->Size * CircleData->Size){
            return ObjectID;
          }
          break;
        }
        case ShapeEnum_t::Rectangle:{
          auto RectangleData = ShapeData_Rectangle_Get(ObjectData->ShapeList.ptr[ShapeID.ID].ShapeID);
          auto spos = ObjectData->Position + RectangleData->Position;
          auto ssize = RectangleData->Size;
          auto dc = _dc;
          while(dc--){
            if(pos[dc] < spos[dc] - ssize[dc] || pos[dc] > spos[dc] + ssize[dc]){
              break;
            }
          }
          if(dc < _dc){
            return ObjectID;
          }
          break;
        }
        case ShapeEnum_t::DPF:{
          /* not possible to implement */
          break;
        }
      }
    }
    ObjectID = ObjectID.Next(&this->ObjectList);
  }

  return ObjectID_t(false);
}
