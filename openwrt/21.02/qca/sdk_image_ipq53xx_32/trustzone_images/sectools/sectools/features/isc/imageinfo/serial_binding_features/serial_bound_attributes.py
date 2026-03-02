# ===============================================================================
#
#  Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from functools import partial

from sectools.common.utils.datautils.enumerated_types import ENABLE, ENABLE_BOUND
from sectools.common.utils.datautils.enumerated_types import enumerated_values
from sectools.features.isc.cfgparser import defines
from sectools.features.isc.cfgparser.auto_gen_xml_config import complex_multi_serial_numbers
from sectools.features.isc.cfgparser.defines import ENUMERATED_DEBUG_MAPPING
from sectools.features.isc.defines import SECBOOT_VERSION_1_0


def debug_single_serial_bound(unconditional=False):
    def wrapper(func):
        def decorated(cls, config):
            if len(config.valid_serial_numbers) == 1:
                debug = int(config.general_properties.debug, 16)
                if unconditional:
                    # Debug enabled unconditionally, bound to one serial number
                    debug = ENUMERATED_DEBUG_MAPPING[ENABLE][SECBOOT_VERSION_1_0]
                if debug == ENUMERATED_DEBUG_MAPPING[ENABLE][SECBOOT_VERSION_1_0]:
                    debug |= int(config.valid_serial_numbers[0], 16) << 32
                config.general_properties.debug = "0x%016X" % debug
                config.general_properties.multi_serial_numbers = \
                    complex_multi_serial_numbers([])
            func(cls, config)
        return decorated
    return wrapper


class DebugUnconditionalSingleSerialBound(object):
    """ Enable debug attribute when a single serial number is provided. """
    @classmethod
    @debug_single_serial_bound(unconditional=True)
    def bind(cls, config):
        super(DebugUnconditionalSingleSerialBound, cls).bind(config)


class DebugConditionalSingleSerialBound(object):
    """ Bind debug attribute to a single serial number if debug is enabled. """
    @classmethod
    @debug_single_serial_bound(unconditional=False)
    def bind(cls, config):
        super(DebugConditionalSingleSerialBound, cls).bind(config)


def debug_multi_serial_bound(unconditional=False):
    def wrapper(func):
        def decorated(cls, config):
            if len(config.valid_serial_numbers) > 1:
                debug = int(config.general_properties.debug, 16)
                if unconditional:
                    # Debug enabled unconditionally, bound to one or more serial numbers
                    debug = ENUMERATED_DEBUG_MAPPING[ENABLE][SECBOOT_VERSION_1_0]
                config.general_properties.debug = "0x%016X" % debug
                if debug == ENUMERATED_DEBUG_MAPPING[ENABLE][SECBOOT_VERSION_1_0]:
                    config.general_properties.multi_serial_numbers = \
                        complex_multi_serial_numbers(config.valid_serial_numbers)
            func(cls, config)
        return decorated
    return wrapper


class DebugUnconditionalMultiSerialBound(object):
    """ Enable debug attribute if multiple serial numbers are provided. """
    @classmethod
    @debug_multi_serial_bound(unconditional=True)
    def bind(cls, config):
        super(DebugUnconditionalMultiSerialBound, cls).bind(config)


class DebugConditionalMultiSerialBound(object):
    """ Bind debug attribute to multiple serial numbers if debug is enabled. """
    @classmethod
    @debug_multi_serial_bound(unconditional=False)
    def bind(cls, config):
        super(DebugConditionalMultiSerialBound, cls).bind(config)


class SingleMultiSerialBound(object):
    """ Bind the device to one or more serial numbers. """
    @classmethod
    def bind(cls, config):
        config.general_properties.use_serial_number_in_signing = 0
        config.general_properties.multi_serial_numbers = \
            complex_multi_serial_numbers(config.valid_serial_numbers)
        super(SingleMultiSerialBound, cls).bind(config)


class SingleSerialBound(object):
    """ Bind the device to a single serial number. """
    @classmethod
    def bind(cls, config):
        if len(config.valid_serial_numbers) == 1:
            config.general_properties.use_serial_number_in_signing = 1
            config.general_properties.serial_number = config.valid_serial_numbers[0]
            config.general_properties.multi_serial_numbers = \
                complex_multi_serial_numbers([])
        super(SingleSerialBound, cls).bind(config)


