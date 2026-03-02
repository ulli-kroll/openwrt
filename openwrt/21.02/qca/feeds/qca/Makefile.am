# Makefile.am - Automake script for installing qca-wifi files.
ACLOCAL_AMFLAGS = -I m4

ini_dir = ${sysconfdir}/misc/ipq/ini
dist_ini__DATA = net/qca-wifi/files/ini/global.ini
dist_ini__DATA += net/qca-wifi/files/ini/QCN9000.ini

internal_ini_dir = ${sysconfdir}/misc/ipq/ini/internal
dist_internal_ini__DATA = net/qca-wifi/files/ini/internal/global_i.ini
dist_internal_ini__DATA += net/qca-wifi/files/ini/internal/QCN9000_i.ini

initscripts_dir = ${sysconfdir}/initscripts
dist_initscripts__SCRIPTS = net/qca-wifi/files/start_qca_wifi
dist_initscripts__SCRIPTS += net/qca-hostap/files/qca-hostapd.init
dist_initscripts__SCRIPTS += net/qca-hostap/files/qca-wpa-supplicant.init

lib_dir = ${base_libdir}/wifi
dist_lib__DATA = net/qca-wifi/files/qcawificfg80211.sh
dist_lib__DATA += net/qca-hostap/files/hostapd.sh
dist_lib__DATA += net/qca-hostap/files/wpa_supplicant.sh
dist_lib__DATA += net/qca-hostap/files/wps-hostapd-nbh-update.sh
dist_lib__DATA += net/qca-hostap/files/wps-hostapd-update-uci.sh
dist_lib__DATA += net/qca-hostap/files/wps-supplicant-update-uci.sh
dist_lib__DATA += net/qca-hostap/files/dpp-hostapd-update-uci.sh
dist_lib__DATA += net/qca-hostap/files/dpp-supplicant-update-uci.sh
dist_lib__DATA += net/qca-hostap/files/dpp-operating-class-update.sh
dist_lib__DATA += utils/qca-icm/files/icm.sh

hotplug_ddir = ${sysconfdir}/hotplug.d/button
dist_hotplug_d_DATA = net/qca-hostap/files/wps-hotplug.sh
dist_hotplug_d_DATA += net/qca-hostap/files/wps-reset-hotplug.sh
dist_hotplug_d_DATA += net/qca-hostap/files/wps-supplicant-hotplug.sh
dist_hotplug_d_DATA += net/qca-hostap/files/wps-extender-hotplug.sh

default_wireless_dir = ${sysconfdir}/config/default_wifi_configs
dist_default_wireless__DATA = net/qca-wifi/files/default_uci_wireless/6g_wireless
dist_default_wireless__DATA += net/qca-wifi/files/default_uci_wireless/5g_wireless
dist_default_wireless__DATA += net/qca-wifi/files/default_uci_wireless/2g_wireless

ath_dir = ${sysconfdir}/ath
dist_ath__DATA = utils/qca-icm/files/icm.conf

firmware_ini_dir = ${base_libdir}/firmware/
dist_firmware_ini__DATA = net/qca-wifi/files/ini/global.ini
dist_firmware_ini__DATA += net/qca-wifi/files/ini/QCN9000.ini

firmware_internal_ini_dir = ${base_libdir}/firmware/internal
firmware_internal_ini__DATA = net/qca-wifi/files/ini/internal/global_i.ini
firmware_internal_ini__DATA += net/qca-wifi/files/ini/internal/QCN9000_i.ini

