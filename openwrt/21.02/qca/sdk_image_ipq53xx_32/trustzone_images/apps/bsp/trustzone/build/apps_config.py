#===========================================================================
#  Copyright (c) 2016-2018,2021 Qualcomm Technologies Incorporated.
#  All Rights Reserved.
#  Qualcomm Confidential and Proprietary
#===========================================================================

#initialize empty dict
app_params_dict = dict()

# Initialize parameters for aptcryptotestapp
def addParams(app_name, app_dict):
  app_params_dict[app_name] = dict()
  app_params_dict[app_name] = app_dict

# Load the Secure app builder in the environment
def callInit(env, app_dict):
  #this can be used by the library to identify the app name in cleanpack
  env.Replace(APP_NAME = app_dict['aliases'][0])
  #this allows the apps to use their own name in cleanpack
  env[app_dict['aliases'][0]] = app_dict
  image_name=app_dict['aliases'][0].upper()
  env.Replace(IMAGE_ALIASES=app_dict['aliases'] + ['all'])
  env.Replace(BUILDPATH=env.subst(image_name).lower() + "/${PROC}/${BUILD_ID}")
  for t in app_dict['tags']:
    env[t] = True


#======================================================
# skeleton
skeleton_dict = {
  'aliases' : ['skeleton'],
  'arch' : 'A53_64',
  'tags' : ['SKELETON_IMAGE'],
  'sconspath' : '${BUILD_ROOT}/apps/securemsm/trustzone/qsapps/skeleton/src/SConscript',
}
addParams('skeleton', skeleton_dict)

