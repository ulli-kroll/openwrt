#  Copyright (c) 2020,2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.


"""Collection of general-purpose tools/methods for use in SSG SConscripts"""

import hashlib
import os
import sys
import SCons
import sys
import os
import threading
import SCons.Defaults
import time
import datetime
from xml.etree import ElementTree
#import QueryKrnlSvcNCmnlibApi
#import sdk_dpl_files_track

THIS_DIR = os.path.abspath(os.path.dirname(__file__))


#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
    return env.Detect('ssg_environment')


def generate(env):
    elf_scripts_path = env.subst('${BUILD_ROOT}/core/bsp/build/scripts/')
    sys.path.append(os.path.abspath(elf_scripts_path))
    env.AddMethod(feature_flag, "DeclareFeature")
    env.AddMethod(get_scons_abspath, "SConsAbsPath")
    env.AddMethod(get_app_id_by_image_name, "GetAppIDByImageName")
    env.AddMethod(offtarget_testing, "OfftargetTesting")
    env.AddMethod(is_64_bit_image, "Is64BitImage")
    env.AddMethod(standalone_sdk, "StandaloneSdk")
    env.AddMethod(file_exists, "SconsFileExists")
    env.AddMethod(dir_exists, "SconsDirExists")
    env.AddMethod(get_image_config_xml, "GetImageConfigXml")
    env.AddMethod(has_apps_component,"HasAppsComponent")
    env.AddMethod(xbl_sec_feature_list, "XblSecFeatureList")

#    env.AddMethod(QueryKrnlSvcNCmnlibApi.query_supported_krnl_svc, "QuerySupportedKrnlSvc")
#    env.AddMethod(QueryKrnlSvcNCmnlibApi.query_supported_cmnlib_api, "QuerySupportedCmnlibApi")

    env.AddMethod(offtarget_is_enabled, 'OfftargetIsEnabled')

#    env.AddMethod(sdk_dpl_files_track.sdk_lib_src_track, 'SdkLibsSrcTrack')
    #We need to have a dummy DoUnitTest method for when we aren't building the unittest target
    #the actual unittest sconscript will load the actual tools/features
    env.AddMethod(dummy_method,"DoBareTest")
    env.AddMethod(dummy_method,"DoGoogleTest")

    #Dummy Doxygen method
    env.AddMethod(dummy_method,"AddDoxySource")
    
    #Dummy publish and require APIs
    env.AddMethod(dummy_method,"PublishProtectedApi")
    env.AddMethod(dummy_method,"PublishPrivateApi")
    env.AddMethod(dummy_method,"RequirePublicApi")
    env.AddMethod(dummy_method,"RequireRestrictedApi")
    env.AddMethod(dummy_method,"RequireProtectedApi")
    env.AddMethod(dummy_method,"RequirePrivateApi")
    
    env.AddMethod(dummy_method,"SyscallEntry")
    env.AddMethod(dummy_method,"AddTZBSPSyscalls")
    env.AddMethod(dummy_method,"SyscallListAsBuilderSource")
    env.AddMethod(dummy_method,"BuilderSourceAsSyscallList")
    env.AddMethod(dummy_method,"GenerateTZBSPSyscallDef")

    env.AddMethod(uncloneable_list,'UncloneableList')
    env.AddMethod(uncloneable_dict,'UncloneableDict')
    env.AddMethod(uncloneable_tuple,'UncloneableTuple')
    env.AddMethod(add_bool_config_var,"AddBoolConfigVar")
    env.AddMethod(add_config_vars,"AddConfigVars")
    env.AddMethod(add_commandline_argument,"AddCommandlineArgument")
    
    env.AddMethod(bool_variable,'BoolVariable')
    env.AddMethod(value_variable,'ValueVariable')
    env.AddMethod(enum_variable,'EnumVariable')
    env.AddMethod(list_variable,'ListVariable')
    env.AddMethod(package_variable,'PackageVariable')
    env.AddMethod(path_variable,'PathVariable')
    env.AddMethod(feature_is_enabled,'FeatureIsEnabled')
    env.AddMethod(uid_to_class_num,'UIDToClassNum')
    
    
    
    # now use DeclareFeature to declare this script itself
    env.DeclareFeature("SSG_BUILD_ENVIRONMENT", uses_prefix=True)
    
    
