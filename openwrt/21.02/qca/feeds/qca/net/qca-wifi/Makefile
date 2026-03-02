include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG:=qca-wifi
PKG_NAME:=$(PKG)
PKG_SOURCE_PROTO:=git
PKG_BRANCH:=master
PKG_RELEASE:=1

DRIVER_PATH:=$(TOPDIR)/qca/src/qca-wifi
# legacy and hawkeye header files.
FW_INCLUDE_PATH:=$(TOPDIR)/dl/include
FW_COMMON_INCLUDE_PATH:=$(TOPDIR)/dl/wlan/fwcommon/include
FW_INTERFACE_TOOLS_PATH:=$(TOPDIR)/dl/wlan/fwcommon/fw_interface/tools/
FW_HDR_INCLUDE_PATH:=$(TOPDIR)/qca/src/qca-wifi/cmn_dev/fw_hdr
HALPHY_TOOLS_PATH:=$(TOPDIR)/dl/wlan/halphy_tools
PKG_BUILD_ID:=1

include $(INCLUDE_DIR)/local-development.mk
ifeq ($(DUMP)$(PKG_VERSION),)
  PKG_REV:=$(shell git ls-remote $(PKG_SOURCE_URL) $(PKG_BRANCH) | cut -b -7)
  PKG_VERSION:=g$(PKG_REV)
endif
CUSTOM_DIRECTORY?=1
ifeq ($(CUSTOM_DIRECTORY),1)
  PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(BUILD_VARIANT)/$(PKG_NAME)
else
  PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)-$(BUILD_VARIANT)/$(PKG_NAME)-$(PKG_VERSION)
endif
PKG_ASLR_PIE:=0

