#==============================================================================
#
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
#===============================================================================
#
# Gen_xbl_nand_elf
#
# GENERAL DESCRIPTION
#    Contains functionality to generate xbl_nand.elf 
#             by combining xbl.elf with xbl_preamble.mbn
#
#   USAGE - python Gen_xbl_nand_elf <xbl.elf> -options
#           -f NAND_4K   or -f NAND_2K
#-------------------------------------------------------------------------------
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who      what, where, why
# --------   ---      ---------------------------------------------------------
# 05/06/22   mignatiu  Added support for NAND_4K
# 02/25/22   mignatiu  CR 3135449: Created initial script.
#===============================================================================

import struct
import os
import shutil
from optparse import OptionParser

#----------------------------------------------------------------------------
# GLOBAL VARIABLES BEGIN
#----------------------------------------------------------------------------
PAD_BYTE_1                = 255             # Padding byte 1s
PAD_BYTE_0                = 0               # Padding byte 0s
VIRTUAL_BLOCK_SIZE        = 131072          # Virtual block size for MCs insertion in SBL1 if ENABLE_VIRTUAL_BLK ON

# Magic numbers filled in for boot headers
FLASH_CODE_WORD                       = 0x844BDCD1
MAGIC_NUM                             = 0x73D71034
AUTODETECT_PAGE_SIZE_MAGIC_NUM        = 0x7D0B435A
AUTODETECT_PAGE_SIZE_MAGIC_NUM64      = 0x7D0B5436
AUTODETECT_PAGE_SIZE_MAGIC_NUM128     = 0x7D0B6577
SBL_VIRTUAL_BLOCK_MAGIC_NUM           = 0xD48B54C6

#----------------------------------------------------------------------------
# Pad a file with specific number of bytes
# Note: Assumes the fp is seeked to the correct location of padding
#----------------------------------------------------------------------------
def pad_file(fp, num_bytes, value):

   if num_bytes < 0:
      raise RuntimeError, "Number of bytes to pad must be greater than zero"

   while num_bytes > 0:
      fp.write('%c' % value)
      num_bytes -= 1

   return

#----------------------------------------------------------------------------
# Helper functions to open a file and return a valid file object
#----------------------------------------------------------------------------
def OPEN(file_name, mode):
    try:
       fp = open(file_name, mode)
    except IOError:
       raise RuntimeError, "The file could not be opened: " + file_name

    # File open has succeeded with the given mode, return the file object
    return fp

#----------------------------------------------------------------------------
# Concatenates the files listed in 'sources' in order and writes to 'target'
#----------------------------------------------------------------------------
def concat_files (target, sources):
   if type(sources) is not list:
      sources = [sources]

   target_file = OPEN(target,'wb')

   for fname in sources:
      file = OPEN(fname,'rb')
      while True:
         bin_data = file.read(65536)
         if not bin_data:
            break
         target_file.write(bin_data)
      file.close()
   target_file.close()

#----------------------------------------------------------------------------
# image_preamble
#----------------------------------------------------------------------------
def image_preamble(gen_dict, preamble_file_name, boot_sbl_header, num_of_pages=None):
   # Generate the preamble file
   preamble_fp = OPEN(preamble_file_name, 'wb')

   # Initialize
   max_size_verify = 8192
   flash_max_page = 8192

   # To support preamble addition for both 4k and 2k NAND elf's
   if(options.flashtype == 'NAND_4K'):
       flash_min_page = 4096
   else:
       flash_min_page = 2048

   autodetectpage = [int('0xFFFFFFFF',16)] * max_size_verify

   # The first three entries in the preamble must include the following values
   autodetectpage[0] = FLASH_CODE_WORD
   autodetectpage[1] = MAGIC_NUM
   if (num_of_pages == 64):
     autodetectpage[2] = AUTODETECT_PAGE_SIZE_MAGIC_NUM64
   elif (num_of_pages == 128):
     autodetectpage[2] = AUTODETECT_PAGE_SIZE_MAGIC_NUM128
   else:
     autodetectpage[2] = AUTODETECT_PAGE_SIZE_MAGIC_NUM

   # Package the list into binary data to be written to the preamble
   s = struct.Struct('I' * max_size_verify)
   packed_data = s.pack(*autodetectpage)

   # Output preamble pages based on maximum/minimum page size support
   for i in range(flash_max_page/flash_min_page):
      preamble_fp.write(packed_data[:flash_min_page])

   # Determine appropriate amount of padding for the preamble and
   # update the boot_sbl_header accordingly

   amount_to_write = 2048

   pad_file(preamble_fp, amount_to_write, PAD_BYTE_1)
   preamble_fp.close()

