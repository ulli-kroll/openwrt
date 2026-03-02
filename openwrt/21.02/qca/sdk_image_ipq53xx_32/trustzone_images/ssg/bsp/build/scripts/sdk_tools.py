import SCons.Script

def dummy(*args, **kwargs):
  pass

def exists(env):
  return True
  
def check_alias(env, aliases):
  return len(set(aliases).intersection(set(SCons.Script.COMMAND_LINE_TARGETS))) > 0
  
def require_env_vars(env,vars):
  vars = env.Flatten(vars)
  for var in vars:
    if not env.has_key(var):
      print """{} is not defined in env!
Pass the key value pair on the command line. e.g.
CHIPSET=sdm845"""
      raise KeyError()
  
def generate(env):
  env.AddMethod(init_arch,'InitArch')
  env.AddMethod(check_alias,'CheckAlias')
  env.AddMethod(require_env_vars,'RequireEnvVars')
  

  
#inits tools and environment variables for the given arch
# acceptable values for arch:
#    'aarch32'
#    'aarch64'
#    'x86-32'
#    'x86-64'    
def init_arch(env, arch):
  env.Replace(OUT_DIR='.')
  env.Replace(LIB_OUT_DIR='.')
  if arch == 'aarch64':
    env.Replace(PROC='A53_64')
    env.Replace(TZ_EXEC_MODE='aarch64')
    env.Replace(APP_EXEC_MODE='aarch64')
    env.Replace(OFFTARGET_TESTING=False)
  elif arch == 'aarch32':
    env.Replace(PROC='scorpion')
    env.Replace(TZ_EXEC_MODE='aarch32')
    env.Replace(APP_EXEC_MODE='aarch32')
    env.Replace(OFFTARGET_TESTING=False)
  elif arch == 'x86-32':
    env.Replace(PROC='x86-32')
    env.Replace(TZ_EXEC_MODE='x86-32')
    env.Replace(APP_EXEC_MODE='x86-32')
    env.Replace(OFFTARGET_TESTING=True)
  elif arch == 'x86-64':
    env.Replace(PROC='x86-64')
    env.Replace(TZ_EXEC_MODE='x86-64')
    env.Replace(APP_EXEC_MODE='x86-64')
    env.Replace(OFFTARGET_TESTING=True)
    pass
  else:
    raise ValueError('{} is not a valid supported architecture'.format(arch))
    
  env.Tool('secure_app_builder',toolpath=['$SDK_ROOT/scripts'])
  
