#ifndef CLEAN_PC_CGAME_MP_CG_LOCAL_H
#define CLEAN_PC_CGAME_MP_CG_LOCAL_H

#include "common_types.h"

extern ALIGNED(128) cg_t cgArray[1];
extern cg_t * const cg;
extern ALIGNED(128) cgs_t cgsArray[1];
extern cgs_t * const cgs;
extern ALIGNED(128) centity_t cg_entitiesArray[1][1024];
extern centity_t * const cg_entities;

#endif
