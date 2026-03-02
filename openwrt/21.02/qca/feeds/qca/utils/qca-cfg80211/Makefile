include $(TOPDIR)/rules.mk

PKG:=qca-cfg80211
PKG_NAME:=$(PKG)
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=ssh://qca-git01.qualcomm.com:29418/wifi/$(PKG_NAME).git
PKG_BRANCH:=master
PKG_RELEASE:=1

LOCAL_SRC:=$(TOPDIR)/qca/src/common-tools
DRIVER_PATH:=$(TOPDIR)/qca/src/qca-wifi
include $(INCLUDE_DIR)/local-development.mk
ifeq ($(DUMP)$(PKG_VERSION),)
  PKG_REV:=$(shell git ls-remote $(PKG_SOURCE_URL) $(PKG_BRANCH) | cut -b -7)
  PKG_VERSION:=g$(PKG_REV)
endif


PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=QCA
  CATEGORY:=QTI software
  URL:=http://www.qca.qualcomm.com
  MAINTAINER:=Qualcomm Atheros
  TITLE:= QCA cfg80211 utils
  DEPENDS:=+libnl +libpthread +iw
endef

TARGET_CFLAGS += \
		     -I$(STAGING_DIR)/usr/include/ \
		     -I$(BUILD_DIR_TOOLCHAIN)/linux-dev/include/ \
		     -I$(STAGING_DIR)/usr/include/libnl \
		     -I$(STAGING_DIR)/usr/include/libnl3 \
                     -D_GNU_SOURCE=1 -D__USE_GNU=1

TARGET_LDFLAGS += \
	             -lnl-3 -lnl-genl-3

TARGET_CFLAGS += -fpie
TARGET_CFLAGS += -fPIC
TARGET_CFLAGS += -DKERNEL_NL80211_HEADER=1
TARGET_CFLAGS += -DQCANL_LIB_SUPPORT_SINGLE_WIPHY_MLO=1
TARGET_LDFLAGS += -pie

define Package/InstallDev
	$(CP) $(PKG_BUILD_DIR)/cfg80211_nlwrapper_api.h $(STAGING_DIR)/usr/include/cfg80211_nlwrapper_api.h
	$(CP) $(PKG_BUILD_DIR)/cfg80211_nlwrapper_pvt.h $(STAGING_DIR)/usr/include/cfg80211_nlwrapper_pvt.h
	$(CP) $(PKG_BUILD_DIR)/libqca_nl80211_wrapper.so $(STAGING_DIR)/usr/lib/
	$(CP) $(DRIVER_PATH)/component_dev/tools/linux/cfg80211_ven_cmd.h $(STAGING_DIR)/usr/include
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/qca_nl80211_lib \
	CC="$(TARGET_CC)" \
	CFLAGS="$(TARGET_CFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS)"
	$(INSTALL_DIR) $(STAGING_DIR)/usr/include/qca-cfg80211
	$(CP) $(PKG_BUILD_DIR)/qca_nl80211_lib/cfg80211_nlwrapper_api.h $(STAGING_DIR)/usr/include/cfg80211_nlwrapper_api.h
	$(CP) $(PKG_BUILD_DIR)/qca_nl80211_lib/cfg80211_nlwrapper_pvt.h $(STAGING_DIR)/usr/include/cfg80211_nlwrapper_pvt.h
	$(CP) $(PKG_BUILD_DIR)/qca_nl80211_lib/qca-vendor.h $(STAGING_DIR)/usr/include/qca-cfg80211/qca-vendor.h
	$(CP) $(PKG_BUILD_DIR)/qca_nl80211_lib/libqca_nl80211_wrapper.so $(STAGING_DIR)/usr/lib/
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/local/lib
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/qca_nl80211_lib/libqca_nl80211_wrapper.so $(1)/usr/local/lib/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
