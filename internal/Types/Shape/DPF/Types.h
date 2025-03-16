struct ShapeData_DPF_t{
  struct p_t{
    _vf arr[_dc];
    _vf &operator[](uintptr_t x){
      return arr[x];
    }
  }p;
  #ifdef BCOL_set_DPFStore
    struct u_t{
      BCOL_set_DPFStore
    }u;
  #endif
};

/* this is for BakeCurrentForVisualSolve()'s UseEmbree=1 */
static_assert(sizeof(ShapeData_DPF_t::p_t) == sizeof(_vf) * _dc);

#define BLL_set_prefix ShapeList_DPF
#define BLL_set_Usage 1
#define BLL_set_Language 1
#define BLL_set_AreWeInsideStruct 1
#define BLL_set_NodeReference_Overload_Declare \
  ShapeList_DPF_NodeReference_t(ShapeID_t ShapeID){ \
    this->NRI = ShapeID.ID; \
  }
#define BLL_set_type_node uint32_t
#define BLL_set_NodeDataType ShapeData_DPF_t
#include <BLL/BLL.h>
