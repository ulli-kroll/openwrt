include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

BUILD_DATE:=$(shell cat $(TARGET_DIR)/etc/rom_version)

UBIFS_OPTS = -m 2048 -e 124KiB -c 4096 -U -F
UBI_OPTS = -m 2048 -p 128KiB

IPQ53XX_KERNEL_SIZE=6044k
IPQ53XX_ROOTFS_SIZE=40036k
IPQ53XX_32_KERNEL_LOADADDR = 0x40008000
IPQ53XX_KERNEL_LOADADDR = 0x40080000

DEVICE_DTS_DIR := $(PLATFORM_SUBDIR)/$(subst DEVICE_,,$(PROFILE))
SUPPORTED_DEVICES := $(subst DEVICE_,,$(PROFILE))
BOARD_NAME:=$(SUPPORTED_DEVICES)

define CompressLzma
	$(STAGING_DIR_HOST)/bin/lzma e $(1) -lc1 -lp2 -pb2 $(2)
endef

define sizecheck
	@[ $$(($(subst k,* 1024,$(subst m, * 1024k,$(1))))) -ge "$$(stat -c%s $(2))" ] || { \
		echo "WARNING: Image file $(2) is too big, size is $$(stat -c%s $(2)) compare to max allowed $(1)" >&2; \
		rm -f $(2); \
	}
endef

define Image/BuildKernel/FIT
	$(CP) $(KDIR)/Image $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.bin
	gzip -9 -c $(KDIR)/Image > $(KDIR)/Image.gz
	$(call CompressLzma,$(KDIR)/Image,$(KDIR)/Image.lzma)
	$(call Image/BuildKernel/MkFIT,$(1), $(KDIR)/Image.lzma, $(DTS_DIR)/$(1).dtb,lzma,$(2),$(2))
	$(CP) $(KDIR)/fit-$(1).itb $(BIN_DIR)/$(IMG_PREFIX)-$(1)-fit-uImage.itb
	mkdir -p $(BIN_DIR)/dtbs/
	$(CP) $(DTS_DIR)/ipq5332*.dtb  $(BIN_DIR)/dtbs/
	$(CP) $(KDIR)/Image $(BIN_DIR)/dtbs/
endef

define Image/BuildKernel/FITInitramfs
	$(CP) $(KDIR)/Image-initramfs $(BIN_DIR)/dtbs/
	$(CP) $(KDIR)/Image-initramfs $(BIN_DIR)/$(IMG_PREFIX)-vmlinux-initramfs.bin
	$(call Image/BuildKernel/MkFIT,$(1), $(KDIR)/Image-initramfs, $(DTS_DIR)/$(1).dtb, none,$(2),$(2))
	$(CP) $(KDIR)/fit-$(1).itb $(BIN_DIR)/$(IMG_PREFIX)-$(1)-fit-uImage-initramfs.itb
endef

define Image/Prepare
	$(CP) $(LINUX_DIR)/vmlinux $(KDIR)/$(IMG_PREFIX)-vmlinux.elf
endef

define Image/BuildKernel/MultiDTBFIT
	$(CP) $(KDIR)/Image $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.bin
	gzip -9 -c $(KDIR)/Image > $(KDIR)/Image.gz
	$(foreach dtb, $(2), gzip -9 -c $(DTS_DIR)/$(dtb).dtb > $(DTS_DIR)/$(dtb).dtb.gz;)
	$(call CompressLzma,$(KDIR)/Image,$(KDIR)/Image.lzma)
	$(call Image/BuildKernel/MkFITMulti,$(1),$(KDIR)/Image.lzma,$(2),lzma,$(3),$(3),$(4),$(5))
	$$(if $$(CONFIG_DEBUG), ,$$(call sizecheck, $(IPQ53XX_KERNEL_SIZE), $(KDIR)/fit-$(1).itb))
	$(CP) $(KDIR)/fit-$(1).itb $(BIN_DIR)/$(IMG_PREFIX)-$(1)-fit-uImage.itb
endef

# some tree's don't have all the device tree's so we filter out
# the ones that are not present
# $(1) is the filter to apply
FindDeviceTrees ?= $(notdir $(basename $(shell find $(DTS_DIR)/ -name $(1)*.dtb $(2) 2>/dev/null)))

# For imagebuilder builds, check if debug file exists before copying
DebugImageName ?= $(KDIR)/vmlinux$(subst openwrt-ipq53xx$(if $(SUBTARGET),-$(SUBTARGET)),,$(IMG_PREFIX)).debug
DebugFileCheck ?= $(shell if [ -f $(call DebugImageName) ];then echo "1"; fi)

define Image/BuildKernel/template
	$(if $(CONFIG_IB_STANDALONE), $(if $(call DebugFileCheck), $(CP) $(call DebugImageName) $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.elf), $(CP) $(call DebugImageName) $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.elf)
endef

# Defined from cdt xml filename, for selecting dtb to be packed into image
BOARD_ID = $(shell cd $(DEVICE_DTS_DIR); stat -c %n *.dts | sed -e 's/\.dts//g' | cut -d '-' -f 2 | tr -t [A-Z] [a-z])

ifneq ($(findstring mi,$(BOARD_ID)),)
define Image/BuildKernel/generic
	$(call Image/BuildKernel/$(1),$(call FindDeviceTrees, ipq5332-$(BOARD_ID)), $(IPQ53XX_KERNEL_LOADADDR))


	$(call Image/BuildKernel/MultiDTBFIT,ipq5332-mixx, \
		$(call FindDeviceTrees, ipq5332-), $(IPQ53XX_KERNEL_LOADADDR))
