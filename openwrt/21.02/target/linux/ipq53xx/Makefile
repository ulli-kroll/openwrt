include $(TOPDIR)/rules.mk

ARCH:=aarch64
BOARD:=ipq53xx
BOARDNAME:=Qualcomm Technologies, Inc. IPQ53XX
FEATURES:=squashfs fpu ramdisk nand pcie usbgadget
CPU_TYPE:=cortex-a53
CPU_SUBTYPE:=neon-vfpv4
SUBTARGETS:=generic ipq53xx_32


KERNELNAME:=Image dtbs
LINUX_SRC_VERSION:=$(shell basename $(TOPDIR)/qca/src/linux-*.* | sed 's/[^0-9.]*\([0-9.]*\).*/\1/')

ifeq ($(LINUX_SRC_VERSION),.)
  LINUX_SRC_VERSION:=$(subst _,.,$(subst CONFIG_LINUX_,,$(subst =y,,$(shell grep CONFIG_LINUX_[0-9]_[0-9] $(TOPDIR)/.config ))))
endif

ifeq ($(CONFIG_LINUX_IS_FLATTENED_TREE),)
  KERNEL_PATCHVER=$(LINUX_SRC_VERSION)
else
  KERNEL_PATCHVER:=$(LINUX_SRC_VERSION)
  # check .config for an overridden Linux dir, otherwise use qca/src/linux
  CONFIG_EXTERNAL_KERNEL_TREE=$(shell \
          grep -q CONFIG_EXTERNAL_KERNEL_TREE="[^\/]*.\/" $(TOPDIR)/.config && \
          sed -n 's,CONFIG_EXTERNAL_KERNEL_TREE="\(.*\)",\1,p' $(TOPDIR)/.config || \
          echo '$(TOPDIR)/qca/src/linux-$(LINUX_SRC_VERSION)')

  #CONFIG_EXTERNAL_KERNEL_TREE=$(shell echo '$(TOPDIR)/qca/src/linux-$(LINUX_SRC_VERSION)')
  ifneq ($(CONFIG_EXTERNAL_KERNEL_TREE),"")
    $(eval _LINUX_DIR:=$(CONFIG_EXTERNAL_KERNEL_TREE))
    $(shell touch $(_LINUX_DIR)/.scmversion)
    _LINUX_MAJOR:=$(shell sed -n 's,VERSION = \([0-9]*\)$$,\1,p' $(_LINUX_DIR)/Makefile)
    _LINUX_MINOR:=$(shell sed -n 's,PATCHLEVEL = \([0-9]*\)$$,\1,p' $(_LINUX_DIR)/Makefile)
    _LINUX_SUBLEVEL:=$(shell sed -n 's,SUBLEVEL = \([0-9]*\)$$,\1,p' $(_LINUX_DIR)/Makefile)
    _LINUX_EXTRAVERSION:=$(shell sed -n 's,EXTRAVERSION = \([-rc0-9]*\)$$,\1,p' $(_LINUX_DIR)/Makefile)
    LINUX_VERSION:=$(_LINUX_MAJOR).$(_LINUX_MINOR)$(if $(_LINUX_SUBLEVEL),.$(_LINUX_SUBLEVEL))$(if $(_LINUX_EXTRAVERSION),$(_LINUX_EXTRAVERSION))
  endif
endif

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += \
	uboot-envtools kmod-leds-gpio kmod-gpio-button-hotplug swconfig