#    env.Tool('image_configuration', toolpath=[THIS_DIR])
#    env.Tool('cmdline_config', toolpath=[THIS_DIR])
    
    env.AddCommandlineArgument(env.BoolVariable('do_build_resource_stats','print out stats about memory and cpu load while building',0))
    default_env = SCons.Defaults._fetch_DefaultEnvironment()
    #This file is called from a few potential places during a full build; we only want to spawn the
    #stats printer thread once
    if env.get('do_build_resource_stats') and not default_env.get('CPU_LOAD_THREADING_STUFF_INITTED'):
      default_env['CPU_LOAD_THREADING_STUFF_INITTED'] = True
      init_print_cpu_load(env)


def print_cpu_load(psutil):
  p = psutil.Process()
  #as far as the terminal output is concerned, one print statement is atomic. so we put all of
  #the output into one string so that the stats aren't interleaved with other build output
  with p.oneshot():
    s = '__cpustats__ ' + str(time.time())
    s += ' getloadavg' + str(psutil.getloadavg())
    s += ' cpu_percent(' + str(psutil.cpu_percent(percpu=False))
    s += ') ' + str(psutil.virtual_memory())
  print s
  
def print_cpu_load_daemon(psutil, *args, **kwargs):
  while True:
    print_cpu_load(psutil)
    time.sleep(10)

def init_print_cpu_load(env):
  # Gathering cpu/ram usage stats requires psutil, which is not part of the standard pythonlib2
  # library. On internal linux harvesters, we can grab it from /pkg/ssg/..., but can't make that
  # guarantee in any other way really, so we try to import it, and if we fail, we just dont gather
  # build stats
  try:
    if os.path.isdir('/pkg/ssg'):
      sys.path.append('/pkg/ssg/pythonlib2.7/site-packages')
    import psutil
    print_cpu_load(psutil)
    #we use threading rather than multiprocessing because scons does black magic
    #to rename the Pickling module, which is also used by multiprocessing. Said
    #black magic kills the multiprocessing spawning code with weird errors. It does
    #not, however, affect how threading spawns threads. The print_cpu_load is very
    #nice and sleeps a long time, so it does not affect build time significantly,
    #regardless of the GIL existing
    thread = threading.Thread(target=print_cpu_load_daemon, args=(psutil,))
    # spawning the print_cpu_load thread as a daemon means it will get killed when 
    # the parent scons process is also killed
    thread.daemon = True
    thread.start()
    print "Initialized build resource gathering statistics!"
  except:
    print "Could not initialize build resource gathering statistics!"
    env['do_build_resource_stats'] = False
    if '/pkg/ssg/pythonlib2.7/site-packages' in sys.path:
      sys.path.remove('/pkg/ssg/pythonlib2.7/site-packages')


def feature_flag(env, flag, uses_prefix=False):
    """Adds a feature-definition flag to the environment in a way that is
       compatible with both QC SCons (USES_FLAGS) & Open Source SCons. Either
       implementation can be checked in SConscripts with 'if <flag> in env'.
       If 'uses_prefix' is True, also declares a USES_<flag> version of the
       flag, even if QC SCons is not available (useful for tools/features loaded
       globally before QC SCons)."""
    try:
        env.AddUsesFlags(flag)
    except AttributeError:
        env[flag] = True

    # most CoreBSP scripts expect that uses flags will always begin with
    # USES_* for consistency. If this feature flag does not already do that,
    # define an additional flag for their benefit.
    if flag.startswith("USES_"):
        return

    try:
        env.AddUsesFlags("USES_" + flag)
    except AttributeError:
        if uses_prefix:
            env["USES_" + flag] = True


def get_scons_abspath(env, item):
    """Returns the absolute path of the given string or SCons Node"""
    ret = None
    if type(item) == str:
      ret = env.File(item).get_abspath()
    elif isinstance(item, SCons.Node.Node):
      ret = item.get_abspath()
    elif isinstance(item, SCons.Node.Alias.Alias):
      ret = item.get_abspath()

    assert ret != None, "Unexpected item type '{}'".format(str(type(item)))
    return ret


def file_exists(env, filepath):
    """Returns True if 'filepath' exists, else False. Does not work for
       directories"""
    return os.path.exists(str(env.File(filepath).srcnode()))


def dir_exists(env, dirpath):
    """Returns True if 'dirpath' exists, else False. Only works for
       directories"""
    return os.path.exists(str(env.Dir(dirpath).srcnode()))


