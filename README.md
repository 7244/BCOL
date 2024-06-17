# Buildable COllision Library
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

BCOL is physics/collision library. Mainly made for fast realtime tick based collisions.

## Features
- Low SLOC and memory usage. Currently 2301 SLOC.
- No hardcoded dimension. It can run any dimension based on `BCOL_set_Dimension` define. (only 2 and 3 is tested)
- grid, rectangle, circle collisions.
- Raycast (visual solve)
- Raycast can use embree or slow native implement. (embree is only supported if `BCOL_set_Dimension` is 3)

## This library only receives improvements for specific needs.

### Depends On
* https://github.com/6413/fan (or fan style vector library)
* https://github.com/7244/BVEC
* https://github.com/7244/BLL

#### Demo that uses BCOL collisions and raycast capability for render same time
[![a]()](https://github.com/7244/BCOL/assets/123005911/3f4df4d0-dd90-463f-8cfa-45539f6584f3)

### Developers
* [github.com/7244](https://github.com/7244) - main design, algorithms, suffering
* [github.com/6413](https://github.com/6413) - fan, math formulas

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
