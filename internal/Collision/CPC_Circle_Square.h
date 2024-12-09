void CPC_Circle_Square(
  _vf p0,
  _f p0Size,
  _vf p1,
  _f p1Size,
  _vf *op0,
  _vf *oDirection
){
  _vf diff = p0 - p1;
  _vf side = ((diff.abs() - diff.abs().max() + 0.00001) * 9999999).clamp(_f(+0), _f(+1));
  _vf dirsign = (diff * 9999999).clamp(_f(-1), _f(+1));
  _vf outdir = (diff.abs() - p1Size).max(_vf(0));
  *op0 = p1 + _vf(p1Size) * side * dirsign + _vf(p0Size) * side * dirsign + diff * (_vf(1) - side);
  *oDirection = (outdir / p0Size * dirsign).normalize();
}