endef

define Image/BuildKernel/ipq53xx_32
	$(call Image/BuildKernel/$(1),$(call FindDeviceTrees, ipq5332-$(BOARD_ID)), $(IPQ53XX_32_KERNEL_LOADADDR))


	$(call Image/BuildKernel/MultiDTBFIT,ipq5332-mixx, \
		$(call FindDeviceTrees, ipq5332-), $(IPQ53XX_32_KERNEL_LOADADDR))
endef

else

define Image/BuildKernel/ipq53xx_32
	$(foreach device, $(call FindDeviceTrees, ipq5332-), \
		$(call Image/BuildKernel/$(1),$(device),$(IPQ53XX_32_KERNEL_LOADADDR));)

	$(call Image/BuildKernel/MultiDTBFIT,ipq5332-mixx, \
		$(call FindDeviceTrees, ipq5332-), $(IPQ53XX_32_KERNEL_LOADADDR))
endef

define Image/BuildKernel/generic
	$(foreach device, $(call FindDeviceTrees, ipq5332-), \
		$(call Image/BuildKernel/$(1),$(device),$(IPQ53XX_KERNEL_LOADADDR));)

	$(call Image/BuildKernel/MultiDTBFIT,ipq5332-mixx, \
		$(call FindDeviceTrees, ipq5332-), $(IPQ53XX_KERNEL_LOADADDR))
endef

endif
define Image/BuildKernel
	$(call Image/BuildKernel/template,FIT)
	$(call Image/BuildKernel/$(SUBTARGET),FIT)
endef

define Image/BuildKernel/Initramfs
	$(CP) $(KDIR)/vmlinux-initramfs$(subst openwrt-ipq53xx$(if $(SUBTARGET),-$(SUBTARGET)),,$(IMG_PREFIX)).debug $(BIN_DIR)/$(IMG_PREFIX)-vmlinux-initramfs.elf
	$(call Image/BuildKernel/template,FITInitramfs)
	$(call Image/BuildKernel/$(SUBTARGET),FITInitramfs)
endef


define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
	$$(if $$(CONFIG_DEBUG), ,$$(call sizecheck, $(IPQ53XX_ROOTFS_SIZE), $(KDIR)/root.squashfs))
endef

SDK_QCA_DIR:=$(TOPDIR)/qca/sdk_image_$(SUBTARGET)
SDK_QCA_BUILD_DIR:=$(SDK_QCA_DIR)/common/build
SDK_QCA_OUT_DIR:=$(SDK_QCA_BUILD_DIR)/ipq
SDK_QCA_BUILD_BIN_DIR:=$(SDK_QCA_DIR)/common/build/bin

define Image/SDKPartition/ubifs
	if [ -e $(DEVICE_DTS_DIR)/config.xml ]; then \
		cp $(DEVICE_DTS_DIR)/config.xml $(SDK_QCA_DIR)/apss_proc/out/meta-tools/ipq5332/; \
	fi

	if [ -e $(DEVICE_DTS_DIR)/*_DDR*.xml ]; then \
		cp -f $(DEVICE_DTS_DIR)/*_DDR*.xml $(SDK_QCA_DIR)/apss_proc/out/meta-tools/ipq5332/cdt; \
	fi
	mkdir -p $(SDK_QCA_OUT_DIR)/
	rm -rf $(SDK_QCA_OUT_DIR)/*
	cp $(BIN_DIR)/openwrt* $(SDK_QCA_OUT_DIR)
	cp $(BIN_DIR)/dtbs/* $(SDK_QCA_OUT_DIR)
	cp $(STAGING_DIR_HOST)/bin/ubinize $(SDK_QCA_OUT_DIR)
	cp -rf $(SDK_QCA_DIR)/skales/* $(SDK_QCA_OUT_DIR)
	cp $(SDK_QCA_BUILD_DIR)/mksquashfs4 $(SDK_QCA_OUT_DIR)
	cp -rf $(SDK_QCA_DIR)/wlan_proc/build/ms/bin/5332.wlanfw.eval/FW_IMAGES/* $(SDK_QCA_OUT_DIR)
	cp -rf $(SDK_QCA_DIR)/TMEL.WNS.1.1/patch/signed/tmel-ipq53xx-patch.elf $(SDK_QCA_OUT_DIR)
	cd $(SDK_QCA_BUILD_DIR)/;export BLD_ENV_BUILD_ID=P; python update_common_info.py 32;
endef


define Image/Sysupgrade/ubifs
	mkdir -p $(BIN_DIR)/../$(1)
	$(CP) $(SDK_QCA_BUILD_BIN_DIR)/nand-ipq5332-apps.img $(BIN_DIR)/../$(1)/
	$(CP) $(SDK_QCA_BUILD_BIN_DIR)/nand-ipq5332-single.img $(BIN_DIR)/../$(1)/
endef


define Image/Build
	$(call Image/Build/$(1),$(1))
	$$(if $(CONFIG_USES_UBIFS), ,$$(call sizecheck, $(IPQ53XX_ROOTFS_SIZE), $(KDIR)/root$(2).$(1)))
	dd if=$(KDIR)/root$(2).$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-$(1)-root$(3).img bs=2k conv=sync
	$(call Image/SDKPartition/$(1))
	$(call Image/Sysupgrade/$(1)/$(PROFILE))
endef

$(eval $(call BuildImage))
