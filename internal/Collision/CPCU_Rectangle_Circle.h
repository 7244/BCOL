struct CPCU_Rectangle_Circle_t{
  _vf dirsign;
  _vf outdir;
};

bool CPCU_Rectangle_Circle_Pre(
  _vf p0,
  _vf p0Size,
  _vf p1,
  _f p1Size,
  CPCU_Rectangle_Circle_t *Data
){
  _vf p0_p1 = p0 - p1;
  Data->dirsign = (p0_p1 * 9999999).clamp(_f(-1), _f(+1));
  Data->outdir = (p0_p1.abs() - p0Size).max(_vf(0));
  return Data->outdir.length() >= p1Size;
}

void CPCU_Rectangle_Circle_Solve(
  _vf p0,
  _vf p0Size,
  _vf p1,
  _f p1Size,
  CPCU_Rectangle_Circle_t *Data,
  _vf *op0,
  _vf *oDirection
){
  *op0 = p0 + (Data->outdir * (p1Size / Data->outdir.length()) - Data->outdir) * Data->dirsign;
  *oDirection = (Data->outdir / p1Size * Data->dirsign).normalize();
}
