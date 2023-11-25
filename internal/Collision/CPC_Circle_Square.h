#if 0
void CPC_Circle_Square(
  _vf p0,
  _f p0Size,
  _vf p1,
  _f p1Size,
  _vf *op0,
  _vf *oDirection
){
  _vf Sp0 = p0 - p1;
  _vf SPp0 = Sp0.abs();
  bool AllBigger = true;
  for(uint32_t d = 0; d < _dc; d++){
    if(SPp0[d] <= p1Size){
      AllBigger = false;
      break;
    }
  }
  if(AllBigger == true){
    _vf Corner = SPp0 - p1Size;
    _f Divider = Corner.length();
    _vf CircleOffset = Corner / Divider;
    for(uint32_t d = 0; d < _dc; d++){ /* TODO use vectorel copysign */
      (*op0)[d] = p1[d] + copysign(CircleOffset[d] * p0Size + p1Size, Sp0[d]);
      (*oDirection)[d] = copysign(CircleOffset[d], Sp0[d]);
    }
  }
  else{
    for(uint32_t d = 0; d < _dc; d++){
      (*op0)[d] = p1[d] + copysign(p1Size + p0Size, Sp0[d]);
      (*oDirection)[d] = copysign(_f(1), Sp0[d]);
    }
    for(uint32_t d = 0; d < _dc; d++){
      if(SPp0[d] > p1Size){
        continue;
      }
      (*op0)[d] = p0[d];
      (*oDirection)[d] = 0;
      break;
    }
  }
}
#endif

void CPC_Circle_Square(
  _vf p0,
  _f p0Size,
  _vf p1,
  _f p1Size,
  _vf *op0,
  _vf *oDirection
){
  _vf p0_p1 = p1 - p0;
  _vf dirsign = (p0_p1 * 9999999).clamp(_f(-1), _f(+1));
  *op0 = p0; /* TODO */
  *oDirection = (p0_p1.abs() - p1Size).max(_vf(0));
  *oDirection /= p0Size;
  *oDirection *= -dirsign;
  *oDirection = (*oDirection).normalize();
}
