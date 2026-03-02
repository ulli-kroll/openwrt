
include $(TOPDIR)/rules.mk

PKG:=qca-hostap
PKG_NAME:=$(PKG)
PKG_SOURCE_PROTO:=git
PKG_BRANCH:=master
PKG_RELEASE:=1

ifeq ($(CONFIG_LINUX_6_6),y)
PKG_SOURCE_URL:=git://w1.fi/srv/git/hostap.git
PATCH_DIR:=$(TOPDIR)/qca/feeds/qca/net/$(PKG_NAME)/patches
else
PKG_SOURCE_URL:=ssh://qca-git01.qualcomm.com:29418/wifi/$(PKG_NAME).git
PATCH_DIR:=$(TOPDIR)/qca/src/$(PKG_NAME)/qca-patches
include $(INCLUDE_DIR)/local-development.mk
endif

ifeq ($(DUMP)$(PKG_VERSION),)
  PKG_REV:=$(shell git ls-remote $(PKG_SOURCE_URL) $(PKG_BRANCH) | cut -b -7)
  PKG_VERSION:=g$(PKG_REV)
endif

LOCAL_TYPE=$(strip \
		$(if $(findstring qca-hapd-supp,$(BUILD_VARIANT)),hapd-supp, \
		$(if $(findstring supplicant,$(BUILD_VARIANT)),supplicant, \
		hostap \
	)))

LOCAL_VARIANT=$(BUILD_VARIANT)
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(BUILD_VARIANT)/$(PKG)-$(PKG_VERSION)

ifeq ($(CONFIG_LINUX_6_6),y)
PKG_SOURCE_SUBDIR:=$(PKG)-$(PKG_VERSION)
PKG_SOURCE_VERSION=1dfcafff36e5972dce3b28d70d741d680a80c68e
PKG_SOURCE:=$(PKG)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
PKG_MIRROR_MD5SUM:=skip
endif

PKG_BUILD_DEPENDS:=qca-wifi

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)/Default
  SECTION:=QCA
  CATEGORY:=QTI software
  URL:=http://www.qca.qualcomm.com
  MAINTAINER:=Qualcomm Atheros
endef

define Package/$(PKG_NAME)
  $(call Package/$(PKG_NAME)/Default)
  TITLE:=Hostap authenticator
  DEPENDS:=@$(WIFI_PKGS) @(!(PACKAGE_hostapd)) +libnl +libopenssl
  VARIANT:=default
  CONFLICTS:=qca-hapd-supp qca-hostapd-cli qca-wpa-cli
endef

define Package/$(PKG_NAME)-lite
  $(call Package/$(PKG_NAME)/Default)
  TITLE:=Hostap authenticator for lite profile
  DEPENDS:=@$(WIFI_PKGS) @(!(PACKAGE_hostapd)) +libnl +libopenssl
  VARIANT:=lite
  CONFLICTS:=qca-hapd-supp
endef

define Package/$(PKG_NAME)-mlo
  $(call Package/$(PKG_NAME)/Default)
  TITLE:=Hostap authenticator for mlo profile
  DEPENDS:=@$(WIFI_PKGS) @(!(PACKAGE_hostapd)) +libnl +libopenssl
  VARIANT:=mlo
  CONFLICTS:=qca-hapd-supp
endef

define Package/$(PKG_NAME)/description
 This package contains the IEEE 802.1x/WPA/EAP/RADIUS Authenticator.
endef

define Package/$(PKG_NAME)-lite/description
 This package contains the IEEE 802.1x/WPA/EAP/RADIUS Authenticator.
endef

define Package/$(PKG_NAME)-mlo/description
 This package contains the IEEE 802.1x/WPA/EAP/RADIUS Authenticator.
endef

define Package/$(PKG_NAME)-macsec
  $(call Package/$(PKG_NAME))
  TITLE:=Hostap authenticator (MACSEC)
  DEPENDS+= @TARGET_ipq806x||TARGET_ipq||TARGET_ipq807x||TARGET_ipq50xx||TARGET_ipq60xx||TARGET_ipq95xx||TARGET_ipq53xx +kmod-qca-nss-macsec
  VARIANT:=macsec
  CONFLICTS:=qca-hapd-supp qca-hapd-supp-macsec
endef

define Package/$(PKG_NAME)-macsec/Description
  Hostap authenticator with MACSEC support for IPQ807x.
endef

define Package/qca-hostapd-cli
  $(call Package/$(PKG_NAME)/Default)
  DEPENDS:=@(PACKAGE_qca-hapd-supp) +librt +libnl +libopenssl
  DEPENDS+= PACKAGE_kmod-qca-nss-macsec:kmod-qca-nss-macsec
  TITLE:=Hostap authenticator command line interface
