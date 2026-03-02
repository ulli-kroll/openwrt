# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2012-2015 OpenWrt.org
# Copyright (C) 2016-2017 LEDE project

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

BUILD_DATE:=$(shell cat $(TARGET_DIR)/etc/rom_version)

define Build/sysupgrade-emmc
	rm -f $@.recovery
	mkfs.fat -C $@.recovery 3070

	./gen_$(SUBTARGET)_emmc_img.sh $@ \
		$(IMAGE_KERNEL) \
		$@.recovery \
		$(IMAGE_ROOTFS)
endef

# build squashfs-hashed
define Build/squashfs-hashed
	$(CP) $(ROOTFS/$(FILESYSTEMS)/$(DEVICE_NAME)) $(ROOTFS/$(FILESYSTEMS)/$(DEVICE_NAME))-hashed
	$(TOPDIR)/scripts/make-squashfs-hashed.sh \
		$(ROOTFS/$(FILESYSTEMS)/$(DEVICE_NAME))-hashed \
		$(STAGING_DIR_HOST) \
		$(TOPDIR) \
		$(ROOTFS/$(FILESYSTEMS)/$(DEVICE_NAME))-hashed-summary
	fdt-patch-dm-verify $(ROOTFS/$(FILESYSTEMS)/$(DEVICE_NAME))-hashed-summary \
		$(KDIR)/image-$(firstword $(DEVICE_DTS)).dtb $(KDIR)/image-sb-$(firstword $(DEVICE_DTS)).dtb \
		$(HASHED_BOOT_DEVICE)
endef

# build fw-ar-ver
get_fw_ar_ver = \
  $(if $(wildcard $(2)),$(shell rm -rf $(2))) \
  $(if $(wildcard $(1)),$(info $(shell $(STAGING_DIR_HOST)/bin/ar-tool fw_ar_table create_ar_conf $(1) $(2)))) \
  $(if $(wildcard $(2)),$(eval include $(2))) \
  $(if $(FW_AR_VER),$(info FW_AR_VER = $(FW_AR_VER)))

define Build/fw-ar-ver
	$(call get_fw_ar_ver,$(ANTI_ROLLBACK_TABLE),$(AUTO_AR_CONF))
endef

define append-metadata
	$(if $(SUPPORTED_DEVICES),-echo $(call metadata_json,$(SUPPORTED_DEVICES)) | fwtool -I - $(1))
endef

#sysupagrade.bin: [bootloader + sig(256 byte) + (2KB-256B) + ( firmware(kernel_fit_image + rootfs)+ metadata )]
define Build/append-uboot
	mkdir -p $(BIN_DIR)/../$(DEVICE_NAME)
	$(CP) $@ $(BIN_DIR)/$(DEVICE_NAME)-squashfs-sysupgrade.bin
	(dd if=$(BIN_DIR)/openwrt-mediatek-mt7981-$(DEVICE_NAME)-uboot-atf-mtk.bin bs=$(FIP_SIZE) conv=sync;) >$(BIN_DIR)/$(IMG_PREFIX)-$(DEVICE_NAME)-FIP-tmp.bin 
	$(STAGING_DIR_HOST)/bin/oem-sign -b $(BIN_DIR)/$(IMG_PREFIX)-$(DEVICE_NAME)-FIP-tmp.bin -s $@ -o $(BIN_DIR)/$(IMG_PREFIX)-$(DEVICE_NAME)-sysupgrade-sign -k $(TOPDIR)/oem-keys/oem.key
    dd if=$(BIN_DIR)/$(IMG_PREFIX)-$(DEVICE_NAME)-sysupgrade-sign of=$(BIN_DIR)/$(IMG_PREFIX)-$(DEVICE_NAME)-FIP-tmp.bin bs=1 seek=$(FIP_SIZE) conv=sync
	(dd if=$(BIN_DIR)/$(IMG_PREFIX)-$(DEVICE_NAME)-FIP-tmp.bin bs=$(FIRMWARE_OFFSET) conv=sync;) >$(BIN_DIR)/$(IMG_PREFIX)-$(DEVICE_NAME)-sysupgrade-tmp.bin 
	cat $(BIN_DIR)/$(IMG_PREFIX)-$(DEVICE_NAME)-sysupgrade-tmp.bin $(BIN_DIR)/$(DEVICE_NAME)-squashfs-sysupgrade.bin >$(BIN_DIR)/../$(DEVICE_NAME)/$(DEVICE_NAME)-$(BUILD_DATE)-sysupgrade.bin
	#$(call append-metadata,$(BIN_DIR)/../$(DEVICE_NAME)/$(DEVICE_NAME)-$(BUILD_DATE)-sysupgrade.bin)
