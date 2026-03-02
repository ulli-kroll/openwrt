#===============================================================================
#
# BUILDSPEC file builder
#
# GENERAL DESCRIPTION
#    build rules script
#
# Copyright (c) 2009-2017 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header:
#  $DateTime: 2020/11/02 23:12:26 $
#  $Author: pwbldsvc $
#  $Change: 27014951 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 2015/13/06 pdosi   initail version
#===============================================================================

import collections
import os.path
import shutil
from SCons.Script import *

TZBSP_MODULE_BASE_RSVD = 0
TZ_MSK_RSVD_BIT_SHFT = 30
TZ_MSK_MODULE_BIT_SHFT = 20
TZ_MSK_RSVD_MSK = (0x3 << TZ_MSK_RSVD_BIT_SHFT)
TZ_MSK_MODULE_MSK = (0x3FF << TZ_MSK_MODULE_BIT_SHFT)
TZ_MSK_CODE_MSK = 0xFFFFF
prevLine = ""
moduleMacroName="TZBSP_DEF_MODULE"
errorCodeMarcoName="TZBSP_ERR_CODE"
errFatalMacroName = "TZBSP_KRNL_FATAL_ERR_TYPE"

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('errorcodedict_builder')

def generate(env):
   dict_generate(env)

#===============================================================================
# build rules
#===============================================================================
def dict_generate(env):
   #-------------------------------------------------------------------------------
   # def builder for  error codes dictionary generation
   #-------------------------------------------------------------------------------
   env.AddMethod(err_dict_builder_wrapper, "ErrorCodeDictBuilder")
   errdict_act = env.GetBuilderAction(err_dictionary_builder)
   errdict_bld = env.Builder(
        action = errdict_act,
    )
   env.Append(BUILDERS = {'ErrorDictInternalBuilder' : errdict_bld})

     
#----------------------------------------------------------------------------
# builder wrapper
#----------------------------------------------------------------------------
def err_dict_builder_wrapper(env, target):
    '''    
    This implementation is just a dummy wrapper function, but arguments of
    this function can be changed as needed whereas Builder arguments must
    always be exactly (target, source, env).
    '''
    rootenv = env.get('IMAGE_ENV')
    source_list = []
    target_list = []
    source_list.append(env.subst('${BUILD_ROOT}/ssg/securemsm/trustzone/qsee/include/tzbsp_errno.h'))
    source_list.append(env.subst('${BUILD_ROOT}/ssg/securemsm/trustzone/qsee/include/tzbsp_module_def.h'))
    source_list.append(env.subst('${BUILD_ROOT}/ssg/securemsm/trustzone/qsee/include/tzbsp_err_fatal.h'))
    target_list.append(env.subst('${BUILD_ROOT}/ssg/bsp/qsee/'+target))
    
    return env.ErrorDictInternalBuilder(target_list, source_list, build_root="${BUILD_ROOT}")


    
            
def err_dictionary_builder(target, source, env):
   #-------------------------------------------------------------------------------
   # def builder for  error codes dictionary generation
   #-------------------------------------------------------------------------------
   #rootenv = env.get('IMAGE_ENV')
   if not SCons.Script.COMMAND_LINE_TARGETS[0]:
      env.PrintError("Cannot execute errorcodedict builder option without a build Target!! ")
   errorCodesDictBkp = os.path.join(env.get('build_root'), 'ssg','bsp','qsee','errorCodesDict_1.txt') 	
   backupExistingDict(env.get('build_root'))
   generateDictionary(env, source, target)
   if os.path.exists(errorCodesDictBkp):
      os.remove(errorCodesDictBkp)   
   

def err_dictionary_validator(source, target): 
   tzbsp_errno_h = os.path.join(str(source[0]))
   if not os.path.exists(tzbsp_errno_h):
       raise RuntimeError, "File not found" + tzbsp_errno_h
   tz_mod_h = os.path.join(str(source[1]))
   if not os.path.exists(tz_mod_h):
       raise RuntimeError, "File not found" + tz_mod_h     
   tz_err_fatal_h = os.path.join(str(source[2]))
   if not os.path.exists(tz_err_fatal_h):
       raise RuntimeError, "File not found" + tz_err_fatal_h           
   assert isinstance(str(target),basestring), "ErrorCodeDictBuilder requires a valid string for the target."
   
#===============================================================================
# method to calculate error codes, from module name and error code.

"""Macro to create error code.
 32*****************30*********************20***********************1
+-------------------+---------------------+------------------------+
|      2 bits for   |                     |               20 bits  |
|      future use   |          10 bit     |               error code
|                   |          module name|                        +
|                   |                     |                        |
+-------------------+---------------------+------------------------+
"""
#===============================================================================
def backupExistingDict(build_root) :
    errorCodesDict = os.path.join(build_root, 'ssg','bsp','qsee','errorCodesDict.txt') 	
    errorCodesDictBkp =os.path.join(build_root, 'ssg','bsp','qsee','errorCodesDict_1.txt') 	
    if os.path.exists(errorCodesDict) :
        shutil.copyfile(errorCodesDict, errorCodesDictBkp)

