import os
import SCons.Script

def exists(env):
  return True
  
def generate(env):
  env.AddMethod(do_bare_test,"DoBareTest")
  env.AddMethod(do_googletest_test,"DoGoogleTest")
  env.AddMethod(run_test_executable,"RunTestExecutable")
  base_env = env
  
  #the standalone sdk comes with the gtest objects so no need to do this
  if not env.StandaloneSdk():
    for p in ['x86-32', 'x86-64']:
      #we only want to generate these objects once
      if 'GTEST_LIB' + p.replace('-','_') in env['ROOT_GLOBAL_ENV']:
        continue
      env = base_env.Clone()
      env.Replace(OFF_TARGET = True)
      target = 'unittest32' if p == 'x86-32' else 'unittest64'

      aliases = ['unittest', target]
      specific_tag = 'UNITTESTING64_IMAGE' if target == 'unittest64' else 'UNITTESTING32_IMAGE'

      env.InitImageVars(
        alias_list = aliases,       # list of aliases, unique name index [0]
        proc = p,                   # proc settings
        config = 'apps',            # config settings
        build_tags = ['APPS_PROC', 'UNITTESTING_IMAGE', specific_tag],
      )

      env.Tool('offtarget_toolchain',toolpath=['${BUILD_ROOT}/ssg/bsp/build/scripts/'])
      env.InitBuildConfig(False)

      SRCPATH = "${BUILD_ROOT}/ssg/bsp/unittests"
      env.VariantDir('${BUILDPATH}', SRCPATH, duplicate=0)
      
      env.Append(CPPPATH=['${BUILD_ROOT}/ssg/bsp/unittests/'])
      env.Append(CPPPATH=['${BUILD_ROOT}/ssg/bsp/unittests/gtest'])
      env.Replace(APP_EXEC_MODE=p)
      gtest_object = [env.Object('$BUILDPATH/gtest/gtest-all.cc'),env.Object('$BUILDPATH/src/gtest_main.cpp')]
      
      env['ROOT_GLOBAL_ENV']['GTEST_LIB' + p.replace('-','_')] = env.Flatten(gtest_object)
      env.Append(LINKFLAGS=' -pthread ')
      env.Append(CXXFLAGS=' -std=c++11 ')
      env.Append(LINKFLAGS=' -std=c++11 ')
  
  
def do_googletest_test(env, prog_name, sources, includes=None, **kwargs):
  proc = 'x86-32' if env['PROC'] == 'scorpion' else 'x86-64'
  env = env.Clone()
  if env.StandaloneSdk():
    gtest_objs = ['$SDK_ROOT/libs/' + proc + '/gtest_main.o', '$SDK_ROOT/libs/' + proc + '/gtest-all.o']
  else:
    gtest_objs = env['ROOT_GLOBAL_ENV']['GTEST_LIB' + proc.replace('-','_')]
  env.Append(CPPPATH=['${BUILD_ROOT}/ssg/bsp/unittests/'])
  env.Append(CPPPATH=['${BUILD_ROOT}/ssg/bsp/unittests/gtest'])
  env.Append(LINKFLAGS=' -pthread ')
  env.Append(CXXFLAGS=' -std=c++11 ')
  env.Append(LINKFLAGS=' -std=c++11 ')
  kwargs['user_libs'] = gtest_objs
  return do_bare_test(env, prog_name, sources, includes, doing_google_test=True, **kwargs)

def do_bare_test(env, prog_name, sources, includes=None, **kwargs):
  if not kwargs.get('doing_google_test'):
    #we already cloned in do_googletest_test
    env = env.Clone()
  
  env['OFF_TARGET'] = True
  if env.StandaloneSdk():
    env.Tool('offtarget_toolchain',toolpath=['${SDK_ROOT}/scripts/'])
  else:
    env.Tool('offtarget_toolchain',toolpath=['${BUILD_ROOT}/ssg/bsp/build/scripts/'])
 
  
  test64only = kwargs.get('test64only')
  test32only = kwargs.get('test32only')
  
  if test64only and test32only:
    raise ValueError('error: cant specify test64only=True and test32only=True at the same time')
    
  if test32only and env.IsKeyEnable('UNITTESTING64_IMAGE'):
    return
  if test64only and env.IsKeyEnable('UNITTESTING32_IMAGE'):
    return
    
  if 'linker_script' in kwargs:
    linker_script = kwargs['linker_script']
    scl_path = os.path.join('$BUILDPATH', os.path.splitext(os.path.basename(linker_script))[0])
    scl = env.SclBuilder(scl_path, linker_script)
    kwargs['LINKFLAGS'] = env['LINKFLAGS'] + ' -Wl,-T,' + env.RealPath(scl[0].get_abspath())
    
  if not includes:
    includes = []
    
  prog = env.OfftargetClientAppBuilder(sources=sources,includes=includes,image=prog_name,no_emulation_libs=True,**kwargs)
  
  if 'linker_script' in kwargs:
    env.Depends(prog, scl)

  env.RunTestExecutable(prog)
  aliases = env.Flatten([kwargs.get('alias', env['IMAGE_ALIASES']), prog_name])
  a = env.Alias(aliases, prog)
  env.AlwaysBuild(prog)
  
  return a
  
def run_test_executable(env, prog):
  prog_path = prog[0].abspath
    
  if env.DoLineCoverage():
    raw_prof_path = prog_path + '.profraw'
    indexed_prof_path = prog_path + '.profdata'
    cmd = ['LLVM_PROFILE_FILE={} '.format(raw_prof_path) + prog_path,
    env['PROFDATA'] + ' merge -sparse {} -o {}'.format(raw_prof_path,indexed_prof_path),
    env['COV'] + ' report {} -instr-profile={}'.format(prog_path, indexed_prof_path)]
    cmd.insert(0,env.get('LD_LIBRARY_PATH_STR',''))
  else:
    cmd = env.get('LD_LIBRARY_PATH_STR','') + prog_path
    
  
  env.AddPostAction(prog,cmd)
    
  
  
    
  