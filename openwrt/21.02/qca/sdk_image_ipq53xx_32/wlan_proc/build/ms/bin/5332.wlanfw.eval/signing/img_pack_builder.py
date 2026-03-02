#============================================================================
#
# Packing of IPQxxxx artifacts with oem build
#
# GENERAL DESCRIPTION
#    python img_pack_builder.py --base_dir=<path to FW_IMAGES> --chipset=chipid --ops=pack                                    --to re-pack default builds
#    python img_pack_builder.py --base_dir=<path to FW_IMAGES> --chipset=chipid --ops=pack --oem_pds=rpd,pd0 --iu=iu_img      --to pack ipq53xx artifacts with ipq53xx pd
#    python img_pack_builder.py --base_dir=<path to FW_IMAGES> --chipset=chipid --ops=pack --oem_pds=rpd,pd0,pd1 --iu=iu_img  --to pack ipq53xx artifacts with oem ipq53xx and oem qcn9160 oem pds
#
# Copyright (c) 2023 by Qualcomm Technologies Inc.
# All Rights Reserved.
# Qualcomm Technologies, Inc. Confidential and Proprietary
#
#----------------------------------------------------------------------------
import os
import re
import sys
import struct
import shutil
import string
import argparse

#USER Customistation arear
squashfs_path='/pkg/qct/software/ubuntu/mksquashfs/mksquashfs4'
miami_iu_ddr_addr = 0x4B540000
miami_york_iu_ddr_addr = 0x4BE40000

#Implementation, don't change anything after this line
parser = argparse.ArgumentParser(description="script to pack ipq53xx artifacts as squashfs", add_help=False)
arglist = parser.add_argument_group('List of supported arguments')
#arglist.add_argument("--out_path", help = "Path to the output folder")
arglist.add_argument("--base_dir", help = "Optional base directory, if not given current directory will be treated as base directory")
arglist.add_argument("--chipset", help = "Chipset")
arglist.add_argument("--ops", help = "coma separated operations. Valid values [pack,combine]")
arglist.add_argument("--oem_pds", help = "coma separated oem pd(s) name with path relative to FW_IMAGES ")
arglist.add_argument("--iu", help = "coma separated iu img name with path relative to FW_IMAGES ")
arglist.add_argument("--imgs", help = "coma separated squashfs images to be packed, ex: --imgs=file1,file2,combinedFile")
arglist.add_argument("--license", help = "To pass the license file to be packed with the final squashfs")
arglist.add_argument("--help", help = "Help")
args = parser.parse_args()

#CURR_DIR is where this script is placed and it should be placed at wlan_proc/build/ms/bin/<variant>/signing
CURR_DIR = os.path.dirname(__file__)

# ELF Definitions
ELF_HDR_SIZE              = 52          
ELF_PHDR_SIZE             = 32          
ELFINFO_MAG0_INDEX        = 0
ELFINFO_MAG1_INDEX        = 1
ELFINFO_MAG2_INDEX        = 2
ELFINFO_MAG3_INDEX        = 3
ELFINFO_MAG0              = '\x7f'
ELFINFO_MAG1              = 'E'
ELFINFO_MAG2              = 'L'
ELFINFO_MAG3              = 'F'
ELFINFO_CLASS_INDEX       = 4
ELFINFO_CLASS_32          = '\x01'
ELFINFO_VERSION_INDEX     = 6
ELFINFO_VERSION_CURRENT   = '\x01'
ELF_BLOCK_ALIGN           = 0x1000
ALLOWED_IMG_ALIGN_VALUES = [0x100000, 0x400000] #do not change unless Peripheral image loader (apps driver) is compliant with more align values.

# ELF Program Header Types
NULL_TYPE                 = 0x0
LOAD_TYPE                 = 0x1
DYNAMIC_TYPE              = 0x2
INTERP_TYPE               = 0x3
NOTE_TYPE                 = 0x4
SHLIB_TYPE                = 0x5
PHDR_TYPE                 = 0x6
TLS_TYPE                  = 0x7

