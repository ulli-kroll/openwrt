
include $(TOPDIR)/rules.mk

PKG_NAME:=qapp-store
PKG_VERSION:=1.0
PKG_RELEASE:=1

include $(INCLUDE_DIR)/local-development.mk
ifeq ($(DUMP)$(PKG_VERSION),)
 PKG_REV:=$(shell git ls-remote $(PKG_SOURCE_URL) $(PKG_BRANCH) | cut -b -7)
 PKG_VERSION:=g$(PKG_REV)
endif

include $(INCLUDE_DIR)/package.mk

define Package/qapp-store
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=qapp-store Utilities
  DEPENDS=+libopenssl
endef

define Package/qapp-store/description
  qapp-store Utilities
endef

TARGET_LDFLAGS += \
	-lssl -lcrypto \
	-L$(STAGING_DIR)/usr/lib

define Package/qapp-store/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/app $(1)/usr/bin
endef

$(eval $(call BuildPackage,qapp-store))
