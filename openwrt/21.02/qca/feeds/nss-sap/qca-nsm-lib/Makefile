include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=qca-nsm-lib
PKG_VERSION:=1
PKG_RELEASE:=1
PKG_DIST_VERSION=1.0

LOCAL_SRC:=$(TOPDIR)/qca/src/qca-nss-sap/libnsm/

include $(INCLUDE_DIR)/local-development.mk
include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/$(PKG_NAME)
  SECTION:=Libs
  CATEGORY:=Libraries
  DEPENDS:=@TARGET_ipq95xx||TARGET_ipq53xx +libnl +kmod-qca-nss-nsm
  TITLE:=Network status module library
endef

define Package/$(PKG_NAME)/Description
  Network status module userspace library
endef

TARGET_CFLAGS += -Werror -Wall \
  -fpic \
  -I$(STAGING_DIR)/usr/include/qca-nss-nsm

TARGET_LDFLAGS += \
  -L$(STAGING_DIR)/usr/lib

TARGET_LDFLAGS += -pie

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR)/usr/include/libnsm
	$(CP) $(PKG_BUILD_DIR)/libnsm.so $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/exports/sal_nsm.h $(STAGING_DIR)/usr/include/libnsm
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/lib
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/libnsm.so $(1)/lib
endef

$(eval $(call BuildPackage,qca-nsm-lib))
