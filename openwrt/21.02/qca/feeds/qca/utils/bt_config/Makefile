include $(TOPDIR)/rules.mk

PKG:=bt_config
PKG_NAME:=$(PKG)
PKG_VERSION:=1.0
PKG_RELEASE:=1

LOCAL_SRC:=$(TOPDIR)/qca/src/common-tools/bt_config
include $(INCLUDE_DIR)/local-development.mk
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=QTI
  CATEGORY:=QTI software
  URL:=http://www.qualcomm.com
  MAINTAINER:=Qualcomm Technologies, Inc
  TITLE:=Application for downloading patch and nvm in york
  DEPENDS:=@TARGET_ipq53xx
endef

define Package/$(PKG_NAME)/description
  Application for downloading patch and nvm in york
endef

$(info "Make" $(MAKE))
$(info "TARGET_CC - " $(TARGET_CC))
$(info "TARGET_LD - " $(TARGET_LD))
$(info "TARGET_CFLAGS - " $(TARGET_CFLAGS))
$(info "TARGET_LDFLAGS - " $(TARGET_LDFLAGS))

define Build/Compile
$(MAKE) -C $(PKG_BUILD_DIR) \
	CC="$(TARGET_CC)" \
	LD="$(TARGET_LD)" \
	CFLAGS="$(TARGET_CFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/btconfig $(1)/usr/sbin

	$(INSTALL_DIR) $(1)/bluetooth
	$(INSTALL_BIN) ./files/msbtfw11.tlv $(1)/bluetooth/msbtfw11.tlv

	$(INSTALL_DIR) $(1)/bluetooth
	$(INSTALL_BIN) ./files/msnv11.bin $(1)/bluetooth/msnv11.bin

	$(INSTALL_DIR) $(1)/bluetooth
	$(INSTALL_BIN) ./files/btconfig.sh $(1)/bluetooth/btconfig.sh
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
