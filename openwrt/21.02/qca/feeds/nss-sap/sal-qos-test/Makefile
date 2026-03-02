include $(TOPDIR)/rules.mk

PKG_NAME:=qca-sal-qos-test
PKG_VERSION:=1
PKG_RELEASE:=1
PKG_DIST_VERSION=1.0

LOCAL_SRC:=$(TOPDIR)/qca/src/qca-nss-sap/sal-qos/sal-qos-test/

include $(INCLUDE_DIR)/local-development.mk
ifeq ($(DUMP)$(PKG_VERSION),)
  PKG_VERSION:=$(shell git ls-remote $(PKG_SOURCE_URL) $(PKG_BRANCH) | cut -b -7)
  PKG_VERSION:=g$(PKG_REV)
endif

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/qca-sal-qos-test
 SECTION:=utils
 CATEGORY:=Utilities
 DEPENDS:=@TARGET_ipq95xx||TARGET_ipq53xx  +libjson-c +qca-sal-qos
 TITLE:=QOS configuration test application
endef

define Package/qca-sal-qos-test/Description
 Userspace application for configuring the QOS parameters using the APIs provided by SAL QOS library
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
        -Dstrlcat=g_strlcat \
        -I$(STAGING_DIR)/usr/include

TARGET_LDFLAGS += \
        -L$(STAGING_DIR)/usr/lib \

TARGET_LDFLAGS += -pie -ljson-c

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin $(1)/etc/sal_qos
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ipkg-install/usr/bin/* $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/conf/qos_conf.json $(1)/etc/sal_qos/qos_conf.json
endef

$(eval $(call BuildPackage,qca-sal-qos-test))
