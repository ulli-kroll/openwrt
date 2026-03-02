def exists(env):
  return True
  
  
class SyscallEntry:
  __slots__ = ['mink_service', 'smc_id', 'param_id', 'flags', 'func', 'prefix', 'suffix', 'weak']
  def __init__(self, mink_service, smc_id, param_id, flags, func, prefix, suffix, weak):
    self.mink_service = mink_service
    self.smc_id = smc_id
    self.param_id = param_id
    self.flags = flags
    self.func = func
    self.prefix = prefix
    self.suffix = suffix
    self.weak = weak
    
  def unpack(self):
    return self.mink_service, self.smc_id, self.param_id, self.flags, self.func, self.prefix, self.suffix, self.weak
    
  def get_entry(self):
    return self.mink_service, self.smc_id, self.param_id, self.flags, self.func
    
  def __str__(self):
    return '''
    mink_service = {}
    smc_id = {}
    param_id = {}
    flags = {}
    func = {}
    prefix = {}
    suffix = {}
    weak = {}
    '''.format(self.mink_service, self.smc_id, self.param_id, self.flags, self.func, self.prefix, self.suffix, self.weak)
    
  def __eq__(self, other):
    for thing in SyscallEntry.__slots__:
      if getattr(self,thing) != getattr(other,thing):
        return False
  
    return True
    
  def __hash__(self):
    ret = 0
    for thing in SyscallEntry.__slots__:
      ret += hash(thing)
    return ret
  
def syscall_entry(env, mink_service, smc_id, param_id, flags, func, prefix='', suffix='', weak=False):
  return SyscallEntry(mink_service, smc_id, param_id, flags, func, prefix, suffix, weak)
  
def generate(env):
  env['TZBSP_SYSCALLS'] = env.UncloneableDict()
  
  env.AddMethod(syscall_entry, 'SyscallEntry')
  env.AddMethod(add_tzbsp_syscalls,'AddTZBSPSyscalls')
  env.AddMethod(syscall_list_as_builder_source,'SyscallListAsBuilderSource')
  env.AddMethod(builder_source_as_syscall_list,'BuilderSourceAsSyscallList')
  b = env.Builder(action = gen_tzbsp_syscall_def)
  env.Append(BUILDERS={'GenerateTZBSPSyscallDef': b})
  
def chunk(lst, size):
  for i in range(0, len(lst), size):
    yield lst[i:i + size]
   
def sanity_check(new_syscalls, existing_syscalls, attr):

  new_things = [getattr(s,attr) for s in new_syscalls]
  existing_things = [getattr(s,attr) for s in existing_syscalls]
  
  #check for duplicates in new_syscalls
  duplicates = set([x for x in new_things if new_things.count(x) > 1])
  if len(duplicates) > 0:
    print "ERROR! trying to add multiple of these",attr," in the same env.AddTZBSPSyscall"
    for d in duplicates:
      print "ERROR!",d
    raise ValueError("ERROR! trying to add multiple of these ",attr," in the same env.AddTZBSPSyscall")
    
  #check for syscalls that have already been added
  duplicates = set(new_things).intersection(set(existing_things))
  if len(duplicates) > 0:
    print "ERROR! trying to add multiple of these ",attr," between different env.AddTZBSPSyscall calls"
    for d in duplicates:
      print "ERROR!",d
    raise ValueError("ERROR! trying to add multiple of these ",attr," between different env.AddTZBSPSyscall calls")


   
def add_tzbsp_syscalls(env, images, syscall_list):
  assert isinstance(syscall_list, list)
  if not isinstance(images, list):
    images = [images]
  
  for i in env.Flatten(images):
    if i not in env['TZBSP_SYSCALLS']:
      env['TZBSP_SYSCALLS'][i] = list()
    
    #let's do some sanity checks
    sanity_check(env['TZBSP_SYSCALLS'][i], syscall_list, 'smc_id')
    #Looks like there's a use case for duplicating PARAM_ID
    #sanity_check(env['TZBSP_SYSCALLS'][i], syscall_list, 'param_id')
    sanity_check(env['TZBSP_SYSCALLS'][i], syscall_list, 'func')
    env['TZBSP_SYSCALLS'][i].extend(syscall_list)
  

#syscall_list is a list of the SyscallEntry objects that are passed to env.AddTZBSPSyscall(image, syscalls)
def syscall_list_as_builder_source(env, do_lookup_func, prefix, syscall_list, suffix):
  ret = [env.Value(do_lookup_func), env.Value(prefix)]
  for entry in env.Flatten(syscall_list):
    ret.extend([env.Value(v) for v in entry.unpack()])
  ret.append(env.Value(suffix))
  return ret
  
def builder_source_as_syscall_list(env, source):
  syscall_list = []
  do_lookup_func = source.pop(0).get_contents() == 'True'
  prefix = source.pop(0).get_contents()
  suffix = source.pop().get_contents()
  for entry in chunk(source, 8):
    #turning a scons Value(bool) internalizes it as a string, then gives us back
    #the string value when calling get_contents(). That's... not what we want, so
    #explicitly check here for the string value then turn it back into an actual bool
    weak = entry[7].get_contents() == 'True'
    syscall_list.append(
      env.SyscallEntry(entry[0].get_contents(),
      entry[1].get_contents(),
      entry[2].get_contents(),
      entry[3].get_contents(),
      entry[4].get_contents(),
      entry[5].get_contents(),
      entry[6].get_contents(),
      weak))
  return do_lookup_func, prefix, syscall_list, suffix

case_entry = '''
  case {smc_id}:
  {{
    ret->smc_id = {smc_id};
    ret->param_id = {param_id};
    ret->flags = {flags};
    ret->func = {func};
    return E_SUCCESS;
  }}
'''

def gen_syscall_defs(f, syscall_list):
  for entry in syscall_list:
    #TODO: check that the given syscall is actually enabled
    if entry.prefix:
      f.write(entry.prefix + '\n')
    syscall_statement = 'TZBSP_DEFINE_WEAK_SYSCALL' if entry.weak else 'TZBSP_DEFINE_SYSCALL'
    f.write('{}(\n\t{},\n\t{},\n\t{},\n\t{},\n\t{});\n'.format(syscall_statement, *entry.get_entry()))
    if entry.suffix:
      f.write(entry.suffix + '\n')
  
lookup_func_header = '''int tzbsp_syscall_lookup_new(uintnt smc_id, tzbsp_syscall_entry_t* ret)
{
  switch(smc_id){
'''

lookup_func_footer = '''}
  return E_FAILURE;
}'''
  
def gen_lookup_func(f, syscall_list):
  f.write(lookup_func_header)
  for entry in syscall_list:
    if entry.prefix:
      f.write(entry.prefix + '\n')
    f.write('extern int {} ();\n'.format(entry.func))
    f.write(case_entry.format(smc_id=entry.smc_id, param_id=entry.param_id,flags=entry.flags,func=entry.func))
    if entry.suffix:
      f.write(entry.suffix + '\n')
  f.write(lookup_func_footer)
  
def gen_tzbsp_syscall_def(env,target,source):
  print 'generating_syscall_def',target[0].abspath
  do_lookup_func, prefix, syscall_list, suffix = env.BuilderSourceAsSyscallList(source)
  with open(target[0].abspath, 'w+') as f:
    f.write(prefix + '\n')
    if do_lookup_func:
      gen_lookup_func(f,syscall_list)
    else:
      gen_syscall_defs(f,syscall_list) 
    f.write(suffix + '\n')  
    
      
