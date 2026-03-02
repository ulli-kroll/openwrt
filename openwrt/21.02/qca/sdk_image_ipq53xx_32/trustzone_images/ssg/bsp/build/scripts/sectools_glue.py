# ===============================================================================
#  Copyright (c) 2016,2020, 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
# ===============================================================================

import os
import atexit
from optparse import OptionParser
import sys

def exists(env):
   return env.Detect('sectools_glue')

def generate(env):
  env.AddCommandlineArgument(('SIGNTYPE','SIGNTYPE',''))
  env.AddCommandlineArgument(('SIGNSERVER','SIGNSERVER',''))
  env.AddCommandlineArgument(('SIGNPORT','SIGNPORT',''))

  env.AddMethod(build, "SectoolBuilderGlue")
  env.AddMethod(sign_for_qti, "QTIExclusiveSign")
  env.AddMethod(sign_for_oem, "OEMExclusiveSign")
  env.AddMethod(double_sign, "DoubleSign")
  # env.AddMethod(sign_for_qti_v2, "QTIExclusiveSign_v2")
  env.AddMethod(sign_for_oem_v2, "OEMExclusiveSign_v2")
  env.AddMethod(double_sign_v2, "DoubleSign_v2")

  if env.get('SIGNTYPE') == 'CASS':
    if not env.get('SIGNSERVER'):
      raise ValueError("Error: For SIGNTYPE CASS SIGNSERVER must be specified as \"SIGNSERVER = <hostname> \" ")
    else:
      os.environ['SECTOOLS_SIGNER_HOST'] = "http://" + env.get('SIGNSERVER')
      if env.get('SIGNPORT'):
        os.environ['SECTOOLS_SIGNER_PORT'] = env.get('SIGNPORT')
  #if env.get('SIGNSERVER'):
  #   os.environ['SECTOOLS_SIGNER_HOST'] = "http://" + env.get('SIGNSERVER')
  #   os.environ['SECTOOLS_SIGNER_PORT'] = env.get('SIGNPORT')


def get_profile(env, sign_id):
  # grep "image id=" trustzone_images/ssg/waipio_tz_security_profile.xml | cut -d "\"" -f 2 | sort -u
  if env.get('CHIPSET') == 'ipq90xx':
    profile = "${BUILD_ROOT}/ssg/Alder_tz_security_profile.xml"
  else:
    profile = "${BUILD_ROOT}/ssg/Miami_tz_security_profile.xml"
  return profile

# def create_unsigned_v2(env, input_elf, output_elf, sign_id):
#   sectools = "/prj/sectools/v2/latest/Linux/sectools"
#   profile = "/local/mnt/workspace/tomz/builds/waipio_sign_v7/trustzone_images/ssg/imports/waipio_tme_security_profile.xml"
#   env.Command(output_elf, input_elf, "{sectools} secure-image $SOURCE --outfile $TARGET --security-profile {profile} --image-id {sign_id} --sign --signing-mode TEST".format(sectools=sectools, profile=profile, sign_id=sign_id))
#   return output_elf


def sign_for_oem_v2(env, input_elf, output_elf_name, sign_id, app_id=None):
  profile = get_profile(env, sign_id)
  output_elf = os.path.join("${MBN_ROOT}", output_elf_name)
  # sectools = env['SECTOOLS']  # "/pkg/sectools/v2/latest/Linux/sectools"

  if app_id is None:
    app_id = ""
  else:
    app_id = "--secondary-software-id {}".format(app_id)

  retval = env.Command(output_elf, input_elf, "{sectools} secure-image $SOURCE --outfile $TARGET --security-profile {profile} --image-id {sign_id} {app_id} --sign --signing-mode TEST".format(sectools=env['SECTOOLS'], profile=profile, sign_id=sign_id, app_id=app_id))
  return retval


