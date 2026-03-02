include $(TOPDIR)/rules.mk

PKG_NAME:=softsku_qwin_agent
PKG_VERSION:=g76f5dd0
PKG_RELEASE:=1
PKG_DIST_VERSION=1.0

LOCAL_SRC:=$(TOPDIR)/qca/src/qwes_tools/softsku_qwin_agent
include $(INCLUDE_DIR)/local-development.mk
ifeq ($(DUMP)$(PKG_VERSION),)
PKG_VERSION:=g76f5dd0
PKG_VERSION:=g76f5dd0
endif

PKG_USE_NINJA=0

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/softsku_qwin_agent
 SECTION:=softsku_qwin_agent
 CATEGORY:=softsku_qwin_agent
 DEPENDS:=@TARGET_ipq_ipq807x||TARGET_ipq_ipq807x_64||TARGET_ipq_ipq60xx||TARGET_ipq_ipq60xx_64||TARGET_ipq_ipq50xx||TARGET_ipq_ipq50xx_64||TARGET_ipq807x||TARGET_ipq50xx||TARGET_ipq95xx||TARGET_ipq60xx||TARGET_ipq53xx +libstdcpp +libcurl +libjson-c +softsku_sdk +qti-softsku-license-loader-libs
 TITLE:=Userspace application for device softsku_qwin_agent
endef

define Package/softsku_qwin_agent/Description
 Userspace application for device softsku_qwin_agent
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
        -L$(STAGING_DIR)/usr/lib/qti-softsku-license-loader-libs

TARGET_LDFLAGS += -pie -lcurl -ljson-c -lsoftsku_sdk

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/init.d/
	$(INSTALL_DIR) $(1)/usr/bin $(1)/etc/softsku-qwin-agent
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ipkg-install/usr/bin/* $(1)/usr/bin
	$(INSTALL_BIN) $(TOPDIR)/qca/src/qwes_tools/softsku_qwin_agent/files/softsku_get_feature_list.sh $(1)/usr/bin
	$(INSTALL_BIN) $(TOPDIR)/qca/src/qwes_tools/softsku_qwin_agent/files/softsku_feature_list.conf $(1)/etc/softsku-qwin-agent/softsku_feature_list.conf
	$(INSTALL_BIN) $(TOPDIR)/qca/src/qwes_tools/softsku_qwin_agent/files/softsku_qwin_agent.conf $(1)/etc/softsku-qwin-agent/softsku_qwin_agent.conf
	$(INSTALL_BIN) $(TOPDIR)/qca/src/qwes_tools/softsku_qwin_agent/files/softsku_qwin_agent.init $(1)/etc/init.d/softsku_qwin_agent
endef

$(eval $(call BuildPackage,softsku_qwin_agent))
