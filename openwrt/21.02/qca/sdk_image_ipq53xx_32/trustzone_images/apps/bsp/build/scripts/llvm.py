#===============================================================================
#
# ARM tool rules
#
# GENERAL DESCRIPTION
#    ARM 5.0 and greater Tools definitions
#
# Copyright (c) 2009-2017 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/apps.tzw/1.0/bsp/build/scripts/llvm.py#1 $
#  $DateTime: 2018/09/03 04:30:12 $
#  $Change: 17027196 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
#03/28/17  pdosi   added support for 3.9 LLVM and minor cleanup.
#06/25/16  pdosi   added the  $LINKFLAGSAN to add flags in between start and end tags
#                  linking line.
#04/19/16  pdosi   added -fno-vectorize-loops for 32 bit images.
#03/04/16  pdosi   added support for QCLD and 3.7 LLVM.
#01/28/14  ygui    add new feature of switch 32bit or 64bit compiler conditionally
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
   
def exists(env):
   return True

def generate(env):

   env.Object.builder.add_emitter(suffix='.s', emitter=S_static_obj_emitter)

   # Standard include paths always searched
   env.Append(CPPPATH = [
      "#",
      ".",
      "#../../build/cust",
      "#../../build/ms",
   ])

   if 'LLVMBIN' in os.environ:
      env.Replace(LLVMBIN = os.environ['LLVMBIN'])
   else:
      print ("-------------------------------------------------------------------------------")
      print ("*** LLVMBIN is not defined on this environment")
      print ("*** Please set up the LLVM enviroment")
      print ("-------------------------------------------------------------------------------")
      raise RuntimeError("LLVM is not defined on this environment")

   if env.PathExists("${LLVMBIN}"):
      LLVMBIN = env.RealPath("${LLVMBIN}", posix=False)
      if env['PLATFORM'] in ['windows', 'win32']:
         LLVMBIN = LLVMBIN + "\\"
      else:
         LLVMBIN = LLVMBIN + "/"
   else:
     raise RuntimeError("${LLVMBIN} not found using system environment.")

   env.Replace(LLVMBIN = os.environ['LLVMBIN'])
   env.Replace(LLVMLIB = os.environ['LLVMLIB'])
   env.Replace(LLVMINC = os.environ['LLVMINC'])
   env.Replace(LLVM32INC = os.environ['LLVM32INC'])
   env.Replace(LLVMTOOLPATH = os.environ['LLVMTOOLPATH'])


   llvmbin=LLVMBIN

   if llvmbin.find("Bullseye")!= -1 :
      llvmver = get_llvm_version(os.environ['BULLSEYE_PATH'])
      linkerPath=os.environ['BULLSEYE_PATH']
   else:
      llvmver=get_llvm_version(llvmbin)
      linkerPath=LLVMBIN
    
   if env['PLATFORM'] in ['windows', 'win32']:
      if 'GNULIBC' in os.environ:
         env.Replace(GNULIBC = os.environ['GNULIBC'])
      else:
         print ("-------------------------------------------------------------------------------")
         print ("*** GNULIBC is not defined on this environment")
         print ("*** Please set up the GNU enviroment")
         print ("-------------------------------------------------------------------------------")
         raise RuntimeError("GNULIBC is not defined on this environment")

      if 'GNUTOOL' in os.environ:
         env.Replace(GNUTOOL = os.environ['GNUTOOL'])
      else:
         print ("-------------------------------------------------------------------------------")
         print ("*** GNUTOOL is not defined on this environment")
         print ("*** Please set up the GNU enviroment")
         print ("-------------------------------------------------------------------------------")
         raise RuntimeError("GNUTOOL is not defined on this environment")

   if 'GNUROOT' in os.environ:
      env.Replace(GNUROOT = os.environ['GNUROOT'])
   if 'GNUARM7' in os.environ:
      env.Replace(GNUARM7 = os.environ['GNUARM7'])
   if 'MUSLPATH' in os.environ:
      env.Replace(MUSLPATH = os.environ['MUSLPATH'])
   if 'MUSL32PATH' in os.environ:
      env.Replace(MUSL32PATH = os.environ['MUSL32PATH'])


   if env['PROC'] == 'A53_64':
      do_64 = True
   else:
      do_64 = False

   llvm_flags_tz=[]
   
   if ARGUMENTS.get('deterministic',0) == 1:
      llvm_flags_tz.append(' -D__TIME__=\\"ss:mm:hh\\"')
      llvm_flags_tz.append(' -D__DATE__=\\"mmmddyyyy\\"')
      llvm_flags_tz.append(' -D__FILE__=\\"${SOURCE.file}\\"')
      llvm_flags_tz.append('-Wno-builtin-macro-redefined')
   

   '''Flags for 3.9.x 64 bit
       -target aarch64-linux-gnu , -mcpu=cortex-a53, -nostdinc, -mno-unaligned-access,-Wno-tautological-constant-out-of-range-compare
     Flags for 3.9.x 32 bit
       -target arm-linux-gnueabi, -mfloat-abi=softfp, -mno-unaligned-access,-Wno-tautological-constant-out-of-range-compare,-mllvm -post-RA-scheduler=false,-mcpu=cortex-a7
   '''
   
   if not llvmver >= (3, 9):
         assert False, "Unsupported LLVM version: {}".format(llvmver)
   
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
        "-mllvm -post-RA-scheduler=false",         
      ])

      env.Replace(AS = env.subst("${LLVMBIN}/clang${EXE_EXT} -target arm-linux-gnu -mfloat-abi=softfp -mcpu=cortex-a8 -x assembler -allow-integrated-as-for-asm-input -c "))
   
   LLVMFlags = " ".join(llvm_flags_tz)
   env.Replace(CC = env.subst("${LLVMBIN}/clang${EXE_EXT} " + LLVMFlags + ' ' + build_dflags + ' '))
   env.Replace(CXX = env.subst("${LLVMBIN}/clang${EXE_EXT} " + LLVMFlags + ' ' + build_dflags + ' '))
      

   # ARM SDT library archiver
   env.Replace(AR = env.subst("${LLVMTOOLPATH}/arm-ar"))
   # Expose the NM tool for scripts to use for extracting symbol/variable
   # names/sizes/other details
   env.Replace(NMBIN = env.subst("${LLVMTOOLPATH}/arm-nm"))
   
   if do_64:
     env.Replace(LINK = linkerPath + "/ld.qcld -march aarch64 -z now ")
   else:
     env.Replace(LINK = linkerPath + "/ld.qcld -march arm -z now ")
   
   env.Replace(NM_FILE = env.RealPath("${BUILD_SCRIPTS_ROOT}/nm.py"))
   env.Replace(NM = "${PYTHONCMD} ${NM_FILE}")
  
   sym_bld1 = Builder(action='${LLVMTOOLPATH}/arm-nm $SOURCE > $TARGET', suffix='.tempsym', src_suffix='.elf')
   sym_bld2 = Builder(action='$NM -f $SOURCE -o $TARGET',suffix='.sym',src_suffix='.tempsym')
   env.Append(BUILDERS = {'_GetSymbol1' : sym_bld1})
   env.Append(BUILDERS = {'_GetSymbol2' : sym_bld2})
   def GetSymbol(env,target, source,*args,**kwargs):
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
   env.Replace(CXXFLAGS =" ")
   env.Replace(ASCPPFLAGS = " ")
   env.Replace(ASMFLAGS = " ")
   env.Replace(ASFLAGS = " ")
   env.Replace(LINKFLAGS = " ")
   env.Replace(ARFLAGS = " -r -c ")
   
   if 'USES_NO_DEBUG' not in env:
       env.Append(CCFLAGS = " -g ")
   
   if ARGUMENTS.get('deterministic',1) == 1:
     env.Append(ARFLAGS= " -D ")

   env.Append(LINKFLAGS = "-static -nostdlib -no-undefined -T ${TARGET.base}.scl -Map ${TARGET.base}.map ")

   if do_64:
     CCCOM = "{} $CCFLAGS  $_CCCOMCOM  -I{} -c $SOURCES -o $TARGET".format(env['CC'],env['LLVMINC'])
     CCPPCOM = "{} $CCFLAGS  $_CCCOMCOM  -I{} -E -xc $SOURCES -o $TARGET".format(env['CC'],env['LLVMINC'])
   else:
     CCCOM =  "{} $CCFLAGS  $_CCCOMCOM  -I{} -c $SOURCES -o $TARGET".format(env['CC'],env['LLVM32INC'])
     CCPPCOM =  "{} $CCFLAGS  $_CCCOMCOM  -I{} -E -xc $SOURCES -o $TARGET".format(env['CC'],env['LLVM32INC'])

   env.Replace(CCCOM =       CCCOM)
   env.Replace(CCCOMCPY =    CCCOM)
   env.Replace(CCPPCOM =     CCPPCOM)
   
   env.Replace(CCPPCLCOM =   "$PPASM -f $SOURCES -o $TARGET")
   env.Replace(CCPPCL_ACOM = "$PPASM -f ${TARGET}.pp -o ${TARGET}.i")

   CXXCOM = "$CXX $CXXFLAGS $CCFLAGS $_CCCOMCOM  -c $SOURCES -o $TARGET"
   env.Replace(CXXCOM =       CXXCOM)
   env.Replace(CXXCOMCPY =    CXXCOM)
   env.Replace(CXXPPCOM =     CXXCOM.replace('-c','-E -xc'))
   env.Replace(CXXMIXCOM =    CXXCOM.replace('-c','-S --asm --interleave'))


   env.Replace(ARCOM =      "$AR $ARFLAGS $TARGET $SOURCES")
   env.Replace(LINKCOM =    "$LINK $LINKFLAGS $_LIBDIRFLAGS --start-group $_LISTFILES $_LIBFLAGS  $SOURCES $LINKFLAGSAN --end-group $LINKFLAGS2 -o $TARGET")

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

   env.Tool('fastcpp', toolpath=[THIS_DIR])
   