def single_serial_number_binding(binding_value, attr_name):
    def wrapper(func):
        def decorated(cls, config):
            attr_value = int(getattr(config.general_properties, attr_name), 16)
            if attr_value == enumerated_values[defines.ENUMERATED_TAG_MAPPING[attr_name]][binding_value]:
                if len(config.valid_serial_numbers) > 1:
                    raise RuntimeError("%s cannot bind to multiple serial numbers for chipset %s." %
                                       (attr_name, config.chipset))
                # Bind one and only serial number to the signing attribute.
                attr_value |= int(config.valid_serial_numbers[0], 16) << 32
            setattr(config.general_properties, attr_name, "0x%016x" % attr_value)
            func(cls, config)
        return decorated
    return wrapper


def serial_binding_not_supported(binding_value, attr_name):
    def wrapper(func):
        def decorated(cls, config):
            attr_value = int(getattr(config.general_properties, attr_name), 16)
            if attr_value == enumerated_values[defines.ENUMERATED_TAG_MAPPING[attr_name]][binding_value]:
                raise RuntimeError("%s serial binding is not supported for chipset %s." %
                                   (attr_name, config.chipset))
            func(cls, config)
        return decorated
    return wrapper


enable_bound_hex64_single_serial_number = partial(single_serial_number_binding, ENABLE_BOUND)
enable_bound_hex64_not_supported = partial(serial_binding_not_supported, ENABLE_BOUND)


class ActivationEnablementSingleNoMultiSerialBound(object):

    @classmethod
    @enable_bound_hex64_single_serial_number(defines.ACTIVATION_ENABLEMENT_TAG)
    def bind(cls, config):
        super(ActivationEnablementSingleNoMultiSerialBound, cls).bind(config)


class ActivationEnablementNotSupported(object):

    @classmethod
    @enable_bound_hex64_not_supported(defines.ACTIVATION_ENABLEMENT_TAG)
    def bind(cls, config):
        super(ActivationEnablementNotSupported, cls).bind(config)


class RevocationEnablementSingleNoMultiSerialBound(object):

    @classmethod
    @enable_bound_hex64_single_serial_number(defines.REVOCATION_ENABLEMENT_TAG)
    def bind(cls, config):
        super(RevocationEnablementSingleNoMultiSerialBound, cls).bind(config)


class RevocationEnablementNotSupported(object):

    @classmethod
    @enable_bound_hex64_not_supported(defines.REVOCATION_ENABLEMENT_TAG)
    def bind(cls, config):
        super(RevocationEnablementNotSupported, cls).bind(config)


class RootRevokeActivateEnableSingleNoMultiSerialBound(object):

    @classmethod
    @enable_bound_hex64_single_serial_number(defines.ROOT_REVOKE_ACTIVATE_ENABLE_TAG)
    def bind(cls, config):
        super(RootRevokeActivateEnableSingleNoMultiSerialBound, cls).bind(config)


class RootRevokeActivateEnableNotSupported(object):

    @classmethod
    @enable_bound_hex64_not_supported(defines.ROOT_REVOKE_ACTIVATE_ENABLE_TAG)
    def bind(cls, config):
        super(RootRevokeActivateEnableNotSupported, cls).bind(config)


class UIEKeySwitchEnableSingleNoMultiSerialBound(object):

    @classmethod
    @enable_bound_hex64_single_serial_number(defines.UIE_KEY_SWITCH_ENABLE_TAG)
    def bind(cls, config):
        super(UIEKeySwitchEnableSingleNoMultiSerialBound, cls).bind(config)


class UIEKeySwitchEnableNotSupported(object):

    @classmethod
    @enable_bound_hex64_not_supported(defines.UIE_KEY_SWITCH_ENABLE_TAG)
    def bind(cls, config):
        super(UIEKeySwitchEnableNotSupported, cls).bind(config)


enable_hex64_single_serial_bound = partial(single_serial_number_binding, ENABLE)
enable_hex64_not_supported = partial(serial_binding_not_supported, ENABLE)


class RotEnSingleNoMultiSerialBound(object):

    @classmethod
    @enable_hex64_single_serial_bound(defines.ROT_EN_TAG)
    def bind(cls, config):
        super(RotEnSingleNoMultiSerialBound, cls).bind(config)


class RotEnNotSupported(object):

    @classmethod
    @enable_hex64_not_supported(defines.ROT_EN_TAG)
    def bind(cls, config):
        super(RotEnNotSupported, cls).bind(config)


class CrashDumpSingleNoMultiSerialBound(object):

    @classmethod
    @enable_hex64_single_serial_bound(defines.CRASH_DUMP_TAG)
    def bind(cls, config):
        super(CrashDumpSingleNoMultiSerialBound, cls).bind(config)


class CrashDumpNotSupported(object):

    @classmethod
    @enable_hex64_not_supported(defines.CRASH_DUMP_TAG)
    def bind(cls, config):
        super(CrashDumpNotSupported, cls).bind(config)
