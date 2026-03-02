include $(TOPDIR)/rules.mk

PKG:=cfr_tools
PKG_NAME:=$(PKG)

LOCAL_SRC:=$(TOPDIR)/qca/src/common-tools/cfr_tools
include $(INCLUDE_DIR)/local-development.mk


PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=QTI
  CATEGORY:=QTI software
  URL:=http://www.qualcomm.com
  MAINTAINER:=Qualcomm Atheros
  TITLE:= QTI cfr test utils
endef

TARGET_CFLAGS += -MMD -O2 -Wall -g -I$(STAGING_DIR)/usr/include/

ifneq ($(CONFIG_PACKAGE_kmod-mac80211),)
TARGET_CFLAGS+=-DWIN_AP_HOST_OPEN=1
endif

define Build/Compile
	$(TARGET_CC) -o $(PKG_BUILD_DIR)/cfr_test_app $(TARGET_CFLAGS) $(TARGET_LDFLAGS) $(PKG_BUILD_DIR)/cfr_test_app.c
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/cfr_test_app $(1)/usr/sbin
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