#----------------------------------------------------------------------------
# ELF Header Class 
#----------------------------------------------------------------------------
class Elf32_Ehdr:
   # Structure object to align and package the ELF Header
   s = struct.Struct('16sHHIIIIIHHHHHH') 
   
   def __init__(self, data): 
      unpacked_data       = (Elf32_Ehdr.s).unpack(data)
      self.unpacked_data  = unpacked_data
      self.e_ident        = unpacked_data[0]
      self.e_type         = unpacked_data[1]
      self.e_machine      = unpacked_data[2]
      self.e_version      = unpacked_data[3]
      self.e_entry        = unpacked_data[4]
      self.e_phoff        = unpacked_data[5]
      self.e_shoff        = unpacked_data[6]
      self.e_flags        = unpacked_data[7]
      self.e_ehsize       = unpacked_data[8]
      self.e_phentsize    = unpacked_data[9]
      self.e_phnum        = unpacked_data[10]
      self.e_shentsize    = unpacked_data[11]
      self.e_shnum        = unpacked_data[12]
      self.e_shstrndx     = unpacked_data[13]

   def printValues(self):
      print "ATTRIBUTE / VALUE"
      for attr, value in self.__dict__.iteritems():
         print attr, value

   def getPackedData(self):
      values = [self.e_ident,
                self.e_type,
                self.e_machine,
                self.e_version,
                self.e_entry,
                self.e_phoff,
                self.e_shoff,
                self.e_flags,
                self.e_ehsize,
                self.e_phentsize,
                self.e_phnum,
                self.e_shentsize,
                self.e_shnum,
                self.e_shstrndx
               ]
      
      return (Elf32_Ehdr.s).pack(*values)

#----------------------------------------------------------------------------
# ELF Program Header Class 
#----------------------------------------------------------------------------
class Elf32_Phdr:

   # Structure object to align and package the ELF Program Header 
   s = struct.Struct('I' * 8) 
   
   def __init__(self, data): 
      unpacked_data       = (Elf32_Phdr.s).unpack(data)
      self.unpacked_data  = unpacked_data
      self.p_type         = unpacked_data[0]
      self.p_offset       = unpacked_data[1]
      self.p_vaddr        = unpacked_data[2]
      self.p_paddr        = unpacked_data[3]
      self.p_filesz       = unpacked_data[4]
      self.p_memsz        = unpacked_data[5]
      self.p_flags        = unpacked_data[6]
      self.p_align        = unpacked_data[7]

   def printValues(self):
      print "ATTRIBUTE / VALUE"
      for attr, value in self.__dict__.iteritems():
         print attr, value
            
   def getPackedData(self):
      values = [self.p_type,
                self.p_offset,
                self.p_vaddr,
                self.p_paddr,
                self.p_filesz,
                self.p_memsz,
                self.p_flags,
                self.p_align
               ]
      
      return (Elf32_Phdr.s).pack(*values)

def RunCmds(cmds):
    for cmd in cmds:
        os.system(cmd)
        #print("CMD: {}".format(cmd))
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
# Perform file copy given offsets and the number of bytes to copy
#----------------------------------------------------------------------------
def file_copy_offset(in_fp, in_off, out_fp, out_off, num_bytes):
   in_fp.seek(in_off)
   read_in = in_fp.read(num_bytes)
   out_fp.seek(out_off)
   out_fp.write(read_in)
 
   return num_bytes


#----------------------------------------------------------------------------
# Verify ELF header contents from an input ELF file
#----------------------------------------------------------------------------
def verify_elf_header(elf_header):
   if (elf_header.e_ident[ELFINFO_MAG0_INDEX] != ELFINFO_MAG0) or \
      (elf_header.e_ident[ELFINFO_MAG1_INDEX] != ELFINFO_MAG1) or \
      (elf_header.e_ident[ELFINFO_MAG2_INDEX] != ELFINFO_MAG2) or \
      (elf_header.e_ident[ELFINFO_MAG3_INDEX] != ELFINFO_MAG3) or \
      (elf_header.e_ident[ELFINFO_CLASS_INDEX] != ELFINFO_CLASS_32) or \
      (elf_header.e_ident[ELFINFO_VERSION_INDEX] != ELFINFO_VERSION_CURRENT):

      return False
   else:
      return True

