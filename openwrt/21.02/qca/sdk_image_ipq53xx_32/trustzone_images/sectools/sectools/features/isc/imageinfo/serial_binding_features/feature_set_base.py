# ===============================================================================
#
#  Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from abc import ABCMeta

from sectools.common.core.plugin import SecPluginIntf_Abs
from sectools.common.utils.c_data import remove_duplicates_ordered
from sectools.features.isc.cfgparser.chipset_profiles import CHIPSET_PROFILES
from . import serial_bound_feature_manager


class FeatureSetBase(SecPluginIntf_Abs):

    __metaclass__ = ABCMeta

    feature_id = None

    def __init__(self):
        self.msm_parts = []
        self.chipset_soc_vers = []
        for chipset in self.allowed_chipsets():
            self.msm_parts += [x[3:-3] for x in CHIPSET_PROFILES[chipset]["msm_part"]]
            # Make sure soc_vers only use lowercase letters. The is_matched method
            # uses the same to find a matching feature set.
            self.chipset_soc_vers += [
                x.lower() for x in CHIPSET_PROFILES[chipset].get("soc_vers", [])]

    @classmethod
    def get_plugin_manager(cls):
        return serial_bound_feature_manager

    @classmethod
    def is_plugin(cls):
        return False

    @classmethod
    def get_plugin_id(cls):
        return cls.feature_id

    @classmethod
    def max_serial_number_count(cls):
        raise NotImplementedError("max_serial_number_count must be implemented")

    @classmethod
    def check_max_serial_number_count(cls, config):
        max_serial_number_count = cls.max_serial_number_count()
        if len(config.valid_serial_numbers) > max_serial_number_count:
            verb_be = "is" if max_serial_number_count == 1 else "are"
            raise RuntimeError(
                "{0} serials were provided. A maximum of {1} {2} allowed.".format(
                    len(config.valid_serial_numbers), max_serial_number_count, verb_be))

    def is_matched(self, config):

        if config.general_properties.in_use_soc_hw_version == 0:
            msm_part = config.general_properties.msm_part
            if any([x.upper() in msm_part.upper()[3:-3] for x in self.msm_parts]):
                return True
        else:
            soc_vers = []
            if config.general_properties.soc_vers:
                soc_vers.extend(config.general_properties.soc_vers)
            soc_hw_version = config.general_properties.soc_hw_version
            if soc_hw_version:
                soc_vers.append(soc_hw_version[:-4])
            soc_vers = remove_duplicates_ordered(*soc_vers)
            if soc_vers:
                return (set([x.lower() for x in soc_vers]) -
                        set(self.chipset_soc_vers)) == set()
        return False

    @classmethod
    def allowed_chipsets(cls):
        raise NotImplementedError("allowed_chipsets must be implemented")

    @classmethod
    def bind(cls, config):
        """ Terminate the super-chain of bind method of all feature implementations. """
        pass
