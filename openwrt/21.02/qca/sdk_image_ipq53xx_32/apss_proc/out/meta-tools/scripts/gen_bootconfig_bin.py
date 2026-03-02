#!/usr/bin/python
# ===========================================================================
#Copyright (c) 2017 Qualcomm Technologies, Inc.
#All Rights Reserved.
#Confidential and Proprietary - Qualcomm Technologies, Inc.
# ===========================================================================

import xml.etree.ElementTree as ET
import itertools
import os
import subprocess
import struct, sys
import shutil
from getopt import getopt
from getopt import GetoptError

cdir = os.path.dirname("")
cdir = os.path.abspath(cdir)
outputdir = ""

def write_bin_file(filename, data):
    fp = open(filename, 'wb')
    # Add padding to make data 4-byte aligned
    padding = (len(data)/2)%4
    if padding!=0:
        data = data + '00'*(4 - padding)
    for i in range(0, len(data), 2):
        data_item = data[i:i+2]
        fp.write(chr(int(data_item, 16)))
    fp.close()

def generate_try_mode_bin(filepath, age):
	start_magic = 'a0a1a2a3'
	try_magic = 'a1a1a2a3'
	fp = open(filepath, 'rb')
	data = fp.read().encode('hex')
	for i in range(0,len(data),8):
		if data[i:i+8] == start_magic:
			data = data[:i]+try_magic+data[i+8:]
			data = data[:9] + age + data[10:]
			write_bin_file(filepath,data)
			fp.close()
			break

def process_bootconfig(config_file):
	global mbn_gen
	global outputdir
	global cdir

        tree = ET.parse(config_file)
        root = tree.getroot()

	arch = root.find(".//data[@type='ARCH']/SOC")
	ARCH_NAME = str(arch.text)

	if ARCH_NAME == "ipq5018" or ARCH_NAME == "ipq5332":
		# IPQ5018 and IPQ5332 flash optimization needs bootconfig.bin to fit in 64KB
		# Hence using NOR params - blocksize will be 64 KB
		nand_param = root.find(".//data[@type='NOR_PARAMETER']")
		nand_pagesize = int(nand_param.find('page_size').text)
		nand_pages_per_block = int(nand_param.find('pages_per_block').text)
	else:
		if root.find(".//data[@type='NAND_PARAMETER']/entry") != None:
			entries = root.findall("./data[@type='NAND_PARAMETER']/entry")[:1]
			for entry in entries:
				nand_pagesize = int(entry.find(".//page_size").text)
				nand_pages_per_block = int(entry.find(".//pages_per_block").text)
		else:
			nand_param = root.find(".//data[@type='NAND_PARAMETER']")
			nand_pagesize = int(nand_param.find('page_size').text)
			nand_pages_per_block = int(nand_param.find('pages_per_block').text)

	bootconfig_dir = "$$/" + ARCH_NAME + "/bootconfig/"
	bootconfig_dir = bootconfig_dir.replace('$$', cdir)

	mbn_gen = '$$/scripts/nand_mbn_generator.py'
        mbn_gen = mbn_gen.replace('$$', cdir)

	print '\tNand page size: ' + str(nand_pagesize) + ', pages/block: ' \
		+ str(nand_pages_per_block)
	print '\tBootconfig dir: ' + bootconfig_dir

# Create user partition
	nand_raw_bootconfig='nand_raw_bootconfig.bin'

	files = os.listdir(bootconfig_dir)
	for file in files:
		print '\tCreating raw partition'
		prc = subprocess.Popen(['python', mbn_gen, bootconfig_dir + file,
					nand_raw_bootconfig], cwd=outputdir)
		prc.wait()
		if prc.returncode != 0:
			print 'ERROR: unable to create raw partition'
			return prc.returncode
		else:
			print '...Raw partition created'

		bootconfig_gen = outputdir + '/bootconfig_tool'
		os.chmod(bootconfig_gen, 0744)
		rawpart_path = os.path.join(outputdir, nand_raw_bootconfig)
		nand_bootconfig = outputdir + '/' + os.path.splitext(file)[0] + '.bin'

		# Create nand bootconfig
		print '\tCreating Nand bootconfig',
		prc = subprocess.Popen([
				bootconfig_gen,
				'-s',
				str(nand_pagesize),
				'-p',
				str(nand_pages_per_block),
				'-i',
				rawpart_path,
				'-o',
				nand_bootconfig,
				], cwd=outputdir)
		prc.wait()
		if prc.returncode != 0:
			print 'ERROR: unable to create system partition'
			return prc.returncode
		else:
			print '...Bootconfig created\n\n'

			try_bootconfig_0 = outputdir + '/' + 'bootconfig0-try.bin'
			try_bootconfig_1 = outputdir + '/' + 'bootconfig1-try.bin'

			shutil.copy(nand_bootconfig, try_bootconfig_0)
			shutil.copy(nand_bootconfig, try_bootconfig_1)

			generate_try_mode_bin(try_bootconfig_0, '1')
			generate_try_mode_bin(try_bootconfig_1, '0')

			print '\n...Bootconfig created\n\n'

	return 0

def main():
	global bootconfig_gen
	global outputdir
	if len(sys.argv) > 1:
		try:
			opts, args = getopt(sys.argv[1:], "c:o:")
		except GetoptError, e:
			print "config file and bootconfig type are needed to generate bootconfig binaries"
			raise
		for option, value in opts:
			if option == "-c":
				config_file = value
			elif option == "-o":
				outputdir = value
	else:
		print "Error: config file and output path are needed to generate bootconfig binaries\n"
		return -1

	if process_bootconfig(config_file) < 0:
			return -1


if __name__ == '__main__':
    main()
