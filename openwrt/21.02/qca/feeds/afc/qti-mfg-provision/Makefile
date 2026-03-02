include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/package.mk

PKG_NAME:=qti-mfg-provision
PKG_VERSION:=1.0
PKG_RELEASE:=1

ifeq ($(CONFIG_TARGET_sdx75),y)
LOCAL_SRC:=$(TOPDIR)/src/ipq/afc/$(PKG_NAME)
TARGET_CFLAGS += -DCONFIG_NO_DEVMEM
else
LOCAL_SRC:=$(TOPDIR)/qca/src/afc/$(PKG_NAME)
endif

define Package/$(PKG_NAME)
	SECTION:=qti-mfg-provision
	CATEGORY:=qti-mfg-provision
	TITLE:=QTI AFC Library
	DEPENDS:=@TARGET_ipq_ipq807x||TARGET_ipq_ipq807x_64||TARGET_ipq_ipq60xx||TARGET_ipq_ipq60xx_64||TARGET_ipq_ipq50xx||TARGET_ipq_ipq50xx_64||TARGET_ipq807x||TARGET_ipq50xx||TARGET_ipq95xx||TARGET_ipq60xx||TARGET_ipq53xx||TARGET_sdx75 +libjson-c +qti-encrypt
endef

define Package/$(PKG_NAME)/description
	QTI MFG PROVISIONING
endef

define Package/$(PKG_NAME)/config
        source "$(SOURCE)/Config.in"
endef

TARGET_CFLAGS += \
	-DDEBUG \
	-DBTD_DEBUG \
	-I$(STAGING_DIR)/usr/include \
	-Wall \
	-Werror \
	-DCONFIG_FTM_AFC

TARGET_LDFLAGS += -ljson-c

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	cp $(LOCAL_SRC)/* $(PKG_BUILD_DIR)
	$(Build/Patch)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR)/usr/include/$(PKG_NAME)
	-$(CP) $(PKG_BUILD_DIR)/provision.h $(STAGING_DIR)/usr/include/$(PKG_NAME)
	$(CP) $(PKG_BUILD_DIR)/libprovision.so $(STAGING_DIR)/usr/lib
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/lib $(1)/etc/pairing $(1)/etc/afc
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/libprovision.so $(1)/usr/lib
	$(CP) ./files/pairing_config.json $(1)/etc/pairing
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
