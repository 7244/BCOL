static auto min(auto p0, auto p1){
  return p0 < p1 ? p0 : p1;
}
static auto max(auto p0, auto p1){
  return p0 > p1 ? p0 : p1;
}
static auto abs(auto p0){
  return p0 < 0 ? -p0 : p0;
}
static _f clamp(_f v, _f mi, _f ma){
  return v < mi ? mi : v > ma ? ma : v;
}

static _vf math_MultiplyVectorByMatrix(_vf vector, _vf matrix[_dc]){
  _vf res = 0;

  for(uintptr_t d0 = 0; d0 < _dc; d0++){
    for(uintptr_t d1 = 0; d1 < _dc; d1++){
      res[d0] += matrix[d0][d1] * vector[d1];
    }
  }

  return res;
}

static void math_SetMatrixRotationSingle(
  _vf matrix[_dc],
  uintptr_t p0,
  uintptr_t p1,
  _f angle
){
  for(auto dc0 = _dc; dc0--;){
    for(auto dc1 = _dc; dc1--;){
      matrix[dc0][dc1] = dc0 == dc1;
    }
  }

  matrix[p0][p0] = cos(angle);
  matrix[p0][p1] = -sin(angle);
  matrix[p1][p0] = sin(angle);
  matrix[p1][p1] = cos(angle);
}

static _vf math_RotatePosition(_vf Position, _vf Around, _rotf Rotation){
  _vf rotpairmat[_rotc][_dc];

  {
    uintptr_t rotc = 0;
    for(uintptr_t dc0 = 0; dc0 < _dc; dc0++){
      for(uintptr_t dc1 = dc0 + 1; dc1 < _dc; dc1++){
        math_SetMatrixRotationSingle(rotpairmat[rotc], dc0, dc1, Rotation[rotc]);
        rotc++;
      }
    }
  }

  _vf result = Position - Around;
  for(uintptr_t rotc = 0; rotc < _rotc; rotc++){
    result = math_MultiplyVectorByMatrix(result, rotpairmat[rotc]);
  }

  return result + Around;
}

#include "Grid.h"
#include "Ray.h"

static _vf CalculateNormalFromRectangleIntersection(_vf Position, _vf Size, _vf Intersection){
  _vf normal = _vf(0);
  _vf diff = Position - Intersection;
  _vf diffd = diff.abs() / Size;
  _f ma = diffd.max();
  for(uint32_t d = 0; d < 3; d++){
    if(ma == abs(diffd[d])){
      normal[d] = copysign(_f(1), -diff[d]);
    }
  }
  normal = normal.normalized(); /* incase corner intersection */
  return normal;
}

static _vf CalculateNormalFromCircleIntersection(_vf Position, _vf Intersection){
  return (Intersection - Position).normalized();
}

static _v<_dc - 1, _f> CalculateBarycentricFromRectangleIntersection(_vf Position, _vf Size, _vf Intersection){
  _v<_dc - 1, _f> r;

  _vf diff = Position - Intersection;
  diff /= Size;

  uintptr_t biggest_d;
  _f biggest_f = -1;
  for(uintptr_t d = 0; d < _dc; d++){
    if(fabs(diff[d]) > biggest_f){
      biggest_d = d;
      biggest_f = fabs(diff[d]);
    }
  }
  uintptr_t r_index = 0;
  for(uintptr_t d = 0; d < _dc; d++){
    if(d == biggest_d){
      continue;
    }
    r[r_index] = diff[d];
    r[r_index] += 0.5;
    r[r_index] = clamp(r[r_index], 0, 1);
    r_index++;
  }

  return r;
}
