#!/bin/sh

# Running in QEMU MiniVM
if ! grep -q " /etc/hostname " /proc/mounts; then
	mkdir -p /lib/modules && mount /dev/sdb /lib/modules
	[ -f /dev/net/tun ] || modprobe tun
	modprobe openvswitch
	acpid
	hostname "$(cat /etc/hostname)"
	ip link set lo up
	sed -n 's/^ *\(eth[0-9]*\): .*/\1/p' /proc/net/dev | sort -V | \
		while read -r intf; do ip link set dev "$intf" up; done
	ifup -a -f
fi

/etc/openvswitch/init.sh
exec bash -i