def double_sign_v2(env, input_elf, output_elf_name, sign_id, app_id=None):
  profile = get_profile(env, sign_id)
  output_elf = os.path.join("${MBN_ROOT}", output_elf_name)
  # sectools = env['SECTOOLS']  # "/pkg/sectools/v2/latest/Linux/sectools"

  if app_id is None:
    app_id = ""
  else:
    app_id = "--secondary-software-id {}".format(app_id)

  retval = env.Command(output_elf, input_elf, "{sectools} secure-image $SOURCE --outfile $TARGET --security-profile {profile} --image-id {sign_id} {app_id} --sign --qti \
    --signing-mode QTI-REMOTE \
    --cass-capability qtee_test_v2_qtee_release \
    --qti-remote-signing-server-port 50110 \
    --qti-remote-signing-server-url https://crm-sign \
     && \
    {sectools} secure-image $TARGET --outfile $TARGET --security-profile {profile} --image-id {sign_id} {app_id} --sign \
    --signing-mode TEST".format(sectools=env['SECTOOLS'], profile=profile, sign_id=sign_id, app_id=app_id))
  return retval


def create_unsigned(env, input, output, sign_id, app_id=None, config=None):
  if config is None:
    config = '${SECTOOLS_DIR}/config/integration/${SECIMAGE_CONFIG}'

  if 'USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN' in env:
     del env['USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN']
  env['USES_SEC_POLICY_DEFAULT_UNSIGN'] = True
  unsigned_mbn=env.SectoolBuilder(
    target_base_dir='${SHORT_BUILDPATH}',
    source=input,
    sign_id=sign_id,
    app_id=app_id,
    config=config,
    soc_hw_version=env.get("SOC_HW_VERSION"),
    soc_vers=env.get("SOC_VERS"),
    sectools_install_base_dir=os.path.join('${MBN_ROOT}', 'unsigned'),
    install_file_name=output
  )
  del env['USES_SEC_POLICY_DEFAULT_UNSIGN']
  return unsigned_mbn


def sign_for_qti(env, input, output, sign_id, app_id=None, config=None):
  if config is None:
    config = '${SECTOOLS_DIR}/config/integration/${SECIMAGE_CONFIG_QTI}'
  unsigned_mbn = create_unsigned(env, input, output, sign_id, app_id, config)

  env['USES_SEC_POLICY_DEFAULT_SIGN'] = True
  qti_signed_mbn=env.SectoolBuilder(
    target_base_dir='${SHORT_BUILDPATH}',
    source=unsigned_mbn,
    sign_id=sign_id,
    app_id=app_id,
    max_num_root_certs=env.get("MAX_NUM_ROOT_CERTS"),
    signer='qti_remote' if env.get('SIGNTYPE') == 'CASS' else 'local_v2',
    qti_sign=True,
    sectools_install_base_dir='${MBN_ROOT}',
    config=config,
    soc_hw_version=env.get("SOC_HW_VERSION"),
    soc_vers=env.get("SOC_VERS"),
    install_file_name=output
  )
  del env['USES_SEC_POLICY_DEFAULT_SIGN']
  return [unsigned_mbn, qti_signed_mbn]


def sign_for_oem(env, input, output, sign_id, app_id=None, config=None):
  if config is None:
    config = '${SECTOOLS_DIR}/config/integration/${SECIMAGE_CONFIG}'
  unsigned_mbn = create_unsigned(env, input, output, sign_id, app_id, config)

  env['USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN'] = True
  signed_mbn=env.SectoolBuilder(
    target_base_dir='${SHORT_BUILDPATH}',
    source=unsigned_mbn,
    sign_id=sign_id,
    app_id=app_id,
    max_num_root_certs=env.get("MAX_NUM_ROOT_CERTS"),
    sectools_install_base_dir='${MBN_ROOT}',
    config=config,
    soc_hw_version=env.get("SOC_HW_VERSION"),
    soc_vers=env.get("SOC_VERS"),
    install_file_name=output
  )
  return [unsigned_mbn, signed_mbn]


