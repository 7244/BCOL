void CPD_Circle_Square(
  _vf p0,
  _f p0Size,
  _vf p1,
  _f p1Size,
  _vf *op0,
  _vf *oDirection
){
  _vf diff = p0 - p1;
  _vf dirsign = (diff * 9999999).clamp(_f(-1), _f(+1));

  _vf side_outside = (diff.abs() - p1Size).ceil().clamp(_f(+0), _f(+1));
  _vf side_inside = (diff.abs() - diff.abs().max() + 1).max(_vf(0)).floor();

  _vf side = (side_outside + side_inside).min(_vf(1));

  _vf point = p1 + side * dirsign * p1Size;
  _vf outdir = ((p0 - point).abs() * side * dirsign).normalized();

  *op0 = point + outdir * p0Size + diff * (_vf(1) - side);
  *oDirection = outdir;
}

struct CPCU_Circle_Rectangle_t{
  _vf point;
  _vf outdiff;
};

bool CPCU_Circle_Rectangle_Pre(
  _vf p0,
  _f p0Size,
  _vf p1,
  _vf p1Size,
  _rotf p1rot,
  CPCU_Circle_Rectangle_t *Data
){
  auto np0 = math_RotatePosition(p0, p1, p1rot);

  auto diff = np0 - p1;
  Data->point = diff.clamp(-p1Size, +p1Size);
  Data->outdiff = diff - Data->point;

  return Data->outdiff.length() > p0Size;
}

void CPCU_Circle_Rectangle_Solve(
  _vf p0,
  _f p0Size,
  _vf p1,
  _vf p1Size,
  _rotf p1rot,
  CPCU_Circle_Rectangle_t *Data,
  _vf *op0,
  _vf *oDirection
){
  _f pdiv = (Data->point.abs() / p1Size).max();
  _vf outdir;
  if(pdiv != 1 || Data->outdiff.length() == 0){
    /* TODO not good implement */

    if(pdiv != 0){
      Data->point /= pdiv;
      outdir = ((Data->point.abs() - Data->point.abs().max()).floor() + 1).max(_vf(0)) * (Data->point / Data->point.abs().max());
    }
    else{
      /* TODO find clearest place to move it */
      Data->point = p1Size;
      outdir = _vf(1).normalized();
    }
  }
  else{
    outdir = Data->outdiff.normalized();
  }

  *op0 = Data->point + outdir * p0Size;
  *op0 = math_RotatePosition(*op0, 0, -p1rot);
  *op0 += p1;
  *oDirection = outdir;
  *oDirection = math_RotatePosition(*oDirection, 0, -p1rot);
}
