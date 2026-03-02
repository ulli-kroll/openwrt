import sys
import SCons
import os

#be sure to pass the following arguments to build_all.py when using include-what-you-use
# -k tzbsp_enable_sa=0 include_what_you_use=1

# output will be in $BUILD_ROOT/build/ms/iwyu_output.txt

def dummy(*args, **kwargs):
  pass
  
def remove_flag(env, target,source):
  env['CCFLAGS'] = env['CCFLAGS'].replace('-fno-vectorize-loops','')

def exists(env):
  return True
  
def generate(env):
  if int(SCons.Script.ARGUMENTS.get('include_what_you_use')) != 1:
    return
 
  if not env.GetOption('keep_going'):
    print "please execute build_all.py with -k when running include what you use"
    sys.exit(1)
    
  env.SetOption('num_jobs',1)
  
  if env['PROC'] == 'A53_64':
    CCCOM = "include-what-you-use -Xiwyu --verbose=0 -Xiwyu --no_comments $CC $CCFLAGS $_CCCOMCOM $INCPATHS -I$LLVMINC $ARMCC_COMPILE_CMD $SOURCES $ARMCC_OUTPUT_CMD $TARGET 2>> iwyu_output.txt  "
  else:
    CCCOM = "include-what-you-use -Xiwyu --verbose=0 -Xiwyu --no_comments $CC $CCFLAGS $_CCCOMCOM $INCPATHS -I$LLVM32INC $ARMCC_COMPILE_CMD $SOURCES $ARMCC_OUTPUT_CMD $TARGET 2>> iwyu_output.txt  "

  output = env.RealPath('${BUILD_ROOT}/build/ms/iwyu_output.txt')
  if os.path.exists(output):
    os.remove(output)
    
  env.Replace(CCCOM =       CCCOM)
  env['PRINT_CMD_LINE_FUNC'] = dummy
  env.Prepend(CCFLAGS=' -w ')
  env.Append(CCFLAGS=' -w ')
  env.PrependCCAction(remove_flag)
 
  
  print env['CCFLAGS']
  