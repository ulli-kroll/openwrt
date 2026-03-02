include $(TOPDIR)/rules.mk

PKG_NAME:=qca-sal-rule-test
PKG_VERSION:=1
PKG_RELEASE:=1
PKG_DIST_VERSION=1.0

LOCAL_SRC:=$(TOPDIR)/qca/src/qca-nss-sap/sal-rule/sal-rule-test/
PKG_BUILD_DEPENDS:=qca-sal-rule
include $(INCLUDE_DIR)/local-development.mk
ifeq ($(DUMP)$(PKG_VERSION),)
  PKG_VERSION:=$(shell git ls-remote $(PKG_SOURCE_URL) $(PKG_BRANCH) | cut -b -7)
  PKG_VERSION:=g$(PKG_REV)
endif

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/qca-sal-rule-test
 SECTION:=utils
 CATEGORY:=Utilities
 DEPENDS:=@TARGET_ipq95xx||TARGET_ipq53xx +libjson-c +libnl +qca-sal-rule
 TITLE:=Userspace SAL rule test application
endef

define Package/qca-sal-rule-test/Description
 Userspace test application for configuring network rules
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
	$(INSTALL_DIR) $(1)/usr/bin $(1)/etc/sal_rule
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ipkg-install/usr/bin/* $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/conf/spm_config_rules.json $(1)/etc/sal_rule/spm_config_rules.json
endef

$(eval $(call BuildPackage,qca-sal-rule-test))

