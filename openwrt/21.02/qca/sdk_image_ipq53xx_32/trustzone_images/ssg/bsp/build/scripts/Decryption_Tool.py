#=============================================================================
# FILE:         Decryption_Tool.py
#
# OVERVIEW:     Get Plaintext of Encrypted TZ/QSEE Logs
#
# DEPENDENCIES: pycryptodome
#
#               Copyright (c) 2020,2022 Qualcomm Technologies, Inc.
#               All Rights Reserved.
#               Qualcomm Technologies Proprietary and Confidential.
#=============================================================================
#=============================================================================
# EDIT HISTORY FOR MODULE
#
# when        who  what, where, why
# ----------  --------  ------------------------------------------------------
# 2022-04-04  as        Integrate script to ssg branch
# 2020-07-09  c_hqayum  Initial revision.
# 2020-07-28  c_hqayum  Added functionality for Encrypted tz log in OCIMEM.
#===========================================================================*/

import re
import os
import struct
import codecs
import argparse

try:
	from Crypto.Hash import SHA256
	from Crypto.PublicKey import RSA
	from Crypto.Cipher import PKCS1_OAEP, AES
except:
	raise Exception("Pycryptodome is required. Please install this library.")


#========================================================
# All Macros
#========================================================
SHARED_IMEM = 0x146BF000
START_OF_OCIMEM = 0x14680000
TZ_DIAG_BASEPTR = 0x146BF720
OFFSET_FOR_TZ_DIAG_BASE_POINTER = TZ_DIAG_BASEPTR - START_OF_OCIMEM
TZ_DIAG_ENC_COOKIE_ADDRESS = 0x146BF80C
OFFSET_OF_COOKIE = TZ_DIAG_ENC_COOKIE_ADDRESS - START_OF_OCIMEM
SUCCESS_COOKIE = 0x747A0101

TZBSP_AES_256_ENCRYPTED_KEY_SIZE = 256
TZBSP_LOG_ENCR_NONCE_LEN = 12
TZBSP_LOG_ENCR_TAG_LEN = 16

#========================================================
# TZBSP Diag structures
#========================================================
tzbsp_log_pos_s = "".join([
	"I",	# /* Ring buffer wrap-around ctr */ uint32_t wrap;

	"I"		# /* Ring buffer current position */ uint32_t offset;
	])

tzbsp_diag_s ="".join ([
	"I",	# Magic Number  uint32 magic_num;

	"I",	# Major.Minor version uint32 version;

	"I",	# Number of CPU's uint32 cpu_count;

	"I",	# Offset of VMID Table uint32 vmid_info_off;

	"I",	# Offset of Boot Table uint32 boot_info_off;

	"I",	# Offset of Reset Table uint32 reset_info_off;

	"I",	# Offset of Interrupt Table uint32 int_info_off;

	"I",	# Ring Buffer Offset uint32 ring_off;

	"I",	# Ring Buffer Len uint32 ring_len;

	"I",	# Offset for Wakeup info uint32 wakeup_info_off;

	])



