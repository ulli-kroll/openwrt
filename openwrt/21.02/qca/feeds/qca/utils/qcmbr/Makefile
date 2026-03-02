#
# Copyright (c) 2014 Qualcomm Atheros, Inc..
#
# All Rights Reserved.
# Qualcomm Atheros Confidential and Proprietary.
#

include $(TOPDIR)/rules.mk

PKG:=qcmbr
PKG_NAME:=$(PKG)
PKG_SOURCE_PROTO:=git
PKG_BRANCH:=master
PKG_RELEASE:=1

LOCAL_SRC:=$(TOPDIR)/qca/src/qcmbr
include $(INCLUDE_DIR)/local-development.mk

ifeq ($(DUMP)$(PKG_VERSION),)
  PKG_VERSION:=$(shell git ls-remote $(PKG_SOURCE_URL) $(PKG_BRANCH) | cut -b -7)
endif

ifeq ($(BUILD_VARIANT),netlink)
QCMBR_APP_OPTS+=USE_NETLINK=1
endif

PKG_SOURCE:=$(PKG)-$(PKG_VERSION).tar.gz
PKG_SOURCE_SUBDIR:=$(PKG)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)/Default
  SECTION:=QCA
  CATEGORY:=QTI software
  URL:=http://www.qca.qualcomm.com
  MAINTAINER:=Qualcomm Atheros
  TITLE:=$(1) - QCA ART Driver & tools
  DEPENDS:=+libpthread +libc +librt
  VARIANT:=default
endef

define Package/$(PKG_NAME)/description/Default
  ART package support for qca-wifi
endef

ifeq ($(BUILD_VARIANT),netlink)
TARGET_CPPFLAGS += \
	-I$(STAGING_DIR)/usr/include/libnl-tiny

TARGET_LDFLAGS+=-lnl-tiny
endif

define Build/Compile
	$(QCMBR_APP_OPTS) \
	CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" \
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		LD="$(TARGET_CC)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		Qcmbr
endef

define Package/$(PKG_NAME)/install/Default
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/Qcmbr $(1)/usr/sbin
	$(INSTALL_BIN) ./files/qcmbr.init $(1)/etc/init.d/qcmbr
endef

define Package/qcmbr
$(call Package/qcmbr/Default)
endef

define Package/qcmbr/Description
$(call Package/$(PKG_NAME)/Description/Default)
endef

define Package/qcmbr/config
$(call Package/qcmbr/config/Default)
endef

define Package/qcmbr/install
$(call Package/qcmbr/install/Default,$(1))
endef

$(eval $(call BuildPackage,qcmbr))

define Package/qcmbr-netlink
  $(call Package/qcmbr/Default,netlink)
  VARIANT:=netlink
  DEPENDS+=+libnl-tiny
endef

define Package/qcmbr-netlink/Description
  $(call Package/qcmbr/Description/Default)
endef

define Package/qcmbr-netlink/config
  $(call Package/qcmbr/config/Default,netlink)
endef

define Package/qcmbr-netlink/install
  $(call Package/qcmbr/install/Default,$(1))
endef

$(eval $(call BuildPackage,qcmbr-netlink))
