import subprocess
import hashlib
import hmac
import os
import shlex

READELF_TEXT_OFFSET_COLUMN = 5
READELF_SYMBOL_OFFSET_COLUMN = 1

def exists(env):
    return True

def generate(env):
    env.AddMethod(fips_add_integrity_hmac, 'FipsAddIntegrityHmac')

    builder = env.Builder(action=add_fips_integrity_hmac)
    env.Append(BUILDERS={'FipsIntegrityHmacBuilder':builder})

#----------------------------------------------------------------------------
# find_symbol_offset
#----------------------------------------------------------------------------
def find_symbol_offset(symbol, output, offset):
    for line in output.split('\n'):
        if symbol in line:
            return int(line.split()[READELF_SYMBOL_OFFSET_COLUMN], 16) + offset
    return -1

#----------------------------------------------------------------------------
# find_text_offset
#----------------------------------------------------------------------------
def find_text_offset(output):
    for line in output.split('\n'):
        if '.text' in line:
            return int(line.split()[READELF_TEXT_OFFSET_COLUMN], 16)
    return -1

#----------------------------------------------------------------------------
# run_readelf
#----------------------------------------------------------------------------
def run_readelf(env, options, file_path):
    readelf_path = env.RealPath('${LLVMBIN}/llvm-readelf')
    command = '{0} {1} {2}'.format(readelf_path, options, file_path)
    return subprocess.check_output(shlex.split(command, posix=False), shell=False)

def is_symbol_missing(integrity_start, integrity_end, hmac_start, hmac_end):
    if -1 not in [integrity_start, integrity_end, hmac_start, hmac_end]:
        return False
    print "WARNING: Could not generate FIPS Integrity HMAC. Missing symbol."
    if integrity_start == -1:
        print "Missing Symbol: Image$$INTEGRITY_MARKER$$Base"
    if integrity_end == -1:
        print "Missing Symbol: Image$$INTEGRITY_MARKER$$Limit"
    if hmac_start == -1:
        print "Missing Symbol: Image$$INTEGRITY_HMAC$$Base"
    if hmac_end == -1:
        print "Missing Symbol: Image$$INTEGRITY_HMAC$$Limit"
    return True

#----------------------------------------------------------------------------
# fips_add_integrity_hmac
#----------------------------------------------------------------------------
def fips_add_integrity_hmac(env, elf_file_path, target_path):
    elf_readelf = run_readelf(env, '-a', elf_file_path)

    offset = find_text_offset(elf_readelf)

    if offset == -1:
        print "WARNING: Could not generate FIPS Integrity HMAC. Could not find loadable section in mbn"
        return

    integrity_start = find_symbol_offset('Image$$INTEGRITY_MARKER$$Base', elf_readelf, offset)
    integrity_end = find_symbol_offset('Image$$INTEGRITY_MARKER$$Limit', elf_readelf, offset)
    hmac_start = find_symbol_offset('Image$$INTEGRITY_HMAC$$Base', elf_readelf, offset)
    hmac_end = find_symbol_offset('Image$$INTEGRITY_HMAC$$Limit', elf_readelf, offset)

    # Could not find a needed symbol in the readelf output
    if is_symbol_missing(integrity_start, integrity_end, hmac_start, hmac_end):
        return

    hmac_key = bytearray(32)

    integrity_hmac = hmac.new(hmac_key, digestmod=hashlib.sha256)
    data = None
    with open(elf_file_path, 'rb') as mbn:
        data = bytearray(mbn.read())
        image_to_hmac = data[integrity_start:integrity_end]
        integrity_hmac.update(image_to_hmac)

    script_dir = os.path.dirname(os.path.abspath(__file__))
    hmac_file_location = os.path.join(script_dir, 'crypto_module_hmac.txt')
    write_option = 'a' if os.path.isfile(hmac_file_location) else 'w+'
    with open(hmac_file_location, write_option) as hmac_file:
        target_file_name = "cmnlib64" if "A53_64" in elf_file_path else "cmnlib32"
        hmac_file.write("{0}: {1}\n".format(target_file_name, integrity_hmac.hexdigest()))

    with open(target_path, 'wb+') as result:
        result_bin = data
        result_bin[hmac_start:hmac_end] = integrity_hmac.digest()
        result.write(result_bin)

def add_fips_integrity_hmac(env, target, source):
    elf = source[0].get_abspath()
    out = target[0].get_abspath()

    env.FipsAddIntegrityHmac(elf, out)

    return None
