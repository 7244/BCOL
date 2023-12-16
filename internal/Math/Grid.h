struct iterate_grid_for_rectangle_t{
  public:
    _vsi32 gs; /* grid start */
  private:
    uint8_t c = 0;
    _vsi32 ge; /* grid end */
    bool NeedInit = true;

    void InitCurrent(const auto &gbs, const auto &wp, f32_t er){
      if(c < _dc){
        _f wpm = wp[c] - er;
        _f wpp = wp[c] + er;
        gs[c] = wpm / gbs[c];
        ge[c] = wpp / gbs[c];
        if(wpm < 0){
          gs[c]--;
        }
        if(wpp < 0){
          ge[c]--;
        }
      }
      if(c + 1 == _dc){
        gs[c]--; /* we increase this even before check ge[c] so this is needed */
      }
    }
    void Increase(const auto &gbs, const auto &wp, f32_t er /* end radius */){
      c++;
      InitCurrent(gbs, wp, er);
    }
    void Decrease(){
      c--;
      if(c < _dc){
        gs[c]++;
        if(gs[c] > ge[c]){
          Decrease();
        }
      }
    }

    bool _it(
      const auto &gbs, /* grid block size */
      const auto &wp, /* world position */
      _vf s /* size */
    ){
      while(1){
        if(c + 1 < _dc){
          Increase(gbs, wp, s[c + 1]);
        }
        else if(c < _dc){
          gs[c]++;
          if(gs[c] <= ge[c]){
            return true;
          }
          Decrease();
        }
        else{
          return false;
        }
      }
    }
  public:
    bool it(const auto &gbs, const auto &wp, _vf &s){
      if(NeedInit){
        NeedInit = false;
        InitCurrent(gbs, wp, s[0]);
      }
      return _it(gbs, wp, s);
    }
};

struct iterate_grid_for_circle_t{
  public:
    _vsi32 gs; /* grid start */
  private:
    uint8_t c = 0;
    _vsi32 ge; /* grid end */
    bool NeedInit = true;

    void InitCurrent(const auto &gbs, const _vf &wp, f32_t er){
      if(c < _dc){
        _f wpm = wp[c] - er;
        _f wpp = wp[c] + er;
        gs[c] = wpm / gbs[c];
        ge[c] = wpp / gbs[c];
        if(wpm < 0){
          gs[c]--;
        }
        if(wpp < 0){
          ge[c]--;
        }
      }
      if(c + 1 == _dc){
        gs[c]--; /* we increase this even before check ge[c] so this is needed */
      }
    }
    void Increase(const auto &gbs, const _vf &wp, f32_t er /* end radius */){
      c++;
      InitCurrent(gbs, wp, er);
    }
    void Decrease(){
      c--;
      if(c < _dc){
        gs[c]++;
        if(gs[c] > ge[c]){
          Decrease();
        }
      }
    }

    /* get biggest of dimension */
    _f gbod(const auto &gbs, const auto &wp, _f r){
      _v<_vf::size() - 1, _f> rp;
      for(uint8_t d = 0; d < c + 1; d++){
        rp[d] = _f(gs[d]) * gbs[d] - wp[d];
      }
      {
        uint8_t d = 0;
        for(; d < c + 1; d++){
          if(rp[d] <= 0 && rp[d] + gbs[d] >= 0); else{
            break;
          }
        }
        if(d == c + 1){
          return r;
        }
      }
      _f ret = 1;
      for(uint8_t d = 0; d < c + 1; d++){
        _f v = min(abs(rp[d]), abs(rp[d] + gbs[d]));
        v = min(v / r, _f(1));
        ret -= v * v;
      }
      if(ret < 0){
        return 0;
      }
      return std::sqrt(ret) * r;
    }
    bool _it(
      const auto &gbs, /* grid block size */
      const auto &wp, /* world position */
      f32_t r /* radius */
    ){
      while(1){
        if(c + 1 < _dc){
          f32_t roff = gbod(gbs, wp, r); /* relative offset */
          Increase(gbs, wp, roff);
        }
        else if(c < _dc){
          gs[c]++;
          if(gs[c] <= ge[c]){
            return true;
          }
          Decrease();
        }
        else{
          return false;
        }
      }
    }
  public:
    bool it(const auto &gbs, const auto &wp, f32_t r){
      if(NeedInit){
        NeedInit = false;
        InitCurrent(gbs, wp, r);
      }
      return _it(gbs, wp, r);
    }
};
