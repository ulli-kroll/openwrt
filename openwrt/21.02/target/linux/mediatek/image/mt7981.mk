KERNEL_LOADADDR := 0x48080000

define Device/mt7981-spim-nand-2500wan-gmac2
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := mt7981-spim-nand-2500wan-gmac2
  DEVICE_DTS := mt7981-spim-nand-2500wan-gmac2
  DEVICE_DTS_DIR := $(DTS_DIR)/mediatek
  SUPPORTED_DEVICES := mediatek,mt7981-spim-snand-2500wan-gmac2-rfb
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += mt7981-spim-nand-2500wan-gmac2

define Device/R31
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := R31
  DEVICE_DTS := R31
  DEVICE_DTS_DIR := ../dts
  SUPPORTED_DEVICES := R31
  FIP_SIZE := 524288
  FIRMWARE_OFFSET := 526336
endef
TARGET_DEVICES += R31

define Device/R36
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := R36
  DEVICE_DTS := R36
  DEVICE_DTS_DIR := ../dts
  SUPPORTED_DEVICES := R36
  FIP_SIZE := 524288
  FIRMWARE_OFFSET := 526336
endef
TARGET_DEVICES += R36

define Device/R37
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := R37
  DEVICE_DTS := R37
  DEVICE_DTS_DIR := ../dts
  SUPPORTED_DEVICES := R37
  FIP_SIZE := 2097152
  FIRMWARE_OFFSET := 2099200
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata | append-uboot-ubi
endef
TARGET_DEVICES += R37

define Device/R47
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := R47
  DEVICE_DTS := R47
  DEVICE_DTS_DIR := ../dts
  SUPPORTED_DEVICES := R47
  FIP_SIZE := 2097152
  FIRMWARE_OFFSET := 2099200
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata | append-uboot-ubi
endef
TARGET_DEVICES += R47

define Device/R49
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := R49
  DEVICE_DTS := R49
  DEVICE_DTS_DIR := ../dts
  SUPPORTED_DEVICES := R49
  FIP_SIZE := 2097152
  FIRMWARE_OFFSET := 2099200
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata | append-uboot-ubi
endef
TARGET_DEVICES += R49

define Device/R50
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := R50
  DEVICE_DTS := R50
  DEVICE_DTS_DIR := ../dts
  SUPPORTED_DEVICES := R50
  FIP_SIZE := 2097152
  FIRMWARE_OFFSET := 2099200
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata | append-uboot-ubi
endef
TARGET_DEVICES += R50

define Device/R51
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := R51
  DEVICE_DTS := R51
  DEVICE_DTS_DIR := ../dts
  SUPPORTED_DEVICES := R51
  FIP_SIZE := 2097152
  FIRMWARE_OFFSET := 2099200
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata | append-uboot-ubi
endef
TARGET_DEVICES += R51

define Device/R53
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := R53
  DEVICE_DTS := R53
  DEVICE_DTS_DIR := ../dts
  SUPPORTED_DEVICES := R53
  FIP_SIZE := 2097152
  FIRMWARE_OFFSET := 2099200
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata | append-uboot-ubi
endef
TARGET_DEVICES += R53

define Device/R57
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := R57
  DEVICE_DTS := R57
  DEVICE_DTS_DIR := ../dts
  SUPPORTED_DEVICES := R57
  FIP_SIZE := 2097152
  FIRMWARE_OFFSET := 2099200
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata | append-uboot-ubi
endef
TARGET_DEVICES += R57

define Device/R58
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := R58
  DEVICE_DTS := R58
  DEVICE_DTS_DIR := ../dts
  SUPPORTED_DEVICES := R58
  FIP_SIZE := 2097152
  FIRMWARE_OFFSET := 2099200
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata | append-uboot-ubi
endef
TARGET_DEVICES += R58

define Device/R59
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := R59
  DEVICE_DTS := R59
  DEVICE_DTS_DIR := ../dts
  SUPPORTED_DEVICES := R59
  FIP_SIZE := 2097152
  FIRMWARE_OFFSET := 2099200
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata | append-uboot-ubi
endef
TARGET_DEVICES += R59

define Device/R63
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := R63
  DEVICE_DTS := R63
  DEVICE_DTS_DIR := ../dts
  SUPPORTED_DEVICES := R63
  FIP_SIZE := 2097152
  FIRMWARE_OFFSET := 2099200
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata | append-uboot-ubi
endef
TARGET_DEVICES += R63

define Device/R68
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := R68
  DEVICE_DTS := R68
  DEVICE_DTS_DIR := ../dts
  SUPPORTED_DEVICES := R68
  FIP_SIZE := 2097152
  FIRMWARE_OFFSET := 2099200
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata | append-uboot-ubi
endef
TARGET_DEVICES += R68