COPY_TO_BUILD_DIR:=$(shell cp -fpR -u --verbose $(LOCAL_SRC)/* $(PKG_BUILD_DIR)/)
include $(INCLUDE_DIR)/package.mk

PRODUCT := $(subst DEVICE_,,$(PROFILE))

FW_INSTALL_PATH:=/lib/firmware/

TARGET_TYPE_FIRST:=$(CONFIG_QCAWLAN_TARGET_TYPE_FIRST)
FW_INSTALL_PATH_FIRST:=$(FW_INSTALL_PATH)/$(TARGET_TYPE_FIRST)

TARGET_TYPE_SECOND:=$(CONFIG_QCAWLAN_TARGET_TYPE_SECOND)
FW_INSTALL_PATH_SECOND:=$(FW_INSTALL_PATH)/$(TARGET_TYPE_SECOND)

ifneq ($(CONFIG_QCAWLAN_TARGET_TYPE_THIRD),"")
TARGET_TYPE_THIRD:=$(CONFIG_QCAWLAN_TARGET_TYPE_THIRD)
FW_INSTALL_PATH_THIRD:=$(FW_INSTALL_PATH)/$(TARGET_TYPE_THIRD)
endif

ifneq ($(CONFIG_QCAWLAN_TARGET_FW_PATH_FIRST),)
	# /lib/firmware/IPQ5332
	FW_PATH_FIRST:=$(FW_INSTALL_PATH)/$(CONFIG_QCAWLAN_TARGET_FW_PATH_FIRST)
else
	FW_PATH_FIRST:=$(FW_INSTALL_PATH_FIRST)
endif

ifneq ($(CONFIG_QCAWLAN_TARGET_FW_PATH_SECOND),)
	FW_PATH_SECOND:=$(FW_INSTALL_PATH)/$(CONFIG_QCAWLAN_TARGET_FW_PATH_SECOND)
else
	FW_PATH_SECOND:=$(FW_INSTALL_PATH_SECOND)
endif

ifneq ($(CONFIG_QCAWLAN_TARGET_FW_PATH_THIRD),"")
	FW_PATH_THIRD:=$(FW_INSTALL_PATH)/$(CONFIG_QCAWLAN_TARGET_FW_PATH_THIRD)
else
ifneq ($(CONFIG_QCAWLAN_TARGET_TYPE_THIRD),"")
FW_PATH_THIRD:=$(FW_INSTALL_PATH_THIRD)
endif
endif

FW_10_2_4:=qca-wifi-fw-10.2.4
FW_10_4:=qca-wifi-fw-10.4
FW_HK:=qca-hk
FW_Cyp:=qca-cyp

FW:=$(FW_10_2_4) $(FW_10_4) $(FW_HK) $(FW_Cyp)


ifeq ($(CONFIG_TARGET_ipq_ipq807x_QSDK_Deluxe),y)
export COPY_6290_HEADER:=1
endif

ifeq ($(CONFIG_TARGET_ipq807x),y)
export ENABLE_QCA8074_HEADER:=1
endif

ifeq ($(CONFIG_TARGET_ipq_ipq60xx)$(CONFIG_TARGET_ipq_ipq60xx_64)$(CONFIG_TARGET_ipq60xx),y)
export ENABLE_6018_HEADER:=1
endif

ifeq ($(CONFIG_TARGET_ipq_ipq50xx)$(CONFIG_TARGET_ipq_ipq50xx_64)$(CONFIG_TARGET_ipq50xx),y)
export ENABLE_QCA5018_HEADER:=1
endif

ifeq ($(CONFIG_TARGET_ipq95xx),y)
export ENABLE_QCA9574_HEADER:=1
endif

ifeq ($(CONFIG_TARGET_ipq53xx),y)
export ENABLE_QCA5332_HEADER:=1
endif

ifeq ($(CONFIG_WHUNT),y)
export CONFIG_WHUNT=1
export QCA_SINGLE_WIFI_3_0=1
ifeq ($(CONFIG_LINUX_6_6)$(CONFIG_LINUX_5_4),y)
export AHB_FW_SIM=0
export PCIE_FW_SIM=1
else
export AHB_FW_SIM=1
export PCIE_FW_SIM=0
endif
endif

ifeq ($(CONFIG_GCC_USE_VERSION_7),y)
export QCA_GCC_7_SUPPORT=1
endif

ifeq ($(CONFIG_GCC_USE_VERSION_12),y)
export QCA_GCC_12_SUPPORT=1
endif

#Set QCAWLAN_MAKEOPTS according to selected BUILD_VARIANT and platform
ifneq ($(KERNEL_BUILD_DIR),)
export DRIVER_PATH
include $(DRIVER_PATH)/config.mk
endif

#########################################################
############ WLAN DRIVER BUILD CONFIGURATION ############
#########################################################

ifeq ($(CONFIG_KERNEL_KASAN),y)
	export QDF_TIMER_MULTIPLIER_FRAC=1
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-wifi-wkk-profile),)
	export ENABLE_FIPS_WKK=1
	export ENABLE_WKK_PROFILE=1
endif

ifeq ($(CONFIG_LINUX_5_4),y)
export QCA_WIFI_KERNEL_VERSION_5_4=1
endif

ifeq ($(CONFIG_LINUX_6_1),y)
export QCA_WIFI_KERNEL_VERSION_6_1=1
endif

ifeq ($(CONFIG_LINUX_6_6),y)
export QCA_WIFI_KERNEL_VERSION_6_6=1
endif

ifeq ($(ARCH),mips)
BIG_ENDIAN_HOST=1
endif
ifeq ($(ARCH),arm)
BIG_ENDIAN_HOST=0
endif

ifneq ($(CONFIG_TARGET_ipq)$(CONFIG_TARGET_ipq806x),)
	QCA_PLATFORM="ipq806x"
else
	QCA_PLATFORM="$(ARCH_PACKAGES)"
endif

QCAWLAN_MODULE_NAME:= $(notdir $(basename $(strip $(QCAWLAN_MODULE_LIST))))

#########################################################
##################### CONFIG RULES ######################
#########################################################

define KernelPackage/$(PKG_NAME)/Default
  SECTION:=QCA
  CATEGORY:=QTI software
  URL:=http://www.qca.qualcomm.com
  MAINTAINER:=Qualcomm Atheros
  TITLE:=QCA Wireless LAN Driver
  DEPENDS:=+libgcc +libc +wireless-tools +librt +kmod-cfg80211-internal +libroxml +libnl +qca-cfg80211 \
	  @(!(PACKAGE_kmod-ath&&PACKAGE_kmod-mac80211)) +TARGET_ipq_ipq807x:kmod-msm-mproc +TARGET_ipq_ipq807x_64:kmod-msm-mproc \
	  +TARGET_ipq_ipq60xx:kmod-msm-mproc +TARGET_ipq_ipq60xx_64:kmod-msm-mproc +TARGET_ipq_ipq50xx:kmod-msm-mproc +TARGET_ipq_ipq50xx_64:kmod-msm-mproc +WHUNT:kmod-qca-whunt-noship\
	  +!(LINUX_5_4||WHUNT||LINUX_6_6||LINUX_6_1&&TARGET_ipq95xx):qca-qrtr \
	  +CNSS2_MODULE:kmod-qca-cnss \
	  +(LINUX_5_4&&TARGET_ipq95xx):qca-sal-rule +(LINUX_5_4&&TARGET_ipq53xx):qca-sal-rule \
	  +kmod-bonding +PACKAGE_kmod-hota-driver:kmod-hota-driver
  DEPENDS+=@PACKAGE_qca-qmc:qca-qmc
  PKG_BUILD_DEPENDS+=$(FW) iproute2 +(LINUX_5_4&&TARGET_ipq95xx):qca-sal-rule||+(LINUX_5_4&&TARGET_ipq53xx):qca-sal-rule
  FILES:= $(strip $(QCAWLAN_MODULE_LIST))
  MENU:=1
  PROVIDES:=kmod-qca-wifi
endef

-include $(STAGING_DIR)/usr/include/qca-qmc/qmc_setup.mk

$(eval $(call RequireCommand,uuencode, \
	Please install sharutils which provides uuencode required for build \
))

define KernelPackage/$(PKG_NAME)/Description/Default
  This package contains a driver for QCA WLAN chipsets
  It also installs the QCA WLAN tools, such as:
  - wlanconfig
  - athstats
  - pktlogconf
  - ...
endef

define KernelPackage/$(PKG_NAME)/config/Default

if (TARGET_ipq_ipq807x||TARGET_ipq807x_ipq807x_32)
   config WIFI_TARGET_WIFI_2_0
	bool "Enable WIFI_2_0 support for IPQ807x platform"
	default y
	help
	   This option enables support for WiFi 2.0 in boards
   config WIFI_TARGET_WIFI_3_0
	bool "Enable WIFI_3_0 support for IPQ807x platform"
	default y
	help
	   This option enables support for WiFi 3.0 in boards
   config WIFI_EMULATION_WIFI_3_0
	bool "Enable EMULATION_WIFI_3_0 support for Emulation platform"
	default n
	help
	   This option enables support for Emulation/VP WiFi 3.0 on RUMI
endif
if (TARGET_ipq_ipq807x_64||TARGET_ipq807x_generic)
   config WIFI_TARGET_WIFI_2_0
	bool "Enable WIFI_2_0 support for IPQ807x 64bit platform"
	default y
	help
	   This option enables support for WiFi 2.0 in boards
   config WIFI_TARGET_WIFI_3_0
	bool "Enable WIFI_3_0 support for IPQ807x 64bit platform"
	default y
	help
	   This option enables support for WiFi 3.0 in boards
   config WIFI_EMULATION_WIFI_3_0
	bool "Enable EMULATION_WIFI_3_0 support for Emulation platform"
	default n
	help
	   This option enables support for Emulation/VP WiFi 3.0 on RUMI
endif
if (TARGET_ipq_ipq60xx||TARGET_ipq60xx_ipq60xx_32)
   config WIFI_TARGET_WIFI_2_0
	bool "Enable WIFI_2_0 support for IPQ60xx platform"
	default y
	help
	   This option enables support for WiFi 2.0 in boards
   config WIFI_TARGET_WIFI_3_0
	bool "Enable WIFI_3_0 support for IPQ60xx platform"
	default y
	help
	   This option enables support for WiFi 3.0 in boards
   config WIFI_EMULATION_WIFI_3_0
	bool "Enable EMULATION_WIFI_3_0 support for Emulation platform"
	default n
	help
	   This option enables support for Emulation/VP WiFi 3.0 on RUMI
endif
if (TARGET_ipq_ipq60xx_64||TARGET_ipq60xx_generic)
   config WIFI_TARGET_WIFI_2_0
	bool "Enable WIFI_2_0 support for IPQ60xx 64bit platform"
	default y
	help
	   This option enables support for WiFi 2.0 in boards
   config WIFI_TARGET_WIFI_3_0
	bool "Enable WIFI_3_0 support for IPQ60xx 64bit platform"
	default y
	help
	   This option enables support for WiFi 3.0 in boards
   config WIFI_EMULATION_WIFI_3_0
	bool "Enable EMULATION_WIFI_3_0 support for Emulation platform"
	default n
	help
	   This option enables support for Emulation/VP WiFi 3.0 on RUMI
endif
if (TARGET_ipq_ipq50xx||TARGET_ipq50xx_ipq50xx_32)
   config WIFI_TARGET_WIFI_2_0
	bool "Enable WIFI_2_0 support for IPQ50xx platform"
	default y
	help
	   This option enables support for WiFi 2.0 in boards
   config WIFI_TARGET_WIFI_3_0
	bool "Enable WIFI_3_0 support for IPQ50xx platform"
	default y
	help
	   This option enables support for WiFi 3.0 in boards
   config WIFI_EMULATION_WIFI_3_0
	bool "Enable EMULATION_WIFI_3_0 support for Emulation platform"
	default n
	help
	   This option enables support for Emulation/VP WiFi 3.0 on RUMI
endif
if (TARGET_ipq_ipq50xx_64||TARGET_ipq50xx_generic)
   config WIFI_TARGET_WIFI_2_0
	bool "Enable WIFI_2_0 support for IPQ50xx 64bit platform"
	default y
	help
	   This option enables support for WiFi 2.0 in boards
   config WIFI_TARGET_WIFI_3_0
	bool "Enable WIFI_3_0 support for IPQ50xx 64bit platform"
	default y
	help
	   This option enables support for WiFi 3.0 in boards
   config WIFI_EMULATION_WIFI_3_0
	bool "Enable EMULATION_WIFI_3_0 support for Emulation platform"
	default n
	help
	   This option enables support for Emulation/VP WiFi 3.0 on RUMI
endif

if (TARGET_ipq95xx_ipq95xx_32)
   config WIFI_TARGET_WIFI_2_0
	bool "Enable WIFI_2_0 support for IPQ95xx platform"
	default n
	help
	   This option enables support for WiFi 2.0 in boards
   config WIFI_TARGET_WIFI_3_0
	bool "Enable WIFI_3_0 support for IPQ95xx platform"
	default y
	help
	   This option enables support for WiFi 3.0 in boards
   config WIFI_EMULATION_WIFI_3_0
	bool "Enable EMULATION_WIFI_3_0 support for Emulation platform"
	default n
	help
	   This option enables support for Emulation/VP WiFi 3.0 on RUMI
endif
if (TARGET_ipq95xx_generic)
   config WIFI_TARGET_WIFI_2_0
	bool "Enable WIFI_2_0 support for IPQ95xx 64bit platform"
	default n
	help
	   This option enables support for WiFi 2.0 in boards
   config WIFI_TARGET_WIFI_3_0
	bool "Enable WIFI_3_0 support for IPQ95xx 64bit platform"
	default y
	help
	   This option enables support for WiFi 3.0 in boards
   config WIFI_EMULATION_WIFI_3_0
	bool "Enable EMULATION_WIFI_3_0 support for Emulation platform"
	default n
	help
	   This option enables support for Emulation/VP WiFi 3.0 on RUMI
endif

if (TARGET_ipq53xx_ipq53xx_32)
   config WIFI_TARGET_WIFI_2_0
	bool "Enable WIFI_2_0 support for IPQ53xx platform"
	default n
	help
	   This option enables support for WiFi 2.0 in boards
   config WIFI_TARGET_WIFI_3_0
	bool "Enable WIFI_3_0 support for IPQ53xx platform"
	default y
	help
	   This option enables support for WiFi 3.0 in boards
   config WIFI_EMULATION_WIFI_3_0
	bool "Enable EMULATION_WIFI_3_0 support for Emulation platform"
	default n
	help
	   This option enables support for Emulation/VP WiFi 3.0 on RUMI
endif
if (TARGET_ipq53xx_generic)
   config WIFI_TARGET_WIFI_2_0
	bool "Enable WIFI_2_0 support for IPQ53xx 64bit platform"
	default n
	help
	   This option enables support for WiFi 2.0 in boards
   config WIFI_TARGET_WIFI_3_0
	bool "Enable WIFI_3_0 support for IPQ53xx 64bit platform"
	default y
	help
	   This option enables support for WiFi 3.0 in boards
   config WIFI_EMULATION_WIFI_3_0
	bool "Enable EMULATION_WIFI_3_0 support for Emulation platform"
	default n
	help
	   This option enables support for Emulation/VP WiFi 3.0 on RUMI
endif

config QCAWLAN_TARGET_TYPE_FIRST
	string "First type of the fwtarget"
	default ""
config QCAWLAN_TARGET_TYPE_SECOND
	string "Second type of the fwtarget"
	default ""
config QCAWLAN_TARGET_TYPE_THIRD
	string "Third type of the fwtarget"
	default ""
config QCAWLAN_TARGET_FW_PATH_FIRST
	string "First type of the fwtarget path"
	default ""
config QCAWLAN_TARGET_FW_PATH_SECOND
	string "Second type of the fwtarget"
	default ""	
config QCAWLAN_TARGET_FW_PATH_THIRD
	string "Third type of the fwtarget"
	default ""

if TARGET_x86_64
   config WIFI_TARGET_WIFI_2_0
	bool "Enable WIFI_2_0 support for x86 platform"
	default y
	help
	   This option enables support for WiFi 2.0 in boards
   config WIFI_TARGET_WIFI_3_0
	bool "Enable WIFI_3_0 support for x86 platform"
	default y
	help
	   This option enables support for WiFi 3.0 in boards
endif
config WIFI_MEM_DEBUG
    bool "Enable WiFi memory debug support"
    default n
    help
       This option enables memory debug support in WiFi modules
config WLAN_REF_ID_TRACE
    bool "Enable wlan objmgr ref trace"
    default n
    help
       This option objmgr ref debug support in WiFi modules
config WIFI_CE_TASKLET_DEBUG
    bool "Enable Wifi CE tasklet debug support"
    default n
    help
       This option enables ce tasklet debug support in Wifi modules
config WIFI_LOG_UTILS
    bool "Enable logging through user space application"
    default n
    help
       This enables sending host driver logs to user space application (cnss_diag)
config WIFI_VDEV_PEER_PROT_COUNT
    bool "Enable per-peer protocol counts"
    default n
    help
       This tracks selected protocol packets per peer and maintains count entering and exiting driver on both tx and rx side
config WIFI_VDEV_PEER_PROT_TESTING
    bool "Enable per-peer protocol counts"
    default n
    help
       This tracks selected protocol packets per peer and maintains count entering and exiting driver on both tx and rx side
config WLAN_IOT_SIM_SUPPORT
    bool "Enable iot simulation feature"
    default n
    help
       This enables iot simulation feature
config WIFI_MONITOR_SUPPORT
    bool "Enable Monitor wifi support"
    default y
    help
       This option enables monitor support in wifi modules
endef

#########################################################
################### INSTALLDEV RULES ####################
#########################################################
define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR)/usr/include
	$(INSTALL_DIR) $(STAGING_DIR)/usr/include/qca-wifi
ifeq ($(CONFIG_WHUNT),1)
	$(INSTALL_DIR) $(STAGING_DIR)/usr/include/linux
	$(CP) -f $(LINUX_DIR)/include/uapi/linux/nl80211.h $(STAGING_DIR)/usr/include/linux
endif
	$(MAKE) -C $(PKG_BUILD_DIR) INSTALL_DEST="$(STAGING_DIR)/usr/include/" install_headers
	$(CP) -f $(DRIVER_PATH)/component_dev/qca_mscs/inc/qca_mscs_if.h $(STAGING_DIR)/usr/include/qca-wifi
	$(CP) -f $(DRIVER_PATH)/component_dev/qca_mesh_latency/inc/qca_mesh_latency_if.h $(STAGING_DIR)/usr/include/qca-wifi
	$(CP) -f $(DRIVER_PATH)/component_dev/qca_sawf/qca_sawf_if.h $(STAGING_DIR)/usr/include/qca-wifi/qca_sawf_if.h
	$(CP) -f $(DRIVER_PATH)/component_dev/qca_scs/inc/qca_scs_if.h $(STAGING_DIR)/usr/include/qca-wifi
	$(CP) -f $(DRIVER_PATH)/component_dev/qca_fse/qca_fse_if.h $(STAGING_DIR)/usr/include/qca-wifi
	$(CP) -f $(DRIVER_PATH)/component_dev/qca_wifi_metadata_info/qca_wifi_metadata_info_if.h $(STAGING_DIR)/usr/include/qca-wifi
	$(CP) -f $(DRIVER_PATH)/component_dev/qca_wifi_metadata_info/qca_wifi_mlo_metadata_info/qca_wifi_mlo_metadata_info_if.h $(STAGING_DIR)/usr/include/qca-wifi
endef

#########################################################
################# BUILD/INSTALL RULES ###################
#########################################################
Hooks/Prepare/Pre += qca-wifi_update_patches_version
define qca-wifi_update_patches_version

	[ ! -h patches ] || rm patches
	[ ! -d patches-$(PKG_BRANCH) ] || \
		ln -s patches-$(PKG_BRANCH) patches
endef

PKG_EXTMOD_SUBDIRS=os/linux

define Build/Compile
		$(CP) -f $(DRIVER_PATH)/component_dev/tools/linux/cfg80211_ven_cmd.h $(STAGING_DIR)/usr/include
ifeq ($(CONFIG_CNSS2_MODULE),y)
		$(CP) -f $(STAGING_DIR)/usr/include/qca-cnss/cnss2.h $(STAGING_DIR)/usr/include
endif
		$(if $(CONFIG_PACKAGE_qca-qmc), $(call qmc_setup,compile,$(PKG_BUILD_DIR)))
		export CONFIG_CNSS2_MODULE="$(CONFIG_CNSS2_MODULE)"; \
		export CONFIG_WIFI_TARGET_WIFI_2_0="$(CONFIG_WIFI_TARGET_WIFI_2_0)"; \
		export CONFIG_WIFI_TARGET_WIFI_3_0="$(CONFIG_WIFI_TARGET_WIFI_3_0)"; \
		export FW_INCLUDE_PATH="$(FW_INCLUDE_PATH)"; \
		export FW_COMMON_INCLUDE_PATH="$(FW_COMMON_INCLUDE_PATH)"; \
		export FW_INTERFACE_TOOLS_PATH="$(FW_INTERFACE_TOOLS_PATH)"; \
		export FW_HDR_INCLUDE_PATH="$(FW_HDR_INCLUDE_PATH)"; \
		export HALPHY_TOOLS_PATH="$(HALPHY_TOOLS_PATH)"; \
		export MAKE="$(MAKE)"; \
		export KERNEL_MAKE="$(KERNEL_MAKE)"; \
		export LINUX_VERSION="$(LINUX_VERSION)"; \
		export LINUX_DIR="$(LINUX_DIR)"; \
		export LINUX_SRC_DIR="$(LINUX_SRC_DIR)"; \
		export ARCH="$(ARCH)"; \
		export BUILD_VARIANT="$(BUILD_VARIANT)"; \
		export TARGET_CROSS="$(TARGET_CROSS)"; \
		export QCA_PLATFORM="$(QCA_PLATFORM)"; \
		export BIG_ENDIAN_HOST="$(BIG_ENDIAN_HOST)"; \
		export TARGET_CFLAGS="$(TARGET_CFLAGS) -g"; \
		export TARGET_LDFLAGS="$(TARGET_LDFLAGS) -lpthread -ldl"; \
		export CONFIG_WIFI_MEM_DEBUG="$(CONFIG_WIFI_MEM_DEBUG)"; \
		export CONFIG_WLAN_REF_ID_TRACE="$(CONFIG_WLAN_REF_ID_TRACE)"; \
		export CONFIG_WIFI_CE_TASKLET_DEBUG="$(CONFIG_WIFI_CE_TASKLET_DEBUG)"; \
		export CONFIG_WIFI_LOG_UTILS="$(CONFIG_WIFI_LOG_UTILS)"; \
		export CONFIG_WIFI_VDEV_PEER_PROT_COUNT="$(CONFIG_WIFI_VDEV_PEER_PROT_COUNT)"; \
		export CONFIG_WIFI_VDEV_PEER_PROT_TESTING="$(CONFIG_WIFI_VDEV_PEER_PROT_TESTING)"; \
		export CONFIG_WIFI_EMULATION_WIFI_3_0="$(CONFIG_WIFI_EMULATION_WIFI_3_0)"; \
		export CONFIG_WIFI_IPQ_MEM_PROFILE="$(CONFIG_KERNEL_IPQ_MEM_PROFILE)"; \
		export CONFIG_TARGET_ipq_ipq807x_64_QSDK_Enterprise="$(CONFIG_TARGET_ipq_ipq807x_64_QSDK_Enterprise)"; \
		export CONFIG_TARGET_ipq_ipq807x_QSDK_Enterprise="$(CONFIG_TARGET_ipq_ipq807x_QSDK_Enterprise)"; \
		export CONFIG_TARGET_ipq_ipq806x="$(CONFIG_TARGET_ipq_ipq806x)"; \
		export CONFIG_TARGET_ipq_ipq40xx="$(CONFIG_TARGET_ipq_ipq40xx)"; \
		export CONFIG_TARGET_ipq807x="$(CONFIG_TARGET_ipq807x)"; \
		export CONFIG_TARGET_ipq95xx="$(CONFIG_TARGET_ipq95xx)"; \
		export CONFIG_TARGET_ipq53xx="$(CONFIG_TARGET_ipq53xx)"; \
		export CONFIG_TARGET_ipq60xx="$(CONFIG_TARGET_ipq60xx)"; \
		export CONFIG_64BIT="$(CONFIG_64BIT)"; \
		export CONFIG_KERNEL_KASAN="$(CONFIG_KERNEL_KASAN)"; \
		export CONFIG_KERNEL_SKB_DEBUG="$(CONFIG_KERNEL_SKB_DEBUG)"; \
		export CONFIG_KERNEL_SLUB_DEBUG="$(CONFIG_KERNEL_SLUB_DEBUG)"; \
		export CONFIG_WLAN_IOT_SIM_SUPPORT="$(CONFIG_WLAN_IOT_SIM_SUPPORT)"; \
		export CONFIG_LOWMEM_FLASH="$(CONFIG_LOWMEM_FLASH)"; \
		export CONFIG_DEBUG="$(CONFIG_DEBUG)"; \
		export CONFIG_WIFI_MONITOR_SUPPORT="$(CONFIG_WIFI_MONITOR_SUPPORT)"; \
		$(KERNEL_MAKE) -C $(PKG_BUILD_DIR) ARCH="$(ARCH)" src_prepare all_build
		$(CP) $(PKG_BUILD_DIR)/os/linux/tools/libqca_tools.so $(STAGING_DIR)/usr/lib/
ifeq ($(BUILD_VARIANT),$(filter $(BUILD_VARIANT), unified-profile unified-profile-nonss))
		[ ! -f $(PKG_BUILD_DIR)/os/linux/tools/libstats_lib.so ] || \
			$(CP) $(PKG_BUILD_DIR)/os/linux/tools/libstats_lib.so $(STAGING_DIR)/usr/lib/
endif
ifneq ($(CONFIG_LOWMEM_FLASH),y)
		$(CP) $(PKG_BUILD_DIR)/os/linux/tools/libwlanfw_htt.so $(STAGING_DIR)/usr/lib/
		$(CP) $(PKG_BUILD_DIR)/os/linux/tools/libapstats.so $(STAGING_DIR)/usr/lib/
		$(CP) $(PKG_BUILD_DIR)/os/linux/tools/libwifistats.so $(STAGING_DIR)/usr/lib/
		$(CP) $(PKG_BUILD_DIR)/os/linux/tools/libwlanfw_wmi.so $(STAGING_DIR)/usr/lib/
else
ifeq ($(CONFIG_DEBUG),y)
		$(CP) $(PKG_BUILD_DIR)/os/linux/tools/libwlanfw_htt.so $(STAGING_DIR)/usr/lib/
		$(CP) $(PKG_BUILD_DIR)/os/linux/tools/libapstats.so $(STAGING_DIR)/usr/lib/
		$(CP) $(PKG_BUILD_DIR)/os/linux/tools/libwifistats.so $(STAGING_DIR)/usr/lib/
		$(CP) $(PKG_BUILD_DIR)/os/linux/tools/libwlanfw_wmi.so $(STAGING_DIR)/usr/lib/
endif
endif
endef

define KernelPackage/$(PKG_NAME)/install/Default
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/usr/lib
	$(INSTALL_DIR) $(1)/lib/wifi
	$(INSTALL_DIR) $(1)/lib/wifi/debug
	$(INSTALL_DIR) $(1)/lib/wifi/mount
	$(INSTALL_DIR) $(1)/lib/sbin
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_DIR) $(1)/ini
	$(INSTALL_DIR) $(1)/sawf
	$(INSTALL_DIR) $(1)/sawf/telemetry
	$(INSTALL_DIR) $(1)/ini/internal
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/data/vendor/wifi
	$(INSTALL_DIR) $(1)/etc/hotplug.d/ieee80211
	$(INSTALL_DIR) $(1)/etc/hotplug.d/firmware
	$(INSTALL_DIR) $(1)/etc/hotplug.d/net

	#$(INSTALL_DATA) ./files/qcawifi.sh $(1)/lib/wifi
	$(INSTALL_BIN) ./files/mount_fw_partition.sh $(1)/lib/wifi/mount/mount_fw_partition.sh
	$(INSTALL_BIN) ./files/umount_fw_partition.sh $(1)/lib/wifi/mount/umount_fw_partition.sh
	$(INSTALL_BIN) ./files/boot-ftm $(1)/etc/init.d/
	$(INSTALL_BIN) ./files/diag_socket_app $(1)/etc/init.d/
	$(INSTALL_BIN) ./files/is_nocal $(1)/sbin/

	$(INSTALL_BIN) ./files/counters_wlan_prompt_hawkeye.sh $(1)/lib/wifi/debug

	echo $(QCAWLAN_MODULE_NAME) | sed 's, ,\n,g' > $(1)/lib/wifi/qca-wifi-modules

	$(INSTALL_BIN) ./files/performance_configs.sh $(1)/lib
	$(INSTALL_BIN) ./files/perf_debug_counters.sh $(1)/lib
	$(INSTALL_DATA) ./files/wifi-utils.sh $(1)/lib/wifi
	$(INSTALL_DATA) ./files/qcawificfg80211.sh $(1)/lib/wifi
	$(INSTALL_BIN) ./files/launch_vap.sh $(1)/lib/wifi
	$(INSTALL_DATA) ./files/wifi-fw.hotplug $(1)/etc/hotplug.d/firmware/11-wifi-fw
	$(INSTALL_DATA) ./files/qcawifi.hotplug $(1)/etc/hotplug.d/net/11-qcawifi
	$(INSTALL_CONF) ./files/qcacfg80211.config $(1)/etc/config/qcacfg80211
	$(INSTALL_CONF) ./files/wireless.config $(1)/etc/config/wireless
	$(INSTALL_DATA) ./files/qcawifi_countrycode.txt $(1)/lib/wifi
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/cfg80211_configs/cfg80211tool $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/cfg80211_configs/*.xml $(1)/lib/wifi
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/umac/son/cfg80211_configs/*.xml $(1)/lib/wifi
	ln -s /usr/sbin/cfg80211tool $(1)/usr/sbin/cfg80211tool_mesh

	$(INSTALL_DIR) $(1)/lib/upgrade/keep.d
	$(INSTALL_DATA) ./files/wifi_nss.upgrade $(1)/lib/upgrade/keep.d/wifi_nss

	$(INSTALL_BIN) ./files/wifi_try $(1)/usr/sbin/
	$(INSTALL_BIN) ./files/wifi_try $(1)/lib/sbin/

	$(INSTALL_CONF) ./files/ssid-steering.config $(1)/etc/config/ssid-steering
	$(INSTALL_BIN) ./files/ssid_steering.init $(1)/etc/init.d/ssid_steering

	$(INSTALL_BIN) ./files/wifi_hw_mode $(1)/usr/sbin/

	$(INSTALL_CONF) ./files/sawf/*.json $(1)/sawf/
	$(INSTALL_CONF) ./files/sawf/telemetry/*.json $(1)/sawf/telemetry/
	$(if $(CONFIG_PACKAGE_qca-qmc), $(call qmc_setup,install,$(1)))
	$(INSTALL_DATA) ./files/tools_config $(1)/lib/wifi
ifeq ($(QCA_GCC_12_SUPPORT), 1)
	$(INSTALL_BIN) ./files/qcawifi-legacy $(1)/etc/init.d/
endif
ifeq ($(CONFIG_LINUX_5_4),y)
	$(INSTALL_BIN) ./files/qcawifi-config-cmd $(1)/etc/init.d/
	$(INSTALL_BIN) ./files/load_cnss2 $(1)/etc/init.d/
endif

ifeq ($(CONFIG_LINUX_6_1),y)
	$(INSTALL_BIN) ./files/qcawifi-config-cmd $(1)/etc/init.d/
	$(INSTALL_BIN) ./files/load_cnss2 $(1)/etc/init.d/
endif

ifeq ($(CONFIG_LINUX_6_6),y)
	$(INSTALL_BIN) ./files/qcawifi-config-cmd $(1)/etc/init.d/
	$(INSTALL_BIN) ./files/load_cnss2 $(1)/etc/init.d/
endif

# First Target Type
# /lib/firmware/IPQ5332/WIFI_FW
	$(INSTALL_DIR) $(1)/$(FW_INSTALL_PATH_FIRST)/WIFI_FW
	$(CP) ./files/target/$(FW_PATH_FIRST)/* $(1)/$(FW_INSTALL_PATH_FIRST)/WIFI_FW;

# Second Target Type
# /lib/firmware/IPQ5332/WIFI_FW/qcn9224
	$(INSTALL_DIR) $(1)/$(FW_INSTALL_PATH_FIRST)/WIFI_FW/$(TARGET_TYPE_SECOND)
	$(CP) ./files/target/$(FW_PATH_SECOND)/* $(1)/$(FW_INSTALL_PATH_FIRST)/WIFI_FW/$(TARGET_TYPE_SECOND);


# Third Target Type
ifneq ($(CONFIG_QCAWLAN_TARGET_TYPE_THIRD),"")
	$(INSTALL_DIR) $(1)/$(FW_INSTALL_PATH_THIRD)
	$(CP) ./files/target/$(FW_PATH_THIRD)/* $(1)/$(FW_INSTALL_PATH_THIRD); \

endif

	[ ! -d files-$(PKG_BRANCH) ] || \
		$(CP) ./files-$(PKG_BRANCH)/* $(1)/
	export INSTALL_BIN="$(INSTALL_BIN)"; \
	export INSTALL_DATA="$(INSTALL_DATA)"; \
	export CONFIG_WIFI_TARGET_WIFI_2_0="$(CONFIG_WIFI_TARGET_WIFI_2_0)"; \
	export CONFIG_WIFI_TARGET_WIFI_3_0="$(CONFIG_WIFI_TARGET_WIFI_3_0)"; \
	$(MAKE) -C $(PKG_BUILD_DIR) QCAWLAN_TOOL_LIST="$(QCAWLAN_TOOL_LIST)" INSTALL_BIN_DEST="$(1)/usr/sbin" INSTALL_LIB_DEST="$(1)/usr/lib" tools_installonly
endef
define KernelPackage/$(PKG_NAME)/install/Lowmem
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/usr/lib
	$(INSTALL_DIR) $(1)/lib/wifi
	$(INSTALL_DIR) $(1)/lib/wifi/debug
	$(INSTALL_DIR) $(1)/lib/sbin
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_DIR) $(1)/ini
	$(INSTALL_DIR) $(1)/ini/internal
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/data/vendor/wifi
	$(INSTALL_DIR) $(1)/etc/hotplug.d/firmware
	$(INSTALL_DIR) $(1)/etc/hotplug.d/net

	echo $(QCAWLAN_MODULE_NAME) | sed 's, ,\n,g' > ./files/qca-wifi-modules
	$(INSTALL_DATA) ./files/qca-wifi-modules $(1)/lib/wifi

	$(INSTALL_DATA) ./files/qcawificfg80211.sh $(1)/lib/wifi
	$(INSTALL_DATA) ./files/wifi-utils.sh $(1)/lib/wifi
	$(INSTALL_BIN) ./files/launch_vap.sh $(1)/lib/wifi
	$(INSTALL_DATA) ./files/wifi-fw.hotplug $(1)/etc/hotplug.d/firmware/11-wifi-fw
	$(INSTALL_DATA) ./files/qcawifi.hotplug $(1)/etc/hotplug.d/net/11-qcawifi
	$(INSTALL_CONF) ./files/qcacfg80211.config $(1)/etc/config/qcacfg80211
	$(INSTALL_CONF) ./files/wireless.config $(1)/etc/config/wireless
	$(INSTALL_DATA) ./files/qcawifi_countrycode.txt $(1)/lib/wifi
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/cfg80211_configs/cfg80211tool $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/cfg80211_configs/*.xml $(1)/lib/wifi
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/umac/son/cfg80211_configs/*.xml $(1)/lib/wifi
	ln -s /usr/sbin/cfg80211tool $(1)/usr/sbin/cfg80211tool_mesh
ifeq ($(CONFIG_PACKAGE_qca-diag),y)
	$(INSTALL_BIN) ./files/diag_socket_app $(1)/etc/init.d/
endif
ifeq ($(CONFIG_LINUX_5_4),y)
	$(INSTALL_BIN) ./files/qcawifi-config-cmd $(1)/etc/init.d/
	$(INSTALL_BIN) ./files/load_cnss2 $(1)/etc/init.d/
endif

ifeq ($(CONFIG_LINUX_6_1),y)
	$(INSTALL_BIN) ./files/qcawifi-config-cmd $(1)/etc/init.d/
	$(INSTALL_BIN) ./files/load_cnss2 $(1)/etc/init.d/
endif

ifeq ($(CONFIG_LINUX_6_6),y)
	$(INSTALL_BIN) ./files/qcawifi-config-cmd $(1)/etc/init.d/
	$(INSTALL_BIN) ./files/load_cnss2 $(1)/etc/init.d/
endif


	$(if $(CONFIG_PACKAGE_qca-qmc), $(call qmc_setup,install,$(1)))

	$(INSTALL_DATA) ./files/tools_config $(1)/lib/wifi
	
# First Target Type
# /lib/firmware/IPQ5332/WIFI_FW
	$(INSTALL_DIR) $(1)/$(FW_INSTALL_PATH_FIRST)/WIFI_FW
	$(CP) ./files/target/$(FW_PATH_FIRST)/* $(1)/$(FW_INSTALL_PATH_FIRST)/WIFI_FW;


# Second Target Type
# /lib/firmware/IPQ5332/WIFI_FW/qcn9224
	$(INSTALL_DIR) $(1)/$(FW_INSTALL_PATH_FIRST)/WIFI_FW/$(TARGET_TYPE_SECOND)
	$(CP) ./files/target/$(FW_PATH_SECOND)/* $(1)/$(FW_INSTALL_PATH_FIRST)/WIFI_FW/$(TARGET_TYPE_SECOND); 

# Third Target Type
ifneq ($(CONFIG_QCAWLAN_TARGET_TYPE_THIRD),)
	$(INSTALL_DIR) $(1)/$(FW_INSTALL_PATH_THIRD)
	$(CP) ./files/target/$(FW_PATH_THIRD)/* $(1)/$(FW_INSTALL_PATH_THIRD); \

endif
	
	[ ! -d files-$(PKG_BRANCH) ] || \
		$(CP) ./files-$(PKG_BRANCH)/* $(1)/
	export INSTALL_BIN="$(INSTALL_BIN)"; \
	export INSTALL_DATA="$(INSTALL_DATA)"; \
	export CONFIG_WIFI_TARGET_WIFI_2_0="$(CONFIG_WIFI_TARGET_WIFI_2_0)"; \
	export CONFIG_WIFI_TARGET_WIFI_3_0="$(CONFIG_WIFI_TARGET_WIFI_3_0)"; \
	$(MAKE) -C $(PKG_BUILD_DIR) QCAWLAN_TOOL_LIST="$(QCAWLAN_TOOL_LIST)" INSTALL_BIN_DEST="$(1)/usr/sbin" INSTALL_LIB_DEST="$(1)/usr/lib" tools_installonly
endef

#########################################################
################# VARIANTS DEFINITION ###################
#########################################################
ifeq ($(CONFIG_TARGET_ipq)$(CONFIG_TARGET_ipq807x)$(CONFIG_TARGET_ipq50xx)$(CONFIG_TARGET_ipq95xx)$(CONFIG_TARGET_ipq60xx)$(CONFIG_TARGET_ipq53xx),y)
subtarget:=$(SUBTARGET)
else
subtarget:=$(CONFIG_TARGET_BOARD)
endif

ifeq ($(BUILD_VARIANT),$(filter $(BUILD_VARIANT), unified-profile-nonss unified-profile custc-profile lowmem-profile))
ifeq ($(CONFIG_TARGET_ipq95xx)$(CONFIG_TARGET_ipq53xx),y)
 QCA_PPE_VP_ENABLED:=1
 QCA_PPE_DS_ENABLED:=1
 export QCA_PPE_VP_ENABLED
 export QCA_PPE_DS_ENABLED
endif
endif

ifneq ($(CONFIG_TARGET_ipq95xx),y)
ifneq ($(CONFIG_TARGET_ipq53xx),y)
ifneq ($(BUILD_VARIANT),$(filter $(BUILD_VARIANT), unified-profile-nonss lowmem-profile-nonss))
ifeq ($(subtarget),$(filter $(subtarget),ipq806x ipq807x ipq807x_64 generic ipq807x_32 ipq60xx ipq60xx_64 ipq60xx_32 ipq50xx ipq50xx_64 ipq50xx_32))
 QCA_NSS_ENABLED:=1
 export QCA_NSS_ENABLED
endif
endif
endif
endif

ifeq ($(BUILD_VARIANT),$(filter $(BUILD_VARIANT), wkk-profile))
 QCA_NSS_ENABLED:=0
 export QCA_NSS_ENABLED
 QCA_PPE_VP_ENABLED:=1
 export QCA_PPE_VP_ENABLED
endif

# Default variant (Akronite only)
define KernelPackage/$(PKG_NAME)-akronite-perf
$(call KernelPackage/$(PKG_NAME)/Default)
  VARIANT:=akronite-perf
  DEPENDS+=@TARGET_ipq806x +TARGET_ipq806x:kmod-qca-nss-drv
endef

define KernelPackage/$(PKG_NAME)-akronite-perf/Description
$(call define KernelPackage/$(PKG_NAME)/Description/Default)
endef

ifneq ($(CONFIG_TARGET_ipq806x),)
define KernelPackage/$(PKG_NAME)-akronite-perf/config
$(call KernelPackage/$(PKG_NAME)/config/Default,-akronite-perf)
endef
endif

define KernelPackage/$(PKG_NAME)-akronite-perf/install
$(call KernelPackage/$(PKG_NAME)/install/Default,$(1))
endef
$(eval $(call KernelPackage,$(PKG_NAME)-akronite-perf))

# Unified variant
define KernelPackage/$(PKG_NAME)-unified-profile
$(call KernelPackage/$(PKG_NAME)/Default)
  VARIANT:=unified-profile
  DEPENDS+=@TARGET_ipq||TARGET_ipq807x||TARGET_ipq50xx||TARGET_ipq95xx||TARGET_ipq53xx||TARGET_ipq60xx||TARGET_x86 \
	   +TARGET_ipq_ipq807x:kmod-qca-nss-drv \
	   +TARGET_ipq_ipq807x_64:kmod-qca-nss-drv +TARGET_ipq_ipq60xx:kmod-qca-nss-drv \
	   +TARGET_ipq_ipq60xx_64:kmod-qca-nss-drv +TARGET_ipq_ipq50xx:kmod-qca-nss-drv \
	   +TARGET_ipq_ipq50xx_64:kmod-qca-nss-drv +TARGET_ipq807x:kmod-qca-nss-drv \
	   +TARGET_ipq95xx:kmod-qca-nss-ppe-vp \
	   +TARGET_ipq95xx:kmod-qca-nss-ppe-ds \
	   +TARGET_ipq53xx:kmod-qca-nss-ppe-vp \
	   +TARGET_ipq53xx:kmod-qca-nss-ppe-ds \
	   +TARGET_ipq50xx:kmod-qca-nss-drv \
	   +TARGET_ipq60xx:kmod-qca-nss-drv
endef

define KernelPackage/$(PKG_NAME)-unified-profile/Description
$(call define KernelPackage/$(PKG_NAME)/Description/Default)
endef

define KernelPackage/$(PKG_NAME)-unified-profile/config
$(call KernelPackage/$(PKG_NAME)/config/Default,-unified-profile)
endef

define KernelPackage/$(PKG_NAME)-unified-profile/install
$(call KernelPackage/$(PKG_NAME)/install/Default,$(1))
endef
$(eval $(call KernelPackage,$(PKG_NAME)-unified-profile))

# custc variant
define KernelPackage/$(PKG_NAME)-custc-profile
$(call KernelPackage/$(PKG_NAME)/Default)
  VARIANT:=custc-profile
  DEPENDS+=@TARGET_ipq||TARGET_x86||TARGET_ipq807x||TARGET_ipq95xx||TARGET_ipq53xx \
	   +TARGET_ipq_ipq807x:kmod-qca-nss-drv \
	   +TARGET_ipq_ipq807x_64:kmod-qca-nss-drv +TARGET_ipq_ipq60xx:kmod-qca-nss-drv \
	   +TARGET_ipq_ipq60xx_64:kmod-qca-nss-drv +TARGET_ipq_ipq50xx:kmod-qca-nss-drv \
	   +TARGET_ipq_ipq50xx_64:kmod-qca-nss-drv \
	   +TARGET_ipq95xx:kmod-qca-nss-ppe-vp \
	   +TARGET_ipq95xx:kmod-qca-nss-ppe-ds \
	   +TARGET_ipq53xx:kmod-qca-nss-ppe-vp \
	   +TARGET_ipq53xx:kmod-qca-nss-ppe-ds
endef

define KernelPackage/$(PKG_NAME)-custc-profile/Description
$(call define KernelPackage/$(PKG_NAME)/Description/Default)
endef

define KernelPackage/$(PKG_NAME)-custc-profile/config
$(call KernelPackage/$(PKG_NAME)/config/Default,-custc-profile)
endef

define KernelPackage/$(PKG_NAME)-custc-profile/install
$(call KernelPackage/$(PKG_NAME)/install/Default,$(1))
endef
$(eval $(call KernelPackage,$(PKG_NAME)-custc-profile))

# IPQ8074 Lowmem variant
define KernelPackage/$(PKG_NAME)-lowmem-profile
$(call KernelPackage/$(PKG_NAME)/Default)
  VARIANT:=lowmem-profile
  DEPENDS+=+TARGET_ipq_ipq807x:kmod-qca-nss-drv \
	   +TARGET_ipq_ipq50xx:kmod-qca-nss-drv \
	   +TARGET_ipq50xx:kmod-qca-nss-drv \
	   +TARGET_ipq807x:kmod-qca-nss-drv \
	   +TARGET_ipq53xx:kmod-qca-nss-ppe-vp \
	   +TARGET_ipq53xx:kmod-qca-nss-ppe-ds \
	   +TARGET_ipq60xx:kmod-qca-nss-drv
endef

define KernelPackage/$(PKG_NAME)-lowmem-profile/Description
$(call define KernelPackage/$(PKG_NAME)/Description/Default)
endef

define KernelPackage/$(PKG_NAME)-lowmem-profile/config
$(call KernelPackage/$(PKG_NAME)/config/Default,-lowmem-profile)
endef

define KernelPackage/$(PKG_NAME)-lowmem-profile/install
$(call KernelPackage/$(PKG_NAME)/install/Default,$(1))
endef
$(eval $(call KernelPackage,$(PKG_NAME)-lowmem-profile))

# IPQ8074 flash_16mb  svariant
define KernelPackage/$(PKG_NAME)-flash_16mb-profile
$(call KernelPackage/$(PKG_NAME)/Default)
  VARIANT:=flash_16mb-profile
  DEPENDS+=@TARGET_ipq||TARGET_ipq50xx||TARGET_ipq53xx \
	   +TARGET_ipq_ipq50xx:kmod-qca-nss-drv \
	   +TARGET_ipq50xx:kmod-qca-nss-drv
endef

define KernelPackage/$(PKG_NAME)-flash_16mb-profile/Description
$(call define KernelPackage/$(PKG_NAME)/Description/Default)
endef

define KernelPackage/$(PKG_NAME)-flash_16mb-profile/config
$(call KernelPackage/$(PKG_NAME)/config/Default,-flash_16mb-profile)
endef

define KernelPackage/$(PKG_NAME)-flash_16mb-profile/install
$(call KernelPackage/$(PKG_NAME)/install/Lowmem,$(1))
endef
$(eval $(call KernelPackage,$(PKG_NAME)-flash_16mb-profile))

# Minimal Profile variant
define KernelPackage/$(PKG_NAME)-minimal-profile
$(call KernelPackage/$(PKG_NAME)/Default)
  VARIANT:=minimal-profile
endef

define KernelPackage/$(PKG_NAME)-minimal-profile/Description
$(call define KernelPackage/$(PKG_NAME)/Description/Default)
endef

define KernelPackage/$(PKG_NAME)-minimal-profile/config
$(call KernelPackage/$(PKG_NAME)/config/Default,-minimal-profile)
endef

define KernelPackage/$(PKG_NAME)-minimal-profile/install
$(call KernelPackage/$(PKG_NAME)/install/Default,$(1))
endef
$(eval $(call KernelPackage,$(PKG_NAME)-minimal-profile))

# Debug Profile variant
define KernelPackage/$(PKG_NAME)-debug-profile
$(call KernelPackage/$(PKG_NAME)/Default)
  VARIANT:=debug-profile
endef

define KernelPackage/$(PKG_NAME)-debug-profile/Description
$(call define KernelPackage/$(PKG_NAME)/Description/Default)
endef

define KernelPackage/$(PKG_NAME)-debug-profile/config
$(call KernelPackage/$(PKG_NAME)/config/Default,-debug-profile)
endef

define KernelPackage/$(PKG_NAME)-debug-profile/install
$(call KernelPackage/$(PKG_NAME)/install/Default,$(1))
endef
$(eval $(call KernelPackage,$(PKG_NAME)-debug-profile))

# Waikiki profile
define KernelPackage/$(PKG_NAME)-wkk-profile
$(call KernelPackage/$(PKG_NAME)/Default)
  VARIANT:=wkk-profile
  DEPENDS+=@TARGET_ipq||TARGET_ipq807x||TARGET_ipq50xx||TARGET_ipq95xx||TARGET_ipq53xx||TARGET_x86 \
	   +TARGET_ipq_ipq807x:kmod-qca-nss-drv \
	   +TARGET_ipq_ipq807x_64:kmod-qca-nss-drv +TARGET_ipq_ipq60xx:kmod-qca-nss-drv \
	   +TARGET_ipq807x:kmod-qca-nss-drv \
	   +TARGET_ipq95xx:kmod-qca-nss-ppe-vp \
	   +TARGET_ipq95xx:kmod-qca-nss-ppe-ds \
	   +TARGET_ipq53xx:kmod-qca-nss-ppe-vp \
	   +TARGET_ipq53xx:kmod-qca-nss-ppe-ds \
	   +TARGET_ipq_ipq60xx_64:kmod-qca-nss-drv
endef

define KernelPackage/$(PKG_NAME)-wkk-profile/Description
$(call define KernelPackage/$(PKG_NAME)/Description/Default)
endef

define KernelPackage/$(PKG_NAME)-wkk-profile/config
$(call KernelPackage/$(PKG_NAME)/config/Default,-wkk-profile)
endef

define KernelPackage/$(PKG_NAME)-wkk-profile/install
$(call KernelPackage/$(PKG_NAME)/install/Default,$(1))
endef
$(eval $(call KernelPackage,$(PKG_NAME)-wkk-profile))

# Unified Host variant
define KernelPackage/$(PKG_NAME)-unified-profile-nonss
$(call KernelPackage/$(PKG_NAME)/Default)
  VARIANT:=unified-profile-nonss
  DEPENDS+=@TARGET_ipq||TARGET_ipq807x||TARGET_ipq50xx||TARGET_ipq95xx||TARGET_ipq53xx||TARGET_x86 \
	   +TARGET_ipq95xx:kmod-qca-nss-ppe-vp \
	   +TARGET_ipq95xx:kmod-qca-nss-ppe-ds \
	   +TARGET_ipq53xx:kmod-qca-nss-ppe-vp \
	   +TARGET_ipq53xx:kmod-qca-nss-ppe-ds
endef

define KernelPackage/$(PKG_NAME)-unified-profile-nonss/Description
$(call define KernelPackage/$(PKG_NAME)/Description/Default)
endef

define KernelPackage/$(PKG_NAME)-unified-profile-nonss/config
$(call KernelPackage/$(PKG_NAME)/config/Default,-unified-profile-nonss)
endef

define KernelPackage/$(PKG_NAME)-unified-profile-nonss/install
$(call KernelPackage/$(PKG_NAME)/install/Default,$(1))
endef
$(eval $(call KernelPackage,$(PKG_NAME)-unified-profile-nonss))

#Lowmem Host variant
define KernelPackage/$(PKG_NAME)-lowmem-profile-nonss
$(call KernelPackage/$(PKG_NAME)/Default)
  VARIANT:=lowmem-profile-nonss
endef

define KernelPackage/$(PKG_NAME)-lowmem-profile-nonss/Description
$(call define KernelPackage/$(PKG_NAME)/Description/Default)
endef

define KernelPackage/$(PKG_NAME)-lowmem-profile-nonss/config
$(call KernelPackage/$(PKG_NAME)/config/Default,-lowmem-profile-nonss)
endef

define KernelPackage/$(PKG_NAME)-lowmem-profile-nonss/install
$(call KernelPackage/$(PKG_NAME)/install/Default,$(1))
endef
$(eval $(call KernelPackage,$(PKG_NAME)-lowmem-profile-nonss))

define KernelPackage/cfg80211-internal
  SUBMENU:=$(WIRELESS_MENU)
  TITLE:=cfg80211 - wireless configuration API
  DEPENDS:=+iw
  KCONFIG:=CONFIG_CFG80211 \
        CONFIG_CFG80211_DEVELOPER_WARNINGS=n \
        CONFIG_CFG80211_REG_DEBUG=n \
        CONFIG_CFG80211_DEFAULT_PS=n \
        CONFIG_CFG80211_DEBUGFS=n \
        CONFIG_CFG80211_INTERNAL_REGDB=n \
        CONFIG_CFG80211_CRDA_SUPPORT=n \
        CONFIG_ATH_CARDS=n \
        CONFIG_MWIFIEX=n \
        CONFIG_WILC1000_DRIVER=n \
        CONFIG_CFG80211_WEXT=y \
        CONFIG_WIRELESS_EXT=y \
        CONFIG_WEXT_PRIV=y \
        CONFIG_CFG80211=m \
        CONFIG_NL80211_TESTMODE=y
  FILES:=$(LINUX_DIR)/net/wireless/cfg80211.ko
  AUTOLOAD:=$(call AutoLoad,30,cfg80211)
endef

define KernelPackage/cfg80211-internal/description
cfg80211 is the Linux wireless LAN (802.11) configuration API.
endef

$(eval $(call KernelPackage,cfg80211-internal))