class Encrypted:
	def __init__(self, magic='', version='', log_size='', wrap='', enc_key='', nonce='', tag='',  logs=''):
		self.magic = magic
		self.version = version
		self.log_size = log_size
		self.wrap = wrap
		self.enc_key = enc_key
		self.nonce = nonce
		self.tag = tag
		self.logs = logs

	# Parses OCIMEM and collects needed arguments in class instance variables
	def parse_OCIMEM(self, infile):
		with open(infile, "rb") as file:
			file.seek(OFFSET_OF_COOKIE)
			enc_cookie = struct.unpack('<I', file.read(4))[0]
			if enc_cookie != SUCCESS_COOKIE:
				raise Exception("Invalid TZ Diag Encrypt Cookie. Retrieved {}, should be {}".format(hex(enc_cookie), hex(SUCCESS_COOKIE)))

			file.seek(OFFSET_FOR_TZ_DIAG_BASE_POINTER)
			TZDiagAddr = (struct.unpack('<I', file.read(4)))[0] - START_OF_OCIMEM # 0x16000

			file.seek(TZDiagAddr)
			g_tzbsp_diag = struct.unpack(tzbsp_diag_s, file.read(struct.calcsize(tzbsp_diag_s)))

			self.magic = hex(g_tzbsp_diag[0])
			self.version = hex(g_tzbsp_diag[1])
			self.log_size = hex(g_tzbsp_diag[8])

			RING_BUFFER_OFFSET = g_tzbsp_diag[7]
			BEFORE_LOGS = TZBSP_AES_256_ENCRYPTED_KEY_SIZE + TZBSP_LOG_ENCR_NONCE_LEN + TZBSP_LOG_ENCR_TAG_LEN + struct.calcsize(tzbsp_log_pos_s)
			
			file.seek(TZDiagAddr + RING_BUFFER_OFFSET - BEFORE_LOGS)
			self.enc_key = file.read(TZBSP_AES_256_ENCRYPTED_KEY_SIZE).hex()
			self.nonce = file.read(TZBSP_LOG_ENCR_NONCE_LEN).hex()
			self.tag = file.read(TZBSP_LOG_ENCR_TAG_LEN).hex()

			file.seek(TZDiagAddr + RING_BUFFER_OFFSET)
			self.logs = file.read(int(self.log_size, 16)).hex()


	# Parses Encrypted Logs and collects the needed arguments in class instance variables
	def parse_encrypt(self, enc_file):
		with open(enc_file, "r") as file:
			fileString = file.read()

			self.magic = self.parse_helper("Magic.*\n(.*)", fileString)
			self.version = self.parse_helper("Ver.*\n(.*)", fileString)
			self.log_size = self.parse_helper(".*Size.*\n(.*)", fileString)
			self.wrap = self.parse_helper("Wrap.*\n(.*)", fileString)
			self.nonce = re.search("Nonce.*\n(.*)", fileString).group(1)
			self.tag = re.search("Tag.*\n(.*)", fileString).group(1)

			self.enc_key = re.search("Key.*\n(.*\n.*\n.*\n.*\n.*\n.*\n.*\n.*)", fileString).group(1)
			self.logs = re.search("Log :(.*)", fileString, re.DOTALL).group(1)

			self.enc_key = self.remove_whitespace(self.enc_key)
			self.logs = self.remove_whitespace(self.logs)
			self.nonce = self.remove_whitespace(self.nonce)
			self.tag = self.remove_whitespace(self.tag)

	# Regex grep in message, if found return it, else return default empty string
	def parse_helper(self, grep, message):
		parsed = re.search(grep, message)
		if parsed: return parsed.group(1)
		return ''

	# Removes whitespace for Class Instance Variables
	def remove_whitespace(self, var):
		return re.sub(" *\n*", '', var)

	# Prints the class instance variables (Used for debugging)
	def print_debug_info(self):
		#print_blue("\nDebug Output:\n")
		print_blue("Magic_Number:")
		print(self.magic)
		print_blue("\nVersion:")
		print(self.version)
		print_blue("\nLog Size(s):")
		print(self.log_size)
		print_blue("\nWrap_Count:")
		print(self.wrap)
		print_blue("\nKey:")
		print(self.enc_key)
		print_blue("\nNonce:")
		print(self.nonce)
		print_blue("\nTag:")
		print(self.tag)
		print_blue("\nLogs:")
		print(self.logs)
		print()
		# print("Key:\n{}\n".format(self.enc_key))
		# print("Nonce:\n{}\n".format(self.nonce))
		# print("Tag:\n{}\n".format(self.tag))
		# print("Log Sizes:\n{}\n".format(self.log_size))
		# print("Logs:\n{}\n".format(self.logs))


# Get Private key for RSA Decryption
def get_key(key_fname):
	with open(key_fname, "r") as file:
		return RSA.importKey(file.read())


# Decrypts the Encrypted Wrapped Key with the Private Key
def rsa_decrypt(priv_key, encrypted_key):
	cipher = PKCS1_OAEP.new(priv_key, hashAlgo=SHA256.new())
	return cipher.decrypt(encrypted_key)