def get_image_config_xml(env, xml_file, alias_name):
    """Returns a valid, chipset-specific path for the IMAGE_CONFIG_XML
       environment variable required by devcfg"""
    img_cfg = "${BUILD_ROOT}/core/bsp/build/chipset/${CHIPSET}/" + xml_file
    if env.SconsFileExists(img_cfg):
        return env.subst(img_cfg)

    env.PrintWarning("Using stubs IMAGE_CONFIG_XML for " + alias_name)
    return env.subst("${BUILD_ROOT}/ssg/bsp/build/chipset/sdm0000/" + xml_file)


def get_app_id_by_image_name(env, image_name):
    """Return app id by calculating hash value with image name

       On MBNv6, app id has 4 byte length (8 hex characters) while previous
       version supported 8 byte, 16 hex characters. So, we limit it as 4 byte
       with exception (sdm670/sdm845) due to potential regression
       """

    if env["CHIPSET"] in ['sdm670', 'sdm845']:
        app_id_hexstr_len = 16
    else:
        app_id_hexstr_len = 8

    return '0x' + hashlib.sha256(image_name).hexdigest()[-app_id_hexstr_len:]

def offtarget_testing(env):
    return env.get('OFF_TARGET')

def is_64_bit_image(env):
    return env['PROC'] in ['A53_64', 'x86-64']
    
def standalone_sdk(env):
    return env.get('STANDALONE_SDK')
    
def has_apps_component(env):
    return env.get('HAS_APPS_COMPONENT')

def xbl_sec_feature_list(env):
    uses_stubs = False
    if env.get("MSM_ID") != "0000" and \
            env.SconsFileExists("${BUILD_ROOT}/ssg/securemsm/xbl_sec/build/cfg/xbl_sec_build_config_${MSM_ID}.xml"):
        build_config_xml = env.subst('${BUILD_ROOT}/ssg/securemsm/xbl_sec/build/cfg/xbl_sec_build_config_${MSM_ID}.xml')
    else:
        uses_stubs = True
        build_config_xml = env.subst('${BUILD_ROOT}/ssg/securemsm/xbl_sec/build/cfg/xbl_sec_build_config_0000.xml')

    # parse target specific configuration file to get list of features the
    # target needs to be built with.
    feature_list = []
    build_config_tree = ElementTree.parse(build_config_xml)
    build_config_root = build_config_tree.getroot()
    for child in build_config_root:
        if child.attrib["name"] == env['CHIPSET'] or uses_stubs:
            for feature in child.iter("feature"):
                feature_list.append(feature.text)

    return feature_list

def dummy_method(*args, **kwargs):
    pass

class UncloneableList(list):
  def __semi_deepcopy__(self):
    return self
  
def uncloneable_list(env):
  return UncloneableList()
  
class UncloneableDict(dict):
  def __semi_deepcopy__(self):
    return self
  
def uncloneable_dict(env):
  return UncloneableDict()
  
class UncloneableTuple(tuple):
  def __semi_deepcopy__(self):
    return self

class ConfigVar:
  def __init__(self, var, enable_map, disable_map, enable_if):
    self.var = var
    self.enable_map = enable_map
    self.disable_map = disable_map
    self.enable_if = enable_if

  def unpack(self):
    return self.var, self.enable_map, self.disable_map, self.enable_if


def _var_setup(var, enable_map, disable_map, enable_if):
  if not any([enable_map,disable_map,enable_if]):
    return var
  if enable_map is None:
    enable_map = {}
  if disable_map is None:
    disable_map = {}
  return ConfigVar(var, enable_map, disable_map, enable_if)

def add_bool_config_var(env,
                        config_name,
                        help=None,
                        default_value=None,
                        enable_map=None,
                        disable_map=None,
                        enable_if=None):
  if enable_map is None:
    enable_map = {}
  if disable_map is None:
    disable_map = {}
  if help is None:
    help = ""
  var = env.BoolVariable(config_name, help, default_value)
  return ConfigVar(var, enable_map, disable_map, enable_if)
                        
