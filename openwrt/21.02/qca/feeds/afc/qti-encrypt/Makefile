include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/package.mk

PKG_NAME:=qti-encrypt
PKG_VERSION:=1.0
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

ifeq ($(CONFIG_TARGET_sdx75),y)
LOCAL_SRC:=$(TOPDIR)/src/ipq/afc/qti-encrypt
BUILD_LD_FLAGS="-s -w -linkmode external -X 'main.configNoDevmem=yes'"
else
LOCAL_SRC:=$(TOPDIR)/qca/src/afc/qti-encrypt
BUILD_LD_FLAGS="-s -w -linkmode external"
endif

RSTRIP:=:
STRIP:=:

ifneq (, $(findstring _32, $(SUBTARGET)))
MAKE_FLAGS=GOARCH=arm
else
MAKE_FLAGS=GOARCH=arm64
endif

TARGET_LDFLAGS += \
    -L$(STAGING_DIR)/usr/lib

PKG_BUILD_DEPENDS:=qti-golang/host

define Package/$(PKG_NAME)
 SECTION:=Encrypt App
 CATEGORY:=Encrypt App
 DEPENDS:=@TARGET_ipq_ipq807x||TARGET_ipq_ipq807x_64||TARGET_ipq_ipq60xx||TARGET_ipq_ipq60xx_64||TARGET_ipq_ipq50xx||TARGET_ipq_ipq50xx_64||TARGET_ipq807x||TARGET_ipq50xx||TARGET_ipq95xx||TARGET_ipq60xx||TARGET_ipq53xx||TARGET_sdx75 +openssl-util
 TITLE:=Encrypt App
endef

define Package/$(PKG_NAME)/description
	Encrypt App
endef

define Build/Prepare
	cp $(LOCAL_SRC)/* $(PKG_BUILD_DIR)
endef

define Build/Compile
	$(MAKE_FLAGS) CGO_ENABLED=1 CC=$(TARGET_CC) GO111MODULE=off go build -buildmode=pie -v -ldflags=$(BUILD_LD_FLAGS) -tags 'osusergo,netgo,static_build' -a -o $(PKG_BUILD_DIR)/encrypt_client_app $(PKG_BUILD_DIR)/encrypt.go
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/encrypt_client_app $(1)/usr/sbin
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
