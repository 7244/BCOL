#ifdef BCOL_set_HaveDefaultCB
  #error outdated setting BCOL_set_HaveDefaultCB. it's always enabled now.
#endif

#ifndef BCOL_set_prefix
  #error ifndef BCOL_set_prefix
#endif
#ifndef BCOL_set_Dimension
  #error BCOL_set_Dimension needs to be defined.
#endif
#ifndef BCOL_set_PreferredFloatSize
  #define BCOL_set_PreferredFloatSize 32
#endif
#ifndef BCOL_set_StoreExtraDataInsideObject
  #define BCOL_set_StoreExtraDataInsideObject 0
#endif
#ifndef BCOL_set_SupportGrid
  #define BCOL_set_SupportGrid 0
#endif
#ifndef BCOL_set_DynamicDeltaFunction
  #define BCOL_set_DynamicDeltaFunction
#endif
#ifndef BCOL_set_DynamicToDynamic
  #define BCOL_set_DynamicToDynamic 1
#endif
#ifndef BCOL_set_StepNumber
  #define BCOL_set_StepNumber 0
#endif
#ifndef BCOL_set_VisualSolve
  #define BCOL_set_VisualSolve 0
#endif
#ifndef BCOL_set_VisualSolve_dmin
  #define BCOL_set_VisualSolve_dmin 0.1
#endif
#ifndef BCOL_set_VisualSolve_dmax
  #define BCOL_set_VisualSolve_dmax 99999999
#endif
#ifndef BCOL_set_VisualSolve_CalculateBarycentric
  #define BCOL_set_VisualSolve_CalculateBarycentric 0
#endif
#ifndef BCOL_set_DefaultVelocity0
  #define BCOL_set_DefaultVelocity0 1
#endif
#ifndef BCOL_set_UseEmbree
  #define BCOL_set_UseEmbree 0
#endif

#if BCOL_set_UseEmbree == 1
  #if BCOL_set_Dimension != 3
    #error embree only works with dimension 3
  #endif
#endif

#if BCOL_set_VisualSolve != 0
  #if BCOL_set_SupportGrid != 0
    #ifndef BCOL_set_VisualSolve_GridContact
      #error define BCOL_set_VisualSolve_GridContact
    #endif
  #endif
#endif

#if BCOL_set_StoreExtraDataInsideObject == 0
#elif BCOL_set_StoreExtraDataInsideObject == 1
  #ifndef BCOL_set_ExtraDataInsideObject
    #error ?
  #endif
#else
  #error ?
#endif

#if BCOL_set_SupportGrid == 0
#elif BCOL_set_SupportGrid == 1
#else
  #error ?
#endif

#include "internal/rest.h"

#ifndef BCOL_set_PostSolve_Grid_CollisionNormal
  #undef BCOL_set_PostSolve_Grid_CollisionNormal
#endif
#ifndef BCOL_set_PostSolve_Grid
  #undef BCOL_set_PostSolve_Grid
#endif
#ifndef BCOL_set_ConstantBumpFriction
  #undef BCOL_set_ConstantBumpFriction
#endif
#ifndef BCOL_set_ConstantFriction
  #undef BCOL_set_ConstantFriction
#endif

#ifdef BCOL_set_DPFStore
  #undef BCOL_set_DPFStore
#endif
#undef BCOL_set_UseEmbree
#undef BCOL_set_DefaultVelocity0
#ifdef BCOL_set_VisualSolve_GridContact
  #undef BCOL_set_VisualSolve_GridContact
#endif
#undef BCOL_set_VisualSolve_CalculateBarycentric
#undef BCOL_set_VisualSolve_dmax
#undef BCOL_set_VisualSolve_dmin
#undef BCOL_set_VisualSolve
#undef BCOL_set_StepNumber
#undef BCOL_set_DynamicToDynamic
#undef BCOL_set_DynamicDeltaFunction
#undef BCOL_set_SupportGrid
#if BCOL_set_StoreExtraDataInsideObject == 0
#elif BCOL_set_StoreExtraDataInsideObject == 1
  #undef BCOL_set_ExtraDataInsideObject
#endif
#undef BCOL_set_StoreExtraDataInsideObject
#undef BCOL_set_PreferredFloatSize
#undef BCOL_set_Dimension
#undef BCOL_set_prefix