#===============================================================================

#===============================================================================

def TZBSP_ERR_CODE(tz_module,code) :
    return (((TZ_MSK_MODULE_MSK&(tz_module<<TZ_MSK_MODULE_BIT_SHFT))|(TZ_MSK_CODE_MSK&(code)))|  (TZ_MSK_RSVD_MSK&(TZBSP_MODULE_BASE_RSVD<<TZ_MSK_RSVD_BIT_SHFT)))

#===============================================================================
# method to get module dic
#===============================================================================

def getModuleDict(tz_mod_h):
    moduleName=None
    moduleNumber=None
    module = collections.defaultdict(list)
    with  open(tz_mod_h, 'r') as fileHandle :
        for line in fileHandle:
            if ((moduleMacroName and  "#define"  in line) and ( "TZBSP_MODULE_BASE_RSVD" not in line ) and len(line.split(moduleMacroName)) == 2) :
                moduleName, moduleNumber = line.split(moduleMacroName)
                moduleName = "".join(moduleName.split("#define")[1]).strip()
                moduleNumber = moduleNumber.replace('(', '').replace(')', '')
            if (moduleName):
                module[moduleName].append(moduleNumber.strip())
    fileHandle.close()

    if len(module) == 0:
        raise RuntimeError, "No modules where found for generating error code dictionary"

    return module
#===============================================================================
# method to get fatal error Dictionary
#===============================================================================

def getFatalErrorDict(tz_err_fatal_h):
    errFatalName = None
    errFatalNumber = 0
    errFatal = dict()
    with  open(tz_err_fatal_h, 'r') as fileHandle :
        for line in fileHandle:
            if (("#define" and 'TZBSP_ERR_FATAL' in line) and 'TZBSP_ERR_FATAL_H' not in line) :
                errFatalNameTemp = line.split('TZBSP_ERR_FATAL_')[1]  
                errFatalNameTemp =  ' '.join(errFatalNameTemp .split())    
                errFatalName, errFatalNumber = errFatalNameTemp.split(' ')[0], errFatalNameTemp.split(' ')[1] 
                errFatalName = 'TZBSP_ERR_FATAL_'+errFatalName
                if (errFatalName):
                    errFatal[errFatalNumber] = errFatalName
    fileHandle.close()

    if len(errFatal) == 0:
        raise RuntimeError, "No Fatal eror where found for generating error code dictionary"
    return errFatal

#===============================================================================
# method to generate the Dictionary
#===============================================================================

def generateDictionary(env, source , target) :
    err_dictionary_validator(source, target)
    dic = open(str(target[0]), 'a')
    dic.truncate()
    tzbsp_errno_h = str(source[0])
    tz_mod_h = str(source[1]) 
    tz_err_fatal_h = str(source[2]) 
    module = getModuleDict(tz_mod_h)
    errFatal = getFatalErrorDict(tz_err_fatal_h) 

    with  open(tzbsp_errno_h, 'r') as errno:
        for line in errno:
            # get the list of modules and relevant module number and create a dict	
            #Generate a dictionary	
            if ((errorCodeMarcoName in line) and ( "TZ_MODULE" not in line ) and ("," in line)) :
                errorCodeVal=None
                moduleDec=None
                errNum=None
                moduleNameEC=None

                if ((line.split(errorCodeMarcoName)[0]) and  ("#define" in line.split(errorCodeMarcoName)[0])) :
                    errorCodeVal=(line.split(errorCodeMarcoName)[0]).split("#define")[1]
                    errorCodeVal=errorCodeVal.strip()
                if ((line.split(errorCodeMarcoName)[1]) and ( "," in line.split(errorCodeMarcoName)[1] )) :
                    moduleNameEC = line.split(errorCodeMarcoName)[1].split(',')[0].replace('(', '')
                    moduleNameEC =moduleNameEC.strip()
                    errNum=line.split(errorCodeMarcoName)[1].split(',')[1].split(')')[0].strip()

                if ((moduleNameEC) and (moduleNameEC in module)):
                    moduleDec = int(module[moduleNameEC][0],16)

                if (errNum) and (moduleNameEC) and (errorCodeVal) and (moduleDec):	
                    if ('0x' in errNum ) :
                        errNum =int(errNum,16)
                    else:
                        errNum =int(errNum)
                    if ("/*" in prevLine) and  (errorCodeMarcoName not in prevLine) and ("Error codes for logging only." not in prevLine) :
                        val=prevLine.split('/*')[1].split('*/')[0].replace('*', '')
                    else:
                        val=" "
                    dic.write( format(TZBSP_ERR_CODE(moduleDec,errNum), '02x')+":"+errorCodeVal + ":"+ val +'\n')
            prevLine = line

    errno.close()
    #Write Error Fatal dictionary; 
    dic.write("+++++++++++++++Start Error Fatal Codes for "+errFatalMacroName+"+++++++++++++++\n")
    for k in sorted(errFatal.keys()):
        dic.write("%s:%s \n" % (k, errFatal[k]))
    dic.write("+++++++++++++++End Error Fatal Codes+++++++++++++++++")
    dic.close()
