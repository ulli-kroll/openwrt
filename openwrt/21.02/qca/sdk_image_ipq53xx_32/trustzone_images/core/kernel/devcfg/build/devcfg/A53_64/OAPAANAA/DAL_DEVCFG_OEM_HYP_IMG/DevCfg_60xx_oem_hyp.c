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

const DALSYSPropStructTblType DALPROP_StructPtrs_60xx_oem_hyp[1] =  {
	{0, 0 } 
 };
const uint32 DALPROP_PropBin_60xx_oem_hyp[] = {

			0x000000b0, 0x00000020, 0x00000070, 0x00000070, 0x00000070, 
			0x00000001, 0x00000001, 0x0000007c, 0x456c6144, 0x5400766e, 
			0x65677261, 0x67664374, 0x73696400, 0x656c6261, 0x6d6d735f, 
			0x63615f75, 0x4d454f00, 0x72656b5f, 0x5f6c656e, 0x65646f63, 
			0x6765725f, 0x006e6f69, 0x5f4d454f, 0x6e72656b, 0x635f6c65, 
			0x5f65646f, 0x657a6973, 0x00000000, 0x00000001, 0x00000000, 
			0x00002070, 0x02800000, 0x80000002, 0x14800007, 0x00000000, 
			0xff00ff00, 0x02800011, 0x00000000, 0xff00ff00, 0x02800021, 
			0x41100000, 0x02800038, 0x006f0000, 0xff00ff00 };



const StringDevice driver_list_60xx_oem_hyp[] = {
			{"/ac/smmu",4045178921u, 144, NULL, 0, NULL },
			{"/hyp/oem",1136174517u, 156, NULL, 0, NULL }
};
