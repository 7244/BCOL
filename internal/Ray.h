#if BCOL_set_VisualSolve == 1
  #if BCOL_set_SupportGrid == 1
    struct _Ray_grid_result_t{
      _vf np; /* normalized position */
      _vf at;
      _vsi32 gi;
    };

    void _Ray_SolveGrid(
      _vf position,
      _vf direction,
      _Ray_grid_result_t &grid_result
    ){
      grid_result.np = position / GridBlockSize;
      grid_result.at = grid_result.np;
      for(uint32_t d = 0; d < _dc; d++){
        grid_result.gi[d] = grid_result.at[d] + (grid_result.at[d] < _f(0) ? _f(-1) : _f(0));
      }

      _vf r = grid_result.at - grid_result.gi;
      while(1){
        {
          bool Contact;
          BCOL_set_VisualSolve_GridContact
          if((position - grid_result.at * GridBlockSize).length() < BCOL_set_VisualSolve_dmin){
            Contact = false;
          }
          if(Contact == true){
            break;
          }
        }

        _vf left;
        #if 0
          for(uint32_t i = 0; i < _dc; i++){
            if(direction[i] > 0){
              left[i] = f32_t(1) - r[i];
            }
            else{
              left[i] = r[i];
            }
          }
          _vf multiplers = left / direction.abs();
        #elif 1
          left = ((direction * 9999999).clamp(_f(0), _f(1)) - r).abs();
          _vf multiplers = left / direction.abs();
        #elif 0
          left = (direction * 9999999).clamp(_f(-0.0000001), _f(1)) - r;
          _vf multiplers = left / direction;
        #endif

        f32_t min_multipler = multiplers.min();
        for(uint32_t i = 0; i < _dc; i++){
          if(multiplers[i] == min_multipler){
            grid_result.gi[i] += copysign((sint32_t)1, direction[i]);
            r[i] -= copysign((f32_t)1, direction[i]);
          }
        }
        _vf min_dir = direction * min_multipler;
        grid_result.at += min_dir;
        r += min_dir;
      }
    }
  #endif

  struct _Ray_closest_shape_t{
    ShapeInfoPack_t sip;
    _vf intersection_pos;
  };

  void _Ray_SolveShape(
    _vf position,
    _vf direction,
    _Ray_closest_shape_t &closest_shape
  ){
    ShapeInfoPack_t sip;
    sip.ObjectID = this->ObjectList.GetNodeFirst();
    while(sip.ObjectID != this->ObjectList.dst){
      auto ObjectData = this->GetObjectData(sip.ObjectID);

      for(sip.ShapeID.ID = 0; sip.ShapeID.ID < ObjectData->ShapeList.Current; sip.ShapeID.ID++){
        sip.ShapeEnum = ObjectData->ShapeList.ptr[sip.ShapeID.ID].ShapeEnum;
        switch(sip.ShapeEnum){
          case ShapeEnum_t::Circle:{
            auto CircleData = ShapeData_Circle_Get(ObjectData->ShapeList.ptr[sip.ShapeID.ID].ShapeID);
            auto circle_pos = ObjectData->Position + CircleData->Position;
            _vf intersection_pos;
            if(ray_circle_intersection(position, direction, circle_pos, CircleData->Size, intersection_pos) == false){
              break;
            }
            if((intersection_pos - position).length() < BCOL_set_VisualSolve_dmin){
              break;
            }
            if((intersection_pos - position).length() < (closest_shape.intersection_pos - position).length()){
              closest_shape.sip = sip;
              closest_shape.intersection_pos = intersection_pos;
            }
            break;
          }
          case ShapeEnum_t::Rectangle:{
            auto sd = ShapeData_Rectangle_Get(ObjectData->ShapeList.ptr[sip.ShapeID.ID].ShapeID);
            auto sp = ObjectData->Position + sd->Position;

            _vf intersection_pos;
            if(ray_rectangle_intersection<false>(position, direction, sp, sd->Size, intersection_pos) == false){
              break;
            }
            if((intersection_pos - position).length() < BCOL_set_VisualSolve_dmin){
              break;
            }
            if((intersection_pos - position).length() < (closest_shape.intersection_pos - position).length()){
              closest_shape.sip = sip;
              closest_shape.intersection_pos = intersection_pos;
            }
            break;
          }
          /* TODO */
          #if BCOL_set_Dimension == 3
          case ShapeEnum_t::DPF:{
            auto sd = ShapeData_DPF_Get(ObjectData->ShapeList.ptr[sip.ShapeID.ID].ShapeID);
            _vf sp[_dc];
            for(uintptr_t d = 0; d < _dc; d++){
              sp[d] = ObjectData->Position + sd->p[d];
            }

            _vf intersection_pos;
            if(ray_dpf_intersection<false>(position, direction, sp, intersection_pos) == false){
              break;
            }
            if((intersection_pos - position).length() < BCOL_set_VisualSolve_dmin){
              break;
            }
            if((intersection_pos - position).length() < (closest_shape.intersection_pos - position).length()){
              closest_shape.sip = sip;
              closest_shape.intersection_pos = intersection_pos;
            }
            break;
          }
          #endif
        }
      }

      sip.ObjectID = sip.ObjectID.Next(&this->ObjectList);
    }
  }

  VisualSolve_t _Ray_GetVisualSolve(
    #if set_bcol_UseEmbree == 1
      _vf Normal,
      _v<_dc - 1, _f> Barycentric,
    #endif
    _vf position,
    _vf direction,
    _Ray_closest_shape_t &closest_shape
    #if BCOL_set_SupportGrid == 1
      , _Ray_grid_result_t &grid_result
    #endif
  ){
    VisualSolve_t VisualSolve;

    #if BCOL_set_SupportGrid == 1
      if((grid_result.at * GridBlockSize - position).length() < (closest_shape.intersection_pos - position).length()){
        this->VisualSolve_Grid_cb(
          this,
          grid_result.gi,
          grid_result.np,
          grid_result.at,
          &VisualSolve);
      }
      else
    #endif
    {
      if(closest_shape.sip.ObjectID.iic()){
        return VisualSolve_t(0);
      }

      #if set_bcol_UseEmbree == 1
        /* Normal and Barycentric is in parameter */
        {
          Normal = Normal.normalize();
          /* TODO this normal always points same direction in triangle */
        }
      #else
        _vf Normal;
        _v<_dc - 1, _f> Barycentric;
        auto ObjectData = this->GetObjectData(closest_shape.sip.ObjectID);
        switch(closest_shape.sip.ShapeEnum){
          case ShapeEnum_t::Circle:{
            auto sd = ShapeData_Circle_Get(ObjectData->ShapeList.ptr[closest_shape.sip.ShapeID.ID].ShapeID);
            auto sp = ObjectData->Position + sd->Position;
            Normal = CalculateNormalFromCircleIntersection(sp, closest_shape.intersection_pos);
            Barycentric = _v<_dc - 1, _f>(0); /* TODO */
            break;
          }
          case ShapeEnum_t::Rectangle:{
            auto sd = ShapeData_Rectangle_Get(ObjectData->ShapeList.ptr[closest_shape.sip.ShapeID.ID].ShapeID);
            auto sp = ObjectData->Position + sd->Position;
            Normal = CalculateNormalFromRectangleIntersection(sp, sd->Size, closest_shape.intersection_pos);
            Barycentric = CalculateBarycentricFromRectangleIntersection(sp, sd->Size, closest_shape.intersection_pos);
            break;
          }
          case ShapeEnum_t::DPF:{
            auto sd = ShapeData_DPF_Get(ObjectData->ShapeList.ptr[closest_shape.sip.ShapeID.ID].ShapeID);
            Normal = _vf(0); /* TODO */
            Barycentric = _v<_dc - 1, _f>(0); /* TODO */
            break;
          }
        }
      #endif
      VisualSolve_Shape_cb(
        this,
        &closest_shape.sip,
        position,
        closest_shape.intersection_pos,
        Normal,
        #if BCOL_set_VisualSolve_CalculateBarycentric == 1
          Barycentric,
        #endif
        &VisualSolve
      );
    }

    return VisualSolve;
  }

  /* this function is thread safe */
  VisualSolve_t Ray(
    _vf position,
    _vf direction
  ){
    for(uint32_t i = 0; i < _dc; i++){
      if(direction[i] == 0){
        direction[i] = 0.00001;
      }
    }

    #if BCOL_set_SupportGrid == 1
      _Ray_grid_result_t grid_result;
      _Ray_SolveGrid(position, direction, grid_result);
    #endif

    _Ray_closest_shape_t closest_shape;
    closest_shape.sip.ObjectID.sic();
    closest_shape.intersection_pos = position + 999999999;

    #if BCOL_set_UseEmbree == 1
      RTCRayHit rayhit;
      rayhit.ray.org_x = position[0];
      rayhit.ray.org_y = position[1];
      rayhit.ray.org_z = position[2];
      rayhit.ray.dir_x = direction[0];
      rayhit.ray.dir_y = direction[1];
      rayhit.ray.dir_z = direction[2];
      rayhit.ray.tnear = BCOL_set_VisualSolve_dmin;
      rayhit.ray.tfar = BCOL_set_VisualSolve_dmax;
      rayhit.ray.time = 0;
      rayhit.ray.mask = -1;
      rayhit.ray.id = 0;
      rayhit.ray.flags = 0;
      rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;

      rtcIntersect1(embree.scene, &rayhit);

      if(rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID){
        if(rayhit.ray.tfar < (closest_shape.intersection_pos - position).length()){
          closest_shape.sip = embree.sipList[rayhit.hit.primID];
          closest_shape.intersection_pos = position + direction * rayhit.ray.tfar;
        }
      }
    #else
      _Ray_SolveShape(position, direction, closest_shape);
    #endif

    return _Ray_GetVisualSolve(
      #if BCOL_set_UseEmbree == 1
        _vf(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z),
        _v<_dc - 1, _f>(rayhit.hit.u, rayhit.hit.v),
      #endif
      position,
      direction,
      closest_shape
      #if BCOL_set_SupportGrid == 1
        , grid_result
      #endif
    );
  }

  /* this does not give better performance, why? */
  void Ray16(
    _vf *position,
    _vf *direction,
    VisualSolve_t *VisualSolve
  ){
    for(uint8_t i = 0; i < 16; i++){
      for(uint32_t d = 0; d < _dc; d++){
        if(direction[i][d] == 0){
          direction[i][d] = 0.00001;
        }
      }
    }

    #if BCOL_set_SupportGrid == 1
      _Ray_grid_result_t grid_result[16];
      for(uint8_t i = 0; i < 16; i++){
        _Ray_SolveGrid(position[i], direction[i], grid_result[i]);
      }
    #endif

    _Ray_closest_shape_t closest_shape[16];
    for(uint8_t i = 0; i < 16; i++){
      closest_shape[i].sip.ObjectID.sic();
      closest_shape[i].intersection_pos = position[i] + 999999999;
    }

    #if BCOL_set_UseEmbree == 1
      RTCRayHit16 rayhit;
      int valid[16];
      for(uint8_t i = 0; i < 16; i++){
        rayhit.ray.org_x[i] = position[i][0];
        rayhit.ray.org_y[i] = position[i][1];
        rayhit.ray.org_z[i] = position[i][2];
        rayhit.ray.dir_x[i] = direction[i][0];
        rayhit.ray.dir_y[i] = direction[i][1];
        rayhit.ray.dir_z[i] = direction[i][2];
        rayhit.ray.tnear[i] = BCOL_set_VisualSolve_dmin;
        rayhit.ray.tfar[i] = BCOL_set_VisualSolve_dmax;
        rayhit.ray.time[i] = 0;
        rayhit.ray.mask[i] = -1;
        rayhit.ray.id[i] = 0;
        rayhit.ray.flags[i] = 0;
        rayhit.hit.geomID[i] = RTC_INVALID_GEOMETRY_ID;
        valid[i] = -1;
      }

      rtcIntersect16(valid, embree.scene, &rayhit);

      for(uint8_t i = 0; i < 16; i++){
        if(rayhit.hit.geomID[i] == RTC_INVALID_GEOMETRY_ID){
          continue;
        }
        if(rayhit.ray.tfar[i] > (closest_shape[i].intersection_pos - position[i]).length()){
          continue;
        }
        closest_shape[i].sip.ObjectID.NRI = 0;
        closest_shape[i].sip.ShapeEnum = ShapeEnum_t::DPF;
        closest_shape[i].sip.ShapeID = ShapeList_Rectangle_NodeReference_t{0};
        closest_shape[i].intersection_pos = position[i] + direction[i] * rayhit.ray.tfar[i];
      }
    #else
      for(uint8_t i = 0; i < 16; i++){
        _Ray_SolveShape(position[i], direction[i], closest_shape[i]);
      }
    #endif

    for(uint8_t i = 0; i < 16; i++){
      VisualSolve[i] = _Ray_GetVisualSolve(
        #if BCOL_set_UseEmbree == 1
          _vf(rayhit.hit.Ng_x[i], rayhit.hit.Ng_y[i], rayhit.hit.Ng_z[i]),
          _v<_dc - 1, _f>(rayhit.hit.u[i], rayhit.hit.v[i]),
        #endif
        position[i],
        direction[i],
        closest_shape[i]
        #if BCOL_set_SupportGrid == 1
          , grid_result[i]
        #endif
      );
    }
  }
#endif
