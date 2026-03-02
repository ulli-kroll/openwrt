include $(TOPDIR)/rules.mk

PKG_NAME:=native_lib
PKG_VERSION:=g76f5dd0
PKG_RELEASE:=1
PKG_DIST_VERSION=1.0

ifeq ($(CONFIG_TARGET_sdx75),y)
LOCAL_SRC:=$(TOPDIR)/src/ipq/qwes_tools/native_lib
-include $(TOPDIR)/owrt-qti-ipq-prop/include/local-development.mk
else
PKG_USE_NINJA=0
LOCAL_SRC:=$(TOPDIR)/qca/src/qwes_tools/native_lib
-include $(INCLUDE_DIR)/local-development.mk
endif

ifeq ($(DUMP)$(PKG_VERSION),)
PKG_VERSION:=g76f5dd0
PKG_VERSION:=g76f5dd0
endif

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/native_lib
 SECTION:=native_lib
 CATEGORY:=native_lib
 DEPENDS:=@TARGET_ipq_ipq807x||TARGET_ipq_ipq807x_64||TARGET_ipq_ipq60xx||TARGET_ipq_ipq60xx_64||TARGET_ipq_ipq50xx||TARGET_ipq_ipq50xx_64||TARGET_ipq807x||TARGET_ipq50xx||TARGET_ipq95xx||TARGET_ipq60xx||TARGET_ipq53xx||TARGET_sdx75 +libstdcpp +libwamr +qca-sal-rule
 TITLE:=native_lib agreement before continuing with the installation.
endef

define Package/native_lib/Description
 Location Client Application
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
	-I$(STAGING_DIR)/usr/include/libsalrule \
	-I$(STAGING_DIR)/usr/include/libnl3

TOOL_LDFLAGS:= $(TARGET_LDFLAGS) -lsalrule
define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CC="$(TARGET_CC)"\
		LDFLAGS="$(TOOL_LDFLAGS)" \
		LD_LIBRARY_PATH="$(TOOL_LDFLAGS)"
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/lib $(1)/etc/wasm_bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ipkg-install/usr/lib/* $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/native_lib.h $(STAGING_DIR)/usr/include
	$(CP) $(PKG_BUILD_DIR)/libnative_lib.so $(STAGING_DIR)/usr/lib
endef

$(eval $(call BuildPackage,native_lib))