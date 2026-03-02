#===========================================================================
#
#  @file hwio_Sdm660.py
#  @brief HWIO config file for the HWIO generation scripts for SDM660.
#
#  This file can be invoked by calling:
#
#    HWIOGen.py --cfg=hwio_Sdm660.py --flat=ARM_ADDRESS_FILE.FLAT
#
#  ===========================================================================
#
#  Copyright (c) 2015 QUALCOMM Technologies Incorporated.  
#  All Rights Reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================
#
#
#  ===========================================================================

CHIPSET = 'Sdm660'

# ============================================================================
# HWIO_BASE_FILES
# ============================================================================

base_resize = {
  'APSS_L2_MEM':0x00200000,
  'SYSTEM_IMEM': 0x00008000,
  'LPASS_IMEM':0x0,
  'CATS_128BIT':0x01000000,
  'CATS_64BIT':0x01000000,
  'RPM_CODE_RAM_START_ADDRESS': 0x0,
  'RPM_DATA_RAM_START_ADDRESS': 0x0,
  'RPM_SS_MSG_RAM_START_ADDRESS': 0x6000,
  'BOOT_ROM_START_ADDRESS': 0x0,
  'BOOTIMEM': 0x180000,
}

HWIO_BASE_FILES = [
  {
    'filename': '../../Include/msmhwiobase.h',
    'bases': ['.*'],
    'modules': ['TSENS*', 'OCIMEM_MPU*'],
    'filter-exclude': [
      '.*_END$',
      '.*_END_ADDRESS$',
      '.*_SIZE$',
      'EBI1_MEM_RPM_REGION.',
    ],
    'map-type': 'physical',
    'resize': base_resize,
    'map-filename': '../../Include/msmhwiomap.h',
    'devcfg-filename': '../../settings/HWIO/core/HWIOBaseMap.c',
    'check-sizes': True,
    'check-for-overlaps': True,
  }
]


# ============================================================================
# HWIO_T32VIEW_FILES
# ============================================================================

HWIO_T32VIEW_FILES = [
  {
    'symbol-filename': 'scripts/hwio.cmm',
    'limit-array-size': [ 10, 4 ],
    'per-filename': 'scripts/hwioreg',
    'filter-exclude': ['RESERVED', 'DUMMY']
  },
]


# ============================================================================
# Main
#
# Entry point when invoking this directly.
# ============================================================================

if __name__ == "__main__":
  from subprocess import Popen
  hwiogen = Popen([r"python", r"\\ben\corebsp_labdata_0001\sysdrv\hwio\HWIOGen.py", "--cfg=hwio.py", "--flat=ARM_ADDRESS_FILE.FLAT"], shell=True)
  hwiogen.wait()
