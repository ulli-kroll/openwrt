import os
import sys
import SCons

def exists(env):
  return True
  
def generate(env):
  if sys.platform.startswith("linux"):
    platform = "linux"
    ending = ""
  elif sys.platform.startswith("win"):
    platform = "win"
    ending = ".exe"
  bin_path = os.path.dirname(os.path.realpath(__file__))
  bin_path = os.path.join(bin_path, 'bin', platform)
  
  minkidl_builder = env.Builder(generator = minkidl_generator,
                               src_suffix = '.idl',
                               suffix = '.h',
                               emitter = minkidl_emitter)
  
  env['MINKIDL']          = '{}/minkidl{}'.format(bin_path,ending)
  #leaving off $IDLINCPATH since we need to handle it specially in the generator
  env['MINKIDLCOM']       = '$MINKIDL -o $TARGET $SOURCE ' 
  env['MINKIDLCOMSTR']    = 'minkidl ${TARGET.basename}.h'
  env.Replace(IDLINCPATH=[])
      
  env.Append(BUILDERS={'MINKIDL' : minkidl_builder})
  
  env.AddMethod(run_minkidl,'RunMinkidl')
  ignore_builder = env.Builder(action=ignore_file_builder)
  env.Append(BUILDERS={'IgnoreFileBuilder':ignore_builder})
  env.Append(IDLINCPATH='${BUILD_ROOT}/ssg/securemsm/trustzone/qsee/idl/public')
  try:
    root_env = env['ROOT_GLOBAL_ENV']
    if not root_env.get('MINKIDL_HAS_RUN'):
      env.RunMinkidl()
      root_env['MINKIDL_HAS_RUN'] = True
  except:
    env.RunMinkidl()
    
  
def ignore_file_builder(env,target,source):
  with open(str(target[0]),'w+') as f:
    f.write('*.o\n')
    f.write('*.lib\n')
    f.write('*~\n')
    f.write('*.pp\n')
    f.write('*.pyc\n')
    f.write('*.elf\n')
    f.write('*.map\n')
    f.write('*.scl\n')
    f.write('*.mbn\n')
    f.write('*.so\n')
    f.write('*.elf\n')
    f.write('*.pbn\n')
    f.write('*.hash\n')
    f.write('*.dat\n')
    f.write('*.enc\n')
    f.write('*.bin\n')
    f.write('*.efi\n')
    f.write('*.log\n')
    f.write('*.fv\n')
    f.write('*.obj\n')
    f.write('*.tempsym\n')
    f.write('*.i\n')
    f.write('*.scl\n')
    f.write('*.tempscl\n')
    f.write('scorpion/\n')
    f.write('A53_64/\n')
    f.write(env.subst('${BUILD_ID}/') + '\n')
    for idl in sorted(source, key=lambda x: x.get_abspath()):
      rel_path = str(idl).split('trustzone_images')[1]
      if rel_path.startswith('/ssg'):
        rel_path = rel_path.replace('/ssg/','')
        f.write(rel_path + '\n')
  
    
def minkidl_generator(env, target, source, for_signature):
  cmds = []
  for t in target:
    cmd = env.subst('$MINKIDLCOM ', target=t, source=source[0])
    incpaths = env.get('IDLINCPATH')
    if incpaths:
      incpaths = [env.RealPath(x) for x in env.Flatten(incpaths)]
      incpathstr = ' -I ' + ' -I '.join(incpaths)
      cmd += incpathstr
    if str(t).endswith('_invoke.h'):
      cmd += ' --skel '
    cmds.append(env.Action(cmd,'minkidl ' + str(t)))
  return cmds

def minkidl_emitter(env, target, source):
    base, ext = os.path.splitext(str(target[0]))
    incl = base + '.h'
    t = [incl]
    if os.path.basename(str(source[0])).startswith('I'):
      skeleton = base + '_invoke.h'
      t.append(skeleton)
    env.Alias('minkidl_headers',t)
    return (t,source)


def run_minkidl(env,paths=None):
  if paths is None:
    paths = []
  all_idls = []
  if not env.StandaloneSdk():
    env = env.Clone()
    env.Tool('deploy_sdk', toolpath=['$BUILD_ROOT/ssg/bsp/build/scripts'])
  else:
    paths += ['$SDK_ROOT']
  for p in paths + ['${BUILD_ROOT}']:
    for dir, subdirs, files in os.walk(env.RealPath(p)):
      if dir.endswith(((os.path.join('ssg','securemsm','trustzone','qsee','idl','public')),
                       (os.path.join('ssg','securemsm','trustzone','qsee','idl','private')))):
        # idl files that live under ssg/securemsm/trustzone/qsee/idl/(public/private)
        # are special cases so we skip them
        continue
      for file in files:
        if file.endswith('.idl'):
          all_idls.extend(env.MINKIDL(os.path.join(dir,file)))

  target_dir = '${BUILD_ROOT}/ssg/securemsm/trustzone/qsee/include'
  idl_files  = env.Glob('${BUILD_ROOT}/ssg/securemsm/trustzone/qsee/idl/public/*.idl')
  for idl_file in idl_files:
    target = os.path.join(target_dir, os.path.basename(os.path.splitext(str(idl_file))[0] + '.h'))
    ret = env.MINKIDL(target=target, source=idl_file)
    all_idls.extend(ret)
	
  idl_files = env.Glob('${BUILD_ROOT}/ssg/securemsm/trustzone/qsee/idl/private/*.idl')
  for idl_file in idl_files:
    target = os.path.join(target_dir, os.path.basename(os.path.splitext(str(idl_file))[0] + '.h'))
    all_idls.extend(env.MINKIDL(target=target, source=idl_file))
  all_idls = list(set(all_idls))
  if not env.StandaloneSdk():
    ret = env.IgnoreFileBuilder(target='$BUILD_ROOT/ssg/.p4ignore',source=all_idls)
    env.Alias('P4IgnoreFile',ret)
        
