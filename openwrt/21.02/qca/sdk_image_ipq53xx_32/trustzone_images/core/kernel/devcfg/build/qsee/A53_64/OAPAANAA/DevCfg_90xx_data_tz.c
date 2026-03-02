#include "devcfgTypes.h" 

extern const DALSYSPropStructTblType DALPROP_StructPtrs_90xx_tz[];

extern const uint32 DALPROP_PropBin_90xx_tz[];

extern const StringDevice driver_list_90xx_tz[];


const DALProps DALPROP_PropsInfo_90xx_tz = {(const byte*)DALPROP_PropBin_90xx_tz, DALPROP_StructPtrs_90xx_tz, 14, driver_list_90xx_tz};
