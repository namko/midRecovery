#!/bin/sh

set_perm() {
  local o="$1" # owner
  local g="$2" # group
  local m="$3" # mode
  local f="$4" # filename

  if [ -e $f ]; then
    chown $o:$g $f
    chmod $m $f
  fi
}

set_perm_recursive() {
  local o="$1" # owner
  local g="$2" # group
  local dm="$3" # directory mode
  local fm="$4" # file mode
  local f="$5" # filename

  if [ -d $f ]; then
    chown -R $o:$g $f
    find $f -type d -exec chmod $dm {} \;
    find $f -type f -exec chmod $fm {} \;
  fi
}

set_perm_recursive 0 0 0755 0644 "/mnt/root/system"
set_perm_recursive 0 0 0755 0755 "/mnt/root/system/addon.d"
set_perm_recursive 0 2000 0755 0755 "/mnt/root/system/bin"
set_perm 0 3003 02750 "/mnt/root/system/bin/netcfg"
set_perm 0 3004 02755 "/mnt/root/system/bin/ping"
set_perm 0 2000 06750 "/mnt/root/system/bin/run-as"
set_perm_recursive 1002 1002 0755 0440 "/mnt/root/system/etc/bluetooth"
set_perm 0 0 0755 "/mnt/root/system/etc/bluetooth"
set_perm 1000 1000 0640 "/mnt/root/system/etc/bluetooth/auto_pairing.conf"
set_perm 0 0 0644 "/mnt/root/system/etc/bluetooth/bdaddr"
set_perm 3002 3002 0444 "/mnt/root/system/etc/bluetooth/blacklist.conf"
set_perm 1002 1002 0440 "/mnt/root/system/etc/dbus.conf"
set_perm 1014 2000 0550 "/mnt/root/system/etc/dhcpcd/dhcpcd-run-hooks"
set_perm_recursive 0 2000 0755 0755 "/mnt/root/system/etc/init.d"
set_perm 0 0 0755 "/mnt/root/system/etc/init.d"
set_perm 0 2000 0550 "/mnt/root/system/etc/init.goldfish.sh"
set_perm 0 0 0544 "/mnt/root/system/etc/install-recovery.sh"
set_perm_recursive 0 0 0755 0555 "/mnt/root/system/etc/ppp"
set_perm 0 2000 0755 "/mnt/root/system/vendor"
set_perm_recursive 0 2000 0755 0755 "/mnt/root/system/vendor/bin"
set_perm_recursive 0 2000 0755 0644 "/mnt/root/system/vendor/etc"
set_perm 0 0 0644 "/mnt/root/system/vendor/etc/audio_effects.conf"
set_perm 0 2000 0755 "/mnt/root/system/vendor/lib"
set_perm_recursive 0 2000 0755 0644 "/mnt/root/system/vendor/lib/drm"
set_perm 0 0 0644 "/mnt/root/system/vendor/lib/drm/libdrmwvmplugin.so"
set_perm_recursive 0 2000 0755 0755 "/mnt/root/system/xbin"
set_perm 0 0 06755 "/mnt/root/system/xbin/librank"
set_perm 0 0 06755 "/mnt/root/system/xbin/procmem"
set_perm 0 0 06755 "/mnt/root/system/xbin/procrank"
set_perm 0 0 06755 "/mnt/root/system/xbin/su"
set_perm 0 0 04755 "/mnt/root/system/bin/pppd"
set_perm 0 0 0755 "/mnt/root/system/etc/check_property.sh"
set_perm 0 0 0755 "/mnt/root/system/etc/reboot"
set_perm 0 0 0755 "/mnt/root/system/etc/insmod.sh"
set_perm 0 0 0755 "/mnt/root/system/etc/audio_sw.sh"
set_perm 0 0 0777 "/mnt/root/system/etc/apns-conf.xml"