DEVICE_PROFILE:=$(call qstrip,$(CONFIG_TARGET_PROFILE))
DEVICE_DTS_DIR:=$(PLATFORM_SUBDIR)/$(subst DEVICE_,,$(DEVICE_PROFILE))
OemKernelDTS := $(DEVICE_DTS_DIR)/*.dts
OemKernelDTSI := $(DEVICE_DTS_DIR)/*.dtsi

#$(error target/linux debug PLATFORM_SUBDIR:$(PLATFORM_SUBDIR), DEVICE_DTS_DIR:$(DEVICE_DTS_DIR))


#copy from include/kernel-build.mk, and redefine Kernel/CompileImage to add copy command
define Kernel/CompileImage
	if [ \! -e $(OemKernelDTS) ]; then \
		echo "ERROR:file $(OemKernelDTS) was NOT found "; \
	else \
        cp -rf $(OemKernelDTS) $(LINUX_DIR)/arch/arm64/boot/dts/qcom; \
	fi;

	if [ \! -e $(OemKernelDTSI) ]; then \
		echo "ERROR:file $(OemKernelDTSI) was NOT found "; \
	else \
		cp -rf $(OemKernelDTSI) $(LINUX_DIR)/arch/arm64/boot/dts/qcom; \
	fi;
	$(call Kernel/CompileImage/Default)
	$(call Kernel/CompileImage/Initramfs)
endef

#copy from include/kernel-defaults.mk, and redefine Kernel/Prepare/Default to add copy command
ifeq ($(strip $(CONFIG_EXTERNAL_KERNEL_TREE)),"")
  ifeq ($(strip $(CONFIG_KERNEL_GIT_CLONE_URI)),"")
    define Kernel/Prepare/Default
	$(LINUX_CAT) $(DL_DIR)/$(LINUX_SOURCE) | $(TAR) -C $(KERNEL_BUILD_DIR) $(TAR_OPTIONS)
	$(Kernel/Patch)
	$(if $(QUILT),touch $(LINUX_DIR)/.quilt_used)
	if [ \! -e $(OemKernelDTS) ]; then \
		echo "ERROR:file $(OemKernelDTS) was NOT found "; \
	else \
        cp -rf $(OemKernelDTS) $(LINUX_DIR)/arch/arm64/boot/dts/qcom; \
	fi;

	if [ \! -e $(OemKernelDTSI) ]; then \
		echo "ERROR:file $(OemKernelDTSI) was NOT found "; \
	else \
		cp -rf $(OemKernelDTSI) $(LINUX_DIR)/arch/arm64/boot/dts/qcom; \
	fi;
    endef
  else
    define Kernel/Prepare/Default
	$(LINUX_CAT) $(DL_DIR)/$(LINUX_SOURCE) | $(TAR) -C $(KERNEL_BUILD_DIR) $(TAR_OPTIONS)
	if [ \! -e $(OemKernelDTS) ]; then \
		echo "ERROR:file $(OemKernelDTS) was NOT found "; \
	else \
        cp -rf $(OemKernelDTS) $(LINUX_DIR)/arch/arm64/boot/dts/qcom/; \
	fi;

	if [ \! -e $(OemKernelDTSI) ]; then \
		echo "ERROR:file $(OemKernelDTSI) was NOT found "; \
	else \
		cp -rf $(OemKernelDTSI) $(LINUX_DIR)/arch/arm64/boot/dts/qcom/; \
	fi;
    endef
  endif
else
  define Kernel/Prepare/Default
	mkdir -p $(KERNEL_BUILD_DIR)
	if [ -d $(LINUX_DIR) ]; then \
		rmdir $(LINUX_DIR); \
	fi
	cp -r $(CONFIG_EXTERNAL_KERNEL_TREE) $(LINUX_DIR)
	if [ -d $(LINUX_DIR)/user_headers ]; then \
		rm -rf $(LINUX_DIR)/user_headers; \
	fi
	if [ \! -e $(OemKernelDTS) ]; then \
		echo "ERROR:file $(OemKernelDTS) was NOT found "; \
	else \
        cp -rf $(OemKernelDTS) $(LINUX_DIR)/arch/arm64/boot/dts/; \
	fi;

	if [ \! -e $(OemKernelDTSI) ]; then \
		echo "ERROR:file $(OemKernelDTSI) was NOT found "; \
	else \
		cp -rf $(OemKernelDTSI) $(LINUX_DIR)/arch/arm64/boot/dts/; \
	fi;
  endef
endif



$(eval $(call BuildTarget))
