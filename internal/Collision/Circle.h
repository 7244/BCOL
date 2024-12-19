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
  _vf outdir = ((p0 - point).abs() * side * dirsign).normalize();

  *op0 = point + outdir * p0Size + diff * (_vf(1) - side);
  *oDirection = outdir;
}

struct CPCU_Circle_Rectangle_t{
  _vf dirsign;
  _vf outdir;
};

bool CPCU_Circle_Rectangle_Pre(
  _vf p0,
  _f p0Size,
  _vf p1,
  _vf p1Size,
  CPCU_Circle_Rectangle_t *Data
){
  _vf p0_p1 = p0 - p1;
  Data->dirsign = (p0_p1 * 9999999).clamp(_f(-1), _f(+1));
  Data->outdir = (p0_p1.abs() - p1Size).max(_vf(0));
  return Data->outdir.length() >= p0Size;
}

void CPCU_Circle_Rectangle_Solve(
  _vf p0,
  _f p0Size,
  _vf p1,
  _vf p1Size,
  CPCU_Circle_Rectangle_t *Data,
  _vf *op0,
  _vf *oDirection
){
  *op0 = p0 + (Data->outdir * (p0Size / Data->outdir.length()) - Data->outdir) * Data->dirsign;
  *oDirection = (Data->outdir / p0Size * Data->dirsign).normalize();
}
