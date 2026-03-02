#===============================================================================
#
# SCL build rules
#
# GENERAL DESCRIPTION
#    build script
#
# Copyright (c) 2009-2009 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/apps.tzw/1.0/bsp/build/scripts/scl_builder.py#1 $
#  $DateTime: 2018/09/03 04:30:12 $
#  $Author: pwfwsvc $
#  $Change: 17027196 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
#
#===============================================================================
import os

def exists(env):
   True

def generate(env):
  scl_bld1 = env.Builder(action=env.Action('$$CCPPCOM',"=== Preprocess $${BUILDPATH}/$$SOURCE.name"), suffix='.tempscl', src_suffix='.ld')
  scl_bld2 = env.Builder(action='$$PPASM -f $$SOURCE -o $$TARGET',suffix='.scl',src_suffix='.tempscl')
  env.Append(BUILDERS = {'_SclBuilder1' : scl_bld1})
  env.Append(BUILDERS = {'_SclBuilder2' : scl_bld2})
  def SclBuilder(env,target, source,*args,**kwargs):
    temp_target = os.path.join(os.path.dirname(target),os.path.splitext(os.path.basename(source))[0])
    step1 = env._SclBuilder1(source=source,target=temp_target,*args,**kwargs)
    return env._SclBuilder2(source=step1, target=target,*args,**kwargs)
  env.AddMethod(SclBuilder,'SclBuilder')

