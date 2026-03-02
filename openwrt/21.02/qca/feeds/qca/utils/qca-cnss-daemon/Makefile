
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=qca-cnss-daemon
PKG_SOURCE_PROTO:=git
PKG_BRANCH:=master
PKG_RELEASE:=1

include $(INCLUDE_DIR)/local-development.mk
ifeq ($(DUMP)$(PKG_VERSION),)
  PKG_VERSION:=$(shell git ls-remote $(PKG_SOURCE_URL) $(PKG_BRANCH) | cut -b -7)
endif

include $(INCLUDE_DIR)/package.mk

ifeq ($(CONFIG_LINUX_5_4),y)
MAKE_FLAGS += TARGET_USES_NO_FW_QMI_CLIENT:=true
endif

define Package/qca-cnss-daemon
  SECTION:=QCA
  CATEGORY:=QTI software
  TITLE:=QCA Linux QMI sotware
  DEPENDS:=@LINUX_5_4 @TARGET_ipq_ipq807x||TARGET_ipq_ipq807x_64||TARGET_ipq_ipq60xx||TARGET_ipq_ipq60xx_64||TARGET_ipq_ipq50xx||TARGET_ipq_ipq50xx_64||TARGET_ipq807x||TARGET_ipq50xx||TARGET_ipq95xx||TARGET_ipq53xx||TARGET_ipq60xx +libnl +libpthread +qca-qmi-framework +CNSSD_SUPPORTS_BREAKPAD:breakpad-wrapper
endef

define Package/qca-cnss-daemon/config
	config CNSSD_SUPPORTS_BREAKPAD
		bool "Enables breakpad support for cnss-daemon"
		default n
		help
			This option is to enable support for breakpad in cnss-daemon
endef

define Package/qca-cnss-daemon/Description
  This package contains a Linux qmi framework for the user space
endef

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include -fcommon

ifeq ($(CONFIG_CNSSD_SUPPORTS_BREAKPAD),y)
TARGET_CFLAGS += -DINCLUDE_BREAKPAD  -g3
TARGET_LDFLAGS += -lbreakpad_qcawrapper
endif

define Build/InstallDev
	$(INSTALL_DIR) $(1)
	mkdir -p $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/*.h $(1)/usr/include/
endef

define Package/qca-cnss-daemon/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lib/cnssdaemon $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lib/cnsscli $(1)/usr/bin
endef


$(eval $(call BuildPackage,qca-cnss-daemon))
