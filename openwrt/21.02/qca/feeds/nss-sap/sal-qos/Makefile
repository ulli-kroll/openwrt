include $(TOPDIR)/rules.mk

PKG_NAME:=qca-sal-qos
PKG_VERSION:=1
PKG_RELEASE:=1
PKG_DIST_VERSION=1.0

LOCAL_SRC:=$(TOPDIR)/qca/src/qca-nss-sap/sal-qos

include $(INCLUDE_DIR)/local-development.mk
ifeq ($(DUMP)$(PKG_VERSION),)
  PKG_VERSION:=$(shell git ls-remote $(PKG_SOURCE_URL) $(PKG_BRANCH) | cut -b -7)
  PKG_VERSION:=g$(PKG_REV)
endif

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/qca-sal-qos
 SECTION:=utils
 CATEGORY:=Utilities
 DEPENDS:=@TARGET_ipq95xx||TARGET_ipq53xx +libstdcpp
 TITLE:=Qos configuration library
endef

define Package/qca-sal-qos/Description
 QOS configuration library that provides APIs for tc cake qdisc commands
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

TARGET_LDFLAGS += -pie

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ipkg-install/usr/lib/* $(1)/usr/lib
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR)/usr/include/libsalqos
	$(CP) $(PKG_BUILD_DIR)/libsalqos.so $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/sal_qos.h $(STAGING_DIR)/usr/include/libsalqos
endef

$(eval $(call BuildPackage,qca-sal-qos))