endef

#sysupagrade.bin: [bootloader + sig(256 byte) + (2KB-256B) + ( FIT{ubi(kernel_fit_image+rootfs)}+ metadata )]
define Build/append-uboot-ubi
	mkdir -p $(BIN_DIR)/../$(DEVICE_NAME)
	$(CP) $(PLATFORM_SUBDIR)/$(DEVICE_NAME)/sysupgrade-ubi.its $(BIN_DIR)/
	PATH=$(LINUX_DIR)/scripts/dtc:$(PATH) mkimage -f $(BIN_DIR)/sysupgrade-ubi.its $(BIN_DIR)/$(DEVICE_NAME)-squashfs-sysupgrade.bin
	$(call append-metadata,$(BIN_DIR)/$(DEVICE_NAME)-squashfs-sysupgrade.bin)
	(dd if=$(BIN_DIR)/openwrt-mediatek-mt7981-$(DEVICE_NAME)-uboot-atf-mtk.bin bs=$(FIP_SIZE) conv=sync;) >$(BIN_DIR)/$(IMG_PREFIX)-$(DEVICE_NAME)-FIP-tmp.bin 
	$(STAGING_DIR_HOST)/bin/oem-sign -b $(BIN_DIR)/$(IMG_PREFIX)-$(DEVICE_NAME)-FIP-tmp.bin -s $(BIN_DIR)/$(DEVICE_NAME)-squashfs-sysupgrade.bin -o $(BIN_DIR)/$(IMG_PREFIX)-$(DEVICE_NAME)-sysupgrade-sign -k $(TOPDIR)/oem-keys/oem.key
    dd if=$(BIN_DIR)/$(IMG_PREFIX)-$(DEVICE_NAME)-sysupgrade-sign of=$(BIN_DIR)/$(IMG_PREFIX)-$(DEVICE_NAME)-FIP-tmp.bin bs=1 seek=$(FIP_SIZE) conv=sync
	(dd if=$(BIN_DIR)/$(IMG_PREFIX)-$(DEVICE_NAME)-FIP-tmp.bin bs=$(FIRMWARE_OFFSET) conv=sync;) >$(BIN_DIR)/$(IMG_PREFIX)-$(DEVICE_NAME)-sysupgrade-tmp.bin 
	cat $(BIN_DIR)/$(IMG_PREFIX)-$(DEVICE_NAME)-sysupgrade-tmp.bin $(BIN_DIR)/$(DEVICE_NAME)-squashfs-sysupgrade.bin >$(BIN_DIR)/../$(DEVICE_NAME)/$(DEVICE_NAME)-$(BUILD_DATE)-sysupgrade.bin
endef

# build signed fit
define Build/fit-sign
	$(TOPDIR)/scripts/mkits.sh \
		-f $(DEVICE_NAME) \
		-D $(DEVICE_NAME) \
		-o $@.its \
		-k $@ \
		$(if $(word 2,$(1)),-d $(word 2,$(1))) -C $(word 1,$(1)) \
		-a $(KERNEL_LOADADDR) \
		-e $(if $(KERNEL_ENTRY),$(KERNEL_ENTRY),$(KERNEL_LOADADDR)) \
		-c $(if $(DEVICE_DTS_CONFIG),$(DEVICE_DTS_CONFIG),"config-1") \
		-A $(LINUX_KARCH) \
		-v $(LINUX_VERSION) \
		$(if $(FIT_KEY_NAME),-S $(FIT_KEY_NAME)) \
		$(if $(FW_AR_VER),-r $(FW_AR_VER)) \
		$(if $(CONFIG_TARGET_ROOTFS_SQUASHFS),-R $(ROOTFS/squashfs/$(DEVICE_NAME)))
	PATH=$(LINUX_DIR)/scripts/dtc:$(PATH) mkimage \
		-f $@.its \
		$(if $(FIT_KEY_DIR),-k $(FIT_KEY_DIR)) \
		-r \
		$@.new
	@mv $@.new $@
endef

# default all platform image(fit) build 
define Device/Default
  PROFILES = Default $$(DEVICE_NAME)
  KERNEL_NAME := Image
  KERNEL = kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS = kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  FILESYSTEMS := squashfs
  DEVICE_DTS_DIR := $(DTS_DIR)
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := append-kernel | pad-to 128k | append-rootfs | \
	pad-rootfs | append-metadata | append-uboot
  FIT_KEY_DIR :=
  FIT_KEY_NAME :=