#----------------------------------------------------------------------------
# Preprocess an ELF file and return the ELF Header Object and an 
# array of ELF Program Header Objects
#----------------------------------------------------------------------------
def preprocess_elf_file(elf_file_name):

   # Initialize
   elf_fp = OPEN(elf_file_name, 'rb')
   elf_header = Elf32_Ehdr(elf_fp.read(ELF_HDR_SIZE))
   phdr_table = []

   # Verify ELF header information
   if verify_elf_header(elf_header) is False:
      raise RuntimeError, "ELF file failed verification: " + elf_file_name

   # Get program header size
   phdr_size = elf_header.e_phentsize

   # Find the program header offset 
   file_offset = elf_header.e_phoff
   elf_fp.seek(file_offset) 
   
   # Read in the program headers
   for i in range(elf_header.e_phnum):
      phdr_table.append(Elf32_Phdr(elf_fp.read(phdr_size))) 

   elf_fp.close()
   return [elf_header, phdr_table]
   
#----------------------------------------------------------------------------
# wrap_elf_header in an image
#----------------------------------------------------------------------------

def wrap_elf_header_in_images(elf_in_file_name,elf_out_file_name, p_addr=0x0):
    elf_fp = OPEN(elf_in_file_name, 'rb')
    elf_hdr_new = Elf32_Ehdr(elf_fp.read(ELF_HDR_SIZE))
    elf_hdr_new.e_phnum = 3
    elf_hdr_new.e_phoff = ELF_HDR_SIZE
    elf_fp.close()

    [elf_header, phdr_table] = preprocess_elf_file(elf_in_file_name)
    phdr_new = Elf32_Phdr('\0' * ELF_PHDR_SIZE)
    elf_in_fp = OPEN(elf_in_file_name, "rb")

    if elf_out_file_name is not None:
        elf_out_fp = OPEN(elf_out_file_name, "wb+")

    num_phdrs = elf_header.e_phnum
    header_size = 0
    if p_addr is not 0x0:
        elf_header.e_entry = p_addr
    else:
        for i in range(num_phdrs):
            curr_phdr = phdr_table[i]
            if curr_phdr.p_type == LOAD_TYPE:
                elf_header.e_entry = curr_phdr.p_paddr
                header_size = curr_phdr.p_offset
                break

    #Adjust entry address of upper elf accounting ELF and Hash segments of original elf
    phdr_new.p_paddr    = elf_header.e_entry - header_size
    temp_paddr          = phdr_new.p_paddr
    phdr_new.p_vaddr    = phdr_new.p_paddr
    elf_hdr_new.e_entry = phdr_new.p_paddr

    phdr_new.p_memsz  = phdr_table[-1].p_offset + phdr_table[-1].p_memsz
    temp_msize        = phdr_new.p_memsz
    phdr_new.p_filesz = phdr_table[-1].p_offset + phdr_table[-1].p_filesz
    tmp_filesize      = phdr_new.p_filesz
    phdr_new.p_align  = 0x1000
    phdr_new.p_offset  = 0x2000
    phdr_new.p_type  = LOAD_TYPE

    elf_out_fp.seek(ELF_HDR_SIZE + 2 * ELF_PHDR_SIZE)
    elf_out_fp.write(phdr_new.getPackedData())

    phdr_new.p_paddr    = 0
    phdr_new.p_vaddr    = 0
    phdr_new.p_memsz  = 0
    phdr_new.p_filesz = ELF_HDR_SIZE + 3 * ELF_PHDR_SIZE
    phdr_new.p_align  = 0
    phdr_new.p_offset  = 0
    phdr_new.p_type  = NULL_TYPE
    phdr_new.p_flags = 0x07000000

    elf_out_fp.seek(ELF_HDR_SIZE)
    elf_out_fp.write(phdr_new.getPackedData())
    
    phdr_new.p_paddr    = temp_paddr + temp_msize
    phdr_new.p_vaddr    = temp_paddr + temp_msize
    phdr_new.p_memsz    = 0x1000
    phdr_new.p_filesz = 0x88
    phdr_new.p_align  = 0x1000
    phdr_new.p_offset  = 0x1000
    phdr_new.p_type  = NULL_TYPE
    phdr_new.p_flags = 0x02200000

    elf_out_fp.seek(ELF_HDR_SIZE + 1 * ELF_PHDR_SIZE)
    elf_out_fp.write(phdr_new.getPackedData())


    file_copy_offset(elf_in_fp, 0x0, elf_out_fp, 0x2000, tmp_filesize)


    elf_out_fp.seek(0)
    elf_out_fp.write(elf_hdr_new.getPackedData())

    elf_in_fp.close()
    if elf_out_file_name is not None:
      elf_out_fp.close()

    return elf_out_file_name
    
