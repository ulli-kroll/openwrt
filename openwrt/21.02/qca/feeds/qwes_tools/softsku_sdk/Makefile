include $(TOPDIR)/rules.mk

PKG_NAME:=softsku_sdk
PKG_VERSION:=g76f5dd0
PKG_RELEASE:=1
PKG_DIST_VERSION=1.0

LOCAL_SRC:=$(TOPDIR)/qca/src/qwes_tools/softsku_sdk

include $(INCLUDE_DIR)/local-development.mk
ifeq ($(DUMP)$(PKG_VERSION),)
PKG_VERSION:=g76f5dd0
PKG_VERSION:=g76f5dd0
endif

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

PKG_USE_NINJA=0

define Package/softsku_sdk
 SECTION:=softsku_sdk
 CATEGORY:=softsku_sdk
 DEPENDS:=@TARGET_ipq_ipq807x||TARGET_ipq_ipq807x_64||TARGET_ipq_ipq60xx||TARGET_ipq_ipq60xx_64||TARGET_ipq_ipq50xx||TARGET_ipq_ipq50xx_64||TARGET_ipq807x||TARGET_ipq50xx||TARGET_ipq95xx||TARGET_ipq60xx||TARGET_ipq53xx +libstdcpp +libcurl +curl
 TITLE:=softsku_sdk agreement before continuing with the installation.
endef

define Package/softsku_sdk/Description
 SCM driver libarary
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
        -Dstrlcpy=g_strlcpy \
        -Dstrlcat=g_strlcat

TARGET_LDFLAGS += -pie

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/lib $(1)/etc/afc
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ipkg-install/usr/lib/* $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/softsku_sdk.h $(STAGING_DIR)/usr/include
	$(CP) $(PKG_BUILD_DIR)/libsoftsku_sdk.so $(STAGING_DIR)/usr/lib
endef

$(eval $(call BuildPackage,softsku_sdk))
