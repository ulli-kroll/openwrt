KERNEL_LOADADDR := 0x80100000

define Device/Default
  PROFILES := Default
  BLOCKSIZE := 64k
  FILESYSTEMS := squashfs
  DEVICE_DTS_DIR := ../dts
  KERNEL := kernel-bin | append-dtb | lzma | uImage lzma
  KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma | uImage lzma
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin = append-kernel | pad-to $$$$(BLOCKSIZE) | \
	append-rootfs | pad-rootfs | append-metadata
endef

define Device/siflower_sf19a2890-evb
  DEVICE_VENDOR := Siflower
  DEVICE_MODEL := SF19A2890 EVB
  BOARD_NAME := siflower,sf19a2890-evb
  DEVICE_DTS := sf19a2890_evb
  DEVICE_PACKAGES := kmod-switch-rtl8367b swconfig
endef
TARGET_DEVICES += siflower_sf19a2890-evb

define Device/bananapi_wifi5
  DEVICE_VENDOR := Bananapi
  DEVICE_MODEL := Wifi5
  BOARD_NAME := bananapi,wifi5
  DEVICE_DTS := sf19a2890_bananapi-wifi5
endef
TARGET_DEVICES += bananapi_wifi5

define Device/glinet_gl-sf1200
  DEVICE_VENDOR := GL.iNET
  DEVICE_MODEL := GL-SF1200
  BOARD_NAME := glinet,sf1200
  DEVICE_DTS := sf19a2890_glinet-sf1200
endef
TARGET_DEVICES += glinet_gl-sf1200

define Device/glinet_gl-sft1200
  DEVICE_VENDOR := GL.iNET
  DEVICE_MODEL := GL-SFT1200
  BOARD_NAME := glinet,sft1200
  DEVICE_DTS := sf19a2890_glinet-sft1200
endef
TARGET_DEVICES += glinet_gl-sft1200
