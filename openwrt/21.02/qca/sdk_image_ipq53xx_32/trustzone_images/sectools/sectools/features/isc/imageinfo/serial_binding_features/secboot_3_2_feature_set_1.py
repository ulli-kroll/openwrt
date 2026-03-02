# ===============================================================================
#
#  Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.features.isc.advanced_defines import SECBOOT_VERSION_2_0
from .secboot_3_feature_set_1 import Secboot3FeatureSetBase
from .serial_bound_attributes import ActivationEnablementNotSupported
from .serial_bound_attributes import CrashDumpNotSupported
from .serial_bound_attributes import DebugConditionalMultiSerialBound
from .serial_bound_attributes import DebugConditionalSingleSerialBound
from .serial_bound_attributes import RevocationEnablementNotSupported
from .serial_bound_attributes import RootRevokeActivateEnableSingleNoMultiSerialBound
from .serial_bound_attributes import RotEnNotSupported
from .serial_bound_attributes import UIEKeySwitchEnableSingleNoMultiSerialBound
from .serial_bound_attributes import SingleMultiSerialBound
from ..serial_binding_features import MAX_NUM_SERIALS_MAP


class Secboot3_2FeatureSet1(DebugConditionalSingleSerialBound,
                            DebugConditionalMultiSerialBound,
                            SingleMultiSerialBound,
                            RotEnNotSupported,
                            UIEKeySwitchEnableSingleNoMultiSerialBound,
                            RevocationEnablementNotSupported,
                            ActivationEnablementNotSupported,
                            RootRevokeActivateEnableSingleNoMultiSerialBound,
                            CrashDumpNotSupported,
                            Secboot3FeatureSetBase):

    feature_id = "setboot_3_2_feature_set_1"

    def is_matched(self, config):
        if super(Secboot3_2FeatureSet1, self).is_matched(config):
            return config.general_properties.secboot_version == SECBOOT_VERSION_2_0
        return False

    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def max_serial_number_count(cls):
        return MAX_NUM_SERIALS_MAP[SECBOOT_VERSION_2_0]
