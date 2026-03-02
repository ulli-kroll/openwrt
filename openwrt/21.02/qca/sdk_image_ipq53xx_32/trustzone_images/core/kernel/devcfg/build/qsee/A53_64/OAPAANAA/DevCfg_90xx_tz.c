#include "DALPropDef.h"
#include "DALDeviceId.h"
#include "DALPropDef.h"
#include "HALvmidmt.h"
#include "HALvmidmtTarget.h"
#include "HALcomdef.h"
#include "com_dtypes.h"
#include "HALvmidmtInfoImpl.h"
#include "HALhwio.h"
#include "HALvmidmtHWIOTarget.h"
#include "msmhwiobase.h"
#include "ddr_defs.h"
#include "tzbsp_target.h"
#include "customer.h"
#include "custoapaanaaa.h"
#include "targoapaanaaa.h"
#include "custremoteapis.h"
#include "custtarget.h"
#include "custsdcc.h"
#include "custsurf.h"
#include "custdiag.h"
#include "custefs.h"
#include "custpmic.h"
#include "custsio_8660.h"
#include "custsec.h"
#include "custsfs.h"
#include "tzbsp_tz_memmap.h"
#include "tzbsp_quad_target.h"
#include "HALqgic.h"
#include "timer_defs.h"
#include "dbg_cfg.h"
#include "dbg_cfg_arch.h"
#ifndef DEVCFG_STANDALONE 
#include "DALStdDef.h" 
#include "DALSysTypes.h" 

#else 
#include "stubs.h" 

#include "devcfgTypes.h" 

#endif 
extern void * bam_tgt_config_90xx_tz;
extern void * spmiInfo_90xx_tz;
extern void * NOCERR_propdata_90xx_tz;
extern void * ABT_propdata_90xx_tz;
extern void * info_90xx_tz;