#----------------------------------------------------------------------------
# Helper functions to insert MCs in SBL1(Badger) if ENABLE_VIRTUAL_BLK is ON
#----------------------------------------------------------------------------
def insert_SBL1_magicCookie (env, target):
    file = open(target, "rb")
    #read the file contents
    filedata = file.read()
    length = len(filedata)
    file.close()

    if (length <= VIRTUAL_BLOCK_SIZE):
        return None
    else:
        #remove the previous file
        os.remove(target)
        #generate new file for appending target data + required MCs
        file = open(target, "ab")

        filedata_till_128kb = filedata[0:VIRTUAL_BLOCK_SIZE]
        filedata_after_128kb = filedata[VIRTUAL_BLOCK_SIZE:length]

        a = str(hex(FLASH_CODE_WORD))
        mc1 = chr(int(a[8:10],16)) + chr(int(a[6:8],16)) + chr(int(a[4:6],16)) + chr(int(a[2:4],16))

        b = str(hex(MAGIC_NUM))
        mc2 = chr(int(b[8:10],16)) + chr(int(b[6:8],16)) + chr(int(b[4:6],16)) + chr(int(b[2:4],16))

        c = str(hex(SBL_VIRTUAL_BLOCK_MAGIC_NUM))
        mc3 = chr(int(c[8:10],16)) + chr(int(c[6:8],16)) + chr(int(c[4:6],16)) + chr(int(c[2:4],16))

        while length > VIRTUAL_BLOCK_SIZE:
            file.write(filedata_till_128kb)
            filedata = mc1 + mc2 + mc3 + filedata_after_128kb
            length = len(filedata)
            filedata_till_128kb = filedata[0:VIRTUAL_BLOCK_SIZE]
            filedata_after_128kb = filedata[VIRTUAL_BLOCK_SIZE:length]

        #copy the leftover data (<128KB) in output file
        if length > 0:
            file.write(filedata)

        #close the final output file
        file.close()
    # MC_insertion code end


##############################################################################
# sign_nand_image
##############################################################################
def sign_nand_image(input_image):

	# Setup temp files
	output_dir_root = os.getcwd()
	preamble_file_name = "xbl_preamble.mbn"
	target_combined_unsigned = "xbl_combined_unsigned.mbn"
	target_intermediate_elf = "xbl_intermediate_elf.mbn"
    
    # create the output directory
        output_dir_user = os.path.join(output_dir_root, options.outputDir)
        if not os.path.isdir(output_dir_user):
            os.makedirs(output_dir_user)
    
    # copy input image to temp files
	shutil.copy2(input_image, target_combined_unsigned)
	shutil.copy2(input_image, target_intermediate_elf)

	print "[buildit.py] Generating " + options.flashtype +" signed image...",

	image_preamble([], preamble_file_name, None)
		
	files_to_cat_in_order = [target_combined_unsigned]
	files_to_cat_in_order.insert(0, preamble_file_name)
	concat_files(target_intermediate_elf, files_to_cat_in_order)
	insert_SBL1_magicCookie([], target_intermediate_elf)
		
	if(options.flashtype == 'NAND_4K'):
            shutil.copy2(target_intermediate_elf, os.path.join(output_dir_user, "xbl_nand_4K.elf"))		
        else:
            shutil.copy2(target_intermediate_elf, os.path.join(output_dir_user, "xbl_nand.elf"))

		
	print "Done"

if __name__ == '__main__':
    usage = 'usage: %prog INFILE1 [options] \n' + '\t Supported options "-f" "-o" \n'

    parser = OptionParser(usage)

    parser.add_option('-f', '--flashtype', default="NAND_2K",
                                          help="NAND_4K or NAND_2K " + \
                                                 "Example: -f NAND_4K or -f NAND_2K")
                                           
    parser.add_option('-o', '--outputDir', default=".",
                                          help="specify the output directory default=PWD \n" + \
                                                 "Example: -o ../preambleImage/")

    (options, args) = parser.parse_args()

    if len(args) != 1:
        print("Invalid arguments! Exiting...\n")
        parser.print_help()
        sys.exit(1)

    options.inputfile1 = args[0]

    sign_nand_image(options.inputfile1)
    os.remove("xbl_combined_unsigned.mbn")
    os.remove("xbl_intermediate_elf.mbn")
    os.remove("xbl_preamble.mbn")
