bool ray_circle_intersection(
  _vf ray_pos,
  _vf ray_dir,
  _vf cpos,
  _f r,
  _vf &intersection_position
){
  _f a = ray_dir.dot(ray_dir);
  _f b = ray_dir.dot(ray_pos - cpos) * 2;
  _f c = (ray_pos - cpos).dot(ray_pos - cpos) - r * r;
  _f discriminant = b * b - 4 * a * c;

  if(discriminant < 0){
    return false;
  }
  else if(discriminant > 0){
    _f t = (-b + sqrt(discriminant)) / (2 * a);
    _f t2 = -b / a - t;
    if(abs(t2) < abs(t)){
      t = t2;
    }
    if(t < 0 && t2 < 0){
      return false;
    }
    intersection_position = ray_pos + ray_dir * t;
    return true;
  }
  else{
    intersection_position = ray_pos + ray_dir * (-0.5 * b / a);
    return true;
  }
}

template<bool expect>
bool ray_rectangle_intersection(
  _vf ray_pos,
  _vf ray_dir,
  _vf shape_pos,
  _vf shape_size,
  _vf &intersection_position
){
  _vf edge_diff = (shape_pos - ray_pos).abs() - shape_size;
  edge_diff = edge_diff.max(_vf(0));

  _f need = (edge_diff / ray_dir).abs().max();
  if(need == 0){
    /* ray is inside shape */
    return false;
  }
  _vf intersection = ray_pos + ray_dir * need;

  #if 0
    /* TODO need better formula here */
  #elif 1
    /* good method but can be broken in big floating points */
    _vf intersub = intersection - shape_pos;
    intersub = intersub.abs() - shape_size;
    if(intersub.max() > 0.0001){
      return false;
    }
  #endif

  intersection_position = intersection;

  return true;
}

/* TODO */
#if BCOL_set_Dimension == 3
  #if 1
    template<bool expect>
    bool ray_dpf_intersection(
      _vf ray_pos,
      _vf ray_dir,
      _vf shape_pos[_dc],
      _vf &intersection_position
    ){
      const _f EPSILON = 0.0000001;
      _vf edge1 = shape_pos[1] - shape_pos[0];
      _vf edge2 = shape_pos[2] - shape_pos[0];
      _vf rayVecXe2 = ray_dir.cross(edge2);
      _f det = edge1.dot(rayVecXe2);

      if (det > -EPSILON && det < EPSILON){
        return false;
      }

      _f invDet = 1.0 / det;
      _vf s = ray_pos - shape_pos[0];
      _f u = invDet * s.dot(rayVecXe2);

      if (u < 0.0 || u > 1.0){
        return false;
      }

      _vf sXe1 = s.cross(edge1);
      _f v = invDet * ray_dir.dot(sXe1);

      if (v < 0.0 || u + v > 1.0){
        return false;
      }

      float t = invDet * edge2.dot(sXe1);

      if(t > EPSILON){
        intersection_position = ray_pos + ray_dir * t;
        return true;
      }
      else{
        return false;
      }
    }
  #elif 0
    template<bool expect>
    bool ray_dpf_intersection(
      _vf ray_pos,
      _vf ray_dir,
      _vf shape_pos[_dc],
      _vf &intersection_position
    ){
      const _f EPSILON = 0.0000001;
      _vf rayVecXe2 = ray_dir.cross(shape_pos[2]);
      _f det = shape_pos[1].dot(rayVecXe2) + EPSILON;

      _f invDet = 1.0 / det;
      _vf s = ray_pos - shape_pos[0];
      _f u = invDet * s.dot(rayVecXe2);

      if (EXPECT(abs(u - 0.5) > 0.5, true)){
        return false;
      }

      _vf sXe1 = s.cross(shape_pos[1]);
      _f v = invDet * ray_dir.dot(sXe1);

      if (EXPECT(v < 0.0 || u + v > 1.0, true)){
        return false;
      }

      float t = invDet * shape_pos[2].dot(sXe1);

      if(EXPECT(t <= EPSILON, true)){
        return false;
      }

      intersection_position = ray_pos + ray_dir * t;
      return true;
    }
  #endif
#endif