# Decrypts Logs with the Decrypted Wrapped Key and Nonce, and verifies the Tags
def gcm_decrypt(dec_key, nonce, tag, log, outPath):
	cipher = AES.new(dec_key, AES.MODE_GCM, nonce=codecs.decode(nonce, "hex_codec"))
	plaintext = cipher.decrypt(codecs.decode(log, "hex_codec"))

	pt = codecs.decode(plaintext, "UTF-8")
	with open(outPath, "w") as file:
		file.write(pt)

	flag = True
	try:
		cipher.verify(codecs.decode(tag, "hex_codec"))
	except:
		flag = False
	return (pt, flag)


# Gets info of key (Used for debugging)
def print_info(k):
	print("Length =", len(k))
	print("Type   =", type(k))
	print("Data   =", k)
	print()


# Prints text in red
def print_red(myStr):
	os.system('')
	print("\033[91m{}\033[00m".format(myStr))


# Prints text in green
def print_green(myStr):
	os.system('')
	print("\033[92m{}\033[00m".format(myStr))


# Prints text in green
def print_blue(myStr):
	os.system('')
	print("\033[96m{}\033[00m".format(myStr))



if __name__ == "__main__":

	# Parse Command line arguments
	parser = argparse.ArgumentParser(
		formatter_class=argparse.RawTextHelpFormatter,
		description="Must have pycryptodome installed (pip3 install pycryptodome)\n")

	optional = parser._action_groups.pop()
	required = parser.add_argument_group("required arguments")
	required.add_argument("-f", "--file_path", help="Path to File", type=str, required=True)
	required.add_argument("-t", "--type", help="File Type (Must pick valid option from list of choices)", type=str, choices=["tz_log", "qsee_log", "OCIMEM"], required=True)
	required.add_argument("-pk", "--private_key_path", help="Path to Private Key", type=str, required=True)
	optional.add_argument("-o", "--output_dir", help="directory to store output files. DEFAULT: current working directory", type=str, default=os.getcwd())
	optional.add_argument("-d", "--debug_output", help="Prints info for debugging purposes", action="store_true")
	parser._action_groups.append(optional)
	args = parser.parse_args()

	# file to write output to
	out_file = ''
	if args.type == "tz_log":
		out_file = "tz_dec.log"
	elif args.type == "qsee_log":
		out_file = "qsee_dec.log"
	else:
		out_file = "tz_logs.log"

	# Validate output directory
	if os.path.isdir(args.output_dir) == False:
		try:
			os.makedirs(args.output_dir)
		except:
			raise Exception("Given invalid Output Directory Path, cannot create path.")

	# Class object
	Encrypted_Logs = Encrypted()

	# Parses the Encrypted Log File for the information needed
	if args.type == "OCIMEM":
		Encrypted_Logs.parse_OCIMEM(args.file_path)
	else:
		Encrypted_Logs.parse_encrypt(args.file_path)
	
	# Get Private Key for RSA Decryption
	private_key = get_key(args.private_key_path)

	# Decode Encrypted Key to Byte String
	encrypted_log_key = codecs.decode(Encrypted_Logs.enc_key, "hex_codec")
	
	# Decrypt the Encrypted Key with the Private Key
	decrypted_key = rsa_decrypt(private_key, encrypted_log_key)

	# Decrypt the Logs and check if tag is verified
	dec_result = gcm_decrypt(decrypted_key, Encrypted_Logs.nonce, Encrypted_Logs.tag, Encrypted_Logs.logs, os.path.join(args.output_dir, out_file))

	# For debug option enabled
	if args.debug_output:
		Encrypted_Logs.print_debug_info()
		print_blue("\nEncrypted Wrapped Key Info:")
		print_info(encrypted_log_key)
		print_blue("\nDecrypted Wrapped Key Info:")
		print_info(decrypted_key)
		print_blue("\nPlaintext:")
		print(dec_result[0])

	# Prints results to stdout
	if dec_result[1]:
		print_green("\nTag is Verified, Message is Authentic!")
	else:
		print_red("\nIncorrect Key or Corrupted Message!")

