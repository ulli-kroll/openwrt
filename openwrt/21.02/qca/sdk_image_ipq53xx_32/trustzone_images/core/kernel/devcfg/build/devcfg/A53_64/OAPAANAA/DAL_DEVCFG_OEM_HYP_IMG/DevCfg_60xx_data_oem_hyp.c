#include "devcfgTypes.h" 

extern const DALSYSPropStructTblType DALPROP_StructPtrs_60xx_oem_hyp[];

extern const uint32 DALPROP_PropBin_60xx_oem_hyp[];

extern const StringDevice driver_list_60xx_oem_hyp[];


const DALProps DALPROP_PropsInfo_60xx_oem_hyp = {(const byte*)DALPROP_PropBin_60xx_oem_hyp, DALPROP_StructPtrs_60xx_oem_hyp, 2, driver_list_60xx_oem_hyp};