#----------------------------------------------------------------------------
# pil splitter
#----------------------------------------------------------------------------
def pil_splitter(target, source):
   target_prefix = os.path.splitext(target)[0]
   cmds = ['python ' + CURR_DIR + '/pil-splitter.py %s %s' % (source, target_prefix)]
   RunCmds(cmds)
   return 0

def combine_images(inf0, inf1, outf, base_dir):
    '''
    Given two squashfs images, this API will combine them into one.
    '''
    combineCmds = [
        'unsquashfs -d ' + base_dir + 'FW_IMG1 '+ inf0,
        'unsquashfs -d ' + base_dir + 'FW_IMG2 '+ inf1,
        'cp -r -f ' + base_dir + 'FW_IMG2/* ' + base_dir + 'FW_IMG1',
        squashfs_path + ' ' + base_dir + 'FW_IMG1/ ' + base_dir + 'wifi_fw_tmp.squashfs -nopad -noappend -root-owned -comp xz -Xpreset 9 -Xe -Xlc 0 -Xlp 2 -Xpb 2 -Xbcj arm -b 256k -processors 1',
        'rm -rf ' + base_dir + outf,
        'dd if='  + base_dir + 'wifi_fw_tmp.squashfs of=' + base_dir + outf + " bs=2k conv=sync",
        'rm -rf ' + base_dir + 'wifi_fw_tmp.squashfs',
        'rm -rf ' + base_dir + 'FW_IMG1',
        'rm -rf ' + base_dir + 'FW_IMG2']

    RunCmds(combineCmds)
    print("Combined image generated at : {}".format(base_dir + outf))
    return (outf)

def repack_mpd_images(pds, base_dir, ius=None):
    '''
    This will re-pack all existing squashfs available in base_dir/FW_IMAGES folder
    '''
    img_list = []
    if os.path.exists(img_path):
        for f in os.listdir(img_path):
            if f.endswith('squashfs.img'):
                img_list.append(f)
    else:
        print("ERROR: No squashfs images found to repack. base={}".format(img_path))
        return (None)
        
    RunCmds(['mkdir ' + base_dir + 'TMP_PIL_IMGS'])

    pd_id = 0
    for pd in pds:
        pil_splitter(base_dir+ 'TMP_PIL_IMGS/q6_fw'+str(pd_id)+'.mdt', pd)
        pd_id = pd_id+1

    for iu in ius:
        pil_splitter(base_dir+ 'TMP_PIL_IMGS/iu_fw.mdt', iu)

    if args.license is not None:
        license_file = os.path.abspath(args.license)
        if not os.path.exists(license_file):
            print("Error: license_file {} not found".format(license_file))
            exit(1)
        else:
            RunCmds(['cp ' + license_file + ' ' + base_dir + 'TMP_PIL_IMGS'])

    for img in img_list:
        packCmds = [
            'unsquashfs -d ' + base_dir + 'TMP_IMG ' + base_dir + img,
            'cp -rf ' + base_dir+ 'TMP_PIL_IMGS/* ' + base_dir + 'TMP_IMG',
            squashfs_path + ' ' + base_dir + 'TMP_IMG/ ' + base_dir + 'wifi_fw_tmp.squashfs -nopad -noappend -root-owned -comp xz -Xpreset 9 -Xe -Xlc 0 -Xlp 2 -Xpb 2 -Xbcj arm -b 256k -processors 1',
            'rm -rf ' + base_dir + 'TMP_IMG',
            'rm -rf ' + img,
            'dd if=' + base_dir + 'wifi_fw_tmp.squashfs of=' + base_dir + img + " bs=2k conv=sync",
            'rm -rf ' + base_dir+ 'wifi_fw_tmp.squashfs']
        RunCmds(packCmds)
    RunCmds(['rm -rf ' + base_dir + 'TMP_PIL_IMGS'])
    return img_list

