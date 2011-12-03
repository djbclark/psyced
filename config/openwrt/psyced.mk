include $(TOPDIR)/rules.mk

PKG_NAME:=psyced
PKG_VERSION:=20090617
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/openwrt-$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=openwrt-$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://www.psyced.org/files/
PKG_MD5SUM:=a0ab513c6557f83272a03541f03c6ad2

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
    SECTION:=Net
    CATEGORY:=Network
    SUBMENU:=Instant Messaging
    DEPENDS:= \
	+psyclpc +perl +perlbase-autoloader +perlbase-bytes \
	+perlbase-data +perlbase-essential +perlbase-fcntl \
	+perlbase-file +perlbase-getopt +perlbase-posix \
	+perlbase-tie +perlbase-xsloader +sudo
    TITLE:=Interserver multiprotocol messaging server
    URL:=http://www.psyced.org/
endef

define Package/$(PKG_NAME)/description
    This is psyced, a scalable multi-protocol multi-casting chat, 
    messaging and social server solution to build decentralized 
    chat networks upon, released as open source. Powerful, not 
    bloated, not too hard to get into.
endef

define Build/Compile
endef

define Package/$(PKG_NAME)/install
    $(INSTALL_DIR) $(1)/opt/psyced
    $(CP) $(PKG_BUILD_DIR)/psyced-20090321/opt/psyced/* $(1)/opt/psyced/
    $(INSTALL_DIR) $(1)/etc
    $(CP) $(PKG_BUILD_DIR)/psyced-20090321/etc/psyced.ini $(1)/etc/
    $(INSTALL_DIR) $(1)/etc/init.d
    $(CP) $(PKG_BUILD_DIR)/psyced-20090321/etc/init.d/psyced $(1)/etc/init.d/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))