endef

define Package/qca-hostapd-cli/description
 This package contains a command line utility to control the
 IEEE 802.1x/WPA/EAP/RADIUS Authenticator.
endef

define Package/qca-wpa-supplicant
  $(call Package/$(PKG_NAME)/Default)
  TITLE:=WPA Supplicant
  DEPENDS:=@$(WIFI_PKGS) @(!(PACKAGE_wpa-supplicant)) +libopenssl +libnl
  VARIANT:=supplicant-default
  CONFLICTS:=qca-hapd-supp qca-hostapd-cli qca-wpa-cli
endef

define Package/qca-wpa-supplicant-lite
  $(call Package/$(PKG_NAME)/Default)
  TITLE:=WPA Supplicant for lite profile
  DEPENDS:=@$(WIFI_PKGS) @(!(PACKAGE_wpa-supplicant)) +libopenssl +libnl
  VARIANT:=supplicant-lite
  CONFLICTS:=qca-hapd-supp
endef

define Package/qca-wpa-supplicant-mlo
  $(call Package/$(PKG_NAME)/Default)
  TITLE:=WPA Supplicant for mlo profile
  DEPENDS:=@$(WIFI_PKGS) @(!(PACKAGE_wpa-supplicant)) +libopenssl +libnl
  VARIANT:=supplicant-mlo
  CONFLICTS:=qca-hapd-supp
endef

define Package/qca-wpa-supplicant/Description
  WPA Supplicant
endef

define Package/qca-wpa-supplicant-lite/Description
  WPA Supplicant for lite profile
endef

define Package/qca-wpa-supplicant-mlo/Description
  WPA Supplicant for mlo profile
endef

define Package/qca-wpa-supplicant-macsec
  $(call Package/qca-wpa-supplicant)
  TITLE:=WPA Supplicant (MACSEC)
  DEPENDS+= @TARGET_ipq806x||TARGET_ipq||TARGET_ipq807x||TARGET_ipq50xx||TARGET_ipq60xx||TARGET_ipq95xx||TARGET_ipq53xx +kmod-qca-nss-macsec
  VARIANT:=supplicant-macsec
  CONFLICTS:=qca-hapd-supp qca-hapd-supp-macsec
endef

define Package/qca-wpa-supplicant-macsec/Description
  WPA Supplicant with MACSEC support for IPQ8066.
endef

define Package/qca-wpa-cli
  $(call Package/$(PKG_NAME)/Default)
  DEPENDS:=@(PACKAGE_qca-hapd-supp) +librt  +libopenssl +libnl
  TITLE:=WPA Supplicant command line interface
  VARIANT:=qca-hapd-supp-full
endef

define Package/qca-wpa-cli/Description
  WPA Supplicant control utility
endef

ifneq (,$(CONFIG_LINUX_6_6))
define remove_patch
	rm -rf $(PATCH_DIR)/541-qca-hostap-proprietery-240MHz-handling.patch
	rm -rf $(PATCH_DIR)/557-qca-hostap-Fix-segmentation-fault-due-to-puncture_bi.patch
endef
endif

Hooks/Prepare/Pre += remove_patch

define Package/qca-hapd-supp
$(call Package/qca-hapd-supp/Default)
  TITLE+= (full)
  DEPENDS+=+libopenssl +libnl
  VARIANT:=qca-hapd-supp-full
endef

define Package/qca-hapd-supp/description
 This package contains a full featured IEEE 802.1x/WPA/EAP/RADIUS
 Authenticator and Supplicant
endef

define Package/qca-hapd-supp-macsec
$(call Package/qca-hapd-supp-macsec/Default)
  TITLE+= (full)
  DEPENDS+=@TARGET_ipq806x||TARGET_ipq||TARGET_ipq807x||TARGET_ipq50xx||TARGET_ipq60xx||TARGET_ipq95xx||TARGET_ipq53xx +kmod-qca-nss-macsec +libopenssl +libnl
  VARIANT:=qca-hapd-supp-macsec-full
endef

define Package/qca-hapd-supp-macsec/description
 This package contains a full featured IEEE 802.1x/WPA/EAP/RADIUS
 Authenticator and Supplicant macsec
endef

define Package/qca-hapd-supp-lite
$(call Package/qca-hapd-supp-lite/Default)
  TITLE+= (full)
  DEPENDS+=@(!(PACKAGE_hostapd)) +libnl +libopenssl
  VARIANT:=qca-hapd-supp-lite-full
