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
      {
        RTCRayHit rayhit;
        rayhit.ray.org_x = position.x;
        rayhit.ray.org_y = position.y;
        rayhit.ray.org_z = position.z;
        rayhit.ray.dir_x = direction.x;
        rayhit.ray.dir_y = direction.y;
        rayhit.ray.dir_z = direction.z;
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
            closest_shape.sip.ObjectID.NRI = 0;
            closest_shape.sip.ShapeEnum = ShapeEnum_t::DPF;
            closest_shape.sip.ShapeID = ShapeList_Rectangle_NodeReference_t{0};
            closest_shape.intersection_pos = position + direction * rayhit.ray.tfar;
          }
        }
      }
    #else
      _Ray_SolveShape(position, direction, closest_shape);
    #endif

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
      VisualSolve_Shape_cb(this, &closest_shape.sip, position, closest_shape.intersection_pos, &VisualSolve);
    }

    return VisualSolve;
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
      {
        RTCRayHit16 rayhit;
        int valid[16];
        for(uint8_t i = 0; i < 16; i++){
          rayhit.ray.org_x[i] = position[i].x;
          rayhit.ray.org_y[i] = position[i].y;
          rayhit.ray.org_z[i] = position[i].z;
          rayhit.ray.dir_x[i] = direction[i].x;
          rayhit.ray.dir_y[i] = direction[i].y;
          rayhit.ray.dir_z[i] = direction[i].z;
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
      }
    #else
      for(uint8_t i = 0; i < 16; i++){
        _Ray_SolveShape(position[i], direction[i], closest_shape[i]);
      }
    #endif

    for(uint8_t i = 0; i < 16; i++){
      #if BCOL_set_SupportGrid == 1
        if((grid_result[i].at * GridBlockSize - position[i]).length() < (closest_shape[i].intersection_pos - position[i]).length()){
          this->VisualSolve_Grid_cb(
            this,
            grid_result[i].gi,
            grid_result[i].np,
            grid_result[i].at,
            &VisualSolve[i]);
        }
        else
      #endif
      {
        if(closest_shape[i].sip.ObjectID.iic()){
          VisualSolve[i] = VisualSolve_t(0);
        }
        else{
          VisualSolve_Shape_cb(this, &closest_shape[i].sip, position[i], closest_shape[i].intersection_pos, &VisualSolve[i]);
        }
      }
    }
  }
#endif
