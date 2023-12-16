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
  normal = normal.normalize(); /* incase corner intersection */
  return normal;
}

static _vf CalculateNormalFromCircleIntersection(_vf Position, _vf Intersection){
  return (Intersection - Position).normalize();
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
