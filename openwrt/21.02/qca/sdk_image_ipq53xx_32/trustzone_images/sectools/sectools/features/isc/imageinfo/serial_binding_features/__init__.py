# ===============================================================================
#
#  Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import os
import sys
from glob import glob

from sectools.common.core.plugin import SecPluginMgr
from sectools.common.utils.c_logging import logger
from sectools.features.isc.advanced_defines import SECBOOT_VERSION_2_0, SECBOOT_VERSION_3_0
from sectools.features.isc.defines import SECBOOT_VERSION_1_0

MAX_NUM_SERIALS_8 = 8
MAX_NUM_SERIALS_MAP = {
    SECBOOT_VERSION_1_0: 1,
    # Secboot version 2.0 and some version 1.0 images support
    # up to 200 serial numbers in cert SN OU field.
    SECBOOT_VERSION_2_0: 200,
    SECBOOT_VERSION_3_0: MAX_NUM_SERIALS_8,
}


class SerialBoundFeatureSetContextManager(object):

    def __init__(self, config):
        self.config = config
        self.feature_set = serial_bound_feature_manager.get_feature_set(config)

    def __enter__(self):
        self.feature_set.check_max_serial_number_count(self.config)
        return self.feature_set

    def __exit__(self, *args):
        pass


class SerialBoundFeatureManager(SecPluginMgr):

    # All serial-binding implementation instances are reused. They are kept in the list.
    feature_implementations = []

    def get_feature_set(self, config):
        if not self.feature_implementations:
            self.feature_implementations = [x() for x in self.get_map().values()]
        matched_implementations = [x for x in self.feature_implementations if
                                   x.is_matched(config) is True]
        if len(matched_implementations) == 1:
            return matched_implementations[0]
        logger.debug("matched_implementations: " + str(matched_implementations))
        raise RuntimeError("Serial binding is not supported for the given configuration")


serial_bound_feature_manager = SerialBoundFeatureManager()

# Import all serial-binding feature Python modules
for py_file_name in glob(os.path.join(os.path.dirname(__file__), "*.py")):
    module_name = os.path.splitext(os.path.basename(py_file_name))[0]
    if module_name in ["__init__"]:
        continue
    if hasattr(sys.modules[__name__], module_name):
        continue
    __import__(".".join([__name__, module_name]))
