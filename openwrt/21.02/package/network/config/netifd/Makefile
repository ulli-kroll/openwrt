include $(TOPDIR)/rules.mk

PKG_NAME:=netifd
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=$(PROJECT_GIT)/project/netifd.git
ifeq ($(CONFIG_TARGET_ipq53xx_ipq53xx_32),y)
PKG_SOURCE_DATE:=2021-01-09
PKG_SOURCE_VERSION:=753c351bc729967a691d99e27693be5aec334028
PKG_MIRROR_HASH:=e7d95bde520fc660d0a49e28c5bb50fff3071d7f48fe7fc3fc610f38edfc7df1
PATCH_DIR:=$(TOPDIR)/qca/src/netifd/patches
else
PKG_SOURCE_DATE:=2021-10-30
PKG_SOURCE_VERSION:=8f82742ca4f47f459284f3a07323d04da72ea5f6
PKG_MIRROR_HASH:=5e519bb1aec9bb30782213f32f19f12e874c909e42826618dd4332ded816d2fe
endif
PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/netifd
  SECTION:=base
  CATEGORY:=Base system
  DEPENDS:=+libuci +libnl-tiny +libubus +ubus +ubusd +jshn +libubox
  TITLE:=OpenWrt Network Interface Configuration Daemon
endef

define Package/netifd/conffiles
/etc/udhcpc.user
/etc/udhcpc.user.d/
endef

TARGET_CFLAGS += \
	-I$(STAGING_DIR)/usr/include/libnl-tiny \
	-I$(STAGING_DIR)/usr/include \
	-flto

TARGET_LDFLAGS += -flto -fuse-linker-plugin

CMAKE_OPTIONS += \
	-DLIBNL_LIBS=-lnl-tiny \
	-DDEBUG=1

ifeq ($(CONFIG_TARGET_ipq53xx_ipq53xx_32),y)
define Package/netifd/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/netifd $(1)/sbin/
	$(CP) ./files/* $(1)/
	$(CP) $(PKG_BUILD_DIR)/scripts/* $(1)/lib/netifd/
endef
else
define Package/netifd/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/netifd $(1)/sbin/
	$(CP) ./files/* $(1)/
	$(INSTALL_DIR) $(1)/etc/udhcpc.user.d/
	$(CP) $(PKG_BUILD_DIR)/scripts/* $(1)/lib/netifd/
endef
endif

$(eval $(call BuildPackage,netifd))
