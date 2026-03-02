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
#include "custmapaanaaa.h"
#include "targmapaanaaa.h"
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
extern void * bam_tgt_config_53xx_tz;
extern void * spmiInfo_53xx_tz;
extern void * ABT_propdata_53xx_tz;
extern void * NOCERR_propdata_53xx_tz;
extern void * info_53xx_tz;


static HAL_vmidmt_InfoType	devcfg_5={((0x07980000+0x00000000)+0x00000000),{10,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_6={((0x07880000+0x00000000)+0x00000000),{11,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_7={((0x04000000+0x00049000)+0x00000000),{2,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_8={((0x04000000+0x00108000)+0x00000000),{2,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_9={((0x00075000+0x00000000)+0x00000000),{32,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_10={((0x00073000+0x00000000)+0x00000000),{2,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_11={((0x00071000+0x00000000)+0x00000000),{2,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_12={((0x00070000+0x00000000)+0x00000000),{1,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_13={((0x0006b000+0x00000000)+0x00000000),{4,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_14={((0x0006a000+0x00000000)+0x00000000),{4,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_15={((0x00069000+0x00000000)+0x00000000),{4,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_16={((0x00068000+0x00000000)+0x00000000),{4,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_17={((0x00046000+0x00000000)+0x00000000),{2,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_18={((0x00045000+0x00000000)+0x00000000),{2,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_19={((0x00042000+0x00000000)+0x00000000),{2,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_20={((0x00041000+0x00000000)+0x00000000),{2,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_21={((0x00040000+0x00000000)+0x00000000),{2,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_22={((0x00030000+0x00000000)+0x00000000),{1,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_23={((0x0002a000+0x00000000)+0x00000000),{4,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_24={((0x00029000+0x00000000)+0x00000000),{4,0,0,0,0,0}};


static ddr_config_data_t	devcfg_25={4,(0x40000000+0x0A600000),0x0200000,};


static HAL_qgic_BaseAddressType	devcfg_26={0xB000000,0xB002000,0xB001000,0xB004000,0x0,};


static timer_reg_data_t	devcfg_27={1,7,18,42,0x4000,{0x0B120000},};


static dbg_cfg_data_t	devcfg_28={0x004AA000,0x0B01700C,0x0B111004,0x01814000,349,36,255,235,};

const DALSYSPropStructTblType DALPROP_StructPtrs_53xx_tz[30] =  {
	 {sizeof(void *), &bam_tgt_config_53xx_tz},
	 {sizeof(void *), &spmiInfo_53xx_tz},
	 {sizeof(void *), &ABT_propdata_53xx_tz},
	 {sizeof(void *), &NOCERR_propdata_53xx_tz},
	 {sizeof(void *), &info_53xx_tz},
	 {sizeof(devcfg_5), &devcfg_5},
	 {sizeof(devcfg_6), &devcfg_6},
	 {sizeof(devcfg_7), &devcfg_7},
	 {sizeof(devcfg_8), &devcfg_8},
	 {sizeof(devcfg_9), &devcfg_9},
	 {sizeof(devcfg_10), &devcfg_10},
	 {sizeof(devcfg_11), &devcfg_11},
	 {sizeof(devcfg_12), &devcfg_12},
	 {sizeof(devcfg_13), &devcfg_13},
	 {sizeof(devcfg_14), &devcfg_14},
	 {sizeof(devcfg_15), &devcfg_15},
	 {sizeof(devcfg_16), &devcfg_16},
	 {sizeof(devcfg_17), &devcfg_17},
	 {sizeof(devcfg_18), &devcfg_18},
	 {sizeof(devcfg_19), &devcfg_19},
	 {sizeof(devcfg_20), &devcfg_20},
	 {sizeof(devcfg_21), &devcfg_21},
	 {sizeof(devcfg_22), &devcfg_22},
	 {sizeof(devcfg_23), &devcfg_23},
	 {sizeof(devcfg_24), &devcfg_24},
	 {sizeof(devcfg_25), &devcfg_25},
	 {sizeof(devcfg_26), &devcfg_26},
	 {sizeof(devcfg_27), &devcfg_27},
	 {sizeof(devcfg_28), &devcfg_28},
	{0, 0 } 
 };
const uint32 DALPROP_PropBin_53xx_tz[] = {

			0x000002dc, 0x00000020, 0x0000010c, 0x0000011c, 0x0000011c, 
			0x00000001, 0x00000000, 0x00000178, 0x5f6d6162, 0x5f746774, 
			0x666e6f63, 0x6f006769, 0x72656e77, 0x6d707300, 0x65645f69, 
			0x65636976, 0x61440073, 0x766e456c, 0x72615400, 0x43746567, 
			0x47006766, 0x7443626c, 0x57487478, 0x6574754d, 0x6d754e78, 
			0x00726562, 0x72736374, 0x7361625f, 0x616e5f65, 0x7400656d, 
			0x5f727363, 0x65736162, 0x74756d00, 0x6f5f7865, 0x65736666, 
			0x77007374, 0x65636e6f, 0x66666f5f, 0x73746573, 0x54424100, 
			0x6f72505f, 0x74616470, 0x4f4e0061, 0x72724543, 0x505f726f, 
			0x64706f72, 0x00617461, 0x6f666e69, 0x6d756e00, 0x6168635f, 
			0x6c656e6e, 0x756e0073, 0x65735f6d, 0x6e656d67, 0x61007374, 
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
			0x1280007d, 0x00000002, 0xff00ff00, 0x1280008a, 0x00000003, 
			0xff00ff00, 0x1280009c, 0x00000004, 0xff00ff00, 0x028000a1, 
			0x00000001, 0x028000ae, 0x00000003, 0xff00ff00, 0x028000bb, 
			0x00000000, 0xff00ff00, 0x12000027, 0x00000005, 0x12000018, 
			0x00000006, 0x12000022, 0x00000007, 0x12000021, 0x00000008, 
			0x1200004b, 0x00000009, 0x12000047, 0x0000000a, 0x1200004a, 
			0x0000000b, 0x12000055, 0x0000000c, 0x1200004d, 0x0000000d, 
			0x1200004c, 0x0000000e, 0x12000051, 0x0000000f, 0x12000050, 
			0x00000010, 0x12000053, 0x00000011, 0x12000052, 0x00000012, 
			0x12000054, 0x00000013, 0x12000048, 0x00000014, 0x12000049, 
			0x00000015, 0x12000025, 0x00000016, 0x1200004f, 0x00000017, 
			0x1200004e, 0x00000018, 0xff00ff00, 0x12000001, 0x00000019, 
			0xff00ff00, 0x12000001, 0x0000001a, 0xff00ff00, 0x12000001, 
			0x0000001b, 0xff00ff00, 0x12000001, 0x0000001c, 0xff00ff00, 
			0x028000d6, 0x00000000, 0xff00ff00 };



const StringDevice driver_list_53xx_tz[] = {
			{"/core/hwengines/bam",1285428979u, 344, NULL, 0, NULL },
			{"DALDEVICEID_SPMI_DEVICE",3290583706u, 356, NULL, 0, NULL },
			{"/dev/core/mproc/smem",1012060316u, 404, NULL, 0, NULL },
			{"/dev/ABTimeout",814297740u, 440, NULL, 0, NULL },
			{"/dev/NOCError",1518077100u, 452, NULL, 0, NULL },
			{"/icb/uarb",3907582491u, 464, NULL, 0, NULL },
			{"/dev/icbcfg/boot",2382255043u, 476, NULL, 0, NULL },
			{"/storage/rpmb",2005825481u, 496, NULL, 0, NULL },
			{"/dev/vmidmt",337776915u, 508, NULL, 0, NULL },
			{"ddr",193489311u, 672, NULL, 0, NULL },
			{"int_controller",2851810835u, 684, NULL, 0, NULL },
			{"timer",275614598u, 696, NULL, 0, NULL },
			{"dbg_cfg",26643713u, 708, NULL, 0, NULL },
			{"boot_restrict",2479631688u, 720, NULL, 0, NULL }
};
