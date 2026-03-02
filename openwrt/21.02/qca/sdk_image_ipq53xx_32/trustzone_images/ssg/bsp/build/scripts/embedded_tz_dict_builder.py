#!/usr/bin/env python
# Copyright (c) 2018, 2019 Qualcomm Technologies, Inc.  All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.

"""
Parses the tz config file to determine which images need to be embedded with the 
TZ.MBN
"""
import os
import sys
import platform
import re

#----------------------------------------------------------------------------
# Hooks for Scons
#----------------------------------------------------------------------------
def exists(env):
  return True

def generate(env):
  env.AddMethod(parse_config_file, "ParseConfigFile")
  env.AddMethod(validate_embed_images, "ValidateEmbedImages")

def parse_config_file(env, path):
  ei_dict = {}
  #Parse the config file and construct the config dictionary
  if not os.path.isfile(path):
    raise Exception("Path %s must be a file" % path)
  file = open(path)
  try:
    lines = file.readlines()
    for line in lines:
      if line.find("#") == 0:
        continue
      ay = line.split("=")
      if (len(ay) <> 2):
        raise Exception("Unknown option: "  + line)
        #Treat all variables as hex. This works for both hex and bool types
      ei_dict[ay[0]] = int(ay[1])
  finally:
    file.close()
  
  return ei_dict
  
def validate_embed_images(env, ei_dict, global_dict_name):
  for key in ei_dict:
    #If the config enables the image to be embedded but there is no embed object error out
    if ei_dict[key] == 1 and key != 'tz_total_size':
      if (not (key in env['ROOT_GLOBAL_ENV']['EMBEDDED_IMAGES'][global_dict_name])) :
        print "Embedding image {0} without embed object".format(key)
        raise Exception("No embed object present")
    #if the config file has the image disabled remove the embed object
    elif ei_dict[key] == 0 and key != 'tz_total_segments':
      if key in env['ROOT_GLOBAL_ENV']['EMBEDDED_IMAGES'][global_dict_name]:
        #Do not embed this image, remove it from the dictionary
        env.RemoveEmbeddedImage(key, global_dict_name)

    #If there is an embed object but no entry in config file do not embed
    #This is done to prevent the case where incorrectly images are added in chipsets with
    #space constraints like Napali, where the config file won't have an entry but the individual
    #image sconscripts will add the embed object
    #The global dict needs to be deep copied since we cannot delete an entry from the dictionary and
    #iterate at the same time
    global_image_dict = env['ROOT_GLOBAL_ENV']['EMBEDDED_IMAGES'][global_dict_name].copy()
    for key in global_image_dict:
      if key.find('hyp') == -1 and key.find('mon') == -1 and key.find('qsee') == -1 and key != 'tz_total_segments':
        if key not in ei_dict:
          print "Image {0} not configured for embedding removing it from dictionary".format(key) 
          env.RemoveEmbeddedImage(key, global_dict_name)