static HAL_vmidmt_InfoType	devcfg_5={((0x00700000+0x00000000)+0x00000000),{17,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_6={((0x07880000+0x00000000)+0x00000000),{19,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_7={((0x00280000+0x00008000)+0x00000000),{1,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_8={((0x07980000+0x00000000)+0x00000000),{8,0,0,0,0,0}};


static ddr_config_data_t	devcfg_9={4,(0x40000000+0x0A600000),0x0200000,};


static HAL_qgic_BaseAddressType	devcfg_10={0xB000000,0xB002000,0xB001000,0xB004000,0x0,};


static timer_reg_data_t	devcfg_11={1,7,18,42,0x4000,{0x0B120000},};


static dbg_cfg_data_t	devcfg_12={0x004AA000,0x0B01700C,0x0B111004,0x01814000,213,36,269,206,329,543,};

const DALSYSPropStructTblType DALPROP_StructPtrs_90xx_tz[14] =  {
	 {sizeof(void *), &bam_tgt_config_90xx_tz},
	 {sizeof(void *), &spmiInfo_90xx_tz},
	 {sizeof(void *), &NOCERR_propdata_90xx_tz},
	 {sizeof(void *), &ABT_propdata_90xx_tz},
	 {sizeof(void *), &info_90xx_tz},
	 {sizeof(devcfg_5), &devcfg_5},
	 {sizeof(devcfg_6), &devcfg_6},
	 {sizeof(devcfg_7), &devcfg_7},
	 {sizeof(devcfg_8), &devcfg_8},
	 {sizeof(devcfg_9), &devcfg_9},
	 {sizeof(devcfg_10), &devcfg_10},
	 {sizeof(devcfg_11), &devcfg_11},
	 {sizeof(devcfg_12), &devcfg_12},
	{0, 0 } 
 };
const uint32 DALPROP_PropBin_90xx_tz[] = {

			0x0000025c, 0x00000020, 0x0000010c, 0x0000011c, 0x0000011c, 
			0x00000001, 0x00000000, 0x00000178, 0x5f6d6162, 0x5f746774, 
			0x666e6f63, 0x6f006769, 0x72656e77, 0x6d707300, 0x65645f69, 
			0x65636976, 0x61440073, 0x766e456c, 0x72615400, 0x43746567, 
			0x47006766, 0x7443626c, 0x57487478, 0x6574754d, 0x6d754e78, 
			0x00726562, 0x72736374, 0x7361625f, 0x616e5f65, 0x7400656d, 
			0x5f727363, 0x65736162, 0x74756d00, 0x6f5f7865, 0x65736666, 
			0x77007374, 0x65636e6f, 0x66666f5f, 0x73746573, 0x434f4e00, 
			0x6f727245, 0x72505f72, 0x6164706f, 0x6e006174, 0x635f6d75, 
			0x6e6e6168, 0x00736c65, 0x5f6d756e, 0x6d676573, 0x73746e65, 
			0x54424100, 0x6f72505f, 0x74616470, 0x6e690061, 0x61006f66, 
			0x776f6c6c, 0x6172655f, 0x695f6573, 0x65735f6e, 0x65727563, 
			0x646f6d5f, 0x79620065, 0x73736170, 0x6f6f625f, 0x65725f74, 
			0x69727473, 0x00007463, 0x45524f43, 0x504f545f, 0x5253435f, 
			0x00000000, 0x00000001, 0x00000000, 0x00002070, 0x00000007, 
			0x00005000, 0x00006000, 0x00007000, 0x00008000, 0x00009000, 
			0x0000a000, 0x0000b000, 0x0000c000, 0x00000001, 0x0003d000, 
			0x0003d004, 0x12800000, 0x00000000, 0xff00ff00, 0x0280000f, 
			0x00000001, 0x12800015, 0x00000001, 0xff00ff00, 0x02800022, 
			0x10000001, 0x14800029, 0x00000000, 0x02800033, 0x00000009, 
			0xff00ff00, 0x11800048, 0x00000000, 0x02800057, 0x01900000, 
			0x14800061, 0x0000000c, 0x1480006f, 0x00000030, 0xff00ff00, 
			0x1280007d, 0x00000002, 0xff00ff00, 0x0280008f, 0x00000001, 
			0x0280009c, 0x00000003, 0xff00ff00, 0x128000a9, 0x00000003, 
			0xff00ff00, 0x128000b6, 0x00000004, 0xff00ff00, 0x028000bb, 
			0x00000000, 0xff00ff00, 0x12000002, 0x00000005, 0x12000018, 
			0x00000006, 0x12000023, 0x00000007, 0x12000027, 0x00000008, 
			0xff00ff00, 0x12000001, 0x00000009, 0xff00ff00, 0x12000001, 
			0x0000000a, 0xff00ff00, 0x12000001, 0x0000000b, 0xff00ff00, 
			0x12000001, 0x0000000c, 0xff00ff00, 0x028000d6, 0x00000000, 
			0xff00ff00 };



const StringDevice driver_list_90xx_tz[] = {
			{"/core/hwengines/bam",1285428979u, 344, NULL, 0, NULL },
			{"DALDEVICEID_SPMI_DEVICE",3290583706u, 356, NULL, 0, NULL },
			{"/dev/core/mproc/smem",1012060316u, 404, NULL, 0, NULL },
			{"/dev/NOCError",1518077100u, 440, NULL, 0, NULL },
			{"/dev/icbcfg/boot",2382255043u, 452, NULL, 0, NULL },
			{"/dev/ABTimeout",814297740u, 472, NULL, 0, NULL },
			{"/icb/uarb",3907582491u, 484, NULL, 0, NULL },
			{"/storage/rpmb",2005825481u, 496, NULL, 0, NULL },
			{"/dev/vmidmt",337776915u, 508, NULL, 0, NULL },
			{"ddr",193489311u, 544, NULL, 0, NULL },
			{"int_controller",2851810835u, 556, NULL, 0, NULL },
			{"timer",275614598u, 568, NULL, 0, NULL },
			{"dbg_cfg",26643713u, 580, NULL, 0, NULL },
			{"boot_restrict",2479631688u, 592, NULL, 0, NULL }
};