endef

define Package/qca-hapd-supp-lite/description
 This package contains a full featured IEEE 802.1x/WPA/EAP/RADIUS
 Authenticator and Supplicant lite
endef


ifneq ($(wildcard $(PKG_BUILD_DIR)/.config_*),$(subst .configured_,.config_,$(STAMP_CONFIGURED)))
  define Build/Configure/rebuild
	$(FIND) $(PKG_BUILD_DIR) -name \*.o -or -name \*.a | $(XARGS) rm -f
	rm -f $(PKG_BUILD_DIR)/hostapd/hostapd
	rm -f $(PKG_BUILD_DIR)/wpa_supplicant/wpa_supplicant
	rm -f $(PKG_BUILD_DIR)/.config_*
	touch $(subst .configured_,.config_,$(STAMP_CONFIGURED))
  endef
endif

define Build/Configure
	$(Build/Configure/rebuild)
	$(if $(wildcard ./files/hostapd-$(LOCAL_VARIANT).config), \
		$(CP) ./files/hostapd-$(LOCAL_VARIANT).config $(PKG_BUILD_DIR)/hostapd/.config \
	)
	$(CP) ./files/wpa_supplicant-$(LOCAL_VARIANT).config $(PKG_BUILD_DIR)/wpa_supplicant/.config
endef

TARGET_CPPFLAGS += \
	-I$(STAGING_DIR)/usr/include \
	-I$(STAGING_DIR)/usr/include/qca-nss-macsec

TARGET_LDFLAGS += \
	-lnl-3 -lnl-genl-3 -lm -lssl -lcrypto -lpthread -ldl -pie

TARGET_CFLAGS += -fpie -Werror -Wall -g3 -O0 -fPIC

TARGET_CFLAGS += -Wno-deprecated-declarations -Wno-implicit-function-declaration -Wno-int-conversion -Wno-discarded-qualifiers -Wno-use-after-free -Wno-address -Wno-error=format
ifeq ($(LOCAL_VARIANT),qca-hapd-supp-macsec-full)
TARGET_CFLAGS += -Wno-unused-but-set-variable
endif

ifneq ($(LOCAL_TYPE),hostap)
    TARGET_LDFLAGS += \
		-lssl -lcrypto
endif

ifeq ($(LOCAL_VARIANT),qca-hapd-supp-macsec-full)
    TARGET_LDFLAGS += -lfal -L$(STAGING_DIR)/usr/lib
endif

define Build/RunMake
	CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" \
	$(MAKE) -C $(PKG_BUILD_DIR)/$(1) \
	$(TARGET_CONFIGURE_OPTS) \
	LIBS="$(TARGET_LDFLAGS)" \
	$(2)
endef

define Build/Compile/qca-hostap
	$(call Build/RunMake,hostapd, \
		hostapd hostapd_cli \
	)
endef

define Build/Compile/qca-supplicant
	$(call Build/RunMake,wpa_supplicant, \
		wpa_supplicant wpa_cli \
	)
endef

define Build/Compile/qca-hapd-supp
	echo ` \
		$(call Build/RunMake,hostapd,-s UNIFIEDCALL=1 unified_cflags); \
		$(call Build/RunMake,wpa_supplicant,-s UNIFIEDCALL=1 unified_cflags) | \
		sed -e 's,-n ,,g' -e 's^$(TARGET_CFLAGS)^^' \
	` > $(PKG_BUILD_DIR)/.cflags
	+$(call Build/RunMake,hostapd, \
		CFLAGS="$$$$(cat $(PKG_BUILD_DIR)/.cflags)" \
		UNIFIEDCALL=1 \
		hostapd_cli hostapd_unified.a \
	)
	+$(call Build/RunMake,wpa_supplicant, \
		CFLAGS="$$$$(cat $(PKG_BUILD_DIR)/.cflags)" \
		UNIFIEDCALL=1 \
		wpa_cli wpa_supplicant_unified.a \
	)
	$(TARGET_CC) -o $(PKG_BUILD_DIR)/qca-hapd-supp \
		$(TARGET_CFLAGS) \
		./files/qca-wpad-common.c \
		$(PKG_BUILD_DIR)/hostapd/hostapd_unified.a \
		$(PKG_BUILD_DIR)/wpa_supplicant/wpa_supplicant_unified.a \
		$(TARGET_LDFLAGS)
endef