install-data-hook:
	$(LN_S) -nf $(ini_dir)/global.ini $(DESTDIR)$(firmware_ini_dir)/global.ini
	$(LN_S) -nf $(ini_dir)/QCN9000.ini $(DESTDIR)$(firmware_ini_dir)/QCN9000.ini
	$(LN_S) -nf $(internal_ini_dir)/global_i.ini $(DESTDIR)$(firmware_internal_ini_dir)/global_i.ini
	$(LN_S) -nf $(internal_ini_dir)/QCN9000_i.ini $(DESTDIR)$(firmware_internal_ini_dir)/QCN9000_i.ini
	chown root:1001 $(DESTDIR)$(ini_dir)/global.ini
	chown root:1001 $(DESTDIR)$(ini_dir)/QCN9000.ini
	chown root:1001 $(DESTDIR)$(internal_ini_dir)/global_i.ini
	chown root:1001 $(DESTDIR)$(internal_ini_dir)/QCN9000_i.ini
	chown root:1001 $(DESTDIR)$(default_wireless_dir)/6g_wireless
	chown root:1001 $(DESTDIR)$(default_wireless_dir)/5g_wireless
	chown root:1001 $(DESTDIR)$(default_wireless_dir)/2g_wireless
	chmod 0666 $(DESTDIR)$(ini_dir)/global.ini
	chmod 0666 $(DESTDIR)$(ini_dir)/QCN9000.ini
	chmod 0666 $(DESTDIR)$(internal_ini_dir)/global_i.ini
	chmod 0666 $(DESTDIR)$(internal_ini_dir)/global_i.ini
	chmod 0666 $(DESTDIR)$(default_wireless_dir)/6g_wireless
	chmod 0666 $(DESTDIR)$(default_wireless_dir)/5g_wireless
	chmod 0666 $(DESTDIR)$(default_wireless_dir)/2g_wireless
	mv $(DESTDIR)$(base_libdir)/wifi/wps-hostapd-nbh-update.sh $(DESTDIR)$(base_libdir)/wifi/wps-hostapd-nbh-update
	mv $(DESTDIR)$(base_libdir)/wifi/wps-hostapd-update-uci.sh $(DESTDIR)$(base_libdir)/wifi/wps-hostapd-update-uci
	mv $(DESTDIR)$(base_libdir)/wifi/wps-supplicant-update-uci.sh $(DESTDIR)$(base_libdir)/wifi/wps-supplicant-update-uci
	mv $(DESTDIR)$(base_libdir)/wifi/dpp-hostapd-update-uci.sh $(DESTDIR)$(base_libdir)/wifi/dpp-hostapd-update-uci
	mv $(DESTDIR)$(base_libdir)/wifi/dpp-supplicant-update-uci.sh $(DESTDIR)$(base_libdir)/wifi/dpp-supplicant-update-uci
	mv $(DESTDIR)$(base_libdir)/wifi/dpp-operating-class-update.sh $(DESTDIR)$(base_libdir)/wifi/dpp-operating-class-update
	chmod 0755 $(DESTDIR)$(base_libdir)/wifi/wps-hostapd-nbh-update
	chmod 0755 $(DESTDIR)$(base_libdir)/wifi/wps-hostapd-update-uci
	chmod 0755 $(DESTDIR)$(base_libdir)/wifi/wps-supplicant-update-uci
	chmod 0755 $(DESTDIR)$(base_libdir)/wifi/dpp-hostapd-update-uci
	chmod 0755 $(DESTDIR)$(base_libdir)/wifi/dpp-supplicant-update-uci
	chmod 0755 $(DESTDIR)$(base_libdir)/wifi/dpp-operating-class-update
	mv $(DESTDIR)${sysconfdir}/hotplug.d/button/wps-hotplug.sh $(DESTDIR)${sysconfdir}/hotplug.d/button/50-wps
	mv $(DESTDIR)${sysconfdir}/hotplug.d/button/wps-reset-hotplug.sh $(DESTDIR)${sysconfdir}/hotplug.d/button/51-wps-reset
	mv $(DESTDIR)${sysconfdir}/hotplug.d/button/wps-supplicant-hotplug.sh $(DESTDIR)${sysconfdir}/hotplug.d/button/52-wps-supplicant
	mv $(DESTDIR)${sysconfdir}/hotplug.d/button/wps-extender-hotplug.sh $(DESTDIR)${sysconfdir}/hotplug.d/button/54-wps-extender
