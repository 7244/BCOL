struct ShapeProperties_DPF_t{
  ShapeData_DPF_t::p_t p;
};

ShapeData_DPF_t *ShapeData_DPF_Get(
  ShapeID_t ShapeID
){
  return &this->ShapeList_DPF[ShapeID];
}

ShapeID_t NewShape_DPF(
  ObjectID_t ObjectID,
  ShapeProperties_DPF_t *Properties
){
  auto ShapeID = this->ShapeList_DPF.NewNodeLast();
  auto SData = this->ShapeData_DPF_Get(ShapeID);
  SData->p = Properties->p;
  this->AddShapeToObject(ObjectID, ShapeEnum_t::DPF, ShapeID);
  return ShapeID;
}

ShapeData_DPF_t::p_t GetShape_DPF_WorldPosition(
  ObjectID_t ObjectID,
  ShapeID_t ShapeID
){
  auto OData = this->GetObjectData(ObjectID);
  auto SData = this->ShapeData_DPF_Get(ShapeID);
  ShapeData_DPF_t::p_t r;
  for(uintptr_t d = 0; d < _dc; d++){
    r[d] = OData->Position + SData->p[d];
  }
  return r;
}

void UnlinkRecycleOrphanShape_DPF(
  ShapeID_t ShapeID
){
  this->ShapeList_DPF.unlrec(ShapeID);
}
