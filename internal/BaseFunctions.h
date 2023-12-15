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
      #if BCOL_set_Dimension != 3
        #error TODO
      #endif

      rtcDetachGeometry(embree.scene, embree.geoid);
      embree.geom = rtcNewGeometry(embree.device, RTC_GEOMETRY_TYPE_TRIANGLE);

      auto *vb = (ShapeData_DPF_t::p_t *)rtcSetNewGeometryBuffer(
        embree.geom,
        RTC_BUFFER_TYPE_VERTEX,
        0,
        RTC_FORMAT_FLOAT3,
        3 * sizeof(_f),
        ShapeList_DPF.Usage() * 3);

      {
        uintptr_t ci = 0;
        auto nr = ShapeList_DPF.GetNodeFirst();
        while(nr != ShapeList_DPF.dst){
          auto n = ShapeList_DPF.GetNodeByReference(nr);
          vb[ci] = n->data.p;
          nr = n->NextNodeReference;
          ci++;
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