def gen_squashfs_miami_images(pds, base_dir, iu_imgs=None):
    wpds = []
    wius = []
    idx = 0
    wpds.append(pds[0])
    for pd in pds[1:]:
        wpd = wrap_elf_header_in_images(pd, base_dir + 'qdsp6_user_pd'+ str(idx)+ '_wrapped.mbn')
        wpds.append(wpd)
        idx=idx+1

    if args.chipset in ['ipq5332','ipq5322']:
        iu_ddr_addr = miami_iu_ddr_addr
    elif args.chipset in ['ipq5332+qcn9160','ipq5322+qcn9160']:
        iu_ddr_addr = miami_york_iu_ddr_addr

    if iu_imgs is not None:
        for iu in iu_imgs:
            wiu = wrap_elf_header_in_images(iu, base_dir + 'phy_ucode_prod_signed_wrapped.elf', iu_ddr_addr)
            wius.append(wiu)

    imgs = repack_mpd_images(wpds, base_dir, wius)

    for pd in wpds[1:]:
        RunCmds(['rm -rf ' + pd])
    
    for iu in wius:
        RunCmds(['rm -rf ' + iu])

    print("IPQ53xx packed image generated at : {}".format(imgs))

def gne_squashfs_wkk_images():
    print("QCN9224 image generation")
 
#----------------------------------------------------------------------------
# Work based on passed argument
#----------------------------------------------------------------------------
if args.ops in ['combine']:
    if args.base_dir is None:
        print("Error: base_dir not provided")
        exit(1)

    img_out_path = os.path.abspath(args.base_dir) + '/'

    if args.imgs is not None:
        img_files = args.imgs.split(',')
    else:
        print("Error: Input images are not provided. Please provide the images to be combined")
        exit(1)

    for img in img_files[:2]:
        if not os.path.exists(img):
            print("Error: squashfs image {} not found".format(img))
            exit(1)
    
    if len(img_files) != 3:
            print("Error: Invalid no of images, only 3 is accepted, passed:{}".format(img_files))
            exit(1)

    combine_images(img_files[0], img_files[1], img_files[2], img_out_path)
    
    
elif args.chipset in ['ipq5332','ipq5322','ipq5332+qcn9160','ipq5322+qcn9160']:
    '''
    ipq5332 packing
    '''
    #Base directories
    if args.base_dir is None:
        img_path = CURR_DIR + '/../FW_IMAGES/'
    else:
        img_path = os.path.abspath(args.base_dir) + '/'

    if args.oem_pds is not None:
        img_pds = args.oem_pds.split(',')
    elif os.path.exists(img_path+'../BIN'):
        if args.chipset in ['ipq5332','ipq5322']:
            img_pds = [ img_path + '/../BIN/qdsp6_rpd_prod_signed.mbn',
                        img_path + '/../BIN/qdsp6_user_pd0_prod_signed.mbn']
        elif args.chipset in ['ipq5332+qcn9160','ipq5322+qcn9160']:
            img_pds = [ img_path + '/../BIN/qdsp6_rpd_prod_signed.mbn',
                        img_path + '/../BIN/qdsp6_user_pd0_prod_signed.mbn',
                        img_path + '/../BIN/qdsp6_user_pd1.mbn']
            
    if args.iu is not None:
        iu_imgs = args.iu.split(',')
    elif os.path.exists(img_path+'../BIN'):
        iu_imgs = [ img_path + '/../BIN/phy_ucode_prod_signed.elf' ]

    if os.path.exists(img_path):
        print("Generating IPQ53xx packed images")
        
        iu_cnt = 1
        if args.chipset in ['ipq5332','ipq5322']:
            pd_cnt = 2
        elif args.chipset in ['ipq5332+qcn9160','ipq5322+qcn9160']:
            pd_cnt = 3

        for pd in img_pds:
            if not os.path.exists(pd):
                print("Error: PD {} not found".format(pd))
                exit(1)

        for iu in iu_imgs:
            if not os.path.exists(iu):
                print("Error: iu {} not found".format(iu))
                exit(1)

        if len(img_pds) != pd_cnt:
            print("Error: Invalid no of PDs, only {} PDs is accepted, passed:{}".format(pd_cnt, img_pds))
            exit(1)
        if len(iu_imgs) != iu_cnt:
            print("Error: Invalid no of IU, only {} IU is accepted, passed:{}".format(iu_cnt, iu_imgs))
            exit(1)
        gen_squashfs_miami_images(img_pds, img_path, iu_imgs)
    else:
        print("Error: Invalid base path:{}".format(img_path))
        exit(1)
   
elif args.chipset in ['qcn9224']:
    gne_squashfs_wkk_images()
else:
    print("Error: chipset {}, is not supported".format(args.chipset))
