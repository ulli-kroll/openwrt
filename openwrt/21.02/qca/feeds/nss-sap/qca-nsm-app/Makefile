include $(TOPDIR)/rules.mk

PKG_NAME:=qca-nsm-app
PKG_VERSION:=1
PKG_RELEASE:=1
PKG_DIST_VERSION=1.0

LOCAL_SRC:=$(TOPDIR)/qca/src/qca-nss-sap/nsm-app/

include $(INCLUDE_DIR)/local-development.mk
include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=@TARGET_ipq95xx||TARGET_ipq53xx +qca-nsm-lib
  TITLE:=Network status module application
endef

define Package/$(PKG_NAME)/Description
  Network status module userspace application
endef

TARGET_CFLAGS += -Werror -Wall \
  -fpic \
  -I$(STAGING_DIR)/usr/include/qca-nss-nsm \
  -I$(STAGING_DIR)/usr/include/libnsm

TARGET_LDFLAGS += \
  -L$(STAGING_DIR)/usr/lib

define Build/Compile
$(MAKE) -C $(PKG_BUILD_DIR) \
	CC="$(TARGET_CC)" \
	CFLAGS="$(TARGET_CFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/nsm-app $(1)/usr/bin
endef

$(eval $(call BuildPackage,qca-nsm-app))
