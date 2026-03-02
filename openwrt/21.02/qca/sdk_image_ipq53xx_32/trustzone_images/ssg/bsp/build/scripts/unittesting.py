import os
import SCons.Script

def exists(env):
  return True
  
def generate(env):
  env.AddMethod(do_bare_test,"DoBareTest")
  env.AddMethod(do_googletest_test,"DoGoogleTest")
  env.AddMethod(run_test_executable,"RunTestExecutable")
  
def do_googletest_test(env, prog_name, sources, includes=None, **kwargs):
  env = env.Clone()
  return do_bare_test(env, prog_name, [sources,env['GTEST_LIB']], includes, doing_google_test=True, **kwargs)

def do_bare_test(env, prog_name, sources, includes=None, **kwargs):
  if not kwargs.get('doing_google_test'):
    #we already cloned in do_googletest_test
    env = env.Clone()
  
  
  test64only = kwargs.get('test64only')
  test32only = kwargs.get('test32only')
  
  if test64only and test32only:
    raise ValueError('error: cant specify test64only=True and test32only=True at the same time')
    
  if test32only and env.IsKeyEnable('UNITTESTING64_IMAGE'):
    return
  if test64only and env.IsKeyEnable('UNITTESTING32_IMAGE'):
    return
    
  if env.IsKeyEnable('UNITTESTING_IMAGE'):
    if 'linker_script' in kwargs:
      linker_script = kwargs['linker_script']
      scl_path = os.path.join('$BUILDPATH', os.path.splitext(os.path.basename(linker_script))[0])
      scl = env.SclBuilder(scl_path, linker_script)
      kwargs['LINKFLAGS'] = env['LINKFLAGS'] + ' -Wl,-T,' + env.RealPath(scl[0].get_abspath())
      
    if not includes:
      includes = []
      
    env.Append(CPPPATH=includes)
    
    prog = env.Program('$BUILDPATH/' + prog_name, sources, PROGSUFFIX='', **kwargs)
    
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
    
  
  
    
  