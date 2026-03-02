#include "devcfgTypes.h" 

extern const DALSYSPropStructTblType DALPROP_StructPtrs_60xx_hyp[];

extern const uint32 DALPROP_PropBin_60xx_hyp[];

extern const StringDevice driver_list_60xx_hyp[];


const DALProps DALPROP_PropsInfo_60xx_hyp = {(const byte*)DALPROP_PropBin_60xx_hyp, DALPROP_StructPtrs_60xx_hyp, 1, driver_list_60xx_hyp};
