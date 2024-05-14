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
	/etc/openvswitch/init.sh
	ifup -a -f
else
	/etc/openvswitch/init.sh
	for intf in br0 br1 br2 br3; do ifup -f "$intf"; done
fi

exec bash -i
