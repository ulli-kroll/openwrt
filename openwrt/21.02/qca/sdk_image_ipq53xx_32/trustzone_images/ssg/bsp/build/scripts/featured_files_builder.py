
#
# feature-driven source file list generator
#
# GENERAL DESCRIPTION
#    generate list of source files based on input feature list
#
# Copyright 2018 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#
#  $Header: //components/rel/ssg.tzw/1.2/bsp/build/scripts/featured_files_builder.py#1 $
#  $DateTime: 2020/11/02 23:12:26 $
#  $Author: pwbldsvc $
#  $Change: 27014951 $
#
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who       what, where, why
# --------   ---       ------------------------------------------------------
#
#============================================================================
#----------------------------------------------------------------------------
# Global values
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# Hooks for Scons
#----------------------------------------------------------------------------
def exists(env):
  return env.Detect('featured_files_builder')

def generate(env):
  env.AddMethod(generate_files_and_flags, "FeatureGenFilesAndFlags")
  env.AddMethod(featuredfiles_unit_test, "FeatureFilesUTest")

'''
============================================================================
Public API,
generate source file list and flag list based on feature set definition
env: scons environment
feature_list: the list of feature keywords
source_file_dict: the mapping from feature keywords to dependencies, which
could be other feature keywords, source files and feature flags
============================================================================
'''
def generate_files_and_flags(env, feature_list, source_file_dict):
	ff_env = getFeaturedFilesContext(env)
	ff_env.setFileDict(source_file_dict)
	set_files = set()
	set_flags = set()
	for key in feature_list:
		if key in ff_env.file_dict:
			files, flags = getFilesFlagsHelper(ff_env, key, ff_env.file_dict[key])
			set_files.update(files)
			set_flags.update(flags)
	return list(set_files), list(set_flags)

#============================================================================
# Supporting functions
#============================================================================
class featured_files_data:
	def __init__(self):
		self.debug_msg = False # debug flag for unit testing
		self.ut_run_times = 0  # # of times of running the unit test

	def setFileDict(self, source_file_dict):
		self.file_dict = dict(source_file_dict)
		for item in self.file_dict:
			if 'visited' in self.file_dict[item]:
				del self.file_dict[item]['visited'] # clear up for re-visiting feature nodes

# within build env, this should be singleton
def getFeaturedFilesContext(env):
	if 'ff_context' not in env:
		env['ff_context'] = featured_files_data()
	return env['ff_context']

# get a list of source files based on feature keyword
def getFilesFlagsHelper(env, s_key, d_feat_items):
	files=set()
	flags=set()

	if d_feat_items is None: return files, flags

	if 'visited' in d_feat_items:
		return files, flags  # visited only once

	d_feat_items['visited'] = 'True'

	if 'features' in d_feat_items:
		if isinstance(d_feat_items['features'], list):
			for feat_item in d_feat_items['features']:
				if feat_item in env.file_dict: # 
					sub_files, sub_flags = \
							getFilesFlagsHelper(env, feat_item, env.file_dict[feat_item])
					files.update(sub_files)
					flags.update(sub_flags)		
		else:
			feat_item = d_feat_items['features']
			if feat_item in env.file_dict:
				sub_files, sub_flags = getFilesFlagsHelper(env, feat_item, env.file_dict[feat_item])
				files.update(sub_files)
				flags.update(sub_flags)

	if 'files' in d_feat_items:
		if isinstance(d_feat_items['files'], list):
			for file_item in d_feat_items['files']:
				files.add(file_item)
		else:
			files.add(d_feat_items['files'])

	if 'flags' in d_feat_items:
		if isinstance(d_feat_items['flags'], list):
			for flag_item in d_feat_items['flags']:
				flags.add(flag_item)
		else:
			flags.add(d_feat_items['flags'])

	return files, flags

def verify_test_result(ff_env, msg, files, expected_files, flags, expected_flags):
	diff_file = set(expected_files).difference(files)
	diff_flag = set(expected_flags).difference(flags)
	test_pass = not diff_file and not diff_flag
	if ff_env.debug_msg:
		print("** INFO: featured_files_builder UT{0}: {1} TEST PASSED: {2} ".format(ff_env.ut_run_times, msg, test_pass))
	if not test_pass: 
		print("** ERROR: featured_files_builder UT{0}: {1} TEST FAILED".format(ff_env.ut_run_times, msg))
	return test_pass

