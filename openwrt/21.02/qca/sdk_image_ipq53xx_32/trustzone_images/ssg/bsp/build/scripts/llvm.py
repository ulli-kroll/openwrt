#===============================================================================
#
# ARM tool rules
#
# GENERAL DESCRIPTION
#    ARM 5.0 and greater Tools definitions
#
# Copyright (c) 2009-2017, 2019-2020, 2022 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#===============================================================================
from __future__ import print_function
import os
import string
import re
import SCons.Action
from SCons.Script import *

THIS_DIR = os.path.abspath(os.path.dirname(__file__))

# Assembly compile flags (note first pass is through armcc using -E option then passed to armas, see
# build rule below
# standard compile flags
build_dflags = '-DCUST_H=\\"${CUST_H}\\" -DBUILD_TARGET=\\"${BUILD_ID}\\" -DBUILD_VER=\\"${BUILD_VER}\\" -DBUILD_ASIC=\\"${BUILD_ASIC}\\"'

#global variable for regex to compute llvm version.
LLVM_VERSION_REGEX = re.compile("(([0-9]+)\\.([0-9])+(\\.([0-9]+))*)[/\\\\]bin")


def S_static_obj_emitter(target, source, env):
  #import pdb; pdb.set_trace()

  # call defualt static object emitter
  tgt, src = SCons.Defaults.StaticObjectEmitter(target, source, env)

  # add cleaners for assembling side-effects, taht scons can't auto detect
  env.Clean(tgt, "".join([str(tgt[0]), ".i"]))
  env.Clean(tgt, "".join([str(tgt[0]), ".pp"]))


  return (tgt, src)

#------------------------------------------------------------------------------
# Get LLVM version
#------------------------------------------------------------------------------
def get_llvm_version(llvm_bin):
  match = LLVM_VERSION_REGEX.search(llvm_bin)
  if match:
    version = (int(match.group(2)), int(match.group(3)))
  else:
    version = None
  return version

def smart_link(source, target, env, for_signature):
  cpp_suffixes = ['.cpp', '.cc', '.cxx', '.c++', '.C++', '.mm']

  def iscplusplus(source):
    if not source:
      return False
    for s in source:
      if s.sources:
        ext = os.path.splitext(str(s.sources[0]))[1]
        if ext in cpp_suffixes:
          return True
    return False

  if iscplusplus(source):
    return "${LINK} $LINKFLAGS $_LIBDIRFLAGS --start-group $SOURCES $_LISTFILES $_LIBFLAGS -static -lc++ -lc++abi -lunwind --end-group $LINKFLAGSAN $LINKFLAGS2 -o $TARGET"
  else:
    return "${LINK} $LINKFLAGS $_LIBDIRFLAGS --start-group $SOURCES $_LISTFILES $_LIBFLAGS --end-group $LINKFLAGSAN $LINKFLAGS2 -o $TARGET"

def exists(env):
  return True