define Build/Compile
	$(Build/Compile/qca-$(LOCAL_TYPE))
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/include/utils
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/src/common/wpa_ctrl.h $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/src/common/qca-vendor.h $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/src/common/ieee802_11_defs.h $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/src/utils/includes.h $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/src/utils/build_config.h $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/src/utils/eloop.h $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/src/utils/os.h $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/src/utils/common.h $(1)/usr/include/utils
	$(CP) $(PKG_BUILD_DIR)/src/utils/wpa_debug.h $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/src/utils/wpabuf.h $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/src/drivers/linux_ioctl.h $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/src/drivers/linux_wext.h $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/src/drivers/netlink.h $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/src/drivers/priv_netlink.h $(1)/usr/include/

	$(if $(findstring hostap,$(LOCAL_TYPE)),
		$(CP) $(PKG_BUILD_DIR)/build/hostapd/src/common/wpa_ctrl.o $(1)/usr/lib/
		$(CP) $(PKG_BUILD_DIR)/build/hostapd/src/drivers/netlink.o $(1)/usr/lib/
		$(CP) $(PKG_BUILD_DIR)/build/hostapd/src/utils/eloop.o $(1)/usr/lib/
		$(CP) $(PKG_BUILD_DIR)/build/hostapd/src/utils/wpa_debug.o $(1)/usr/lib/
		$(CP) $(PKG_BUILD_DIR)/build/hostapd/src/utils/common.o $(1)/usr/lib/
		$(CP) $(PKG_BUILD_DIR)/build/hostapd/src/utils/os_unix.o $(1)/usr/lib/
		,
		$(CP) $(PKG_BUILD_DIR)/build/wpa_supplicant/src/common/wpa_ctrl.o $(1)/usr/lib/
		$(CP) $(PKG_BUILD_DIR)/build/wpa_supplicant/src/drivers/netlink.o $(1)/usr/lib/
		$(CP) $(PKG_BUILD_DIR)/build/wpa_supplicant/src/utils/eloop.o $(1)/usr/lib/
		$(CP) $(PKG_BUILD_DIR)/build/wpa_supplicant/src/utils/wpa_debug.o $(1)/usr/lib/
		$(CP) $(PKG_BUILD_DIR)/build/wpa_supplicant/src/utils/common.o $(1)/usr/lib/
		$(CP) $(PKG_BUILD_DIR)/build/wpa_supplicant/src/utils/os_unix.o $(1)/usr/lib/
	)

endef

define Install/$(PKG_NAME)
	$(INSTALL_DIR) $(1)/lib/wifi
	$(INSTALL_DATA) ./files/hostapd.sh $(1)/lib/wifi/hostapd.sh
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/qca-hostapd.init $(1)/etc/init.d/qca-hostapd
	$(INSTALL_DIR) $(1)/usr/sbin
endef

define Install/qca-wpa-supplicant
	$(INSTALL_DIR) $(1)/lib/wifi
	$(INSTALL_DATA) ./files/wpa_supplicant.sh $(1)/lib/wifi/wpa_supplicant.sh
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/qca-wpa-supplicant.init $(1)/etc/init.d/qca-wpa-supplicant
	$(INSTALL_DIR) $(1)/usr/sbin
endef

define Package/$(PKG_NAME)/install
	$(call Install/$(PKG_NAME),$(1))
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/hostapd/hostapd $(1)/usr/sbin/
endef

define Package/$(PKG_NAME)-lite/install
	$(call Install/$(PKG_NAME),$(1))
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/hostapd/hostapd $(1)/usr/sbin/
endef

define Package/$(PKG_NAME)-macsec/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/hostapd/hostapd $(1)/usr/sbin/hostapd-macsec
endef

define Package/$(PKG_NAME)-mlo/install
	$(call Install/$(PKG_NAME),$(1))
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/hostapd/hostapd $(1)/usr/sbin/
endef

define Package/qca-hapd-supp/install
	$(call Install/$(PKG_NAME),$(1))
	$(call Install/qca-wpa-supplicant,$(1))
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/qca-hapd-supp $(1)/usr/sbin/
	$(LN) qca-hapd-supp $(1)/usr/sbin/hostapd
	$(LN) qca-hapd-supp $(1)/usr/sbin/wpa_supplicant
endef

define Package/qca-hapd-supp-macsec/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/qca-hapd-supp $(1)/usr/sbin/qca-hapd-supp-macsec
	$(LN) qca-hapd-supp-macsec $(1)/usr/sbin/hostapd-macsec
	$(LN) qca-hapd-supp-macsec $(1)/usr/sbin/wpa_supplicant-macsec
	$(INSTALL_DIR) $(1)/etc/init.d $(1)/etc/config
	$(INSTALL_BIN) ./files/macsec.init $(1)/etc/init.d/macsec
	$(INSTALL_BIN) ./files/macsec.config $(1)/etc/config/macsec
