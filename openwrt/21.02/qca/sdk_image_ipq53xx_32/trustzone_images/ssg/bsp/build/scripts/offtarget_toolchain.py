# Copyright (c) 2017 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR

import os
import string
import re
import sys
import SCons.Action
from SCons.Script import *

def exists(env):
   return True

def generate(env):
  env.AddMethod(do_linecoverage,'DoLineCoverage')
  #'PROC' must be in the env at this point
  oldProc = env['PROC']
  if oldProc == 'scorpion':
    env['PROC'] = 'x86-32'
  elif oldProc == 'A53_64':
    env['PROC'] = 'x86-64'
    
  if env['PROC'] == 'x86-32':
    tools_prefix = '/pkg/ssg/toolchain32'
    dynamic_linker = '${OFFTARGET_TOOLS_PREFIX}/lib/i386-linux-gnu/ld-linux.so.2'
    ld_library_path = [
      '${OFFTARGET_TOOLS_PREFIX}/usr/lib/i386-linux-gnu/',
      '${OFFTARGET_TOOLS_PREFIX}/lib/i386-linux-gnu/',
      '${OFFTARGET_TOOLS_PREFIX}/lib',
      '${OFFTARGET_TOOLS_PREFIX}/usr/lib/',
      '${OFFTARGET_TOOLS_PREFIX}/usr/lib/gcc/i686-linux-gnu/7',
    ]
    inc_paths = [
      '${OFFTARGET_TOOLS_PREFIX}/usr/include',
      '${OFFTARGET_TOOLS_PREFIX}/usr/include/c++/7.3.0',
      '${OFFTARGET_TOOLS_PREFIX}/usr/include/c++/7.3.0/backward',
      '${OFFTARGET_TOOLS_PREFIX}/usr/include/i386-linux-gnu',
      '${OFFTARGET_TOOLS_PREFIX}/usr/include/i386-linux-gnu/c++/7.3.0',
      '${OFFTARGET_TOOLS_PREFIX}/usr/lib/llvm-6.0/lib/clang/6.0.0/include',
      '${OFFTARGET_TOOLS_PREFIX}/usr/local/include',
    ]
  else:
    tools_prefix = '/pkg/ssg/toolchain64'
    dynamic_linker = '${OFFTARGET_TOOLS_PREFIX}/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2'
    ld_library_path = [
      '${OFFTARGET_TOOLS_PREFIX}/usr/lib/x86_64-linux-gnu/',
      '${OFFTARGET_TOOLS_PREFIX}/lib/x86_64-linux-gnu/',
      '${OFFTARGET_TOOLS_PREFIX}/lib',
      '${OFFTARGET_TOOLS_PREFIX}/usr/lib/',
      '${OFFTARGET_TOOLS_PREFIX}/usr/lib/gcc/x86_64-linux-gnu/7',
    ]
    inc_paths = [
      '${OFFTARGET_TOOLS_PREFIX}/usr/include',
      '${OFFTARGET_TOOLS_PREFIX}/usr/include/c++/7.3.0',
      '${OFFTARGET_TOOLS_PREFIX}/usr/include/c++/7.3.0/backward',
      '${OFFTARGET_TOOLS_PREFIX}/usr/include/x86_64-linux-gnu',
      '${OFFTARGET_TOOLS_PREFIX}/usr/include/x86_64-linux-gnu/c++/7.3.0',
      '${OFFTARGET_TOOLS_PREFIX}/usr/lib/llvm-6.0/lib/clang/6.0.0/include',
      '${OFFTARGET_TOOLS_PREFIX}/usr/local/include',
    ]
  ld_library_path_str = 'LD_LIBRARY_PATH="' + ':'.join(ld_library_path) + '"  '
  env['OFFTARGET_TOOLS_PREFIX'] = tools_prefix
  env['OFFTARGET_DYNAMIC_LD'] = dynamic_linker
  
  tools_bin_dir = [os.path.join(tools_prefix,'bin'), os.path.join(tools_prefix,'usr','bin')]
  

  """ To fix the CCCOMSTR etc that use BUILDPATH to display the name of the file
      being compiled.  Without this it will display the old PROC value such as
      scorpion instead of 'x86-32'
      This should be fixed by changing config.py (maybe others too) to not use
      env.subst() - but I have no idea if that breaks everything or not, so for now...
  """
  buildpath = env.get('BUILDPATH',oldProc)
  env.Replace(BUILDPATH = buildpath.replace(oldProc, env['PROC']))

  if not env.StandaloneSdk():
    env.Append(CPPPATH = [
      "${BUILD_ROOT}/build/cust",
      "${BUILD_ROOT}/build/ms",
    ])

  #os.environ['PATH'] is a string, where paths are separated by ':' (linux) and ';' (windows)
  if sys.platform.startswith('win'):
    separator=';'
  elif sys.platform.startswith('linux'):
    separator=':'

  search_paths = os.environ['PATH'].split(separator)
  if ARGUMENTS.get('use_local_clang'):
    clang_search_paths = search_paths + tools_bin_dir
  else:
    clang_search_paths = tools_bin_dir + search_paths

  gcc_search_paths   = ['/pkg/qct/software/gnu/gcc/7.2.0/bin'] + search_paths
  #filter the ARM llvm path out of our search paths
  clang_search_paths = filter(lambda x: not x.startswith('/pkg/qct/software/llvm/release/arm/'), clang_search_paths)
  if env.WhereIs('clang', path=clang_search_paths):
    env.Replace(CC        = env.WhereIs("clang",       path = clang_search_paths))
    env.Replace(CXX       = env.WhereIs("clang++",       path = clang_search_paths))
    env.Replace(AR        = env.WhereIs("llvm-ar",     path = clang_search_paths))
    env.Replace(NMBIN     = env.WhereIs("llvm-nm",     path = clang_search_paths))
    env.Replace(LINK      = env.WhereIs("clang++",      path = clang_search_paths))
    env.Replace(RANLIB    = env.WhereIs("llvm-ranlib", path = clang_search_paths))
    env.Replace(AS        = env.WhereIs("llvm-as",     path = clang_search_paths))
    env.Replace(COV       = env.WhereIs("llvm-cov",     path = clang_search_paths))
    env.Replace(PROFDATA  = env.WhereIs("llvm-profdata",     path = clang_search_paths))
  elif env.WhereIs('gcc', path=gcc_search_paths):
    env.Replace(CC        = env.WhereIs("gcc",    path = gcc_search_paths))
    env.Replace(CXX        = env.WhereIs("g++",    path = gcc_search_paths))
    env.Replace(AR        = env.WhereIs("ar",     path = gcc_search_paths))
    env.Replace(NMBIN     = env.WhereIs("nm",     path = gcc_search_paths))
    #env.Replace(LINK      = env.WhereIs("",     path = gcc_search_paths))
    env.Replace(RANLIB    = env.WhereIs("ranlib", path = gcc_search_paths))
    env.Replace(AS        = env.WhereIs("as",     path = gcc_search_paths))
  else:
    print "ERROR: cannot find a suitable x86-64 compiler in $PATH! Did you install a clang+llvm 4.0.0+ toolchain?"
    sys.exit(1)

  #XXX: klockwork doesnt properly support sysroot and gcc-toolchain and promptly dies because it can't find include
  #paths, so we'll add them all manually for now
  
  #We assume that, if one offtarget executable comes from /pkg/ssg/..., then they all do
  if env['CC'].startswith(tools_prefix):
    env['LD_LIBRARY_PATH_STR'] = ld_library_path_str
    env.Append(CXX=' --gcc-toolchain=${OFFTARGET_TOOLS_PREFIX}/usr/ --sysroot=${OFFTARGET_TOOLS_PREFIX} ')
    for p in inc_paths:
      env.Append(CXX=' -I' + p)
    
    env.Append(CC=' --gcc-toolchain=${OFFTARGET_TOOLS_PREFIX}/usr/ --sysroot=${OFFTARGET_TOOLS_PREFIX} -lstdc++ -lstdc++abi ')
    for p in inc_paths:
      if 'c++' not in p:
        env.Append(CC=' -I' + p)

    env.Append(LINK= '  --gcc-toolchain=${OFFTARGET_TOOLS_PREFIX}/usr/ --sysroot=${OFFTARGET_TOOLS_PREFIX} -Wl,--dynamic-linker,${OFFTARGET_DYNAMIC_LD} ')#
    for path in ld_library_path:
      env.Append(LINK=" -L{0} -Wl,-rpath,{0} ".format(path))

    #Klockwork messes with the LD_LIBRARY_PATH, which kills the offtarget toolchain. So append our LD_LIBRARY_PATH here so that 
    #it occurs after the Klockwork LD_LIBRARY_PATH changes in order for our offtarget toolchain to actually work
    for env_var in ['CC','CXX','AR','NMBIN','RANLIB','AS','LINK','COV','PROFDATA',]:
      env[env_var] = ld_library_path_str + env[env_var]
     
  for env_var in ['CC','CXX','AR','NMBIN','RANLIB','AS','LINK']:
    if not env.get(env_var):
      print "ERROR: no executable found for offtarget {} in $PATH. Did you install a 4.0.0+ x86-64 clang+llvm toolchain?".format(env_var)
      sys.exit(1)
      
  if env.get('COV') and env.get('PROFDATA'):
    env['DO_LINE_COVERAGE'] = True
     
  print 'cc',env['CC']
  
  #TODO: are these necessary?
  env.Replace(NM = "${PYTHONCMD} ${BUILD_SCRIPTS_ROOT}/nm.py")
  env.Replace(PPASM = "${PYTHONCMD} ${BUILD_SCRIPTS_ROOT}/ppasm.py -g ")

  env.Replace(INCPREFIX = "-I")
  env.Replace(CPPDEFPREFIX  = "-D")
  env.Replace(OBJSUFFIX = ".o")

  env.Replace(SHLIBPREFIX = "lib")
  env.Replace(SHLIBSUFFIX = ".so")

  env.Replace(LIBPREFIX = "")
  env.Replace(LIBSUFFIX = ".lib")
  env.Replace(LIBSUFFIXES = ['$LIBSUFFIX', '.l', '.a', '$SHLIBSUFFIX'])
  env.Replace(LIBLINKPREFIX = "-l")
  env.Replace(DO_NOT_USE_LIBLINKSUFFIX = "")
  env.Replace(LIBDIRPREFIX = "-L ")
  env.Replace(PROGSUFFIX='.elf')
  env.Replace(LINKCOM='$LINK -o $TARGET $LINKFLAGS $SOURCES $_LIBDIRFLAGS $_LIBFLAGS   ')
  env.Replace(LINKFLAGS="")
  if env['PLATFORM'] in ['windows', 'win32']:
    env.Replace(MAXLINELENGTH = 32000)
  else:
    env.Replace(MAXLINELENGTH = 100000)

  env.Replace(TEMPFILEPREFIX = "@")
  env.Replace(TEMPFILESUFFIX = "")

  #"char" type is unsigned on ARM but signed on x86, so force unsigned for x86
  env.Append(CCFLAGS = " -funsigned-char ")
  env.Append(CFLAGS=" ")
  env.Replace(CXXFLAGS = " ")
  env.Replace(ASCPPFLAGS = " ")
  env.Replace(ASFLAGS = " ")
  env.Replace(ARFLAGS = " rcD  ")
  env.Replace(CPPDEFINES=[])
  env.Append(CPPDEFINES='OFFTARGET')
  if env['PROC'] == 'x86-32':
    env.Append(CCFLAGS= ' -m32 ')
    env.Append(LINKFLAGS= ' -m32 ')
  else:
    env.Append(CCFLAGS= ' -m64 ')
    env.Append(LINKFLAGS= ' -m64  ')

  env.Append(CCFLAGS=' -DCUST_H=\\"${CUST_H}\\"  ')
  

  CCCOM = "$CC $CFLAGS $CCFLAGS $_CCCOMCOM -c $SOURCES -o $TARGET -Wno-unused-command-line-argument "

  env.Replace(CCCOM =       CCCOM)
  env.Replace(CCPPCOM =     CCCOM.replace('-c','-E -xc', 1))
  env.Replace(CCCOMCPY =    CCCOM)
  env.Replace(CCPPCLCOM =   "$PPASM -f $SOURCES -o $TARGET")
  env.Replace(CCPPCL_ACOM = "$PPASM -f ${TARGET}.pp -o ${TARGET}.i")

  CXXCOM = "$CXX $CXXFLAGS $CCFLAGS $_CCCOMCOM  -c $SOURCES -o $TARGET -Wno-unused-command-line-argument "
  env.Replace(CXXCOM =       CXXCOM)
  env.Replace(CXXCOMCPY =    CXXCOM)
  env.Replace(CXXPPCOM =     CXXCOM.replace('-c','-E -xc++', 1))
  env.Replace(CXXPPCLCOM =   env['CCPPCLCOM'])
  env.Replace(CXXPPCL_ACOM = env['CCPPCL_ACOM'])

  env.Replace(ARCOM =      "$AR $ARFLAGS $TARGET $SOURCES")

  env.Replace(ASCPPCOM =  "$CC $ASCPPFLAGS $_CCCOMCOM -E -xc $SOURCES -o ${TARGET}.pp")
  env.Replace(ASASCOM =   "$AS $ASFLAGS ${TARGET}.i -o $TARGET")

  AsmPreprocessAction = SCons.Action.Action('$ASCPPCOM', '$ASCPPCOMSTR')
  AsmCleanAction = SCons.Action.Action('$CCPPCL_ACOM', '$CCPPCLCOM_ASTR')
  AsmAssemblyAction = SCons.Action.Action('$ASASCOM', '$ASASCOMSTR')

  env.Replace(ASCOM = [
    AsmPreprocessAction,                   # C pre-process assembly files
    AsmCleanAction,                        # Removed junck generated by C preprocess
    AsmAssemblyAction,                     # Do the actuall assymbly
  ])

  if env.StandaloneSdk():
    env.Tool('comcc_addt',toolpath=['$SDK_ROOT/scripts'])
  else:
    env.LoadToolScript("${BUILD_SCRIPTS_ROOT}/comcc_addt.py")


  #fastcpp may not exist if using system scons
  try:
    env.Tool('fastcpp')
  except:
    pass

  if ARGUMENTS.get('OFFTARGET_DEBUG'):
    env.Append(CCFLAGS=' -g -Og ')
  elif ARGUMENTS.get('OFFTARGET_RELEASE'):
    print "offtarget_release"
    env.Append(CCFLAGS=' -g0 -O3 ')
    env.Append(LINKFLAGS=' -s -Wl,-s ')
    env.Append(CCCOM=' -fno-function-sections -fno-data-sections ')
    env.Append(CXXCOM=' -fno-function-sections -fno-data-sections ')
  else:
    #Allow debugging for all symbols when we didn't explicitly set release or debugging
    #release is always set for external versions of the SDK, so this is ok
    env.Append(CCFLAGS= ' -g ')
  
  if ARGUMENTS.get('OFFTARGET_WEVERYTHING'):
    env.Prepend(CCFLAGS=' -Wno-error -Weverything -pedantic -Wno-documentation \
    -Wno-gnu-zero-variadic-macro-arguments -Wno-unused-parameter -Wno-unused-macros -Wno-missing-prototypes -Wno-reserved-id-macro \
    -Wno-newline-eof -Wno-documentation-unknown-command -Wno-dollar-in-identifier-extension -Wno-unused-variable \
    -Wno-strict-prototypes    ')
    
  #if ARGUMENTS.get('OFFTARGET_ANALYZE'):
  #  env.Append(CCFLAGS=' --analyze --analyzer-output html ')
    
  if env.DoLineCoverage():
    env.Append(CCFLAGS=' -fprofile-instr-generate -fcoverage-mapping ')
    env.Append(LINKFLAGS=' -fprofile-instr-generate -fcoverage-mapping ')

  using_sanitizer=False
  if env['PROC'] == 'x86-64':
    if ARGUMENTS.get('OFFTARGET_ASAN'):
      env.Append(CCFLAGS=' -fsanitize=address -fsanitize-recover=address  ')
      env.Append(LINKFLAGS=' -fsanitize=address -fsanitize-recover=address  ')
      using_sanitizer=True
    elif ARGUMENTS.get('OFFTARGET_UBSAN'):
      env.Append(CCFLAGS=' -fsanitize=undefined -g -fsanitize-recover=undefined ')
      env.Append(LINKFLAGS=' -fsanitize=undefined -fsanitize-recover=undefined ')
      using_sanitizer=True
    elif ARGUMENTS.get('OFFTARGET_MSAN'):
      env.Append(CCFLAGS=' -fsanitize=memory -fPIE -fsanitize-recover=memory ')
      env.Append(LINKFLAGS=' -fsanitize=memory -fPIE -fsanitize-recover=memory ')
      using_sanitizer=True
    elif ARGUMENTS.get('OFFTARGET_THREADSAN'):
      env.Append(CCFLAGS=' -fsanitize=thread -fPIE -fsanitize-recover=thread ')
      env.Append(LINKFLAGS=' -fsanitize=thread -fPIE -fsanitize-recover=thread ')
      using_sanitizer=True
  if not using_sanitizer:
    env.Append(LINKFLAGS=' -Wl,-z,defs ')
  else:
    env.Append(CCFLAGS=' -fno-omit-frame-pointer -g  -Og -fsanitize-recover=all ')
    env.Append(LINKFLAGS=' -fsanitize-recover=all ')



def do_linecoverage(env):
  #we only do line coverage for 64 bit as the standard 64 bit clang binary distribution
  #doesnt have the requisite libraries for doing code coverage for 32 bit binaries
  return ARGUMENTS.get('linecoverage') and env['PROC'] == 'x86-64' and env.get('DO_LINE_COVERAGE')