def generate(env):
  env.AddCommandlineArgument(env.BoolVariable('deterministic','deterministic',0))
  env.Object.builder.add_emitter(suffix='.s', emitter=S_static_obj_emitter)

  env.Append(CPPPATH = [
    ".",
    "${INC_ROOT}/build/cust",
    "${INC_ROOT}/build/ms",
  ])
  for env_var in ['LLVMBIN','LLVMLIB','LLVMINC','LLVM32INC','LLVMTOOLPATH','GNUROOT','GNUARM7','MUSLPATH','MUSL32PATH','GNULIBC','GNUTOOL','LLVMCLANG','LLVMROOT']:
    val = env.get(env_var,os.environ.get(env_var))
    if val is None:
      print ("-------------------------------------------------------------------------------")
      print ("*** {} is not defined on this environment".format(val))
      print ("*** Please set up the build environment")
      print ("-------------------------------------------------------------------------------")
      raise RuntimeError("LLVM is not defined on this environment")
    env[env_var] = val

  llvmver = get_llvm_version(env.subst(env['LLVMBIN']))
  if not llvmver >= (3, 9):
    raise ValueError("Unsupported LLVM version: {}\nLLVM version must be >= 3.9.1".format(llvmver))

  if env['PROC'] == 'A53_64':
    do_64 = True
  else:
    do_64 = False

  llvm_flags_tz=[]

  if env.get('deterministic',0) == 1:
    llvm_flags_tz.append(' -D__TIME__=\\"ss:mm:hh\\"')
    llvm_flags_tz.append(' -D__DATE__=\\"mmmddyyyy\\"')
    llvm_flags_tz.append(' -D__FILE__=\\"${SOURCE.file}\\"')
    llvm_flags_tz.append('-Wno-builtin-macro-redefined')


  '''Flags for 3.9.x 64 bit
     -target aarch64-linux-gnu , -mcpu=cortex-a53, -nostdinc, -mno-unaligned-access,-Wno-tautological-constant-out-of-range-compare
   Flags for 3.9.x 32 bit
     -target arm-linux-gnueabi, -mfloat-abi=softfp, -mno-unaligned-access,-Wno-tautological-constant-out-of-range-compare,-mllvm -post-RA-scheduler=false,-mcpu=cortex-a7
  '''

  if do_64:
    llvm_flags_tz.extend([
         "-target aarch64-linux-gnu",                                # Generate code for the given target.
         "-mcpu=cortex-a53",                                         # Specify CPU.
         "-nostdinc",
         "-mno-unaligned-access",           # Prevent the preprocessor from searching any of the default system header directories.
         "-Wno-tautological-constant-out-of-range-compare",
     ])
    #on windows we only have the assembler for linux-gnu, but on linux we only have it for none-elf???
    #it doesnt matter as the assembler doesnt pull in external symbols that may be in linux-gnu
    #documenting this for posterity
    if env['PLATFORM'] in ['windows', 'win32']:
      env.Replace(AS = env.subst("$GNUROOT/bin/aarch64-linux-gnu-as${EXE_EXT} -c "))
    else:
      env.Replace(AS = env.subst("$GNUROOT/bin/aarch64-none-elf-as${EXE_EXT} -c "))
  else:
    llvm_flags_tz.extend([
      "-mthumb",
      "-target arm-linux-gnueabi",                                   # Generate code for the given target.
      "-mfloat-abi=softfp",                                          # Specifies which floating-point ABI to use.
      "-mno-unaligned-access",                                       # Force all memory accesses to be aligned.
      "-mcpu=cortex-a7",                                          # Specify CPU.
      "-Wno-tautological-constant-out-of-range-compare",          # This is a diagnostic flag by clang.
      #"-mllvm -post-RA-scheduler=false",
    ])

    env.Replace(AS = env.subst("${LLVMBIN}/clang${EXE_EXT} -target armv7-none-eabi -mfloat-abi=softfp -mcpu=cortex-a8 -x assembler -c "))

  LLVMFlags = " ".join(llvm_flags_tz)
  env.Replace(CC = env.subst("${LLVMBIN}/clang${EXE_EXT} " + LLVMFlags + ' ' + build_dflags + ' '))
  env.Replace(CXX = env.subst("${LLVMBIN}/clang++${EXE_EXT} " + LLVMFlags + ' '+ '-std=c++11' + ' ' + build_dflags + ' '))
  env.Replace(LINK='${LLVMBIN}/ld.qcld')

  # ARM SDT library archiver
  env.Replace(AR = env.subst("${LLVMTOOLPATH}/arm-ar"))
  # Expose the NM tool for scripts to use for extracting symbol/variable
  # names/sizes/other details
  env.Replace(NMBIN = env.subst("${LLVMTOOLPATH}/arm-nm"))

  env.Replace(NM_FILE = env.RealPath("${BUILD_SCRIPTS_ROOT}/nm.py"))
  env.Replace(NM = "${PYTHONCMD} ${NM_FILE}")

  sym_bld1 = Builder(action='${LLVMTOOLPATH}/arm-nm $SOURCE > $TARGET', suffix='.tempsym', src_suffix='.elf')
  sym_bld2 = Builder(action='$NM -f $SOURCE -o $TARGET',suffix='.sym',src_suffix='.tempsym')
  env.Append(BUILDERS = {'_GetSymbol1' : sym_bld1})
  env.Append(BUILDERS = {'_GetSymbol2' : sym_bld2})
  def GetSymbol(env, target, source,*args,**kwargs):
   step1 = env._GetSymbol1(source=source,*args,**kwargs)
   return env._GetSymbol2(source=step1, target=target,*args,**kwargs)
  env.AddMethod(GetSymbol,'GetSymbol')

  env.Replace(BINTOOL = "${LLVMTOOLPATH}/arm-elfcopy")

  # Preprocess assembly cleaner
  env.Replace(PPASM_FILE = env.RealPath(os.path.join(THIS_DIR,"ppasm.py")))
  env.Replace(PPASM = "${PYTHONCMD} ${PPASM_FILE} -g ")

  env.Replace(INCPREFIX = "-I")
  env.Replace(CPPDEFPREFIX  = "-D")
  env.Replace(OBJSUFFIX = ".o")

  env.Replace(LIBPREFIX = "")
  env.Replace(LIBSUFFIX = ".lib")
  env.Replace(LIBSUFFIXES = ['.lib', '.a'])
  env.Replace(LIBLINKPREFIX = "")
  env.Replace(LIBDIRPREFIX = "-L ")

  env.Replace(PROGPREFIX = "")
  env.Replace(PROGSUFFIX = ".elf")

  # Init FLAGS
  env.Replace(CCFLAGS=" -Os $CFLAGS $ARMCC_OPT $ARM_OPT_STACK_GUARD ")

  env.Replace(CXXFLAGS ="  -mcpu=cortex-a53 -fno-exceptions -fno-rtti ")
  env.Replace(ASCPPFLAGS = " ")
  env.Replace(ASMFLAGS = " ")
  env.Replace(ASFLAGS = " ")
  env.Replace(LINKFLAGS = " ")
  env.Replace(ARFLAGS = " -r -c ")

  if 'USES_NO_DEBUG' not in env:
    env.Append(CCFLAGS = " -g ")

  env.Append(ARFLAGS= " -D ")

  env.Append(LINKFLAGS = " -z now -nostdlib -no-undefined -T ${TARGET.base}.scl -Map ${TARGET.base}.map ")
  if env.get('verbose') or int(env.GetOption('verbose')):
    env.Append(LINKFLAGS = ' -v ')

  if do_64:
    env.Append(LINKFLAGS='  -march aarch64   -L${LLVMROOT}/aarch64-none-elf/libc/lib -L${LLVMROOT}/aarch64-none-elf/lib ')
    CCCOM = "{} $CCFLAGS  $_CCCOMCOM  -isystem {} -isystem {}/include -c $SOURCES -o $TARGET".format(env['CC'],env['LLVMINC'], env['LLVMCLANG'])
    CCPPCOM = "{} $CCFLAGS  $_CCCOMCOM  -isystem {} -isystem {}/include -E -xc $SOURCES -o $TARGET".format(env['CC'],env['LLVMINC'], env['LLVMCLANG'])

    LLVMCPPINC = "$LLVMROOT/aarch64-none-elf/include/c++/v1"
    CXXCOM = "{} $CXXFLAGS $CCFLAGS $_CCCOMCOM -isystem {} -isystem {}  -c $SOURCES -o $TARGET".format(env['CXX'], LLVMCPPINC, env['LLVMINC'], )
  else:
    env.Append(LINKFLAGS='  -march arm  -L${LLVMROOT}/armv7-none-eabi/libc/lib -L${LLVMROOT}/armv7-none-eabi/lib ')
    CCCOM =  "{} $CCFLAGS  $_CCCOMCOM  -isystem {} -isystem {}/include -c $SOURCES -o $TARGET".format(env['CC'],env['LLVM32INC'], env['LLVMCLANG'])
    CCPPCOM =  "{} $CCFLAGS  $_CCCOMCOM  -isystem {} -isystem {}/include -E -xc $SOURCES -o $TARGET".format(env['CC'],env['LLVM32INC'], env['LLVMCLANG'])

    LLVM32CPPINC = "$LLVMROOT/armv7-none-eabi/include/c++/v1"
    CXXCOM = "{} $CXXFLAGS $CCFLAGS $_CCCOMCOM -isystem {} -isystem {}  -c $SOURCES -o $TARGET".format(env['CXX'], LLVM32CPPINC, env['LLVM32INC'], )

  env.AddCommandlineArgument(env.BoolVariable('disable_werror','disable werror',False))
  env.AddCommandlineArgument(env.BoolVariable('enable_weverything','enable weverything',False))
  if env.get('enable_weverything'):
    env['disable_werror'] = True
  if env.get('disable_werror'):
    CXXCOM += ' -Wno-error '
    CCCOM += ' -Wno-error '
  else:
    CXXCOM += ' -Werror '
    CCCOM += ' -Werror '

  if env.get('enable_weverything'):
    #TODO: maybe get rid of -Wno-reserved-id-macro after using -isystem correctly
    disables = ' -Wno-documentation -Wno-dollar-in-identifier-extension -Wno-reserved-id-macro '
    CXXCOM += ' -Weverything  '
    CCCOM += ' -Weverything '



  env.Replace(CCCOM =       CCCOM)
  env.Replace(CCCOMCPY =    CCCOM)
  env.Replace(CCPPCOM =     CCPPCOM)

  env.Replace(CCPPCLCOM =   "$PPASM -f $SOURCES -o $TARGET")
  env.Replace(CCPPCL_ACOM = "$PPASM -f ${TARGET}.pp -o ${TARGET}.i")


  env.Replace(CXXCOM =       CXXCOM)
  env.Replace(CXXCOMCPY =    CXXCOM)
  env.Replace(CXXPPCOM =     CXXCOM.replace('-c','-E -xc'))
  env.Replace(CXXMIXCOM =    CXXCOM.replace('-c','-S --asm --interleave'))
  env.Replace(CXXPPCLCOM =   env['CCPPCLCOM'])
  env.Replace(CXXPPCL_ACOM = env['CCPPCL_ACOM'])


  env.Replace(ARCOM =    "$AR $ARFLAGS $TARGET $SOURCES")
  env.Replace(LINKCOM1 = smart_link)
  env.Replace(LINKCOM = "$LINKCOM1")

  env.Replace(ASCPPCOM =  "$CC $ASCPPFLAGS $AFLAGS $_CCCOMCOM  -I${LLVMINC} -E -xc $SOURCES -o ${TARGET}.pp")
  env.Replace(ASASCOM =   "$AS $ASFLAGS ${TARGET}.i -o $TARGET")

  AsmPreprocessAction = SCons.Action.Action("$ASCPPCOM", '$ASCPPCOMSTR')
  AsmCleanAction = SCons.Action.Action('$CCPPCL_ACOM', '$CCPPCLCOM_ASTR')
  AsmAssemblyAction = SCons.Action.Action("$ASASCOM", '$ASASCOMSTR')

  env.Replace(ASCOM = [
    AsmPreprocessAction,                   # C pre-process assembly files
    AsmCleanAction,                        # Removed junck generated by C preprocess
    AsmAssemblyAction,                     # Do the actuall assymbly
  ])

  # avoid having a rallib run for no reason, scons defaults does this, but is maybe casing issues
  # as there is no actual ranlibcom
  env.Replace(RANLIBCOM = "")
  env.Replace(RANLIBCOMSTR = "")

  #============================================================================
  # COMMON CC ADDITIONS
  #============================================================================
  comcc_addt = "${BUILD_SCRIPTS_ROOT}/comcc_addt.py"

  if env.PathExists(comcc_addt):
    env.LoadToolScript(comcc_addt)
  if env.PathExists(os.path.join(THIS_DIR,'fastcpp.py')):
    env.Tool('fastcpp', toolpath=[THIS_DIR])