#files can contain either raw strings of paths to config files, or tuples of (str,bool)
#where tuple[0] is the path to the config file and tuple[1] is a bool designating whether
#or not the file is required. If the file is required and doesnt exist, we error out. If 
#the file is not required and doesnt exist, we just print a warning
def add_config_vars(env, files, args):
  files_processed = []
  for f in files:
    required = True
    if isinstance(f,tuple):
      path, required = f
    else:
      path = f
    path = env.RealPath(path)
    if not env.SconsFileExists(path):
      if required:
        raise ValueError('ERROR: cannot open config file {}'.format(path))
      else:
        print 'WARNING: cannot open config file {}'.format(path)
    files_processed.append(path)
    
  files_processed = [env.RealPath(f) for f in env.Flatten(files_processed)]
  variables = SCons.Script.Variables(files_processed, args=SCons.Script.ARGUMENTS)
  for arg in args:
    if isinstance(arg,ConfigVar):
      var = arg.var
    else:
      var = arg
    variables.Add(var)
  variables.Update(env)
  for arg in args:
    #at this point, there isn't anything to do for vanilla scons options. they've already been 
    #loaded into the env and that's all that they can do
    if isinstance(arg, ConfigVar):
      var, enable_map, disable_map, enable_if = arg.unpack()
      config_name = var[0]
      if callable(enable_if):
        if not enable_if(env):
          env.Append(**disable_map)
          continue
      if env.get(config_name):
        env.Append(**enable_map)
      else:
        env.Append(**disable_map)

def uid_to_class_num(env,uid_str):

  uid_str = long_uid_to_short_name(env,uid_str)

  if 'IDL_CLASS_MAP' not in env['ROOT_GLOBAL_ENV']:
    sys.path.insert(0, env.subst('${BUILD_ROOT}/ssg/bsp/build/scripts'))
    from SecureAppMetadata import IDLScanner
    idl_class_ids = IDLScanner().create_class_id_map(env)
    env['ROOT_GLOBAL_ENV']['IDL_CLASS_MAP'] = idl_class_ids

  idl_class_ids = env['ROOT_GLOBAL_ENV']['IDL_CLASS_MAP']
  return idl_class_ids[uid_str]

#this can receive either long form (e.g. 'CApp_UID') uid strings or short for
# (e.g. just 'app')
def long_uid_to_short_name(env,uid_str):
  if uid_str.startswith('C'):
    uid_str = uid_str[1:]
  return uid_str.lower().replace('_uid','')

def feature_is_enabled(env, feature):
  #TODO: unstub me once commandline overhaul is in
  return True
 
def add_commandline_argument(env, variable):
  vars = SCons.Script.Variables(args=SCons.Script.ARGUMENTS)
  vars.Add(variable)
  vars.Update(env)
  #if the default value for variable is None and the variable isnt specified on the commandline
  #scons just wont add it to the env. That doesn't make any sense, so let's fix that here
  default_val = variable[2]
  key = variable[0]
  if default_val is None and key not in env:
    env[key] = None
  
def bool_variable(env, key, help, default, enable_map=None, disable_map=None, enable_if=None):
  var = SCons.Variables.BoolVariable(key, help, default)
  return _var_setup(var, enable_map, disable_map, enable_if)

def value_variable(env, key, help, default, enable_map=None, disable_map=None, enable_if=None):
  var = (key, help, default)
  return _var_setup(var, enable_map, disable_map, enable_if)
 
def enum_variable(env, key, help, default, allowed_values, map={}, ignorecase=0):
  return SCons.Variables.EnumVariable(key, help, default, allowed_values, map, ignorecase)
  
def list_variable(env, key, help, default, names, map={}):
  return SCons.Variables.ListVariable(key, help, default, names, map)
  
def package_variable(env, key, help, default, searchfunc=None):
  return SCons.Variables.PackageVariable(key, help, default, searchfunc)
  
def path_variable(env, key, help, default, validator=None):
  return SCons.Variables.PathVariable(key, help, default, validator)
  
class UncloneableList(list):
  def __semi_deepcopy__(self):
    return self
  
def uncloneable_list(env):
  return UncloneableList()
  
class UncloneableDict(dict):
  def __semi_deepcopy__(self):
    return self
  
def uncloneable_dict(env):
  return UncloneableDict()
  
class UncloneableTuple(tuple):
  def __semi_deepcopy__(self):
    return self

def uncloneable_tuple(env):
  return UncloneableTuple()

def offtarget_is_enabled(env):
  return env.get('OFF_TARGET_ENABLED')
