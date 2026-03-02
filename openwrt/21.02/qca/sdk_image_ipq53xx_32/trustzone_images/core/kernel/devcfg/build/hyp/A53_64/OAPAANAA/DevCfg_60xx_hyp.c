#include "DALPropDef.h"
#include "DALDeviceId.h"
#include "DALPropDef.h"
#ifndef DEVCFG_STANDALONE 
#include "DALStdDef.h" 
#include "DALSysTypes.h" 

#else 
#include "stubs.h" 

#include "devcfgTypes.h" 

#endif 

const DALSYSPropStructTblType DALPROP_StructPtrs_60xx_hyp[1] =  {
	{0, 0 } 
 };
const uint32 DALPROP_PropBin_60xx_hyp[] = {

			0x00000084, 0x00000020, 0x00000050, 0x00000050, 0x00000050, 
			0x00000001, 0x00000000, 0x0000005c, 0x456c6144, 0x5400766e, 
			0x65677261, 0x67664374, 0x626c4700, 0x74787443, 0x754d5748, 
			0x4e786574, 0x65626d75, 0x6e650072, 0x656c6261, 0x00706b5f, 
			0x00000001, 0x00000000, 0x00002070, 0x02800000, 0x10000002, 
			0x14800007, 0x00000000, 0x02800011, 0x0000000a, 0xff00ff00, 
			0x02800026, 0x00000001, 0xff00ff00 };



const StringDevice driver_list_60xx_hyp[] = {
			{"/kp/general",1520996892u, 120, NULL, 0, NULL }
};
