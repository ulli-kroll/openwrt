include $(TOPDIR)/rules.mk

PKG_NAME:=package_manager_agent
PKG_VERSION:=g76f5dd0
PKG_RELEASE:=1
PKG_DIST_VERSION=1.0

LOCAL_SRC:=$(TOPDIR)/qca/src/qwes_tools/package_manager_agent
include $(INCLUDE_DIR)/local-development.mk
ifeq ($(DUMP)$(PKG_VERSION),)
PKG_VERSION:=g76f5dd0
PKG_VERSION:=g76f5dd0
endif

PKG_USE_NINJA=0

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/$(PKG_NAME)
 SECTION:=$(PKG_NAME)
 CATEGORY:=$(PKG_NAME)
 DEPENDS:=@TARGET_ipq_ipq807x||TARGET_ipq_ipq807x_64||TARGET_ipq_ipq60xx||TARGET_ipq_ipq60xx_64||TARGET_ipq_ipq50xx||TARGET_ipq_ipq50xx_64||TARGET_ipq807x||TARGET_ipq50xx||TARGET_ipq95xx||TARGET_ipq60xx||TARGET_ipq53xx +libstdcpp +libcurl +libjson-c
 TITLE:=Userspace application for device package_manager_agent
endef

define Package/$(PKG_NAME)/Description
 Userspace application for device package_manager_agent
endef

TARGET_CFLAGS += -g -O0 -Werror -Wall \
         -fpic \
        -D__packed__= \
        -DIMAGE_APPS_PROC \
        -DFEATURE_Q_NO_SELF_QPTR \
        -DAEEINTERFACE_CPLUSPLUS \
        -DFEATURE_DIAG_MP_MASTER_APPS \
        -DFEATURE_MULTIPROCESSOR \
        -DFEATURE_IPQ_OPENWRT \
        -DFEATURE_NO_REX \
        -I$(STAGING_DIR)/usr/include

TARGET_LDFLAGS += \
        -L$(STAGING_DIR)/usr/lib \

TARGET_LDFLAGS += -pie -lcurl -ljson-c

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin $(1)/etc/package-manager-agent $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ipkg-install/usr/bin/* $(1)/usr/bin
	$(INSTALL_BIN) ./files/package_manager.init $(1)/etc/init.d/package_manager
	$(INSTALL_BIN) $(TOPDIR)/qca/src/qwes_tools/package_manager_agent/conf/package-manager.conf $(1)/etc/package-manager-agent/package-manager.conf
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