def double_sign(env, input, output, sign_id, app_id=None, config=None, config_qti=None):
  if config is None:
    config = '${SECTOOLS_DIR}/config/integration/${SECIMAGE_CONFIG}'
  if config_qti is None:
    config_qti = '${SECTOOLS_DIR}/config/integration/${SECIMAGE_CONFIG_QTI}'
  unsigned_mbn = create_unsigned(env, input, output, sign_id)

  env['USES_SEC_POLICY_DEFAULT_SIGN'] = True
  qti_signed_mbn=env.SectoolBuilder(
    target_base_dir='${SHORT_BUILDPATH}',
    source=unsigned_mbn,
    sign_id=sign_id,
    app_id=app_id,
    max_num_root_certs=env.get("MAX_NUM_ROOT_CERTS"),
    signer='qti_remote' if env.get('SIGNTYPE') == 'CASS' else 'local_v2',
    qti_sign=True,
    config=config_qti,
    soc_hw_version=env.get("SOC_HW_VERSION"),
    soc_vers=env.get("SOC_VERS"),
  )
  del env['USES_SEC_POLICY_DEFAULT_SIGN']

  env['USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN'] = True
  double_signed_mbn=env.SectoolBuilder(
    target_base_dir='${SHORT_BUILDPATH}',
    source=qti_signed_mbn,
    sign_id=sign_id,
    app_id=app_id,
    max_num_root_certs=env.get("MAX_NUM_ROOT_CERTS"),
    sectools_install_base_dir='${MBN_ROOT}',
    config=config,
    soc_hw_version=env.get("SOC_HW_VERSION"),
    soc_vers=env.get("SOC_VERS"),
    install_file_name=output
  )
  return [unsigned_mbn, qti_signed_mbn, double_signed_mbn]


def build(env,
    glue_target_base_dir,
    glue_source,
    glue_sign_id,
    glue_signer=None,
    glue_qti_sign=False,
    glue_sectools_install_base_dir=None,
    glue_install_file_name=None,
    glue_msmid=None,
    glue_msmid_jtagid_dict=None,
    glue_jtag_id=None,
    glue_config = None,
    glue_config_qti = None,
    glue_soc_hw_version=None,
    glue_app_id=None,
    glue_soc_vers = None,
    glue_max_num_root_certs = None,
    glue_is_step1 = False):

  if glue_is_step1:
    if 'USES_SEC_POLICY_STEP1_QC_SIGN' in env:
      if 'USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN' in env:
        del env['USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN']
      env['USES_SEC_POLICY_DEFAULT_SIGN'] = True
    sectools_signed_mbn_step1 = env.SectoolBuilder(
            target_base_dir = glue_target_base_dir,
            source=glue_source,
            sign_id=glue_sign_id,
            signer = glue_signer,
            qti_sign = glue_qti_sign,
            sectools_install_base_dir = glue_sectools_install_base_dir,
            install_file_name = glue_install_file_name,
            config = glue_config_qti  if glue_qti_sign else glue_config,
            soc_hw_version=glue_soc_hw_version,
            soc_vers=glue_soc_vers,
            target_image_type_filter = env.SectoolImageTypeSign(),
            max_num_root_certs = glue_max_num_root_certs)
    return  sectools_signed_mbn_step1
  else:
    if 'USES_SEC_POLICY_STEP2_OEM_SIGN' in env:
      if 'USES_SEC_POLICY_DEFAULT_SIGN' in env:
         del env['USES_SEC_POLICY_DEFAULT_SIGN']
      env['USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN'] = True
    sectools_signed_mbn_step2 = env.SectoolBuilder(
            target_base_dir = glue_target_base_dir,
            source=glue_source,
            sign_id=glue_sign_id,
            signer = glue_signer,
            qti_sign = glue_qti_sign,
            sectools_install_base_dir = glue_sectools_install_base_dir,
            install_file_name = glue_install_file_name,
            config= glue_config,
            soc_hw_version=glue_soc_hw_version,
            soc_vers=glue_soc_vers,
            max_num_root_certs = glue_max_num_root_certs)
    return sectools_signed_mbn_step2
