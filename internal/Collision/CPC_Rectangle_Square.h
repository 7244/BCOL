void CPC_Rectangle_Square(
  _vf p0,
  _vf p0Size,
  _vf p1,
  _f p1Size,
  _vf *op0,
  _vf *oDirection
){
  _vf p0_p1 = p0 - p1;
  _vf dirsign = (p0_p1 * 9999999).clamp(_f(-1), _f(+1));
  _vf outdir = (p0_p1.abs() - p0Size).max(_vf(0));
  *op0 = p0 + (outdir * (p1Size / outdir.max()) - outdir) * dirsign;
  *oDirection = ((outdir / outdir.max()).floor() * dirsign).normalized();
}