define Device/mt7981-spim-nand-rfb
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := mt7981-spim-nand-rfb
  DEVICE_DTS := mt7981-spim-nand-rfb
  DEVICE_DTS_DIR := $(DTS_DIR)/mediatek
  SUPPORTED_DEVICES := mediatek,mt7981-spim-snand-rfb
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += mt7981-spim-nand-rfb

define Device/mt7981-spim-nand-gsw
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := mt7981-spim-nand-gsw
  DEVICE_DTS := mt7981-spim-nand-gsw
  DEVICE_DTS_DIR := $(DTS_DIR)/mediatek
  SUPPORTED_DEVICES := mediatek,mt7981-rfb,ubi
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += mt7981-spim-nand-gsw

define Device/mt7981-emmc-rfb
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := mt7981-emmc-rfb
  DEVICE_DTS := mt7981-emmc-rfb
  SUPPORTED_DEVICES := mediatek,mt7981-emmc-rfb
  DEVICE_DTS_DIR := $(DTS_DIR)/mediatek
  DEVICE_PACKAGES := mkf2fs e2fsprogs blkid blockdev losetup kmod-fs-ext4 \
		     kmod-mmc kmod-fs-f2fs kmod-fs-vfat kmod-nls-cp437 \
		     kmod-nls-iso8859-1
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += mt7981-emmc-rfb

define Device/mt7981-sd-rfb
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := mt7981-sd-rfb
  DEVICE_DTS := mt7981-sd-rfb
  SUPPORTED_DEVICES := mediatek,mt7981-sd-rfb
  DEVICE_DTS_DIR := $(DTS_DIR)/mediatek
  DEVICE_PACKAGES := mkf2fs e2fsprogs blkid blockdev losetup kmod-fs-ext4 \
		     kmod-mmc kmod-fs-f2fs kmod-fs-vfat kmod-nls-cp437 \
		     kmod-nls-iso8859-1
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += mt7981-sd-rfb

define Device/mt7981-snfi-nand-2500wan-p5
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := mt7981-snfi-nand-2500wan-p5
  DEVICE_DTS := mt7981-snfi-nand-2500wan-p5
  DEVICE_DTS_DIR := $(DTS_DIR)/mediatek
  SUPPORTED_DEVICES := mediatek,mt7981-snfi-snand-pcie-2500wan-p5-rfb
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += mt7981-snfi-nand-2500wan-p5

define Device/mt7981-fpga-spim-nor
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := mt7981-fpga-spim-nor
  DEVICE_DTS := mt7981-fpga-spim-nor
  DEVICE_DTS_DIR := $(DTS_DIR)/mediatek
  SUPPORTED_DEVICES := mediatek,mt7981-fpga-nor
endef
TARGET_DEVICES += mt7981-fpga-spim-nor

define Device/mt7981-fpga-snfi-nand
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := mt7981-fpga-snfi-nand
  DEVICE_DTS := mt7981-fpga-snfi-nand
  DEVICE_DTS_DIR := $(DTS_DIR)/mediatek
  SUPPORTED_DEVICES := mediatek,mt7981-fpga-snfi-snand
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += mt7981-fpga-snfi-nand

define Device/mt7981-fpga-spim-nand
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := mt7981-fpga-spim-nand
  DEVICE_DTS := mt7981-fpga-spim-nand
  DEVICE_DTS_DIR := $(DTS_DIR)/mediatek
  SUPPORTED_DEVICES := mediatek,mt7981-fpga-spim-snand
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += mt7981-fpga-spim-nand

define Device/mt7981-fpga-emmc
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := mt7981-fpga-emmc
  DEVICE_DTS := mt7981-fpga-emmc
  SUPPORTED_DEVICES := mediatek,mt7981-fpga-emmc
  DEVICE_DTS_DIR := $(DTS_DIR)/mediatek
  DEVICE_PACKAGES := mkf2fs e2fsprogs blkid blockdev losetup kmod-fs-ext4 \
		     kmod-mmc kmod-fs-f2fs kmod-fs-vfat kmod-nls-cp437 \
		     kmod-nls-iso8859-1
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += mt7981-fpga-emmc

define Device/mt7981-fpga-sd
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := mt7981-fpga-sd
  DEVICE_DTS := mt7981-fpga-sd
  SUPPORTED_DEVICES := mediatek,mt7981-fpga-sd
  DEVICE_DTS_DIR := $(DTS_DIR)/mediatek
  DEVICE_PACKAGES := mkf2fs e2fsprogs blkid blockdev losetup kmod-fs-ext4 \
		     kmod-mmc kmod-fs-f2fs kmod-fs-vfat kmod-nls-cp437 \
		     kmod-nls-iso8859-1
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += mt7981-fpga-sd
