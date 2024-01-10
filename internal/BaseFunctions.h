struct OpenProperties_t{
  #if BCOL_set_SupportGrid == 1
    _f GridBlockSize;
  #endif
};

void Open(const OpenProperties_t &p){
  ObjectList.Open();
  ShapeList_Circle.Open();
  ShapeList_Rectangle.Open();
  ShapeList_DPF.Open();

  #if BCOL_set_SupportGrid == 1
    GridBlockSize = p.GridBlockSize;
  #endif

  #if BCOL_set_StepNumber == 1
    StepNumber = 0;
  #endif

  #if BCOL_set_UseEmbree == 1
    embree.device = rtcNewDevice(NULL);
    embree.scene = rtcNewScene(embree.device);
    embree.geom = rtcNewGeometry(embree.device, RTC_GEOMETRY_TYPE_TRIANGLE);
    rtcCommitGeometry(embree.geom);
    embree.geoid = rtcAttachGeometry(embree.scene, embree.geom);
    rtcReleaseGeometry(embree.geom);
    rtcCommitScene(embree.scene);

    embree.sipList = NULL;
  #endif
}

void Close(){
  #if BCOL_set_UseEmbree == 1
    rtcReleaseScene(embree.scene);
    rtcReleaseDevice(embree.device);
  #endif

  ObjectList.Close();
  ShapeList_Circle.Close();
  ShapeList_Rectangle.Close();
  ShapeList_DPF.Close();
}

#if BCOL_set_StepNumber == 1
  uint64_t GetStepNumber(){
    return StepNumber;
  }
#endif

#if BCOL_set_VisualSolve == 1
  void BakeCurrentForVisualSolve(){
    #if BCOL_set_UseEmbree == 1
      #if BCOL_set_PreferredFloatSize != 32
        #error TODO make embree use same float size as PreferredFloatSize
      #endif

      A_resize(embree.sipList, NULL);
      rtcDetachGeometry(embree.scene, embree.geoid);
      embree.geom = rtcNewGeometry(embree.device, RTC_GEOMETRY_TYPE_TRIANGLE);

      embree.sipList = (ShapeInfoPack_t *)A_resize(NULL, ShapeList_DPF.Usage() * sizeof(ShapeInfoPack_t));

      auto *vb = (ShapeData_DPF_t::p_t *)rtcSetNewGeometryBuffer(
        embree.geom,
        RTC_BUFFER_TYPE_VERTEX,
        0,
        RTC_FORMAT_FLOAT3,
        3 * sizeof(_f),
        ShapeList_DPF.Usage() * 3);

      uint32_t DPFIndex = 0;

      TraverseObjects_t to;
      TraverseObjects_init(&to);
      while(TraverseObjects_loop(&to)){
        ShapeInfoPack_t sip;
        sip.ObjectID = to.ObjectID;
        auto ObjectData = this->GetObjectData(sip.ObjectID);
        for(sip.ShapeID.ID = 0; sip.ShapeID.ID < ObjectData->ShapeList.Current; sip.ShapeID.ID++){
          sip.ShapeEnum = ObjectData->ShapeList.ptr[sip.ShapeID.ID].ShapeEnum;
          switch(sip.ShapeEnum){
            case ShapeEnum_t::Circle:{
              // TODO
              break;
            }
            case ShapeEnum_t::Rectangle:{
              // TODO
              break;
            }
            case ShapeEnum_t::DPF:{
              auto sd = ShapeData_DPF_Get(ObjectData->ShapeList.ptr[sip.ShapeID.ID].ShapeID);
              embree.sipList[DPFIndex] = sip;
              vb[DPFIndex] = sd->p;
              DPFIndex++;
              break;
            }
          }
        }
      }

      uint32_t *ib = (uint32_t *)rtcSetNewGeometryBuffer(
        embree.geom,
        RTC_BUFFER_TYPE_INDEX,
        0,
        RTC_FORMAT_UINT3,
        3 * sizeof(uint32_t),
        ShapeList_DPF.Usage());

      for(uint32_t i = 0; i < ShapeList_DPF.Usage(); i++){
        ib[i * 3 + 0] = i * 3 + 0;
        ib[i * 3 + 1] = i * 3 + 1;
        ib[i * 3 + 2] = i * 3 + 2;
      }

      rtcCommitGeometry(embree.geom);
      rtcAttachGeometry(embree.scene, embree.geom);
      rtcReleaseGeometry(embree.geom);
      rtcCommitScene(embree.scene);
    #endif
  }
#endif