'''
============================================================================
Public API, unit testing to verify functionality of generate_files_and_flags()
============================================================================
'''
def featuredfiles_unit_test(env):
	ff_env = getFeaturedFilesContext(env)

	if ff_env.ut_run_times >= 1:
		return True

	test_group_features = {
	  # adding keyword 'secapi' in group1 for testing dependency on parsed keyword
	  'group1' : ['default_general', 'secapi' , 'MSA_avail', 'pkhash_sha256', 'legacy_debug_disable'],
	  'group2' : ['default_general', 'MSA_removal', 'pkhash_sha384', 'legacy_debug_disable'],
	  'group3' : ['default_general', 'MSA_removal', 'pkhash_sha384', 'debug_disable_rearch'],
	  'group4' : ['stubs'],
	}

	# define mapping of feature key words to source files and feature flags
	feature_sources = {
	  'default_general' : { 'features' : ['MRC', 'anti_rollback', 'sec.dat', 'secapi'] },
	  'MRC' : {'files' : '${BUILDPATH}/src/tzbsp_mrc_fuseprov.c'},
	  'anti_rollback' : {'files' : '${BUILDPATH}/src/tzbsp_rollback_version_fuseprov.c' },
	  'sec.dat' : {'files' : [ '${BUILDPATH}/src/tzbsp_fuseprov.c', '${BUILDPATH}/src/tzbsp_fuseprov_utils.c']},
	  'secapi' : {'files' : '${BUILDPATH}/src/tzbsp_secctrl.c'},

	  'MSA_avail' : {'features' : 'secapi'},
	  'MSA_removal' : { 'features' : 'secapi',
	                    'flags' : 'SECAPI_CHECK_ONLY_APPS_AUTH_EN'
	                  },
	  'pkhash_sha256' : {'features' : 'secapi'},
	  'pkhash_sha384' : { 'features' : 'secapi',
	                      'flags' : 'SECAPI_CHECK_PK_HASH_SHA384'
	                    },
	  'legacy_debug_disable' : {'features' : 'secapi'},
	  'debug_disable_rearch' :  { 'features' : 'secapi',
	                              'files' : '${BUILDPATH}/src/secapi/tzbsp_secctrl_rearch_debug.c',
	                              'flags' : 'SECAPI_CHECK_REARCH_DEBUG_DISABLE'
	                            },
	  # stub files
	  'stubs' : { 'features' : ['no_MRC', 'anti_rollback', 'sec.dat', 'secapi', 'loop_item']},
	  'no_MRC' : {'files' : '${BUILDPATH}/src/stubs/tzbsp_mrc_fuseprov_stubs.c'},
	  'loop_item' : { 'features' : ['stubs']},
	}

	files, flags = generate_files_and_flags(env, test_group_features['group1'], feature_sources)
	expected_files1 = ['${BUILDPATH}/src/tzbsp_mrc_fuseprov.c', '${BUILDPATH}/src/tzbsp_rollback_version_fuseprov.c', 
						'${BUILDPATH}/src/tzbsp_fuseprov.c', '${BUILDPATH}/src/tzbsp_secctrl.c', 
						'${BUILDPATH}/src/tzbsp_fuseprov_utils.c']
	expected_flags1 = []
	if not verify_test_result(ff_env, 'featured files test for group1', files, expected_files1, flags, expected_flags1):
		return False

	files, flags = generate_files_and_flags(env, test_group_features['group2'], feature_sources)
	expected_files2 = expected_files1
	expected_flags2 = ['SECAPI_CHECK_PK_HASH_SHA384', 'SECAPI_CHECK_ONLY_APPS_AUTH_EN']
	if not verify_test_result(ff_env, 'featured files test for group2', files, expected_files2, flags, expected_flags2):
		return False

	files, flags = generate_files_and_flags(env, test_group_features['group3'], feature_sources)
	expected_files3 = expected_files2 + ['${BUILDPATH}/src/secapi/tzbsp_secctrl_rearch_debug.c']
	expected_flags3 = expected_flags2 + ['SECAPI_CHECK_REARCH_DEBUG_DISABLE']
	if not verify_test_result(ff_env, 'featured files test for group3', files, expected_files3, flags, expected_flags3):
		return False

	files, flags = generate_files_and_flags(env, test_group_features['group4'], feature_sources)
	expected_files4 = ['${BUILDPATH}/src/stubs/tzbsp_mrc_fuseprov_stubs.c', '${BUILDPATH}/src/tzbsp_rollback_version_fuseprov.c',
						'${BUILDPATH}/src/tzbsp_fuseprov.c', '${BUILDPATH}/src/tzbsp_secctrl.c', 
						'${BUILDPATH}/src/tzbsp_fuseprov_utils.c']
	expected_flags4 = []
	if not verify_test_result(ff_env, 'featured files test for group4', files, expected_files4, flags, expected_flags4):
		return False

	ff_env.ut_run_times += 1

	return True

'''
============================================================================
unit test outside build env, 
<local tz build>\trustzone_images\ssg\bsp\build\scripts>python featured_files_builder.py
============================================================================
'''
if __name__ == '__main__':
	env = dict()
	print('run the 1st time with debug_msg = False. tests should run but not print out test result')
	featuredfiles_unit_test(env)
	print('run the 2nd time with debug_msg = True, ut_run_times = 0. tests should run and print out test result')
	env['ff_context'].debug_msg = True
	env['ff_context'].ut_run_times = 0
	featuredfiles_unit_test(env)
	print('run the 2nd time with debug_msg = True, ut_run_times = 1, tests should not run, as ut_run_times is updated to 1')
	env['ff_context'].debug_msg = True
	featuredfiles_unit_test(env)
