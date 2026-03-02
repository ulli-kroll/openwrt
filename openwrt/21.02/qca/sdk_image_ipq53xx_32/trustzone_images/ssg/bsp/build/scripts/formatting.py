#===============================================================================
#
# CoreBSP Text Formatting Rules
#
# GENERAL DESCRIPTION
#    Fortmatting text output defenition
#
# Copyright (c) 2009-2009 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/ssg.tzw/1.2/bsp/build/scripts/formatting.py#1 $
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
#
#===============================================================================
import sys
import os
import string
import atexit
import SCons.Action
import SCons.Script

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return True

def print_cmd_line_func(cmdline, targets, sources, env):
    if SCons.Script.ARGUMENTS.get('verbose',0) or int(env.GetOption('verbose')):
        print cmdline
    else:
        paths = [str(s) for s in targets]
        paths = [p.split('trustzone_images')[1] if 'trustzone_images' in p else p for p in paths]
        for p in paths:
          print "=== building " + p
   
def generate(env):
   env['PRINT_CMD_LINE_FUNC'] = print_cmd_line_func