endef

define Package/qca-hapd-supp-lite/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/qca-hapd-supp $(1)/usr/sbin/qca-hapd-supp-lite
	$(LN) qca-hapd-supp-lite $(1)/usr/sbin/hostapd-lite
	$(LN) qca-hapd-supp-lite $(1)/usr/sbin/wpa_supplicant-lite
endef
ifneq ($(LOCAL_TYPE),supplicant)
ifneq ($(LOCAL_VARIANT),qca-hapd-supp-macsec-full)
ifneq ($(LOCAL_VARIANT),macsec)
  define Package/qca-hostapd-cli/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/hotplug.d/button
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/hostapd/hostapd_cli $(1)/usr/sbin/
	$(INSTALL_DATA) ./files/wps-hotplug.sh $(1)/etc/hotplug.d/button/50-wps
	$(INSTALL_DATA) ./files/wps-extender-hotplug.sh \
			$(1)/etc/hotplug.d/button/54-wps-extender
	#$(INSTALL_DATA) ./files/wps-reset-hotplug.sh $(1)/etc/hotplug.d/button/51-wps-reset

	$(INSTALL_DIR) $(1)/lib/wifi
	$(INSTALL_BIN) ./files/wps-hostapd-update-uci.sh \
			$(1)/lib/wifi/wps-hostapd-update-uci

	$(INSTALL_DIR) $(1)/lib/wifi
	$(INSTALL_BIN) ./files/dpp-supplicant-update-uci.sh \
			$(1)/lib/wifi/dpp-supplicant-update-uci

	$(INSTALL_BIN) ./files/wps-hostapd-nbh-update.sh \
			$(1)/lib/wifi/wps-hostapd-nbh-update

	$(INSTALL_BIN) ./files/dpp-hostapd-update-uci.sh \
			$(1)/lib/wifi/dpp-hostapd-update-uci
	$(INSTALL_BIN) ./files/dpp-operating-class-update.sh \
			$(1)/lib/wifi/dpp-operating-class-update.sh

	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) ./files/hapd.sh \
			$(1)/usr/sbin/hapd
	$(INSTALL_BIN) ./files/wpsd.sh \
			$(1)/usr/sbin/wpsd

	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/hostapd_cli.sh $(1)/etc/init.d/hostapd_cli
  endef
endif
endif
endif

define Package/qca-wpa-supplicant/install
	$(call Install/qca-wpa-supplicant,$(1))
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wpa_supplicant/wpa_supplicant $(1)/usr/sbin/
endef

define Package/qca-wpa-supplicant-lite/install
	$(call Install/qca-wpa-supplicant,$(1))
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wpa_supplicant/wpa_supplicant $(1)/usr/sbin/
endef

define Package/qca-wpa-supplicant-mlo/install
	$(call Install/qca-wpa-supplicant,$(1))
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wpa_supplicant/wpa_supplicant $(1)/usr/sbin/
endef

define Package/qca-wpa-supplicant-macsec/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wpa_supplicant/wpa_supplicant $(1)/usr/sbin/wpa_supplicant-macsec
	$(INSTALL_DIR) $(1)/etc/init.d $(1)/etc/config
	$(INSTALL_BIN) ./files/macsec.init $(1)/etc/init.d/macsec
	$(INSTALL_BIN) ./files/macsec.config $(1)/etc/config/macsec
endef

ifneq ($(LOCAL_TYPE),hostap)
ifneq ($(LOCAL_VARIANT),qca-hapd-supp-macsec-full)
ifneq ($(LOCAL_VARIANT),macsec)
  define Package/qca-wpa-cli/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/wpa_supplicant/wpa_cli $(1)/usr/sbin/

	$(INSTALL_DIR) $(1)/etc/hotplug.d/button
	$(INSTALL_DATA) ./files/wps-supplicant-hotplug.sh \
			$(1)/etc/hotplug.d/button/52-wps-supplicant

	$(INSTALL_DIR) $(1)/lib/wifi
	$(INSTALL_BIN) ./files/wps-supplicant-update-uci.sh \
			$(1)/lib/wifi/wps-supplicant-update-uci
  endef
endif
endif
endif

$(eval $(call BuildPackage,qca-hapd-supp))
$(eval $(call BuildPackage,qca-hapd-supp-macsec))
$(eval $(call BuildPackage,qca-hostapd-cli))
$(eval $(call BuildPackage,qca-wpa-cli))
$(eval $(call BuildPackage,qca-hapd-supp-lite))