endef

include $(SUBTARGET).mk

define Image/Flash/squashfs
	cp $(TOPDIR)/factory.bin $(BIN_DIR)
	mkdir -p $(BIN_DIR)/../$(1)
	./makeFlash.sh $(PLATFORM_SUBDIR)/$(1)/partition.info $(BIN_DIR) $(BIN_DIR)/../$(1)/$(1)-$(BUILD_DATE)-factory.bin
endef

define Image/Flash/squashfs/DEVICE_R31
	$(call Image/Flash/squashfs,R31)
endef

define Image/Flash/squashfs/DEVICE_R36
	$(call Image/Flash/squashfs,R36)
endef

define Image/Flash/squashfs/DEVICE_R37
        $(call Image/Flash/squashfs,R37)
		./maketag.sh $(BIN_DIR)/../R37/R37-$(BUILD_DATE)-factory.bin $(BIN_DIR)/../R37/R37-$(BUILD_DATE)-factory-tag.bin
endef

define Image/Flash/squashfs/DEVICE_R47
        $(call Image/Flash/squashfs,R47)
		./maketag.sh $(BIN_DIR)/../R47/R47-$(BUILD_DATE)-factory.bin $(BIN_DIR)/../R47/R47-$(BUILD_DATE)-factory-tag.bin
endef

define Image/Flash/squashfs/DEVICE_R49
        $(call Image/Flash/squashfs,R49)
		./maketag.sh $(BIN_DIR)/../R49/R49-$(BUILD_DATE)-factory.bin $(BIN_DIR)/../R49/R49-$(BUILD_DATE)-factory-tag.bin 2048 128
endef

define Image/Flash/squashfs/DEVICE_R50
        $(call Image/Flash/squashfs,R50)
		./maketag.sh $(BIN_DIR)/../R50/R50-$(BUILD_DATE)-factory.bin $(BIN_DIR)/../R50/R50-$(BUILD_DATE)-factory-tag.bin
endef

define Image/Flash/squashfs/DEVICE_R51
        $(call Image/Flash/squashfs,R51)
		./maketag.sh $(BIN_DIR)/../R51/R51-$(BUILD_DATE)-factory.bin $(BIN_DIR)/../R51/R51-$(BUILD_DATE)-factory-tag.bin
endef

define Image/Flash/squashfs/DEVICE_R53
        $(call Image/Flash/squashfs,R53)
		./maketag.sh $(BIN_DIR)/../R53/R53-$(BUILD_DATE)-factory.bin $(BIN_DIR)/../R53/R53-$(BUILD_DATE)-factory-tag.bin
endef

define Image/Flash/squashfs/DEVICE_R57
        $(call Image/Flash/squashfs,R57)
		./maketag.sh $(BIN_DIR)/../R57/R57-$(BUILD_DATE)-factory.bin $(BIN_DIR)/../R57/R57-$(BUILD_DATE)-factory-tag.bin 2048 128
endef

define Image/Flash/squashfs/DEVICE_R58
        $(call Image/Flash/squashfs,R58)
		./maketag.sh $(BIN_DIR)/../R58/R58-$(BUILD_DATE)-factory.bin $(BIN_DIR)/../R58/R58-$(BUILD_DATE)-factory-tag.bin
endef

define Image/Flash/squashfs/DEVICE_R59
        $(call Image/Flash/squashfs,R59)
		./maketag.sh $(BIN_DIR)/../R59/R59-$(BUILD_DATE)-factory.bin $(BIN_DIR)/../R59/R59-$(BUILD_DATE)-factory-tag.bin
endef

define Image/Flash/squashfs/DEVICE_R63
        $(call Image/Flash/squashfs,R63)
		./maketag.sh $(BIN_DIR)/../R63/R63-$(BUILD_DATE)-factory.bin $(BIN_DIR)/../R63/R63-$(BUILD_DATE)-factory-tag.bin
endef

define Image/Flash/squashfs/DEVICE_R68
        $(call Image/Flash/squashfs,R68)
		./maketag.sh $(BIN_DIR)/../R68/R68-$(BUILD_DATE)-factory.bin $(BIN_DIR)/../R68/R68-$(BUILD_DATE)-factory-tag.bin
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
	$(call Image/Flash/$(1)/$(PROFILE))
endef

$(eval $(call BuildImage))

